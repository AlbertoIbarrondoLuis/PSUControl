/*
 * GeneralLibrary.h
 *
 *	Implements methods for various general purposes:
 *		- mergeBuffer: copy from a BYTE buffer to another one
 *		- printBuffer: prints the contents of an array of any kind ( float, hex, int, WORD )
 *		- printBufferPositions; prints the nonzero positions of a BYTE array
 *		- Ascii2Hex: conversion from keyboard input to hex.
 *		- ftos: float to string (char array) to use with iprintf
 *		- fillWithBlank: fill a string(char array) with empty spaces
 *		- itoa: int to string (char array) in ascii
 *		- reverse: reverse a string
 *		- demux4to16: 8 = 10000000; 3 = 00000100
 *		- hammWeight: 01100100 = 3 hamming weight (number of ones) of a WORD
 *
 *  Created on: 29-ene-2015
 *      Author: Alberto
 */



#ifndef LIB_GENER
#define LIB_GENER


#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "predef.h"
#include <ctype.h>
#include <basictypes.h>
#include <constants.h>
#include <stdio.h>


//=====================================GENERAL METHODS===========================================//
//*********Public Methods*********//
void mergeBuffer(BYTE buf1[], BYTE buf2[], int size);

void printBuffer(float buf1[]);
void printBuffer(float buf1[], uint bufferSize);
void printBuffer(BYTE buf1[]);
void printBuffer(BYTE buf1[], uint bufferSize);
void printBuffer(int buf1[]);
void printBuffer(int buf1[], uint bufferSize);
void printBuffer(WORD buf1[]);
void printBuffer(WORD buf1[], uint bufferSize);

void printBufferPositions(BOOL buf1[], uint bufferSize);

DWORD Ascii2Hex( char* buf, int num);

char* ftos (float f);
char* ftos (float f, int nDecimals);
char* ftos (float f, int nDecimals, int ThisBufNum, BOOL global_or_this_bufNum);
char* ftos (float f, int n, char * stringingBufferTotal);

void fillWithBlank ( BYTE* buffer, uint totalSize);

char* intToBuffer(char * buffer, int data);

void concChar(char* p, char* msg);

void itoa(int n, char s[]);

void reverse(char s[]);

WORD demux4to16 (int a);

WORD hammWeight(WORD x);

// DEMUX METHOD
#define DEMUX(a) (0x1 << (a-1))

#endif /* LIB_GENER */


