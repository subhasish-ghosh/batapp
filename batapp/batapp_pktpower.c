/*
 * Copyright (c) 2021, ABC Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /**
  * @file batapp_pktpower.c
  * @brief Battery Packet Power State Interface
  * @author Subhasish Ghosh
  */

#include "batapp_pkttypes.h"
#include "batapp_logger.h"
#include "batapp_pktutils.h"

/* The minimum log len */
#define BATAPP_PKTPOWER_LOGLEN		100UL
/* Packet debounce interval on ms */
#define BATAPP_PKTPOWER_DBOUNCE		10UL

/* packed structure for storing power packet information */
typedef PACK(struct {
	uint32_t	ts;
	uint32_t	v;
	uint64_t	c;
	uint8_t		error;
}) batapp_pktpower_t;

/* array for storing valid state transitions */
static const bool batapp_statetable[4][4] = {
	true, true, false, false,
	false, true, true, false,
	true, false, true, true,
	false, false, true, true,
};

/* valid power state levels */
typedef enum {
	BATAPP_PKTPOWER_STATE_MIN,
	BATAPP_PKTPOWER_STATE_0 = BATAPP_PKTPOWER_STATE_MIN,
	BATAPP_PKTPOWER_STATE_1,
	BATAPP_PKTPOWER_STATE_2,
	BATAPP_PKTPOWER_STATE_3,
	BATAPP_PKTPOWER_STATE_MAX,
} batapp_pktpower_state_t;

/* This enum defines the actual (CH), previous (prev) and current (curr)
 * packet state and time stamp
 */
typedef enum {
	BATAPP_PKTPOWER_STATE_CH_MIN,
	BATAPP_PKTPOWER_STATE_CH = BATAPP_PKTPOWER_STATE_CH_MIN,
	BATAPP_PKTPOWER_STATE_CH_PREV,
	BATAPP_PKTPOWER_STATE_CH_CURR,
	BATAPP_PKTPOWER_STATE_CH_MAX,
} batapp_pktpower_state_ch_t;

/* This struct provides a storage for state+timestamp */
typedef struct {
	batapp_pktpower_state_t state;
	uint32_t ts;
} batapp_pktpower_state_ch_dat_t;

/* This is the logging buffer provided by the packet type */
static char* batapp_logbuff = NULL;

/**
  * This function is used to retrieve the logbuffer
  * @return the logbuffer
  */
static char* batapp_pktpower_getlogbuff(void) {
	if (batapp_logbuff != NULL) {
		return batapp_logbuff;
	}
	else {
		return "invalid logbuff";
	}
}

/**
  * This function returns the correct state depending upon the power level
  * @param v voltage retrieved from the paket
  * @param c voltage retrieved from the paket
  * @return The calculated state or an invalid state for error handling
  */
static batapp_pktpower_state_t batapp_pktpower_getstate(uint32_t v, uint64_t c) {

	uint64_t mwatt = v * c;

	if ((mwatt >= 0) && (mwatt <= 200)) {
		return BATAPP_PKTPOWER_STATE_0;
	}
	else if ((mwatt >= 300) && (mwatt <= 450)) {
		return BATAPP_PKTPOWER_STATE_1;
	}
	else if ((mwatt >= 550) && (mwatt <= 650)) {
		return BATAPP_PKTPOWER_STATE_2;
	}
	else if ((mwatt >= 800) && (mwatt <= 1200)) {
		return BATAPP_PKTPOWER_STATE_3;
	}
	else {
		return BATAPP_PKTPOWER_STATE_MAX;
	}

	return BATAPP_PKTPOWER_STATE_MAX;
}

/**
  * This function executes the power state machine once
  * @param fp file pointer
  * @return bool returns success/failure for the function
  */
static bool batapp_pktpower_step(FILE* fp) {
	/* init local and static variables */
	batapp_pktpower_t pktpower; /* this is used to retrieve the aligned packet data */
	bool retval = false;
	static uint32_t acc_dbounce = 0; /* this is used to store the accumulated debounce */
	/* store acctual, current and previous state and time info*/
	static batapp_pktpower_state_ch_dat_t state_change_data[] = {
		{BATAPP_PKTPOWER_STATE_0, 0},
		{BATAPP_PKTPOWER_STATE_0, 0},
		{BATAPP_PKTPOWER_STATE_0, 0},
	};

	/* read the packet */
	if (fread(&pktpower, sizeof(batapp_pktpower_t), 1, fp) != 1) {
		batapp_pkt_logbuff(batapp_logbuff, "failed to read data file");
		return retval;
	}

	/* check for any packet errors, this gets reported as ERR; while printing the log */
	retval = batapp_pkt_error(&pktpower, sizeof(batapp_pktpower_t), BATAPP_PACKETSTYPE_BATTERYPOWER);
	if (!retval) {
		batapp_pkt_logbuff(batapp_logbuff, "packet error!");
		return retval;
	}

	/* update the current state data */
	state_change_data[BATAPP_PKTPOWER_STATE_CH_CURR].state = batapp_pktpower_getstate(batapp_ntohl(pktpower.v), batapp_ntohll(pktpower.c));
	state_change_data[BATAPP_PKTPOWER_STATE_CH_CURR].ts = batapp_ntohl(pktpower.ts);

	/* check if the current and previous states are same, then accumulate debounce */
	if (state_change_data[BATAPP_PKTPOWER_STATE_CH_CURR].state == state_change_data[BATAPP_PKTPOWER_STATE_CH_PREV].state) {
		acc_dbounce += state_change_data[BATAPP_PKTPOWER_STATE_CH_CURR].ts - state_change_data[BATAPP_PKTPOWER_STATE_CH_PREV].ts;
	}
	else {
		acc_dbounce = 0; /* if new state, then restart accumulating debounce */
	}

	/* backup current data into previous data */
	state_change_data[BATAPP_PKTPOWER_STATE_CH_PREV] = state_change_data[BATAPP_PKTPOWER_STATE_CH_CURR];

	/* check if enough debounce is accumulated */
	if (acc_dbounce >= BATAPP_PKTPOWER_DBOUNCE) {
		batapp_pktpower_state_t from_state = state_change_data[BATAPP_PKTPOWER_STATE_CH].state;
		batapp_pktpower_state_t to_state = state_change_data[BATAPP_PKTPOWER_STATE_CH_CURR].state;

		/* check if the state transition is valid */
		if (batapp_statetable[from_state][to_state]) {
			/* if valid state, then copy state and timestamp into actual data storage */
			state_change_data[BATAPP_PKTPOWER_STATE_CH].ts = state_change_data[BATAPP_PKTPOWER_STATE_CH_CURR].ts - acc_dbounce;
			state_change_data[BATAPP_PKTPOWER_STATE_CH].state = state_change_data[BATAPP_PKTPOWER_STATE_CH_CURR].state;

			/* log data if state changed */
			if (from_state != to_state) {
				batapp_pkt_logbuff(batapp_logbuff, "%u;%u;%u", state_change_data[BATAPP_PKTPOWER_STATE_CH].ts / 1000, from_state, to_state);
			}
			else {
				/* don't log anything is same state */
				batapp_pkt_logbuff(batapp_logbuff, NULL);
			}
		}
		else {
			/* log ERR; state transition is not valid */
			batapp_pkt_logbuff(batapp_logbuff, "%u;%u;%u",
				(state_change_data[BATAPP_PKTPOWER_STATE_CH_CURR].ts - acc_dbounce) / 1000, from_state, to_state);
			retval = false;
		}

		acc_dbounce = 0;
	}
	else {
		/* if debounce is not reached, then nothing to log */
		batapp_pkt_logbuff(batapp_logbuff, NULL);
	}

	return retval;
}

/**
  * This function inits the power state machine.
  * @param loglen log buffer length
  * @return bool returns success/failure for the function
  */
static bool batapp_pktpower_init(size_t loglen) {
	bool retval = false;

	/* allocate atleast LOGLEN buffer */
	if (loglen < BATAPP_PKTPOWER_LOGLEN)
		loglen = BATAPP_PKTPOWER_LOGLEN;

	batapp_logbuff = malloc(loglen);

	/* check if malloc successfully allocated memory */
	if (batapp_logbuff != NULL)
		retval = true;

	return retval;
}

/**
  * This function cleans up the power state machine.
  */
static void batapp_pktpower_exit(void) {

	/* free the log buffer */
	free(batapp_logbuff);
}

/**
  * This structure defines a standard packet operations interface
  */
static batapp_pktops_t batapp_pktpower_ops = {
	.init = batapp_pktpower_init, /* Init the power state machine */
	.pkthdr = BATAPP_PKTPOWER_HDR, /* packet header string to print */
	.step = batapp_pktpower_step, /* core state machine for the packet type */
	.getlogbuff = batapp_pktpower_getlogbuff, /* retrieve the log bugger */
	.exit = batapp_pktpower_exit, /* cleanup during exit */
};

/**
  * This function returns the power state machine operations
  * @return batapp_pktops_t* returns a pointer to the operations.
  */
batapp_pktops_t* get_pktpower_obj(void) {
	return &batapp_pktpower_ops;
}