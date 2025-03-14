#pragma once

#ifndef STDUCP_H
#define STDUCP_H

#include <windows.h>

// Define for output in stderr errors
// Comment if you don`t need get an error in stderr
#define UCPERR


#define BUFFER_SIZE 256

#define BAUDRATE CBR_9600
#define STOPBITS ONESTOPBIT
#define PARITY NOPARITY
#define BYTESIZE 8

#define READINTERVALTIMEOUT 50
#define READTOTALTIMEOUTCONSTANT 50
#define READTOTALTIMEOUTMULTIPLIER 10
#define WRITETOTALTIMEOUTCONSTANT 50
#define WRITETOTALTIMEOUTMULTIPLIER 50

HANDLE hCom;
static _Bool isConnected = FALSE;

#ifdef UCPERR
#include <stdio.h>
#endif

#ifdef __cplusplus
#define EXTERN extern "C"
#else 
#define EXTERN extern
#endif

EXTERN
HANDLE open_com_port(const char* portName);

EXTERN
void close_com_port(void);

EXTERN
const char* read_from_com_port(void);

EXTERN
long is_available(void);

EXTERN
_Bool write_to_com_port(const char* data);

#endif // !STDUCP_H