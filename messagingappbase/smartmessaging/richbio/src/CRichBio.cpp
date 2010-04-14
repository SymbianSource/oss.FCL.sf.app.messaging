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
*     This is a wrapper class for rich text editor.
*
*/



// INCLUDES

#include "CRichBio.h"                       // Own header
#include <txtrich.h>                        // CRichText
#include <calslbs.h>                        // Fonts
#include <MsgExpandableControlEditor.h>     // CMsgExpandableControlEditor
#include <MsgEditorCommon.h>                // MaxBodyHeight
#include <aknlayoutdef.h>       // TAknLayoutId
#include <AknUtils.h>

#include    <applayout.cdl.h> // LAF
#include    <aknlayoutscalable_apps.cdl.h>

/// panic codes
enum TRichBioPanicCodes
    {
    EPanicCacheProblem1 = 100,
    EPanicCacheProblem2,
    EPanicCacheMismatch1,
    EPanicCacheMismatch2,
    EPanicCacheMismatch3,
    EPanicEditorIsNull1,
    EPanicEditorIsNull2,
    EPanicNoCache,
    EPanicConstructedTwice,
    EPanicNoParent
    };

// LOCAL CONSTANTS

// Define the margins used in editor here.
const TInt KMarginTop = 0;
const TInt KMarginBottom = 0;
const TInt KMarginLeft = 3;
const TInt KMarginRight = 1;

// CLASS DECLARATION

/**
* CRichbio inner class, which implements the observer
* interface.
*/
class CRichBio::CEdwinSizeObserver : public CBase,
    public MEikEdwinSizeObserver
    {
    public: // from MEikEdwinSizeObserver

        /**
        * Constructor.
        * @param Reference to the richbio object.
        */
        CEdwinSizeObserver( CRichBio& aRichBio );

        /**
         * Destructor.
         */
        ~CEdwinSizeObserver() {};

        /**
         * Handles edwin size event. Sets new size for this control and
         * reports the observer if the control editor's size is about to
         * change. Returns ETrue if size change is allowed.
         * @param aEdwin Edwin control.
         * @param aEventType Type of the event.
         * @param aDesirableEdwinSize control's desired size.
         * @return Boolean
         */
        TBool HandleEdwinSizeEventL(CEikEdwin* aEdwin,
            TEdwinSizeEvent aEventType,
            TSize aDesirableEdwinSize);
    private:
        /// Reference to the richbio control.
        CRichBio& iRichBio;
    };

CRichBio::CEdwinSizeObserver::CEdwinSizeObserver( CRichBio& aRichBio ) :
    iRichBio( aRichBio )
    {
    }

// ---------------------------------------------------------
// CEdwinSizeObserver::HandleEdwinSizeEventL
//
// Handles edwin size event. Sets new size for this control and reports the
// observer if the control editor's size is about to change. Returns ETrue
// if size change is allowed.
// ---------------------------------------------------------
//
TBool CRichBio::CEdwinSizeObserver::HandleEdwinSizeEventL(
    CEikEdwin* /*aEdwin*/,
    TEdwinSizeEvent aEventType,
    TSize aDesirableEdwinSize)
    {
    if (aEventType == EEventSizeChanging)
        {
        TSize thisSize(iRichBio.Size());
        aDesirableEdwinSize.iHeight = MsgEditorCommons::MaxBodyHeight();

        TInt delta( aDesirableEdwinSize.iHeight - iRichBio.iEditor->Size().iHeight );

        if (delta)
            {
            thisSize.iHeight = aDesirableEdwinSize.iHeight;
            iRichBio.SetSizeWithoutNotification(thisSize);
            iRichBio.iEditor->SetSize(aDesirableEdwinSize);
            }
        return ETrue;

        }
    return EFalse;
    }

EXPORT_C CRichBio* CRichBio::NewL(const CCoeControl* aParent,
    TRichBioMode aMode)
    {
    CRichBio* self = new (ELeave) CRichBio( aMode );
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );
    return self;
    }

CRichBio::~CRichBio()
    {
    delete iEdwinSizeObserver;
    delete iEditor;
    delete iLabelCache;
    delete iValueCache;
    }

EXPORT_C void CRichBio::AddItemL(const TDesC& aLabel, const TDesC& aValue)
    {
    if (iEditor)
        {
        AddItemToRichTextL(aLabel, aValue);
        }
    // Items needs to be always cached, because of possible layout changes.
    CacheItemL(aLabel, aValue);
    }

EXPORT_C TRect CRichBio::CurrentLineRect()
    {
    TRect lineRect(0,0,0,0);
	// if fails return 0x0 rect
    // this method is part of public SDK and this cannot be changed to leaveable.
    TRAP_IGNORE((lineRect = iEditor->CurrentLineRectL()));
    return lineRect;
    }

EXPORT_C CEikRichTextEditor& CRichBio::Editor()
    {
    __ASSERT_ALWAYS(iEditor, Panic(EPanicEditorIsNull1));
    return *iEditor;
    }

EXPORT_C TBool CRichBio::IsEditorBaseMode() const
    {
    return (ERichBioModeEditorBase == iMode);
    }

EXPORT_C void CRichBio::Reset()
    {
    iEditor->Reset();
    iIsFirstItem = ETrue;
    TRAP_IGNORE(LoadFromCacheL());
    }

void CRichBio::AddItemToRichTextL(const TDesC& aLabel, const TDesC& aValue)
    {
    TCharFormat format;
    TCharFormatMask mask;

    if ( aLabel.Length() > 0 )
        {
        ApplyLabelFormat( format, mask );
        AppendTextL( aLabel , format, mask );
        }

    if ( aValue.Length() > 0 )
        {
        ApplyValueFormat( format, mask );
        AppendTextL( aValue , format, mask );
        }
    }

void CRichBio::AppendTextL( const TDesC& aText,
    const TCharFormat& aFormat,
    const TCharFormatMask& aMask )
    {
    // Create the modifiable descriptor
    HBufC* buf = HBufC::NewLC( aText.Length() );
    TPtr modifiedText = buf->Des();
    modifiedText = aText;

	// This is for example for arabic numbers
	AknTextUtils::DisplayTextLanguageSpecificNumberConversion( modifiedText );
    TInt pos(0);

    /*
    We have four cases, where specific character(s) must be
    converted to the CRichText understandable linebreak character
    (CEditableText::ELineBreak).
    1)  Text contains ascii code linefeed (LF) character 0x0A, which
        is converted to linebreak.
    2)  Text contains ascii code carrier return (CR) character 0x0D
    3)  Text contains both LF and CR characters particularly in this order.
        These two characters must be converted to one linebreak.
    */

    TUint LF(0x0A); // Linefeed character in ascii set
    TUint CR(0x0D); // Carriage return character in ascii set


	//this implementation won't handle mixed set of CRLF / LFCR / LF / CR 
	//linefeeds "quite" properly but that situation is not very likely
	
    // Case 1 and 3
    while ( ( pos = modifiedText.Locate( LF ) ) != KErrNotFound ) // Locate LF
        {
        if ( pos + 1 < modifiedText.Length() && CR == modifiedText[pos + 1] ) // Is the next CR?
            {
            modifiedText.Delete( pos + 1, 1 ); // Delete if CR
            }
        modifiedText[pos] = CEditableText::ELineBreak; // Replace LF with linebreak
        
        //as added measure we will check the previous character 
        if ( (pos-1) >= 0 && CR == modifiedText[pos - 1]  ) // Is the previous CR?
            {
            modifiedText.Delete( pos -1 , 1 ); // Delete if CR
            }
        }

    // Case 2
    while ( ( pos = modifiedText.Locate( CR ) ) != KErrNotFound ) // Locate CR
        {
		if ( pos + 1 < modifiedText.Length() && LF == modifiedText[pos + 1]  ) // Is the next LF?
            {
            modifiedText.Delete( pos + 1, 1 ); // Delete if LF
            }
        modifiedText[pos] = CEditableText::ELineBreak; // Replace CR with linebreak
        
        //as added measure we will check the previous character 
        if ( (pos-1) >= 0 && LF == modifiedText[pos - 1 ] ) // Is the previous LF?
            {
            modifiedText.Delete( pos -1, 1 ); // Delete if LF
            }
        }

    // Append the modified text to the richtext.
    CRichText* rich = iEditor->RichText();
    User::LeaveIfNull( rich );

    TInt documentPos = rich->DocumentLength();

    // Every time when text is added, the cursor is left at the end of the line.
    // When the new text is added we must first add linebreak and then the text
    // linebreak is not added if the text is first item.
    if ( !iIsFirstItem )
        {
        // Append the linebreak to the end of the richtext.
        rich->InsertL(documentPos, CEditableText::ELineBreak);
        }
    else
        {
        iIsFirstItem = EFalse;
        }

    documentPos = rich->DocumentLength();
    rich->SetInsertCharFormatL(aFormat, aMask, documentPos);
    rich->InsertL( documentPos, modifiedText );

    rich->CancelInsertCharFormat();

    CleanupStack::PopAndDestroy( buf );
    }

TKeyResponse CRichBio::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    return iEditor->OfferKeyEventL(aKeyEvent, aType);
    }

EXPORT_C void CRichBio::SetAndGetSizeL(TSize& aSize)
    {
    TInt maxHeight( MsgEditorCommons::MaxBodyHeight() );
    iEditor->SetMaximumHeight( maxHeight );
    
    iEditor->SetAndGetSizeL( aSize );
    

    if ( aSize.iHeight > maxHeight )
        {
        aSize.iHeight = maxHeight;
        }
    SetSizeWithoutNotification( aSize );
    }

EXPORT_C TInt CRichBio::VirtualHeight()
    {
    return iEditor->VirtualHeight();
    }

EXPORT_C TInt CRichBio::VirtualVisibleTop()
    {
    return iEditor->VirtualVisibleTop();
    }

EXPORT_C TBool CRichBio::IsCursorLocation(TMsgCursorLocation aLocation) const
    {
    TBool location(EFalse);
    switch (aLocation)
        {
        case EMsgTop:
            location = iEditor->CursorInFirstLine();
            break;
        case EMsgBottom:
            location = iEditor->CursorInLastLine();
            break;
        default:
            break;
        }

    return location;
    }

void CRichBio::ApplyLabelFormat(TCharFormat& aFormat, TCharFormatMask& aMask)
    {
    ApplyFormat(aFormat, aMask, ETrue);
    }

void CRichBio::ApplyValueFormat(TCharFormat& aFormat, TCharFormatMask& aMask)
    {
    ApplyFormat(aFormat, aMask, EFalse);
    }

void CRichBio::ApplyFormat(TCharFormat& aFormat, TCharFormatMask& aMask, TBool aIsLabel)
    {
	if ( aIsLabel )
		{
		TAknTextComponentLayout msg_body_pane_t2 =
			AknLayoutScalable_Apps::msg_body_pane_t2( 0 );
		TAknTextLineLayout textLayout = msg_body_pane_t2.LayoutLine();//AppLayout::Smart_Messages_Line_1( 0 );
	    const CFont* labelFont = AknLayoutUtils::FontFromId( textLayout.FontId() );
		aFormat.iFontSpec = labelFont->FontSpecInTwips();
		}
	else
		{
		TAknTextComponentLayout msg_body_pane_t3 =
			AknLayoutScalable_Apps::msg_body_pane_t3( 0 );
		TAknTextLineLayout editorLayout = msg_body_pane_t3.LayoutLine(); //AppLayout::Smart_Messages_Line_2( 0 );
		const CFont* editorFont = AknLayoutUtils::FontFromId( editorLayout.FontId() );
		aFormat.iFontSpec = editorFont->FontSpecInTwips();
		}

    aMask.ClearAll();
    aMask.SetAttrib(EAttFontStrokeWeight);
    aMask.SetAttrib(EAttFontHeight);
    aMask.SetAttrib(EAttFontTypeface);
    }

EXPORT_C CRichBio::CRichBio(TRichBioMode aMode) : iMode(aMode)
    {
    }

EXPORT_C void CRichBio::ConstructL(const CCoeControl* aParent)
    {
    __ASSERT_DEBUG(!iEditor, Panic( EPanicConstructedTwice ));
    TUint32 flags = 0;
    iEditor = new (ELeave) CMsgExpandableControlEditor( aParent, flags, NULL);
    iEditor->ConstructL();

    TAknLayoutText headerTextLayout;
    TMargins8 edwinMargins;
    if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
        TAknLayoutRect msgTextPane;
        msgTextPane.LayoutRect(
            MsgEditorCommons::MsgDataPane(),
            AknLayoutScalable_Apps::msg_text_pane( 0 ).LayoutLine() );

        TAknLayoutRect msgBodyPane;
        msgBodyPane.LayoutRect(
            msgTextPane.Rect(),
            AknLayoutScalable_Apps::msg_body_pane().LayoutLine() );
        headerTextLayout.LayoutText(
            msgBodyPane.Rect(),
            AknLayoutScalable_Apps::msg_body_pane_t2( 0 ).LayoutLine() );

	    edwinMargins.iTop    = KMarginTop;
	    edwinMargins.iBottom = KMarginBottom;
	    edwinMargins.iLeft   = headerTextLayout.TextRect().iTl.iX; //KMarginLeft;
	    edwinMargins.iRight  =
	    	msgTextPane.Rect().iBr.iX -
	    	headerTextLayout.TextRect().iBr.iX;// KMarginRight;
        }
	else
		{
		// use old margins
		edwinMargins.iTop    = KMarginTop;
		edwinMargins.iBottom = KMarginBottom;
		edwinMargins.iLeft   = KMarginLeft;
		edwinMargins.iRight  = KMarginRight;
		}
    iEditor->SetBorderViewMargins(edwinMargins);

    // Create the observer.
    iEdwinSizeObserver = new (ELeave) CEdwinSizeObserver( *this );
    iEditor->SetEdwinSizeObserver( iEdwinSizeObserver );
    iIsFirstItem = ETrue;
    iEditor->SetReadOnly(ETrue);
    if (CacheExists())
        {
        LoadFromCacheL();
        }
    SetContainerWindowL(*aParent);
    }

TInt CRichBio::CountComponentControls() const
    {
    return 1; // iEditor ( created in constructL, so always 1 )
    }

CCoeControl* CRichBio::ComponentControl(TInt aIndex) const
    {
    if (aIndex == 0)
        {
        return iEditor;
        }
    return NULL;
    }

void CRichBio::SizeChanged()
    {
    iEditor->SetExtent(Position(), iEditor->Size());
    }

void CRichBio::Panic( TInt aPanic )
    {
    _LIT(KDll, "richbio.dll");
    User::Panic(KDll, aPanic);
    }

TBool CRichBio::CacheExists()
    {
    if (iLabelCache)
        {
        __ASSERT_DEBUG(iValueCache, Panic(EPanicCacheProblem1));
        return ETrue;
        }
    __ASSERT_DEBUG(!iValueCache, Panic(EPanicCacheProblem2));
    return EFalse;
    }

void CRichBio::LoadFromCacheL()
    {
    __ASSERT_DEBUG(iLabelCache && iValueCache, Panic(EPanicNoCache));

    TInt count(iLabelCache->Count());
    __ASSERT_DEBUG(iValueCache->Count() == count,
        Panic(EPanicCacheMismatch1));
    for (TInt n(0); n < count; n++)
        {
        AddItemToRichTextL((*iLabelCache)[n], (*iValueCache)[n]);
        }
    }

void CRichBio::CacheItemL(const TDesC& aLabel, const TDesC& aValue)
    {
    if (!iLabelCache || !iValueCache)
        {
        delete iLabelCache;
        delete iValueCache;
        iLabelCache = NULL;
        iValueCache = NULL;
        iLabelCache = new (ELeave) CDesC16ArrayFlat(10); //granularity of 10
        iValueCache = new (ELeave) CDesC16ArrayFlat(10); //granularity of 10
        }
    iLabelCache->AppendL(aLabel);
    iValueCache->AppendL(aValue);

    __ASSERT_DEBUG(iLabelCache->Count() == iValueCache->Count(),
        Panic(EPanicCacheMismatch2));
    __ASSERT_DEBUG(iLabelCache->Count() > 0, Panic(EPanicCacheMismatch3));
    }
    
void CRichBio::Draw(const TRect& /*aRect*/) const
    {
    //removed the clearing of window background,causes flicker
    }

#ifdef RD_SCALABLE_UI_V2
EXPORT_C TInt CRichBio::ScrollL( TInt aPixelsToScroll, TMsgScrollDirection aDirection )
    {
    TInt pixelsToScroll( 0 );
    TInt marginPixels( 0 );

    CTextLayout* textLayout = iEditor->TextLayout();

    if ( aDirection == EMsgScrollDown )
        {
        TInt pixelsAboveBand( textLayout->PixelsAboveBand() );
        TInt bandHeight( textLayout->BandHeight() );
        TInt virtualHeight( iEditor->VirtualHeight() );

        if ( pixelsAboveBand + bandHeight + aPixelsToScroll >= virtualHeight )
            {
            pixelsToScroll = -( virtualHeight - ( pixelsAboveBand + bandHeight ) );

            if ( pixelsToScroll != 0 )
                {
                // Control has reaches end. Mark margin pixels scrolled.
                marginPixels = iEditor->Size().iHeight - Size().iHeight;
                }
            }
        else
            {
            // Negative pixels means scrolling down.
            pixelsToScroll = -aPixelsToScroll;
            }
        }
    else
        {
        pixelsToScroll = aPixelsToScroll;
        }

    if ( pixelsToScroll != 0 )
        {
		//scrolling to be restricted to full lines
		//so there will not be any clipping of text on view        
        textLayout->RestrictScrollToTopsOfLines( ETrue );
        iEditor->TextView()->ScrollDisplayPixelsL( pixelsToScroll );

        }

    return Abs( pixelsToScroll ) + marginPixels;
    }

EXPORT_C void CRichBio::NotifyViewEvent( TMsgViewEvent aEvent, TInt /*aParam*/ )
    {
    switch ( aEvent )
        {
        case EMsgViewEventPrepareFocusTransitionUp:
            {
            if ( IsFocused() )
                {
                //possible leaves are discarded
                TRAP_IGNORE( iEditor->ClearSelectionL() );
                }
            break;
            }
        case EMsgViewEventPrepareFocusTransitionDown:
            {
            if ( IsFocused() )
                {
                //possible leaves are discarded
                TRAP_IGNORE( iEditor->ClearSelectionL() );
                }
            break;
            }
        case EMsgViewEventSetCursorFirstPos:
            {
            if ( iEditor->TextView() )
            	{
            	if ( iEditor->IsFirstLineVisible()  )
                    {
                    //possible leaves are discarded
            		TRAP_IGNORE( iEditor->SetCursorPosL( 0, EFalse ) );
					}
				}
            break;
            }
        case EMsgViewEventSetCursorLastPos:
            {
            if ( iEditor->TextView() )
                {
                TInt len = iEditor->TextLength();

                if ( iEditor->IsLastLineVisible()  )
                    {
                    //possible leaves are discarded
                    TRAP_IGNORE( iEditor->SetCursorPosL( len, EFalse ) );
                    }
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
EXPORT_C TInt CRichBio::ScrollL( TInt /*aPixelsToScroll*/, 
                                 TMsgScrollDirection /*aDirection*/ )
    {
    return 0;
    }

EXPORT_C void CRichBio::NotifyViewEvent( TMsgViewEvent /*aEvent*/,
                                         TInt /*aParam*/ )
    {
    //no op
    }
    
#endif //RD_SCALABLE_UI_V2



//end of file

