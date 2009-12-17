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
* Description:  BaseControl adapter
*
*/



// INCLUDE FILES
#include    "CMailBaseControlAdapter.h"
#include    <MsgEditorView.h>  // CMsgEditorView
#include 	<MsgEditorCommon.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMailBaseControlAdapter::CMailBaseControlAdapter
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMailBaseControlAdapter::CMailBaseControlAdapter(CCoeControl& aControl)
    :iControl(aControl)
    {
    }

// -----------------------------------------------------------------------------
// CMailBaseControlAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMailBaseControlAdapter::ConstructL()
    {
    iControlType = EMsgBodyControl;
    }

// -----------------------------------------------------------------------------
// CMailBaseControlAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMailBaseControlAdapter* CMailBaseControlAdapter::NewL(
    CCoeControl* aParent,
    CCoeControl& aControl)
    {
    CMailBaseControlAdapter* self = new( 
        ELeave ) CMailBaseControlAdapter(aControl);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    aControl.SetContainerWindowL(*aParent);
    CleanupStack::Pop(); // self
    return self;
    }

    
// Destructor
CMailBaseControlAdapter::~CMailBaseControlAdapter()
    {
    }

TRect CMailBaseControlAdapter::CurrentLineRect()
    {
    return iControl.Rect();
    }
void CMailBaseControlAdapter::SetAndGetSizeL(TSize& aSize)
    {
    SetExtent(Position(), aSize); 
    }
void CMailBaseControlAdapter::ClipboardL(TMsgClipboardFunc /*aFunc*/)
    {
    // empty
    }
void CMailBaseControlAdapter::EditL(TMsgEditFunc /*aFunc*/)
    {
    // empty
    }
TBool CMailBaseControlAdapter::IsFocusChangePossible(TMsgFocusDirection aDirection) const
    {
    return (aDirection == EMsgFocusUp);
    }
TBool CMailBaseControlAdapter::IsCursorLocation(TMsgCursorLocation aLocation) const
    {
	if (aLocation == EMsgTop)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}   
    }
TUint32 CMailBaseControlAdapter::EditPermission() const
    {
    return 0;
    }

TInt CMailBaseControlAdapter::CountComponentControls() const
    {
    return 1; // iControl
    }
CCoeControl* CMailBaseControlAdapter::ComponentControl(TInt /*aIndex*/) const
    {
    return &iControl; //iControl;
    }
void CMailBaseControlAdapter::SizeChanged()
    {
    iControl.SetExtent(Position(), iControl.Size());
    }
void CMailBaseControlAdapter::FocusChanged(TDrawNow aDrawNow)
    {
    iControl.SetFocus(IsFocused(), aDrawNow);
    }
TInt CMailBaseControlAdapter::VirtualHeight()
    {
    return iControl.Rect().Height();
    }
TInt CMailBaseControlAdapter::VirtualVisibleTop()
    {
    return iControl.Rect().iTl.iY; // left corner
    }

/// From CCoeControl
void CMailBaseControlAdapter::HandleResourceChange(TInt aType)
    {
    iControl.HandleResourceChange( aType );
    }
TKeyResponse CMailBaseControlAdapter::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    return iControl.OfferKeyEventL(aKeyEvent, aType);
    }

void CMailBaseControlAdapter::PrepareForReadOnly(TBool /*aReadOnly*/)
    {
    }

//  End of File  
