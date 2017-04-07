/*
 * 	DataListsCTRL.cpp
 *
 *	Definition of psuList and supList. Default Values and printing methods for both.
 *	Getters & Setters for both. Conversion from boolean array to word in psuLists
 *
 *  Created on: 04-feb-2015
 *   Author: Alberto Ibarrondo Luis
*/

#include "Controller/Controller.h"

//==============================================VARIABLES==============================================//
// Data Lists (both defined here)
PSU_TYPE psuList[PSU_NUMBER];									// Power supply units' array list
SnI_TYPE supList[SUP_NUMBER];									// Supply & Internal voltages list

// Selection Lists
BOOL psuSelectionList[PSU_NUMBER]={0,0,0,0,0,0,0,0,0,0,0,0};	// Used for functions requiring multiple psus
BOOL sniSelectionList[SUP_NUMBER]={0,0,0,0,0,0,0,0,0,0,0,0,0};// Used for functions requiring multiple snis
BOOL alarmSelectionList[ALARM_NUMBER]={0,0,0,0};				// Used for functions requiring multiple alarms

// Address Lists
BYTE bridgeI2CAddressList[12]= PSUs_I2C_ADDRESS_ARRAY;

// Verify key
extern DWORD VERIFY_KEY;

//=====================================================================================================//
//=================================    DATA LISTS' METHODS    =========================================//
//=====================================================================================================//


//-------------------------------------------------------------------------------------------------------
// defaultValuesPSU - Replaces the current PSU values in RAM by those predefined as DEFAULT in
//					file "defineConstants.cpp"
// 		#INPUT: +int psuNum - 0 to 11, number of the PSU in psuList to be given the default values
//				+BOOL defaultStatus - Set psuStatus and RelayStatus to default
// 		#OUTPUT: None
//		>E.G.: defaultValuesPSU (11); - used in initializeValuesPSUsSnIs();
//-------------------------------------------------------------------------------------------------------
void defaultValuesPSU (int psuNum) { defaultValuesPSU (psuNum, true);}
void defaultValuesPSU (int psuNum, BOOL defaultStatus) {
	float auxArray0[4]=DEFAULT_alarmLimitValues_psu_pos;
	float auxArray1[4]=DEFAULT_alarmLimitValues_psu_neg;
	int auxArray2[4]=DEFAULT_alarmLimitTimes_psu;
	BOOL auxArray3[12]=DEFAULT_alarmProtocols_psu;
	WORD k = demux4to16(psuNum+1);
	WORD auxArray4[4]={k, k, k, k};
	float auxArray5[4]=DEFAULT_alarmProtocolVoltage_psu;
	int auxArray6[4]=DEFAULT_alarmCounters_psu;
	BOOL auxArray7[4]=DEFAULT_alarmStatus_psu;
	BOOL auxArray8[4]=DEFAULT_alarmLimitReached_psu;
	BOOL auxArray9[4]=DEFAULT_alarmWatch_psu;
	float auxArray10[12]=DEFAULT_iOffset_psu;

	if (defaultStatus){
		psuList[psuNum].relayStatus=DEFAULT_relayStatus_psu;
		psuList[psuNum].psuStatus=DEFAULT_psuStatus_psu;
	}
	psuList[psuNum].progVolt=DEFAULT_rdacValue_psu;
	psuList[psuNum].bridgeI2CAdr=bridgeI2CAddressList[psuNum];
	psuList[psuNum].rdacAdr=(psuNum&0x1?LOWER_SLAVE_SPI_ADDRESS:UPPER_SLAVE_SPI_ADDRESS);

	memcpy(psuList[psuNum].alarmLimitValues, (psuNum<=SF5_B?auxArray0:auxArray1), sizeof(psuList[psuNum].alarmLimitValues));
	memcpy(psuList[psuNum].alarmLimitTimes, auxArray2, sizeof(psuList[psuNum].alarmLimitTimes));
	memcpy(psuList[psuNum].alarmProtocols, auxArray3, sizeof(psuList[psuNum].alarmProtocols));
	memcpy(psuList[psuNum].alarmProtocolShutdown, auxArray4, sizeof(psuList[psuNum].alarmProtocolShutdown));
	memcpy(psuList[psuNum].alarmProtocolVoltage, auxArray5, sizeof(psuList[psuNum].alarmProtocolVoltage));
	memcpy(psuList[psuNum].alarmCounters, auxArray6, sizeof(psuList[psuNum].alarmCounters));
	memcpy(psuList[psuNum].alarmStatus, auxArray7, sizeof(psuList[psuNum].alarmStatus));
	memcpy(psuList[psuNum].alarmLimitReached, auxArray8, sizeof(psuList[psuNum].alarmLimitReached));
	memcpy(psuList[psuNum].alarmWatch, auxArray9, sizeof(psuList[psuNum].alarmWatch));

	psuList[psuNum].rShunt = DEFAULT_rShunt_psu;
	psuList[psuNum].iOffset = auxArray10[psuNum];

	psuList[psuNum].vOut = DEFAULT_vOut_psu;
	psuList[psuNum].vMean = DEFAULT_vMean_psu;
	psuList[psuNum].cOut = DEFAULT_cOut_psu;
	psuList[psuNum].cMean = DEFAULT_cMean_psu;
	//psuList[psuNum].VerifyKey = VERIFY_KEY;
}


//-------------------------------------------------------------------------------------------------------
// printValuesPSU - prints by stdio all the values in RAM from the selected PSU
// 		#INPUT: int psuNum - 0 to 11, number of the PSU in psuList to be printed
// 		#OUTPUT: console output of all the values
//		>E.G.: printValuesPSU (11); - used in TEST_DataListsCTRL();
//-------------------------------------------------------------------------------------------------------
void printValuesPSU (int psuNum) {
	iprintf("\nPSU-NUMBER: %d\n", psuNum);
	iprintf("- relayStatus: %d\n",psuList[psuNum].relayStatus);
	iprintf("- psuStatus: %d\n",psuList[psuNum].psuStatus);
	iprintf("- rdacValue: %s\n",ftos(psuList[psuNum].progVolt));
	iprintf("- bridgeI2CDir: 0x%x\n",psuList[psuNum].bridgeI2CAdr);
	iprintf("- rdacAdr: %d\n",psuList[psuNum].rdacAdr);
	iprintf("- alarmLimitValues:");printBuffer(psuList[psuNum].alarmLimitValues);
	iprintf("\n- alarmLimitTimes:");printBuffer(psuList[psuNum].alarmLimitTimes);
	iprintf("\n- alarmProtocols:");printBuffer(psuList[psuNum].alarmProtocols);
	iprintf("\n- alarmProtocolShutdown:");printBuffer(psuList[psuNum].alarmProtocolShutdown);
	iprintf("\n- alarmProtocolVoltage:");printBuffer(psuList[psuNum].alarmProtocolVoltage);
	iprintf("\n- alarmCounters:");printBuffer(psuList[psuNum].alarmCounters);
	iprintf("\n- alarmStatus:");printBuffer(psuList[psuNum].alarmStatus);
	iprintf("\n- alarmLimitReached:");printBuffer(psuList[psuNum].alarmLimitReached);
	iprintf("\n- alarmWatch:");printBuffer(psuList[psuNum].alarmWatch);
	iprintf("\n- rShunt: %d\n",psuList[psuNum].rShunt);
	iprintf("- iOffset: %s\n",ftos(psuList[psuNum].iOffset,4));
	iprintf("- vOut: %s\n",ftos(psuList[psuNum].vOut,4));
	iprintf("- cOut: %s\n",ftos(psuList[psuNum].cOut,4));

}


//-------------------------------------------------------------------------------------------------------
// defaultValuesSnI - Replaces the current SnI values in RAM by those predefined as DEFAULT in
//					file "defineConstants.cpp"
// 		#INPUT: int sniNum - 0 to 13, number of the SnI in supList to be given the default values
// 		#OUTPUT: None
//		>E.G.: defaultValuesSnI (12); - used in initializeValuesPSUsSnIs();
//-------------------------------------------------------------------------------------------------------
void defaultValuesSnI (int sniNum) {
	float auxArray0[14]=DEFAULT_nominalVoltage_sni;
	int auxArray2[4]=DEFAULT_alarmLimitTimes_sni;
	BOOL auxArray3[12]=DEFAULT_alarmProtocols_sni;
	int auxArray4[4]=DEFAULT_alarmProtocolShutdown_sni;
	int auxArray6[4]=DEFAULT_alarmCounters_sni;
	BOOL auxArray7[4]=DEFAULT_alarmStatus_sni;
	BOOL auxArray8[4]=DEFAULT_alarmLimitReached_sni;
	BOOL auxArray9[4]=DEFAULT_alarmWatch_sni;

	supList[sniNum].sniStatus=DEFAULT_sniStatus_sni;
	supList[sniNum].nominalVolt=auxArray0[sniNum];
	supList[sniNum].alarmLimitValues[0] = (supList[sniNum].nominalVolt>0?supList[sniNum].nominalVolt * SnI_INF_PERC_ALARM_VALUE:supList[sniNum].nominalVolt * SnI_SUP_PERC_ALARM_VALUE);
	supList[sniNum].alarmLimitValues[1] = (supList[sniNum].nominalVolt>0?supList[sniNum].nominalVolt * SnI_SUP_PERC_ALARM_VALUE:supList[sniNum].nominalVolt * SnI_INF_PERC_ALARM_VALUE);
	supList[sniNum].alarmLimitValues[2]	= 0;
	supList[sniNum].alarmLimitValues[3]	= 0;

	memcpy(supList[sniNum].alarmLimitTimes, auxArray2, sizeof(supList[sniNum].alarmLimitTimes));
	memcpy(supList[sniNum].alarmProtocols, auxArray3, sizeof(supList[sniNum].alarmProtocols));
	memcpy(supList[sniNum].alarmProtocolShutdown, auxArray4, sizeof(supList[sniNum].alarmProtocolShutdown));
	memcpy(supList[sniNum].alarmCounters, auxArray6, sizeof(supList[sniNum].alarmCounters));
	memcpy(supList[sniNum].alarmStatus, auxArray7, sizeof(supList[sniNum].alarmStatus));
	memcpy(supList[sniNum].alarmLimitReached, auxArray8, sizeof(supList[sniNum].alarmLimitReached));
	memcpy(supList[sniNum].alarmWatch, auxArray9, sizeof(supList[sniNum].alarmWatch));

	supList[sniNum].vOut = DEFAULT_vOut_sni;
	supList[sniNum].VerifyKey = VERIFY_KEY;
}


//-------------------------------------------------------------------------------------------------------
// printValuesSnI - prints by stdio all the values in RAM from the selected SnI
// 		#INPUT: int sniNum - 0 to 13, number of the SnI in supList to be printed
// 		#OUTPUT: console output of all the values
//		>E.G.: printValuesSnI (11); - used in TEST_DataListsCTRL();
//-------------------------------------------------------------------------------------------------------
void printValuesSnI (int sniNum) {
	iprintf("SnI-Number: %d\n", sniNum);
	iprintf("- sniStatus: %d\n",supList[sniNum].sniStatus);
	iprintf("- nominalVoltage: %s\n",ftos(supList[sniNum].nominalVolt));
	iprintf("- alarmLimitValues: ");printBuffer(supList[sniNum].alarmLimitValues);
	iprintf("\n- alarmLimitTimes: ");printBuffer(supList[sniNum].alarmLimitTimes);
	iprintf("\n- alarmProtocols: ");printBuffer(supList[sniNum].alarmProtocols);
	iprintf("\n- alarmProtocolShutdown: ");printBuffer(supList[sniNum].alarmProtocolShutdown);
	iprintf("\n- alarmCounters:  ");printBuffer(supList[sniNum].alarmCounters);
	iprintf("\n- alarmStatus:  ");printBuffer(supList[sniNum].alarmStatus);
	iprintf("\n- alarmLimitReached:  ");printBuffer(supList[sniNum].alarmLimitReached);
	iprintf("\n- alarmWatch:  ");printBuffer(supList[sniNum].alarmWatch);
	iprintf("\n- vOut: %s\n",ftos(supList[sniNum].vOut,4));

}

//-------------------------------------------------------------------------------------------------------
// PSU_TYPE and SnI_TYPE getters
//-------------------------------------------------------------------------------------------------------
PSU_TYPE getPSU ( int psuNum ){return psuList[psuNum];}
SnI_TYPE getSnI ( int sniNum ){return supList[sniNum];}



//-------------------------------------------------------------------------------------------------------
// Conversion for shutdown protocols from a boolean array (where each boolean in the array represents
//		its corresponding PSU being shut down when the protocol is executed) to a WORD (where each PSU
//		is stored as a bit in the word) and vice versa.
// 		#I/O: 	+WORD originPsuWord				->E.G.:  0x05F3 to [0,1,1,0, 1,1,1,1, 0,0,1,1]
//				 BOOL destArray[PSU_NUMBER]
//				+BOOL originArray[PSU_NUMBER] 	->E.G.: [0,0,1,0, 1,1,1,1, 0,0,1,1] to 0x02F3
//-------------------------------------------------------------------------------------------------------
void conversionShutdownWORDtoARRAY (WORD originPsuWord, BOOL destArray[PSU_NUMBER] ){
	uint i = 0; 	WORD auxWord = originPsuWord;
	for (i=0; i<PSU_NUMBER; i++){ destArray[i]=(auxWord&0x1);	auxWord=auxWord>>1;	}
}

WORD conversionShutdownARRAYtoWORD (BOOL originArray[PSU_NUMBER]){
	uint i = 0;		WORD auxWord = 0;
	for (i=0; i<PSU_NUMBER; i++){ auxWord=auxWord+(originArray[i]?0x1<<i:0);	}
	return auxWord;
}






// CURRENTLY NOT IN USE, BUT FULLY FUNCTIONAL

//-------------------------------------------------------------------------------------------------------
// setters for PSU_TYPE variables
//-------------------------------------------------------------------------------------------------------
void setrelayStatusPSU(BOOLEAN rlSt, int psuNum){psuList[psuNum].relayStatus = rlSt;}
void setpsuStatusPSU(BOOLEAN psSt, int psuNum){psuList[psuNum].psuStatus = psSt;}
void setrdacValuePSU(float rdVal, int psuNum){psuList[psuNum].progVolt = rdVal;}
void setrbridgeI2CAdrPSU(BYTE brAddr, int psuNum){psuList[psuNum].progVolt = brAddr;}
void setrrdacAdrPSU(BYTE rdacAddr, int psuNum){psuList[psuNum].rdacAdr = rdacAddr;}
void setalarmLimitValuesPSU(float value, BOOLEAN inf_sup, BOOLEAN volt_curr, int psuNum){psuList[psuNum].alarmLimitValues[_(inf_sup, volt_curr)] = value;};
void setalarmLimitTimesPSU(int time, BOOLEAN inf_sup, BOOLEAN volt_curr, int psuNum){psuList[psuNum].alarmLimitTimes[_(inf_sup, volt_curr)] = time;};
void setalalarmProtocolsPSU(BOOLEAN protocol_flag, BOOLEAN inf_sup, BOOLEAN volt_curr, BYTE protocol, int psuNum){psuList[psuNum].alarmProtocols[___(inf_sup, volt_curr, protocol)] = protocol_flag;};
void setalarmProtocolShutdownPSU(WORD psus, BOOLEAN inf_sup, BOOLEAN volt_curr, int psuNum){psuList[psuNum].alarmProtocolShutdown[_(inf_sup, volt_curr)] = psus;};
void setalarmProtocolVoltagePSU(float voltage, BOOLEAN inf_sup, BOOLEAN volt_curr, int psuNum){psuList[psuNum].alarmProtocolVoltage[_(inf_sup, volt_curr)] = voltage;};
void setalarmWatchPSU(BOOLEAN alarm_flag, BOOLEAN inf_sup, BOOLEAN volt_curr, int psuNum){psuList[psuNum].alarmWatch[_(inf_sup, volt_curr)] = alarm_flag;};
void setrShuntPSU(int rSh, int psuNum){psuList[psuNum].rShunt = rSh;};

//-------------------------------------------------------------------------------------------------------
// setters for SnI_TYPE variables
//-------------------------------------------------------------------------------------------------------
void setsniStatusSnI(BOOLEAN snSt, int sniNum){supList[sniNum].sniStatus = snSt;}
void setnominalVoltageSnI(float nomVal, int sniNum){supList[sniNum].nominalVolt = nomVal;}
void setalarmLimitValuesSnI(float value, BOOLEAN inf_sup, BOOLEAN volt_curr, int sniNum){supList[sniNum].alarmLimitValues[_(inf_sup, volt_curr)] = value;};
void setalarmLimitTimesSnI(int time, BOOLEAN inf_sup, BOOLEAN volt_curr, int sniNum){supList[sniNum].alarmLimitTimes[_(inf_sup, volt_curr)] = time;};
void setalalarmProtocolsSnI(BOOLEAN protocol_flag, BOOLEAN inf_sup, BOOLEAN volt_curr, BYTE protocol, int sniNum){supList[sniNum].alarmProtocols[___(inf_sup, volt_curr, protocol)] = protocol_flag;};
void setalarmProtocolShutdownSnI(WORD psus, BOOLEAN inf_sup, BOOLEAN volt_curr, int sniNum){supList[sniNum].alarmProtocolShutdown[_(inf_sup, volt_curr)] = psus;};
void setalarmWatchSnI(BOOLEAN alarm_flag, BOOLEAN inf_sup, BOOLEAN volt_curr, int sniNum){supList[sniNum].alarmWatch[_(inf_sup, volt_curr)] = alarm_flag;};

