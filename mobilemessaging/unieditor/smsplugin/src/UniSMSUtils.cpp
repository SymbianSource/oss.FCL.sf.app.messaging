/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*      Multiple line query dialog
*
*/

 

// INCLUDE FILES
#include <commonphoneparser.h>		// Common phone number validity checker
#include <aknnotewrappers.h>
#include <StringLoader.h>           // StringLoader
#include "UniSmsUtils.h"
#include <data_caging_path_literals.hrh> 
#include <bautils.h>  //BaflUtils::NearestLanguageFile
#include <UniSmsPluginD.rsg>                    // resouce identifiers
#include <layoutmetadata.cdl.h>

_LIT(KUniSmsPluginResourceFile,     	"UniSmsPluginD.rsc");

// ---------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------
CUniSmsEMultilineQueryDialog::CUniSmsEMultilineQueryDialog( TBool aEmailDlg, TTone aTone )
    : CAknMultiLineDataQueryDialog( aTone ), iEmailDlg( aEmailDlg )
    {
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CUniSmsEMultilineQueryDialog::~CUniSmsEMultilineQueryDialog()
    {
    if ( iResourceFile )
        {
        iEikonEnv->DeleteResourceFile( iResourceFile );
        }
    }

// ---------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------
CUniSmsEMultilineQueryDialog* CUniSmsEMultilineQueryDialog::NewL( 
	TDes& aText1, 
	TDes& aText2, 
	TBool aEmailDlg, 
	TTone aTone )
    {
    CUniSmsEMultilineQueryDialog* self 
        = new (ELeave) CUniSmsEMultilineQueryDialog( aEmailDlg, aTone );
    CleanupStack::PushL( self );
                
    self->SetDataL( aText1, aText2 );
                
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CUniSmsEMultilineQueryDialog::SetInitialCurrentLine
// ---------------------------------------------------------
void CUniSmsEMultilineQueryDialog::SetInitialCurrentLine()
    { 
    TRAP_IGNORE( TryToChangeLineL() );
    }

// ---------------------------------------------------------
// CUniSmsEMultilineQueryDialog::TryToChangeLineL
// ---------------------------------------------------------
void CUniSmsEMultilineQueryDialog::TryToChangeLineL()
    {
    ActivateFirstPageL();
    TryChangeFocusToL( iEmailDlg ? EMultilineFirstLine : EMultilineSecondLine );
    
    if ( !Layout_Meta_Data::IsLandscapeOrientation() )
    	{
    	TryChangeFocusToL( iEmailDlg ? EMultilineFirstLine : EMultilineSecondLine );
    	}
    else
        {
        TryChangeFocusToL( EMultilineFirstLine );
        // Show the ok key
        UpdateLeftSoftKeyL();
        } 
    }

// ----------------------------------------------------
// CUniSmsEMultilineQueryDialog::OkToExitL
// ----------------------------------------------------
TBool CUniSmsEMultilineQueryDialog::OkToExitL(
    TInt aButtonId )
    {
    // Fetch the iSecondText from the control
    TInt returnValue = CAknMultiLineDataQueryDialog::OkToExitL( aButtonId );
    TPtr* addrPtr = &SecondData(TPtr(0,0));
    AknTextUtils::ConvertDigitsTo( *addrPtr, EDigitTypeWestern );

	if( returnValue )
		{
	    // Check that the phonenumber is ok
		if ( CommonPhoneParser::IsValidPhoneNumber( 
            *addrPtr, CommonPhoneParser::ESMSNumber ) )
			{
			return ETrue;
			}
		else 
	        {
        	// If not; show note, be sure focus is in correct line and don't exit
	        CAknInformationNote* note = new ( ELeave ) CAknInformationNote(); 
	        HBufC* text = StringLoader::LoadLC( R_QTN_INFO_INVALID_NUMBER, iCoeEnv );
	        note->ExecuteLD( *text );
	        CleanupStack::PopAndDestroy( text );
    	    }
		}
   	else  if ( !Layout_Meta_Data::IsLandscapeOrientation() )
        {
        TRAP_IGNORE( TryChangeFocusToL( EMultilineSecondLine ));	
        }
    else
        {
        // Show the ok key
        TRAP_IGNORE( UpdateLeftSoftKeyL() );
        }
                
    return EFalse;
    }

// ----------------------------------------------------
// CUniSmsEMultilineQueryDialog::ExecuteLD
// ----------------------------------------------------
TInt CUniSmsEMultilineQueryDialog::ExecuteLD( TInt aResourceId )
    {
    TInt ret;
    TParse parse;
    parse.Set( KUniSmsPluginResourceFile, &KDC_RESOURCE_FILES_DIR, NULL ); 
    TFileName fileName( parse.FullName() );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    iResourceFile = iEikonEnv->AddResourceFileL( fileName );
    ret = CAknMultiLineDataQueryDialog::ExecuteLD( aResourceId );
    
    return ret;
    }


//  End of File  

