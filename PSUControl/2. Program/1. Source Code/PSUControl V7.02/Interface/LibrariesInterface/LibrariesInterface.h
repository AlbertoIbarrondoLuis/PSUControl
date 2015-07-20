/*
 * LibrariesInterface.h
 *
 *	Interface Menu and Options for each Library in folder Libraries, making almost every
 *	libraries' method ready to use
 *
 *  Created on: 22-jun-2015
 *      Author: Alberto Ibarrondo Luis
 */

#ifndef LIBRARIESINTERFACE_H_
#define LIBRARIESINTERFACE_H_

#include "Headers.h"
#include "Libraries/Libraries.h"		// All the Libraries' Methods
#include "Controller/Controller.h"		// All the Controller Methods

//-------------------------------------- AGC Library ------------------------------------------//
//*********Public Methods*********//
void AGCLibraryINFC ( void );				// AGC MENU
//*********Private Methods********//
void setAGCMinimum_AGC_INFC ( void );		// (1)
void setAGCMaximum_AGC_INFC ( void );		// (2)
void scaleAGCGain_AGC_INFC ( void );		// (3)
void setAGCGain_AGC_INFC ( void );			// (4)
void countsToGain_AGC_INFC ( void );		// (5)
void gainToCounts_AGC_INFC ( void );		// (6)
void getAGCStatus_AGC_INFC ( void );		// (7)


//------------------------------------ General Library ----------------------------------------//
int selectPSUNumINFC ( void );


//------------------------------------ I2CnSPI Library ----------------------------------------//
//*********Public Methods*********//
void I2CnSPILibraryINFC( void );			// I2CnSPI MENU
//*********Private Methods********//
void configChannels_I2CSPI_INFC( void );	// (1)
void sendI2CMessage_I2CnSPI_INFC( void );	// (2)
void changeI2CAddress_I2CnSPI_INFC( void );	// (+)
void toggleSPIAddress_I2CnSPI_INFC( void );	// (-)
void setAddressesPSU_I2CnSPI_INFC( void );	// (*)

//--------------------------------------- MUX Library ------------------------------------------//
//*********Public Methods*********//
void MUXLibraryINFC ( void );				// MUX MENU
//*********Private Methods********//
void setMuxes_MUX_INFC ( void );			// (1)
void readPSUVoltage_MUX_INFC ( void );		// (2)
void readPSUCurrent_MUX_INFC ( void );		// (3)
void readSnIVoltage_MUX_INFC ( void );		// (4)
void getMuxesStatus_MUX_INFC ( void );		// (5)


//------------------------------------- RDAC Library ------------------------------------------//
//*********Public Methods*********//
void RDACLibraryINFC( void );				// RDAC MENU
//*********Private Methods********//
void setRDACValue_RDAC_INFC ( void );		// (1)
void readRDACValue_RDAC_INFC ( void );		// (2)
void toggleRDACCtrl_RDAC_INFC ( void );		// (3)
void getRDACCtrl_RDAC_INFC ( void );		// (4)
void resetRDAC_RDAC_INFC ( void );			// (5)
void higImpSDO_RDAC_INFC ( void );			// (6)
void scaleSetRDACValue_RDAC_INFC ( void );	// (7)
void programMemRDAC_RDAC_INFC ( void );		// (8)
void readMemRDAC_RDAC_INFC ( void );		// (9)
void voltsToCounts_RDAC_INFC ( void );		// (A)
void countsToVolts_RDAC_INFC ( void );		// (B)


//-------------------------------------- Relay Library -----------------------------------------//
//*********Public Methods*********//
void RelayFunctionsDISP ( void );			// Relay MENU
//*********Private Methods********//
void connectRelay_Relay_INFC ( void );		// (1)
void disconnectRelay_Relay_INFC ( void );	// (2)
void relayStatus_Relay_INFC ( void );		// (3)


#endif /* LIBRARIESINTERFACE_H_ */
