/*
 * Copyright (c) 2021, ABC Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file batapp_logger.h
 * @brief Battery Logger Interface
 * @author Subhasish Ghosh
 */

#ifndef BATAPP_LOGGER_H
#define BATAPP_LOGGER_H

 /**
   * This function is used to print logs on the console
   * @param priority This is the logging priority
   * @param hdr The header string for the type of packet
   * @param ... variable arguments, similar to printf.
   * @return void
   */
extern void batapp_log(int priority, const char* hdr, const char* format, ...);

/**
  * This function is used to the set the log level
  * @param level The log level setting
  * @return void
  */
extern void batapp_setloglevel(int level);

/* enum to store the log levels */
enum batapp_loglevel {
	BATAPP_LOGGER_LEVEL_ERROR,
	BATAPP_LOGGER_LEVEL_INFO,
	BATAPP_LOGGER_LEVEL_DBG
};

#endif //BATAPP_LOGGER_H