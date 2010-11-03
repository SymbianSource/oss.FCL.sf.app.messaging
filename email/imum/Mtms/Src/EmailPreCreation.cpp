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
*       Class implementation file
*
*/

#include <e32base.h>
#include <eikenv.h>
#include <miuthdr.h>
#include <smtpset.h>
#include <MuiuMsvUiServiceUtilities.h>
#include <SenduiMtmUids.h> // mtm uids
#include <ImumInMailboxServices.h>

#include "ImumMtmLogging.h"
#include "EmailPreCreation.h"
#include "EmailUtils.H"
#include "ImumMboxSettingsUtils.h"

const TMsvId KPreCreatedMsgFolderId = KMsvDraftEntryId;
const TInt KPreCreateMsgCreatorPriority = ( CActive::EPriorityLow + 1 );

// ----------------------------------------------------------------------------
// CEmailPreCreation::NewL()
// ----------------------------------------------------------------------------
//
CEmailPreCreation* CEmailPreCreation::NewL( CImumInternalApi& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CEmailPreCreation::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CEmailPreCreation* me = new ( ELeave ) CEmailPreCreation( aMailboxApi );
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop();
    IMUM_OUT();
    return me;
    }

// ----------------------------------------------------------------------------
// CEmailPreCreation::~CEmailPreCreation()
// ----------------------------------------------------------------------------
//
CEmailPreCreation::~CEmailPreCreation()
    {
    IMUM_CONTEXT( CEmailPreCreation::~CEmailPreCreation, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iEntry;
    iEntry = NULL;
    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CEmailPreCreation::CEmailPreCreation()
// ---------------------------------------------------------------------------
//
CEmailPreCreation::CEmailPreCreation( CImumInternalApi& aMailboxApi )
    :
    iMailboxApi( aMailboxApi )
    {
    IMUM_CONTEXT( CEmailPreCreation::CEmailPreCreation, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
        
    }


// ----------------------------------------------------------------------------
// CEmailPreCreation::PreCreateBlankEmailL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CEmailPreCreation::PreCreateBlankEmailL(
    TMsvId aServiceId,
    TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CEmailPreCreation::PreCreateBlankEmailL, 0, KImumMtmLog );
    IMUM_IN();
    
    const TMsvPartList partList =
        ( KMsvMessagePartBody | KMsvMessagePartAttachments );
    TMsvEmailTypeList type = KMsvEmailTypeListInvisibleMessage;
    if(ServiceUsesMHTMLEncodingL(aServiceId))
        {
        type |= KMsvEmailTypeListMHTMLMessage;
        }
    aStatus = KRequestPending;
    IMUM_OUT();

    return CImEmailOperation::CreateNewL(
        aStatus,
        iEntry->Session(),
        KPreCreatedMsgFolderId,
        aServiceId,
        partList,
        type,
        KUidMsgTypeSMTP,
        KPreCreateMsgCreatorPriority);
    }


// ----------------------------------------------------------------------------
// CEmailPreCreation::DeleteAllPreCreatedEmailsL()
// ----------------------------------------------------------------------------
//
void CEmailPreCreation::DeleteAllPreCreatedEmailsL(
    TMsvId aServiceId )
    {
    IMUM_CONTEXT( CEmailPreCreation::DeleteAllPreCreatedEmailsL, 0, KImumMtmLog );
    IMUM_IN();
    
    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);

    if(iEntry->EntryId() != KPreCreatedMsgFolderId)
        {
        iEntry->SetEntryL(KPreCreatedMsgFolderId);
        }
    // Search children for invisible messages which match service id.
    TInt child = iEntry->Count();
    while(child--)
        {
        const TMsvEntry& entry = (*iEntry)[child];
        if( (!entry.Visible()) && (entry.iServiceId == aServiceId) )
            {
            sel->AppendL(entry.Id());
            }
        }

    if(sel->Count())
        {
        // Do deletion synchronously so that we can guarantee that the
        // Email editor will not be launched with a message which
        // we are about to delete.
        TMsvLocalOperationProgress opProgress;
        iEntry->DeleteL(*sel, opProgress);
        }

    CleanupStack::PopAndDestroy(sel);

    DeleteEmailWithoutServiceL();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CEmailPreCreation::DeleteAllPreCreatedEmailsL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CEmailPreCreation::DeleteAllPreCreatedEmailsL(
    TMsvId aServiceId,
    TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CEmailPreCreation::DeleteAllPreCreatedEmailsL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Do deletion synchronously so that we can guarantee that the
    // Email editor will not be launched with a message which
    // we are about to delete.
    DeleteAllPreCreatedEmailsL( aServiceId );

    CMsvOperation* op = CMsvCompletedOperation::NewL(
        iEntry->Session(), KUidMsvLocalServiceMtm, KNullDesC8,
        KMsvLocalServiceIndexEntryId, aStatus);
    IMUM_OUT();
    return op;
    }


// ----------------------------------------------------------------------------
// CEmailPreCreation::FindPreCreatedEmailL()
// ----------------------------------------------------------------------------
//
TMsvId CEmailPreCreation::FindPreCreatedEmailL(
    TMsvId aServiceId,
    TMsvId aExclude)
    {
    IMUM_CONTEXT( CEmailPreCreation::FindPreCreatedEmailL, 0, KImumMtmLog );
    IMUM_IN();
    
    TMsvId ret = KMsvNullIndexEntryId;
    if(iEntry->EntryId() != KPreCreatedMsgFolderId)
        {
        iEntry->SetEntryL(KPreCreatedMsgFolderId);
        }
    // Search children for invisible messages which match service id.
    TInt child = iEntry->Count();
    while(child--)
        {
        TMsvEntry entry = (*iEntry)[child];
        if( (!entry.Visible()) &&
            (!entry.InPreparation()) &&
            (entry.iServiceId == aServiceId) &&
            (entry.Id() != aExclude) )
            {
            ret = entry.Id();
            // set InPreparation flag
            iEntry->SetEntryL( ret );
            entry.SetInPreparation( ETrue );
            iEntry->ChangeL( entry );
            break;
            }
        }
    IMUM_OUT();
    return ret;
    }


// ----------------------------------------------------------------------------
// CEmailPreCreation::ConstructL()
// ----------------------------------------------------------------------------
//
void CEmailPreCreation::ConstructL()
    {
    IMUM_CONTEXT( CEmailPreCreation::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    TMsvSelectionOrdering ordering;
    ordering.SetShowInvisibleEntries(ETrue);
    iEntry = CMsvEntry::NewL(
        iMailboxApi.MsvSession(), KPreCreatedMsgFolderId, ordering );
    IMUM_OUT();
    }


// ----------------------------------------------------------------------------
// CEmailPreCreation::ServiceUsesMHTMLEncodingL()
// ----------------------------------------------------------------------------
//
TBool CEmailPreCreation::ServiceUsesMHTMLEncodingL( TMsvId aServiceId )
    {
    IMUM_CONTEXT( CEmailPreCreation::ServiceUsesMHTMLEncodingL, 0, KImumMtmLog );
    IMUM_IN();
    
    CImumInSettingsData* accountsettings = 
        iMailboxApi.MailboxServicesL().LoadMailboxSettingsL(
            aServiceId );
    CleanupStack::PushL( accountsettings );
    
    TInt encoding = ImumMboxSettingsUtils::QuickGetL<TInt32>( 
        *accountsettings, 
        TImumInSettings::EKeyDefaultMsgCharSet );

    CleanupStack::PopAndDestroy( accountsettings );
    accountsettings = NULL;
    IMUM_OUT();

    return ( encoding == TImumInSettings::EValueEncodingMHtmlAsMime ) ||
           ( encoding == TImumInSettings::EValueEncodingMHtmlAltAsMime );
    }


// ----------------------------------------------------------------------------
// CEmailPreCreation::DeleteEmailWithoutServiceL()
// ----------------------------------------------------------------------------
//
void CEmailPreCreation::DeleteEmailWithoutServiceL()
    {
    IMUM_CONTEXT( CEmailPreCreation::DeleteEmailWithoutServiceL, 0, KImumMtmLog );
    IMUM_IN();
    
    CMsvEntrySelection* smtpServices =
        MsvUiServiceUtilities::GetListOfAccountsWithMTML(
            iEntry->Session(),
            KSenduiMtmSmtpUid,
            ETrue );
    CleanupStack::PushL( smtpServices );

    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( sel );

    if(iEntry->EntryId() != KPreCreatedMsgFolderId)
        {
        iEntry->SetEntryL( KPreCreatedMsgFolderId );
        }
    // Search children for invisible email messages which service not found
    TInt child = iEntry->Count();
    while(child--)
        {
        const TMsvEntry& entry = (*iEntry)[child];
        if( !entry.Visible() &&
            entry.iMtm == KSenduiMtmSmtpUid &&
            smtpServices->Find( entry.iServiceId ) == KErrNotFound )
            {
            sel->AppendL(entry.Id());
            }
        }

    if(sel->Count())
        {
        // Do deletion synchronously so that we can guarantee that the
        // Email editor will not be launched with a message which we
        // are about to delete.
        TMsvLocalOperationProgress opProgress;
        iEntry->DeleteL(*sel, opProgress);
        }

    CleanupStack::PopAndDestroy( 2, smtpServices ); // CSI: 47 # sel, smtpServices.
    IMUM_OUT();
    }

// End of File

