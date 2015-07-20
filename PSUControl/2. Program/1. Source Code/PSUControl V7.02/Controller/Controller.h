/*
 * Controller.h
 *
 *	Contains all the high level functions of PSUControl: monitors PSUs and SnIs' outputs (MonitorCTRL),
 *	triggers alarms when limits reached (AlarmCTRL), manages the PSU and SnI object Lists (DataLists),
 *	saves and restores those lists using Flash memory (FlashMemCTRL), changes/reads voltage and reads
 *	current for PSUs/SnIs (VoltCurrCTRL), and controls the On/Off switching of the PSUs (SwitchOnCTRL)
 *
 *  Created on: 04-feb-2015
 *   Author: Alberto Ibarrondo Luis
 */

#ifndef CONTROLLER
#define CONTROLLER


#include "Headers.h"
#include "Libraries/Libraries.h"


//=====================================ALARM METHODS==========================================//
//*********Public Methods*********//
void alarmTask (void *p);
void resetAlarms ( void ); 				void resetAlarms ( BOOL Selection[] ,  BOOL alarmSelection[ALARM_NUMBER] , BOOL psu_sni);
void toggleAlarms ( BOOL almWatch ); 	void toggleAlarms ( BOOL Selection[] ,  BOOL alarmSelection[ALARM_NUMBER] , BOOL psu_sni, BOOL almWatch );
void setAlarm ( BOOL psu_sni, int num, BOOL inf_sup, BOOL volt_curr, float value, int time);

//*********Private Methods*********//
void updateAlarms ( void );
void alarmCheck (int Num, BOOL inf_sup, BOOL volt_corr, BOOL psu_aux);
void executeAlarmProtocol (int Num, BOOL inf_sup, BOOL volt_corr, BYTE protocolNum, BOOL psu_sni);



//====================================CONFIGURATION METHODS====================================//
//*********Public Methods*********//
// Initialization
int allSemInit (void);
// Configuration
void config_alarmUpdatePeriod_x50MS ( int newPeriodx50MS );
void config_AlarmCTRLUpdate_SnI ( BOOL sniUpdateFLAG );
void config_MonitorCTRL_SnI ( BOOL monitorSnI );
void config_consoleOutput_RDAC ( BOOL consoleOut );
void config_consoleOutput_I2CnSPI ( BOOL consoleOut );
// Pause Tasks
void iddleMode_AlarmCTRL_Task ( BOOL iddleModeAlarmCTRLFLAG );
void sequentialMode_MonitorCTRL_Task ( BOOL seqModeMonitorCTRLFLAG );
// Test Mode
void testMode_MonitorCTRL_Measure ( BOOL testModeMonitorCTRLMeasureFLAG );
void testMode_set_Measure ( WORD testMeasure );
void testMode_SwitchOnCTRL_Task ( BOOL testModeSwitchOnCTRLTaskFLAG );



//=====================================DATA LISTS' METHODS====================================//
//*********Public Methods*********//
void defaultValuesPSU (int psuNum);		void defaultValuesPSU (int psuNum, BOOL defaultStatus);
void printValuesPSU (int psuNum);
void defaultValuesSnI (int sniNum);
void printValuesSnI (int sniNum);
PSU_TYPE getPSU (int psuNum);
SnI_TYPE getSnI(int sniNum);
// Conversion for shutdown protocols: BOOL array <--> WORD
void conversionShutdownWORDtoARRAY (WORD originPsuWord, BOOL destArray[PSU_NUMBER] );
WORD conversionShutdownARRAYtoWORD (BOOL originArray[PSU_NUMBER]);



//===================================FLASH MEMORY METHODS=====================================//
//*********Public Methods*********//
int saveInFlashValuesPSUsSNIs (void);
int initializeValuesPSUsSnIs(void);

//*********Private Methods*********//
void loadFlashValuesPSUs (void);
void loadFlashValuesSNIs (void);
void readFlashValuesPSU(int psuNum, PSU_TYPE *pData);
void createVERIFY_KEY(void);



//=======================================MONITOR METHODS=======================================//
//*********Public Methods*********//
void monitorTask (void* p);



//===================================SWITCH ON PSUs METHODS===================================//
//*********Public Methods*********//
void switchONPSUs ( BOOL psuSelection[PSU_NUMBER] );	void switchONPSUs( void );
void switchOFFPSUs( BOOL psuSelection[PSU_NUMBER] );	void switchOFFPSUs( void );
// Button
void buttonTask ( void );

//*********Private Methods*********//
void switchONPSUsTask ( BOOL psuSelection[PSU_NUMBER] );
void connectPSU ( int psuNum );
void disconnectPSU ( int psuNum );



//===============================VOLTAGE & CURRENT METHODS====================================//
//*********Public Methods*********//
BOOL adjustRdac (int psuNum, float Voltage);
void resetRdacs ( void );
void updateVoltagePSUs ( BOOL psuSelection[PSU_NUMBER] );
void readVoltageValue ( int Num, BOOL psu_sni );
void readCurrentValue ( int psuNum );
void resetMeasuresAll ( void );
WORD VoltORCurrToADCCounts ( float value, int samplingFunction, int Num );
float ADCCountsToVoltORCurr ( WORD ADCCounts, int samplingFunction, int Num );








// CURRENTLY NOT IN USE, BUT FULLY FUNCTIONAL
// setters for PSU_TYPE variables
void setrelayStatusPSU(BOOLEAN rlSt, int psuNum);
void setpsuStatusPSU(BOOLEAN psSt, int psuNum);
void setrdacValuePSU(float rdVal, int psuNum);
void setrbridgeI2CAdrPSU(BYTE brAddr, int psuNum);
void setrrdacAdrPSU(BYTE rdacAddr, int psuNum);
void setalarmLimitValuesPSU(float value, BOOLEAN inf_sup, BOOLEAN volt_curr, int psuNum);
void setalarmLimitTimesPSU(int time, BOOLEAN inf_sup, BOOLEAN volt_curr, int psuNum);
void setalalarmProtocolsPSU(BOOLEAN protocol_flag, BOOLEAN inf_sup, BOOLEAN volt_curr, BYTE protocol, int psuNum);
void setalarmProtocolShutdownPSU(WORD psus, BOOLEAN inf_sup, BOOLEAN volt_curr, int psuNum);
void setalarmProtocolVoltagePSU(float voltage, BOOLEAN inf_sup, BOOLEAN volt_curr, int psuNum);
void setalarmWatchPSU(BOOLEAN alarm_flag, BOOLEAN inf_sup, BOOLEAN volt_curr, int psuNum);
void setrShuntPSU(int rSh, int psuNum);
// setters for SnI_TYPE variables
void setsniStatusSnI(BOOLEAN snSt, int sniNum);
void setnominalVoltageSnI(float nomVal, int sniNum);
void setalarmLimitValuesSnI(float value, BOOLEAN inf_sup, BOOLEAN volt_curr, int sniNum);
void setalarmLimitTimesSnI(int time, BOOLEAN inf_sup, BOOLEAN volt_curr, int sniNum);
void setalalarmProtocolsSnI(BOOLEAN protocol_flag, BOOLEAN inf_sup, BOOLEAN volt_curr, BYTE protocol, int sniNum);
void setalarmProtocolShutdownSnI(WORD snis, BOOLEAN inf_sup, BOOLEAN volt_curr, int sniNum);
void setalarmWatchSnI(BOOLEAN alarm_flag, BOOLEAN inf_sup, BOOLEAN volt_curr, int sniNum);


#endif /* CONTROLLER */


