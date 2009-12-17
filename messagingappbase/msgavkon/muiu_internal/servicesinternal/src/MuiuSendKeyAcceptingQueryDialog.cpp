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
* Description: MuiuSendKeyAcceptingQueryDialog implementation
*
*/




// INCLUDE FILES
#include <StringLoader.h>
#include <muiu_internal.rsg>
#include <AknPhoneNumberGrouping.h>       //CAknPhoneNumberGrouping
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include "MuiuSendKeyAcceptingQueryDialog.h"

// CONSTANTS
const TUint KMuiuLREMarker = 0x202A;  // Start of phone number. Arabic lanquage need this
const TUint KMuiuPDFMarker = 0x202C;  // End of phone number.   Arabic lanquage need this
const TInt KLREandPDFMarkSize = 2;   // Size LRE mark and PDF mark need.
const TInt KStringsArrayGranularity = 2;
_LIT( KDirAndMuiuResFileName,"muiu_internal.rsc" );



// ========== MEMBER FUNCTIONS =============================



// ---------------------------------------------------------
// CMuiuSendKeyAcceptingQueryDialog::NewL
//
// 
// ---------------------------------------------------------
CMuiuSendKeyAcceptingQueryDialog* CMuiuSendKeyAcceptingQueryDialog::NewL( const TDesC& aNumber,
                                                                          const TDesC& aName, 
                                                                          const TTone& aTone )
    {
    CMuiuSendKeyAcceptingQueryDialog* self = 
                                      new ( ELeave ) CMuiuSendKeyAcceptingQueryDialog( aTone );
    CleanupStack::PushL( self );
    self->ConstructL( aNumber, aName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMuiuSendKeyAcceptingQueryDialog::CMuiuSendKeyAcceptingQueryDialog
//
//
// ---------------------------------------------------------
CMuiuSendKeyAcceptingQueryDialog::CMuiuSendKeyAcceptingQueryDialog( const TTone aTone )
    : CAknQueryDialog( aTone ), iResources( *CCoeEnv::Static() )
    {
    }

// ---------------------------------------------------------
// CMuiuSendKeyAcceptingQueryDialog::~CMuiuSendKeyAcceptingQueryDialog
//
//
// ---------------------------------------------------------
CMuiuSendKeyAcceptingQueryDialog::~CMuiuSendKeyAcceptingQueryDialog()
    {
    iResources.Close();
    }

// ---------------------------------------------------------
// CMuiuSendKeyAcceptingQueryDialog::OfferKeyEventL
//
//
// ---------------------------------------------------------
TKeyResponse CMuiuSendKeyAcceptingQueryDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                               TEventCode aType )
    {
    if ( aType == EEventKey && aKeyEvent.iCode == EKeyYes )
        {
        TryExitL( EKeyYes );
        return EKeyWasConsumed;
        }
    else
        {
        return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }
    }

// ---------------------------------------------------------
// CMuiuSendKeyAcceptingQueryDialog::OkToExitL
//
//
// ---------------------------------------------------------
TBool CMuiuSendKeyAcceptingQueryDialog::OkToExitL( TInt aButtonId )
    {
    if ( EKeyYes == aButtonId )
        {
        return ETrue;
        }
    else
        {
        TBool retValue( CAknDialog::OkToExitL( aButtonId ) );
        return retValue;
        }
    }

// ---------------------------------------------------------
// CMuiuSendKeyAcceptingQueryDialog::MappedCommandId
//
//
// ---------------------------------------------------------
TInt CMuiuSendKeyAcceptingQueryDialog::MappedCommandId( TInt aButtonId )
    {
    switch( aButtonId )
        {
        case EKeyYes:
            return EKeyYes;
        default :
            return CEikDialog::MappedCommandId( aButtonId );
        }
    }


// ---------------------------------------------------------
// CMuiuSendKeyAcceptingQueryDialog::ConstructL
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
void CMuiuSendKeyAcceptingQueryDialog::ConstructL( const TDesC& aNumber, const TDesC& aName )
    {
    TParse parse;
    parse.Set( KDirAndMuiuResFileName, &KDC_RESOURCE_FILES_DIR, NULL ); 
    TFileName fileName( parse.FullName() );
    CDesCArrayFlat* strings = new ( ELeave ) CDesCArrayFlat( KStringsArrayGranularity );
    CleanupStack::PushL( strings );
    strings->AppendL( aName );  // %0U
    HBufC* bufNumber = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( aNumber ); 
    CleanupStack::PushL( bufNumber );
    TPtr   bufNumberPtr = bufNumber->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( bufNumberPtr );

    // LRE and PDF mark is needed for successful Arabic/Hebrew number conversion
    HBufC* bufLREandPDF = HBufC::NewLC( bufNumberPtr.Length() + KLREandPDFMarkSize );
    TPtr   bufLREandPDFPtr = bufLREandPDF->Des();
    bufLREandPDFPtr.Append( KMuiuLREMarker );
    bufLREandPDFPtr.Append( bufNumberPtr );
    bufLREandPDFPtr.Append( KMuiuPDFMarker );

    strings->AppendL( bufLREandPDFPtr ); // %1U
    iResources.OpenL( fileName );
    
    HBufC* prompt = NULL;
    if( aName.Length() > 0 )
        {
        // definition of qtn_muiu_quest_call is "Call to:\n%0U\n%1U"
        prompt = StringLoader::LoadLC( R_QTN_MUIU_QUEST_CALL, *strings );
        }
    else
        {
        // definition of qtn_quest_call is "Call to:\n%U"
        prompt = StringLoader::LoadLC( R_QTN_QUEST_CALL, *bufLREandPDF );
        }
    SetPromptL(*prompt);
    CleanupStack::PopAndDestroy( 4, strings ); // strings,bufNumber,bufLREandPDF,prompt,
    }

//  End of File
