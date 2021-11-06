/*
 * Copyright (c) 2021, ABC Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /**
  * @file batapp_pktparser.c
  * @brief Battery Packet Parser Interface
  * @author Subhasish Ghosh
  */

#include <stddef.h>
#include <stdio.h>
#include "batapp_logger.h"
#include "batapp_pkttypes.h"
#include "batapp_pktutils.h"

  /**
   * batapp_pktobj is an array of function pointers, used to return
   * a specific handle corresponding to a packet type.
   */
static batapp_pktops_t* (*batapp_pktobj[BATAPP_PACKETTYPE_MAX])(void) = {
	get_pktpower_obj,  /* returns the handle for the power state packet */
	get_pktstatus_obj, /* returns the handle for the battery status packet */
	/*
	 * add an entry here for new packet types
	 * The order should match batapp_pkttypes_t
	 */
};

/**
  * This function implements the core packet processing logic.
  * @param datafilepath This is the data file path
  * @return bool returns success/failure for the function
  */
bool batapp_pktparser_run(const char* datafilepath) {
	FILE* fp;
	bool retval = false;
	int pkttype;

	/* Open a binary file */
	if ((fp = fopen(datafilepath, "rb")) == NULL) {
		batapp_log(BATAPP_LOGGER_LEVEL_ERROR, BATAPP_PKTPARSER_HDR, "Failed to open data file");
		return retval;
	}

	/* Initialize all registered packet types */
	for (pkttype = 0; pkttype < ARRAY_SIZE(batapp_pktobj); pkttype++) {
		if (batapp_pktobj[pkttype] != NULL) {
			batapp_pktobj[pkttype]()->init(0);
		}
	}

	retval = true;

	/* read packet header and process */
	while ((pkttype = fgetc(fp)) != EOF) {
		batapp_pktops_t* pktops;

		/* check packet type is correct */
		if ((pkttype >= BATAPP_PACKETTYPE_MAX) || (pkttype < BATAPP_PACKETTYPE_MIN)) {
			batapp_log(BATAPP_LOGGER_LEVEL_ERROR, BATAPP_PKTPARSER_HDR, "Invalid packet type");
			retval = false;
			goto exit_pkttype;
		}

		/* get the packet handler based upon the pkttype */
		if (batapp_pktobj[pkttype] != NULL) {
			pktops = batapp_pktobj[pkttype]();
			/* cycle the pkttype state machine once and print log */
			if (pktops->step(fp)) {
				batapp_log(BATAPP_LOGGER_LEVEL_INFO, pktops->pkthdr, pktops->getlogbuff());
			}
			else {
				/* in case of error, print as ERR: */
				batapp_log(BATAPP_LOGGER_LEVEL_ERROR, pktops->pkthdr, pktops->getlogbuff());
				retval = false;
			}
		}
	}

exit_pkttype:
	/* De-initialize all registered packet types */
	for (pkttype = 0; pkttype < ARRAY_SIZE(batapp_pktobj); pkttype++) {
		if (batapp_pktobj[pkttype] != NULL) {
			batapp_pktobj[pkttype]()->exit();
		}
	}
	/* close the file */
	fclose(fp);
	return retval;
}