/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*     Main settings dialog for Sms.
*
*/



#ifndef SMUMSETTINGSDIALOG_H
#define SMUMSETTINGSDIALOG_H

//  INCLUDES
#include <AknForm.h> // CAknDialog
#include <eiklbo.h>  // MEikListBoxObserver


// MACROS

// DATA TYPES
enum TSmumSettingsYesNo
    {
    ESmumSettingsYes = 0,
    ESmumSettingsNo = 1
    };


// Exitcodes
enum TSmumSmsSettingsExitCodes 
    {
    ESmumSmsSettingsInitialvalue = 0,
    ESmumSmsSettingsBack,
    ESmumSmsSettingsMenuExit,
    ESmumSmsSettingsSystemExit
    };

// Settingstypes
enum TSmumSmsSettingsType 
    {
    ESmumMainSmsSettings = 0,
    ESmumSendingOptions
    };

// Character-set setting values
// Enumeration used also in SmsEditor
enum TCharSetSupport
    {
    ECharSetFull,
    ECharSetReduced,
    ECharSetFullLocked,
    ECharSetReducedLocked
    };
    
// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CSmsSettings;
class CSmsHeader;

// CLASS DECLARATION

/**
*  SMUM's main sms settings dialog.
*/
class CSmumMainSettingsDialog :
    public CAknDialog,
    public MEikListBoxObserver
    {
    public:  // Constructors and destructor
      
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CSmumMainSettingsDialog* NewL(
            CSmsSettings& aSettings,
            TInt aTypeOfSettings,
            TInt& aExitCode,
            TInt& aCharSetSupportForSendingOptions,
            CSmsHeader* aSmsHeader = NULL 
            );
    };

#endif      // SMUMSETTINGSDIALOG_H   

// End of File
