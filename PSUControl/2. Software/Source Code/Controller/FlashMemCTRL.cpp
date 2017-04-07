/*
 * FlashMemCTRL.cpp
 *
 *	Saving and loading PSU and SnI configuration and values into Flash Memory (making it available
 *	after system shutdown).
 *
 *  Created on: 07-may-2015
 *      Author: Alberto Ibarrondo Luis
 */


#include "Controller/Controller.h"

//==============================================VARIABLES==============================================//
// Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];					// MAIN PSU ARRAY LIST
extern SnI_TYPE supList [SUP_NUMBER];				// Supply & Internal voltages List

// used for createVERIFY_KEY()
extern float versionNo;									// Version Number, used to create a unique verify key
DWORD VERIFY_KEY;										// Created verify key


//=====================================================================================================//
//================================== PUBLIC FLASH MEMORY METHODS ======================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// initializeValuesPSUsSnIs - Loads all PSU values from FLASH and checks for each VERIFY_KEY correction.
//						If corrupted, sets the PSU values to default. Also Initializes the values of
//						auxList to their defaults
// 		#INPUT: None
// 		#OUTPUT: int saveParametersPSU - the number of Bytes that have been saved in Flash Memory
//		>E.G.: initializeValuesPSUsSnIs(); - used in switchONPSUsTask();
//-------------------------------------------------------------------------------------------------------
int initializeValuesPSUsSnIs(void){
	 int saveParametersPSU=0; int saveParametersSnI=false; int i=0;
	 PSU_TYPE *pPsuListFLASH = (PSU_TYPE *) GetUserParameters(); 		PSU_TYPE *pPsuListRAM = psuList;
	 SnI_TYPE *pSnIListFLASH = (SnI_TYPE *) (pPsuListFLASH+PSU_NUMBER);	SnI_TYPE *pSnIListRAM = supList;
	 // PSUs
	 for (i=0;i<PSU_NUMBER;i++){
	      if ((DWORD)(pPsuListFLASH->VerifyKey) != VERIFY_KEY){		// Incorrect Data
	    	  defaultValuesPSU(i);
	    	  saveParametersPSU|=1<<i;								// Save values as soon as an invalid key is detected
          }else {
	    	  //readFlashValuesPSU(i,pPsuListRAM); 					//	use in case memory copying didn't work (which currently does)
        	  memcpy (pPsuListRAM, pPsuListFLASH, sizeof(PSU_TYPE));
		  }
	      pPsuListFLASH++; pPsuListRAM++;							// Next PSU reading
	 }
	 // SnIs
	 for (i=0;i<SUP_NUMBER;i++){
		  if (pSnIListFLASH->VerifyKey != VERIFY_KEY){ 				// Incorrect Data
			  defaultValuesSnI(i);
			  saveParametersSnI|=1<<i;								// Save values as soon as an invalid key is detected
		   }else {													// Correct data
			  memcpy (pSnIListRAM, pSnIListFLASH, sizeof(PSU_TYPE));
		  }
		  pSnIListFLASH++; pSnIListRAM++;							//Next SnI reading
	 }

     if (saveParametersSnI|saveParametersPSU){
    	 saveInFlashValuesPSUsSNIs();
     }
     return 0x10000*saveParametersSnI+saveParametersPSU;
}


//-------------------------------------------------------------------------------------------------------
// saveInFlashValuesPSUsSNIs - Stores the current psuList and supList as a whole in the 8Kbits FLASH memory.
//						For the time being, 3Kbits of data are stored by using this method (the rest remains
//						unused).
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.: saveInFlashValuesPSUsSNIs(); - used in initializeValuesPSUsSnIs();
//-------------------------------------------------------------------------------------------------------
int saveInFlashValuesPSUsSNIs (void){
	int aux = SaveUserParameters(psuList, (sizeof(psuList)+sizeof(supList)));
	iprintf("         ~FlashMemCTRL: Saved psuList & sniList in Flash Mem-> %d Bytes\n", aux);
	return aux;
}






//=====================================================================================================//
//================================== PRIVATE FLASH MEMORY METHODS =====================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// LoadFlashValuesPSU - Copies the data from the FLASH memory to the psuList in RAM, updating all
//						PSU programmed values. DOESN'T VERIFY DATA INTEGRITY (Use initializeValuesPSUsSnIs()
//						instead)
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.: loadFlashValuesPSUs(); - used in TEST_FlashMemCTRL_PSU();
//-------------------------------------------------------------------------------------------------------
void loadFlashValuesPSUs (void){
	PSU_TYPE *pData = (PSU_TYPE *) GetUserParameters();
	memcpy (psuList, pData, sizeof(psuList));
	iprintf("    ~FlashMemCTRL: Loaded values of psuList from Flash Mem\n");
}


//-------------------------------------------------------------------------------------------------------
// loadFlashValuesSnI - Copies the data from the FLASH memory to the supList in RAM, updating all
//						SnI programmed values. DOESN'T VERIFY DATA INTEGRITY (Use initializeValuesPSUsSnIs()
//						instead)
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.: loadFlashValuesSNIs(); - used in TEST_FlashMemCTRL_PSU();
//-------------------------------------------------------------------------------------------------------
void loadFlashValuesSNIs (void){
	PSU_TYPE *pData = (PSU_TYPE *) GetUserParameters();
	memcpy (supList, (SnI_TYPE *) (pData + PSU_NUMBER), sizeof(supList));
	iprintf("    ~FlashMemCTRL: Loaded values of psuList from Flash Mem\n");
}



//-------------------------------------------------------------------------------------------------------
// createVERIFY_KEY: uses the versionNo to create a unique DWORD used as ID number
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.: createVERIFY_KEY(); - used in _init();
//-------------------------------------------------------------------------------------------------------
void createVERIFY_KEY( void ){
	VERIFY_KEY = (DWORD) versionNo;
}
