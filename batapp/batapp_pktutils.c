/*
 * Copyright (c) 2021, ABC Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /**
  * @file batapp_pktutils.c
  * @brief Common functions used by all packets
  * @author Subhasish Ghosh
  */

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include "batapp_pkttypes.h"

  /**
	* This function is used to check if a packet contains errors
	* @param pkt The packet retrieved
	* @param len Length of the packet received
	* @param pkttype The type of packet header
	* @return bool returns success/failure for the function
	*/
bool batapp_pkt_error(void* pkt, size_t len, batapp_pkttypes_t pkttype) {
	unsigned char* pktdata = (unsigned char*)pkt;
	unsigned char pkterr = 0;

	/* add up byte-by-byte */
	for (int i = 0; i < len - 1; i++) {
		pkterr += pktdata[i];
	}

	pkterr += pkttype;

	/* check if error value matched */
	return (pkterr == pktdata[len - 1]);
}

/**
  * This function is used to log the print data into a buffer
  * @param logbuff pointer to log data into
  * @param format string to log
  * @param ... printf stype variable parameters
  * @return logbuff pointer
  */
char* batapp_pkt_logbuff(char* logbuff, const char* format, ...) {

	va_list args;
	va_start(args, format);

	/* check if logbuff is non NULL */
	if (logbuff != NULL) {
		/* check if there is data to log */
		if (format != NULL) {
			vsprintf(logbuff, format, args);
		}
		else {
			/* if no data to log, ensure logbuff is not having stale data */
			*logbuff = '\0';
		}
	}

	va_end(args);

	return logbuff;
}