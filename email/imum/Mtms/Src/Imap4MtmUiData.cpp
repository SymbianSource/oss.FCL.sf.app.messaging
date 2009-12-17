/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       CImap4MtmUiData - IMAP4 MTM Ui data layer base API
*
*/


#include <coemain.h>
#include <mtmuidef.hrh>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mtmdef.hrh>
#else
#include <mtmdef.hrh>
#include <mtmuidsdef.hrh>
#endif
#include <mtclbase.h>
#include <miutset.h>
#include "impicmds.h"
#include <imapcmds.h>
#include <impcmtm.h>
#include <miuthdr.h>
#include <eikon.rsg>
#include "ImumPanic.h"
#include <imum.rsg>
#include <muiu.mbg>
#include "Imap4MtmUiData.h"
#include <mtud.hrh>
#include <MtmExtendedCapabilities.hrh>
#include "EmailEditorViewerUids.h"
#include <data_caging_path_literals.hrh>
#include <AknsUtils.h>
#include <AknsConstants.h>//skinned icon ids
#include "ImumMtmLogging.h"

// Constants
const TInt KIMAP4MtmUdNumberOfZoomStates=2; // second one is mask!
const TInt KImumImap4FirstBitmapIndex = EMbmMuiuQgn_prop_mce_remote_sub;
const TInt KImumImap4LastBitmapIndex = EMbmMuiuQgn_prop_folder_sub_unsubs_new_mask;

const TInt KIsSupported=0;
_LIT(KIMAP4MtmUdResourceFile, "IMUM.RSC");
// Correct path is added to literal when it is used.
_LIT(KIMAP4MtmUdBitmapFile, "z:muiu.MBM");

// ----------------------------------------------------------------------------
// NewIMPIMtmUiDataLayerL
// ----------------------------------------------------------------------------
EXPORT_C CBaseMtmUiData* NewIMPIMtmUiDataLayerL(CRegisteredMtmDll& aRegisteredDll)
    {
    return CImap4MtmUiData::NewL(aRegisteredDll);
    }

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
CImap4MtmUiData* CImap4MtmUiData::NewL(CRegisteredMtmDll& aRegisteredDll)
    {
    IMUM_STATIC_CONTEXT( CImap4MtmUiData::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CImap4MtmUiData* self=new(ELeave) CImap4MtmUiData(aRegisteredDll);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); //self
    IMUM_OUT();
    return self;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUiData
// ----------------------------------------------------------------------------
CImap4MtmUiData::CImap4MtmUiData(CRegisteredMtmDll& aRegisteredDll)
    :   CBaseMtmUiData(aRegisteredDll)
    {
    IMUM_CONTEXT( CImap4MtmUiData::CImap4MtmUiData, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    __DECLARE_NAME(_S("CImap4MtmUiData"));
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
void CImap4MtmUiData::ConstructL()
    {
    IMUM_CONTEXT( CImap4MtmUiData::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    CBaseMtmUiData::ConstructL();

    iLastResortIcon = new(ELeave) CArrayPtrFlat<CFbsBitmap>(1);
    CWsBitmap* bitmap = new(ELeave) CWsBitmap(iCoeEnv->WsSession());
    CleanupStack::PushL(bitmap);
    User::LeaveIfError( bitmap->Create(TSize(1, 1), iCoeEnv->ScreenDevice()->DisplayMode()) );
    iLastResortIcon->AppendL(bitmap);
    CleanupStack::Pop(); // bitmap
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// ~CImap4MtmUiData
// ----------------------------------------------------------------------------
CImap4MtmUiData::~CImap4MtmUiData()
    {
    IMUM_CONTEXT( CImap4MtmUiData::~CImap4MtmUiData, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iLastResortIcon)
        {
        iLastResortIcon->ResetAndDestroy();
        delete iLastResortIcon;
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// OperationSupportedL
// ----------------------------------------------------------------------------
TInt CImap4MtmUiData::OperationSupportedL(TInt aOperationId, const TMsvEntry& aContext) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::OperationSupportedL, 0, KImumMtmLog );
    IMUM_IN();
    
    if (!CheckEntry(aContext))
        return R_EIK_TBUF_NOT_AVAILABLE;

    TInt returnValue=R_EIK_TBUF_NOT_AVAILABLE;

    switch (aOperationId)
        {
    case KMtmUiFunctionMessageInfo:
        if(aContext.iType==KUidMsvMessageEntry)
            returnValue=KIsSupported;
        break;
    case KIMAP4MTMConnect:
    case KIMAP4MTMConnectAndSynchronise:
    case KImpiMTMConnect:
    case KImpiMTMConnectAndSyncCompleteAfterConnection:
    case KImpiMTMConnectAndSyncCompleteAfterFolderSync:
    case KImpiMTMConnectAndSyncCompleteAfterDisconnection:
    case KImpiMTMRefreshFolderList:
        if(!aContext.Connected() && aContext.iType==KUidMsvServiceEntry)
            returnValue=KIsSupported;
        break;
    case KIMAP4MTMDisconnect:
        if(aContext.Connected())
            {
            if(aContext.iType==KUidMsvServiceEntry)
                returnValue=KIsSupported;
            }
        else
            {
            returnValue=R_EIK_TBUF_NOT_AVAILABLE;
            }
        break;
    case KImpiMTMPurgeAttachment:
        if(aContext.iType==KUidMsvAttachmentEntry)
            returnValue=KIsSupported;
        break;
    case KImpiMTMSubscribedFolders:
        {
        returnValue=KIsSupported;
        }
        break;
    case KImpiMTMFetchAttachments:
    case KIMAP4MTMCancelBackgroundSynchronise:
    case KIMAP4MTMStartBatch:
    case KIMAP4MTMEndBatch:
    case KIMAP4MTMBusy:
    case KIMAP4MTMSelect:
    case KIMAP4MTMSynchronise:
    case KIMAP4MTMSyncTree:
    case KIMAP4MTMSyncSubscription:
    case KIMAP4MTMFullSync:
    case KIMAP4MTMLocalSubscribe:
    case KIMAP4MTMLocalUnsubscribe:

    case KMtmUiFunctionFetchAll:
    case KMtmUiFunctionFetchNew:
    case KMtmUiFunctionFetchSelected:
    case KMtmUiFunctionGetNew:
        returnValue=KIsSupported;
        break;
    default:
        __ASSERT_DEBUG( 0, User::Panic(KImumMtmUiPanic, EIMAP4MtmUdUnknownOperationId) );
        break;
        };
    IMUM_OUT();
    return returnValue;
    }


// ----------------------------------------------------------------------------
// QueryCapability
// ----------------------------------------------------------------------------
TInt CImap4MtmUiData::QueryCapability(TUid aCapability, TInt& aResponse) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::QueryCapability, 0, KImumMtmLog );
    IMUM_IN();
    
    switch (aCapability.iUid)
        {
    // --- Supported valued capabilities ---
    case KUidMtmQueryMaxBodySizeValue:
    case KUidMtmQueryMaxTotalMsgSizeValue:
        aResponse=KMaxTInt;
        break;
    case KUidMsvMtmQueryEditorUidValue:
        aResponse=KUidMsgInternetMailEditorAppValue;
        break;
    case KUidMsvMtmQueryViewerUidValue:
        aResponse=KUidMsgInternetMailViewerAppValue;
        break;

    // --- Supported non-valued capabilities ---
    case KUidMtmQuerySupportedBodyValue:
    case KUidMtmQueryCanReceiveMsgValue:
    case KUidMtmQuerySupportAttachmentsValue:
    case KUidMsvMtmUiQueryConnectionOrientedServices:
    case KUidMtmQuerySupportsFolderValue:
    case KUidMsvMtmUiQueryExtendedOpenProgress:
    case KUidMsvMtmUiQueryExtendedGetMailProgress:
    case KUidMsvMtmUiQueryCanGetNew:
    case KMtmUiFunctionMessageInfo:
        aResponse=0;
        break;
    // --- Non-supported capabilities ---
    default:
        IMUM_OUT();
        return KErrNotSupported;
        };
    IMUM_OUT();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// ContextIcon
// ----------------------------------------------------------------------------
const CBaseMtmUiData::CBitmapArray& CImap4MtmUiData::ContextIcon(const TMsvEntry& aContext, TInt aStateFlags) const
    {
    IMUM_CONTEXT( CBaseMtmUiData::CBitmapArray, 0, KImumMtmLog );
    IMUM_IN();
    
    // Deferred loading of bitmaps.
    if(!iIconArrays->Count())
        {
        //workaround const with const_cast
        TRAPD( err, const_cast<CImap4MtmUiData*>(this)->CreateSkinnedBitmapsL( KIMAP4MtmUdNumberOfZoomStates ) );
        if(err != KErrNone)
            {
            // Failed to load
            IMUM_OUT();
            return *iLastResortIcon;
            }

        /*
        // --- Populate bitmap array ---
        TRAPD(err, CONST_CAST(CImap4MtmUiData*,this)->CreateBitmapsL(
            KIMAP4MtmUdNumberOfZoomStates,
            KIMAP4MtmUdBitmapFile,
            KImumImap4FirstBitmapIndex,
            KImumImap4LastBitmapIndex));
        if(err != KErrNone)
            {
            // Failed to load
            return *iLastResortIcon;
            }
            */
        }

    TInt retIndex=0;

    TMsvEmailEntry mailEntry = aContext;

    // Service; can include unread messages or not
    if (aContext.iType.iUid == KUidMsvServiceEntryValue)
        {
        retIndex = (aStateFlags & EMtudEntryStateOpen) ?
            EImumPropMceRemoteNewSub :
            EImumPropMceRemoteSub;

        }
    // Messages; normal, deleted, incomplete
    else if ( aContext.iType.iUid==KUidMsvMessageEntryValue )
        {
        if(EDisconnectedDeleteOperation == mailEntry.DisconnectedOperation())
            {
            retIndex = EImumPropMceMailFetReaDel;
            }
        else
            {
            if(!mailEntry.BodyTextComplete())
                {
                retIndex = aContext.Unread() ?
                    EImumPropMceMailUnfetUnread :
                    EImumPropMceMailUnfetRead;
                }
            else
                {
                retIndex = aContext.Unread() ?
                    EImumPropMceMailFetUnread :
                    EImumPropMceMailFetRead;
                }
            }
        }
    // Folders
    else
        {
        TBool hasNew = aStateFlags & EMtudEntryStateOpen;
        TBool hasSub = aStateFlags & EMtudEntryStateSubFolder;

        TMsvEmailEntry eEntry( aContext );
        TBool isSubbed = eEntry.LocalSubscription();

        if( hasSub )
            {
            if( hasNew )
                {
                if( isSubbed )
                    {
                    retIndex = EImumPropFolderSubSubsNew;
                    }//if
                else
                    {
                    retIndex = EImumPropFolderSubUnsubsNew;
                    }//else
                }//if
            else
                {
                if( isSubbed )
                    {
                    retIndex = EImumPropFolderSubSubs;
                    }//if
                else
                    {
                    retIndex = EImumPropFolderSubSmall;
                    }//else
                }//else
            }//if
        else
            {
            if( hasNew )
                {
                if( isSubbed )
                    {
                    retIndex = EImumPropFolderSubsNew;
                    }//if
                else
                    {
                    retIndex = EImumPropFolderUnsubsNew;
                    }//else
                }//if
            else
                {
                if( isSubbed )
                    {
                    retIndex = EImumPropFolderSubs;
                    }//if
                else
                    {
                    retIndex = EImumPropFolderSmall;
                    }
                }//else
            }//else

        }//else
    IMUM_OUT();

    return *iIconArrays->At( retIndex / KIMAP4MtmUdNumberOfZoomStates );
    }

// ----------------------------------------------------------------------------
// PopulateArraysL
// ----------------------------------------------------------------------------
void CImap4MtmUiData::PopulateArraysL()
    {
    IMUM_CONTEXT( CImap4MtmUiData::PopulateArraysL, 0, KImumMtmLog );
    IMUM_IN();
    
    // --- Read commands list from resource ---
    ReadFunctionsFromResourceFileL(R_IMAP4UD_FUNCTION_ARRAY);
    IMUM_OUT();

    // Defer bitmap loading until requested.
    }

// ----------------------------------------------------------------------------
// GetResourceFileName
// ----------------------------------------------------------------------------
void CImap4MtmUiData::GetResourceFileName(TFileName& aFileName) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::GetResourceFileName, 0, KImumMtmLog );
    IMUM_IN();
    
    aFileName=KIMAP4MtmUdResourceFile;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CanCreateEntryL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanCreateEntryL(const TMsvEntry& aParent, TMsvEntry& aNewEntry, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanCreateEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    if (!CheckEntry(aNewEntry))
        {
        IMUM_OUT();
        return EFalse;
        }

    // --- Can create IMAP4 service
    if ((aNewEntry.iType.iUid==KUidMsvServiceEntryValue) && (aParent.Id()==KMsvRootIndexEntryId))
        {
        aReasonResourceId=0;
        IMUM_OUT();
        return ETrue;
        }

    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanOpenEntryL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanOpenEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanOpenEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanCloseEntryL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanCloseEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanCloseEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanViewEntryL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanViewEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanViewEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanEditEntryL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanEditEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanEditEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanDeleteFromEntryL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanDeleteFromEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanDeleteFromEntryL, 0, KImumMtmLog );
    IMUM_IN();

    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    if (!CheckEntry(aContext))
        {
        IMUM_OUT();
        return EFalse;
        }

    const TMsvEmailEntry& context=aContext;

    if(context.iType.iUid==KUidMsvMessageEntryValue &&
       context.DisconnectedOperation()==ENoDisconnectedOperations)
        {
        aReasonResourceId=0;
        IMUM_OUT();
        return ETrue;
        }
    IMUM_OUT();

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanUnDeleteFromEntryL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanUnDeleteFromEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanUnDeleteFromEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    // N.B. Current Message Centre implementation specifies that aContext is actually the entry
    // to be undeleted, not the parent.
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    if (!CheckEntry(aContext))
        {
        IMUM_OUT();
        return EFalse;
        }

    const TMsvEmailEntry& context=aContext;

    if(context.iType.iUid==KUidMsvMessageEntryValue && context.DisconnectedOperation()==EDisconnectedDeleteOperation)
        {
        aReasonResourceId=0;
        IMUM_OUT();
        return ETrue;
        }
    IMUM_OUT();

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanCopyMoveFromEntryL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanCopyMoveFromEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanCopyMoveFromEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanCopyMoveToEntryL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanCopyMoveToEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanCopyMoveToEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanReplyToEntryL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanReplyToEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanReplyToEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG( aContext.iMtm == KUidMsgTypeIMAP4, User::Panic(KImumMtmUiPanic,  EIMAP4MtmUdWrongMtm ));
    if ( aContext.iMtm == KUidMsgTypeIMAP4
        && aContext.iType == KUidMsvMessageEntry )
        {
        IMUM_OUT();
        return ETrue;
        }
    else
        {
        aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
        IMUM_OUT();
        return EFalse;
        }

    }

// ----------------------------------------------------------------------------
// CanForwardEntryL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanForwardEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanForwardEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG( aContext.iMtm == KUidMsgTypeIMAP4, User::Panic(KImumMtmUiPanic,  EIMAP4MtmUdWrongMtm ));
    if ( aContext.iMtm == KUidMsgTypeIMAP4
        && aContext.iType == KUidMsvMessageEntry )
        {
        IMUM_OUT();
        return ETrue;
        }
    else
        {
        aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
        IMUM_OUT();
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CanDeleteServiceL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanDeleteServiceL(const TMsvEntry& /*aService*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanDeleteServiceL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanCancelL
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CanCancelL(const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CanCancelL, 0, KImumMtmLog );
    IMUM_IN();
    
    IMUM_OUT();
    // IMAP messages can never be sent, consequently can never be cancelled.
    return EFalse;
    }

// ----------------------------------------------------------------------------
// StatusTextL
// ----------------------------------------------------------------------------
HBufC* CImap4MtmUiData::StatusTextL(const TMsvEntry& /*aContext*/) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::StatusTextL, 0, KImumMtmLog );
    IMUM_IN();
    
    IMUM_OUT();
    // IMAP messages never have a status in the outbox.
    return HBufC::NewL(1);
    }

// ----------------------------------------------------------------------------
// CheckEntry
// ----------------------------------------------------------------------------
TBool CImap4MtmUiData::CheckEntry(const TMsvEntry& aContext) const
    {
    IMUM_CONTEXT( CImap4MtmUiData::CheckEntry, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    return  (aContext.iMtm==KUidMsgTypeIMAP4) ;
    }

// ----------------------------------------------------------------------------
// CreateSkinnedBitmapsL
// ----------------------------------------------------------------------------
void CImap4MtmUiData::CreateSkinnedBitmapsL( TInt aNumZoomStates )
    {
    IMUM_CONTEXT( CImap4MtmUiData::CreateSkinnedBitmapsL, 0, KImumMtmLog );
    IMUM_IN();
    
    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    TAknsItemID id;
    CFbsBitmap* bitmap;
    CFbsBitmap* bitmapMask;
    TBool found = ETrue;

    CBitmapArray* array = NULL;


    for( TInt i=KImumImap4FirstBitmapIndex; i<KImumImap4LastBitmapIndex+1; i++ )
        {
        found = ETrue;
        switch( i )
            {
            case EMbmMuiuQgn_prop_mce_remote_sub:
                id.Set( KAknsIIDQgnPropMceRemoteSub );
                break;
            case EMbmMuiuQgn_prop_mce_remote_new_sub:
                id.Set( KAknsIIDQgnPropMceRemoteNewSub );
                break;
            case EMbmMuiuQgn_prop_mce_mail_fet_rea_del:
                id.Set( KAknsIIDQgnPropMceMailFetReaDel );
                break;
            case EMbmMuiuQgn_prop_mce_mail_fet_read:
                id.Set( KAknsIIDQgnPropMceMailFetRead );
                break;
            case EMbmMuiuQgn_prop_mce_mail_fet_unread:
                id.Set( KAknsIIDQgnPropMceMailFetUnread );
                break;
            case EMbmMuiuQgn_prop_mce_mail_unfet_read:
                id.Set( KAknsIIDQgnPropMceMailUnfetRead );
                break;
            case EMbmMuiuQgn_prop_mce_mail_unfet_unread:
                id.Set( KAknsIIDQgnPropMceMailUnfetUnread );
                break;
            case EMbmMuiuQgn_prop_folder_small:
                id.Set( KAknsIIDQgnPropFolderSmall );
                break;
            case EMbmMuiuQgn_prop_folder_subs:
                id.Set( KAknsIIDQgnPropFolderSubs );
                break;
            case EMbmMuiuQgn_prop_folder_subs_new:
                id.Set( KAknsIIDQgnPropFolderSubsNew );
                break;
            case EMbmMuiuQgn_prop_folder_sub_small:
                id.Set( KAknsIIDQgnPropFolderSubSmall );
                break;
            case EMbmMuiuQgn_prop_folder_sub_subs:
                id.Set( KAknsIIDQgnPropFolderSubSubs );
                break;
            case EMbmMuiuQgn_prop_folder_sub_subs_new:
                id.Set( KAknsIIDQgnPropFolderSubSubsNew );
                break;
            case EMbmMuiuQgn_prop_folder_unsubs_new:
                id.Set( KAknsIIDQgnPropFolderUnsubsNew );
                break;
            case EMbmMuiuQgn_prop_folder_sub_unsubs_new:
                id.Set( KAknsIIDQgnPropFolderSubUnsubsNew );
                break;
            default:
                found = EFalse;
                break;
            }

        if( found )
            {
            array = new(ELeave) CArrayPtrFlat< CFbsBitmap >( aNumZoomStates );
            CleanupStack::PushL( array );

            TParse tp;
            tp.Set( KIMAP4MtmUdBitmapFile, &KDC_APP_BITMAP_DIR, NULL );

            AknsUtils::CreateIconL( skins, id, bitmap,
                bitmapMask, tp.FullName(), i, i + 1 );

            array->AppendL( bitmap );
            array->AppendL( bitmapMask );
            iIconArrays->AppendL( array );

            CleanupStack::Pop();//array
            }
        }
    IMUM_OUT();
    }

// End of File

