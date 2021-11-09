/*
 * Copyright (c) 2021, ABC Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /**
  * @file batapp_pktstatus.c
  * @brief Battery Packet Status Interface
  * @author Subhasish Ghosh
  */

#include "batapp_pkttypes.h"
#include "batapp_logger.h"
#include "batapp_pktutils.h"

  /* The minimum log len */
#define BATAPP_PKTSTATUS_LOGLEN		100UL

/* battery status levels */
static const char* batapp_status[] = {
	"VLOW",
	"LOW",
	"MED",
	"HIGH",
};

/* packed structure for storing status packet information */
typedef PACK(struct {
	uint32_t	ts;
	uint8_t		status;
	uint8_t		error;
}) batapp_pktstatus_t;

/* This is the logging buffer provided by the packet type */
static char* batapp_logbuff = NULL;

/**
  * This function is used to retrieve the logbuffer
  * @return the logbuffer
  */
static char* batapp_pktstatus_getlogbuff(void) {
	if (batapp_logbuff != NULL) {
		return batapp_logbuff;
	}
	else {
		return "invalid logbuff";
	}
}

/**
  * This function executes the battery status state machine once
  * @param fp file pointer
  * @return bool returns success/failure for the function
  */
static bool batapp_pktstatus_step(FILE* fp) {
	batapp_pktstatus_t pktstatus;/* this is used to retrieve the aligned packet data */
	bool retval = false;

	/* read the packet */
	if (fread(&pktstatus, sizeof(batapp_pktstatus_t), 1, fp) != 1) {
		batapp_pkt_logbuff(batapp_logbuff, "failed to read data file\n");
		return retval;
	}

	/* log battery status */
	batapp_pkt_logbuff(batapp_logbuff, "%u;%s", batapp_ntohl(pktstatus.ts) / 1000, batapp_status[pktstatus.status]);

	/* check for any packet errors, this gets reported as ERR; while printing the log */
	retval = batapp_pkt_error(&pktstatus, sizeof(batapp_pktstatus_t), BATAPP_PACKETSTYPE_BATTERYSTATUS);
	return retval;
}

/**
  * This function inits the battery status state machine.
  * @param loglen log buffer length
  * @return bool returns success/failure for the function
  */
static bool batapp_pktstatus_init(size_t loglen) {
	bool retval = false;

	/* allocate atleast LOGLEN buffer */
	if (loglen < BATAPP_PKTSTATUS_LOGLEN)
		loglen = BATAPP_PKTSTATUS_LOGLEN;

	batapp_logbuff = malloc(loglen);

	/* check if malloc successfully allocated memory */
	if (batapp_logbuff != NULL)
		retval = true;

	return retval;
}

/**
  * This function cleans up the battery status state machine.
  */
static void batapp_pktstatus_exit(void) {

	/* free the log buffer */
	free(batapp_logbuff);
}

/**
  * This structure defines a standard packet operations interface
  */
static batapp_pktops_t batapp_pktstatus_ops = {
	.init = batapp_pktstatus_init, /* Init the battery status state machine */
	.pkthdr = BATAPP_PKTSTATUS_HDR, /* packet header string to print */
	.step = batapp_pktstatus_step, /* core state machine for the packet type */
	.getlogbuff = batapp_pktstatus_getlogbuff, /* retrieve the log bugger */
	.exit = batapp_pktstatus_exit, /* cleanup during exit */
};

/**
  * This function returns the battery status state machine operations
  * @return batapp_pktops_t* returns a pointer to the operations.
  */
batapp_pktops_t* get_pktstatus_obj(void) {
	return &batapp_pktstatus_ops;
}