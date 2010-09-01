/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     Bio control for iCalendar messages.
*
*/




// INCLUDE FILES

#include <eikon.hrh>
#include <e32base.h>
#include <eikmenup.h>
#include <msvapi.h>
#include <s32file.h>                // RFileReadStream
#include <stringloader.h>           // StringLoader
#include <msgbiocontrolobserver.h>  // MMsgBioControlObserver
#include <crichbio.h>               // CRichBio
#include <msgbioutils.h>
#include "CICalBC.h"    			// CICalendarBioControl
#include <bldvariant.hrh>			// __SERIES60_HELP
#include <csxhelp/smart.hlp.hrh>
#include <FeatMgr.h>
#include <MMsvAttachmentManager.h>
#include <f32file.h>
#include <MeetingRequestUids.hrh>
#include <avkon.hrh>


// LOCAL CONSTANTS AND MACROS
const TInt KLengthBeginICalendar = 15; // "BEGIN:VCALENDAR" -string length
const TInt KICalBcHeightReductionBva = 9;

_LIT(KAvkonResourceFile, "avkon.rsc");

// MEMBER FUNCTIONS

CICalendarBioControl::~CICalendarBioControl()
    {
    FeatureManager::UnInitializeLib();
    iVCalStream.Close();
    delete iICalContainer;
    }

EXPORT_C CMsgBioControl* CICalendarBioControl::NewL(
    MMsgBioControlObserver& aObserver,
    CMsvSession* aSession,
    TMsvId aId,
    TMsgBioMode aEditorOrViewerMode,
    const RFile* aFile )
    {
    CICalendarBioControl* self = new(ELeave) CICalendarBioControl(
        aObserver,
        aSession,
        aId,
        aEditorOrViewerMode,
        aFile );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }

void CICalendarBioControl::SetAndGetSizeL(TSize& aSize)
    {
    if ( iIsFileBased )
        {
        SetPosition(TPoint(0, KICalBcHeightReductionBva));
        aSize.iHeight -= KICalBcHeightReductionBva;
        iViewer->SetSize(aSize);
        }
    else
        {
        iViewer->SetSize(aSize);
        }
    SetSizeWithoutNotification(aSize);
    }

void CICalendarBioControl::SetMenuCommandSetL(CEikMenuPane& aMenuPane)
    {
    if (!IsEditor())
        {
        if ( iIsFileBased )
            {
            iICalContainer->DynInitMenuPaneL(0,&aMenuPane);
            }
		}
    }

TBool CICalendarBioControl::HandleBioCommandL(TInt aCommand)
    {
    return iICalContainer->HandleCommandL(aCommand);
    }

TRect CICalendarBioControl::CurrentLineRect() const
    {
    //return iViewer->CurrentLineRect();
    return TRect();
    }

TBool CICalendarBioControl::IsFocusChangePossible(
    TMsgFocusDirection /*aDirection*/) const
    {
    return EFalse;
    }

HBufC* CICalendarBioControl::HeaderTextL() const
    {
    return KNullDesC().AllocL();
    }

TInt CICalendarBioControl::VirtualHeight()
    {
    return 0;
    }

TInt CICalendarBioControl::VirtualVisibleTop()
    {
    return 0;
    }

TBool CICalendarBioControl::IsCursorLocation(TMsgCursorLocation /*aLocation*/) const
    {
  	return EFalse;
    }

TInt CICalendarBioControl::CountComponentControls() const
    {
    return 1; // the viewer component
    }

CCoeControl* CICalendarBioControl::ComponentControl(TInt aIndex) const
    {
    if (aIndex == 0)
        {
        return iViewer;
        }
    return NULL;
    }

void CICalendarBioControl::SizeChanged()
    {
    iViewer->SetExtent(Position(), iViewer->Size());
    }

void CICalendarBioControl::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    iViewer->SetFocus(IsFocused());
    }

void CICalendarBioControl::SetContainerWindowL(const CCoeControl& aContainer)
    {
    CCoeControl::SetContainerWindowL(aContainer);
    iParentControl = const_cast<CCoeControl*>(&aContainer);
	
    iICalContainer->ViewMessageL(iVCalStream,*this);
    }

TKeyResponse CICalendarBioControl::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    return iViewer->OfferKeyEventL(aKeyEvent, aType);
    }

void CICalendarBioControl::FileBasedAddMenuItemL(CEikMenuPane& aMenuPane,
    TInt aStringRes, TInt aCommandOffset)
    {
    CEikMenuPaneItem::SData menuItem;
    menuItem.iCascadeId = NULL;
    menuItem.iFlags = NULL;
    HBufC* menuItemText(StringLoader::LoadLC(aStringRes, iCoeEnv));
    TPtr textPtr = menuItemText->Des();
    if(textPtr.Length() > CEikMenuPaneItem::SData::ENominalTextLength)
    	{
    	menuItem.iText.Format(textPtr.MidTPtr(0,CEikMenuPaneItem::SData::ENominalTextLength) );
    	}
    else
    	{
    	menuItem.iText.Format(textPtr);
    	}
    CleanupStack::PopAndDestroy(menuItemText); // (string)
    menuItem.iCommandId = iBioControlObserver.FirstFreeCommand()
        + aCommandOffset;
    aMenuPane.InsertMenuItemL(menuItem, 0);
    iICalContainer->DynInitMenuPaneL(0,&aMenuPane);
    }

void CICalendarBioControl::OpenStreamLC(RFileReadStream& aStream,
    TFileName& aFileName)
    {
    User::LeaveIfError( aStream.Open(
        CCoeEnv::Static()->FsSession(),
        aFileName,
        EFileStream ));
    aStream.PushL();
    }

CICalendarBioControl::CICalendarBioControl(
    MMsgBioControlObserver& aObserver,
    CMsvSession* aSession,
    TMsvId aId,
    TMsgBioMode aEditorOrViewerMode,
    const RFile* aFile ):
        CMsgBioControl(aObserver,
            aSession,
            aId,
            aEditorOrViewerMode,
            aFile )
    {
    }

void CICalendarBioControl::ConstructL()
    {
    LoadResourceL(KAvkonResourceFile);
    LoadStandardBioResourceL();
    
    // Due to current bio control base problem filehandle & IsFileBased() mustn't
    // be accessed after construction phase!
    RFile fileHandle;
    iIsFileBased = IsFileBased(); 
    if ( iIsFileBased )
        {
		fileHandle = FileHandle();
		iVCalStream.Attach( fileHandle );
        }
    else
        {
        CMsvEntry* entry = MsvSession().GetEntryL( iId );
        CleanupStack::PushL( entry );
		CMsvStore* store = entry->ReadStoreL();
		CleanupStack::PushL(store);
		MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
		fileHandle = attachMan.GetAttachmentFileL( 0 ); //entry is the first attachment
		iVCalStream.Attach( fileHandle );
		CleanupStack::PopAndDestroy( 2 ); // store, entry
        }        

	MStreamBuf* buf = iVCalStream.Source();
    User::LeaveIfNull(buf);
    TInt bufferSize = buf->SizeL();
    if (bufferSize == 0 || (bufferSize < KLengthBeginICalendar))
        {
        User::Leave(KErrMsgBioMessageNotValid);
        }

    FeatureManager::InitializeLibL();

	TUid viewerID = TUid::Uid(KUidMsgMailECOMIcalViewImpl);
    iICalContainer = CMailMessageView::NewL(viewerID);
	
    LoadResourceL(iICalContainer->ResourceFile());
    
    fileHandle.Close();
    }

void CICalendarBioControl::AddAttachmentL(
    CMsvAttachment& /*aAttachmentInfo*/,
    TBool /*aCanBeRemoved*/ )
	{
	
	}

void CICalendarBioControl::AddControlL(CCoeControl& aControl)
    {
    iViewer = &aControl;
    }

void CICalendarBioControl::AddControlL( TInt /*aControlType*/ )
	{
	}
	
void CICalendarBioControl::AddControlL( MMailMsgBaseControl& /*aControl*/ )
	{
	}

CCoeControl& CICalendarBioControl::ParentControl()
    {
    return *iParentControl;
    }
    
CEikStatusPane* CICalendarBioControl::StatusPane()   
	{
	return NULL;
	}

void CICalendarBioControl::SetTitleTextL(TInt aResourceId)
    {
    iTitleResourceID = aResourceId;
    }

void CICalendarBioControl::StartWaitNoteL(/*const TDesC& aText*/)
	{
	}

void CICalendarBioControl::StopWaitNote()
	{
	}
	
void CICalendarBioControl::SetAddressFieldTextL(TInt /*aControlType*/, CDesCArray& /*aText*/)
	{
	}
void CICalendarBioControl::SetAddressFieldTextL(TInt /*aControlType*/, TDesC& /*aText*/)
	{
	}
	
void CICalendarBioControl::SetBodyTextL(CRichText& /*aText*/)
	{
	}
	
void CICalendarBioControl::SetSubjectL(const TDesC& /*aText*/)
	{
	}
	
TRect CICalendarBioControl::MsgViewRect(TInt /*aPane*/)
	{
	return TRect(0,0,0,0);
	}
	
void CICalendarBioControl::AppUiHandleCommandL(TInt aCommand)
	{
	switch( aCommand )
		{
		case EAknSoftkeyBack:
			{
			iBioControlObserver.RequestHandleCommandL(MMsgBioControlObserver::EMsgClose);
			break;
			}
		default:
			{
			break;
			}
		}
	}
	
TBool CICalendarBioControl::IsNextMessageAvailableL( TBool /*aForward*/)
	{
	return EFalse;
	}
        
void CICalendarBioControl::NextMessageL( TBool /*aForward*/ )
	{
	//empty implementation
	}
	

//  End of File
