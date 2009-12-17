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
* Description:  
*		Container class for bio controls
*
*/



// INCLUDE FILES

#include "MsgBioBodyControl.h"          // for CMsgBioBodyControl
#include "MsgBioControlFactory.h"       // for MMsgBioControlFactory
#include "MsgEditorUtils.pan"           // for MsgEditorUtils panics
#include <MsgBaseControlObserver.h>     // for MMsgBaseControlObserver
#include <s32file.h>                    // for CFileStore
#include <msvapi.h>                     // MTM server API
#include <eikmenup.h>
#include "msgbiocontrol.h"              // for CMsgBioControl
#include <msvapi.h>                     // CMsvSession
#include "MsgEditorUtilsLog.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS

//  LOCAL CONSTANTS AND MACROS

const TInt KControlIdBioControl = 0;
const TInt KNumberOfControls = 1;

// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================

EXPORT_C CMsgBioBodyControl::~CMsgBioBodyControl()
    {
    delete iBioControl;
    delete iBioControlFactory;
    }

EXPORT_C CMsgBioBodyControl* CMsgBioBodyControl::NewL(
    MMsgBioControlObserver& aBioControlObserver,
    const TUid& aBioMsgType,  // Bio message type id
    TMsvId aId,
    TMsgBioMode aEditorOrViewerMode,
    CMsvSession& aSession)
    {
    LOG("CMsgBioBodyControl::NewL begin");
    CMsgBioBodyControl* self = new(ELeave) CMsgBioBodyControl(aSession);
    CleanupStack::PushL(self);
    self->ConstructL(aBioControlObserver, aBioMsgType, aId,
        aEditorOrViewerMode);
    CleanupStack::Pop(self);
    LOG("CMsgBioBodyControl::NewL end");
    return self;
    }

void CMsgBioBodyControl::ConstructL(
        MMsgBioControlObserver& aBioControlObserver,
        const TUid& aBioMsgType,
        TMsvId aId,
        TMsgBioMode aEditorOrViewerMode)
    {
	LOG("CMsgBioBodyControl::ConstructL begin");
    iControlType = EMsgBodyControl;

    iBioControlFactory = CMsgBioControlFactory::NewL();
    iBioControl = iBioControlFactory->CreateControlL(
        aBioControlObserver,
        aBioMsgType,
        aEditorOrViewerMode,
        aId,
        &iSession);
    iBioControl->SetBioBodyControl( this );
    LOG("CMsgBioBodyControl::ConstructL end");
    }

CMsgBioBodyControl::CMsgBioBodyControl(CMsvSession& aSession) : iSession(aSession)
    {
    }

EXPORT_C TBool CMsgBioBodyControl::HandleBioCommandL(TInt aCommand)
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));

    return iBioControl->HandleBioCommandL(aCommand);
    }

EXPORT_C HBufC* CMsgBioBodyControl::HeaderTextL(void) const
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));
    return iBioControl->HeaderTextL();
    }

EXPORT_C TRect CMsgBioBodyControl::CurrentLineRect() //virtual
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));
    TRect lineRect = iBioControl->CurrentLineRect();
    lineRect.iTl.iX = Rect().iTl.iX;
    lineRect.iBr.iX = Rect().iBr.iX;

    //This is a translation to screen origin coordinates.
    lineRect.Move(TPoint(0, Position().iY));

    return lineRect;
    }

EXPORT_C void CMsgBioBodyControl::SetAndGetSizeL(TSize& aSize)
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));
    if (iBioControl->IsFileBased())
        {
        iBioControl->SetSize(aSize); //from CCoeControl
        }
    else
        {
        iBioControl->SetAndGetSizeL(aSize);
        }
    SetSizeWithoutNotification(aSize);
    }

//This is from CMsgBaseControl.
EXPORT_C void CMsgBioBodyControl::ClipboardL(TMsgClipboardFunc /*aFunc*/)
    {
    }

//This is from CMsgBaseControl.
EXPORT_C void CMsgBioBodyControl::EditL(TMsgEditFunc /*aFunc*/)
    {
    }

EXPORT_C TUint32 CMsgBioBodyControl::EditPermission() const
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));
    TUint32 editPerm(EMsgEditNone);
	TUint32 bioPerm(0);
	// Implementations should not leave but if it happens ignore.
	TRAP_IGNORE(bioPerm = iBioControl->OptionMenuPermissionsL());
    if ( bioPerm & EMsgBioEditText)
        {
        editPerm |= EMsgEditCut;
        editPerm |= EMsgEditCopy;
        editPerm |= EMsgEditPaste;
        editPerm |= EMsgEditUndo;
        }
    return editPerm;
    }

EXPORT_C TBool CMsgBioBodyControl::IsFocusChangePossible(TMsgFocusDirection aDirection) const
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));
    return iBioControl->IsFocusChangePossible(aDirection);
    }

EXPORT_C TBool CMsgBioBodyControl::IsCursorLocation(TMsgCursorLocation aLocation) const
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));
    return iBioControl->IsCursorLocation(aLocation);
    }

EXPORT_C TInt CMsgBioBodyControl::VirtualHeight()
{
    return iBioControl->VirtualHeight();
}

EXPORT_C TInt CMsgBioBodyControl::VirtualVisibleTop()
{
    return iBioControl->VirtualVisibleTop();
}

TKeyResponse CMsgBioBodyControl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType) //virtual
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));

    return iBioControl->OfferKeyEventL(aKeyEvent, aType);
    }

EXPORT_C TBool CMsgBioBodyControl::HandleBaseControlEventRequestL( 
    TMsgBioControlEventRequest aRequest,
    TInt aDelta)
    {
    TMsgControlEventRequest controlEvent;
    
    switch ( aRequest )
        {
        case EMsgBioUpdateScrollBars:
            {
            controlEvent = EMsgEnsureCorrectFormPosition;
            break;
            }
        default:
            {
            controlEvent = EMsgEnsureCorrectFormPosition;
            break;
            }
        }

    return iBaseControlObserver->HandleBaseControlEventRequestL(
        this, controlEvent, aDelta);

    }

// from CMsgBaseControl
void CMsgBioBodyControl::PrepareForReadOnly(TBool /*aReadOnly*/)
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));
    }

TInt CMsgBioBodyControl::CountComponentControls() const
    {
    return KNumberOfControls;
    }

CCoeControl* CMsgBioBodyControl::ComponentControl(TInt aIndex) const
    {
    __ASSERT_DEBUG(aIndex == KControlIdBioControl, Panic(EMEUIncorrectComponentIndex));
    if (aIndex == KControlIdBioControl)
        {
        return iBioControl;
        }
    return NULL;
    }

void CMsgBioBodyControl::SizeChanged()
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));

    iBioControl->SetExtent(Position(), iBioControl->Size());
    }

// ---------------------------------------------------------
// The menu doesn't need to be restored when the focus
// goes outside the body. Double check!
// ---------------------------------------------------------
//
void CMsgBioBodyControl::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));

    if (!iBioControl->IsNonFocusing())
        {
        iBioControl->SetFocus(IsFocused());
        }
    }

void CMsgBioBodyControl::SetContainerWindowL(const CCoeControl& aContainer) //virtual
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));

    CCoeControl::SetContainerWindowL(aContainer);
    iBioControl->SetContainerWindowL(aContainer);
    }

EXPORT_C void CMsgBioBodyControl::SetMenuCommandSetL(CEikMenuPane& aMenuPane)
    {
    iBioControl->SetMenuCommandSetL(aMenuPane);
    }

EXPORT_C CMsgBioControl& CMsgBioBodyControl::BioControl(void)
    {
    __ASSERT_DEBUG(iBioControl, Panic(EMEUBioControlNotExist));
    return *iBioControl;
    }

#ifdef RD_SCALABLE_UI_V2
EXPORT_C TInt CMsgBioBodyControl::ScrollL( TInt aPixelsToScroll,
                                           TMsgScrollDirection aDirection )
    {
    return iBioControl->ScrollL( aPixelsToScroll, aDirection );
    }

EXPORT_C void CMsgBioBodyControl::NotifyViewEvent( TMsgViewEvent aEvent,
                                                   TInt aParam )
    {
    iBioControl->NotifyViewEvent( aEvent, aParam );
    }
#else
EXPORT_C TInt CMsgBioBodyControl::ScrollL( TInt /*aPixelsToScroll*/,
                                           TMsgScrollDirection /*aDirection*/ )
    {
    return 0;
    }

EXPORT_C void CMsgBioBodyControl::NotifyViewEvent( TMsgViewEvent /*aEvent*/, TInt /*aParam*/ )
	{
	//no op
	}
#endif //RD_SCALABLE_UI_V2

//  End of File
