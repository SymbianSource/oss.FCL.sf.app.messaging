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
*       SMTP MTM Ui data layer base API
*
*/

// INCLUDE FILES
#include <coemain.h>    // ccoeenv
#include <eikon.rsg>

// Messaging includes
#include <mtclbase.h>
#include <mtmuidef.hrh>
#include <smtcmtm.h>// ksmtcmaxtextmessagesize, kuidmsgtypesmtp
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mtmdef.hrh> 
#else
#include <mtmdef.hrh>
#include <mtmuidsdef.hrh>
#endif
#include <MtmExtendedCapabilities.hrh>

// Specific includes
#include "SmtpMtmUiData.h"
#include "ImumPanic.h"
#include <muiu.mbg>
#include <imum.rsg>
#include "EmailEditorViewerUids.h"

#include <AknsUtils.h>
#include <AknsConstants.h>//skinned icon ids
#include <data_caging_path_literals.hrh>
#include <featmgr.h>

// CONSTANTS
const TInt KImumSmtpBitmapIndex = 0; // only one bitmap for create email
const TInt KSmtpMtmUdNumberOfZoomStates =       2; // second one is mask!
_LIT(KSmtpMtmUdResourceFile, "IMUM");
// Correct path is added to literal when it is used.
_LIT(KSmtpMtmUdBitmapFile, "z:MUIU.MBM");


// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
//NewSMTIMtmUiDataLayerL
// ----------------------------------------------------------------------------
EXPORT_C CBaseMtmUiData* NewSMTIMtmUiDataLayerL(CRegisteredMtmDll& aRegisteredDll)
    {
    return CSmtpMtmUiData::NewL(aRegisteredDll);
    }


// ----------------------------------------------------------------------------
//NewL
// ----------------------------------------------------------------------------
CSmtpMtmUiData* CSmtpMtmUiData::NewL(CRegisteredMtmDll& aRegisteredDll)
    {
    CSmtpMtmUiData* self=new(ELeave) CSmtpMtmUiData(aRegisteredDll);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// ~CSmtpMtmUiData
// ----------------------------------------------------------------------------
CSmtpMtmUiData::~CSmtpMtmUiData()
    {
    }

// ----------------------------------------------------------------------------
// OperationSupportedL
// ----------------------------------------------------------------------------
TInt CSmtpMtmUiData::OperationSupportedL(TInt aOperationId, const TMsvEntry& aContext) const
    {
    TInt rid=R_EIK_TBUF_NOT_AVAILABLE;
    if (CheckEntry(aContext))
        {
        if (aContext.iType.iUid==KUidMsvMessageEntryValue)
            {
            switch (aOperationId)
                {
            case KMtmUiFunctionSendAs:
            case KMtmUiFunctionMessageInfo:
                rid=0; // Operation is supported
                break;
            default:
                break;
                }
            }
        }
    return rid;
    }

// ----------------------------------------------------------------------------
// QueryCapability
// ----------------------------------------------------------------------------
TInt CSmtpMtmUiData::QueryCapability(TUid aCapability, TInt& aResponse) const
    {
    switch (aCapability.iUid)
        {
        // --- Supported valued capabilities ---
        case KUidMtmQueryMaxBodySizeValue:
            aResponse=KMaxTInt;
            break;
        case KUidMtmQueryMaxTotalMsgSizeValue:
            aResponse=KMaxTInt;
            break;
        case KUidMsvMtmQueryEditorUidValue:
            aResponse=KUidMsgInternetMailEditorAppValue;
            break;
        case KUidMsvMtmQueryViewerUidValue:
            aResponse=KUidMsgInternetMailViewerAppValue;
            break;
        case KUidMtmQuerySupportedBodyValue:
            aResponse = KMtm7BitBody | KMtm8BitBody | KMtm16BitBody;
            break;
    
        //
        // --- Support depends on variation ---
        case KUidMsvMtmQuerySupportEditor:
            aResponse = ETrue;
            break;
        // --- Supported non-valued capabilities ---
		case KUidMtmQueryCanCreateNewMsgValue:
        case KUidMtmQuerySupportAttachmentsValue:
        case KUidMtmQueryCanSendMsgValue:
        case KUidMsvMtmQueryMessageInfo:
                break;
        default:
            return KErrNotSupported;
        };
    return KErrNone;
    }


#ifdef _DEBUG
const CBaseMtmUiData::CBitmapArray& CSmtpMtmUiData::ContextIcon(const TMsvEntry& aContext,TInt /*aStateFlags*/) const
#else
const CBaseMtmUiData::CBitmapArray& CSmtpMtmUiData::ContextIcon(const TMsvEntry& /*aContext*/,TInt /*aStateFlags*/) const
#endif
    {
    __ASSERT_DEBUG(CheckEntry(aContext), User::Panic(KImumMtmUiPanic, ESmtpMtmUdNoIconAvailable));
    __ASSERT_DEBUG(aContext.iType!=KUidMsvAttachmentEntry, User::Panic(KImumMtmUiPanic, ESmtpMtmUdNoIconForAttachment));

    return *iIconArrays->At( KImumSmtpBitmapIndex ); // smtp has only one icon
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUiData
// ----------------------------------------------------------------------------
CSmtpMtmUiData::CSmtpMtmUiData(CRegisteredMtmDll& aRegisteredDll)
    :       CBaseMtmUiData(aRegisteredDll)
    {
    __ASSERT_DEBUG(aRegisteredDll.MtmTypeUid()==KUidMsgTypeSMTP, User::Panic(KImumMtmUiPanic, ESmtpMtmUdWrongMtmType));
    }

// ----------------------------------------------------------------------------
// PopulateArraysL
// ----------------------------------------------------------------------------
void CSmtpMtmUiData::PopulateArraysL()
    {
    FeatureManager::InitializeLibL();
    iFFEmailVariation = 
        FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework );
    FeatureManager::UnInitializeLib();

    // --- Populate function array ---
    ReadFunctionsFromResourceFileL( iFFEmailVariation ?
        R_SMTPUD_FUNCTION_ARRAY_FFEMAIL : R_SMTPUD_FUNCTION_ARRAY );

    //
    // --- Populate bitmap array ---
    CreateSkinnedBitmapsL( KSmtpMtmUdNumberOfZoomStates );
    }

// ----------------------------------------------------------------------------
// GetResourceFileName
// ----------------------------------------------------------------------------
void CSmtpMtmUiData::GetResourceFileName(TFileName& aFileName) const
    {
    aFileName=KSmtpMtmUdResourceFile;
    }

// ----------------------------------------------------------------------------
// CanCreateEntryL
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CanCreateEntryL(const TMsvEntry& aParent, TMsvEntry& aNewEntry, TInt& aReasonResourceId) const
    {
    if (CheckEntry(aNewEntry))
        {
        aReasonResourceId=0;
        //
        // --- Can create services if they are off root ---
        if (aNewEntry.iType.iUid == KUidMsvServiceEntryValue)
            return (aParent.Id() == KMsvRootIndexEntryIdValue);
        //
        // --- Can create messages in local folders ---
        if (aNewEntry.iType.iUid == KUidMsvMessageEntryValue)
            return (aParent.iMtm.iUid == KMsvLocalServiceIndexEntryIdValue);
        }
    //
    // --- Can't create other types ---
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanOpenEntryL
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CanOpenEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    // not used, just return EFalse
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanCloseEntryL
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CanCloseEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanViewEntryL
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CanViewEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanEditEntryL
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CanEditEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanDeleteServiceL
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CanDeleteServiceL(const TMsvEntry& /*aService*/, TInt& aReasonResourceId) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanDeleteFromEntryL
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CanDeleteFromEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanCopyMoveToEntryL
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CanCopyMoveToEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanCopyMoveFromEntryL
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CanCopyMoveFromEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanReplyToEntryL
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CanReplyToEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanForwardEntryL
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CanForwardEntryL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CanCancelL
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CanCancelL(const TMsvEntry& /*aContext*/, TInt& aReasonResourceId) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// StatusTextL
// ----------------------------------------------------------------------------
HBufC* CSmtpMtmUiData::StatusTextL(const TMsvEntry& aContext) const
    {
    TInt res = 0;
    switch(aContext.SendingState())
        {
        case KMsvSendStateFailed:
            res = R_SMTP_OUTBOX_STATUS_FAILED;
            break;
        // Functionally identical states. (Don't send on next connection)
        case KMsvSendStateUponRequest:
        case KMsvSendStateSuspended:
            res = R_SMTP_OUTBOX_STATUS_SUSPENDED;
            break;

        // Functionally identical states. (Send on next connection)
        case KMsvSendStateWaiting:
        case KMsvSendStateResend:
        case KMsvSendStateScheduled:
            res = R_SMTP_OUTBOX_STATUS_QUEUED;
            break;

        // Sending in progress.
        case KMsvSendStateSending:
            res = R_SMTP_OUTBOX_STATUS_SENDING;
            break;

        case KMsvSendStateSent:
            // how these should be handled?
            break;

        // Unknown (!)
        case KMsvSendStateUnknown:
        default:
#ifdef _DEBUG
            User::Panic(KImumMtmUiPanic, ESmtpMtmUdUnknownMsgStatus);
#endif
            break;
            }
        if(!res)
            return HBufC::NewL(0);
        else
            return iCoeEnv->AllocReadResourceL(res);
    }

// ----------------------------------------------------------------------------
// CheckEntry
// ----------------------------------------------------------------------------
TBool CSmtpMtmUiData::CheckEntry(const TMsvEntry& aContext) const
    {
    return ( (aContext.iMtm==KUidMsgTypeSMTP) && (aContext.iType.iUid!=KUidMsvFolderEntryValue) );
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUiData::CreateSkinnedBitmapsL
//
// ----------------------------------------------------------------------------
void CSmtpMtmUiData::CreateSkinnedBitmapsL( TInt aNumZoomStates )
    {
    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    TAknsItemID id;
    CFbsBitmap* bitmap;
    CFbsBitmap* bitmapMask;

    CBitmapArray* array = NULL;

    id.Set( KAknsIIDQgnPropMceMailFetRead );

    array = new(ELeave) CArrayPtrFlat<CFbsBitmap>( aNumZoomStates );
    CleanupStack::PushL( array );

    TParse tp;
    tp.Set( KSmtpMtmUdBitmapFile, &KDC_APP_BITMAP_DIR, NULL );

    AknsUtils::CreateIconL( skins, id, bitmap,
        bitmapMask, tp.FullName(), EMbmMuiuQgn_prop_mce_mail_fet_read,
        EMbmMuiuQgn_prop_mce_mail_fet_read_mask );


    array->AppendL( bitmap );
    array->AppendL( bitmapMask );
    iIconArrays->AppendL( array );

    CleanupStack::Pop();//array
    }

// End of File

