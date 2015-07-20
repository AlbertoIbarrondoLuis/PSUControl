/*
 * LibrariesTEST.cpp
 *
 *	Hardware + Software tests for all the libraries
 *
 *  Created on: 20/05/2015
 *      Author: Alberto Ibarrondo Luis
 */


#include "Tests/Tests.h"


//==============================================VARIABLES==============================================//
// Keyboard
extern char testChar;					// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
#define WAIT_FOR_KEYBOARD testChar = sgetchar(0);
extern char categorySelectionTEST;

// Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];					// MAIN PSU ARRAY LIST
extern SnI_TYPE sniList [INT_VCC_n12V + 1];				// Supply & Internal voltages List

// Test values
float Test_Rpsu = DEFAULT_rShunt_psu;

// Test Results
extern BYTE results[15];
extern BYTE resultTotal;

// Interface
extern int psuNumINFC;
extern BYTE slaveSPIAddressINFC;	// Set to UPPER_SLAVE_SPI_ADDRESS or LOWER_SLAVE_SPI_ADDRESS
extern BYTE i2cAddressINFC;			// Defined by 3 switches in RDAC bridges

//=====================================================================================================//
//================================    Libraries' Testing METHODS    ===================================//
//=====================================================================================================//

//==================================== MENU & AUXILIARY ===============================================//

void LibrariesTEST ( void ){
	ERASE_CONSOLE
	iprintf("============================= Libraries TEST MENU =============================\r\n" );
	iprintf( "I2C: 0x%x       SLAVE: %s, %d\n",  i2cAddressINFC, (slaveSPIAddressINFC==LOWER_SLAVE_SPI_ADDRESS?"UPPER":"LOWER"), slaveSPIAddressINFC);
	iprintf( "\nADDRESSING OPTIONS\r\n" );
	iprintf( " (+) Change I2C Address destination manually (0x2F to 0x28 for RDACs)\r\n" );
	iprintf( " (-) Toggle SPI Selected slave (Upper/Lower)\r\n" );
	iprintf( " (*) Set I2C & SPI addresses for a certain PSU\r\n" );
	iprintf( " (q) Configure both I2C & SPI Channels\n");
	iprintf( "\nLIBRARIES' TESTS\r\n" );
	iprintf( " (1) TEST_I2CSPILibrary\r\n" );
	iprintf( " (2) TEST_RDACLibrary \r\n" );
	iprintf( " (3) TEST_AGCLibrary\r\n" );
	iprintf( " (4) TEST_RelayLibrary \r\n" );
	iprintf( " (5) TEST_MUXLibrary \r\n" );
	iprintf( "\n (e) EXIT TO TEST MENU \r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "\r\nEnter command: " );
	categorySelectionTEST = sgetchar( 0 ); iprintf ("%c\n", categorySelectionTEST);
	iprintf("\n-------------------------------------------------------------------------------\r\n" );
	switch ( categorySelectionTEST ){
		case '+':		changeI2CAddress_I2CnSPI_INFC();	break;
		case '-':		toggleSPIAddress_I2CnSPI_INFC();	break;
		case '*':		setAddressesPSU_I2CnSPI_INFC(); 	break;
		case 'q': case 'Q': configChannels_I2CSPI_INFC();	break;
		case '1':		TEST_I2CSPILibrary(); 				break;
		case '2':		TEST_RDACLibrary();					break;
		case '3':		TEST_AGCLibrary();					break;
		case '4':		TEST_RelayLibrary();				break;
		case '5':		TEST_MUXLibrary();					break;
		case EXIT: 	iprintf( "\nGOING BACK TO TEST MENU\n");	categorySelectionTEST = EXIT;	break;
		default:	iprintf( "\nINVALID COMMAND -> %c\r\n", categorySelectionTEST);				break;
	}
}



//========================================== TESTS ====================================================//

BOOL TEST_I2CSPILibrary ( void ){
	int i;
	iprintf("\n\n\n\n");
	iprintf("================================ TEST_I2CSPILibrary ===========================\n");
	iprintf(" 1.Sending an I2C message to every bridge ( SPI bus Configuration )\n");
	iprintf("  Press one key to begin test\n");
	WAIT_FOR_KEYBOARD
	for (i = 0; i<6; i++){
		i2cAddressINFC = psuList[2*i].bridgeI2CAdr; iprintf(" Sending an I2C message to Address 0x%x\n", i2cAddressINFC);
		results[i] = (configureSPI( false, false, false, 2, i2cAddressINFC)==I2C_OK);
	}

	iprintf(" \n\n 2.Configuring RDACs' Ctrl register with a non-zero value, and reading Ctrl register\n");
	iprintf("  Press one key to begin test\n");
	WAIT_FOR_KEYBOARD
	for (i = 0; i<6; i++){
		i2cAddressINFC = psuList[i*2].bridgeI2CAdr;
		slaveSPIAddressINFC=UPPER_SLAVE_SPI_ADDRESS;
		iprintf(" Sending an SPI message to Upper slave in I2C Address 0x%x ->", i2cAddressINFC);
		setCtrlRDAC(0,1,0, slaveSPIAddressINFC, i2cAddressINFC);	// Second bit controls RDAC update permission (1)
		results[i+6] = (getCtrlRDAC(slaveSPIAddressINFC, i2cAddressINFC)==0x2);
		slaveSPIAddressINFC=LOWER_SLAVE_SPI_ADDRESS;
		iprintf(" Sending an SPI message to Lower slave in I2C Address 0x%x ->", i2cAddressINFC);
		setCtrlRDAC(0,1,0, slaveSPIAddressINFC, i2cAddressINFC);	// Second bit controls RDAC update permission (1)
		results[i+6] &= (getCtrlRDAC(slaveSPIAddressINFC, i2cAddressINFC)==0x2);
	}

	iprintf("\n\n 3.Using an I2C method from AGC");
	minAGC(); results[12]=getI2CResultAGC()==I2C_OK;
	iprintf("AGC addressing: %s\n", (results[12]?"CORRECT":"INCORRECT"));

	iprintf("\n\n 4.Using an I2C method from Relays (bus expander)");
	disconnectRelay(SF5_B); results[13]=getI2CResultAGC()==I2C_OK;
	iprintf("AGC addressing: %s\n", (results[13]?"CORRECT":"INCORRECT"));

	iprintf("\n TEST_I2CSPILibrary RESULTS:\n");
	iprintf("\n PART 1 - I2C addressing check for PSU bridges\n");
	iprintf(" ~SF1_A & SF1_B: %s\n", (results[0]?"PASSED":"NOT PASSED"));
	iprintf(" ~SF2_A & SF2_B: %s\n", (results[1]?"PASSED":"NOT PASSED"));
	iprintf(" ~SF3_A & SF3_B: %s\n", (results[2]?"PASSED":"NOT PASSED"));
	iprintf(" ~SF4_A & SF4_B: %s\n", (results[3]?"PASSED":"NOT PASSED"));
	iprintf(" ~SF5_A & SF5_B: %s\n", (results[4]?"PASSED":"NOT PASSED"));
	iprintf(" ~SF6_A & SF6_B: %s\n", (results[5]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - SPI addressing check for PSU RDACs\n");
	iprintf(" ~SF1_A & SF1_B: %s\n", (results[6]?"PASSED":"NOT PASSED"));
	iprintf(" ~SF2_A & SF2_B: %s\n", (results[7]?"PASSED":"NOT PASSED"));
	iprintf(" ~SF3_A & SF3_B: %s\n", (results[8]?"PASSED":"NOT PASSED"));
	iprintf(" ~SF4_A & SF4_B: %s\n", (results[9]?"PASSED":"NOT PASSED"));
	iprintf(" ~SF5_A & SF5_B: %s\n", (results[10]?"PASSED":"NOT PASSED"));
	iprintf(" ~SF6_A & SF6_B: %s\n", (results[11]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 3 - I2C addressing for AGC\n");
	iprintf(" ~Result: %s\n", (results[12]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 4 - I2C addressing for Relays (bus expander)\n");
	iprintf(" ~Result: %s\n", (results[13]?"PASSED":"NOT PASSED"));
	resultTotal = (results[0]&&results[1]&&results[2]&&results[3]&&results[4]&&results[5]&&results[6]&&results[7]&&results[8]&&results[9]&&results[10]&&results[11]&&results[12]&&results[13]);
	iprintf("\n OVERALL RESULT: %s\n", (resultTotal?"PASSED":"NOT PASSED"));
	return resultTotal;
}


BOOL TEST_RDACLibrary( void ){
	iprintf("\n\n\n\n");
	iprintf("============================== TEST_RDACLibrary ===============================\n");
	iprintf("\nThis test implies that TEST_I2C&SPILibrary has been successfully passed\n");
	iprintf("\nA single PSU's RDAC will be tested.\n");
	setAddressesPSU_I2CnSPI_INFC();

	iprintf("Do you want to check the output voltage during the test? (y/n) \n");
	char testRDAC = sgetchar(0);
	BOOL check_volt = (testRDAC=='y' || testRDAC=='Y');
	iprintf("Output voltage %s be checked during the test\n", (check_volt?"WILL":"WON'T"));
	iprintf(" Press one key to begin test\n");
	WAIT_FOR_KEYBOARD
	iprintf("\n\n1. Reading RDAC Value\n");
	int setvalue1 = getValRDAC(slaveSPIAddressINFC, i2cAddressINFC);
	results[0] = (setvalue1!=0?true:false);
	if (check_volt){
		printf(" -> Output voltage should be near %.2f\n", countsToVolt(setvalue1, Test_Rpsu));
		WAIT_FOR_KEYBOARD
	}

	int newvalue = (setvalue1>1000?800:900);// 900 = 5V;
	iprintf("\n\n2. Configuring RDAC Value and checking value updating\n");
	newvalue = scanFloatValue(Test_Rpsu);
	setValRDAC(newvalue, slaveSPIAddressINFC, i2cAddressINFC);
	int setvalue2 = getValRDAC(slaveSPIAddressINFC, i2cAddressINFC);
	results[1] = (setvalue2==newvalue);
	if (check_volt){
		iprintf(" -> Output voltage should be near %s\n", ftos(countsToVolt(setvalue2, Test_Rpsu)));
		WAIT_FOR_KEYBOARD
	}

	iprintf("\n\n3. Configuring Control Register to allow RDAC Value to be updated and reading RDAC CTRL Register\n");
	setCtrlRDAC(0,1,0, slaveSPIAddressINFC, i2cAddressINFC);	// Second bit controls RDAC update permission (0)
	results[2] = getCtrlRDAC(slaveSPIAddressINFC, i2cAddressINFC);
	results[2] = (results[2]==0x2);

	iprintf("\n\n4. Configuring Control Register to reject RDAC Value updates and reading RDAC CTRL Register\n");
	setCtrlRDAC(0,0,0, slaveSPIAddressINFC, i2cAddressINFC);	// Second bit controls RDAC update permission (0)
	results[3] = getCtrlRDAC(slaveSPIAddressINFC, i2cAddressINFC);
	results[3] = (results[3]==0);

	iprintf("\n\n5. Resetting RDAC - Value is set to midscale - and reading RDAC Value\n");
	resetRDAC(slaveSPIAddressINFC, i2cAddressINFC);
	int setvalue5 = getValRDAC(slaveSPIAddressINFC, i2cAddressINFC);
	iprintf("Default counts in RDAC should be 0x%x, is that correct? (y/n): "); testRDAC = sgetchar(0); iprintf("%c\n");
	results[4] = (testRDAC=='y' || testRDAC=='Y');
	if (check_volt){
		iprintf(" -> Output voltage should be near %s\n", ftos(countsToVolt(setvalue5, Test_Rpsu)));
		WAIT_FOR_KEYBOARD
	}

	iprintf("\n TEST_RDACLibrary RESULTS FOR PSU %d\n", psuNumINFC);
	iprintf("\n PART 1 - Reading Rdac Value\n");
	iprintf(" ~results[0]: %s\n", (results[0]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - Changing Rdac Value\n");
	iprintf(" ~results[1]: %s\n", (results[1]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 3 - Allowing Rdac updating\n");
	iprintf(" ~results[2]: %s\n", (results[2]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 4 - Rejecting Rdac updates\n");
	iprintf(" ~results[3]: %s\n", (results[3]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 5 - Resetting Rdac\n");
	iprintf(" ~results[4]: %s\n", (results[4]?"PASSED":"NOT PASSED"));
	resultTotal = (results[0]&&results[1]&&results[2]&&results[3]&&results[4]);
	iprintf("\n OVERALL RESULT: %s\n", (resultTotal?"PASSED":"NOT PASSED"));
	return resultTotal;
}


BOOL TEST_AGCLibrary( void ){
	iprintf("\n\n\n\n");
	iprintf("================================= TEST_AGCLibrary =============================\n");
	WAIT_FOR_KEYBOARD
	iprintf("\n\n1. Setting AGC Gain to minimum\n");
	minAGC();		 results[0]= getI2CResultAGC==I2C_OK;
	readValuesAGC(); results[0]&=getCountsAGC()==RDAC_MIN_VALUE;
	getAGCStatus_AGC_INFC();

	iprintf("\n\n2. Scaling AGC Gain to double (2*minimum)\n");
	scaleGainAGC(2); results[1]= getI2CResultAGC==I2C_OK;
	readValuesAGC(); results[1]&=getCountsAGC()==RDAC_MIN_VALUE;
	getAGCStatus_AGC_INFC();

	iprintf("\n\n3. Setting AGC Gain to maximum\n");
	maxAGC();		 results[2]= getI2CResultAGC==I2C_OK;
	readValuesAGC(); results[2]&=getCountsAGC()==RDAC_MAX_VALUE;
	getAGCStatus_AGC_INFC();

	iprintf("\n\n4. Setting AGC Gain to 40\n");
	setGainAGC(40);	 results[3]= getI2CResultAGC==I2C_OK;
	readValuesAGC(); results[3]&=getCountsAGC()==gainToCountsAGC(40);
	getAGCStatus_AGC_INFC();

	iprintf("\n\n5. Exceeding the maximum limit ( scale Gain * 100 )\n");
	scaleGainAGC(100);	results[4]= getI2CResultAGC==I2C_OK;
	readValuesAGC(); 	results[4]&=getCountsAGC()==RDAC_MAX_VALUE;
	getAGCStatus_AGC_INFC();

	iprintf("\n\n6. Surpassing the minimum limit ( set Gain to 0.5 )\n");
	setGainAGC(0.5);	results[5]= getI2CResultAGC==I2C_OK;
	readValuesAGC(); 	results[5]&=getCountsAGC()==RDAC_MIN_VALUE;
	getAGCStatus_AGC_INFC();

	iprintf("\n TEST_AGCLibrary RESULTS\n");
	iprintf("\n PART 1 - Minimum AGC Gain\n");
	iprintf(" ~results[0]: %s\n", (results[0]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - Scaling Gain to double\n");
	iprintf(" ~results[1]: %s\n", (results[1]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 3 - Maximum AGC Gain\n");
	iprintf(" ~results[2]: %s\n", (results[2]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 4 - Setting Gain to 40\n");
	iprintf(" ~results[3]: %s\n", (results[3]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 5 - Setting a value higher than maximum\n");
	iprintf(" ~results[4]: %s\n", (results[4]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 6 - Setting a value lower than minimum\n");
	iprintf(" ~results[5]: %s\n", (results[5]?"PASSED":"NOT PASSED"));
	resultTotal = (results[0]&&results[1]&&results[2]&&results[3]&&results[4]&&results[5]);
	iprintf("\n OVERALL RESULT: %s\n", (resultTotal?"PASSED":"NOT PASSED"));
	return resultTotal;
}


BOOL TEST_RelayLibrary ( void ) {
	char testRelay;
	iprintf("\n\n\n\n");
	iprintf("============================ TEST_RelayLibrary ================================\n");
	iprintf("This test implies that both I2C/SPI and RDAC have been successfully passed\n");
	iprintf("To take on this test, a voltmeter is required to measure the output\n");

	iprintf("\n\n1. Connect a relay using GPIO pins, for SF1_A. Setting Min voltage\n");
	adjustRdac(SF1_A, INITIAL_VOLTAGE);
	connectPSU(SF1_A);
	iprintf("Is the output value of SF1_A close to 1.26? (y/n) "); testRelay = sgetchar(0); iprintf("\n");
	results[0]=testRelay=='y' || testRelay=='Y';

	iprintf("\n\n2. Disconnect a relay using GPIO pins, for SF1_A.\n");
	disconnectPSU(SF1_A);
	iprintf("Is the output value of SF1_A close to 0? (y/n) "); testRelay = sgetchar(0); iprintf("\n");
	results[1]=testRelay=='y' || testRelay=='Y';

	iprintf("\n\n3. Connect a relay using I2C Bus expander, for SF5_A. Setting Min Voltage\n");
	adjustRdac(SF5_A, INITIAL_VOLTAGE);
	connectPSU(SF5_A);
	iprintf("Is the output value of SF4_A close to 1.26? (y/n) "); testRelay = sgetchar(0); iprintf("\n");
	results[2]=testRelay=='y' || testRelay=='Y';

	iprintf("\n\n4. Disconnect a relay using I2C Bus expander, for SF5_A.\n");
	disconnectPSU(SF5_A);
	iprintf("Is the output value of SF4_A close to 0? (y/n) "); testRelay = sgetchar(0); iprintf("\n");
	results[3]=testRelay=='y' || testRelay=='Y';


	iprintf("\n TEST_RelayLibrary RESULTS\n");
	iprintf("\n PART 1 - Connect a relay using GPIO pins, for SF1_A\n");
	iprintf(" ~results[0]: %s\n", (results[0]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - Disconnect a relay using GPIO pins, for SF1_A.\n");
	iprintf(" ~results[1]: %s\n", (results[1]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 3 - Connect a relay using I2C Bus expander, for SF5_A.\n");
	iprintf(" ~results[2]: %s\n", (results[2]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 4 - Disconnect a relay using I2C Bus expander, for SF5_A\n");
	iprintf(" ~results[3]: %s\n", (results[3]?"PASSED":"NOT PASSED"));
	resultTotal = (results[0]&&results[1]&&results[2]&&results[3]);
	iprintf("\n OVERALL RESULT: %s\n", (resultTotal?"PASSED":"NOT PASSED"));
	return resultTotal;
}

BOOL TEST_MUXLibrary ( void ) {
	char testMux;
	iprintf("\n\n\n\n");
	iprintf("============================== TEST_MUXLibrary ================================\n");
	iprintf("This test implies that both I2C/SPI, RDAC and Relay tests have been successfully passed\n");
	iprintf("The test, rather than using directly MUXLibrary, uses Controller functions that require Muxes \n");

	iprintf("\n\n1. Setting minimum Voltage to PSU 0 and measuring it\n");
	adjustRdac(0, INITIAL_VOLTAGE);
	connectPSU(0);
	readVoltageValue(0, PSU_TYPE_LIST);
	disconnectPSU(0);
	iprintf("Voltage value = %s V (should be around 1.26)\n", ftos(psuList[0].vOut));
	iprintf("Is this a correct value? (y/n) "); testMux = sgetchar(0); iprintf("\n");
	results[0]=testMux=='y' || testMux=='Y';

	iprintf("\n\n2. Setting 16V to PSU 5 and measuring it\n");
	adjustRdac(5, 16);
	connectPSU(5);
	readVoltageValue(5, PSU_TYPE_LIST);
	disconnectPSU(5);
	iprintf("Voltage value = %s V (should be around 16)\n", ftos(psuList[5].vOut));
	iprintf("Is this a correct value? (y/n) "); testMux = sgetchar(0); iprintf("\n");
	results[1]=testMux=='y' || testMux=='Y';

	iprintf("\n\n3. Setting -10V to PSU 10 and measuring it\n");
	adjustRdac(10, -16);
	connectPSU(10);
	readVoltageValue(10, PSU_TYPE_LIST);
	disconnectPSU(10);
	iprintf("Voltage value = %s V (should be around -16)\n", ftos(psuList[10].vOut));
	iprintf("Is this a correct value? (y/n) "); testMux = sgetchar(0); iprintf("\n");
	results[2]=testMux=='y' || testMux=='Y';

	iprintf("\n\n4. Reading SnI -> SUP_42V_UNREG\n");
	readVoltageValue(SUP_42V_UNREG, SnI_TYPE_LIST);
	iprintf("Voltage value = %s V (should be around 42)\n", ftos(sniList[SUP_42V_UNREG].vOut));
	iprintf("Is this a correct value? (y/n) "); testMux = sgetchar(0); iprintf("\n");
	results[3]=testMux=='y' || testMux=='Y';

	iprintf("\n\n5. Reading SnI -> INT_VCC_n12V\n");
	readVoltageValue(INT_VCC_n12V, SnI_TYPE_LIST);
	iprintf("Voltage value = %s V (should be around -12)\n", ftos(sniList[INT_VCC_n12V].vOut));
	iprintf("Is this a correct value? (y/n) "); testMux = sgetchar(0); iprintf("\n");
	results[4]=testMux=='y' || testMux=='Y';


	iprintf("\n TEST_MUXLibrary RESULTS\n");
	iprintf("\n PART 1 - Minimum Voltage to PSU 0\n");
	iprintf(" ~results[0]: %s\n", (results[0]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - (16V) to PSU 5\n");
	iprintf(" ~results[1]: %s\n", (results[1]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 3 - (-10V) to PSU 10\n");
	iprintf(" ~results[2]: %s\n", (results[2]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 4 - Reading SUP_42V_UNREG\n");
	iprintf(" ~results[3]: %s\n", (results[3]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 5 - Reading INT_VCC_n12V\n");
	iprintf(" ~results[4]: %s\n", (results[4]?"PASSED":"NOT PASSED"));
	resultTotal = (results[0]&&results[1]&&results[2]&&results[3]&&results[4]);
	iprintf("\n OVERALL RESULT: %s\n", (resultTotal?"PASSED":"NOT PASSED"));
	return resultTotal;
}


