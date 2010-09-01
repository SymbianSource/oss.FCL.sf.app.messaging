/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    CMsgBioCtrlTest implementation
*
*/


#include "MsgBioCtrlTest.h"

#include <eikon.hrh>
#include <e32base.h>
#include <eikmenup.h>
#include <msvapi.h>
#include <s32file.h>                // RFileReadStream
#include <stringloader.h>           // StringLoader
#include <msgbiocontrolobserver.h>  // MMsgBioControlObserver
#include <StringLoader.h>
#include <MMsvAttachmentManager.h>
#include <crichbio.h>               // CRichBio
#include <PluginBioControlAPITest.rsg>               // resouce identifiers


// LOCAL CONSTANTS AND MACROS

_LIT(KBCTestResourceFile, "PluginBioControlAPITest.rsc");
_LIT(KAvkonResourceFile, "avkon.rsc");
const TInt KMenuPos                     = 1; //The inserting position of menu item.
const TInt KMenuCommandDoBCTest         = 0;

// MEMBER FUNCTIONS

CMsgBioCtrlTest::~CMsgBioCtrlTest()
    {
    delete iViewer;
    }

EXPORT_C CMsgBioControl* CMsgBioCtrlTest::NewL(
    MMsgBioControlObserver& aObserver,
    CMsvSession* aSession,
    TMsvId aId,
    TMsgBioMode aEditorOrViewerMode,
    const RFile* aFile)
    {
    CMsgBioCtrlTest* self = new(ELeave) CMsgBioCtrlTest(
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

void CMsgBioCtrlTest::SetAndGetSizeL(TSize& aSize)
    {
    iViewer->SetAndGetSizeL(aSize);
    SetSizeWithoutNotification(aSize);

    if(!(iHandledCommands & ESetAndGetSizeL))
        {
        iHandledCommands += ESetAndGetSizeL;
        AddResultsToRichBio(R_API_SETANDGETSIZEL,R_API_PASSED);
        }
    }


void CMsgBioCtrlTest::SetMenuCommandSetL(CEikMenuPane& aMenuPane)
    {
    
    AddMenuItemL(aMenuPane, R_DO_BC_TEST,
                KMenuCommandDoBCTest, KMenuPos);

    if(!(iHandledCommands & ESetMenuCommandSetL))
        {
        iHandledCommands += ESetMenuCommandSetL;
        AddResultsToRichBio(R_API_SETMENUCOMMANDSETL,R_API_PASSED);
        }
    }

TBool CMsgBioCtrlTest::HandleBioCommandL(TInt /*aCommand*/)
    {
    if(!(iHandledCommands & EHandleBioCommandL))
        {
        iHandledCommands += EHandleBioCommandL;
        AddResultsToRichBio(R_API_HANDLEBIOCOMMANDL,R_API_PASSED);
        }
    return EFalse;
    }

TRect CMsgBioCtrlTest::CurrentLineRect() const
    {
    if(!(iHandledCommands & ECurrentLineRect))
        {
        iHandledCommands += ECurrentLineRect;
        AddResultsToRichBio(R_API_CURRENTLINERECT,R_API_PASSED);
        }
    return iViewer->CurrentLineRect();
    }

TBool CMsgBioCtrlTest::IsFocusChangePossible(
    TMsgFocusDirection aDirection) const
    {
    if(!(iHandledCommands & EIsFocusChangePossible))
        {
        iHandledCommands += EIsFocusChangePossible;
        AddResultsToRichBio(R_API_ISFOCUSCHANGEPOSSIBLE,R_API_PASSED);
        }
    if (aDirection == EMsgFocusUp)
        {
        return iViewer->IsCursorLocation(EMsgTop);
        }
    return EFalse;
    }

HBufC* CMsgBioCtrlTest::HeaderTextL() const
    {
    if(!(iHandledCommands & EHeaderTextL))
        {
        iHandledCommands += EHeaderTextL;
        AddResultsToRichBio(R_API_HEADERTEXTL,R_API_PASSED);
        }
    return StringLoader::LoadL(R_TITLE_BC_TEST, iCoeEnv);
    }
    
TUint32 CMsgBioCtrlTest::OptionMenuPermissionsL() const
    {
    if(!(iHandledCommands & EOptionMenuPermissionsL))
        {
        iHandledCommands += EOptionMenuPermissionsL;
        AddResultsToRichBio(R_API_OPTIONMENUPERMISSIONSL,R_API_PASSED);
        }
    return CMsgBioControl::OptionMenuPermissionsL();
    }

TInt CMsgBioCtrlTest::VirtualHeight()
    {
    if(!(iHandledCommands & EVirtualHeight))
        {
        iHandledCommands += EVirtualHeight;
        AddResultsToRichBio(R_API_VIRTUALHEIGHT,R_API_PASSED);
        }
    CMsgBioControl::VirtualHeight();
    iViewer->VirtualHeight();
    return V_HEIGHT;
    }

TInt CMsgBioCtrlTest::VirtualVisibleTop()
    {
    if(!(iHandledCommands & EVirtualVisibleTop) )
        {
        iHandledCommands += EVirtualVisibleTop;
        AddResultsToRichBio(R_API_VIRTUALVISIBLETOP,R_API_PASSED);
        }
    CMsgBioControl::VirtualVisibleTop();
    iViewer->VirtualVisibleTop();
    return V_TOP;
    }

TBool CMsgBioCtrlTest::IsCursorLocation(TMsgCursorLocation aLocation) const
    {
    if(!(iHandledCommands & EIsCursorLocation))
        {
        iHandledCommands += EIsCursorLocation;
        AddResultsToRichBio(R_API_ISCURSORLOCATION,R_API_PASSED);
        }
    CMsgBioControl::IsCursorLocation(aLocation);
    return iViewer->IsCursorLocation(aLocation);
    }

TAny* CMsgBioCtrlTest::BioControlExtension(TInt aExtensionId)
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
 
TInt CMsgBioCtrlTest::ExtScrollL( TInt aPixelsToScroll,
                                        TMsgScrollDirection aDirection )
    {
    return iViewer->ScrollL(aPixelsToScroll, aDirection);
    }
 
void CMsgBioCtrlTest::ExtNotifyViewEvent( TMsgViewEvent aEvent, TInt aParam )
    {
    iViewer->NotifyViewEvent( aEvent, aParam );
    }

void CMsgBioCtrlTest::ProtectedCallsL(TBool fileBased)
    {
    TBool check = IsEditor();
    check = IsFileBased();
    CMsvSession* session = &(MsvSession());
    if( fileBased )
        {
        //if it is not file based, this call panics
        RFile file = FileHandle();
        }
    MEikMenuObserver* menuObserver = new(ELeave)FakeObserver();
    CleanupStack::PushL( menuObserver );
    CEikMenuPane* menuPane = new(ELeave)CEikMenuPane(menuObserver);
    CleanupStack::PushL( menuPane );
    AddMenuItemL( *menuPane, R_API_PASSED, 1, 0);
    CleanupStack::Pop( 2 );// menuPane, menuObserver
    }

void CMsgBioCtrlTest::CallNotifyEditorViewL()
    {
    // NotifyEidtorViewL - this function is not fully covered as 
    // there is no BioBodyControL
    NotifyEditorViewL( EMsgBioUpdateScrollBars, 0 );    
    }

void CMsgBioCtrlTest::RichBioFunctionCalls()
    {
    TRect rect = iViewer->CurrentLineRect();
    TBool isEditor = iViewer->IsEditorBaseMode();
    CEikRichTextEditor* editor = &(iViewer->Editor());
    iViewer->Reset();
    }

TInt CMsgBioCtrlTest::CountComponentControls() const
    {
    return 1; // the viewer component
    }

CCoeControl* CMsgBioCtrlTest::ComponentControl(TInt aIndex) const
    {
    if (aIndex == 0)
        {
        return iViewer;
        }
    return NULL;
    }

void CMsgBioCtrlTest::SizeChanged()
    {
    iViewer->SetExtent(Position(), iViewer->Size());
    }

void CMsgBioCtrlTest::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    iViewer->SetFocus(IsFocused());
    }

void CMsgBioCtrlTest::SetContainerWindowL(const CCoeControl& aContainer)
    {
    CCoeControl::SetContainerWindowL(aContainer);

    // The reason for creating the viewer control here is that the
    // construction of the viewer requires a parent with a window. So it
    // cannot be done in ConstructL().
    //
    SetExtension( this );
    iViewer->ConstructL(&aContainer);
    iViewer->ActivateL();
    }

TKeyResponse CMsgBioCtrlTest::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    return iViewer->OfferKeyEventL(aKeyEvent, aType);
    }

CMsgBioCtrlTest::CMsgBioCtrlTest(
    MMsgBioControlObserver& aObserver,
    CMsvSession* aSession,
    TMsvId aId,
    TMsgBioMode aEditorOrViewerMode,
    const RFile* aFile):
        CMsgBioControl(aObserver,
            aSession,
            aId,
            aEditorOrViewerMode,
            aFile)
    {
    iHandledCommands = 0;
    }

void CMsgBioCtrlTest::ConstructL()
    {
    LoadResourceL(KBCTestResourceFile);
    LoadResourceL(KAvkonResourceFile);
    LoadStandardBioResourceL();
    
    iViewer = new (ELeave) CRichBio(ERichBioModeEditorBase);
    }

void CMsgBioCtrlTest::AddResultsToRichBio(TInt aLabelRes, TInt aValueRes) const
    {
    TRAPD(err,AddItemL(aLabelRes,aValueRes));
    if( err )
        {
        //ARM warnings
        }
    
    if(iHandledCommands == EAllBioCommands)
        {
        TRAP(err,AddItemL(R_API_RESULTS_SUBJECT,R_API_PASSED));
        if( err )
            {
            //ARM warnings
            }
        }
    }

void CMsgBioCtrlTest::AddItemL(TInt aLabelRes, TInt aValueRes) const
    {
    iViewer->AddItemL(
        *StringLoader::LoadLC(aLabelRes, iCoeEnv),
        *StringLoader::LoadLC(aValueRes, iCoeEnv));
    CleanupStack::PopAndDestroy(2); // (label and value text)
    }

//End of File