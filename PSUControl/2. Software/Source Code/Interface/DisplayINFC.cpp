/*
 * DisplayINFC.cpp
 *
 *	Display the status of all the PSUs and SnIs, as well as their configuration
 *
 *  Created on: 27/05/2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "Interface.h"


//===========================================VARIABLES=================================================//
// Imported
extern PSU_TYPE psuList[PSU_NUMBER];					// MAIN PSU ARRAY LIST
extern SnI_TYPE supList[SUP_NUMBER];					// Supply & Internal voltages List

extern char categorySelectionINFC;

extern BOOL config_MonitorCTRL_SnI_FLAG;

extern float radarVolts[12];							// Radar Voltages

// Display Message buffers
BYTE psuAlmStDisp[PSU_NUMBER][4][5];	// Alarm Status Display (v and c), filled with one of the Alarm messages below
BYTE psuValDisp[PSU_NUMBER][16][8];		// PSU values display (progVolt, vOut, cOut, ...)
BYTE psuStDisp[PSU_NUMBER][4];			// "ON " or "OFF"

BYTE sniAlmStDisp[SUP_NUMBER][2][5];	// Alarm Status Display for voltage, filled with one of the Alarm messages below
BYTE sniValDisp[SUP_NUMBER][6][8];		// SnI values display, nominalVoltage, vOut for each


// Alarm messages
// -status
static char alarmMessageConnected [4] = "Con";
static char alarmMessageLimReached [4] = "Lim";
static char alarmMessageTriggered [4] = "Trg";
static char alarmMessageDisconnected [4] = "Dis";

// -config
#define alarmConnected_DISP 'c'
#define alarmDisconnected_DISP 'd'
#define ProtocolON_DISP '#'
#define ProtocolOFF_DISP '.'
#define printHeaderPsuDISP(Num) psuStDisp[Num],psuAlmStDisp[Num][0],psuAlmStDisp[Num][1],psuAlmStDisp[Num][2],psuAlmStDisp[Num][3]
#define printConfigPsuDISP(Num, inf_sup, vol_cur) psuValDisp[Num][(_(inf_sup, vol_cur))*4],psuValDisp[Num][(_(inf_sup, vol_cur))*4+1],psuValDisp[Num][(_(inf_sup, vol_cur))*4+2],psuValDisp[Num][(_(inf_sup, vol_cur))*4+3]
#define printHeaderSniDISP(Num) sniAlmStDisp[Num][0],sniAlmStDisp[Num][1]
#define printConfigSniDISP(Num, inf_sup, vol_cur) sniValDisp[Num][(_(inf_sup, vol_cur))*3],sniValDisp[Num][(_(inf_sup, vol_cur))*3+1],sniValDisp[Num][(_(inf_sup, vol_cur))*3+2]


// Auxiliary
int k, j;
char dispChar;					// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
#define WAIT_FOR_KEYBOARD dispChar = sgetchar(0);

// DETECTED ERROR: For an unknown reason, this single value isn't correctly displayed. All the rest seem to work OK
//		(comment it to see missing value in SnI config display -> INT_VCC_12V inferior voltage protocols incorrectly displayed)
#define DETECTED_ERROR_DISP_INFC sniAlmStDisp[12][0][3]=(supList[12].alarmProtocols[__(0, 2)]?ProtocolON_DISP:ProtocolOFF_DISP);



//=====================================================================================================//
//====================================== PUBLIC DISPLAY MENU ==========================================//
//=====================================================================================================//

void DisplayINFC ( void ){
	ERASE_CONSOLE
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("-------------------------------- DISPLAY MENU ---------------------------------\r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf(" (2) Display Status\r\n" );
	iprintf(" (3) Display PSU Configuration\r\n" );
	iprintf(" (4) Display SnI Configuration\r\n" );
	iprintf("\n (e) EXIT TO INTERFACE MENU \r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("\r\nEnter command: " );
	categorySelectionINFC = sgetchar( 0 ); iprintf("%c\r\n", categorySelectionINFC);
	iprintf("-------------------------------------------------------------------------------\r\n" );
	processCommandDisplayINFC();
}



//=====================================================================================================//
//=================================== PRIVATE DISPLAY METHODS =========================================//
//=====================================================================================================//

// Choose Display
void processCommandDisplayINFC ( void ){
	switch (categorySelectionINFC){
		case '1': valuesDisplayINFC (); 				break;
		case '2': statusDisplayINFC();  				break;
		case '3': configDisplayINFC(PSU_TYPE_LIST);  	break;
		case '4': configDisplayINFC(SnI_TYPE_LIST); 	break;
		case EXIT: iprintf("Exiting to INTERFACE MENU\n");  break;
		default:
				iprintf( "\nINVALID KEY -> %c\r\n", categorySelectionINFC);
				iprintf( " \r\nPRESS ANY KEY TO RETURN TO DISPLAY MENU\r\n" );
				break;
	}
	WAIT_FOR_KEYBOARD
}


//================================= (1 & 2) STATUS DISPLAY METHODS   =======================================//

// (1)
void valuesDisplayINFC ( void ){
	refreshValuesStatusINFC();
		ERASE_CONSOLE
		iprintf("============================== MONITOR DISPLAY ================================\r\n" );
		iprintf("_______________________________________________________________________________\r\n" );
		iprintf("||    1A-%s Vp=%sV     |    2A-%s Vp=%sV    |    3A-%s Vp=%sV    ||\r\n", psuStDisp[0], psuValDisp[0][0], psuStDisp[2], psuValDisp[2][0], psuStDisp[4], psuValDisp[4][0]);
		iprintf("||  vO=%s cO=%s  |  vO=%s cO=%s |  vO=%s cO=%s ||\r\n", psuValDisp[0][1],psuValDisp[0][2],  psuValDisp[2][1],psuValDisp[2][2], psuValDisp[4][1],psuValDisp[4][2]);
		iprintf("_______________________________________________________________________________\r\n" );
		iprintf("||    1B-%s Vp=%sV     |    2B-%s Vp=%sV    |    3B-%s Vp=%sV    ||\r\n", psuStDisp[1], psuValDisp[1][0], psuStDisp[3], psuValDisp[3][0], psuStDisp[5], psuValDisp[5][0]);
		iprintf("||  vO=%s cO=%s  |  vO=%s cO=%s |  vO=%s cO=%s ||\r\n", psuValDisp[1][1],psuValDisp[1][2],  psuValDisp[3][1],psuValDisp[3][2], psuValDisp[5][1],psuValDisp[5][2]);
		iprintf("_______________________________________________________________________________\r\n" );
		iprintf("_______________________________________________________________________________\r\n" );
		iprintf("||    4A-%s Vp=%sV     |    5A-%s Vp=%sV    |    6A-%s Vp=%sV    ||\r\n", psuStDisp[6], psuValDisp[6][0], psuStDisp[8], psuValDisp[8][0], psuStDisp[10], psuValDisp[10][0]);
		iprintf("||  vO=%s cO=%s  |  vO=%s cO=%s |  vO=%s cO=%s ||\r\n", psuValDisp[6][1],psuValDisp[6][2],  psuValDisp[8][1],psuValDisp[8][2], psuValDisp[10][1],psuValDisp[10][2]);
		iprintf("_______________________________________________________________________________\r\n" );
		iprintf("||    4B-%s Vp=%sV     |    5B-%s Vp=%sV    |    6B-%s Vp=%sV    ||\r\n", psuStDisp[7], psuValDisp[7][0], psuStDisp[9], psuValDisp[9][0], psuStDisp[11], psuValDisp[11][0]);
		iprintf("||  vO=%s cO=%s  |  vO=%s cO=%s |  vO=%s cO=%s ||\r\n", psuValDisp[7][1],psuValDisp[7][2],  psuValDisp[9][1],psuValDisp[9][2], psuValDisp[11][1],psuValDisp[11][2]);
		iprintf("_______________________________________________________________________________\r\n\n" );
}

// (2)
void statusDisplayINFC ( void ){
	refreshAlarmStatusINFC();
	refreshValuesStatusINFC();
	iprintf( "\n\n\n\n\n\n" );
	iprintf("=============================== STATUS DISPLAY ================================\r\n" );
	iprintf("_______________________________________________________________________________\r\n" );
	iprintf("||    1A-%s Vp=%sV     |    2A-%s Vp=%sV    |    3A-%s Vp=%sV    ||\r\n", psuStDisp[0], psuValDisp[0][0], psuStDisp[2], psuValDisp[2][0], psuStDisp[4], psuValDisp[4][0]);
	iprintf("||  vO=%s L-%s/U-%s | vO=%s L-%s/U-%s | vO=%s L-%s/U-%s ||\r\n", psuValDisp[0][1],psuAlmStDisp[0][0],psuAlmStDisp[0][1], psuValDisp[2][1],psuAlmStDisp[2][0],psuAlmStDisp[2][1], psuValDisp[4][1],psuAlmStDisp[4][0],psuAlmStDisp[4][1]);
	iprintf("||  cO=%s L-%s/U-%s | cO=%s L-%s/U-%s | cO=%s L-%s/U-%s ||\r\n", psuValDisp[0][2],psuAlmStDisp[0][2],psuAlmStDisp[0][3], psuValDisp[2][2],psuAlmStDisp[2][2],psuAlmStDisp[2][3], psuValDisp[4][2],psuAlmStDisp[4][2],psuAlmStDisp[4][3]);
	iprintf("_______________________________________________________________________________\r\n" );
	iprintf("||    1B-%s Vp=%sV     |    2B-%s Vp=%sV    |    3B-%s Vp=%sV    ||\r\n", psuStDisp[1], psuValDisp[1][0], psuStDisp[3], psuValDisp[3][0], psuStDisp[5], psuValDisp[5][0]);
	iprintf("||  vO=%s L-%s/U-%s | vO=%s L-%s/U-%s | vO=%s L-%s/U-%s ||\r\n", psuValDisp[1][1],psuAlmStDisp[1][0],psuAlmStDisp[1][1], psuValDisp[3][1],psuAlmStDisp[3][0],psuAlmStDisp[3][1], psuValDisp[5][1],psuAlmStDisp[5][0],psuAlmStDisp[5][1]);
	iprintf("||  cO=%s L-%s/U-%s | cO=%s L-%s/U-%s | cO=%s L-%s/U-%s ||\r\n", psuValDisp[1][2],psuAlmStDisp[1][2],psuAlmStDisp[1][3], psuValDisp[3][2],psuAlmStDisp[3][2],psuAlmStDisp[3][3], psuValDisp[5][2],psuAlmStDisp[5][2],psuAlmStDisp[5][3]);
	iprintf("_______________________________________________________________________________\r\n" );
	iprintf("_______________________________________________________________________________\r\n" );
	iprintf("||    4A-%s Vp=%sV     |    5A-%s Vp=%sV    |    6A-%s Vp=%sV    ||\r\n", psuStDisp[6], psuValDisp[6][0], psuStDisp[8], psuValDisp[8][0], psuStDisp[10], psuValDisp[10][0]);
	iprintf("||  vO=%s L-%s/U-%s | vO=%s L-%s/U-%s | vO=%s L-%s/U-%s ||\r\n", psuValDisp[6][1],psuAlmStDisp[6][0],psuAlmStDisp[6][1], psuValDisp[8][1],psuAlmStDisp[8][0],psuAlmStDisp[8][1], psuValDisp[10][1],psuAlmStDisp[10][0],psuAlmStDisp[10][1]);
	iprintf("||  cO=%s L-%s/U-%s | cO=%s L-%s/U-%s | cO=%s L-%s/U-%s ||\r\n", psuValDisp[6][2],psuAlmStDisp[6][2],psuAlmStDisp[6][3], psuValDisp[8][2],psuAlmStDisp[8][2],psuAlmStDisp[8][3], psuValDisp[10][2],psuAlmStDisp[10][2],psuAlmStDisp[10][3]);
	iprintf("_______________________________________________________________________________\r\n" );
	iprintf("||    4B-%s Vp=%sV     |    5B-%s Vp=%sV    |    6B-%s Vp=%sV    ||\r\n", psuStDisp[7], psuValDisp[7][0], psuStDisp[9], psuValDisp[9][0], psuStDisp[11], psuValDisp[11][0]);
	iprintf("||  vO=%s L-%s/U-%s | vO=%s L-%s/U-%s | vO=%s L-%s/U-%s ||\r\n", psuValDisp[7][1],psuAlmStDisp[7][0],psuAlmStDisp[7][1], psuValDisp[9][1],psuAlmStDisp[9][0],psuAlmStDisp[9][1], psuValDisp[11][1],psuAlmStDisp[11][0],psuAlmStDisp[11][1]);
	iprintf("||  cO=%s L-%s/U-%s | cO=%s L-%s/U-%s | cO=%s L-%s/U-%s ||\r\n", psuValDisp[7][2],psuAlmStDisp[7][2],psuAlmStDisp[7][3], psuValDisp[9][2],psuAlmStDisp[9][2],psuAlmStDisp[9][3], psuValDisp[11][2],psuAlmStDisp[11][2],psuAlmStDisp[11][3]);
	iprintf("_______________________________________________________________________________\r\n\n" );
	if (config_MonitorCTRL_SnI_FLAG){
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("_______________________________________________________________________________\r\n" );
	iprintf("||      SUP_42V_UNREG      |      SUP_35V_UNREG     |      SUP_16V_UNREG     ||\r\n");
	iprintf("||  vO=%s L-%s/U-%s | vO=%s L-%s/U-%s | vO=%s L-%s/U-%s ||\r\n", sniValDisp[0][1],sniAlmStDisp[0][0],sniAlmStDisp[0][1], sniValDisp[1][1],sniAlmStDisp[1][0],sniAlmStDisp[1][1], sniValDisp[2][1],sniAlmStDisp[2][0],sniAlmStDisp[2][1]);
	iprintf("||      SUP_32V_REG        |      SUP_16V_REG       |      SUP_12V_F_A       ||\r\n");
	iprintf("||  vO=%s L-%s/U-%s | vO=%s L-%s/U-%s | vO=%s L-%s/U-%s ||\r\n", sniValDisp[3][1],sniAlmStDisp[3][0],sniAlmStDisp[3][1], sniValDisp[4][1],sniAlmStDisp[4][0],sniAlmStDisp[4][1], sniValDisp[5][1],sniAlmStDisp[5][0],sniAlmStDisp[5][1]);
	iprintf("||      SUP_12V_F_B        |      SUP_12V_F_C       |      SUP_n16_REG       ||\r\n");
	iprintf("||  vO=%s L-%s/U-%s | vO=%s L-%s/U-%s | vO=%s L-%s/U-%s ||\r\n", sniValDisp[6][1],sniAlmStDisp[6][0],sniAlmStDisp[6][1], sniValDisp[7][1],sniAlmStDisp[7][0],sniAlmStDisp[7][1], sniValDisp[8][1],sniAlmStDisp[8][0],sniAlmStDisp[8][1]);
	iprintf("||      SUP_n20_UNREG      |      SUP_12V_F_D       |                        ||\r\n");
	iprintf("||  vO=%s L-%s/U-%s | vO=%s L-%s/U-%s |                        ||\r\n", sniValDisp[9][1],sniAlmStDisp[9][0],sniAlmStDisp[9][1], sniValDisp[10][1],sniAlmStDisp[10][0],sniAlmStDisp[10][1]);
	iprintf("_______________________________________________________________________________\r\n" );
	iprintf("||      INT_VCC_3V3        |      INT_VCC_12V       |      INT_VCC_n12V      ||\r\n");
	iprintf("||  vO=%s L-%s/U-%s | vO=%s L-%s/U-%s | vO=%s L-%s/U-%s ||\r\n", sniValDisp[11][1],sniAlmStDisp[11][0],sniAlmStDisp[11][1], sniValDisp[12][1],sniAlmStDisp[12][0],sniAlmStDisp[12][1], sniValDisp[13][1],sniAlmStDisp[13][0],sniAlmStDisp[13][1]);
	iprintf("_______________________________________________________________________________\r\n" );
	}
}


void refreshAlarmStatusINFC ( void ){
	// PSU
	for (k=0; k<PSU_NUMBER; k++){
		for (j=0; j<4; j++){ // All alarms
			if (psuList[k].alarmWatch[j]){
				if (psuList[k].alarmCounters[j]>0){
					if(psuList[k].alarmStatus[j]){	mergeBuffer(psuAlmStDisp[k][j], (BYTE*)alarmMessageTriggered, 3);}	 	// Trg
					else{							mergeBuffer(psuAlmStDisp[k][j], (BYTE*)alarmMessageLimReached, 3);}}	// Lim
				else{								mergeBuffer(psuAlmStDisp[k][j], (BYTE*)alarmMessageConnected, 3);}} 	// Con
			else{									mergeBuffer(psuAlmStDisp[k][j], (BYTE*)alarmMessageDisconnected, 3);}	// Dis
			psuAlmStDisp[k][j][3]='\0';
		}
	}

	// SnI
	for (k=0; k<SUP_NUMBER; k++){
		// Inferior Voltage Alarm
		if (supList[k].alarmWatch[0]){
			if (supList[k].alarmCounters[0]>0){
				if(supList[k].alarmStatus[0]){	mergeBuffer(sniAlmStDisp[k][0], (BYTE*)alarmMessageTriggered, 3);}	 	// Trg
				else{							mergeBuffer(sniAlmStDisp[k][0], (BYTE*)alarmMessageLimReached, 3);}}	// Lim
			else{								mergeBuffer(sniAlmStDisp[k][0], (BYTE*)alarmMessageConnected, 3);}} 	// Con
		else{									mergeBuffer(sniAlmStDisp[k][0], (BYTE*)alarmMessageDisconnected, 3);}	// Dis
		sniAlmStDisp[k][0][3]='\0';
		// Superior Voltage Alarm
		if (supList[k].alarmWatch[1]){
			if (supList[k].alarmCounters[1]>0){
				if(supList[k].alarmStatus[1]){	mergeBuffer(sniAlmStDisp[k][1], (BYTE*)alarmMessageTriggered, 3);}	 	// Trg
				else{							mergeBuffer(sniAlmStDisp[k][1], (BYTE*)alarmMessageLimReached, 3);}}	// Lim
			else{								mergeBuffer(sniAlmStDisp[k][1], (BYTE*)alarmMessageConnected, 3);}} 	// Con
		else{									mergeBuffer(sniAlmStDisp[k][1], (BYTE*)alarmMessageDisconnected, 3);}	// Dis
		sniAlmStDisp[k][1][3]='\0';
	}
}

void refreshValuesStatusINFC ( void ){
	memset(psuValDisp, 0, sizeof(psuValDisp));
	memset(psuStDisp, 0, sizeof(psuStDisp));
	// PSU
	for (k=0; k<PSU_NUMBER; k++){
		mergeBuffer(psuValDisp[k][0],(BYTE*) ftos(radarVolts[k], 1), 7);		fillWithBlank(psuValDisp[k][0],5); // progValue
		mergeBuffer(psuValDisp[k][1],(BYTE*) ftos(psuList[k].vOut, 3), 7);		fillWithBlank(psuValDisp[k][1],7); // vOut
		mergeBuffer(psuValDisp[k][2],(BYTE*) ftos(psuList[k].cOut, 3), 7);		fillWithBlank(psuValDisp[k][2],7); // cOut

		mergeBuffer(psuStDisp[k],(BYTE*) (psuList[k].relayStatus?"ON ":"OFF"), 3); // psuStatus
	}

	if (config_MonitorCTRL_SnI_FLAG){
		// SnI
		for (k=0; k<SUP_NUMBER; k++){
			mergeBuffer(sniValDisp[k][0],(BYTE*) ftos(supList[k].nominalVolt , 3), 7);	fillWithBlank(sniValDisp[k][0],7); 	// nominalVoltage
			mergeBuffer(sniValDisp[k][1],(BYTE*) ftos(supList[k].vOut, 3), 7);				fillWithBlank(sniValDisp[k][1],7); 	// vOut
		}
	}
}




//============================== (3 & 4) CONFIGURATION DISPLAY METHODS   ====================================//

void configDisplayINFC ( BOOL psu_sni ){
	refreshAlarmConfigINFC(psu_sni);
	refreshValuesConfigINFC(psu_sni);
	iprintf( "\n\n\n\n\n\n" );
	iprintf("=============================== CONFIG DISPLAY ================================\r\n" );
	if(psu_sni==PSU_TYPE_LIST){
		iprintf("_______________________________________________________________________________\n" );
		iprintf("|1A-%s  %s%s%s%s |2A-%s  %s%s%s%s |3A-%s  %s%s%s%s |\n", printHeaderPsuDISP(0)						,printHeaderPsuDISP(2)						,printHeaderPsuDISP(4)						);
		iprintf("|vi %s t%s %s/%s|vi %s t%s %s/%s|vi %s t%s %s/%s|\n"   , printConfigPsuDISP(0, INFERIOR, VOLTAGE)	,printConfigPsuDISP(2, INFERIOR, VOLTAGE)	,printConfigPsuDISP(4, INFERIOR, VOLTAGE)	);
		iprintf("|vs %s t%s %s/%s|vs %s t%s %s/%s|vs %s t%s %s/%s|\n"   , printConfigPsuDISP(0, SUPERIOR, VOLTAGE)	,printConfigPsuDISP(2, SUPERIOR, VOLTAGE)	,printConfigPsuDISP(4, SUPERIOR, VOLTAGE)	);
		iprintf("|ci %s t%s %s/%s|ci %s t%s %s/%s|ci %s t%s %s/%s|\n"   , printConfigPsuDISP(0, INFERIOR, CURRENT)	,printConfigPsuDISP(2, INFERIOR, CURRENT)	,printConfigPsuDISP(4, INFERIOR, CURRENT)	);
		iprintf("|cs %s t%s %s/%s|cs %s t%s %s/%s|cs %s t%s %s/%s|\n"   , printConfigPsuDISP(0, SUPERIOR, CURRENT)	,printConfigPsuDISP(2, SUPERIOR, CURRENT)	,printConfigPsuDISP(4, SUPERIOR, CURRENT)	);
		iprintf("_______________________________________________________________________________\n\n" );
		iprintf("|1B-%s  %s%s%s%s |2B-%s  %s%s%s%s |3B-%s  %s%s%s%s |\n", printHeaderPsuDISP(1)						,printHeaderPsuDISP(3)						,printHeaderPsuDISP(5)						);
		iprintf("|vi %s t%s %s/%s|vi %s t%s %s/%s|vi %s t%s %s/%s|\n"   , printConfigPsuDISP(1, INFERIOR, VOLTAGE)	,printConfigPsuDISP(3, INFERIOR, VOLTAGE)	,printConfigPsuDISP(5, INFERIOR, VOLTAGE)	);
		iprintf("|vs %s t%s %s/%s|vs %s t%s %s/%s|vs %s t%s %s/%s|\n"   , printConfigPsuDISP(1, SUPERIOR, VOLTAGE)	,printConfigPsuDISP(3, SUPERIOR, VOLTAGE)	,printConfigPsuDISP(5, SUPERIOR, VOLTAGE)	);
		iprintf("|ci %s t%s %s/%s|ci %s t%s %s/%s|ci %s t%s %s/%s|\n"   , printConfigPsuDISP(1, INFERIOR, CURRENT)	,printConfigPsuDISP(3, INFERIOR, CURRENT)	,printConfigPsuDISP(5, INFERIOR, CURRENT)	);
		iprintf("|cs %s t%s %s/%s|cs %s t%s %s/%s|cs %s t%s %s/%s|\n"   , printConfigPsuDISP(1, SUPERIOR, CURRENT)	,printConfigPsuDISP(3, SUPERIOR, CURRENT)	,printConfigPsuDISP(5, SUPERIOR, CURRENT)	);
		iprintf("_______________________________________________________________________________\n\n" );
		iprintf("-------------------------------------------------------------------------------\n" );
		iprintf("_______________________________________________________________________________\n\n" );
		iprintf("|4A-%s  %s%s%s%s |5A-%s  %s%s%s%s |6A-%s  %s%s%s%s |\n", printHeaderPsuDISP(6)						,printHeaderPsuDISP(8)						,printHeaderPsuDISP(10)						);
		iprintf("|vi %s t%s %s/%s|vi %s t%s %s/%s|vi %s t%s %s/%s|\n"   , printConfigPsuDISP(6, INFERIOR, VOLTAGE)	,printConfigPsuDISP(8, INFERIOR, VOLTAGE)	,printConfigPsuDISP(10, INFERIOR, VOLTAGE)	);
		iprintf("|vs %s t%s %s/%s|vs %s t%s %s/%s|vs %s t%s %s/%s|\n"   , printConfigPsuDISP(6, SUPERIOR, VOLTAGE)	,printConfigPsuDISP(8, SUPERIOR, VOLTAGE)	,printConfigPsuDISP(10, SUPERIOR, VOLTAGE)	);
		iprintf("|ci %s t%s %s/%s|ci %s t%s %s/%s|ci %s t%s %s/%s|\n"   , printConfigPsuDISP(6, INFERIOR, CURRENT)	,printConfigPsuDISP(8, INFERIOR, CURRENT)	,printConfigPsuDISP(10, INFERIOR, CURRENT)	);
		iprintf("|cs %s t%s %s/%s|cs %s t%s %s/%s|cs %s t%s %s/%s|\n"   , printConfigPsuDISP(6, SUPERIOR, CURRENT)	,printConfigPsuDISP(8, SUPERIOR, CURRENT)	,printConfigPsuDISP(10, SUPERIOR, CURRENT)	);
		iprintf("_______________________________________________________________________________\n\n" );
		iprintf("|4B-%s  %s%s%s%s |5B-%s  %s%s%s%s |6B-%s  %s%s%s%s |\n", printHeaderPsuDISP(7)						,printHeaderPsuDISP(9)						,printHeaderPsuDISP(11)						);
		iprintf("|vi %s t%s %s/%s|vi %s t%s %s/%s|vi %s t%s %s/%s|\n"   , printConfigPsuDISP(7, INFERIOR, VOLTAGE)	,printConfigPsuDISP(9, INFERIOR, VOLTAGE)	,printConfigPsuDISP(11, INFERIOR, VOLTAGE)	);
		iprintf("|vs %s t%s %s/%s|vs %s t%s %s/%s|vs %s t%s %s/%s|\n"   , printConfigPsuDISP(7, SUPERIOR, VOLTAGE)	,printConfigPsuDISP(9, SUPERIOR, VOLTAGE)	,printConfigPsuDISP(11, SUPERIOR, VOLTAGE)	);
		iprintf("|ci %s t%s %s/%s|ci %s t%s %s/%s|ci %s t%s %s/%s|\n"   , printConfigPsuDISP(7, INFERIOR, CURRENT)	,printConfigPsuDISP(9, INFERIOR, CURRENT)	,printConfigPsuDISP(11, INFERIOR, CURRENT)	);
		iprintf("|cs %s t%s %s/%s|cs %s t%s %s/%s|cs %s t%s %s/%s|\n"   , printConfigPsuDISP(7, SUPERIOR, CURRENT)	,printConfigPsuDISP(9, SUPERIOR, CURRENT)	,printConfigPsuDISP(11, SUPERIOR, CURRENT)	);
		iprintf("_______________________________________________________________________________\n\n" );
	}
	else{
		DETECTED_ERROR_DISP_INFC
		iprintf("_______________________________________________________________________________\n" );
		iprintf("|| SUP_42V_UNREG %s %s | SUP_35V_UNREG %s %s| SUP_16V_UNREG %s %s||\n"		, printHeaderSniDISP(0)						,printHeaderSniDISP(1)						,printHeaderSniDISP(2) );
		iprintf("||  >vi %s t%s --/%s   |  >vi %s t%s --/%s  |  >vi %s t%s --/%s  ||\n"			, printConfigSniDISP(0, INFERIOR, VOLTAGE)	,printConfigSniDISP(1, INFERIOR, VOLTAGE)	,printConfigSniDISP(2, INFERIOR, VOLTAGE));
		iprintf("||  >vs %s t%s --/%s   |  >vs %s t%s --/%s  |  >vs %s t%s --/%s  ||\n"			, printConfigSniDISP(0, SUPERIOR, VOLTAGE)	,printConfigSniDISP(1, SUPERIOR, VOLTAGE)	,printConfigSniDISP(2, SUPERIOR, VOLTAGE));
		iprintf("|| SUP_32V_REG  %s %s  | SUP_16V_REG  %s %s | SUP_12V_F_A  %s %s ||\n"		, printHeaderSniDISP(3)						,printHeaderSniDISP(4)						,printHeaderSniDISP(5) );
		iprintf("||  >vi %s t%s --/%s   |  >vi %s t%s --/%s  |  >vi %s t%s --/%s  ||\n"			, printConfigSniDISP(3, INFERIOR, VOLTAGE)	,printConfigSniDISP(4, INFERIOR, VOLTAGE)	,printConfigSniDISP(5, INFERIOR, VOLTAGE));
		iprintf("||  >vs %s t%s --/%s   |  >vs %s t%s --/%s  |  >vs %s t%s --/%s  ||\n"			, printConfigSniDISP(3, SUPERIOR, VOLTAGE)	,printConfigSniDISP(4, SUPERIOR, VOLTAGE)	,printConfigSniDISP(5, SUPERIOR, VOLTAGE));
		iprintf("|| SUP_12V_F_B  %s %s  | SUP_12V_F_C  %s %s | SUP_n16_REG  %s %s ||\n"		, printHeaderSniDISP(6)						,printHeaderSniDISP(7)						,printHeaderSniDISP(8) );
		iprintf("||  >vi %s t%s --/%s   |  >vi %s t%s --/%s  |  >vi %s t%s --/%s  ||\n"			, printConfigSniDISP(6, INFERIOR, VOLTAGE)	,printConfigSniDISP(7, INFERIOR, VOLTAGE)	,printConfigSniDISP(8, INFERIOR, VOLTAGE));
		iprintf("||  >vs %s t%s --/%s   |  >vs %s t%s --/%s  |  >vs %s t%s --/%s  ||\n"			, printConfigSniDISP(6, SUPERIOR, VOLTAGE)	,printConfigSniDISP(7, SUPERIOR, VOLTAGE)	,printConfigSniDISP(8, SUPERIOR, VOLTAGE));
		iprintf("|| SUP_n20_UNREG %s %s | SUP_12V_F_D  %s %s |                        ||\n"	, printHeaderSniDISP(9)						,printHeaderSniDISP(10)	);
		iprintf("||  >vi %s t%s --/%s   |  >vi %s t%s --/%s  |                        ||\n"		, printConfigSniDISP(9, INFERIOR, VOLTAGE)	,printConfigSniDISP(10, INFERIOR, VOLTAGE));
		iprintf("||  >vs %s t%s --/%s   |  >vs %s t%s --/%s  |                        ||\n"		, printConfigSniDISP(9, SUPERIOR, VOLTAGE)	,printConfigSniDISP(10, SUPERIOR, VOLTAGE));
		iprintf("_______________________________________________________________________________\n" );
		iprintf("|| INT_VCC_3V3  %s %s  | INT_VCC_12V  %s %s | INT_VCC_n12V %s %s ||\n"		, printHeaderSniDISP(11)					,printHeaderSniDISP(12)						,printHeaderSniDISP(13) );
		iprintf("||  >vi %s t%s --/%s   |  >vi %s t%s --/%s  |  >vi %s t%s --/%s  ||\n"			, printConfigSniDISP(11, INFERIOR, VOLTAGE)	,printConfigSniDISP(12, INFERIOR, VOLTAGE)	,printConfigSniDISP(13, INFERIOR, VOLTAGE));
		iprintf("||  >vs %s t%s --/%s   |  >vs %s t%s --/%s  |  >vs %s t%s --/%s  ||\n"			, printConfigSniDISP(11, SUPERIOR, VOLTAGE)	,printConfigSniDISP(12, SUPERIOR, VOLTAGE)	,printConfigSniDISP(13, SUPERIOR, VOLTAGE));
		iprintf("_______________________________________________________________________________\n" );
	}
}


void refreshAlarmConfigINFC ( BOOL psu_sni ){
	if(psu_sni==PSU_TYPE_LIST){// PSU
		for (k=0; k<PSU_NUMBER; k++){
			for (j=0; j<ALARM_NUMBER; j++){ // All alarms
				psuAlmStDisp[k][j][0]=(psuList[k].alarmWatch[j]?alarmConnected_DISP:alarmDisconnected_DISP);
				psuAlmStDisp[k][j][1]=(psuList[k].alarmProtocols[__(j, 0)]?ProtocolON_DISP:ProtocolOFF_DISP);
				psuAlmStDisp[k][j][2]=(psuList[k].alarmProtocols[__(j, 1)]?ProtocolON_DISP:ProtocolOFF_DISP);
				psuAlmStDisp[k][j][3]=(psuList[k].alarmProtocols[__(j, 2)]?ProtocolON_DISP:ProtocolOFF_DISP);
				psuAlmStDisp[k][j][4]='\0';
			}
		}
	}
	else{// SnI
		for (k=0; k<SUP_NUMBER; k++){
			for (j=0; j<ALARM_NUMBER-2; j++){ // Voltage alarms
				sniAlmStDisp[k][j][0]=(supList[k].alarmWatch[j]?alarmConnected_DISP:alarmDisconnected_DISP);
				sniAlmStDisp[k][j][1]=(supList[k].alarmProtocols[__(j, 0)]?ProtocolON_DISP:ProtocolOFF_DISP);
				sniAlmStDisp[k][j][2]=(supList[k].alarmProtocols[__(j, 1)]?ProtocolON_DISP:ProtocolOFF_DISP);
				sniAlmStDisp[k][j][3]=(supList[k].alarmProtocols[__(j, 2)]?ProtocolON_DISP:ProtocolOFF_DISP);
				sniAlmStDisp[k][j][4]='\0';
			}
		}
	}
}

void refreshValuesConfigINFC ( BOOL psu_sni ){

	if(psu_sni==PSU_TYPE_LIST){// PSU
		for (k=0; k<PSU_NUMBER; k++){
			for (j=0; j<ALARM_NUMBER; j++){ // All alarms
				mergeBuffer(psuValDisp[k][4*j],(BYTE*) ftos(psuList[k].alarmLimitValues[j] , 2), 6);		fillWithBlank(psuValDisp[k][4*j],6); 	// alarmLimitValue
				itoa(psuList[k].alarmLimitTimes[j], (char*) psuValDisp[k][4*j+1]);							fillWithBlank(psuValDisp[k][4*j+1],3); 	// alarmLimitTime
				mergeBuffer(psuValDisp[k][4*j+2],(BYTE*) ftos(psuList[k].alarmProtocolVoltage[j], 2), 6);	fillWithBlank(psuValDisp[k][4*j+2],6); 	// ModifyVoltageProtocol
				snprintf((char*)psuValDisp[k][4*j+3], 4, "%x", psuList[k].alarmProtocolShutdown[j]);		fillWithBlank(psuValDisp[k][4*j+3],3); 	// Shutdown Protocol

			}
			mergeBuffer(psuStDisp[k],(BYTE*) (psuList[k].psuStatus?"ON ":"OFF"), 3); // psuStatus
		}
	}

	else{// SnI
		for (k=0; k<SUP_NUMBER; k++){
			for (j=0; j<ALARM_NUMBER-2; j++){ // All alarms
				mergeBuffer(sniValDisp[k][3*j],(BYTE*) ftos(supList[k].alarmLimitValues[j] , 2), 6);		fillWithBlank(sniValDisp[k][3*j],6); 	// alarmLimitValue
				itoa(supList[k].alarmLimitTimes[j], (char*) sniValDisp[k][3*j+1]);							fillWithBlank(sniValDisp[k][3*j+1],3); 	// alarmLimitTime
				snprintf((char*)sniValDisp[k][3*j+2], 3, "%x", supList[k].alarmProtocolShutdown[j]);		fillWithBlank(psuValDisp[k][3*j+2],3); 	// Shutdown Protocol
			}
		}
	}
}

