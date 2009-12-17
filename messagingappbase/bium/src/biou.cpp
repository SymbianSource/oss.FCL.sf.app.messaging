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
* Description:  biou implementation
*
*/




#include "biou.h"       // Class declarations
#include "bium.pan"     // Panic codes
#include "bium.hrh"     // Resource Ids

#include <bium.rsg>     // Resources
#include <apgcli.h>
#include <eikon.hrh>
#include <mtmuidef.hrh>
#include <MTMStore.h>
#include <msvuids.h>    // KUidMsvLocalServiceMtm
#include <MtmExtendedCapabilities.hrh>  // for KMtmUiNewMessageAction...
#include <Muiumsginfo.h>
#include <Muiumsginfo.hrh>
#include <smuthdr.h>
#include <MuiuMsvUiServiceUtilities.h>
#include <mmsvattachmentmanagersync.h>
#include <mmsvattachmentmanager.h>
#include <biouids.h>    // KUidBIOMessageTypeMtm...
#include <bif.h>        // KUidBioUseDefaultApp, KUidBioUseNoApp
#include <MsgBioUids.h> // KMsgBioUidPictureMsg
#include <aknnotewrappers.h> // CAknInformationNote
#include <StringLoader.h> // StringLoader
#include <mmsgbiocontrol.h>
#include <textresolver.h>
#include <gmsModel.h>


// Data types
class TBioData
    {
public:
    TUid iTypeUid;
    TBool iHasParser;
    TUid iAppUid;
    TFileName iEditorFileName;
    };

// Constants
_LIT(KBiouMtmUiResourceFile,"bium");
const TInt KParseAndEditOpPriority = CActive::EPriorityStandard;
const TInt KArrayGranularity = 8;

const TInt KBiumParserWappLeaveFirst = -617;
const TInt KBiumParserWappLeaveLast  = -601;

// this should always be same as KBspInvalidMessage in bsp.h
const TInt KBiumParserBspLeave1 = -500;

// this should always be same as KBspSmartMessageInvalidToken in bsp.h
const TInt KBiumParserBspLeave2 = -501;

const TUid KBiouMtmUiSMSViewerAppUid={0x100058BD};      // Uid for SMS Viewer application

// This is cloned from smuthdr.cpp, KUidMsvSMSHeaderStream
const TUid KBiumUidMsvSMSHeaderStream       = {0x10001834};

// maximum amount of characters used for GSM message subject
const TInt KMaxSubjectLength = 30;

const TInt KTypeStringLength = 32;

// Reserve 3 chars for ' ', '<' and '>'.
const TInt KCharacterReserve = 3;

_LIT(KNotDefinedBium,"@todo Not defined. BIUM.");

GLDEF_C void Panic(TBioUiPanic aPanic)
    {
    _LIT(KPanicName,"BioUi");
    User::Panic(KPanicName, aPanic);
    }


// -----------------------------------------------------------------------------
// CBioMtmUi::NewL
// -----------------------------------------------------------------------------
//
CBioMtmUi* CBioMtmUi::NewL(CBaseMtm& aBaseMtm, 
	CRegisteredMtmDll& aRegisteredMtmDll)
    {
    CBioMtmUi* self=new(ELeave) CBioMtmUi(aBaseMtm, aRegisteredMtmDll);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::CBioMtmUi
// -----------------------------------------------------------------------------
//
CBioMtmUi::CBioMtmUi(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll)
    :   CBaseMtmUi(aBaseMtm, aRegisteredMtmDll)
    {
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::ConstructL
// -----------------------------------------------------------------------------
//
void CBioMtmUi::ConstructL()
    {
    // --- Make sure base class correctly constructed ---
    CBaseMtmUi::ConstructL();

    // For Crystal 6.0, msg editors are  standalone.
    // Don't run them embedded, and don't wait for them to  exit.
    iFlags=EMtmUiFlagEditorNoWaitForExit;

    // Read in data from BIF
    ResetAndLoadBioDataL();

    iErrorResolver = CTextResolver::NewL(*iCoeEnv);

    iBifObserver=CBifChangeObserver::NewL(*this,iCoeEnv->FsSession());
    iBifObserver->Start();
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::~CBioMtmUi
// -----------------------------------------------------------------------------
//
CBioMtmUi::~CBioMtmUi()
    {
    delete iBioData;
    delete iErrorResolver;
    if(iBifObserver)
        {
        iBifObserver->Cancel();
        }
    delete iBifObserver;
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::ResetAndLoadBioDataL
// -----------------------------------------------------------------------------
//
void CBioMtmUi::ResetAndLoadBioDataL()
    {
    // Reset
    delete iBioData;
    iBioData = NULL;

    // --- Load in only the data we need from BioDatabase ---
    iBioData = new(ELeave) CArrayFixFlat<TBioData>(KArrayGranularity);
    CBIODatabase* bioDatabase = CBIODatabase::NewLC(iCoeEnv->FsSession());
    TBioData bioData;

    const TInt bifCount = bioDatabase->BIOCount();
    iBioData->SetReserveL(bifCount); // this can be left out
    for(TInt bifIndex = 0;bifIndex<bifCount;bifIndex++)
        {
        bioData.iTypeUid = (bioDatabase->BifReader(bifIndex)).MessageTypeUid();
        bioData.iHasParser = ( 
            (bioDatabase->BifReader(bifIndex)).MessageParserName().Length() > 0 ) ? 
                ETrue : 
                EFalse;
        bioData.iAppUid = (bioDatabase->BifReader(bifIndex)).MessageAppUid();
         // Only resolve filename when we have to...
        bioData.iEditorFileName = KNullDesC;
        iBioData->AppendL(bioData);
        }

    CleanupStack::PopAndDestroy(bioDatabase);      // bioDatabase
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::HandleBifChangeL
// from MBifChangeObserver
// -----------------------------------------------------------------------------
//
void CBioMtmUi::HandleBifChangeL(TBifChangeEvent /*aEvent*/, TUid /*aBioId*/)
    {
    ResetAndLoadBioDataL();
    }

//
// Entry creation
//
// Not supported as BIO messages can not be sent.
CMsvOperation* CBioMtmUi::CreateL(const TMsvEntry& /*aEntry*/, 
	CMsvEntry& /*aParent*/, TRequestStatus& /*aStatus*/)
    {
    User::Leave(KErrNotSupported);
    return NULL; // Execution should not get here. This keeps compiler happy.
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::OpenL
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::OpenL(TRequestStatus& aStatus)
    {
    //
    // Validate context
    CheckEntryL(BaseMtm().Entry().Entry());

    //
    // Context must be a BIO message, so view it..
    return ViewL(aStatus);
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::OpenL
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::OpenL(
    TRequestStatus& aStatus, const CMsvEntrySelection& aSelection)
    {
    // Set up context as 1st message in selection
    BaseMtm().SwitchCurrentEntryL(aSelection.At(0));        
    return OpenL(aStatus);          // View message
    }


// -----------------------------------------------------------------------------
// CBioMtmUi::CloseL
// Not supported as you can only close services and BIO does not have services.
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::CloseL(TRequestStatus& /*aStatus*/)
    {
    User::Leave(KErrNotSupported);
    // Execution should not get here. This keeps compiler happy.
    return NULL;       
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::CloseL
// Not supported as you can only close services and BIO does not have services.
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::CloseL(
    TRequestStatus& /*aStatus*/, const CMsvEntrySelection& /*aSelection*/)
    {
    User::Leave(KErrNotSupported);
    // Execution should not get here. This keeps compiler happy.
    return NULL;
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::EditL
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::EditL(TRequestStatus& aStatus)
    {
    // Will leave if context is not a BIO message
    CheckEntryL(BaseMtm().Entry().Entry());
    return LaunchApplicationL(aStatus,EFalse);
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::EditL
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::EditL(
    TRequestStatus& aStatus, const CMsvEntrySelection& aSelection)
    {
    // Set up context as 1st message in selection
    BaseMtm().SwitchCurrentEntryL(aSelection.At(0));
    return EditL(aStatus);
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::ViewL
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::ViewL(TRequestStatus& aStatus)
    {
    // Will leave if context is not a BIO message
    CheckEntryL(BaseMtm().Entry().Entry());
    return LaunchApplicationL(aStatus,ETrue);
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::ViewL
// Views only first in the selection
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::ViewL(
    TRequestStatus& aStatus,const CMsvEntrySelection& aSelection)
    {
    // Set up context as 1st message in selection    
    BaseMtm().SwitchCurrentEntryL(aSelection.At(0));
    return ViewL(aStatus);
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::CancelL
// Currently this function only used to suspend messages queued for sending.
// Can't send BIO messages, so can't cancel them either.
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::CancelL(
    TRequestStatus& /*aStatus*/, const CMsvEntrySelection& /*aSelection*/)
    {
    User::Leave(KErrNotSupported);
    // Execution should not get here. This keeps compiler happy.    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::CopyToL
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::CopyToL(const CMsvEntrySelection& /*aSelection*/, 
	TRequestStatus& /*aStatus*/)
    {
    // Context should be MTM folder/service to copy to
    User::Leave(KErrNotSupported);
    // Execution should not get here. This keeps compiler happy.    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::MoveToL
// Not supported as BIO folders are not supported.
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::MoveToL(const CMsvEntrySelection& /*aSelection*/, 
	TRequestStatus& /*aStatus*/)
    {
    // Context should be MTM folder/service to move to
    User::Leave(KErrNotSupported);
    // Execution should not get here. This keeps compiler happy.
    return NULL;
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::CopyFromL
// Not supported as BIO folders are not supported.
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::CopyFromL(const CMsvEntrySelection& /*aSelection*/, 
	TMsvId /*aTargetId*/, TRequestStatus& /*aStatus*/)
    {
    User::Leave(KErrNotSupported);
    return NULL; // Execution should not get here. This keeps compiler happy.
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::MoveFromL
// Not supported as BIO folders are not supported.
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::MoveFromL(const CMsvEntrySelection& /*aSelection*/, 
	TMsvId /*aTargetId*/, TRequestStatus& /*aStatus*/)
    {
    User::Leave(KErrNotSupported);
    return NULL; // Execution should not get here. This keeps compiler happy.
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::ReplyL
// Not supported as BIO does not support sending of messages
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::ReplyL(TMsvId /*aDestination*/, 
	TMsvPartList /*aPartlist*/, TRequestStatus& /*aCompletionStatus*/)
    {
    User::Leave(KErrNotSupported);
    return NULL; // Execution should not get here. This keeps compiler happy.
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::ForwardL
// Not supported as BIO does not support sending of messages
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::ForwardL(TMsvId /*aDestination*/, 
	TMsvPartList /*aPartList*/, TRequestStatus& /*aCompletionStatus*/)
    {
    User::Leave(KErrNotSupported);
    return NULL;        // Execution should not get here. This keeps compiler happy.
    }


// -----------------------------------------------------------------------------
// CBioMtmUi::DeleteFromL
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::DeleteFromL(const CMsvEntrySelection& /*aSelection*/,
	TRequestStatus& /*aStatus*/)
    {
    User::Leave(KErrNotSupported);
    return NULL; // Execution should not get here. This keeps compiler happy.
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::DeleteServiceL
// Bio services not supported
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::DeleteServiceL(const TMsvEntry& /*aService*/, 
	TRequestStatus& /*aStatus*/)
    {
    User::Leave(KErrNotSupported);
    return NULL; // Execution should not get here. This keeps compiler happy.
    }


// -----------------------------------------------------------------------------
// CBioMtmUi::QueryCapability
// RTTI functions
// -----------------------------------------------------------------------------
//
TInt CBioMtmUi::QueryCapability(TUid aCapability, TInt& aResponse)
    {
    if (aCapability.iUid == KUidMsvMtmQueryMessageInfo )
        {
        aResponse = ETrue;
        return KErrNone;
        }
    return CBaseMtmUi::QueryCapability(aCapability, aResponse);
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::InvokeAsyncFunctionL
// -----------------------------------------------------------------------------
//
// RTTI functions
CMsvOperation* CBioMtmUi::InvokeAsyncFunctionL(TInt aFunctionId, 
	const CMsvEntrySelection& aSelection, TRequestStatus& aCompletionStatus,
    TDes8& aParameter)
    {
    if(aFunctionId == KMtmUiFunctionMessageInfo)
        return ShowMessageInfoL(aCompletionStatus, aParameter);

    return CBaseMtmUi::InvokeAsyncFunctionL(
    	aFunctionId, aSelection, aCompletionStatus, aParameter);
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::ShowMessageInfoL
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::ShowMessageInfoL(TRequestStatus& aCompletionStatus,
	TDes8& /*aParameter*/)
//
// gathers the data to populate the info data array from the mtm and then
// uses the messge info dialog class to display the data;
// only used for viewing bio messages, the smum mtm is actually used for
// creating bio messages
//
    {
    BaseMtm().LoadMessageL();
    const TMsvEntry& tentry=BaseMtm().Entry().Entry();

    TMsgInfoMessageInfoData infoData;

    //show subject if GMS message (picture message)
    if (tentry.iBioType == KMsgBioUidPictureMsg.iUid)
        {
        HBufC* gmsTitle = ExtractTitleFromGMSBodyL(BaseMtm().Body(),
            KMaxSubjectLength);
        CleanupStack::PushL(gmsTitle);
        infoData.iSubject.Set(*gmsTitle);
        }

    //date
    infoData.iDateTime = tentry.iDate;

    //type
    TInt resourceId( KErrNotFound );

    if ( tentry.iBioType == KMsgBioUidVCard.iUid ||
         tentry.iBioType == KMsgBioUidCompBusCard.iUid )
        {
        resourceId = R_QTN_SM_INFO_BUSINESS_CARD;
        }
    else if ( tentry.iBioType == KMsgBioUidVCalendar.iUid )
        {
        resourceId = R_QTN_SM_INFO_CALENDAR;
        }
    else if ( tentry.iBioType == KMsgBioUidIAC.iUid ||
              tentry.iBioType == KMsgBioUidWmlBrSettings.iUid ||
              tentry.iBioType == KMsgBioUidWVSettings.iUid )
        {
        resourceId = R_QTN_SM_TITLE_CONFIGURATION;
        }
    else if ( tentry.iBioType == KMsgBioUidPictureMsg.iUid )
        {
        resourceId = R_QTN_SM_INFO_GMS;
        }
    else if ( tentry.iBioType == KMsgBioUidEmailNotif.iUid )
        {
        resourceId = R_QTN_SM_INFO_EMAIL_NOTIFICATION;
        }
    else if ( tentry.iBioType == KMsgBioUidRingingTone.iUid )
        {
        resourceId = R_QTN_SM_INFO_RINGING_TONE;
        }
    else if ( tentry.iBioType == KMsgBioUidOperatorLogo.iUid )
        {
        resourceId = R_QTN_SM_INFO_OPERATOR_LOGO;
        }
    else if ( tentry.iBioType == KMsgBioUidSyncML.iUid )
        {
        resourceId = R_QTN_SM_INFO_SYNC_PROFILE;
        }
    else
        {
        // Don't change the resourceId
        }

    TBuf<KTypeStringLength> typeString;
    if ( resourceId != KErrNotFound )
        {
        HBufC* type = iEikonEnv->AllocReadResourceLC( resourceId );
        typeString = *type;
        infoData.iType.Set( typeString );
        CleanupStack::PopAndDestroy( type );
        }
    else
        {
        infoData.iType.Set(tentry.iDescription);
        }

    //from
    HBufC* from = CreateFromTextLC(BaseMtm().Entry());
    infoData.iFrom.Set(*from);

    CMsgInfoMessageInfoDialog* infoDialog = CMsgInfoMessageInfoDialog::NewL();
    infoDialog->ExecuteLD(infoData,CMsgInfoMessageInfoDialog::EBiosViewer);
    CleanupStack::PopAndDestroy(from);
    if (tentry.iBioType == KMsgBioUidPictureMsg.iUid)
        {
        CleanupStack::PopAndDestroy(); //gmsTitle
        }
    return CMsvCompletedOperation::NewL(Session(), KUidMsvLocalServiceMtm, 
    	KNullDesC8, KMsvLocalServiceIndexEntryId, aCompletionStatus, KErrNone);
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::ExtractTitleFromGMSBodyL
// -----------------------------------------------------------------------------
//
HBufC* CBioMtmUi::ExtractTitleFromGMSBodyL(const CRichText& aBody, 
	TInt aMaxLength)
    {
    CGmsModel* model = CGmsModel::NewL(CCoeEnv::Static()->FsSession(), 
    	KErrCorrupt);
    CleanupStack::PushL(model);
    model->ImportGMSL(aBody);
    HBufC* text = model->TextAsHBufC16LC();
    HBufC* result = NULL;
    if (text->Length() <= aMaxLength)
        {
        result = text->AllocL();
        }
    else
        {
        // length is greater than max
        result = text->Left(aMaxLength).AllocL();
        }
    CleanupStack::PopAndDestroy(text);
    CleanupStack::PopAndDestroy(model);
    return result;
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::ShowInvalidMsgNote
// -----------------------------------------------------------------------------
//
void CBioMtmUi::ShowInvalidMsgNote() const
    {
    TRAP_IGNORE(
        {
        CAknInformationNote* note = new (ELeave) CAknInformationNote;
        HBufC* text =
            StringLoader::LoadLC(R_QTN_MCE_INFO_MESSAGE_NOT_OPENED);
        note->ExecuteLD(*text);
        CleanupStack::PopAndDestroy(text);
        } );
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::ShowSystemErrorNote
// -----------------------------------------------------------------------------
//
void CBioMtmUi::ShowSystemErrorNote(TInt aError) const
    {
    TRAP_IGNORE(
        {
        const TDesC& error = iErrorResolver->ResolveErrorString( aError );
        CAknErrorNote* note = new (ELeave) CAknErrorNote();
        note->ExecuteLD(error);
        } );
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::InvokeSyncFunctionL
// -----------------------------------------------------------------------------
//
void CBioMtmUi::InvokeSyncFunctionL(TInt aFunctionId, 
	const CMsvEntrySelection& aSelection, TDes8& aParameter)
    {
    //
    // Delegate to client MTM
    CBaseMtmUi::InvokeSyncFunctionL(aFunctionId, aSelection, aParameter);
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::GetResourceFileName
// Utility
// -----------------------------------------------------------------------------
//
void CBioMtmUi::GetResourceFileName(TFileName& aFileName) const
    {
    aFileName=KBiouMtmUiResourceFile;
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::GetProgress
// Progress interpretation
// -----------------------------------------------------------------------------
//
TInt CBioMtmUi::GetProgress(
    const TDesC8& aProgress, 
    TBuf<EProgressStringMaxLen>& aReturnString, 
    TInt& aTotalEntryCount, TInt& aEntriesDone,
    TInt& aCurrentEntrySize, TInt& aCurrentBytesTrans) const
    {
    TInt error=KErrNone;

    //
    // Intialise return data...
    aReturnString.Zero();
    aReturnString.Append(KNotDefinedBium);
    aTotalEntryCount=0;
    aEntriesDone=0;
    aCurrentEntrySize=0;
    aCurrentBytesTrans=0;

    if (!aProgress.Length())
        return KErrNone;

    const TInt progressType=ProgressType(aProgress);

    // Handle any extra Engine/Local specific stuff
    if(IsLocalProgress(progressType))
        {
        //
        // Unpack the progress
        TPckgBuf<TBioUiProgress> paramPack;
        paramPack.Copy(aProgress);
        TBioUiProgress progress(paramPack());

        error=progress.iError;
        }
    else
        {
        //
        // Unpack the progress
        TPckgBuf<TBioProgress> paramPack;
        paramPack.Copy(aProgress);
        TBioProgress progress(paramPack());

        error=progress.iErrorCode;
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::ProgressType
// -----------------------------------------------------------------------------
//
// Decode progress type from progress package
TInt CBioMtmUi::ProgressType(const TDesC8& aProgress) const
    {
    TPckgC<TInt> type(0); // Progress type is the first TInt in the buffer.
    type.Set(aProgress.Left(sizeof(TInt)));
    const TInt progressType = type();

    __ASSERT_DEBUG( IsEngineProgress(progressType) || 
        IsLocalProgress(progressType), Panic(EBioMtmUiUnknownOperation));

    return progressType;
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::IsLocalProgress
// Check if progress is local engine side
// -----------------------------------------------------------------------------
//
TBool CBioMtmUi::IsLocalProgress(TInt aProgressType) const
    {
    return(aProgressType==TBioUiProgress::EBioMtmUiEditing ||
           aProgressType==TBioUiProgress::EBioMtmUiParseAndEdit);
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::IsEngineProgress
// Check if progress is from engine side
// (ie. if it is of type TBioUiProgress)
// -----------------------------------------------------------------------------
//
TBool CBioMtmUi::IsEngineProgress(TInt aProgressType) const
    {
    return(aProgressType==TBioProgress::EBiosWaiting ||
           aProgressType==TBioProgress::EBiosCreating ||
           aProgressType==TBioProgress::EBiosParsing ||
           aProgressType==TBioProgress::EBiosProcessing);
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::DisplayProgressSummary
// Progress interpretation and display
// -----------------------------------------------------------------------------
//
TInt CBioMtmUi::DisplayProgressSummary(const TDesC8& aProgress) const
    {
    TInt err = KErrNone;

    if (!aProgress.Length())
        return KErrNone;

    const TInt progressType=ProgressType(aProgress);

    if(IsEngineProgress(progressType))
        {
        //
        // Handle Engine side progress
        TPckgBuf<TBioProgress> paramPack;
        paramPack.Copy(aProgress);
        TBioProgress progress(paramPack());
        err = progress.iErrorCode;
        if (err == KErrNone)
            {
            return KErrNone;
            }

        // These are the leave codes that originate from the symbian
        // parsers and one other parser.
        if (err == KErrMsgBioMessageNotValid
            || (KBiumParserWappLeaveFirst <= err && err <= KBiumParserWappLeaveLast)
            || err == KBiumParserBspLeave1
            || err == KBiumParserBspLeave2)
            {
            ShowInvalidMsgNote();
            }
        else
            {
            ShowSystemErrorNote(err);
            }
        }
    else
        {
        //
        // Handle UI side progress
        TPckgBuf<TBioUiProgress> paramPack;
        paramPack.Copy(aProgress);
        TBioUiProgress progress(paramPack());
        err = progress.iError;
        if (err == KErrNone)
            {
            return KErrNone;
            }
        if (err == KErrMsgBioMessageNotValid)
            {
            ShowInvalidMsgNote();
            }
        else if( err != KLeaveWithoutAlert )
            {
            ShowSystemErrorNote(err);
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::LaunchApplicationL
// Utility specific to CBioMtmUi
// -----------------------------------------------------------------------------
//
CMsvOperation* CBioMtmUi::LaunchApplicationL(
    TRequestStatus& aStatus,TBool aReadOnly )
    {
    const TMsvEntry& tentry=BaseMtm().Entry().Entry();

    // Locate BioData for given bio message type
    TUid msgTypeUid={tentry.iBioType};
    const TInt bioDataCount=iBioData->Count();

    TBioData* bioData=NULL;


    for(TInt n=0;n<bioDataCount;n++)
        {
        if((iBioData->At(n)).iTypeUid==msgTypeUid)
            {
            bioData=&(iBioData->At(n));
            break;
            }
        }

    // If we have no data for this bio message type then complete.
    if(!bioData)
        {
        //the TBioUiProgress transmits the error code to
        //DisplayProgressSummary(..) to be handled there
        TBioUiProgress progress;
        progress.iType=TBioUiProgress::EBioMtmUiParseAndEdit;
        progress.iError=KErrMsgBioMessageNotValid;
        TPckgBuf<TBioUiProgress> progressPckg(progress);
        
        // Mark entry as read
        CMsvEntry* entry = Session().GetEntryL(tentry.Id());
        CleanupStack::PushL(entry);
        TMsvEntry tEntry(entry->Entry());
        tEntry.SetUnread(EFalse);
        tEntry.SetNew(EFalse);
        entry->ChangeL(tEntry);
        CleanupStack::PopAndDestroy(entry);
        return CMsvCompletedOperation::NewL(
        	Session(), 
        	Type(), 
        	progressPckg, 
        	KMsvLocalServiceIndexEntryId, 
        	aStatus, KErrNone);
        }

    // If requested to launch default app, then set correct uid
    if(bioData->iAppUid == KUidBioUseDefaultApp)
        {
        bioData->iAppUid = KBiouMtmUiSMSViewerAppUid;
        }

    // If app filename has not been resolved AND we need to resolve it, 
    // then try to resolve it now
    if(bioData->iEditorFileName==KNullDesC && 
    	bioData->iAppUid!=KUidBioUseNoApp)
        {
        RApaLsSession appList;
        User::LeaveIfError(appList.Connect());
        CleanupClosePushL(appList);
        TApaAppInfo info;
        TInt err = appList.GetAppInfo(info,bioData->iAppUid);
        CleanupStack::PopAndDestroy();                      // appList
        if(err!=KErrNone)
            {
            TBufC8<1> buf;
            // Mark as read
            CMsvEntry* entry = Session().GetEntryL(tentry.Id());
            CleanupStack::PushL(entry);
            TMsvEntry tEntry(entry->Entry());
            tEntry.SetUnread(EFalse);
            tEntry.SetNew(EFalse);
            entry->ChangeL(tEntry);
            CleanupStack::PopAndDestroy(entry);
            return CMsvCompletedOperation::NewL(
            	Session(), 
            	Type(), 
            	buf, 
            	KMsvLocalServiceIndexEntryId, 
            	aStatus, KErrNone);
            }
        bioData->iEditorFileName = info.iFullName;
        }

    if ( tentry.iMtmData3 == 0 ) //Entry not parsed yet
		{
		if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL( 
			Session(),
			tentry.iSize ) )
			{
			User::Leave( KErrDiskFull );
			}
		}

    //
    // Parse message (if needed) then launch application (if needed)...
    //
    TBuf8<1> blankParams;
    CMsvEntrySelection* sel=new(ELeave) CMsvEntrySelection();
    CleanupStack::PushL(sel);
    // Message Server will use UID of 1st message to load in MTM server.
    sel->AppendL(tentry.Id());
    // BIO Server MTM will use 2nd message to actually parse.
    sel->AppendL(tentry.Id());

    //
    // Create parameters for parseAndEdit operation
    TParseAndEditParams params;
    if(bioData->iAppUid==KUidBioUseNoApp)
        {
        // If we have not been asked to launch an app
        params.iLaunchApp=EFalse;
        // Set the entry id also here. Needed in SetEntryReadAndOldL.
        params.iEditorParams.iId=tentry.Id();
        }
    else
        {
        params.iLaunchApp=ETrue;
        params.iPreferEmbedded = (
            Preferences() & EMtmUiFlagEditorPreferEmbedded );
        params.iEditorFileName=bioData->iEditorFileName;
        // Kick off app with current context id
        params.iEditorParams.iId=tentry.Id();
        params.iEditorParams.iFlags |= ( 
            Preferences() & 
            EMtmUiFlagEditorPreferEmbedded ? 
            EMsgLaunchEditorEmbedded :
            0 );
            
        params.iEditorParams.iFlags |= ( 
            Preferences() & EMtmUiFlagEditorNoWaitForExit ? 
            0 : 
            EMsgLaunchEditorThenWait );
            
        params.iEditorParams.iSpecialAppId = bioData->iAppUid;        
        if(aReadOnly)
            params.iEditorParams.iFlags|=EMsgReadOnly;
        }

    // Create ParseAndEdit operation
    CParseAndEditOp* parseAndEdit = 
        CParseAndEditOp::NewL( Session(), aStatus, params );
    CleanupStack::PushL( parseAndEdit );

    //
    // Create Parse or ParseAndProcess operation, 
    // for giving to ParseAndEdit operation
    // Check that message is of correct type for calling Bio Server MTM,
    // if it's not then there is not much we can do 
    // (so create a completed operation).
    // This condition also ensures that we do not parse messages 
    // which are due for sending.
    CMsvOperation* parseOp;
	if(tentry.iMtm==KUidBIOMessageTypeMtm && bioData->iHasParser)
        {
        if(params.iLaunchApp)
            parseOp=CBaseMtmUi::InvokeAsyncFunctionL(KBiosMtmParse,
                *sel,parseAndEdit->RequestStatus(),blankParams);
        else
            parseOp=CBaseMtmUi::InvokeAsyncFunctionL(KBiosMtmParseThenProcess,
                *sel,parseAndEdit->RequestStatus(),blankParams);
        }
    else
        {
        //the TBioUiProgress transmits the error code to
        //DisplayProgressSummary(..) to be handled there
        TBioUiProgress progress;
        if(bioData->iEditorFileName==KNullDesC
            && bioData->iAppUid==KUidBioUseNoApp
            && ( ! bioData->iHasParser )
            && bioData->iTypeUid != KUidBioUseNoApp)
            {
            progress.iError=KErrMsgBioMessageNotValid;
            }
         else
            {
            progress.iError=KErrNone;
            }
         progress.iType=TBioUiProgress::EBioMtmUiParseAndEdit;
         TPckgBuf<TBioUiProgress> progressPckg(progress);
         parseOp = CMsvCompletedOperation::NewL(
         Session(), 
         KUidBIOMessageTypeMtm, 
         progressPckg, 
         KMsvNullIndexEntryId, 
         parseAndEdit->RequestStatus() );
        }
    
    // Pass 'parse' operation to 'parse and edit' operation
    parseAndEdit->SetParseOpL(parseOp);

    CleanupStack::Pop(parseAndEdit); // parseAndEdit
    CleanupStack::PopAndDestroy(sel); // sel

    return parseAndEdit;
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::CheckEntryL
// Entry validation
// Leaves with KErrNotSupported if entry is not a message of the BIO MTM.
// -----------------------------------------------------------------------------
//
void CBioMtmUi::CheckEntryL(const TMsvEntry& aEntry) const
    {
    // Note: Sending Bio messages will have Mtm type of 
    //      the transport type for sending.
    // Instead Sending Bio messages will have iBioType as non-zero.

    if(aEntry.iType.iUid!=KUidMsvMessageEntryValue ||
      (aEntry.iBioType==0 && aEntry.iMtm!=KUidBIOMessageTypeMtm))
        User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CBioMtmUi::CreateFromTextLC
// Both name and number are shown if they are known.
// Otherwise only the number is shown.
// Number is always known, except for IR & BT.
// -----------------------------------------------------------------------------
//
HBufC* CBioMtmUi::CreateFromTextLC(CMsvEntry& aEntry) const
    {
    CMsvStore* store = aEntry.ReadStoreL();
    CleanupStack::PushL(store);
    TBool smsBased;
    HBufC* toFromSmsBuf;
    if (store->IsPresentL(KBiumUidMsvSMSHeaderStream))
        {
        smsBased = ETrue;
        CPlainText* nullString = CPlainText::NewL();
        CleanupStack::PushL(nullString);
        CSmsHeader* smsHeader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *nullString);
        CleanupStack::PushL(smsHeader);
        smsHeader->RestoreL(*store);
        toFromSmsBuf = smsHeader->Message().SmsPDU().ToFromAddress().AllocL();
        CleanupStack::PopAndDestroy(2, nullString); // smsHeader nullString
        }
    else
        {
        smsBased = EFalse;
        toFromSmsBuf = KNullDesC().AllocL();
        }
    CleanupStack::PopAndDestroy(store);
    CleanupStack::PushL(toFromSmsBuf);
    HBufC* result;
    TPtrC details(aEntry.Entry().iDetails);
    if (smsBased)
        {
        TPtr toFromSmsPtr(toFromSmsBuf->Des());
        toFromSmsPtr.Trim();

        // Does the iDetails have a name or number? Do a comparison in order to
        // find out.
        TBool detailsIsNumber = details.Compare(toFromSmsPtr) == 0;
        if (detailsIsNumber)
            {
            // ...which means that the name was not known when the message arrived.
            // We're not going to fetch it now.
            // Show the number.
            result = details.AllocL();
            }
        else
            {
            // The iDetails contains the matched name. Concatenate the name and
            // toFromSmsBuf which contains the number.
            result = HBufC::NewL(
                details.Length() + KCharacterReserve + toFromSmsBuf->Length());
            TPtr resultPtr(result->Des());
            _LIT(KBiumChSpace, " ");
            _LIT(KBiumChLeft, "<");
            _LIT(KBiumChRight, ">");
            resultPtr.Append(details);
            resultPtr.Append(KBiumChSpace);
            resultPtr.Append(KBiumChLeft);
            resultPtr.Append(*toFromSmsBuf);
            resultPtr.Append(KBiumChRight);
            }
        }
    else
        {
        // It is either IR or BT, so use the iDetails text.
        result = aEntry.Entry().iDetails.AllocL();
        }
    CleanupStack::PopAndDestroy(toFromSmsBuf);
    CleanupStack::PushL(result);
    return result;
    }


//******************************************************************************
//******************************************************************************
//
// class CParseAndEditOp
//
// Operation encapsulating the parse operation 
// and the launch application operation.
//
//******************************************************************************
//******************************************************************************

// -----------------------------------------------------------------------------
// CParseAndEditOp::NewL
// Construction
// -----------------------------------------------------------------------------
//
CParseAndEditOp* CParseAndEditOp::NewL(
    CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus,
    const TParseAndEditParams& aParams)
    {
    CParseAndEditOp* self = 
        new(ELeave) CParseAndEditOp(
            aMsvSession,aObserverRequestStatus,aParams );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CParseAndEditOp::CParseAndEditOp
// -----------------------------------------------------------------------------
//
CParseAndEditOp::CParseAndEditOp(
    CMsvSession& aMsvSession,TRequestStatus& aObserverRequestStatus,
    const TParseAndEditParams& aParams)
    : CMsvOperation(aMsvSession, 
        KParseAndEditOpPriority,aObserverRequestStatus),
        iState(EStateIdle),iParams(aParams)
    {
    iLocalProgress.iType=TBioUiProgress::EBioMtmUiParseAndEdit;
    iLocalProgress.iError=KErrNone;

    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CParseAndEditOp::ConstructL
// Construction
// -----------------------------------------------------------------------------
//
void CParseAndEditOp::ConstructL()
    {
    iMtmStore=CMtmStore::NewL(iMsvSession);
    }

// -----------------------------------------------------------------------------
// CParseAndEditOp::~CParseAndEditOp
// Destruction
// -----------------------------------------------------------------------------
//
CParseAndEditOp::~CParseAndEditOp()
    {
    Cancel();
    delete iOperation;
    delete iMtmStore;
    }

// -----------------------------------------------------------------------------
// CParseAndEditOp::ProgressL
// -----------------------------------------------------------------------------
//
const TDesC8& CParseAndEditOp::ProgressL()
    {
    if(iOperation)
        return iOperation->ProgressL();

    iLocalProgressPckg=iLocalProgress;
    return iLocalProgressPckg;
    }

// -----------------------------------------------------------------------------
// CParseAndEditOp::RequestStatus
// Enquiry
// -----------------------------------------------------------------------------
//
TRequestStatus& CParseAndEditOp::RequestStatus()
    {
    return iStatus;
    }

// -----------------------------------------------------------------------------
// CParseAndEditOp::SetParseOpL
// Setup and kick off operation
// -----------------------------------------------------------------------------
//
void CParseAndEditOp::SetParseOpL(CMsvOperation* aOp)
    {
    __ASSERT_DEBUG(aOp, Panic(EBioMtmUiNullOperation));
    __ASSERT_DEBUG(!iOperation, Panic(EBioMtmUiOperationAlreadyExists));
    iOperation = aOp;
    iMtm = iOperation->Mtm();
    iState = EStateParsing;
    SetActive();
    }
    
// -----------------------------------------------------------------------------
// CParseAndEditOp::DoCancel
// from CActive
// -----------------------------------------------------------------------------
//
void CParseAndEditOp::DoCancel()
    {
    if(iOperation)
        iOperation->Cancel();
    CompleteObserver();
    }

// -----------------------------------------------------------------------------
// CParseAndEditOp::RunL
// from CActive
// -----------------------------------------------------------------------------
//
void CParseAndEditOp::RunL()
    {
    __ASSERT_DEBUG(iOperation,Panic(EBioMtmUiOperationDoesNotExist));

    switch(iState)
        {
        case EStateParsing:     // The parse has completed.
            {
            TBuf<CBaseMtmUi::EProgressStringMaxLen> bufNotUsed1;
            TInt intNotUsed2;
            TInt intNotUsed3;
            TInt intNotUsed4;
            TInt intNotUsed5;
            CBaseMtmUi& mtmUi=iMtmStore->GetMtmUiLC(KUidBIOMessageTypeMtm);
            const TInt err=mtmUi.GetProgress(iOperation->FinalProgress(),
                                bufNotUsed1,intNotUsed2,intNotUsed3,
                                intNotUsed4,intNotUsed5);
            CleanupStack::PopAndDestroy(); // (release cleanup item for mtmUi)
            if(KErrNone==err && iParams.iLaunchApp)
                DoEditL();
            else
                {
                SetEntryReadAndOldL(iParams.iEditorParams.iId);
                CompleteObserver();
                }
            }
            break;
        case EStateEditing:     // Edit has completed.
        	DeleteMessageAttachmentL(iParams.iEditorParams.iId);
            CompleteObserver();
            break;

        default:;// To prevent warning enumeration value
        }
    }

// -----------------------------------------------------------------------------
// CParseAndEditOp::RunError
// from CActive
// RunError will get called if our RunL leaves
// -----------------------------------------------------------------------------
//
TInt CParseAndEditOp::RunError(TInt aError)
    {
    delete iOperation; // So that we will report our local progress
    iOperation=NULL;
    iLocalProgress.iError = aError;
    CompleteObserver();
    return KErrNone; // the Bio MTM UI will handle the error
    }

// -----------------------------------------------------------------------------
// CParseAndEditOp::DoEditL
// -----------------------------------------------------------------------------
//
void CParseAndEditOp::DoEditL()
    {
    delete iOperation;
    iOperation=NULL;

    TBioUiProgress progress;
    progress.iType=TBioUiProgress::EBioMtmUiEditing;
    progress.iError=KErrNone;
    TPckgBuf<TBioUiProgress> progressPckg(progress);

    iOperation = MsgEditorLauncher::LaunchEditorApplicationL(
        iMsvSession,
        iMtm,
        iStatus,
        iParams.iEditorParams,
        iParams.iEditorFileName,
        progressPckg);
        
    iState = EStateEditing;
    SetActive();
    }

// -----------------------------------------------------------------------------
// CParseAndEditOp::CompleteObserver
// -----------------------------------------------------------------------------
//
void CParseAndEditOp::CompleteObserver()
    {
    TRequestStatus* status = &iObserverRequestStatus;
    TInt err;
    if (iOperation)
        {
        err = iOperation->iStatus.Int();
        }
    else
        {
        err = iLocalProgress.iError;
        }

    User::RequestComplete(status, err);
    iState = EStateComplete;
    }

// -----------------------------------------------------------------------------
// CParseAndEditOp::SetEntryReadAndOldL
// -----------------------------------------------------------------------------
//
void CParseAndEditOp::SetEntryReadAndOldL(TMsvId aEntryId)
    {
    CMsvEntry* entry = iMsvSession.GetEntryL(aEntryId);
    CleanupStack::PushL(entry);
    TMsvEntry tEntry(entry->Entry());
    tEntry.SetUnread(EFalse);
    tEntry.SetNew(EFalse);
    entry->ChangeL(tEntry);
    CleanupStack::PopAndDestroy(entry);
    }
    
// -----------------------------------------------------------------------------
// CParseAndEditOp::DeleteMessageAttachmentL
// -----------------------------------------------------------------------------
//
void CParseAndEditOp::DeleteMessageAttachmentL(TMsvId aEntryId)
    {
    CMsvEntry* entry = NULL;
    TRAPD(err,entry = iMsvSession.GetEntryL(aEntryId));
    
    if(err == KErrNone)
    	{
		CleanupStack::PushL(entry);  // 1st push    	
	    TMsvEntry tEntry = entry->Entry();
	    TBool readOnly = tEntry.ReadOnly();
	    tEntry.SetReadOnly(EFalse);
	    entry->ChangeL(tEntry);
	    CMsvStore* store = entry->EditStoreL();
	    CleanupStack::PushL(store);  // 2nd push
	    if(store->AttachmentManagerL().AttachmentCount() > 0)
	    	{
		    store->AttachmentManagerExtensionsL().RemoveAttachmentL(0);
	    	store->CommitL();
	    	    	}
	    if(store->IsPresentL(KUidMsvBIODataStream))
	    	{
	    	store->Remove(KUidMsvBIODataStream);
	    	store->CommitL();
	    	}
	    TMsvEntry tEntryNew = entry->Entry();
	    tEntryNew.SetReadOnly(readOnly);
	    tEntryNew.iMtmData3 = 0;  // EBioMsgNotParsed in bioop.h
	    entry->ChangeL(tEntryNew);	
	    CleanupStack::PopAndDestroy(store);
	    CleanupStack::PopAndDestroy(entry);
    	}
    // we ignore error if attachment was not found
    else if(err != KErrNotFound) 
    	{
    	User::LeaveIfError(err);
    	}
    
    }
    

// end of file
