/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Sets resipients, finalize message entry. 
*
*/



#include <mmsclient.h>
#include <MsgAddressControl.h>  
#include <mmsgenutils.h> 		
#include <StringLoader.h>        
#include "audiomessagesaveoperation.h"

_LIT( KAddressSeparator, ";" );
const TInt  KMaxDetailsLength  = 64;   // Copy max this many chars to TMsvEntry iDetails

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// CAudioMessageSaveOperation::NewL
// ---------------------------------------------------------
//
CAudioMessageSaveOperation* CAudioMessageSaveOperation::NewL( 
		CMsgAddressControl& aAddressCtrl,
        MAmsOperationObserver& aObserver,
        CAudioMessageDocument& aDocument,
        CMsgEditorView& aView )
    {
    CAudioMessageSaveOperation* self = 
        new ( ELeave ) CAudioMessageSaveOperation( aAddressCtrl,aObserver, aDocument, aView);
    return self;
    }

// ---------------------------------------------------------
// CAudioMessageSaveOperation::Constructor
// ---------------------------------------------------------
//
CAudioMessageSaveOperation::CAudioMessageSaveOperation(
		CMsgAddressControl& aAddressCtrl,
        MAmsOperationObserver& aObserver,
        CAudioMessageDocument& aDocument,
        CMsgEditorView& aView ) :
    CAudioMessageOperation( aObserver, aDocument ),
    iView( aView ), iAddressCtrl( aAddressCtrl )
    {
    }

// ---------------------------------------------------------
// CAudioMessageSaveOperation::Destructor
// ---------------------------------------------------------
//
CAudioMessageSaveOperation::~CAudioMessageSaveOperation()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CAudioMessageSaveOperation::Save
// ---------------------------------------------------------
//
void CAudioMessageSaveOperation::Save()
    {
    ResetError();
    iSaveState = EAmsSaveCheck;
    CompleteSelf( KErrNone );
    }


// ---------------------------------------------------------
// CAudioMessageSaveOperation::DoSaveStepL
// ---------------------------------------------------------
//
void CAudioMessageSaveOperation::DoSaveStepL()
    {
    switch ( iSaveState )
        {
       	case EAmsSaveCheck:
            DoSaveChecksL();
            break;
        case EAmsSaveFinalize:
            DoFinalizeSaveL();
            break;
        case EAmsSaveEnd:
            iObserver.EditorOperationEvent(
                EAmsOperationSave,
                EAmsOperationComplete );
            break;
        default:
            iObserver.EditorOperationEvent(
                EAmsOperationSave,
                EAmsOperationError );
            break;
        }
    }

// ---------------------------------------------------------
// CAudioMessageSaveOperation::DoSaveChecksL
// ---------------------------------------------------------
//
void CAudioMessageSaveOperation::DoSaveChecksL()
    {
    iSaveState = EAmsSaveFinalize;
   	CMmsClientMtm& mtm = iDocument.Mtm();
   	
   	mtm.SetSubjectL( KNullDesC() );
	for ( TInt i = mtm.AddresseeList().Count(); --i >=  0; )
    	{
    	mtm.RemoveAddressee( i );
    	}
	const CMsgRecipientArray* recipients = iAddressCtrl.GetRecipientsL();

	// inserts addresses from address control to clientMtm
	TInt addrCnt = recipients->Count();
	for ( TInt ii = 0; ii < addrCnt ; ii++ )
    	{
    	CMsgRecipientItem* addrItem = recipients->At(ii);
    	TPtr realAddress = addrItem->Address()->Des();
    	TPtr alias = addrItem->Name()->Des();
    	// Check that neither string contains illegal characters.
    	// If they does strip illegal chars away and save
   		RemoveIllegalAddrChars( realAddress );
    	if ( alias.Length() > 0)
        	{
     		RemoveIllegalAddrChars( alias );
        	}
    	mtm.AddAddresseeL( EMsvRecipientTo, realAddress, alias );
    	}
	mtm.SaveMessageL();
	CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CAudioMessageSaveOperation::RemoveIllegalAddrChars
// ---------------------------------------------------------
//
TBool CAudioMessageSaveOperation::RemoveIllegalAddrChars( TDes& aString )
    {
    TBool ret( EFalse );
    for ( TInt i = 0; i < aString.Length(); i++ )
        {
        if ( !IsValidAddrChar( (TChar) aString[ i ] ) ) 
            {
            aString.Delete( i, 1 );
            i--;
            ret = ETrue;
            }
        }
    return ret;
    }

// ---------------------------------------------------------
// CAudioMessageSaveOperation::IsValidAddrChar
// ---------------------------------------------------------
//
TBool CAudioMessageSaveOperation::IsValidAddrChar( const TChar& aChar )
    {
    return ( aChar != '<' && aChar != '>' );
    }
    
// ---------------------------------------------------------
// CAudioMessageSaveOperation::DoFinalizeSaveL
// ---------------------------------------------------------
//
void CAudioMessageSaveOperation::DoFinalizeSaveL()
    {
    iSaveState = EAmsSaveEnd;
    CMmsClientMtm& mtm = iDocument.Mtm();

    TMmsMsvEntry tEntry = static_cast<TMmsMsvEntry>( iDocument.Entry() );

    // Set details
    TBuf<KMaxDetailsLength> aDetails; 
    const CMsvRecipientList& addresses = mtm.AddresseeList();
    TInt addrCnt = addresses.Count();

    TPtrC stringToAdd;
    for ( TInt i = 0; i < addrCnt; i++)
        {
        TPtrC alias = TMmsGenUtils::Alias( addresses[i] );
        TPtrC address = TMmsGenUtils::PureAddress( addresses[i] );

        if ( alias.Length() != 0 )
            {
            stringToAdd.Set( alias );
            }
        else
            {
            stringToAdd.Set( address );
            }
        
        if ( ( aDetails.Length() != 0 ) &&   // Not a first address
             ( aDetails.Length() + KAddressSeparator().Length() < KMaxDetailsLength ) )
            {
            // Add separator
            aDetails.Append( KAddressSeparator() );
            }

        if ( aDetails.Length() + stringToAdd.Length() < KMaxDetailsLength ) 
            {
            // whole string fits. Add it.
            aDetails.Append( stringToAdd );
            }
        else
            {
            // Only part of the string fits
            TInt charsToAdd = KMaxDetailsLength - aDetails.Length();

            if ( charsToAdd <= 0 )
                {
                // Cannot add any more chars 
                break;
                }

            if ( charsToAdd >= stringToAdd.Length() )
                {
                // Guarantee that charsToAdd is not larger that stringToAdd lenght 
                charsToAdd = stringToAdd.Length();
                }

            aDetails.Append( stringToAdd.Left( charsToAdd ) );
            break;
            }
        }
    AknTextUtils::ConvertDigitsTo( aDetails, EDigitTypeWestern );
    tEntry.iDetails.Set( aDetails );

    if ( iDocument.IsForward() )
        {
        tEntry.SetForwardedMessage( EFalse );
        }

    if ( !tEntry.EditorOriented() )
        {
        tEntry.SetEditorOriented( ETrue );
        tEntry.iMtmData1 &= ~KMmsMessageMobileTerminated;
        tEntry.iMtmData1 |= KMmsMessageEditorOriented;
        }

    // Update timestamp  
    tEntry.iDate.UniversalTime();

    if ( !tEntry.Visible() )
        {
        // Save from close or exit save.
        // Message should be visible after save
        tEntry.SetVisible( ETrue );
        tEntry.SetInPreparation( EFalse );
       	tEntry.iBioType = KUidMsgSubTypeMmsAudioMsg.iUid;
        }

    iDocument.CurrentEntry().ChangeL( tEntry );
    iDocument.SetHeaderModified( EFalse );
    iDocument.SetBodyModified( EFalse );
    CompleteSelf( KErrNone );
    }
    
// ---------------------------------------------------------
// CAudioMessageSaveOperation::DoCancel
// ---------------------------------------------------------
//
void CAudioMessageSaveOperation::DoCancel()
    {
    }

// ---------------------------------------------------------
// CAudioMessageSaveOperation::RunL
// ---------------------------------------------------------
//
void CAudioMessageSaveOperation::RunL()
    {
    if ( iStatus.Int() != KErrNone )
        {
        SetError( iStatus.Int() );
        iObserver.EditorOperationEvent(
            EAmsOperationSave,
            EAmsOperationError );
        }
    else
        {
        DoSaveStepL();
        }
    }
