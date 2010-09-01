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
#include 	"MailLog.h"
#include    "CMailMsgBaseControlAdapter.h"
#include    "MMailMsgBaseControl.h"
#include    <MsgBaseControl.h>
#include    <MsgEditorView.h>  // CMsgEditorView
#include 	<MsgEditorCommon.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMailMsgBaseControlAdapter::CMailMsgBaseControlAdapter
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMailMsgBaseControlAdapter::CMailMsgBaseControlAdapter(
    MMailMsgBaseControl& aBaseControl)
    :iBaseControl(aBaseControl)
    {
    }

// -----------------------------------------------------------------------------
// CMailMsgBaseControlAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMailMsgBaseControlAdapter::ConstructL()
    {
    iControlType = EMsgBodyControl;
    }

// -----------------------------------------------------------------------------
// CMailMsgBaseControlAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMailMsgBaseControlAdapter* CMailMsgBaseControlAdapter::NewL(
    CCoeControl* /*aParent*/,
    MMailMsgBaseControl& aControl)
    {
    CMailMsgBaseControlAdapter* self = new( 
        ELeave ) CMailMsgBaseControlAdapter(aControl);
    
    CleanupStack::PushL( self );
    self->ConstructL();
//    aControl.SetContainerWindowL(*aParent);
    CleanupStack::Pop(); // self
    return self;
    }

    
// Destructor
CMailMsgBaseControlAdapter::~CMailMsgBaseControlAdapter()
    {
    }

// -----------------------------------------------------------------------------
// CMailMsgBaseControlAdapter::NotifyViewEvent
// -----------------------------------------------------------------------------
//
void CMailMsgBaseControlAdapter::NotifyViewEvent( 
    TMsgViewEvent LOGPARAM_ONLY( aEvent ), TInt LOGPARAM_ONLY( aParam ) )
    {
    LOG2("CMailMsgBaseControlAdapter::NotifyViewEvent aEvent:%d aParam:%d",
        aEvent, aParam);
    TPoint pos = Position();
    TSize sis = Size();    
 /*   switch( aEvent )
        {
        case EMsgViewEventPrepareFocusTransitionUp:
        case EMsgViewEventPrepareFocusTransitionDown:
        case EMsgViewEventPrepareForViewing:
        case EMsgViewEventPrepareForZooming:
        case EMsgViewEventFormHeightChanged:
        case EMsgViewEventFormScrolled:
        case EMsgViewEventSetCursorFirstPos:
        case EMsgViewEventSetCursorLastPos:
        default:
        };*/
    }

// -----------------------------------------------------------------------------
// CMailMsgBaseControlAdapter::CurrentLineRect
// -----------------------------------------------------------------------------
//
TRect CMailMsgBaseControlAdapter::CurrentLineRect()
    {
    return iBaseControl.CurrentLineRect();
    }
    
void CMailMsgBaseControlAdapter::SetAndGetSizeL(TSize& aSize)
    {
    iBaseControl.SetAndGetSizeL( aSize );
    SetSize( aSize );
    }
void CMailMsgBaseControlAdapter::ClipboardL(TMsgClipboardFunc aFunc)
    {
    iBaseControl.ClipboardL( aFunc );
    }
void CMailMsgBaseControlAdapter::EditL(TMsgEditFunc aFunc)
    {
    iBaseControl.EditL( aFunc );
    }

// -----------------------------------------------------------------------------
// CMailMsgBaseControlAdapter::IsFocusChangePossible
// -----------------------------------------------------------------------------
//    
TBool CMailMsgBaseControlAdapter::IsFocusChangePossible(
    TMsgFocusDirection aDirection) const
    {
    return iBaseControl.IsFocusChangePossible( aDirection );
    }

// -----------------------------------------------------------------------------
// CMailMsgBaseControlAdapter::IsCursorLocation
// -----------------------------------------------------------------------------
//    
TBool CMailMsgBaseControlAdapter::IsCursorLocation(
    TMsgCursorLocation aLocation) const
    {
    return iBaseControl.IsCursorLocation( aLocation );
    }
TUint32 CMailMsgBaseControlAdapter::EditPermission() const
    {
    return iBaseControl.EditPermission();
    }

TInt CMailMsgBaseControlAdapter::CountComponentControls() const
    {
    return iBaseControl.CountPluginComponentControls();
    }
    
CCoeControl* CMailMsgBaseControlAdapter::ComponentControl(TInt aIndex) const
    {
    return iBaseControl.PluginComponentControl( aIndex );
    }
    
void CMailMsgBaseControlAdapter::SizeChanged()
    {
    LOG2("CMailMsgBaseControlAdapter::SizeChanged Position:%d iY:%d",
        Position().iX, Position().iY);
    LOG2("CMailMsgBaseControlAdapter::SizeChanged Size w:%d h:%d",
        Size().iWidth, Size().iHeight);
    TPoint editorPosition( Position() );
    TRect viewRect(0,0,0,0);
    if (iBaseControlObserver)
        {
        viewRect = iBaseControlObserver->ViewRect();
        }
    editorPosition.iY += viewRect.iTl.iY;          
    iBaseControl.SetPluginExtent( editorPosition, Size() );
    }
    
void CMailMsgBaseControlAdapter::FocusChanged(TDrawNow aDrawNow)
    {
    iBaseControl.PluginFocusChanged(aDrawNow);
    }

void CMailMsgBaseControlAdapter::SetExtent(
    const TPoint& LOGPARAM_ONLY( aPosition ),const TSize& /*aSize*/)
    {
    LOG2("CMailMsgBaseControlAdapter::SetExtent iX:%d iY:%d",
        aPosition.iX, aPosition.iY);    
    }

void CMailMsgBaseControlAdapter::SetPosition(
    const TPoint& LOGPARAM_ONLY( aPosition ) )
    {
    LOG2("CMailMsgBaseControlAdapter::SetPosition iX:%d iY:%d",
        aPosition.iX, aPosition.iY);
    }
    
TInt CMailMsgBaseControlAdapter::VirtualHeight()
    {
    return iBaseControl.VirtualHeight();
    }
    
TInt CMailMsgBaseControlAdapter::VirtualVisibleTop()
    {
    return iBaseControl.VirtualVisibleTop();
    }
    
void CMailMsgBaseControlAdapter::HandleResourceChange(TInt aType)
    {
    iBaseControl.PluginHandleResourceChange( aType );
    }
    
TKeyResponse CMailMsgBaseControlAdapter::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    return iBaseControl.PluginOfferKeyEventL(aKeyEvent, aType);
    }

void CMailMsgBaseControlAdapter::PrepareForReadOnly(TBool aReadOnly)
    {
    iBaseControl.PrepareForReadOnly( aReadOnly );
    }

//  End of File  
