/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Bio control for vCalendar messages
*
*/




// INCLUDE FILES
#include "VCalendarBioControl.h"    // CVCalendarBioControl
#include "VCalBcDataFiller.h"       // CVCalBcDataFiller
#include "VCalLog.h"				// Logging functionality
#include "VCalBcPanics.h"

#include <eikon.hrh>
#include <eikmenup.h>
#include <msvapi.h>
#include <s32file.h>                // RFileReadStream
#include <StringLoader.h>           // StringLoader
#include <msgbiocontrolObserver.h>  // MMsgBioControlObserver
#include <vcalbc.rsg>               // resouce identifiers
#include <CRichBio.h>               // CRichBio
#include <CalenImporter.h>          // CCalenImporter
#include <calentryview.h>           // CCalEntryView
#include <MsgBioUtils.h>
#include <bldvariant.hrh>			// __SERIES60_HELP
#include <csxhelp/smart.hlp.hrh>
#include <featmgr.h>
#include <calsession.h>
#include <mmsvattachmentmanager.h>
#include <sysutil.h>
#include <ecom/ecom.h>
#include <AknDef.h>
#include <AiwServiceHandler.h>          // CAiwServiceHandler
#include <SmsViewer.rsg>
#include <bva.rsg>
#include <AiwCommon.hrh>
#include <data_caging_path_literals.hrh>

// LOCAL CONSTANTS AND MACROS

const TInt KVCalBcLeaveCodeCorrupt = KErrMsgBioMessageNotValid;
const TInt KIndexFirst = 0;
const TInt KLengthBeginVCalendar = 15; // "BEGIN:VCALENDAR" -string length
const TInt KVCalBcHeightReductionBva = 9;
//recognition string for ical
_LIT8(KVersionICal,		"VERSION:2.0");
//recognition string for vcal
_LIT8(KVersionVCal,		"VERSION:1.0");
const TInt KReadDataAmount = 256;


_LIT(KVCalBCResourceFile, "vcalbc.rsc");
_LIT(KAvkonResourceFile, "avkon.rsc");
_LIT(KSmsViewerResourceFile, "smsviewer.rsc");
_LIT(KBvaResourceFile, "bva.rsc");

// MEMBER FUNCTIONS

CVCalendarBioControl::~CVCalendarBioControl()
    {
    FeatureManager::UnInitializeLib();
    iFileHandle.Close();
    delete iViewer;
    iCalEntryArray.ResetAndDestroy();
    delete iCalEntryView;
    delete iSession;
    if( iCalAiwServiceHandler )
        {
        delete iCalAiwServiceHandler;
        iCalAiwServiceHandler = NULL;
        }
    }

EXPORT_C CMsgBioControl* CVCalendarBioControl::NewL(
    MMsgBioControlObserver& aObserver,
    CMsvSession* aSession,
    TMsvId aId,
    TMsgBioMode aEditorOrViewerMode,
    const RFile* aFile)
    {
	LOG("CVCalendarBioControl::NewL begin");
    CVCalendarBioControl* self = new(ELeave) CVCalendarBioControl(
        aObserver,
        aSession,
        aId,
        aEditorOrViewerMode,
        aFile);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }

void CVCalendarBioControl::SetAndGetSizeL(TSize& aSize)
    {
    if(iIsFileBased)
    	{
    	SetPosition(TPoint(0,KVCalBcHeightReductionBva));
    	aSize.iHeight -= KVCalBcHeightReductionBva;
    	iViewer->SetAndGetSizeL(aSize);
    	SetSizeWithoutNotification(aSize);
    	}
    else
    	{
    	iViewer->SetAndGetSizeL(aSize);

    	//the following compares the window size against the
        //"virtual" height of the viewer. This way there is no
        //scrolling if virtualHeight is less than window size
        if(iViewer->VirtualHeight() <= Window().Size().iHeight)
            {
            aSize.iHeight = iViewer->VirtualHeight();
            SetSizeWithoutNotification(aSize);
            }
        else
            {
            SetSizeWithoutNotification(aSize);
            }
    	}
    }

void CVCalendarBioControl::SetMenuCommandSetL(CEikMenuPane& aMenuPane)
    {
    if (!IsEditor())
        {
        aMenuPane.SetItemDimmed( KAiwCmdSaveToCalendar, EFalse );

        if( !iCalAiwServiceHandler )
            {
            iCalAiwServiceHandler = CAiwServiceHandler :: NewL();
            }

        // Adding to Calendar is only supported for received calendar messages
        // At this stage, aFile which is given to constructor is corrupted.
        // Base class method IsFileBased() cannot be used because file handle
        // in invalid. Guilty: \s60\avkon\src\AknOpenFileService.cpp
        // void CAknOpenFileServiceSession::OpenHandleWithParamsL(const RMessage2& aMessage).
        // File handle which is given to vCal BIO control is ***local variable***
        // in that method. When vCal is successfully viewed, OpenHandleWithParamsL
        // completes and causes file handle to be invalid. 30.6.2005 / Sauli Painilainen
        if( iIsFileBased )
            {
            // Attaching the interest to the menu.
            // This internally load the aiwprovider plug-in.
            iCalAiwServiceHandler->AttachMenuL( R_BVA_MENU, R_BVA_AIW_CALENDAR_INTEREST );

            // This menu should contain AIW menu placeholder,a sanity check.
            __ASSERT_DEBUG( iCalAiwServiceHandler->IsAiwMenu( R_BVA_MENU ), Panic( EPanic2 ) );

            if (iCalAiwServiceHandler->IsAiwMenu( R_BVA_MENU ) )
                {
                iCalAiwServiceHandler->InitializeMenuPaneL( aMenuPane, R_BVA_MENU,
                        KAiwCmdSaveToCalendar, 
                        iCalAiwServiceHandler->InParamListL() );
                }
            }
        else
            {
            // Attaching the interest to the menu.
            // This internally load the aiwprovider plug-in.
            iCalAiwServiceHandler->AttachMenuL( R_SMSV_OPTIONSMENUPANE, R_SMSV_AIW_CALENDAR_INTEREST );

            // This menu should contain AIW menu placeholder,a sanity check.
            __ASSERT_DEBUG( iCalAiwServiceHandler->IsAiwMenu( R_SMSV_OPTIONSMENUPANE ), Panic( EPanic2 ) );

            if (iCalAiwServiceHandler->IsAiwMenu( R_SMSV_OPTIONSMENUPANE ) )
                {
                iCalAiwServiceHandler->InitializeMenuPaneL( aMenuPane, R_SMSV_OPTIONSMENUPANE,
                        KAiwCmdSaveToCalendar, 
                        iCalAiwServiceHandler->InParamListL() );
                }
            }
        }
    }
	
TBool CVCalendarBioControl::HandleBioCommandL(TInt aCommand)
    {
    if( aCommand == KAiwCmdSaveToCalendar )
        {
        // Sending the filehandle to provider as input parameter.
        CAiwGenericParamList& inCalParamList = iCalAiwServiceHandler->InParamListL();

        RFile fileHandle;
        ResolveFileL( fileHandle );
        TAiwGenericParam calenParam( EGenericParamFile );
        calenParam.Value().Set( fileHandle );
        inCalParamList.AppendL( calenParam );
        iCalAiwServiceHandler->ExecuteMenuCmdL( aCommand, 
                inCalParamList,
                iCalAiwServiceHandler->OutParamListL(),
                0, NULL);
        return ETrue;
        }
    return EFalse;
    }

TRect CVCalendarBioControl::CurrentLineRect() const
    {
    return iViewer->CurrentLineRect();
    }

TBool CVCalendarBioControl::IsFocusChangePossible(
    TMsgFocusDirection aDirection) const
    {
    if (aDirection == EMsgFocusUp)
        {
        return iViewer->IsCursorLocation(EMsgTop);
        }
    return EFalse;
    }

HBufC* CVCalendarBioControl::HeaderTextL() const
    {
    return StringLoader::LoadL(R_QTN_SM_TITLE_INVITATION, iCoeEnv);
    }

TInt CVCalendarBioControl::VirtualHeight()
    {
    return iViewer->VirtualHeight();
    }

TInt CVCalendarBioControl::VirtualVisibleTop()
    {
    return iViewer->VirtualVisibleTop();
    }

TBool CVCalendarBioControl::IsCursorLocation(TMsgCursorLocation aLocation) const
    {
    return iViewer->IsCursorLocation(aLocation);
    }
void CVCalendarBioControl::GetHelpContext(
    TCoeHelpContext& aHelpContext) const
    {
	if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
		{
		const TUid KUidSmart = {0x101F4CDA};
		if ( ! iIsFileBased )
			{
			aHelpContext.iContext=IsEditor()?
                KSMART_HLP_CALENDAREDITOR():KSMART_HLP_CALENDARVIEWER() ;
			aHelpContext.iMajor=KUidSmart;
			}
		else
			{
			if ( !IsEditor() )
			    {
				aHelpContext.iContext=KSMART_HLP_CALPULLMESVIEWER;
			    aHelpContext.iMajor=KUidSmart;
                }
            }
        }
    }




TInt CVCalendarBioControl::CountComponentControls() const
    {
    return 1; // the viewer component
    }

CCoeControl* CVCalendarBioControl::ComponentControl(TInt aIndex) const
    {
    if (aIndex == 0)
        {
        return iViewer;
        }
    return NULL;
    }

void CVCalendarBioControl::SizeChanged()
    {
    iViewer->SetExtent(Position(), iViewer->Size());
    }

void CVCalendarBioControl::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    iViewer->SetFocus(IsFocused());
    }

void CVCalendarBioControl::SetContainerWindowL(const CCoeControl& aContainer)
    {

    CCoeControl::SetContainerWindowL(aContainer);

    // The reason for creating the viewer control here is that the
    // construction of the viewer requires a parent with a window. So it
    // cannot be done in ConstructL().
    //
    iViewer->ConstructL(this);
    }

TKeyResponse CVCalendarBioControl::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    return iViewer->OfferKeyEventL(aKeyEvent, aType);
    }

void CVCalendarBioControl::HandleResourceChange( TInt aType )
    {
    LOG("CVCalendarBioControl::HandleResourceChange begin");
    CMsgBioControl::HandleResourceChange( aType );
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iViewer->Reset();
        }
    LOG("CVCalendarBioControl::HandleResourceChange end");
    }

#if defined (__I_CAL_SUPPORT) && defined (RD_MESSAGING_ICAL_IMPORT)
void CVCalendarBioControl::ImportVCalendarL()
    {
	LOG("CVCalendarBioControl::ImportVCalendarL begin");
	
	CCalenImporter* calImporter = CCalenImporter::NewL( *iSession );
    CleanupStack::PushL( calImporter );
    calImporter->SetImportMode( ECalenImportModeExtended );

    RFile fileHandle;
    ResolveFileL( fileHandle );
    RFileReadStream stream( fileHandle,0 );
    CleanupClosePushL( stream );

    TBuf8<KReadDataAmount> vcalData;

    TInt errRead( KErrNone );
    TRAP( errRead,stream.ReadL( vcalData,KReadDataAmount ) );

    if( errRead != KErrEof && errRead != KErrNone )
    	{
    	User::Leave( KVCalBcLeaveCodeCorrupt );
    	}

	//return the read stream back to the beginning for import
    stream.Source()->SeekL( MStreamBuf::ERead, EStreamBeginning, 0 );

    TInt errImport( KErrNone );
    //Reset the array if its already imported.
    if(iCalEntryArray.Count())
    iCalEntryArray.ResetAndDestroy();

    if ( vcalData.FindF( KVersionVCal ) == KErrNotFound )
		{
		//using the ICAl import API
		TRAP( errImport, calImporter->ImportICalendarL( stream, iCalEntryArray ) );
        }
    else if ( vcalData.FindF( KVersionICal ) == KErrNotFound )
        {
        //using the VCAL import API
        TRAP( errImport, calImporter->ImportVCalendarL( stream, iCalEntryArray ) );
        }
    CleanupStack::PopAndDestroy( 2, calImporter ); //stream, calImporter
    if( errImport != KErrNone || iCalEntryArray.Count() == 0 )
        {
	    LOG1("CVCalendarBioControl::ImportVCalendarL ERROR %d", err);
	    LOG1("CVCalendarBioControl::ImportVCalendarL ERROR imported cal entry count %d", iCalEntryArray.Count() );
        // the data was corrupt
        User::Leave( KVCalBcLeaveCodeCorrupt );
        }

    //BIO viewer always sets the local UID to 0 in imported entry
    iCalEntryArray[0]->SetLocalUidL( TCalLocalUid(0) );

	LOG("CVCalendarBioControl::ImportVCalendarL end");
    }
#else
void CVCalendarBioControl::ImportVCalendarL()
    {
    LOG("CVCalendarBioControl::ImportVCalendarL begin");
    
    CCalenImporter* calImporter = CCalenImporter::NewL( *iSession );
    CleanupStack::PushL( calImporter );
    calImporter->SetImportMode( ECalenImportModeExtended );

    RFile fileHandle;
    ResolveFileL( fileHandle );
    RFileReadStream stream( fileHandle,0 );
    CleanupClosePushL( stream );

    iCalEntryArray.ResetAndDestroy();
    TRAPD( err, calImporter->ImportVCalendarL( stream, iCalEntryArray ) );
    CleanupStack::PopAndDestroy( 2,calImporter );//stream, calImporter
    if( err != KErrNone || iCalEntryArray.Count() == 0 )
        {
	    LOG1("CVCalendarBioControl::ImportVCalendarL ERROR %d", err);
	    LOG1("CVCalendarBioControl::ImportVCalendarL ERROR imported cal entry count %d", iCalEntryArray.Count() );
        // the data was corrupt
        User::Leave( KVCalBcLeaveCodeCorrupt );
        }

    //BIO viewer always sets the local UID to 0 in imported entry
    iCalEntryArray[0]->SetLocalUidL( TCalLocalUid(0) );

	LOG("CVCalendarBioControl::ImportVCalendarL end");
    }
#endif

void CVCalendarBioControl::ResolveFileL( RFile& aFile )
    {
    if ( iIsFileBased )
        {
        //if filebased vcalendar message (opened via BVA)
        //we make a copy to a new handle from class variable iFileHandle
        aFile.Duplicate( iFileHandle );
        }
    else
        {
        //if vcal is opened from message then we fetch new handle from attachment
        CMsvEntry* entry = MsvSession().GetEntryL( iId );
        CleanupStack::PushL( entry );
		CMsvStore* store = entry->ReadStoreL();
		CleanupStack::PushL(store);
		MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
		aFile = attachMan.GetAttachmentFileL( 0 ); //entry is the first attachment
		CleanupStack::PopAndDestroy( 2 ); // store, entry
        }
    }

void CVCalendarBioControl::FileBasedAddMenuItemL(CEikMenuPane& aMenuPane,
    TInt aStringRes, TInt aCommandOffset)
    {
    CEikMenuPaneItem::SData menuItem;
    menuItem.iCascadeId = NULL;
    menuItem.iFlags = NULL;
    menuItem.iText.Format(*StringLoader::LoadLC(aStringRes, iCoeEnv));
    CleanupStack::PopAndDestroy(); // (string)
    menuItem.iCommandId = iBioControlObserver.FirstFreeCommand()
        + aCommandOffset;
    aMenuPane.InsertMenuItemL(menuItem, 0);
    }

void CVCalendarBioControl::OpenStreamLC(RFileReadStream& aStream,
    TFileName& aFileName)
    {
    User::LeaveIfError( aStream.Open(
        CCoeEnv::Static()->FsSession(),
        aFileName,
        EFileStream ));
    aStream.PushL();
    }

CVCalendarBioControl::CVCalendarBioControl(
    MMsgBioControlObserver& aObserver,
    CMsvSession* aSession,
    TMsvId aId,
    TMsgBioMode aEditorOrViewerMode,
    const RFile* aFile):
        CMsgBioControl(aObserver,
            aSession,
            aId,
            aEditorOrViewerMode,
            aFile),
        iCalEntryInitErrno( KErrNone )
    {
    }

void CVCalendarBioControl::ConstructL()
    {
	LOG("CVCalendarBioControl::ConstructL begin");

	SetExtension( this );

    iIsFileBased = IsFileBased();

    LoadResourceL(KVCalBCResourceFile);
    LoadResourceL(KAvkonResourceFile);

    LoadResourceL(KBvaResourceFile, KDC_APP_RESOURCE_DIR);
    LoadResourceL(KSmsViewerResourceFile, KDC_APP_RESOURCE_DIR);
    LoadStandardBioResourceL();
    iSession = CCalSession::NewL();
    // Which calendar file should be used.
	TRAPD( err, iSession->OpenL( iSession->DefaultFileNameL() ) );
    if ( err == KErrNotFound )
        {
        // If calendar file was not found, should we create it or not?
        // I think is not BIO control's responsibility to create it.
        //
        iSession->CreateCalFileL( iSession->DefaultFileNameL() );
	    iSession->OpenL( iSession->DefaultFileNameL() );
        }
    else
        {
        User::LeaveIfError( err );
        }

    //make a copy of the filehandle if the vcal is opened from a file
    //this is because it cannot be acquired later since it goes out of scope
    if ( iIsFileBased )
        {
		iFileHandle.Duplicate(FileHandle());
        }

	RFile fileHandle;
	ResolveFileL(fileHandle);
    RFileReadStream stream( fileHandle,0 );
    CleanupClosePushL( stream );

    MStreamBuf* buf = stream.Source();
    buf->PushL();
    User::LeaveIfNull(buf);
    TInt bufferSize = buf->SizeL();
    if (bufferSize == 0 || (bufferSize < KLengthBeginVCalendar))
        {
        User::Leave(KErrMsgBioMessageNotValid);
        }
    FeatureManager::InitializeLibL();

    ImportVCalendarL();
    iViewer = new (ELeave) CRichBio(ERichBioModeEditorBase);
    CVCalBcDataFiller::FillViewerWithDataL(
        *iViewer, *iCalEntryArray[KIndexFirst], *iCoeEnv);

    CleanupStack::PopAndDestroy(2);

	LOG("CVCalendarBioControl::ConstructL end");
    }

void CVCalendarBioControl::Progress(TInt /* aPercentageCompleted */)
    {
    // Empty
    }

void CVCalendarBioControl::Completed(TInt aError)
    {
    iCalEntryInitErrno = aError;
    CActiveScheduler::Stop();
    }

TBool CVCalendarBioControl::NotifyProgress()
    {
    // Return ETrue if you want progress information.
    return EFalse;
    }

TAny* CVCalendarBioControl::BioControlExtension(TInt aExtensionId)
    {
    if ( aExtensionId == KMsgBioControlScrollExtension )
        {
        return static_cast<MMsgBioControlScrollExtension*> (this);
        }
    else
        {
        return NULL;
        }
    }

TInt CVCalendarBioControl::ExtScrollL( TInt aPixelsToScroll,
                                        TMsgScrollDirection aDirection )
    {
    return iViewer->ScrollL(aPixelsToScroll, aDirection);
    }

void CVCalendarBioControl::ExtNotifyViewEvent( TMsgViewEvent aEvent, TInt aParam )
    {
    iViewer->NotifyViewEvent( aEvent, aParam );
    }

GLDEF_C void Panic(TVCalBcPanic aPanic)
    {
    _LIT(KPanicText, "VCalBC.dll");
    User::Panic(KPanicText, aPanic);
    }
//  End of File
