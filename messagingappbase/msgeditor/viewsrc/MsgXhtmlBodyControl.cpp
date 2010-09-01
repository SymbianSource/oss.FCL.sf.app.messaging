/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgXhtmlBodyControl implementation
*
*/



// ========== INCLUDE FILES ================================

#include <AknUtils.h>
#include <AknsUtils.h>
#include <applayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <ItemFinder.h>

#include "MsgEditorCommon.h"
#include "MsgXhtmlBodyControl.h"
#include "MsgBodyControlEditor.h"
#include "MsgEditorPanic.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

#ifdef RD_MSG_XHTML_SUPPORT 
_LIT( KWwwUrlAddress, "www." );
_LIT( KSchemeAddress, ":" );
#endif

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================
#ifdef RD_MSG_XHTML_SUPPORT 
const TInt KObserverArrayGranularity = 2;
#endif

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::CMsgXhtmlBodyControl
//
// Constructor.
// ---------------------------------------------------------
//
CMsgXhtmlBodyControl::CMsgXhtmlBodyControl( MMsgBaseControlObserver& aBaseControlObserver ) : 
    CMsgBodyControl( aBaseControlObserver )
    {
    }

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::~CMsgXhtmlBodyControl
//
// Destructor.
// ---------------------------------------------------------
//
CMsgXhtmlBodyControl::~CMsgXhtmlBodyControl()
    {
    delete iObservers;
#ifdef RD_MSG_XHTML_SUPPORT 
    delete iParser;
#endif
    }

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::NewL
//
// Factory method that creates this control.
// ---------------------------------------------------------
//
EXPORT_C CMsgXhtmlBodyControl* CMsgXhtmlBodyControl::NewL( CMsgEditorView& aParent,
                                                           MMsgAsyncControlObserver* aObserver )
    {
    CMsgXhtmlBodyControl* self = new ( ELeave ) CMsgXhtmlBodyControl( aParent );
    self->SetMopParent( &aParent );
    
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::ConstructL
// ---------------------------------------------------------
//
#ifdef RD_MSG_XHTML_SUPPORT 
void CMsgXhtmlBodyControl::ConstructL( MMsgAsyncControlObserver* aObserver )
    {
    CMsgBodyControl::ConstructL();
    
    SetControlType( EMsgXhtmlBodyControl );
    SetPlainTextMode( EFalse );
    
    iObservers = new( ELeave ) CArrayPtrFlat<MMsgAsyncControlObserver>( KObserverArrayGranularity );
    
    iParser = CXhtmlParser::NewL( this );

    ResolveLayout();
    
    if ( aObserver )
        {
        iObservers->AppendL( aObserver );
        }
    }
#else
void CMsgXhtmlBodyControl::ConstructL( MMsgAsyncControlObserver* /*aObserver*/ )
    {
    User::Leave( KErrNotSupported );
    }
#endif


// ---------------------------------------------------------
// CMsgXhtmlBodyControl::LoadL
// ---------------------------------------------------------
//
#ifdef RD_MSG_XHTML_SUPPORT 
void CMsgXhtmlBodyControl::LoadL( RFile& aFileHandle )
    {
    RFile fileHandle;
    fileHandle.Duplicate( aFileHandle );
    
    iParser->CreateDomL( fileHandle ); // Takes ownership

    SetState( EMsgAsyncControlStateOpening );
    }
#else
void CMsgXhtmlBodyControl::LoadL( RFile& /*aFileHandle*/ )
    {
    }
#endif

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::Cancel
// ---------------------------------------------------------
//
#ifdef RD_MSG_XHTML_SUPPORT 
void CMsgXhtmlBodyControl::Cancel()
    {
    if ( iState == EMsgAsyncControlStateOpening )
        {
        iParser->Cancel();
        SetState( EMsgAsyncControlStateIdle );
        }
    }
#else
void CMsgXhtmlBodyControl::Cancel()
    {
    }
#endif

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::Close
// ---------------------------------------------------------
//
void CMsgXhtmlBodyControl::Close()
    {
    }

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::State
// ---------------------------------------------------------
//
TMsgAsyncControlState CMsgXhtmlBodyControl::State() const
    {
    return iState;
    }

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::Error
// ---------------------------------------------------------
//
TInt CMsgXhtmlBodyControl::Error() const
    {
    return iError;
    }
        
// ---------------------------------------------------------
// CMsgXhtmlBodyControl::AddObserverL
// ---------------------------------------------------------
//
void CMsgXhtmlBodyControl::AddObserverL( MMsgAsyncControlObserver& aObserver )
    {
    iObservers->AppendL( &aObserver );
    }
        
// ---------------------------------------------------------
// CMsgXhtmlBodyControl::RemoveObserver
// ---------------------------------------------------------
//
void CMsgXhtmlBodyControl::RemoveObserver( MMsgAsyncControlObserver& aObserver )
    {
    for ( TInt current = 0; current < iObservers->Count(); current++ )
        {
        if ( iObservers->At( current ) == &aObserver )
            {
            iObservers->Delete( current );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::ParseCompleteL
// ---------------------------------------------------------
//
void CMsgXhtmlBodyControl::ParseCompleteL()
    {
    ParseXhtmlL();
    
    if ( iState ==  EMsgAsyncControlStateOpening )
        {
        SetState( EMsgAsyncControlStateReady );
        }
    }

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::ParseError
// ---------------------------------------------------------
//
void CMsgXhtmlBodyControl::ParseError( TInt aError )
    {
    iError = aError;
    SetState( EMsgAsyncControlStateError );
    }

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::HandleResourceChange
// ---------------------------------------------------------
//
void CMsgXhtmlBodyControl::HandleResourceChange( TInt aType )
    {
    CMsgBodyControl::HandleResourceChange( aType );
        
	if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        ResolveLayout();
        
        if ( iState == EMsgAsyncControlStateReady )
            {
            TRAPD( error, ParseXhtmlL() );
            if ( error != KErrNone )
                {
                ParseError( error );
                }
            }
        }
    
    for ( TInt current = 0; current < iObservers->Count(); current++ )
        {
        iObservers->At( current )->MsgAsyncControlResourceChanged( *this, aType );
        }
    }

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::SetState
// ---------------------------------------------------------
//
void CMsgXhtmlBodyControl::SetState( TMsgAsyncControlState  aState )
    {
    TMsgAsyncControlState oldState = iState;
    iState = aState;
    
    for ( TInt current = 0; current < iObservers->Count(); current++ )
        {
        iObservers->At( current )->MsgAsyncControlStateChanged( *this, iState, oldState );
        }
    }

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::ResolveLayout
// ---------------------------------------------------------
//
#ifdef RD_MSG_XHTML_SUPPORT 
void CMsgXhtmlBodyControl::ResolveLayout()
    {
    TAknLayoutRect msgTextPane;
    msgTextPane.LayoutRect( MsgEditorCommons::MsgDataPane(),
                            AknLayoutScalable_Apps::msg_text_pane( 0 ) );
                            
    TAknLayoutRect msgBodyPane;
    msgBodyPane.LayoutRect( msgTextPane.Rect(),
                            AknLayoutScalable_Apps::msg_body_pane() );
    
    TAknLayoutText normalTextLayout;
    normalTextLayout.LayoutText( msgBodyPane.Rect(),
                                 AknLayoutScalable_Apps::msg_body_pane_t1( 0 ) );

    TAknLayoutText largeTextLayout;
    largeTextLayout.LayoutText( msgBodyPane.Rect(),
                                AknLayoutScalable_Apps::msg_body_pane_t1( 2 ) );

    TAknLayoutText smallTextLayout;
    smallTextLayout.LayoutText( msgBodyPane.Rect(),
                                AknLayoutScalable_Apps::msg_body_pane_t1( 3 ) );

    
    TRgb textColor = normalTextLayout.Color();
    if ( AknsUtils::AvkonSkinEnabled() )
        {
        AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                   textColor,
                                   KAknsIIDQsnTextColors,
                                   EAknsCIQsnTextColorsCG6 );
        
        }
        
    iParser->SetDefaultTextColor( textColor );
    iParser->SetFonts( largeTextLayout.Font(),
                       normalTextLayout.Font(),
                       smallTextLayout.Font(),
                       normalTextLayout.Font() );   
    }
#else
void CMsgXhtmlBodyControl::ResolveLayout()
    {
    }
#endif                      

// ---------------------------------------------------------
// CMsgXhtmlBodyControl::ParseXhtmlL
// ---------------------------------------------------------
//
#ifdef RD_MSG_XHTML_SUPPORT 
void CMsgXhtmlBodyControl::ParseXhtmlL()
    {
    CRichText& text = TextContent();
    text.Reset();
    
    iParser->SetMode( IsPlainTextMode(), IsPlainTextMode() );
    
    iParser->ParseL( text );
        
    CItemFinder* autoFinder = ItemFinder();
    
    TInt linkCount = iParser->HyperLinkCount();
    
    if ( autoFinder && linkCount > 0 )
        {
        CArrayPtrFlat<CItemFinder::CFindItemExt>* linkArray = 
                new( ELeave ) CArrayPtrFlat<CItemFinder::CFindItemExt>( linkCount );
        CleanupStack::PushL( TCleanupItem( CleanupLinkArray, linkArray ) );
        
        TInt startPos( 0 );
        TInt endPos( 0 );
        TPtrC address;
        
        for( TInt current = 0; current < linkCount; current++ )
            {
            User::LeaveIfError( iParser->HyperLink( current, startPos, endPos, address ) );
            
            CItemFinder::CFindItemExt* findItem = new( ELeave ) CItemFinder::CFindItemExt();
            CleanupStack::PushL( findItem );
            
            findItem->iItemType = CItemFinder::ENoneSelected;
            
            if ( address.Length() >= KWwwUrlAddress().Length() &&
                 address.Left( KWwwUrlAddress().Length() ).MatchF( KWwwUrlAddress ) != KErrNotFound )
                {
                findItem->iItemType = CItemFinder::EUrlAddress;    
                }
            else if ( address.Length() >= KSchemeAddress().Length() &&
                      address.FindF( KSchemeAddress() ) != KErrNotFound )
                {
                findItem->iItemType = CItemFinder::EUriScheme;
                }
            
            if ( findItem->iItemType != CItemFinder::ENoneSelected )
                {
                findItem->iItemDescriptor = address.AllocL();
                findItem->iStart = startPos;
                findItem->iEnd = endPos;
        
                linkArray->AppendL( findItem );
                CleanupStack::Pop( findItem );
                }
            else
                {
                CleanupStack::PopAndDestroy( findItem );
                }
            };
        
        User::LeaveIfError( autoFinder->SetExternalLinks( linkArray ) );
        CleanupStack::PopAndDestroy( linkArray );
        }
    
    Editor().SetAmountToFormatL( ETrue );
    }
#else
void CMsgXhtmlBodyControl::ParseXhtmlL()
    {
    }
#endif

// ----------------------------------------------------
// CMsgXhtmlBodyControl::CleanupLinkArray
// ----------------------------------------------------
//
#ifdef RD_MSG_XHTML_SUPPORT 
void CMsgXhtmlBodyControl::CleanupLinkArray( TAny* aArray )
    {
    CArrayPtrFlat<CItemFinder::CFindItemExt>* linkArray = static_cast<CArrayPtrFlat<CItemFinder::CFindItemExt>*>( aArray );
    
    linkArray->ResetAndDestroy();
    delete linkArray;
    }
#else
void CMsgXhtmlBodyControl::CleanupLinkArray( TAny* aArray )
    {
    }
#endif

//  End of File
