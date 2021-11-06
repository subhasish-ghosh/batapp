/*
 * Copyright (c) 2021, ABC Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /**
  * @file batapp_pktparser.h
  * @brief Battery Packet Parser Interface
  * @author Subhasish Ghosh
  */

#ifndef BATAPP_PKTPARSER_H
#define BATAPP_PKTPARSER_H
#include <stdbool.h>

  /**
	* This function implements the core packet processing logic.
	* @param datafilepath This is the data file path
	* @return bool returns success/failure for the function
	*/
extern bool batapp_pktparser_run(const char* datafilepath);

#endif //BATAPP_PKTPARSER_H