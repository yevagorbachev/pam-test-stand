#pragma once
#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>

typedef enum {
	L_COMMENT,
	L_EVENT,
	L_RAW
} print_mode_t;

int uprintf(print_mode_t mode, const char* fmt, ...);
#ifndef LINE_LENGTH
#define LINE_LENGTH 256
#endif
