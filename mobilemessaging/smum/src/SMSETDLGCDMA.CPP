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
*     Help classes to handle Service Centres and settings.
*
*/



// INCLUDES
#include <eikclb.h>                     // CEikColumnListBox
#include <AknPhoneNumberGrouping.h>     // CAknPhoneNumberGrouping
#include <StringLoader.h>               // StringLoader
#include <SMUM.rsg>                     // resource ids
#include "smsetdlgcdma.h"                   // dialogs
#include "SmumSettingsDialogCDMA.h"

//  LOCAL CONSTANTS AND MACROS
const TInt KSmumNumberOfSeparatingColumnCharacters = 2;
const TInt KSmumNumberOfEndColumnCharacters = 1;
const TInt KMaxLengthOfNameAndNumberString = 100;
const TUint KSpaceChar = ' ';
_LIT(KSmumSpaceString, " ");

//  MEMBER FUNCTIONS


/**
*  CSmumSettingsArray - class
*/

// ---------------------------------------------------------
// CSmumSettingsArray::NewL
//
// ---------------------------------------------------------
CSmumSettingsArray* CSmumSettingsArray::NewL( TInt aResourceId )
    {
    CSmumSettingsArray* self = new ( ELeave ) CSmumSettingsArray();
    CleanupStack::PushL( self );
    self->ConstructL( aResourceId ); 
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------
// CSmumSettingsArray::ConstructL
//
// ---------------------------------------------------------
void CSmumSettingsArray::ConstructL( TInt aResourceId )
    {
    iTypeOfSettingsId = aResourceId;
    CMuiuSettingsArray::ConstructL( aResourceId );
    }

// ---------------------------------------------------------
// CSmumSettingsArray::CSmumSettingsArray
//
// ---------------------------------------------------------
CSmumSettingsArray::CSmumSettingsArray() :
    iCallBackNumberString( NULL )
    {
    }

// ---------------------------------------------------------
// CSmumSettingsArray::~CSmumSettingsArray
//
// ---------------------------------------------------------
CSmumSettingsArray::~CSmumSettingsArray()
    {
    delete iCallBackNumberString;
    }

// ---------------------------------------------------------
// CSmumSettingsArray::MdcaPoint
//
// ---------------------------------------------------------
TPtrC CSmumSettingsArray::MdcaPoint( TInt aIndex ) const
    {
    TPtr tempText = iTempText->Des();
    TInt typeOfSettings = 0;
    if (R_SMS_SENDING_OPTIONS_ITEMS_CDMA == iTypeOfSettingsId || R_SMS_SENDING_OPTIONS_ITEMS_CDMA_NO_DELIVERY == iTypeOfSettingsId)
        {
        typeOfSettings = ESmumSendOptCallbackNumberLBICDMA;
        }
    else if (R_SMS_MAIN_SETTINGS_ITEMS_CDMA == iTypeOfSettingsId || R_SMS_MAIN_SETTINGS_ITEMS_CDMA_NO_DELIVERY == iTypeOfSettingsId)
        {
        typeOfSettings = ESmumCallbackNumberLBICDMA;
        }
    if ( typeOfSettings == aIndex )
        {
        tempText.Zero();
        tempText.Append( KSmumSpaceString );
        tempText.Append( KColumnListSeparator );
        tempText.Append( At( aIndex ).iLabelText );
        tempText.Append( KColumnListSeparator );
        tempText.Append( KColumnListSeparator );
        if (iCallBackNumberString)
            {
            HBufC* callbackNumber = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( *iCallBackNumberString );
            CleanupStack::PushL( callbackNumber );

            tempText.Append( *callbackNumber );

            CleanupStack::PopAndDestroy( callbackNumber );
            }
        return tempText;
        }
    else
        {
        return CMuiuSettingsArray::MdcaPoint( aIndex );
        }
    }

// ---------------------------------------------------------
// CSmumSettingsArray::SetCallBackNumberL
//
// ---------------------------------------------------------
void CSmumSettingsArray::SetCallBackNumberL( const TDesC& aCallBackNumber)
    {
    if (iCallBackNumberString)
        {
        delete iCallBackNumberString;
        iCallBackNumberString = NULL;
        }

    iCallBackNumberString = HBufC::NewL( aCallBackNumber.Length());
    iCallBackNumberString->Des().Copy( aCallBackNumber);
    }

// ---------------------------------------------------------
// CSmumSettingsArray::CallBackNumber
//
// ---------------------------------------------------------
TPtrC CSmumSettingsArray::CallBackNumber() const
    {
    if (iCallBackNumberString)
        {
        return iCallBackNumberString->Des();
        }
    return TPtrC (KNullDesC);
    }

//  End of File
