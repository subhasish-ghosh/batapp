/*
 * Copyright (c) 2021, ABC Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /**
  * @file batapp_pkttypes.c
  * @brief Battery Packet Types Interface
  * @author Subhasish Ghosh
  */

#ifndef BATAPP_PKTTYPES_H
#define BATAPP_PKTTYPES_H

#include <stdbool.h>
#include <stdio.h>
#include"batapp_platform.h"

  /* status logging packet headers */
#define BATAPP_PKTSTATUS_HDR	"B"
#define BATAPP_PKTPOWER_HDR		"S"
#define BATAPP_PKTPARSER_HDR	"Z"
#define BATAPP_PKTMAIN_HDR		"M"
#define BATAPP_PKTERROR_HDR		"ERR"

/* packet types currently defined */
typedef enum {
	BATAPP_PACKETTYPE_MIN,
	BATAPP_PACKETSTYPE_BATTERYPOWER = BATAPP_PACKETTYPE_MIN,
	BATAPP_PACKETSTYPE_BATTERYSTATUS,
	BATAPP_PACKETTYPE_MAX
} batapp_pkttypes_t;

/* Common Operations defined for all packet types */
typedef struct {

	/* Init the packet state machine */
	bool (*init)(size_t loglen);

	/* packet header string to print */
	const char* pkthdr;

	/* core state machine for the packet type */
	bool (*step)(FILE* fp);

	/* retrieve the log bugger */
	char* (*getlogbuff)(void);

	/* cleanup during exit */
	void (*exit)(void);

} batapp_pktops_t;

/**
  * These functions returns the specific state machine operations
  * @return batapp_pktops_t* returns a pointer to the operations.
  */
extern batapp_pktops_t* get_pktstatus_obj(void);
extern batapp_pktops_t* get_pktpower_obj(void);

#endif //BATAPP_PKTTYPES_H