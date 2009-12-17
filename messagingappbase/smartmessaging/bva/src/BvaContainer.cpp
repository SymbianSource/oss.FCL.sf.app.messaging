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
*     Container control class that will contain a Bio Control.
*  %version : %
*
*/



// INCLUDE FILES
#include <msgbiocontrol.h>
#include <MsgBioUids.h>
#include <msgbiocontrolfactory.h>
#include <bva.rsg>                  // resouce identifiers
#include "BvaAppUi.h"
#include "BvaContainer.h"
#include "bvalog.h"

#include <AknsBasicBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <featmgr.h>				// Feature manager

#include <AknUtils.h>                   // AknLayoutUtils
#include <aknlayoutscalable_apps.cdl.h> // LAF
#include <MsgEditorCommon.h>


//  LOCAL CONSTANTS AND MACROS

_LIT8(KBeginVCard,          "BEGIN:VCARD");
_LIT8(KBeginVCalendar,      "BEGIN:VCALENDAR");
_LIT8(KICalVersion,      	"VERSION:2.0");

_LIT(KBvaContainer,"CBvaContainer");

const TInt KNumberOfControls = 1;

// This is an estimate based on observed userid lengths in iCal messages
const TInt KICalLengthNeeded = 255;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Symbian OS two phased constructor
// ---------------------------------------------------------
//
void CBvaContainer::ConstructL(const TRect& aRect)
    {
    LOG("CBvaContainer::ConstructL begin");
    FeatureManager::InitializeLibL();

    CreateWindowL();
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
            KAknsIIDQsnBgAreaMainMessage, aRect, ETrue);

    SetSizeWithoutNotification(aRect.Size());
    SetBlank();
    CreateScrollBarL();
    iLineHeight = MsgEditorCommons::MsgBaseLineDelta();
    iBaseLineOffset = MsgEditorCommons::MsgBaseLineOffset();
    LOG("CBvaContainer::ConstructL end");
    }

void CBvaContainer::LoadBioControlL( const RFile& aFile )
    {
    LOG("CBvaContainer::LoadBioControlL begin");
    SetRect(iEikonEnv->EikAppUi()->ClientRect());
    ActivateL();
    delete iFactory;
    iFactory = NULL;
    iFactory = CMsgBioControlFactory::NewL();

    delete iBioControl;
    iBioControl = NULL;

    TUid bioUid = BioUidL( aFile );

    TRAPD(ret,iBioControl = iFactory->CreateControlL(
    	*static_cast<CBvaAppUi*>(iEikonEnv->EikAppUi()),
        bioUid,
        EMsgBioViewerMode,
        aFile ));

    if(ret == KErrNotFound && bioUid == KMsgBioUidICalendar)
        {
        iBioControl = iFactory->CreateControlL(
        *static_cast<CBvaAppUi*>(iEikonEnv->EikAppUi()),
        KMsgBioUidVCalendar,
        EMsgBioViewerMode,
        aFile );
        }
    else if(ret != KErrNone)
        {
        User::Leave(ret);
        }


    iBioControl->SetContainerWindowL(*this);
    iBioControl->ActivateL();
    UpdateScrollBarL();
    TSize size = Size();
    TInt scrollBarWidth = iScrollBar->ScrollBarBreadth(CEikScrollBar::EVertical);
    size.iWidth -= scrollBarWidth;
    iBioControl->SetAndGetSizeL(size);
    iBioControl->SetExtent(TPoint(0,0),iBioControl->Size());

    UpdateScrollBarL();
    DrawNow();
    LOG("CBvaContainer::LoadBioControlL end");
    }

// Destructor
CBvaContainer::~CBvaContainer()
    {
    FeatureManager::UnInitializeLib();
    delete iBioControl;
    delete iFactory;
    delete iScrollBar;
    delete iBgContext;
    }

void CBvaContainer::SizeChanged()
    {
    if ( iBgContext )
        {
        iBgContext->SetRect( Rect() );
        iBgContext->SetParentPos( PositionRelativeToScreen() );
       }
    if( iBioControl )
    	{
    	//the scrollbar width is deducted from the viewer width
    	//this ensures that the scrollbar is not blocked by it
    	TSize size = iEikonEnv->EikAppUi()->ClientRect().Size();
    	TInt scrollBarWidth = iScrollBar->ScrollBarBreadth(CEikScrollBar::EVertical);
    	size.iWidth -= scrollBarWidth;
    	TRAP_IGNORE(iBioControl->SetAndGetSizeL(size));
    	iBioControl->SetExtent(TPoint(0,0), iBioControl->Size());
    	TRAP_IGNORE( UpdateScrollBarL() );
    	}
    }

TInt CBvaContainer::CountComponentControls() const
    {
    TInt countScrollBarComponents( 0 );
    if ( iScrollBar )
        {
        countScrollBarComponents = iScrollBar->CountComponentControls();
        }
    if (!iBioControl)
        {
        return countScrollBarComponents;
        }
    return countScrollBarComponents + KNumberOfControls ;
    }

CCoeControl* CBvaContainer::ComponentControl(TInt aIndex) const
    {
    TInt countScrollBarComponents( 0 );
    if ( iScrollBar )
        {
        countScrollBarComponents = iScrollBar->CountComponentControls();
        }

    switch ( aIndex )
        {
        case 0:
            {
            if( iBioControl )
                {
                return iBioControl;
                }
            else
                {
                return iScrollBar->ComponentControl( aIndex );
                }
            }
        default:
            if (    iScrollBar
                &&  aIndex >= KNumberOfControls
                &&  aIndex < countScrollBarComponents + KNumberOfControls )
                {
                return iScrollBar->ComponentControl( aIndex - KNumberOfControls );
                }
            else
                {
                }
            return NULL;
         }
    }

TTypeUid::Ptr CBvaContainer::MopSupplyObject(TTypeUid aId)
	{
    if (aId.iUid == MAknsControlContext::ETypeId)
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject(aId);
	}

TKeyResponse CBvaContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    TKeyResponse keyResp( EKeyWasNotConsumed );
    if (!iBioControl)
        {
        return EKeyWasNotConsumed;
        }
    if(aKeyEvent.iCode == EKeyEnter	)
    {
    iEikonEnv->EikAppUi()->HandleCommandL(EAknSoftkeyContextOptions);	
    return EKeyWasConsumed;
    }
    keyResp = iBioControl->OfferKeyEventL(aKeyEvent, aType);
    UpdateScrollBarL();
    return keyResp;
    }

CMsgBioControl& CBvaContainer::BioControl()
    {
    __ASSERT_DEBUG(iBioControl, Panic(EBioControlNotExist));
    return *iBioControl;
    }

TUid CBvaContainer::BioUidL(const TFileName& aFileName, RFs& aFs )
    {
    RFile file;

    User::LeaveIfError(file.Open(aFs,aFileName,EFileRead));
    CleanupClosePushL(file); // file on CS
    return BioUidL( file );
    }


 TUid CBvaContainer::BioUidL( const RFile& aFile )
 	{
 	LOG("CBvaContainer::BioUidL begin");
 	TInt size;
    User::LeaveIfError(aFile.Size(size));
    if (!size)
        {
        // File reading failed.
        User::Leave(KErrNotFound);
        }

    HBufC8* dataBuf = HBufC8::NewLC(KICalLengthNeeded); // dataBuf on CS
    TPtr8 data(dataBuf->Des());
    User::LeaveIfError(aFile.Read(data, KICalLengthNeeded));
	TInt lengthNeeded = KBeginVCalendar().Length();

    // Compare file length to text string
    TInt length = dataBuf->Length();
    if (length < lengthNeeded)
        {
        User::Leave(KErrCorrupt);
        }

    // Compare beginning of read buffer with text strings.
    if ( dataBuf->Left( KBeginVCard().Length() ).CompareF(KBeginVCard) == 0)
        {
        CleanupStack::PopAndDestroy(dataBuf); // dataBuf
        return KMsgBioUidVCard;
        }
    else if (
    		dataBuf->FindF(KICalVersion) != KErrNotFound &&
        	dataBuf->FindF(KBeginVCalendar) != KErrNotFound
        	)
        {
        CleanupStack::PopAndDestroy(dataBuf); // dataBuf

        if ( FeatureManager::FeatureSupported( KFeatureIdMeetingRequestSupport ) )
        	{
          	return KMsgBioUidICalendar;	// we use ICalBC to launch the Meeting Request Viewers
        	}
      	else
        	{
          	return KMsgBioUidVCalendar;	// VCalBC is used to open iCal message
          	}
        }
    else if (
        dataBuf->FindF(KICalVersion) == KErrNotFound &&
        dataBuf->FindF(KBeginVCalendar) != KErrNotFound
        )
        {
        CleanupStack::PopAndDestroy(dataBuf); // dataBuf
        return KMsgBioUidVCalendar;
        }
    CleanupStack::PopAndDestroy(dataBuf); // dataBuf
    LOG("CBvaContainer::BioUidL end");
    User::Leave(KErrNotSupported);
    // Never reached.  A compilation warning is avoided if null TUid returned.
    return TUid::Null();
    }

void CBvaContainer::Panic(TInt aCode) const
    {
    User::Panic(KBvaContainer, aCode);
    }

void CBvaContainer::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );

    if( !AknsDrawUtils::Background( skin, cc, this, gc, aRect ) )
    	{
        // Same as CCoeControl draw for blank controls
        CGraphicsContext& gcBlank = SystemGc();
        gcBlank.SetPenStyle( CGraphicsContext::ENullPen );
        gcBlank.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gcBlank.DrawRect( aRect );
      }
	}

void CBvaContainer::HandleResourceChange( TInt aType )
	{
    CCoeControl::HandleResourceChange( aType );
    
    switch( aType )
		{
        case KEikDynamicLayoutVariantSwitch:
        	{
        	iLineHeight = MsgEditorCommons::MsgBaseLineDelta();
            iBaseLineOffset = MsgEditorCommons::MsgBaseLineOffset();

        	SetRect(iEikonEnv->EikAppUi()->ClientRect());

        	AknLayoutUtils::LayoutVerticalScrollBar(
                iScrollBar,
                TRect( TPoint( 0, 0 ), iEikonEnv->EikAppUi()->ClientRect().Size() ),
                AknLayoutScalable_Apps::scroll_pane_cp017().LayoutLine() );
    		SizeChanged();
			break;
			}
        default:
            // fall through
            break;
		}
	}

TBool CBvaContainer::IsBioControl()
    {
    if(iBioControl)
        return ETrue;
    else
        return EFalse;
    }

void CBvaContainer::CreateScrollBarL()
    {
    iScrollBar = new ( ELeave ) CEikScrollBarFrame(
        this,  // CCoeControl* aParentWindow
        this,  // MEikScrollBarObserver* aObserver
        ETrue  // TBool aPreAlloc=EFalse
        );

    // Check which type of scrollbar is to be shown
    CAknAppUiBase* appUi = static_cast<CAknAppUiBase*>( iEikonEnv->EikAppUi() );

    if ( AknLayoutUtils::DefaultScrollBarType( appUi ) ==
        CEikScrollBarFrame::EDoubleSpan )
        {
        // For EDoubleSpan type scrollbar
        // non-window owning scrollbar
        iScrollBar->CreateDoubleSpanScrollBarsL( EFalse, EFalse, ETrue, EFalse );
        iScrollBar->SetTypeOfVScrollBar( CEikScrollBarFrame::EDoubleSpan );
        AknLayoutUtils::LayoutVerticalScrollBar(
            iScrollBar,
            TRect( TPoint( 0, 0 ), iEikonEnv->EikAppUi()->ClientRect().Size() ),
            AknLayoutScalable_Apps::scroll_pane_cp017().LayoutLine() );
        }
    else
        {
        // For EArrowHead type scrollbar
        iScrollBar->SetTypeOfVScrollBar( CEikScrollBarFrame::EArrowHead );
        }

    iScrollBar->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    }

void CBvaContainer::UpdateScrollBarL()
    {
    TEikScrollBarModel horzModel;
    TEikScrollBarModel vertModel;
    TInt height;
    TInt pos;

    GetVirtualFormHeightAndPos( height, pos );

#ifdef _DEBUG
    // CAknScrollIndicator::SetPosition has an __ASSERT_DEBUG
    // for range check even if the control handles out-of-range
    // values properly.
    if ( pos > height ) pos = height;
    if ( pos < -1 )     pos = -1;
#endif

    vertModel.iScrollSpan = height;
    vertModel.iThumbSpan  = iEikonEnv->EikAppUi()->ClientRect().Height();

    if ( height > iEikonEnv->EikAppUi()->ClientRect().Height() )
        {
        vertModel.iThumbPosition = pos;
        }
    else
        {
        vertModel.iThumbPosition = 0;
        }

    TEikScrollBarFrameLayout layout;

    if ( iScrollBar && iScrollBar->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan )
        {
        // For EDoubleSpan type scrollbar
        if ( vertModel.iScrollSpan == vertModel.iThumbPosition )
            {
            vertModel.iThumbPosition--;
            }

        TAknDoubleSpanScrollBarModel hDsSbarModel( horzModel );
        TAknDoubleSpanScrollBarModel vDsSbarModel( vertModel );
        layout.iTilingMode = TEikScrollBarFrameLayout::EInclusiveRectConstant;

        // It seems to be important that we have separate
        // variable for "inclusiveRect" and "clientRect"
        TRect inclusiveRect( Rect() );
        TRect clientRect( Rect() );

        iScrollBar->TileL( &hDsSbarModel, &vDsSbarModel, clientRect, inclusiveRect, layout );
        iScrollBar->SetVFocusPosToThumbPos( vDsSbarModel.FocusPosition() );
        }
    else
        {
        TRect rect( Rect() );
        iScrollBar->TileL( &horzModel, &vertModel, rect, rect, layout );
        iScrollBar->SetVFocusPosToThumbPos( vertModel.iThumbPosition );
        }
    }

void CBvaContainer::GetVirtualFormHeightAndPos( TInt& aHeight, TInt& aPos )
    {
    aHeight = iBioControl->VirtualHeight();
    aPos = iBioControl->VirtualVisibleTop();
    }

#ifdef RD_SCALABLE_UI_V2
void CBvaContainer::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
    {
    switch ( aEventType )
        {
        case EEikScrollUp:
            {
            //ensure that we scroll 2 lines
            ScrollViewL(iLineHeight*2 , EMsgScrollUp, ETrue );
            break;
            }
        case EEikScrollDown:
            {
            //ensure that we scroll 2 lines
            ScrollViewL( iLineHeight*2, EMsgScrollDown, ETrue );
            break;
            }
        case EEikScrollTop:
            {
            // Not supported yet.
            break;
            }
        case EEikScrollBottom:
            {
            // Not supported yet.
            break;
            }
        case EEikScrollThumbReleaseVert:
            {
            // Not implemented yet
            break;
            }
        case EEikScrollPageUp:
        case EEikScrollPageDown:
        case EEikScrollThumbDragVert:
            {
            TInt scrolledPixels(iBioControl->VirtualVisibleTop() - aScrollBar->ThumbPosition());

			//the scrolled amount have to be manipulated to be atleast
			//one line height of pixels
			if(aScrollBar->ThumbPosition() + iBioControl->VirtualVisibleTop() <=
				iBioControl->VirtualHeight() - iLineHeight)
				{
            	if(Abs(scrolledPixels) >= iLineHeight)
                	{
                	scrolledPixels += ( scrolledPixels % (iLineHeight));
                	}
				else
					{
                	scrolledPixels -= iLineHeight;
                	}
				}
			else
				{
				scrolledPixels -= (iLineHeight);
				}

            if ( scrolledPixels != 0 )
                {
                ScrollViewL( Abs( scrolledPixels ),
                             scrolledPixels > 0 ? EMsgScrollUp :
                                                  EMsgScrollDown,
                             EFalse );
				UpdateScrollBarL();
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }
#else
void CBvaContainer::HandleScrollEventL( CEikScrollBar* /*aScrollBar*/,
                                        TEikScrollEvent /*aEventType*/)
    {
    }
#endif //RD_SCALABLE_UI_V2

const TAknDoubleSpanScrollBarModel* CBvaContainer::AknScrollBarModel() const
    {
    return static_cast<const TAknDoubleSpanScrollBarModel*>( iScrollBar->VerticalScrollBar()->Model() );
    }

#ifdef RD_SCALABLE_UI_V2
void CBvaContainer::ScrollViewL( TInt aPixelsToScroll,
                                  TMsgScrollDirection aDirection,
                                  TBool aMoveThumb )
    {
    if ( aMoveThumb )
        {
        const TAknDoubleSpanScrollBarModel* model = AknScrollBarModel();

        TInt directionMultiplier( -1 );
        if ( aDirection == EMsgScrollDown )
            {
            directionMultiplier = 1;
            }
        // Scroll bar thumb is moved only if caller requests it.
        iScrollBar->SetVFocusPosToThumbPos( model->FocusPosition() - 1 +
                                                directionMultiplier * aPixelsToScroll );
        }

    iBioControl->ScrollL( aPixelsToScroll, aDirection );
    }
#else
void CBvaContainer::ScrollViewL( TInt /*aPixelsToScroll*/,
                                  TMsgScrollDirection /*aDirection*/,
                                  TBool /*aMoveThumb*/ )
    {
    }
#endif //RD_SCALABLE_UI_V2

// End of File
