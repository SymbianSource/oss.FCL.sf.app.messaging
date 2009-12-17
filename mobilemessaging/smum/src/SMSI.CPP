/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SMSI.CPP*
*/




// Standard includes
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mtmuidsdef.hrh>
#endif
#include <mtmdef.h>     // MTM specific definitions for constants
#include <eikon.rsg>    // Standard resources
// Messaging includes
#include <msvuids.h>    // Msv UIDs
// Specific includes
#include <MtmExtendedCapabilities.hrh>
#include <SMUM.rsg>                     // for resource IDs
#include <muiu.mbg>                     // bitmaps
#include <avkon.rsg>                    // for resource IDs
#include <StringLoader.h>               // stringloader
#include "SMSI.H"                       
#include "SMSU.HRH"                     // for KMtmUiFunctionSimDialog
#include "smsui.pan"                    // for panics

#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknsConstants.h>//skinned icon ids
#include <data_caging_path_literals.hrh>

#include <csmsaccount.h>
#include <centralrepository.h>
#include <messaginginternalcrkeys.h>
#include <smutset.h>                    // CSmsSettings


//For logging
#include "SmumLogging.h"

//  LOCAL CONSTANTS AND MACROS
const TInt KSmsViewerUid = 0x100058bd;
const TInt KSmsiNumberOfZoomStates = 2; // Nothing to do with zoomstates, picture + mask = 2
//const TUid KUidMsgTypeSMS = {0x1000102C};// already defined in <smut.h>
const TInt KTBuf80Length=80;
// Impossible for the next two figures to be accurate - sizes of messages
// etc. depend greatly on the encoding/compression used on the message
// Approximated to be 30 submessages long default 7 bit encoded sms.
// Note : Rarely or newer used
const TInt KSmsiMaxTotalMsgSize = 153 * 30;  

_LIT(KSmsiResourceFile, "SMUM");
// Correct path is added to literal when it is used.
_LIT(KBitmapFile, "muiu.MBM");

//  MEMBER FUNCTIONS

// Factory constructor function
CSmsMtmUiData* CSmsMtmUiData::NewL( CRegisteredMtmDll& aRegisteredDll )
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUiData::NewL");
    CSmsMtmUiData* smsmtmuidata=new ( ELeave ) CSmsMtmUiData( aRegisteredDll );
    CleanupStack::PushL( smsmtmuidata );
    smsmtmuidata->ConstructL();
    CleanupStack::Pop();
    SMUMLOGGER_LEAVEFN("CSmsMtmUiData::NewL");
    return smsmtmuidata;
    }

// C++ constructor can NOT contain any code, that
// might leave.
CSmsMtmUiData::CSmsMtmUiData( CRegisteredMtmDll& aRegisteredDll ):
    CBaseMtmUiData( aRegisteredDll )
    {
    }

// Symbian OS default constructor can leave.
void CSmsMtmUiData::ConstructL()
    {
    CBaseMtmUiData::ConstructL();
    //Initalize SMS Settings for the first time
    TInt timeStamp = 0;
    CSmsSettings* serviceSettings = CSmsSettings::NewL();
    CleanupStack::PushL( serviceSettings );
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CRepository* cenRepSession = CRepository::NewLC( KCRUidSmum );

    if ( cenRepSession->Get( KSmumShowSMSCTimeStamp, timeStamp ) != KErrNone )
        {
        //In case of any error,set the default value to 0, so that device timestamp is used
        timeStamp = 0;
        SMUMLOGGER_WRITE("CSmsMtmUiData::ConstructL: cenRep get error");
        }
    SMUMLOGGER_WRITE_FORMAT("CSmsMtmUiData::ConstructL: cenRep timesatmp value:%d", timeStamp);
    smsAccount->LoadSettingsL( *serviceSettings );
    serviceSettings->SetUseServiceCenterTimeStampForDate(timeStamp);

    TRAPD( err, smsAccount->SaveSettingsL( *serviceSettings ) );
    if( err != KErrNone)
        {
        SMUMLOGGER_WRITE_FORMAT("CSmsMtmUiData::ConstructL: Settings saving error status: %d", err);
        }
    CleanupStack::PopAndDestroy( 3, serviceSettings ); // serviceSettings, smsAccount, cenRepSession
    }

// Destructor
CSmsMtmUiData::~CSmsMtmUiData()
    {
    }

// ---------------------------------------------------------
// CSmsMtmUiData::ContextIcon
//
// ---------------------------------------------------------
const CBaseMtmUiData::CBitmapArray& CSmsMtmUiData::ContextIcon(
    const TMsvEntry& aContext, 
    TInt /*aStateFlags*/ ) const
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUiData::ContextIcon");
    __ASSERT_DEBUG(aContext.iMtm==KUidMsgTypeSMS, Panic(ESmsiWrongMtm));
    __ASSERT_DEBUG(aContext.iType.iUid!=KUidMsvFolderEntryValue, Panic(ESmsiFoldersNotSupported));
    __ASSERT_DEBUG(aContext.iType.iUid!=KUidMsvAttachmentEntryValue, Panic(ESmsiNoIconForAttachment));

    //    Return the set of icons appropriate to the current context
    TInt bmpIndex; 
    if (aContext.iBioType!=0)
        {
        bmpIndex = aContext.Unread() ? 
            EMbmMuiuQgn_prop_mce_smart_unread : EMbmMuiuQgn_prop_mce_smart_read;
        }
    else
        {
        bmpIndex = aContext.Unread() ? 
            EMbmMuiuQgn_prop_mce_sms_unread : EMbmMuiuQgn_prop_mce_sms_read;
        }

    SMUMLOGGER_LEAVEFN("CSmsMtmUiData::ContextIcon");

    return *iIconArrays->At(( bmpIndex - EMbmMuiuQgn_prop_mce_sms_read ) / KSmsiNumberOfZoomStates );
    }

// ---------------------------------------------------------
// CSmsMtmUiData::OperationSupportedL
//
// ---------------------------------------------------------
TInt CSmsMtmUiData::OperationSupportedL( TInt aOperationId, const TMsvEntry& aContext ) const
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUiData::OperationSupportedL");
    //    SMS mtm client only has two functions which may be of use to the user
    //    NB as this implies, there are other functions.
    switch (aOperationId)
        {
        case KMtmUiFunctionSimDialog:
            return NULL;
        case KMtmUiFunctionMessageInfo:
            if (aContext.iType==KUidMsvMessageEntry)
                {
                return NULL;
                }
            break;
        default:
            break;
        }

    SMUMLOGGER_LEAVEFN("CSmsMtmUiData::OperationSupportedL");
    return R_EIK_TBUF_NOT_AVAILABLE;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::QueryCapability
//
// ---------------------------------------------------------
TInt CSmsMtmUiData::QueryCapability( TUid aFunctionId, TInt& aResponse ) const
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUiData::QueryCapability");
    TInt retVal = KErrNone;
    switch ( aFunctionId.iUid )
        {
        case KUidMsvMtmUiQueryMessagingInitialisation:
            aResponse = ETrue;
            break;

        case KUidMtmQuerySupportsBioMsgValue:   // Supports sending of Bio messages
            aResponse = ETrue;
            break;

        case KUidMtmQueryMaxBodySizeValue:
            aResponse = KSmsiMaxTotalMsgSize;        //    See notes at the definition
            break;

        case KUidMtmQueryMaxTotalMsgSizeValue:
            aResponse = KSmsiMaxTotalMsgSize;        //    See notes at the definition
            break;
        case KUidMtmQuerySupportedBodyValue:
            aResponse = KMtm7BitBody | KMtm8BitBody | KMtm16BitBody;
            break;

        case KUidMsvMtmQueryViewerUidValue:
            aResponse = KSmsViewerUid;
            break;
            
        case KMtmUiFunctionMessageInfo: 
            aResponse = KErrNone;
            break;

        case KUidMtmQueryCanSendMsgValue:        
        case KUidMtmQueryCanReceiveMsgValue:
            break;

        default:
            retVal=KErrNotSupported;
            break;
        }
    SMUMLOGGER_LEAVEFN("CSmsMtmUiData::QueryCapability");
    return retVal;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::PopulateArraysL
//
// ---------------------------------------------------------
void CSmsMtmUiData::PopulateArraysL()
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUiData::PopulateArraysL");
    // Read additional functions
    ReadFunctionsFromResourceFileL( R_SMSI_FUNCTION_ARRAY );
    CreateSkinnedBitmapsL( KSmsiNumberOfZoomStates );
    SMUMLOGGER_LEAVEFN("CSmsMtmUiData::PopulateArraysL");
    }

// ---------------------------------------------------------
// CSmsMtmUiData::GetResourceFileName
//
// ---------------------------------------------------------
void CSmsMtmUiData::GetResourceFileName( TFileName& aFileName ) const
    {
    aFileName = KSmsiResourceFile;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::CanCreateEntryL
//
// ---------------------------------------------------------
TBool CSmsMtmUiData::CanCreateEntryL(
    const TMsvEntry& aParent, 
    TMsvEntry& aNewEntry, 
    TInt& aReasonResourceId ) const
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUiData::CanCreateEntryL");
    __ASSERT_DEBUG( aNewEntry.iMtm == KUidMsgTypeSMS, Panic( ESmsiWrongMtm ));

    //    We can create an entry if it is a message under the local service
    if( aNewEntry.iType == KUidMsvMessageEntry && 
        aNewEntry.iMtm == KUidMsgTypeSMS && 
        aParent.iType == KUidMsvFolderEntry )
        {
        aReasonResourceId = NULL;
        return ETrue;
        }

    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    SMUMLOGGER_LEAVEFN("CSmsMtmUiData::CanCreateEntryL");
    return EFalse;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::CanDeleteServiceL
//
// ---------------------------------------------------------
TBool CSmsMtmUiData::CanDeleteServiceL(
    const TMsvEntry& /*aService*/, 
    TInt& aReasonResourceId ) const
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUiData::CanDeleteServiceL");
    // these CanXXX functions not used in Series 60, except 
    // CanCreateEntryL, CanReplyToEntryL, CanForwardEntryL and 
    // CanUnDeleteFromEntryL 
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    SMUMLOGGER_LEAVEFN("CSmsMtmUiData::CanDeleteServiceL");
    return EFalse;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::CanReplyToEntryL
//
// ---------------------------------------------------------
TBool CSmsMtmUiData::CanReplyToEntryL(
    const TMsvEntry& aContext,
    TInt& aReasonResourceId ) const
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUiData::CanReplyToEntryL");
    __ASSERT_DEBUG( aContext.iMtm == KUidMsgTypeSMS, Panic( ESmsiWrongMtm ));
    if ( aContext.iMtm == KUidMsgTypeSMS
        && aContext.iType == KUidMsvMessageEntry
        && aContext.ReadOnly()
        && !aContext.iBioType )
        {
        return ETrue;
        }
    else
        {
        aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
        return EFalse;
        }
    }


// ---------------------------------------------------------
// CSmsMtmUiData::CanForwardEntryL
//
// ---------------------------------------------------------
TBool CSmsMtmUiData::CanForwardEntryL(
    const TMsvEntry& aContext,
    TInt& aReasonResourceId ) const
    {
    SMUMLOGGER_ENTERFN("CSmsMtmUiData::CanForwardEntryL");
    __ASSERT_DEBUG( aContext.iMtm == KUidMsgTypeSMS, Panic( ESmsiWrongMtm ));
    if ( aContext.iMtm == KUidMsgTypeSMS
        && aContext.iType == KUidMsvMessageEntry
        && aContext.ReadOnly()
        && !aContext.iBioType )
        {
        return ETrue;
        }
    else
        {
        aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CSmsMtmUiData::CanEditEntryL
//
// ---------------------------------------------------------
TBool CSmsMtmUiData::CanEditEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUiData::CanEditEntryL");
    // these CanXXX functions not used in Series 60, except 
    // CanCreateEntryL, CanReplyToEntryL, CanForwardEntryL and 
    // CanUnDeleteFromEntryL 
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::CanViewEntryL
//
// ---------------------------------------------------------
TBool CSmsMtmUiData::CanViewEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUiData::CanViewEntryL");
    // these CanXXX functions not used in Series 60, except 
    // CanCreateEntryL, CanReplyToEntryL, CanForwardEntryL and 
    // CanUnDeleteFromEntryL 
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::CanOpenEntryL
//
// ---------------------------------------------------------
TBool CSmsMtmUiData::CanOpenEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUiData::CanOpenEntryL");
    // these CanXXX functions not used in Series 60, except 
    // CanCreateEntryL, CanReplyToEntryL, CanForwardEntryL and 
    // CanUnDeleteFromEntryL 
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::CanCloseEntryL
//
// ---------------------------------------------------------
TBool CSmsMtmUiData::CanCloseEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUiData::CanCloseEntryL");
    // these CanXXX functions not used in Series 60, except 
    // CanCreateEntryL, CanReplyToEntryL, CanForwardEntryL and 
    // CanUnDeleteFromEntryL 
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::CanDeleteFromEntryL
//
// ---------------------------------------------------------
TBool CSmsMtmUiData::CanDeleteFromEntryL(
    const TMsvEntry& /*aContext*/, 
    TInt& aReasonResourceId ) const
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUiData::CanDeleteFromEntryL");
    // these CanXXX functions not used in Series 60, except 
    // CanCreateEntryL, CanReplyToEntryL, CanForwardEntryL and 
    // CanUnDeleteFromEntryL 
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::CanCopyMoveToEntryL
//
// ---------------------------------------------------------
TBool CSmsMtmUiData::CanCopyMoveToEntryL(
    const TMsvEntry& /*aContext*/, 
    TInt& aReasonResourceId ) const
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUiData::CanCopyMoveToEntryL");
    // these CanXXX functions not used in Series 60, except 
    // CanCreateEntryL, CanReplyToEntryL, CanForwardEntryL and 
    // CanUnDeleteFromEntryL 
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::CanCopyMoveFromEntryL
//
// ---------------------------------------------------------
TBool CSmsMtmUiData::CanCopyMoveFromEntryL(
    const TMsvEntry& /*aContext*/, 
    TInt& aReasonResourceId ) const
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUiData::CanCopyMoveFromEntryL");
    // these CanXXX functions not used in Series 60, except 
    // CanCreateEntryL, CanReplyToEntryL, CanForwardEntryL and 
    // CanUnDeleteFromEntryL 
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::CanCancelL
//
// ---------------------------------------------------------
TBool CSmsMtmUiData::CanCancelL(
    const TMsvEntry& /*aContext*/, 
    TInt& aReasonResourceId ) const
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUiData::CanCancelL");
    // these CanXXX functions not used in Series 60, except 
    // CanCreateEntryL, CanReplyToEntryL, CanForwardEntryL and 
    // CanUnDeleteFromEntryL 
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::StatusTextL
//
// ---------------------------------------------------------
HBufC* CSmsMtmUiData::StatusTextL( const TMsvEntry& aContext ) const
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUiData::StatusTextL");

    TInt res = 0;
    TUint sendingState = aContext.SendingState();
    switch( sendingState )
        {
        case KMsvSendStateFailed:
            res = R_QTN_MCE_OUTBOX_STATUS_FAILED;
            break;

        case KMsvSendStateUponRequest: // Mapped to be the same
        case KMsvSendStateSuspended:
            res = R_QTN_MCE_OUTBOX_STATUS_SUSPENDED;
            break;
        
        case KMsvSendStateScheduled: // Mapped to be the same
        case KMsvSendStateWaiting:
            res = R_QTN_MCE_OUTBOX_STATUS_WAITING;
            break;
            
        case KMsvSendStateResend:
            res = R_QTN_MCE_OUTBOX_STATUS_RETRY_AT;
            break;
            
        case KMsvSendStateSending:
            res = R_QTN_MCE_OUTBOX_STATUS_SENDING;
            break;
        
        case KMsvSendStateSent:
        case KMsvSendStateNotApplicable:
            res = 0;
            break;
            
            // Unknown 
        case KMsvSendStateUnknown:
        default:
#ifdef _DEBUG
            Panic( ESmsiMtmUdUnknownMsgStatus );
#endif
            User::Leave( KErrNotSupported );
            break;
        }
    
        if (!res)
            {
            return HBufC::NewL(0);
            }
      
        if ( sendingState == KMsvSendStateResend )
            {
            TBuf<KTBuf80Length> bufTimeFormat;
            TBuf<KTBuf80Length> bufTimeFormatted;
            StringLoader::Load( 
                bufTimeFormat, R_QTN_TIME_USUAL_WITH_ZERO, iCoeEnv );
                
            TTime localTime = aContext.iDate;
            TLocale locale;
    	    // Add time difference
            localTime += locale.UniversalTimeOffset();
    	    if (locale.QueryHomeHasDaylightSavingOn())          
    		    { // and possible daylight saving time
    		    TTimeIntervalHours daylightSaving(1);          
    		    localTime += daylightSaving;
    		    }
    		localTime.FormatL( bufTimeFormatted, bufTimeFormat );
    		AknTextUtils::LanguageSpecificNumberConversion( bufTimeFormatted );
    	    SMUMLOGGER_LEAVEFN(" CSmsMtmUiData::StatusTextL - 1");
            return StringLoader::LoadL( 
                R_QTN_MCE_OUTBOX_STATUS_RETRY_AT, bufTimeFormatted, iCoeEnv );
            }
        else
            {
            SMUMLOGGER_LEAVEFN(" CSmsMtmUiData::StatusTextL - 2");
            return StringLoader::LoadL( res, iCoeEnv );
            }
    }

// ---------------------------------------------------------
// CSmsMtmUiData::CreateSkinnedBitmapsL
//
// ---------------------------------------------------------
void CSmsMtmUiData::CreateSkinnedBitmapsL( TInt aNumZoomStates )
    {
    SMUMLOGGER_ENTERFN(" CSmsMtmUiData::CreateSkinnedBitmapsL");

    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    TAknsItemID id;
    CFbsBitmap* bitmap;
    CFbsBitmap* bitmapMask;
    TBool found = ETrue;
    
    for( TInt i=EMbmMuiuQgn_prop_mce_sms_read; i<EMbmMuiuQgn_prop_mce_smart_unread_mask+1; i++ )
        {
        found = ETrue;
        switch( i )
            {
            // Add to iIconArrays in this order
            case EMbmMuiuQgn_prop_mce_sms_read:
                id.Set( KAknsIIDQgnPropMceSmsRead );
                break;
            case EMbmMuiuQgn_prop_mce_sms_unread:
                id.Set( KAknsIIDQgnPropMceSmsUnread );
                break;
            case EMbmMuiuQgn_prop_mce_smart_read:
                id.Set( KAknsIIDQgnPropMceSmartRead );
                break;
            case EMbmMuiuQgn_prop_mce_smart_unread:
                id.Set( KAknsIIDQgnPropMceSmartUnread );
                break;
            default:
                found = EFalse;
                break;
            }
            if( found )
                {
                CArrayPtrFlat<CFbsBitmap>* array = 
                    new (ELeave) CArrayPtrFlat<CFbsBitmap>( aNumZoomStates );
                CleanupStack::PushL( array );
                array->SetReserveL( aNumZoomStates ); // AppendLs will not LEAVE
                TParse tp;
                tp.Set( KBitmapFile, &KDC_APP_BITMAP_DIR, NULL );

                AknsUtils::CreateIconL( skins, id, bitmap,
                    bitmapMask, tp.FullName(), i, i + 1 );
                    
                array->AppendL(bitmap);
                array->AppendL(bitmapMask);
                iIconArrays->AppendL(array);

                CleanupStack::Pop();//array
                }
                
        }
    SMUMLOGGER_LEAVEFN(" CSmsMtmUiData::CreateSkinnedBitmapsL");
    }
//  End of File
