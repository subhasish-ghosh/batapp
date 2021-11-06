/*
 * Copyright (c) 2021, ABC Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /**
  * @file batapp_logger.c
  * @brief Battery Moitor Data Logger Interface
  * @author Subhasish Ghosh
  */

#include <stdio.h>
#include <stdarg.h>
#include "batapp_logger.h"

  /*
   * enable debug messages when debugging
   */
#if defined(_DEBUG)
static int loglevel = BATAPP_LOGGER_LEVEL_DBG;
#else
static int loglevel = BATAPP_LOGGER_LEVEL_INFO;
#endif

/**
  * This function is used to print logs on the console
  * @param priority This is the logging priority
  * @param hdr The header string for the type of packet
  * @param ... variable arguments, similar to printf.
  * @return void
  */
void batapp_log(int priority, const char* hdr, const char* format, ...) {

	va_list args;
	va_start(args, format);

	/* check the priority and if there is any data to log at all */
	if ((priority <= loglevel) && (*format != '\0')) {
		/* log ERR; for error packets */
		if (priority == BATAPP_LOGGER_LEVEL_ERROR) {
			printf("ERR;");
		}
		/* add the packet header */
		printf("%s;", hdr);
		vprintf(format, args);
		/* ensure a \n at the end */
		printf("\n");
	}

	va_end(args);
}

/**
  * This function is used to the set the log level
  * @param level The log level setting
  * @return void
  */
void batapp_setloglevel(int level) {
	loglevel = level;
}