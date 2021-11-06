/*
 * Copyright (c) 2021, ABC Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /**
 * @file batapp_pktutils.h
 * @brief Common functions used by all packet types
 * @author Subhasish Ghosh
 */
#ifndef BATAPP_PKTUTILS_H
#define BATAPP_PKTUTILS_H

 /* get the array size */
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))

  /**
	* This function is used to check if a packet contains errors
	* @param pkt The packet retrieved
	* @param len Length of the packet received
	* @param pkttype The type of packet header
	* @return bool returns success/failure for the function
	*/
extern bool batapp_pkt_error(void* pkt, size_t len, batapp_pkttypes_t pkttype);

/**
  * This function is used to log the print data into a buffer
  * @param logbuff pointer to log data into
  * @param format string to log
  * @param ... printf stype variable parameters
  * @return logbuff pointer
  */
extern char* batapp_pkt_logbuff(char* logbuff, const char* format, ...);

#endif //BATAPP_PKTUTILS_H