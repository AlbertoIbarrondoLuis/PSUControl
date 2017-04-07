/*
 * GeneralLibrary.cpp
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
 *      Author: Alberto Ibarrondo Luis
 */

#include "Libraries/GeneralLibrary.h"

//===========================================VARIABLES=================================================//
// Auxiliary Variables for ftos
char stringingBufferTotal1[20];
char stringingBufferTotal2[20];
char stringingBufferTotal3[20];
char stringingBufferTotal4[20];
int GlobalBufNum = 0;


//=====================================================================================================//
//=====================================    GENERAL LIBRARY    =========================================//
//=====================================================================================================//
//-------------------------------------------------------------------------------------------------------
// mergeBuffer writes size bytes from buf2 to buf1
//-------------------------------------------------------------------------------------------------------
	void mergeBuffer(BYTE buf1[], BYTE buf2[], int size){
		BYTE* paux1 = buf1;
		BYTE* paux2 = buf2;
		for (int i = 0; i<size; i++){
			*paux1 = *paux2;
			paux2++; paux1++;
		}

	}

//-------------------------------------------------------------------------------------------------------
// printBuffer displays the contents of a buffer or array using iprintf
//-------------------------------------------------------------------------------------------------------
	void printBuffer(BYTE buf1[]){printBuffer(buf1, sizeof (buf1));}
	void printBuffer(BYTE buf1[], uint bufferSize){
		iprintf(" |[ %x", buf1[0]);
		for (uint i = 1; i<bufferSize; i++){
			iprintf(", %x", buf1[i]);
		}
		iprintf(" ]| ");
	}
	void printBuffer(float buf1[]){printBuffer(buf1, sizeof(buf1));}
	void printBuffer(float buf1[], uint bufferSize){
		iprintf(" |[ %s", ftos(buf1[0]));
		for (uint i = 1; i<bufferSize; i++){
			iprintf(", %s", ftos(buf1[i]));
		}
		iprintf(" ]| ");
	}
	void printBuffer(int buf1[]){printBuffer(buf1, sizeof(buf1));}
	void printBuffer(int buf1[], uint bufferSize){
		iprintf(" |[ %d", buf1[0]);
		for (uint i = 1; i<bufferSize; i++){
			iprintf(", %d", buf1[i]);
		}
		iprintf(" ]| ");
	}
	void printBuffer(WORD buf1[]){printBuffer(buf1, sizeof(buf1));}
	void printBuffer(WORD buf1[], uint bufferSize){
		iprintf(" |[ %x", buf1[0]);
		for (uint i = 1; i<bufferSize; i++){
			iprintf(", %x", buf1[i]);
		}
		iprintf(" ]| ");
	}

//-------------------------------------------------------------------------------------------------------
// printBufferPositions displays the nonzero positions of a BOOL buffer or array using iprintf
//-------------------------------------------------------------------------------------------------------
	void printBufferPositions(BOOL buf1[], uint bufferSize){
		iprintf(" |[ %s", (buf1[0]?"0":""));
		for (uint i = 1; i<bufferSize; i++){
			if (buf1[i]){iprintf(", %d", i);}
		}
		iprintf(" ]| ");
	}

//-------------------------------------------------------------------------------------------------------
// Ascii2Hex converts the first num ASCII chars of a string to a single hex value
//-------------------------------------------------------------------------------------------------------
	DWORD Ascii2Hex( char* buf, int num ){
		DWORD conv[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
		DWORD temp = 0;
		for (int n = 0; n<num; n++){
		   for( int i = 0; i<16; i++){
			  if ( toupper(buf[num-n-1]) == conv[i] ) {
				 temp |= i<<(4*(n));
			  }
		   }
		}
		return temp;
	}


//-------------------------------------------------------------------------------------------------------
// ftos  - converts float to string.
//-------------------------------------------------------------------------------------------------------
	char* ftos (float f){return ftos(f, 2, GlobalBufNum, 0);}
	char* ftos (float f, int nDecimals){return ftos(f, nDecimals, GlobalBufNum, 0);}
	char* ftos (float f, int nDecimals, int ThisBufNum, BOOL global_or_this_bufNum){
		GlobalBufNum = (GlobalBufNum<3?0:GlobalBufNum+1);
		if (ThisBufNum<0 || ThisBufNum >3){ThisBufNum=0;}
		switch (global_or_this_bufNum?ThisBufNum:GlobalBufNum){
			case 0: return ftos(f,nDecimals,stringingBufferTotal1); break;
			case 1: return ftos(f,nDecimals,stringingBufferTotal2); break;
			case 2: return ftos(f,nDecimals,stringingBufferTotal3); break;
			case 3: return ftos(f,nDecimals,stringingBufferTotal4); break;
		}
		return stringingBufferTotal1; //shouldn't be reached
	}

	char* ftos (float f, int n, char * stringingBufferTotal){
		char stringingBufferInt[10];
		char stringingBufferDecimal[10];
		double aux = (double)f;
		memset(stringingBufferInt, 0, sizeof stringingBufferInt);
		memset(stringingBufferDecimal, 0, sizeof stringingBufferDecimal);
		memset(stringingBufferTotal, 0, sizeof stringingBufferTotal);
		if ( aux<0 ){
			if ((int)(round((ceil(aux)-aux)*pow(10,n)))>=(int)(pow(10,n)-1)){
				itoa(((int)ceil(aux))-1, stringingBufferInt);
				itoa(0, stringingBufferDecimal);
			}
			else{
				itoa((int)ceil(aux), stringingBufferInt);
				itoa((int)(round((ceil(aux)-aux)*pow(10,n))), stringingBufferDecimal);
			}
			if((int)ceil(aux)==0){concChar(stringingBufferTotal, "-");}
			concChar(stringingBufferTotal, stringingBufferInt);
			concChar(stringingBufferTotal, ".");
			for (uint i=0; i<n-strlen(stringingBufferDecimal); i++){concChar(stringingBufferTotal, "0");}
			concChar(stringingBufferTotal, stringingBufferDecimal);

		}
		else{
			if (((int)(round((aux-floor(aux))*pow(10,n))))>=(int)(pow(10,n)-1)){
				itoa(((int)floor(aux))+1, stringingBufferInt);
				itoa(0, stringingBufferDecimal);
			}
			else{
				itoa((int)floor(aux), stringingBufferInt);
				itoa((int)(round(( aux-floor(aux) )*pow(10,n))), stringingBufferDecimal);
			}
			concChar(stringingBufferTotal, stringingBufferInt);
			concChar(stringingBufferTotal, ".");
			for (uint i=0; i<n-strlen(stringingBufferDecimal); i++){concChar(stringingBufferTotal, "0");}
			concChar(stringingBufferTotal, stringingBufferDecimal);

		}
		return stringingBufferTotal;
	}


//-------------------------------------------------------------------------------------------------------
// fillWithBlank - fills the buffer with blank spaces for a total size of totalSize, without overflowing
//-------------------------------------------------------------------------------------------------------
	void fillWithBlank ( BYTE* buffer, uint totalSize){
		uint pNum = 0; BYTE* p = buffer;
		while (pNum<totalSize || pNum<(sizeof(buffer)-1)){
			if (*p=='\0'){	*p=' ';p++;*p='\0';}
			else		 {		   p++;		   }
			pNum++;
		}
		*p='\0';
	}

//-------------------------------------------------------------------------------------------------------
// intToBuffer - adds an int value (previously converted to string) to a buffer, with a # sign as separator
//-------------------------------------------------------------------------------------------------------
	char *  intToBuffer(char * buffer, int data){
		char buf2[10];
		char *f=buf2;
		strcat(buffer,"#");
		itoa(data,f);
		strcat(buffer,f);
		return buffer;
	}


//-------------------------------------------------------------------------------------------------------
// concChar  - links the second character chain at the end of the first
//-------------------------------------------------------------------------------------------------------
	void concChar(char* p, char* msg){
		int n = 0;
		while(*p){
			n++;
			p++;
		}
		while(*msg){
			*p = *msg;
			msg++;
			p++;
			n++;
		}
		*p= '\0';					// Pone un NULL al final de la segunda para avisar del final
	}


//-------------------------------------------------------------------------------------------------------
// itoa - convert n to characters in s
//-------------------------------------------------------------------------------------------------------
	void itoa(int n, char s[]){
	     int i, sign;
	     if ((sign = n) < 0)  // record sign
	         n = -n;          // make n positive
	     i = 0;
	     do {       // generate digits in reverse order
	         s[i++] = n % 10 + '0';   // get next digit
	     } while ((n /= 10) > 0);     // delete it
	     if (sign < 0)
	         s[i++] = '-';
	     s[i] = '\0';
	     reverse(s);
	}


//-------------------------------------------------------------------------------------------------------
// reverse:  reverse string s in place
//-------------------------------------------------------------------------------------------------------
	void reverse(char s[]){
	     int i, j;
	     char c;
	     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
	         c = s[i];
	         s[i] = s[j];
	         s[j] = c;
	     }
	}


//-------------------------------------------------------------------------------------------------------
// demux4to16:  8 = 10000000; 3 = 00000100
//-------------------------------------------------------------------------------------------------------
	WORD demux4to16 (int a){
		WORD r = 0;
		if (a>0){
			r = (0x1 << (a-1));
		}
		return r;
	}

//-------------------------------------------------------------------------------------------------------
// hammWeight:  10000000 = 1; 01100100 = 3
//-------------------------------------------------------------------------------------------------------
	WORD hammWeight(WORD x){
		WORD count;
	    for (count=0; x; count++){
	        x &= x-1;
	    }
	    return count;
	}
