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
*     Encapsulates the data which populates the fields in the message 
*     info dialogs (email, sms, bios, mms), also holds the text for the 
*     labels and fields in the message info dialog and displays a message 
*     info dialog for particular message types
*
*/



// INCLUDE FILES
#include "muiumsginfo.h"
#include "muiumsginfo.hrh"
#include <badesca.h>
#include <muiu.rsg>
#include <aknutils.h>                    // AknLayoutUtils::FontFromId
#include <aknlayout.lag>                 // AKN_LAYOUT_TEXT_List_pane_texts__menu_single_heading__Line_2(0)
#include <AknLayoutDef.h>                // TAknTextLineLayout
#include <AknBidiTextUtils.h>            // ConvertToVisualAndWrapToArrayL
#include <AknPhoneNumberGrouping.h>      // CAknPhoneNumberGrouping
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include <AknAppUi.h>                    // iAvkonAppUi

// CONSTANTS
const TInt KWhitespaceBuffer = 1; 
const TInt KLoopStartingIndex = 0;
const TInt KMaxCharsInLine = 50; 	// needs to be longer than the max possible chars that could be in a line in the dialog
const TInt KMuiuArrayGranularity = 4;
const TInt KInfoArrayGranularity = 8;
const TInt KNumOfReplaceChars = 3; 	// for subject field whitebox removal
const TInt KMessageTypeLines = 4;	// specified max lines for message type field
const TInt KLineFeed = 0x0000000a; 	// unicode representation for linefeed.
const TInt KExtraPixels = 2;        // Extra pixels for label field (Value from AVKON LAF document)
const TInt KItemCount = 0;          // Magic
const TInt KTitleVisible = 1;       // Known value ( we always have title in message info)
const TInt KDateSize = 30;          // Size of Date and Time strings in Message info
 
_LIT( KDirAndMuiuResFileName,"muiu.rsc" );
_LIT(KWhitespace, " "); //must be just one char
_LIT(KNullString, "");


// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------
// CMsgInfoLabelAndTextArray::~CMsgInfoLabelAndTextArray
// ---------------------------------------------------------
//
CMsgInfoLabelAndTextArray::~CMsgInfoLabelAndTextArray()
    {
    delete iLabels;
    delete iTextFields;
    delete iLine;
    iFont = NULL;
    iLabelFont = NULL;
    }


// ---------------------------------------------------------
// CMsgInfoLabelAndTextArray::NewL
// ---------------------------------------------------------
//
CMsgInfoLabelAndTextArray* CMsgInfoLabelAndTextArray::NewL()
    {
    CMsgInfoLabelAndTextArray* self = new( ELeave ) CMsgInfoLabelAndTextArray;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }


// ---------------------------------------------------------
// CMsgInfoLabelAndTextArray::CMsgInfoLabelAndTextArray
// ---------------------------------------------------------
//
CMsgInfoLabelAndTextArray::CMsgInfoLabelAndTextArray() : iEnv( *CEikonEnv::Static() )
    {
    TAknTextLineLayout layout( AKN_LAYOUT_TEXT_List_pane_texts__menu_single_heading__Line_2(0) );
    iFont = AknLayoutUtils::FontFromId( layout.FontId() );
    TAknTextLineLayout labelLayout( AKN_LAYOUT_TEXT_List_pane_texts__menu_single_heading__Line_1 );
    iLabelFont = AknLayoutUtils::FontFromId( labelLayout.FontId() );
    }


// ---------------------------------------------------------
// CMsgInfoLabelAndTextArray::ConstructL
// ---------------------------------------------------------
//
void CMsgInfoLabelAndTextArray::ConstructL()
    {
    iLabels = new(ELeave)CDesCArrayFlat( KInfoArrayGranularity ); 
    iTextFields = new(ELeave)CDesCArrayFlat( KInfoArrayGranularity ); 
    iLine = HBufC::NewL( KMaxCharsInLine) ;
    }


// ---------------------------------------------------------
// CMsgInfoLabelAndTextArray::AddLabelAndTextL
// Add label text (identified by resource id) and the corresponding 
// field text to the array structure - may need to add multiple lines
// if the label text is long and aWrap is set to ETrue - this is done
// by making AddWrapLineToArrayL recursive
// ---------------------------------------------------------
//
void CMsgInfoLabelAndTextArray::AddLabelAndTextL(TInt aLabelResourceId, const TPtrC& aFieldText, TBool aWrap)
    {   
    HBufC* labelText = iEnv.AllocReadResourceLC( aLabelResourceId );
    HBufC* visualConvertedLabelStr = NULL;
    visualConvertedLabelStr = ConstructVisualConvertedStringLC( labelText->Des() );

    if( aLabelResourceId == R_MESSAGE_INFO_TYPE_LABEL )
        {
        AddFixedLinesWrapMessageTypeToArrayL( aFieldText, *visualConvertedLabelStr, KMessageTypeLines );
        }
    else if ( aWrap )
        {
        AddWrapMessageTypeToArrayL( aFieldText, *visualConvertedLabelStr );
        }
    else
        {
        iLabels->AppendL( *visualConvertedLabelStr );
        HBufC* visualConvertedFieldTextStr = NULL;
        visualConvertedFieldTextStr = ConstructVisualConvertedFieldTextStringLC( aFieldText );
        iTextFields->AppendL( *visualConvertedFieldTextStr );
        CleanupStack::PopAndDestroy( );
        }
    CleanupStack::PopAndDestroy( 2 ); //labelText, visualConvertedLabelStr
    }


// ---------------------------------------------------------
// CMsgInfoLabelAndTextArray::MdcaCount
//
// Return the total number of message info lines
//
// ---------------------------------------------------------
//
TInt CMsgInfoLabelAndTextArray::MdcaCount() const
    {
    return ( iLabels->Count() );
    }


// ---------------------------------------------------------
// CMsgInfoLabelAndTextArray::MdcaPoint
//
// Return the message info line (label and text separated by a delimiter)
// according to the index
// ---------------------------------------------------------
//
TPtrC CMsgInfoLabelAndTextArray::MdcaPoint( TInt aIndex ) const
    {   
    TPtr ptr = iLine->Des();

    ptr.Zero();
    ptr.Append( ( *iLabels )[aIndex] );
    ptr.Append( KColumnListSeparator );
    ptr.Append( ( *iTextFields )[aIndex] );
    return ptr;
    }


// ---------------------------------------------------------
// CMsgInfoLabelAndTextArray::AddFixedLinesWrapMessageTypeToArrayL
// Add label text and field text into info message array structure.
// Field text has max number of lines which is passed in parameter aMaxLines.
// If field text doesn't fit to these lines so text is truncated and added '...' to end.
// ---------------------------------------------------------
//
void CMsgInfoLabelAndTextArray::AddFixedLinesWrapMessageTypeToArrayL( const TDesC& aField, 
                                                                      const TDesC& aLabel, 
                                                                      TInt aMaxLines )
    {
    const TInt KPixelLineWidth( LineMaxWidthInPixels( ETrue ) );
    HBufC* string = 0;

    if( aField.Length() )
        {
        string = aField.AllocLC();  
        }
    else
        {
        string = HBufC::NewLC( KWhitespaceBuffer );
        string->Des().Append( KWhitespace );
        }

    CArrayFix<TInt>* widthArray = new ( ELeave ) CArrayFixFlat<TInt>( KMuiuArrayGranularity );
    CleanupStack::PushL( widthArray );
    widthArray->AppendL( KPixelLineWidth, aMaxLines );
    CArrayFix<TPtrC>* wrappedArray = new ( ELeave ) CArrayFixFlat<TPtrC>( KMuiuArrayGranularity );
    CleanupStack::PushL( wrappedArray );

    HBufC* convertBuff;
    if( ( string->Des().MaxLength() - string->Des().Length() ) < 
        ( widthArray->Count() * KAknBidiExtraSpacePerLine ) )
        {
        convertBuff = HBufC::NewLC( widthArray->Count() * KAknBidiExtraSpacePerLine + string->Des().Length() );
        }
    else
        {
        convertBuff = HBufC::NewLC( string->Des().MaxLength());
        }

    TPtr strPtr = convertBuff->Des();
    strPtr.Copy( *string );
    AknBidiTextUtils::ConvertToVisualAndWrapToArrayL( strPtr, *widthArray, *iFont, *wrappedArray, ETrue );

    const TInt lines = wrappedArray->Count();
    for ( TInt loop = KLoopStartingIndex; loop < lines; loop++ )
        {
        if ( loop == KLoopStartingIndex )
            {
            iLabels->AppendL( aLabel );
            }
        else
            {
            iLabels->AppendL( KWhitespace );
            }
        iTextFields->AppendL( ( *wrappedArray )[loop]);
        }

    CleanupStack::PopAndDestroy( 4 ); //convertBuff, wrappedArray, widthArray, string
    }


// ---------------------------------------------------------
// CMsgInfoLabelAndTextArray::AddWrapMessageTypeToArrayL
// Add label text and field text into info message array structure.
// If field text doesn't fit to one line then wraps text to lines so that all
// text is shown.
// ---------------------------------------------------------
//
void CMsgInfoLabelAndTextArray::AddWrapMessageTypeToArrayL( const TDesC& aField, const TDesC& aLabel )
    {
    const TInt KPixelLineWidth( LineMaxWidthInPixels( ETrue ) ); 
    HBufC* string = 0;

    if( aField.Length() )
        {
        string = aField.AllocLC();  
        }
    else
        {
        string = HBufC::NewLC( KWhitespaceBuffer );
        string->Des().Append( KWhitespace );
        }

    CArrayFix<TPtrC>* wrappedArray = new ( ELeave ) CArrayFixFlat<TPtrC>( KMuiuArrayGranularity );
    CleanupStack::PushL( wrappedArray );
    HBufC* visual = AknBidiTextUtils::ConvertToVisualAndWrapToArrayL(*string, KPixelLineWidth, *iFont, *wrappedArray);
    CleanupStack::PushL( visual );
    const TInt lines = wrappedArray->Count();
    for ( TInt loop = KLoopStartingIndex; loop < lines; loop++ )
        {
        if ( loop == KLoopStartingIndex )
            {
            iLabels->AppendL(aLabel);
            }
        else
            {
            iLabels->AppendL( KWhitespace );
            }
        iTextFields->AppendL( ( *wrappedArray )[loop]);
        }

    CleanupStack::PopAndDestroy( 3 ); //visual, wrappedArray, string
    }


// ---------------------------------------------------------
// CMsgInfoLabelAndTextArray::ConstructVisualConvertedStringLC
//
// Convert string in parameter "aLabelString" to visual order
// and clips it if necessary and return this visual
// converted string.
//
// ---------------------------------------------------------
//
HBufC* CMsgInfoLabelAndTextArray::ConstructVisualConvertedStringLC( const TDesC& aLabelString ) const
    {
    HBufC* convertedLabelStr = NULL;

    if( aLabelString.Length() )
        {
        convertedLabelStr = HBufC::NewLC( aLabelString.Length() + KAknBidiExtraSpacePerLine );
        convertedLabelStr->Des().Copy( aLabelString );
        }
    else
        {
        convertedLabelStr = HBufC::NewLC( KWhitespaceBuffer + KAknBidiExtraSpacePerLine );
        convertedLabelStr->Des().Copy( KWhitespace );
        }
    const TInt KMaxWidthInPixels( LineMaxWidthInPixels( EFalse ) );
    const TInt KMaxClippedWidthInPixels = KMaxWidthInPixels + KExtraPixels;
    
    TPtr convLabelStrPtr = convertedLabelStr->Des();
    AknBidiTextUtils::ConvertToVisualAndClipL( convLabelStrPtr, *iLabelFont , KMaxWidthInPixels, KMaxClippedWidthInPixels );
    return convertedLabelStr;
    }



// ---------------------------------------------------------
// CMsgInfoLabelAndTextArray::ConstructVisualConvertedFieldTextStringLC
//
// Convert string in parameter "aFieldTextString" to visual order
// and clips it if necessary and return this visual
// converted string.
//
// ---------------------------------------------------------
//
HBufC* CMsgInfoLabelAndTextArray::ConstructVisualConvertedFieldTextStringLC( const TDesC& aFieldTextString ) const
    {
    HBufC* convertedFieldTextStr = NULL;

    if( aFieldTextString.Length() )
        {
        convertedFieldTextStr = HBufC::NewLC( aFieldTextString.Length() + KAknBidiExtraSpacePerLine );
        convertedFieldTextStr->Des().Copy( aFieldTextString );
        }
    else
        {
        convertedFieldTextStr = HBufC::NewLC( KWhitespaceBuffer + KAknBidiExtraSpacePerLine );
        convertedFieldTextStr->Des().Copy( KWhitespace );
        }
    const TInt KMaxWidthInPixels( LineMaxWidthInPixels( ETrue ) );
    TPtr convFieldTextStrPtr = convertedFieldTextStr->Des();
    AknBidiTextUtils::ConvertToVisualAndClipL( convFieldTextStrPtr, *iFont, KMaxWidthInPixels, KMaxWidthInPixels );
    return convertedFieldTextStr;
    }


// ---------------------------------------------------------
// MsgInfoLabelAndTextArray::LineMaxWidthInPixels
//
// Return line max width in pixels in LAF situations:
// AKN_LAYOUT_TEXT_List_pane_texts__menu_single_heading__Line_2(0) and
// AKN_LAYOUT_TEXT_List_pane_texts__menu_single_heading__Line_1.
// If parameter aLine2Pixels is true then return line max width in pixels
// in LAF situation: AKN_LAYOUT_TEXT_List_pane_texts__menu_single_heading__Line_2(0)
// If parameter aLine2Pixels is false then return line max width in pixels
// in LAF situation: AKN_LAYOUT_TEXT_List_pane_texts__menu_single_heading__Line_1
// Because screen size can vary then also max line width in pixels can vary and
// this function is used in this situation.
// ---------------------------------------------------------
//
TInt CMsgInfoLabelAndTextArray::LineMaxWidthInPixels( TBool aLine2Pixels  ) const
    {      
    TRect main_pane = iAvkonAppUi->ClientRect();
    
    TAknLayoutRect popup_menu_graphic_window;
    popup_menu_graphic_window.LayoutRect( main_pane, AknLayout::popup_menu_graphic_window( KItemCount ) );
    
    TAknLayoutRect menu_heading_pane;
    menu_heading_pane.LayoutRect( popup_menu_graphic_window.Rect() , AknLayout::list_menu_heading_pane( KTitleVisible, KItemCount ) );
    
    TAknLayoutRect list_item;
    list_item.LayoutRect( menu_heading_pane.Rect() , AknLayout::list_single_heading_popup_menu_pane( KItemCount ) );
    
    TAknLayoutText text;
    if( aLine2Pixels )
        {
        text.LayoutText( list_item.Rect(), AKN_LAYOUT_TEXT_List_pane_texts__menu_single_heading__Line_2( 0 ) );
        }
    else
        {
        text.LayoutText( list_item.Rect(), AKN_LAYOUT_TEXT_List_pane_texts__menu_single_heading__Line_1 );
        }
    return text.TextRect().Width();
    }




////////////////////////////////////////////////////////////////////////////////////


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::~CMsgInfoMessageInfoDialog
// ---------------------------------------------------------
//
CMsgInfoMessageInfoDialog::~CMsgInfoMessageInfoDialog()
    {
    iResources.Close();
    delete iListBox;
    }


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::CMsgInfoMessageInfoDialog
// ---------------------------------------------------------
//
CMsgInfoMessageInfoDialog::CMsgInfoMessageInfoDialog() : iResources( *CEikonEnv::Static() )
    {}


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::ConstructL
// ---------------------------------------------------------
//
void CMsgInfoMessageInfoDialog::ConstructL()
    {
    TParse parse;
    parse.Set( KDirAndMuiuResFileName, &KDC_RESOURCE_FILES_DIR, NULL ); 
    TFileName fileName( parse.FullName() );
    iResources.OpenL( fileName );
    iListBox = new( ELeave )CAknSingleHeadingPopupMenuStyleListBox;
    }


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::NewL
// ---------------------------------------------------------
//
EXPORT_C CMsgInfoMessageInfoDialog* CMsgInfoMessageInfoDialog::NewL()
    {
    CMsgInfoMessageInfoDialog* self = new( ELeave ) CMsgInfoMessageInfoDialog;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::ExecuteLD
// Show the message info dialog.
// ---------------------------------------------------------
//
EXPORT_C void CMsgInfoMessageInfoDialog::ExecuteLD( TMsgInfoMessageInfoData& aFieldData, TMessageInfoType aMessageType)
    {
    TMsgInfoMessageInfoData convertedFieldData;

    CleanupStack::PushL( this );
    
    iMessageType = aMessageType;


    // Note: Lanquage specific number conversion done here.
    // This way dialog seem to open faster. (Number conversion and memory allocation take time)

    //From ----------------------------------------------------------------------
    HBufC* tmpFromString = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( aFieldData.iFrom );
    CleanupStack::PushL( tmpFromString );
    DoNumberConversionL( tmpFromString, convertedFieldData.iFrom );

    //Subject ----------------------------------------------------------------------
    HBufC* tmpSubjectString = aFieldData.iSubject.AllocLC();
    TPtr tmpSubjecPtr = tmpSubjectString->Des();
    TBuf<KNumOfReplaceChars> replaceChars;
    replaceChars.Zero();
    replaceChars.Append( CEditableText::EParagraphDelimiter );
    replaceChars.Append( CEditableText::ETabCharacter );
    replaceChars.Append( KLineFeed );
    AknTextUtils::ReplaceCharacters(
        tmpSubjecPtr, 
        replaceChars, 
        CEditableText::ESpace );
    if ( tmpSubjecPtr.Length() > 0 )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tmpSubjecPtr );
        }
    convertedFieldData.iSubject.Set( tmpSubjecPtr );

    //Callback ----------------------------------------------------------------------
    HBufC* tmpCallbackString = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( aFieldData.iCallback );
    CleanupStack::PushL( tmpCallbackString );
    DoNumberConversionL( tmpCallbackString, convertedFieldData.iCallback );
	
    //Status ----------------------------------------------------------------------
    HBufC* tmpStatusString = aFieldData.iStatus.AllocLC();
    TPtr tmpStatusPtr = tmpStatusString->Des();
    convertedFieldData.iStatus.Set( tmpStatusPtr );

    // Priority ------------------------------------------------------------------
    HBufC* tmpPriorityString = aFieldData.iPriority.AllocLC();
    TPtr tmpPriorityPtr = tmpPriorityString->Des();
    convertedFieldData.iPriority.Set( tmpPriorityPtr );

    //Type ----------------------------------------------------------------------
    HBufC* tmpTypeString = aFieldData.iType.AllocLC();
    DoNumberConversionL( tmpTypeString, convertedFieldData.iType );

    //Size ----------------------------------------------------------------------
    HBufC* tmpSizeString = aFieldData.iSize.AllocLC();
    DoNumberConversionL( tmpSizeString, convertedFieldData.iSize );

    //To----------------------------------------------------------------------
    HBufC* tmpToString = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( aFieldData.iTo );
    CleanupStack::PushL( tmpToString );
    DoNumberConversionL( tmpToString, convertedFieldData.iTo );

    //CC----------------------------------------------------------------------
    HBufC* tmpCCString = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( aFieldData.iCC );
    CleanupStack::PushL( tmpCCString );
    DoNumberConversionL( tmpCCString, convertedFieldData.iCC );

    //BCC----------------------------------------------------------------------
    HBufC* tmpBCCString = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( aFieldData.iBCC );
    CleanupStack::PushL( tmpBCCString );
    DoNumberConversionL( tmpBCCString, convertedFieldData.iBCC );
 
    CAknPopupList* popupList = CAknPopupList::NewL( iListBox, R_AVKON_SOFTKEYS_OK_EMPTY, AknPopupLayouts::EMenuGraphicHeadingWindow );
    CleanupStack::PushL( popupList );

    TInt flags = EAknListBoxViewerFlags;
    iListBox->ConstructL( popupList, flags );
    iListBox->CreateScrollBarFrameL( ETrue );   
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    convertedFieldData.iDateTime = aFieldData.iDateTime;
    SetInfoArrayL( convertedFieldData );
    SetTitleL( popupList );
    
    CleanupStack::Pop(); //popupList
    popupList->ExecuteLD();
    CleanupStack::PopAndDestroy( 10 ); //tmpFromString, tmpSubjectString, tmpBCCString,
    // tmpTypeString, tmpSizeString, tmpToString, tmpCCString, tmpCallbackString, tmpPriorityString
    // tmpStatusString
    CleanupStack::Pop(); //this
    delete this;
    }


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::SetInfoArrayL
// Construct the array of labels and text fields and add it to the listbox model
// ---------------------------------------------------------
//
void CMsgInfoMessageInfoDialog::SetInfoArrayL( TMsgInfoMessageInfoData& aFieldData )
    {
    TBuf<KDateSize> dateString;
    TBuf<KDateSize> timeString;
    TPtrC dateStr;
    TPtrC timeStr;

    CMsgInfoLabelAndTextArray* infoArray = CMsgInfoLabelAndTextArray::NewL();
    CleanupStack::PushL( infoArray );
    
    //From -----------------------------------------------------------
    switch( iMessageType )
        {
    case EEmailViewer:
    case ESmsViewer:
    case EBiosViewer:
    case EMmsViewer:
        //deliberate fall through 
        if( aFieldData.iFrom.Length() )
            {
            infoArray->AddLabelAndTextL( R_MESSAGE_INFO_FROM_LABEL, aFieldData.iFrom );
            }
        break;
    default:
        break;
        };


    //Subject --------------------------------------------------
    switch(iMessageType)
        {
    case ESmsEditor:
    case ESmsViewer:
    case EBiosEditor:
    case EBiosViewer:
        //deliberate fall through - no wrap for sms subject
        if( aFieldData.iSubject.Length() )
            {
            infoArray->AddLabelAndTextL( R_MESSAGE_INFO_SUBJECT_LABEL, aFieldData.iSubject, EFalse );
            }
        break;
    default:
        if( aFieldData.iSubject.Length() )
            {
            infoArray->AddLabelAndTextL( R_MESSAGE_INFO_SUBJECT_LABEL, aFieldData.iSubject );
            }
        break;
        };

    //Callback ------------------------------------------------------------------
    // CDMA only
    switch( iMessageType )
        {
    case ESmsEditor:
    case ESmsViewer:
        infoArray->AddLabelAndTextL(R_MESSAGE_INFO_CALLBACK_LABEL,aFieldData.iCallback);
        break;
    default:
        break;
        };

    //Status ----------------------------------------------------------------------
    //Only for CDMA MO messages
    //
    switch( iMessageType )
        {
    case ESmsEditor:
        //deliberate fall through - no wrap for sms subject
        infoArray->AddLabelAndTextL(R_MESSAGE_INFO_STATUS_LABEL,aFieldData.iStatus,EFalse);
        break;
    default:
        break;
        };

    //Priority ----------------------------------------------------------------------
    switch( iMessageType )
        {
    case ESmsEditor:
    case ESmsViewer:
        infoArray->AddLabelAndTextL(R_MESSAGE_INFO_PRIORITY_LABEL,aFieldData.iPriority,EFalse);
        break;
    default:
        break;
        };

    //Date and Time -------------------------------------------------------------

    GetFormattedTimeAndDateStringsL( aFieldData.iDateTime, dateString, timeString );
    HBufC* tmpDateString = dateString.AllocLC();
    DoNumberConversionL( tmpDateString, dateStr );
    HBufC* tmpTimeString = timeString.AllocLC();
    DoNumberConversionL( tmpTimeString, timeStr );

    //Date ------------------------------------------------------------------
    if( dateStr.Length() )
        {
        infoArray->AddLabelAndTextL( R_MESSAGE_INFO_DATE_LABEL, dateStr );
        }

    //Time ------------------------------------------------------------------
    if( timeStr.Length() )
        {
        infoArray->AddLabelAndTextL( R_MESSAGE_INFO_TIME_LABEL, timeStr );
        }

    CleanupStack::PopAndDestroy( 2 ); //tmpDateString, tmpTimeString


    //Type ----------------------------------------------------------------------
    if( aFieldData.iType.Length() )
        {
        infoArray->AddLabelAndTextL( R_MESSAGE_INFO_TYPE_LABEL, aFieldData.iType );
        }


    //Size ----------------------------------------------------------------------
    switch( iMessageType )
        {
    case EEmailViewer:
    case EEmailEditor:
    case EMmsViewer:
    case EMmsEditor:
        //deliberate fall through
        if( aFieldData.iSize.Length() )
            {
            infoArray->AddLabelAndTextL( R_MESSAGE_INFO_SIZE_LABEL, aFieldData.iSize );
            }
        break;
    default:
        break;
        };


    //To ----------------------------------------------------------------------
    switch( iMessageType )
        {
    case ESmsViewer:
    case EBiosViewer:
        //deliberate fall through       
        break;
    case EEmailEditor:
    case ESmsEditor:
    case EBiosEditor:
    case EMmsViewer:
    case EMmsEditor:
        //deliberate fall through
        //All editors and mms viewer show always "To" label is "aFieldData.iTo" empty or not
        infoArray->AddLabelAndTextL( R_MESSAGE_INFO_TO_LABEL, aFieldData.iTo );       
        break;
    default:
        if( aFieldData.iTo.Length() )
            {
            infoArray->AddLabelAndTextL( R_MESSAGE_INFO_TO_LABEL, aFieldData.iTo );
            }
        break;
        };

    //Cc ----------------------------------------------------------------------
    switch( iMessageType )
        {
    case EEmailViewer:
    case EEmailEditor:
    case EMmsViewer:
    case EMmsEditor:
        //deliberate fall through
        if( aFieldData.iCC.Length() )
            {
            infoArray->AddLabelAndTextL( R_MESSAGE_INFO_CC_LABEL, aFieldData.iCC );
            }
        break;
    default:
        break;
        };

    //Bcc ----------------------------------------------------------------------
    switch( iMessageType )
        {
    case EEmailViewer:
    case EEmailEditor:
    case EMmsEditor:
        //deliberate fall through
        if( aFieldData.iBCC.Length() )
            {
            infoArray->AddLabelAndTextL( R_MESSAGE_INFO_BCC_LABEL, aFieldData.iBCC );
            }
        break;
    default:
        break;
        };


    // -------------------------------------------------------------------------

    CleanupStack::Pop(); //infoArray
    iListBox->UseLogicalToVisualConversion(EFalse);
    iListBox->Model()->SetItemTextArray( infoArray ); //owns the array
    }


// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::SetTitleL
// Set up the title.
// ---------------------------------------------------------
//
void CMsgInfoMessageInfoDialog::SetTitleL( CAknPopupList* aPopupList ) const
    {
    if( aPopupList != NULL)
        {
        HBufC* title = CEikonEnv::Static()->AllocReadResourceLC( R_MESSAGE_INFO_TITLE );
        aPopupList->SetTitleL( *title );
        CleanupStack::PopAndDestroy(); //title
        }
    }


// ---------------------------------------------------------
// Convert between arabic-indic digits and european digits based on existing language setting.
// So it'll convert any digit from the string
// to use either european digits or arabic-indic digits based on current settings.
// @param aFieldString: Data buffer used in conversion.
// @param aFieldData: Return converted data in this parameter.
// ---------------------------------------------------------
//
void CMsgInfoMessageInfoDialog::DoNumberConversionL( HBufC* aFieldString, TPtrC& aFieldData ) const
    {
    if( aFieldString != NULL)
        {
    	TPtr tmpPtr = aFieldString->Des();
    	if ( tmpPtr.Length() > 0 )
            {
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tmpPtr );
            }
        aFieldData.Set( tmpPtr );
        }
    else
        {
        aFieldData.Set(KNullString);
        }
    }


// ---------------------------------------------------------
// Creates formatted date and time strings and return these strings in parameters
// aDateBuf and aTimeBuf. Function get UTC time in parameter aUTCtime and
// constructs date and time strings with this UTC time value.
// @param aUTCtime: UTC time used in time conversion.
// @param aDateBuf: Return date string after time conversion.
// @param aTimeBuf: Return time string after time conversion. 
//
// ---------------------------------------------------------
//
void CMsgInfoMessageInfoDialog::GetFormattedTimeAndDateStringsL( TTime& aUTCtime, 
                                                                 TDes& aDateBuf, 
                                                                 TDes& aTimeBuf )
    {
    TLocale locale;
    
    // If home daylight saving in effect, add one hour offset. 
    if ( locale.QueryHomeHasDaylightSavingOn() )
        {
        TTimeIntervalHours daylightSaving( 1 );
        aUTCtime+=daylightSaving;
        }
    
    //Set date and time
    HBufC* dateFormat = CEikonEnv::Static()->AllocReadResourceLC( R_QTN_DATE_USUAL_WITH_ZERO );//from avkon resources
    aUTCtime.FormatL( aDateBuf, *dateFormat );
    CleanupStack::PopAndDestroy();//dateFormat
    
    HBufC* timeFormat = CEikonEnv::Static()->AllocReadResourceLC( R_QTN_TIME_USUAL_WITH_ZERO );//from avkon resources
    aUTCtime.FormatL( aTimeBuf, *timeFormat );
    CleanupStack::PopAndDestroy();//timeFormat
    }


// End of file
