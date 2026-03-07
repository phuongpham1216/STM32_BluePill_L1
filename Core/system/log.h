/*
 * log.h
 *
 *  Created on: Mar 1, 2026
 *      Author: P2Lap
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define LOG_ENABLE 1

#if LOG_ENABLE
    #define LOG(fmt, ...) printf("[LOG] " fmt "\r\n", ##__VA_ARGS__)
#else
    #define LOG(fmt, ...)
#endif

#endif
/* SYSTEM_LOG_H_ */
