/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides UniEditor Mms Plugin methods.
*
*/




// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <implementationproxy.h>
#include <ecom.h>

#include <msvapi.h>
#include <mtclreg.h>
#include <cmsvrecipientlist.h>

#include <commdb.h>
#include <mmsapselect.h>
 
#include <mmsconst.h>
#include <mmsclient.h>
#include <mmsheaders.h>
#include <mmssettings.h>
#include <mmsmsventry.h>
#include <AknQueryDialog.h>
#include <UniMmsPluginD.rsg>
#include <bautils.h>
#include <data_caging_path_literals.hrh> 
// Possible values for mms validity period in seconds
const TInt32 KUniMmsValidityPeriod1h = 3600;
const TInt32 KUniMmsValidityPeriod6h = 21600;
const TInt32 KUniMmsValidityPeriod24h = 86400;
const TInt32 KUniMmsValidityPeriod3Days = 259200;
const TInt32 KUniMmsValidityPeriodWeek = 604800;
const TInt32 KUniMmsValidityPeriodMax = 0;

#include <SenduiMtmUids.h>

#include <MuiuOperationWait.h>

#include "UniMsvEntry.h"
#include "UniMmsPlugin.h"
#include "UniSendingSettings.h"
#include "UniClientMtm.h"
#include "UniEditorUids.hrh"

//DEBUG LOGGING
#include "UniEditorLogging.h"

// CONSTANTS   
const TImplementationProxy KImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY( KUidUniEditorMmsPlugin, CUniMmsPlugin::NewL)
	};
	
const TInt  KMaxDetailsLength = 64;   // Copy max this many chars to TMsvEntry::iDetails
_LIT( KAddressSeparator, ";" );

_LIT( KUniMmsPluginResourceFile,     	"UniMmsPluginD.rsc" );

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUniMmsPlugin* CUniMmsPlugin::NewL( TAny* aConstructionParameters )
    {
    UNILOGGER_ENTERFN("CUniMmsPlugin::NewL");
	TUniPluginParams* params = reinterpret_cast<TUniPluginParams*>( aConstructionParameters );
    CUniMmsPlugin* self = new ( ELeave ) CUniMmsPlugin( params->iSession, params->iUniMtm ); 
    UNILOGGER_LEAVEFN("CUniSmsPlugin::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CUniMmsPlugin::~CUniMmsPlugin()
    {
    //In case there is no settings at all leave occurs and resource not freed
    if( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
        }
    delete iCommsDb;
    delete iMmsSettings;
    delete iMmsHeader;
    delete iMmsMtm;
    delete iMtmRegistry;
    }

// -----------------------------------------------------------------------------
// C++ default constructor
// -----------------------------------------------------------------------------
//
CUniMmsPlugin::CUniMmsPlugin( CMsvSession& aSession, CUniClientMtm& aUniMtm )
    : iSession( aSession ),
      iUniMtm( aUniMtm ),
      iResourceOffset ( 0 )
    {
    }

// -----------------------------------------------------------------------------
// LoadHeadersL
// -----------------------------------------------------------------------------
//
void CUniMmsPlugin::LoadHeadersL( CMsvStore* aStore )
    {
    UNILOGGER_ENTERFN("CUniMmsPlugin::LoadHeadersL");  
    delete iMmsHeader;
    iMmsHeader = NULL;
	iMmsHeader = CMmsHeaders::NewL();
	if ( aStore && aStore->IsPresentL( KUidMmsHeaderStream ) )
	    {
    	iMmsHeader->RestoreL( *aStore );
	    }
    else
        {
        CMmsSettings* settings = CMmsSettings::NewL();
        CleanupStack::PushL( settings );
        settings->LoadSettingsL();
        iMmsHeader->SetSettings( settings );
        CleanupStack::PopAndDestroy( settings );
        }
    UNILOGGER_LEAVEFN("CUniSmsPlugin::LoadHeadersL"); 
    }

// -----------------------------------------------------------------------------
// SaveHeadersL
// -----------------------------------------------------------------------------
//
void CUniMmsPlugin::SaveHeadersL( CMsvStore& aStore )
    {
    if ( iMmsHeader )
        {
        iMmsHeader->StoreL( aStore );
        }
    }

// -----------------------------------------------------------------------------
// ConvertFromL
// -----------------------------------------------------------------------------
//
TMsvId CUniMmsPlugin::ConvertFromL( TMsvId aId )
    {
    UNILOGGER_ENTERFN("CUniMmsPlugin::ConvertFromL");
    MmsMtmL()->SwitchCurrentEntryL( aId );
    MmsMtmL()->LoadMessageL();
    iUniMtm.SwitchCurrentEntryL( aId );
    iUniMtm.LoadMessageL();

    // Copy MMS stuff.
    const CMsvRecipientList& mmsRecipients = MmsMtmL()->AddresseeList();
    while ( mmsRecipients.Count() )
        {
        iUniMtm.AddAddresseeL( 
            mmsRecipients.Type( 0 ),
            TMmsGenUtils::PureAddress( mmsRecipients[ 0 ] ),
            TMmsGenUtils::Alias( mmsRecipients[ 0 ] ) );
        MmsMtmL()->RemoveAddressee( 0 );
        }
    
    iUniMtm.SetSubjectL( MmsMtmL()->SubjectL() );
    MmsMtmL()->SetSubjectL( KNullDesC );
    
    
    //Get MMS Client MTM Message root and set it to UniMTM
    iUniMtm.SetMessageRoot( MmsMtmL()->MessageRootAttachment() );
    
    MmsMtmL()->SaveMessageL();
    iUniMtm.SaveMessageL();
    
    TMmsMsvEntry mmsEntry = static_cast<TMmsMsvEntry>( MmsMtmL( )->Entry( ).Entry( ) );
    
    TMsvEntry uniEntry = iUniMtm.Entry().Entry();
    uniEntry.iMtm.iUid = KUidUniMtm;

	// Lets convert the bits to Uni mode
	TUniMsvEntry::SetForwardedMessage( uniEntry, mmsEntry.IsForwardedMessage( ));
	TUniMsvEntry::SetMmsUpload( uniEntry, mmsEntry.iBioType == KUidMsgSubTypeMmsUpload.iUid );
	TUniMsvEntry::SetEditorOriented( uniEntry, mmsEntry.EditorOriented( ) );

    iUniMtm.Entry().ChangeL( uniEntry );
    UNILOGGER_LEAVEFN("CUniSmsPlugin::ConvertFromL");    
    return aId;
    }

// -----------------------------------------------------------------------------
// ConvertToL
// -----------------------------------------------------------------------------
//
TMsvId CUniMmsPlugin::ConvertToL( TMsvId aId )
    {
    UNILOGGER_ENTERFN("CUniMmsPlugin::ConvertToL");
    MmsMtmL()->SwitchCurrentEntryL( aId );
    MmsMtmL()->LoadMessageL();
    iUniMtm.SwitchCurrentEntryL( aId );
    iUniMtm.LoadMessageL();

    // Copy MMS stuff.
    const CMsvRecipientList& uniRecipients = iUniMtm.AddresseeList();
    while ( uniRecipients.Count() )
        {
        MmsMtmL()->AddAddresseeL( 
            uniRecipients.Type( 0 ),
            TMmsGenUtils::PureAddress( uniRecipients[ 0 ] ),
            TMmsGenUtils::Alias( uniRecipients[ 0 ] ) );
        iUniMtm.RemoveAddressee( 0 );
        }
    
    MmsMtmL()->SetSubjectL( iUniMtm.SubjectL() );
    iUniMtm.SetSubjectL( KNullDesC );
    
    //Let's set the message root to point to smil
    MmsMtmL()->SetMessageRootL ( iUniMtm.MessageRoot() );
    
    iUniMtm.SaveMessageL();
    MmsMtmL()->SaveMessageL();
    
    TMsvEntry uniEntry = iUniMtm.Entry( ).Entry( );
    TMsvEntry tEntry = MmsMtmL()->Entry().Entry();

    TBuf<KMaxDetailsLength> detailsBuf;
    MakeDetailsL( detailsBuf );
    tEntry.iDetails.Set( detailsBuf );
    
    tEntry.iMtm.iUid = KSenduiMtmMmsUidValue;
    
	// Lets convert the bits to mms mode
    TMmsMsvEntry mmsEntry = static_cast<TMmsMsvEntry>( tEntry );
    mmsEntry.SetForwardedMessage( TUniMsvEntry::IsForwardedMessage( uniEntry ) );
    mmsEntry.SetEditorOriented( TUniMsvEntry::IsEditorOriented( uniEntry ) );
    if( TUniMsvEntry::IsMmsUpload( uniEntry ) )
    	{
	    mmsEntry.iBioType = KUidMsgSubTypeMmsUpload.iUid; 
    	}
    
    MmsMtmL()->Entry().ChangeL( mmsEntry );
    UNILOGGER_LEAVEFN("CUniSmsPlugin::ConvertToL");    
    return aId;
    }

// -----------------------------------------------------------------------------
// CreateReplyL
// -----------------------------------------------------------------------------
//
TMsvId CUniMmsPlugin::CreateReplyL( TMsvId aSrc, TMsvId aDest, TMsvPartList aParts )
    {
    return DoCreateReplyOrForwardL( ETrue, aSrc, aDest, aParts );
    }

// -----------------------------------------------------------------------------
// CreateForwardL
// -----------------------------------------------------------------------------
//
TMsvId CUniMmsPlugin::CreateForwardL( TMsvId aSrc, TMsvId aDest, TMsvPartList aParts )
    {
    return DoCreateReplyOrForwardL( EFalse, aSrc, aDest, aParts );
    }
   
// -----------------------------------------------------------------------------
// SendL
// -----------------------------------------------------------------------------
//
void CUniMmsPlugin::SendL( TMsvId aId )
    {
    MmsMtmL()->SwitchCurrentEntryL( aId );
    MmsMtmL()->LoadMessageL();
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    CMsvOperation* oper = MmsMtmL()->SendL( wait->iStatus );
    CleanupStack::PushL( oper );
    wait->Start();
    CleanupStack::PopAndDestroy( oper );
    CleanupStack::PopAndDestroy( wait );
    }

// -----------------------------------------------------------------------------
// ValidateServiceL
// -----------------------------------------------------------------------------
//
TBool CUniMmsPlugin::ValidateServiceL( TBool /*aEmailOverSms*/ )
    {
    TMsvId service = MmsMtmL()->DefaultServiceL();
    TBool valid( MmsMtmL()->ValidateService( service ) == KErrNone );
    if ( !valid )
        {
        iFileParse.Set( KUniMmsPluginResourceFile, &KDC_RESOURCE_FILES_DIR, NULL );
   
        iFileName = iFileParse.FullName();
    
        CEikonEnv* env = CEikonEnv::Static( );
     
        BaflUtils::NearestLanguageFile( env->FsSession(), iFileName );
     
        iResourceOffset = env->AddResourceFileL( iFileName );
        //First, a confirmation query must be shown  
        CAknQueryDialog* settingsNotOkQuery = CAknQueryDialog::NewL();
        if ( settingsNotOkQuery->ExecuteLD( R_UNIEDITOR_QUERY_AP_ERROR ) )
            {
            EditAccessPointL();
            valid = ( MmsMtmL()->ValidateService( service ) == KErrNone );	
            }
       env->DeleteResourceFile( iResourceOffset );    
       iResourceOffset = 0;    
       }
    return valid;
    }


// ---------------------------------------------------------
// EditAccessPointL
// ---------------------------------------------------------
//
void CUniMmsPlugin::EditAccessPointL()
    {
    if ( !iMmsSettings )
        {
        iMmsSettings = CMmsSettings::NewL();
        }
    MmsMtmL()->RestoreSettingsL();
    iMmsSettings->CopyL( MmsMtmL()->MmsSettings() );

    if ( !iCommsDb )
        {
        iCommsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
        }

    TUint32 currentAp = iMmsSettings->AccessPointCount()
        ? iMmsSettings->AccessPoint( 0 )
        : KMaxTUint32; // Invalid value
    TUint32 selectedAp( KMaxTUint32 );
    
    // Ignore return value...
    MmsApSelect::SelectMmsAccessPointL(
        *iCommsDb,
        currentAp,
        selectedAp,
        EApBearerTypeGPRS );

    if ( selectedAp != KMaxTUint32 &&
        selectedAp != currentAp )
        {
        // Ap was changed.
        while ( iMmsSettings->AccessPointCount() )
            {
            iMmsSettings->DeleteAccessPointL( 0 );
            }
        if ( selectedAp != 0 )
            {
            // Something else than "None" selected.
            iMmsSettings->AddAccessPointL( selectedAp, 0 );
            }
        MmsMtmL()->SetSettingsL( *iMmsSettings );
        MmsMtmL()->StoreSettingsL();
        }
    }

// -----------------------------------------------------------------------------
// GetSendingSettingsL
// -----------------------------------------------------------------------------
//
void CUniMmsPlugin::GetSendingSettingsL( TUniSendingSettings& aSettings )
    {
    // Modify only the settings this mtm plugin supports
    switch ( iMmsHeader->MessagePriority() )
        {
        case EMmsPriorityLow:
            aSettings.iPriority = TUniSendingSettings::EUniPriorityLow;
            break;
        case EMmsPriorityHigh:
            aSettings.iPriority = TUniSendingSettings::EUniPriorityHigh;
            break;
        case EMmsPriorityNormal:
        default:
            aSettings.iPriority = TUniSendingSettings::EUniPriorityNormal;
            break;
        }

    aSettings.iDeliveryReport = iMmsHeader->DeliveryReport();

    switch ( iMmsHeader->ExpiryInterval() )
        {
        case KUniMmsValidityPeriod1h:
            aSettings.iValidityPeriod = TUniSendingSettings::EUniValidityPeriod1h;
            break;
        case KUniMmsValidityPeriod6h:
            aSettings.iValidityPeriod = TUniSendingSettings::EUniValidityPeriod6h;
            break;
        case KUniMmsValidityPeriod3Days:
            aSettings.iValidityPeriod = TUniSendingSettings::EUniValidityPeriod3Days;
            break;
        case KUniMmsValidityPeriodWeek:
            aSettings.iValidityPeriod = TUniSendingSettings::EUniValidityPeriodWeek;
            break;
        case KUniMmsValidityPeriodMax:
            aSettings.iValidityPeriod = TUniSendingSettings::EUniValidityPeriodMax;
            break;
        default: // default to 24h
        case KUniMmsValidityPeriod24h:
            aSettings.iValidityPeriod = TUniSendingSettings::EUniValidityPeriod24h;
            break;
        }
    }
    
// -----------------------------------------------------------------------------
// SetSendingSettingsL
// -----------------------------------------------------------------------------
//
void CUniMmsPlugin::SetSendingSettingsL( TUniSendingSettings& aSettings )
    {
    switch ( aSettings.iPriority )
        {
        case TUniSendingSettings::EUniPriorityLow:
            iMmsHeader->SetMessagePriority( EMmsPriorityLow );
            break;
        case TUniSendingSettings::EUniPriorityHigh:
            iMmsHeader->SetMessagePriority( EMmsPriorityHigh );
            break;
        case TUniSendingSettings::EUniPriorityNormal:
        default:
            iMmsHeader->SetMessagePriority( EMmsPriorityNormal );
            break;
        }
    }

// -----------------------------------------------------------------------------
// IsServiceValidL
// -----------------------------------------------------------------------------
//
TBool CUniMmsPlugin::IsServiceValidL()
    {
    return MmsMtmL()->ValidateService( MmsMtmL()->DefaultServiceL() ) == KErrNone;
    }

// -----------------------------------------------------------------------------
// MmsMtmL
// -----------------------------------------------------------------------------
//
CMmsClientMtm* CUniMmsPlugin::MmsMtmL()
    {
    if ( !iMmsMtm )
        {
        if ( !iMtmRegistry )
            {            
            iMtmRegistry = CClientMtmRegistry::NewL( iSession );
            }
        iMmsMtm = static_cast<CMmsClientMtm*>( iMtmRegistry->NewMtmL( KSenduiMtmMmsUid ) );
        }
    return iMmsMtm;
    }

// -----------------------------------------------------------------------------
// DoCreateReplyOrForwardL
// -----------------------------------------------------------------------------
//
TMsvId CUniMmsPlugin::DoCreateReplyOrForwardL( TBool aReply, 
                                               TMsvId aSrc, 
                                               TMsvId aDest, 
                                               TMsvPartList aParts )
    {
    MmsMtmL()->SwitchCurrentEntryL( aSrc );
    
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    
    CMsvOperation* oper = aReply
        ? MmsMtmL()->ReplyL( aDest, aParts, wait->iStatus )
        : MmsMtmL()->ForwardL( aDest, aParts, wait->iStatus );
        
    CleanupStack::PushL( oper );
    wait->Start();

    TMsvId newId;
    TPckgC<TMsvId> paramPack( newId );
    const TDesC8& progress = oper->FinalProgress();
    paramPack.Set( progress );
    newId = paramPack();
    CleanupStack::PopAndDestroy( oper );
    CleanupStack::PopAndDestroy( wait );
    return ConvertFromL( newId );   
    }

// ---------------------------------------------------------
// MakeDetails
// ---------------------------------------------------------
//
void CUniMmsPlugin::MakeDetailsL( TDes& aDetails )
    {
    // This very same code can be found in CUniAddressHandler. 
    // They should be put in common location some day...
    const CMsvRecipientList& addresses = MmsMtmL()->AddresseeList();
    TInt addrCnt = addresses.Count();

    TPtrC stringToAdd;
    for ( TInt i = 0; i < addrCnt; i++)
        {
        // Only address is converted to western. 
        // There may numbers in contact name - they must not be converted 
        TPtrC alias = TMmsGenUtils::Alias( addresses[i] );
        HBufC* addressBuf = NULL;
        
        if ( alias.Length() != 0 )
            {
            stringToAdd.Set( alias );
            }
        else
            {
            TPtrC address = TMmsGenUtils::PureAddress( addresses[i] );
            addressBuf = HBufC::NewLC( address.Length() );
            TPtr addressPtr = addressBuf->Des();
            addressPtr.Copy( address );
            stringToAdd.Set( addressPtr );

            // Internal data structures always holds the address data in western format.
            // UI is responsible of doing language specific conversions.    
            AknTextUtils::ConvertDigitsTo( addressPtr, EDigitTypeWestern );      
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
            if ( addressBuf )
                {
                CleanupStack::PopAndDestroy( addressBuf );
                }
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
            if ( addressBuf )
                {
                CleanupStack::PopAndDestroy( addressBuf );
                }
            break;
            }
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ImplementationProxy
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( KImplementationTable ) / sizeof( TImplementationProxy );
	return KImplementationTable;
	}

//------------------------------------------------------------------------------
// CUniMmsPlugin::SetEncodingSetings
// Turkish SMS-PREQ2265 Specific
// To Set encoding settings like encoding type, character support
// and alternative encoding if any
//------------------------------------------------------------------------------
void CUniMmsPlugin::SetEncodingSettings(TBool aUnicodeMode, TSmsEncoding aAlternativeEncodingType, TInt aCharSupportType)
    {
    //Do nothing for MMS.
    }

//------------------------------------------------------------------------------
// CUniMmsPlugin::GetNumPDUs
// Turkish SMS-PREQ2265 Specific
// To get PDU Info: extracts details of number of PDUs, number of remaining chars in last PDU
// and encoding types used.
//------------------------------------------------------------------------------
void CUniMmsPlugin::GetNumPDUsL (
        TDesC& aBuf,
        TInt& aNumOfRemainingChars,
        TInt& aNumOfPDUs,
        TBool& aUnicodeMode, 
        TSmsEncoding& aAlternativeEncodingType)
    {
    //Do nothing for MMS
    }

//  End of File
