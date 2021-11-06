/*
 * Copyright (c) 2021, ABC Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /**
 * @file batapp_platform.h
 * @brief Generic Header for Windows or Linux based compilation
 * @author Subhasish Ghosh
 */
#ifndef BATAPP_PLATFORM_H
#define BATAPP_PLATFORM_H

#include <stdint.h>

#ifdef __GNUC__
#include <netinet/in.h>
#define batapp_ntohl(a)		be32toh(a)
#define batapp_ntohll(a)	be64toh(a)
#define PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#else
#include <winsock2.h>
#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")
#define batapp_ntohl(a)		ntohl(a)
#define batapp_ntohll(a)	ntohll(a)
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

#endif //BATAPP_PLATFORM_H