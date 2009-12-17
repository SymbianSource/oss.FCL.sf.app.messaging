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
*       POP3 MTM Ui data layer base API
*
*/


// Standard includes
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mtmuidsdef.hrh>
#endif
#include <coemain.h>    // ccoeenv
#include <eikon.rsg>

// Messaging include
#include <mtclbase.h>
#include <mtmuidef.hrh>
#include <miutset.h>
#include <mtud.hrh>
#include <MtmExtendedCapabilities.hrh>

#include <AknsUtils.h>
#include <AknsConstants.h>//skinned icon ids

#include <data_caging_path_literals.hrh>

// Specific includes
#include <pop3cmds.h>
#include <popcmtm.h>
#include "ImumPanic.h"
#include <imum.rsg>
#include <muiu.mbg>
#include "Pop3MtmUiData.h"
#include "EmailEditorViewerUids.h"
#include "ImumMtmLogging.h"

// Constants
const TInt KPop3MtmUdNumberOfZoomStates =   2; // second one is mask!
const TInt KImumPop3FirstBitmapIndex = EMbmMuiuQgn_prop_mce_remote_sub;
const TInt KImumPop3LastBitmapIndex = EMbmMuiuQgn_prop_mce_mail_unfet_unread_mask;

// Defines
_LIT( KPop3MtmUdResourceFile, "IMUM");
// Correct path is added to literal when it is used.
_LIT( KPop3MtmUdBitmapFile, "z:muiu.MBM");

// ----------------------------------------------------------------------------
// CPop3MtmUiData::NewPP3IMtmUiDataLayerL()
// ----------------------------------------------------------------------------
EXPORT_C CBaseMtmUiData* NewPP3IMtmUiDataLayerL(CRegisteredMtmDll& aRegisteredDll)
    {
    return CPop3MtmUiData::NewL(aRegisteredDll);
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::NewL()
// ----------------------------------------------------------------------------
CPop3MtmUiData* CPop3MtmUiData::NewL(CRegisteredMtmDll& aRegisteredDll)
    {
    IMUM_STATIC_CONTEXT( CPop3MtmUiData::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CPop3MtmUiData* base=new(ELeave) CPop3MtmUiData(aRegisteredDll);
    CleanupStack::PushL(base);
    base->ConstructL();
    CleanupStack::Pop();
    IMUM_OUT();
    return base;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CPop3MtmUiData()
// ----------------------------------------------------------------------------
CPop3MtmUiData::CPop3MtmUiData(CRegisteredMtmDll& aRegisteredDll)
    :   CBaseMtmUiData(aRegisteredDll)
    {
    IMUM_CONTEXT( CPop3MtmUiData::CPop3MtmUiData, 0, KImumMtmLog );
    IMUM_IN();
    
    __DECLARE_NAME(_S("CPop3MtmUiData"));
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::ConstructL()
// ----------------------------------------------------------------------------
void CPop3MtmUiData::ConstructL()
    {
    IMUM_CONTEXT( CPop3MtmUiData::ConstructL, 0, KImumMtmLog );
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
// CPop3MtmUiData::~CPop3MtmUiData()
// ----------------------------------------------------------------------------
CPop3MtmUiData::~CPop3MtmUiData()
    {
    IMUM_CONTEXT( CPop3MtmUiData::~CPop3MtmUiData, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iLastResortIcon)
        {
        iLastResortIcon->ResetAndDestroy();
        delete iLastResortIcon;
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::OperationSupportedL()
// ----------------------------------------------------------------------------
TInt CPop3MtmUiData::OperationSupportedL(
    TInt aOperationId,
    const TMsvEntry& aContext) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::OperationSupportedL, 0, KImumMtmLog );
    IMUM_IN();
    
    if ( CheckEntry( aContext ) )
        {
        switch ( aOperationId )
            {
            case KMtmUiFunctionFetchAll:
            case KMtmUiFunctionFetchNew:
            case KMtmUiFunctionFetchSelected:
            case KMtmUiFunctionGetNew:
                IMUM_OUT();
                return 0;   // Operation *IS* supported

            case KPOP3MTMDisconnect:
                if ( aContext.Connected() )
                    {
                    IMUM_OUT();
                    return 0;// Operation *IS* supported
                    }
                return R_EIK_TBUF_NOT_AVAILABLE;

            case KPOP3MTMConnect:
                if ( !aContext.Connected() )
                    {
                    IMUM_OUT();
                    return 0;// Operation *IS* supported
                    }
                    IMUM_OUT();
                return R_EIK_TBUF_NOT_AVAILABLE;

            case KMtmUiFunctionMessageInfo:
                if ( aContext.iType.iUid==KUidMsvMessageEntryValue )
                    {
                    IMUM_OUT();
                    return 0;// Operation *IS* supported
                    }
                break;
            default:
                // Not supported.
                break;
            }
        }
    IMUM_OUT();

    return R_EIK_TBUF_NOT_AVAILABLE;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::QueryCapability()
// ----------------------------------------------------------------------------
TInt CPop3MtmUiData::QueryCapability(TUid aCapability, TInt& aResponse) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::QueryCapability, 0, KImumMtmLog );
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
        break;  //
    case KUidMtmQuerySupportedBodyValue:
        aResponse = KMtm7BitBody | KMtm8BitBody | KMtm16BitBody;
        break;

    // --- Supported non-valued capabilities ---
    case KUidMsvMtmUiQueryConnectionOrientedServices:
    case KUidMtmQueryCanReceiveMsgValue:
    case KUidMsvMtmUiQueryCanGetNew:
    case KMtmUiFunctionMessageInfo:
        break;
    default:
        IMUM_OUT();
        return KErrNotSupported;
        };
    IMUM_OUT();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::ContextIcon()
// ----------------------------------------------------------------------------
const CBaseMtmUiData::CBitmapArray& CPop3MtmUiData::ContextIcon(
    const TMsvEntry& aContext,
    TInt aStateFlags ) const
    {
    IMUM_CONTEXT( CBaseMtmUiData::CBitmapArray, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG( CheckEntry(aContext), User::Panic(KImumMtmUiPanic, EPop3MtmUdNoIconAvailable) );

    __ASSERT_ALWAYS( aContext.iType.iUid != KUidMsvAttachmentEntryValue, User::Panic(KImumMtmUiPanic, EPop3MtmUdNoIconForAttachment) );

    if ( iIconArrays && !iIconArrays->Count() )
        {
        TRAPD( err, const_cast<CPop3MtmUiData*>(this)->
            CreateSkinnedBitmapsL( KPop3MtmUdNumberOfZoomStates ) );
        if ( err != KErrNone )
            {
            IMUM_OUT();
            // Failed to load
            return *iLastResortIcon;
            }
        }

    TInt retIndex = 0;
    if ( aContext.iType.iUid == KUidMsvServiceEntryValue )
        {
        retIndex = ( aStateFlags & EMtudEntryStateOpen ) ?
            EImumPropMceRemoteNewSub :
            EImumPropMceRemoteSub;
        }
    else
        {
        // Email message bitmap
        TMsvEmailEntry mailEntry( aContext );
        if ( EDisconnectedDeleteOperation == mailEntry.DisconnectedOperation() )
            {
            retIndex = EImumPropMceMailFetReaDel;
            }
        else if ( !aContext.Complete() )
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
    IMUM_OUT();
    return *iIconArrays->At( retIndex / KPop3MtmUdNumberOfZoomStates );
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::PopulateArraysL()
// ----------------------------------------------------------------------------
void CPop3MtmUiData::PopulateArraysL()
    {
    IMUM_CONTEXT( CPop3MtmUiData::PopulateArraysL, 0, KImumMtmLog );
    IMUM_IN();
    
    ReadFunctionsFromResourceFileL(R_POP3UD_FUNCTION_ARRAY);

    IMUM_OUT();
    // Defer bitmap loading until requested.
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::GetResourceFileName()
// ----------------------------------------------------------------------------
void CPop3MtmUiData::GetResourceFileName(TFileName& aFileName) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::GetResourceFileName, 0, KImumMtmLog );
    IMUM_IN();
    
    aFileName=KPop3MtmUdResourceFile;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanCreateEntryL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanCreateEntryL(const TMsvEntry& aParent, TMsvEntry& aNewEntry, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanCreateEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    if (CheckEntry(aNewEntry))
        {
        // --- Can't create POP3 attachments or messages - They're created when receiving ---
        if ( (aNewEntry.iType.iUid==KUidMsvServiceEntryValue) && (aParent.Id()==KMsvRootIndexEntryIdValue) )
            {
            aReasonResourceId=0;
            IMUM_OUT();
            return ETrue;
            }
        }
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanOpenEntryL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanOpenEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanOpenEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanCloseEntryL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanCloseEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanCloseEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanViewEntryL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanViewEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanViewEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanEditEntryL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanEditEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanEditEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanDeleteFromEntryL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanDeleteFromEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanDeleteFromEntryL, 0, KImumMtmLog );
    IMUM_IN();

    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    if (CheckEntry(aContext))
        {
        const TMsvEmailEntry& context=aContext;
        if(context.iType.iUid==KUidMsvMessageEntryValue &&
           context.DisconnectedOperation()==ENoDisconnectedOperations)
            {
            aReasonResourceId=0;
            IMUM_OUT();
            return ETrue;
            }
        }
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanUnDeleteFromEntryL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanUnDeleteFromEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanUnDeleteFromEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    // N.B. Current Message Centre implementation specifies that aContext is actually the entry
    // to be undeleted, not the parent.
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    if (CheckEntry(aContext))
        {
        const TMsvEmailEntry& context=aContext;
        if(context.iType.iUid==KUidMsvMessageEntryValue && context.DisconnectedOperation()==EDisconnectedDeleteOperation)
            {
            aReasonResourceId=0;
            IMUM_OUT();
            return ETrue;
            }
        }
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanDeleteServiceL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanDeleteServiceL(const TMsvEntry& /*aService*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanDeleteServiceL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanCopyMoveFromEntryL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanCopyMoveFromEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanCopyMoveFromEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanCopyMoveToEntryL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanCopyMoveToEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanCopyMoveToEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanReplyToEntryL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanReplyToEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanReplyToEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG( aContext.iMtm == KUidMsgTypePOP3, User::Panic(KImumMtmUiPanic,  EPop3MtmUdWrongMtm ));
    if ( aContext.iMtm == KUidMsgTypePOP3
        && aContext.iType == KUidMsvMessageEntry )
        {
        IMUM_OUT();
        return ETrue;
        }
    else
        {
        IMUM_OUT();
        aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
        return EFalse;
        }

    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanForwardEntryL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanForwardEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanForwardEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG( aContext.iMtm == KUidMsgTypePOP3, User::Panic(KImumMtmUiPanic,  EPop3MtmUdWrongMtm ));
    if ( aContext.iMtm == KUidMsgTypePOP3
        && aContext.iType == KUidMsvMessageEntry
        && aContext.ReadOnly() )
        {
        IMUM_OUT();
        return ETrue;
        }
    else
        {
        IMUM_OUT();
        aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CanCancelL()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CanCancelL(const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CanCancelL, 0, KImumMtmLog );
    IMUM_IN();
    
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::StatusTextL()
// ----------------------------------------------------------------------------
HBufC* CPop3MtmUiData::StatusTextL(const TMsvEntry& /*aContext*/) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::StatusTextL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    // POP messages never have a status in the outbox.
    return HBufC::NewL(0);
    }

// ----------------------------------------------------------------------------
// CPop3MtmUiData::CheckEntry()
// ----------------------------------------------------------------------------
TBool CPop3MtmUiData::CheckEntry(const TMsvEntry& aContext) const
    {
    IMUM_CONTEXT( CPop3MtmUiData::CheckEntry, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    return ( (aContext.iMtm==KUidMsgTypePOP3) && (aContext.iType.iUid!=KUidMsvFolderEntryValue) );
    }


// ----------------------------------------------------------------------------
// CPop3MtmUiData::CreateSkinnedBitmapsL()
// ----------------------------------------------------------------------------
void CPop3MtmUiData::CreateSkinnedBitmapsL( TInt aNumZoomStates )
    {
    IMUM_CONTEXT( CPop3MtmUiData::CreateSkinnedBitmapsL, 0, KImumMtmLog );
    IMUM_IN();
    
    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    TAknsItemID id;
    CFbsBitmap* bitmap;
    CFbsBitmap* bitmapMask;
    TBool found = ETrue;;

    CBitmapArray* array = NULL;


    for( TInt i=KImumPop3FirstBitmapIndex; i<KImumPop3LastBitmapIndex+1; i++ )
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
            default:
                found = EFalse;
                break;
            }

        if( found )
            {
            array = new(ELeave) CArrayPtrFlat< CFbsBitmap >( aNumZoomStates );
            CleanupStack::PushL( array );

            TParse tp;
            tp.Set( KPop3MtmUdBitmapFile, &KDC_APP_BITMAP_DIR, NULL );

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
