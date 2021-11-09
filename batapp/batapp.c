/*
 * Copyright (c) 2021, ABC Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

 /**
  * @file batapp.c
  * @brief Battery Monitor Application Entry Point
  * @author Subhasish Ghosh
  */

#include <stdio.h>
#include "batapp_logger.h"
#include "batapp_pktparser.h"
#include "batapp_pkttypes.h"


  /**
   * @fn int main(int argc, char **argv)
   * @param argc
   * @param argv
   * @return int
   * @brief The main entry point function
   * @details The main function must be executed with CodingTest.bin as first parameter.
   */
int main(int argc, char** argv)
{
	/* ensure data file was provided */
	if (argc < 2) {
		batapp_log(BATAPP_LOGGER_LEVEL_ERROR, BATAPP_PKTMAIN_HDR, "Invalid or no file provided");
		return -1;
	}

	/* initiate the packet processing engine */
	if (!batapp_pktparser_run(argv[1]))
		return -1;

	return 0;
}