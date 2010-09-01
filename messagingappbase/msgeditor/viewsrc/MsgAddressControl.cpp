/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgAddressControl implementation
*
*/



// ========== INCLUDE FILES ================================

#include <eikenv.h>                        // for CEikonEnv
#include <eikappui.h>                      // for CEikAppUi
#include <barsread.h>                      // for TResourceReader
#include <eikedwin.h>                      // for TClipboardFunc
#include <txtrich.h>                       // for CRichText
#include <AknUtils.h>                      // for AknUtils
#include <aknbutton.h>                     // for CAknButton
#include <eiklabel.h>                      // for CEikLabel

#include "MsgAddressControl.h"             // for CMsgAddressControl
#include "MsgAddressControlEditor.h"       // for CMsgAddressControlEditor
#include "MsgAddressControlEditorField.h"  // for CMsgAddressControlEditorField
#include "MsgBaseControlObserver.h"        // for MMsgBaseControlObserver
#include "MsgRecipientItem.h"              // for CMsgRecipientItem
#include "MsgEditorPanic.h"                // for CMsgEditor panics
#include "MsgEditorCommon.h"               // for KArabicSemicolon

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

const TInt KRecipientArrayGranularity = 10;
const TInt KMaxEntryLength = 256;
const TUint KMsgListSeparator = ';';             // List separator

// Unicode sign for S60 "enter" symbol
const TInt KDownwardsArrowWithTipLeftwards  = 0x21B2;
const TInt KDownwardsArrowWithTipRightwards = 0x21B3;


// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================


// ---------------------------------------------------------
// CMsgAddressControl::CMsgAddressControl
//
// Constructor.
// ---------------------------------------------------------
//
CMsgAddressControl::CMsgAddressControl( MMsgBaseControlObserver& aBaseControlObserver ) : 
    CMsgExpandableControl( aBaseControlObserver )
    {
    }

// ---------------------------------------------------------
// CMsgAddressControl::~CMsgAddressControl
//
// Destructor.
// ---------------------------------------------------------
//
CMsgAddressControl::~CMsgAddressControl()
    {
    if ( iRecipientArray )
        {
        iRecipientArray->ResetAndDestroy();
        }

    delete iRecipientArray;
    }

// ---------------------------------------------------------
// CMsgAddressControl::AddRecipientL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgAddressControl::AddRecipientL( const TDesC& aName,
                                                 const TDesC& aAddress,
                                                 TBool aVerified,
                                                 MVPbkContactLink* aContactLink )
    {
    TInt count   = 0;
    TInt size    = 0;
    TInt entries = iAddressControlEditor->EntryCount();

    GetSizeOfAddresses( count, size );

    // when field is empty, count == 0 but entries == 1.
    // when count > 0, this inserts at the end of the field.
    if ( entries > 1 )
        {
        count = entries;
        }

    iAddressControlEditor->SetCursorPosAndCancelSelectionL(
        iAddressControlEditor->TextLength() );
    iAddressControlEditor->CheckHighlightingL();
    iAddressControlEditor->InsertEntryL(
        count,
        aName,
        aAddress,
        aVerified,
        ETrue,
        aContactLink
         );

    FormatAndSetCursorPosL( iAddressControlEditor->TextLength() );
    DrawDeferred();
    iControlModeFlags |= EMsgControlModeModified;
    }

// ---------------------------------------------------------
// CMsgAddressControl::AddRecipientsL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgAddressControl::AddRecipientsL(
    const CMsgRecipientList& aRecipients )
    {
    CMsgRecipientArray* recipients = GetRecipientsL();
    RefreshL( *recipients );

    TInt count = 0;
    TInt size = 0;
    TInt entries = iAddressControlEditor->EntryCount();

    GetSizeOfAddresses( count, size );

    // when field is empty, count == 0 but entries == 1.
    // when count > 0, this inserts at the end of the field.
    if ( entries > 1 )
        {
        count = entries;
        }

    TInt cc = aRecipients.Count();
    CMsgRecipientItem* item = NULL;

    iAddressControlEditor->SetCursorPosAndCancelSelectionL(
        iAddressControlEditor->TextLength() );
    iAddressControlEditor->CheckHighlightingL();

    TBool bandFormatting( EFalse );
    if ( iAddressControlEditor->TextLayout()->IsFormattingBand() )
        {
        bandFormatting = ETrue;
        }
        
    for ( TInt i = 0; i < cc; i++ )
        {
        item = aRecipients.At( i );
        iAddressControlEditor->InsertEntryL(
            count,
            *item->Name(),
            *item->Address(),
            item->IsVerified(),
            item->IsValidated(),
            item->ContactLink() );
        count++;
        }    
    
    TInt pos = iAddressControlEditor->TextLength();

    iAddressControlEditor->TextView()->SetDocPosL( pos, EFalse );
    
    FormatAndSetCursorPosL( pos );
    
    // Editor changed to band formatting due to address addition.
    // Set the editor height to maximum as text was added directly to
    // richtext and height is not updated correctly because of partial format.
    if ( !bandFormatting &&
         iAddressControlEditor->TextLayout()->IsFormattingBand() )
        {
        // Set newSize to current width and maximum body height and
        // force CMsgExpandableControl::HandleEdwinSizeChangeEventL to
        // update the control size.
        iControlModeFlags |= EMsgControlModeForceSizeUpdate;
            
        TSize newSize( iAddressControlEditor->Size().iWidth,
                       iMaxBodyHeight - 1 );
        
        iAddressControlEditor->SetSize( newSize );
        
        iControlModeFlags &= ~EMsgControlModeForceSizeUpdate;
        }
        
    DrawDeferred();
    iControlModeFlags |= EMsgControlModeModified;
    }


// ---------------------------------------------------------
// CMsgAddressControl::GetRecipientsL
//
//
// ---------------------------------------------------------
//
EXPORT_C CMsgRecipientArray* CMsgAddressControl::GetRecipientsL()
    {
    TInt entryNumber( iAddressControlEditor->EntryCount() );
    const CMsgAddressControlEditorField* field = NULL;
    TBuf<KMaxEntryLength> entryBuf;
    TInt ret( KErrNotFound );

    if ( iRecipientArray == NULL )
        {
        iRecipientArray = new ( ELeave )
            CMsgRecipientArray( KRecipientArrayGranularity );
        }
    else
        {
        iRecipientArray->ResetAndDestroy();
        }

    for ( TInt ii = 0; ii < entryNumber; ii++ )
        {
        field = iAddressControlEditor->Field( ii );

        if ( field && field->IsVerified() )
            {
            HBufC* addressCopy = field->Address()->AllocLC();
            TPtr addrTPtr = addressCopy->Des();
            AknTextUtils::ConvertDigitsTo( addrTPtr, EDigitTypeWestern );
            
            CMsgRecipientItem* item =
                CMsgRecipientItem::NewLC( *field->Name(), *addressCopy );

            const MVPbkContactLink* contactLink = field->ContactLink();
            if ( contactLink )                  
                {
                item->SetContactLink( contactLink );
                }
                
            item->SetVerified( field->IsVerified() );
            item->SetValidated( field->IsValidated() );
            iRecipientArray->AppendL( item );

            CleanupStack::Pop(); // item
            CleanupStack::PopAndDestroy(); // addressCopy
            }
        else
            {
            SetParsingInfo( ii, 0, 0 );
            ret = KErrNone;
            while ( ret == KErrNone )
                {
                ret = GetNextItemOnEntry( entryBuf );
                if ( ( ret == KErrNone ) && ( entryBuf.Length() > 0 ) )
                    {
                    AknTextUtils::ConvertDigitsTo( entryBuf, EDigitTypeWestern );
                    // unverified entries are treated as addresses.
                    CMsgRecipientItem* item =
                        CMsgRecipientItem::NewLC( KNullDesC, entryBuf );

                    iRecipientArray->AppendL( item );

                    CleanupStack::Pop(); // item
                    entryBuf.Zero();
                    }
                }
            }
        }

    ResetParsingInfo();
    return iRecipientArray;
    }

// ---------------------------------------------------------
// CMsgAddressControl::ResetL
//
// Clears content of the address control.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgAddressControl::ResetL()
    {
    ResetParsingInfo();
    TInt entryCount( iAddressControlEditor->EntryCount() );

    iAddressControlEditor->TurnHighlightingOffL();
    iAddressControlEditor->DeleteAllL();
    iControlModeFlags |= EMsgControlModeModified;

    return entryCount;
    }

// ---------------------------------------------------------
// CMsgAddressControl::Reset
//
// from CMsgBaseControl.
// Same as above but non-leaving version.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAddressControl::Reset()
    {
    TRAP_IGNORE( ResetL() );
    }

// ---------------------------------------------------------
// CMsgAddressControl::GetFirstUnverifiedStringL
//
// Finds the first unverified string and updates also the iParserInfo
// correspondingly.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgAddressControl::GetFirstUnverifiedStringL( HBufC*& aString )
    {
    __ASSERT_DEBUG( IsFocused(), Panic( EMsgControlNotFocused ) );

    ResetParsingInfo();
    return GetNextUnverifiedStringL( aString );
    }

// ---------------------------------------------------------
// CMsgAddressControl::GetNextUnverifiedStringL
//
// Finds the next unverified string.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgAddressControl::GetNextUnverifiedStringL( HBufC*& aString )
    {
    __ASSERT_DEBUG( IsFocused(), Panic( EMsgControlNotFocused ) );

    if ( aString != NULL )
        {
        delete aString;
        aString = NULL;
        }

    iAddressControlEditor->TurnHighlightingOffL();

    TInt entryCount = iAddressControlEditor->EntryCount();
    TInt entryNumber( iParsingInfo.iEntryNum );
    TBuf<KMaxEntryLength> entryBuf;
    TInt ret( KErrNotFound );
    HBufC* string = NULL;

    while ( entryNumber < entryCount )
        {
        entryBuf.Zero();
        if ( iAddressControlEditor->IsEntryVerified( entryNumber ) )
            {
            entryNumber++;
            SetParsingInfo( entryNumber, 0, 0 );
            }
        else
            {
            ret = GetNextItemOnEntry( entryBuf );
            if ( ( ret == KErrNone ) && ( entryBuf.Length() > 0 ) )
                {
                string = entryBuf.AllocLC();
                TInt firstPos = iAddressControlEditor->EntryStartPos(
                    iParsingInfo.iEntryNum );
                FormatAndSetCursorPosL( firstPos + iParsingInfo.iEndPos );
                break;
                }
            else if ( iParsingInfo.iEndPos > iParsingInfo.iStartPos )
                {
                // a string could not be found but there seems to be white
                // spaces on an entry which may be also followed by the list
                // separator, hence clean them up
                iAddressControlEditor->DeleteEntryCharsL(
                    iParsingInfo.iEntryNum,
                    iParsingInfo.iStartPos,
                    iParsingInfo.iEndPos );

                SetParsingInfo(
                    entryNumber,
                    iParsingInfo.iStartPos,
                    iParsingInfo.iStartPos );
                }
            else if ( ( iParsingInfo.iEndPos == 0 ) &&
                     ( iParsingInfo.iEntryNum < entryCount - 1 ) )
                {
                // there is just an EParagraphDelimiter on the entry
                iAddressControlEditor->DeleteEntryL( iParsingInfo.iEntryNum );
                SetParsingInfo( entryNumber, 0, 0 );
                entryCount--;
                }
            else
                {
                // there are no strings left on the current entry
                entryNumber++;
                SetParsingInfo( entryNumber, 0, 0 );
                }
            }
        }

    if ( entryNumber >= entryCount )
        {
        FormatAndSetCursorPosL( iAddressControlEditor->TextLength() );
        }

    aString = string;

    if ( string )
        {
        CleanupStack::Pop();  // string
        }

    return ret;
    }


// ---------------------------------------------------------
// CMsgAddressControl::ReplaceUnverifiedStringL
//
// Replaces a string with contents of the recipient array aArray. 
// The string is replaced according to the iParsingInfo.
//
// This function cannot undelete a string if it leaves after/while removing it.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgAddressControl::ReplaceUnverifiedStringL(
    CMsgRecipientArray& aArray )
    {
    __ASSERT_DEBUG( IsFocused(), Panic( EMsgControlNotFocused ) );

    if ( ( iParsingInfo.iEndPos <= iParsingInfo.iStartPos ) || 
        ( iParsingInfo.iEntryNum >= iAddressControlEditor->EntryCount() ) )
        {
        return KErrNotFound;
        }

    iAddressControlEditor->TurnHighlightingOffL();

    TInt entryLength = iAddressControlEditor->EntryLength( iParsingInfo.iEntryNum );
    iAddressControlEditor->DeleteEntryCharsL(
        iParsingInfo.iEntryNum,
        iParsingInfo.iStartPos,
        iParsingInfo.iEndPos );
   
    if ( ( iParsingInfo.iStartPos > 0 ) && ( iParsingInfo.iEndPos < entryLength ) )
        {
        iAddressControlEditor->InsertEntryBreakL(
            iParsingInfo.iEntryNum,
            iParsingInfo.iStartPos );
        iParsingInfo.iEntryNum++;
        }

    TInt entryNumber( iParsingInfo.iEntryNum );
    TInt contactCount = aArray.Count();
    CMsgRecipientItem* recipientItem = NULL;

    for ( TInt i = 0; i < contactCount; i++ )
        {
        recipientItem = aArray.At( i );
        iAddressControlEditor->InsertEntryL(
            entryNumber,
            *recipientItem->Name(),
            *recipientItem->Address(),
            recipientItem->IsVerified(),
            recipientItem->IsValidated(),
            recipientItem->ContactLink() );
            
        entryNumber++;
        }

    SetParsingInfo( iParsingInfo.iEntryNum, 0, 0 );
    TInt firstPos = iAddressControlEditor->EntryStartPos( iParsingInfo.iEntryNum );
    entryLength = iAddressControlEditor->EntryLength( iParsingInfo.iEntryNum );
    TInt endPos = firstPos + entryLength - 1;
    SetParsingInfo( iParsingInfo.iEntryNum, endPos, endPos );
    FormatAndSetCursorPosL( endPos );
    iControlModeFlags |= EMsgControlModeModified;
    return KErrNone;
    }

// ---------------------------------------------------------
// CMsgAddressControl::RefreshL
//
// Refreshes the contents of address control.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgAddressControl::RefreshL( const CMsgRecipientArray& aArray )
    {
    iEditor->SetEdwinSizeObserver( NULL );
    
    TRAPD( error, ResetL() );  // Clears the control and turns highlighting off
    
    iEditor->SetEdwinSizeObserver( this );
    
    if ( error != KErrNone )
        {
        User::Leave( error );
        }

    TInt entryNumber = 0;
    TInt contactCount = aArray.Count();
    CMsgRecipientItem* item = NULL;

    for ( TInt i = 0; i < contactCount; i++ )
        {
        item = aArray.At( i );
        iAddressControlEditor->InsertEntryL(
            entryNumber,
            *item->Name(),
            *item->Address(),
            item->IsVerified(),
            item->IsValidated(),
            item->ContactLink() );
        entryNumber++;
        }

    ResetParsingInfo();
    FormatAndSetCursorPosL( iAddressControlEditor->TextLength() );
    iControlModeFlags |= EMsgControlModeModified;

    return KErrNone;
    }

// ---------------------------------------------------------
// CMsgAddressControl::HighlightUnverifiedStringL
//
// Highlights the found unverified entry.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgAddressControl::HighlightUnverifiedStringL()
    {
    __ASSERT_DEBUG( IsFocused(), Panic( EMsgControlNotFocused ) );

    TInt ret = iParsingInfo.iEndPos - iParsingInfo.iStartPos > 0
        ? KErrNone
        : KErrNotFound;

    if ( IsActivated() && ( ret == KErrNone ) && IsFocused() )
        {
        TInt entryLength( 0 );  // not really needed
        TInt entryStart( iAddressControlEditor->RichText()->CharPosOfParagraph(
            entryLength, iParsingInfo.iEntryNum ) );
        FormatAndSetCursorPosL( entryStart + iParsingInfo.iEndPos );
        iAddressControlEditor->SetSelectionL(
            entryStart + iParsingInfo.iEndPos,
            entryStart + iParsingInfo.iStartPos );
        }

    return ret;
    }

// ---------------------------------------------------------
// CMsgAddressControl::SizeOfAddresses
//
// Returns size of addresses.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAddressControl::GetSizeOfAddresses(
    TInt& aEntryCount, TInt& aSizeOfAddresses )
    {
    const CMsgAddressControlEditorField* field = NULL;
    TBuf<KMaxEntryLength> entryBuf;
    TInt ret( KErrNotFound );

    aEntryCount = 0;
    aSizeOfAddresses = 0;

    for ( TInt entryNumber = iAddressControlEditor->EntryCount() - 1;
         entryNumber >= 0;
         entryNumber-- )
        {
        if ( iAddressControlEditor->IsEntryVerified( entryNumber ) )
            {
            field = iAddressControlEditor->Field( entryNumber );

            aSizeOfAddresses += field->Address()->Length();
            aEntryCount++;
            }
        else
            {
            SetParsingInfo( entryNumber, 0, 0 );
            ret = KErrNone;
            while ( ret == KErrNone )
                {
                ret = GetNextItemOnEntry( entryBuf );
                if ( ( ret == KErrNone ) && ( entryBuf.Length() > 0 ) )
                    {
                    aSizeOfAddresses += entryBuf.Length();
                    aEntryCount++;
                    entryBuf.Zero();
                    }
                }
            }
        }

    ResetParsingInfo();
    }

// ---------------------------------------------------------
// CMsgAddressControl::HighlightUnvalidatedStringL
//
// Highlights the found unvvalidated entry.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgAddressControl::HighlightUnvalidatedStringL()
    {
    ResetParsingInfo();

    iAddressControlEditor->TurnHighlightingOffL();

    TInt entryCount = iAddressControlEditor->EntryCount();
    TInt entryNumber( iParsingInfo.iEntryNum );
    TBuf<KMaxEntryLength> entryBuf;
    TInt ret( KErrNotFound );

    while ( entryNumber < entryCount )
        {
        entryBuf.Zero();
        if ( iAddressControlEditor->IsEntryValidated( entryNumber ) )
            {
            entryNumber++;
            SetParsingInfo( entryNumber, 0, 0 );
            }
        else
            {
            ret = GetNextItemOnEntry( entryBuf );
            if ( ( ret == KErrNone ) && ( entryBuf.Length() > 0 ) )
                {
                TInt firstPos = iAddressControlEditor->EntryStartPos(
                    iParsingInfo.iEntryNum );
                FormatAndSetCursorPosL( firstPos + iParsingInfo.iEndPos );
                break;
                }
            else if ( iParsingInfo.iEndPos > iParsingInfo.iStartPos )
                {
                // a string could not be found but there seems to be white
                // spaces on an entry which may be also followed by the list
                // separator, hence clean them up
                iAddressControlEditor->DeleteEntryCharsL(
                    iParsingInfo.iEntryNum,
                    iParsingInfo.iStartPos,
                    iParsingInfo.iEndPos );

                SetParsingInfo(
                    entryNumber,
                    iParsingInfo.iStartPos,
                    iParsingInfo.iStartPos );
                }
            else if ( ( iParsingInfo.iEndPos == 0 ) &&
                ( iParsingInfo.iEntryNum < entryCount - 1 ) )
                {
                // there is just an EParagraphDelimiter on the entry
                iAddressControlEditor->DeleteEntryL( iParsingInfo.iEntryNum );
                SetParsingInfo( entryNumber, 0, 0 );
                entryCount--;
                }
            else
                {
                // there are no strings left on the current entry
                entryNumber++;
                SetParsingInfo( entryNumber, 0, 0 );
                }
            }
        }

    if ( entryNumber >= entryCount )
        {
        FormatAndSetCursorPosL( iAddressControlEditor->TextLength() );
        }

    ret = iParsingInfo.iEndPos - iParsingInfo.iStartPos > 0
        ? KErrNone
        : KErrNotFound;

    if ( IsActivated() && ( ret == KErrNone ) && IsFocused() )
        {
        TInt entryLength( 0 );  // not really needed
        TInt entryStart( iAddressControlEditor->RichText()->CharPosOfParagraph(
            entryLength, iParsingInfo.iEntryNum ) );
        FormatAndSetCursorPosL( entryStart + iParsingInfo.iEndPos );
        iAddressControlEditor->SetSelectionL(
            entryStart + iParsingInfo.iEndPos,
            entryStart + iParsingInfo.iStartPos );
        }
    return ret;
    }

// ---------------------------------------------------------
// CMsgAddressControl::IsPriorCharSemicolonL()
//
// Checks whether prior character is semicolon. Needed to variable
// selection key functionality in editors.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgAddressControl::IsPriorCharSemicolonL() const
    {
    TInt cursorPos = Editor().CursorPos();
    HBufC* buf = Editor().GetTextInHBufL();
    CleanupStack::PushL( buf );

    TBool semicolonFound = EFalse;
    TUint character;
    while ( cursorPos > 1 && !semicolonFound )
        {
        cursorPos--;
        character = TUint( (*buf)[cursorPos] );

        if ( character == KMsgListSeparator || character == KArabicSemicolon )
            {
            semicolonFound = ETrue;
            }
        else if ( character != CEditableText::ESpace &&
            character != CEditableText::EParagraphDelimiter )
            {
            break;
            }
        }
    
    CleanupStack::PopAndDestroy(); // buf
    return semicolonFound;
    }

// ---------------------------------------------------------
// CMsgAddressControl::SetAddressFieldAutoHighlight()
//
// Turns on or off automatic recipient highlight on this address
// field. Focus must stop to this control if highlight is enabled.
// Updates also the highlight accordingly.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAddressControl::SetAddressFieldAutoHighlight( TBool aValidHighlightable )
    {
    iAddressControlEditor->SetAddressFieldAutoHighlight( aValidHighlightable );    
    DrawDeferred();
    }

// ---------------------------------------------------------
// CMsgAddressControl::AddressFieldAutoHighlight()
//
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgAddressControl::AddressFieldAutoHighlight()
    {
    return iAddressControlEditor->AddressFieldAutoHighlight();
    }

// ---------------------------------------------------------
// CMsgAddressControl::ConstructFromResourceL
//
// Creates this control from resource.
// ---------------------------------------------------------
//
void CMsgAddressControl::ConstructFromResourceL( TInt aResourceId )
    {
    BaseConstructL();  // Sets margins only.

    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, aResourceId );

    // Create caption.
    iCaption = CreateCaptionFromResourceL( reader );
    
    // Read some information about control from resource.
    ReadControlPropertiesFromResourceL( reader );

    // Create editor.
    iEditor = CreateEditorFromResourceL( reader );
    
    iEditor->SetControlType(EMsgAddressControl);
    iEditor->SetObserver( this );
    iEditor->SetEdwinSizeObserver( this );
    iEditor->AddEdwinObserverL( this );
    
    SetPlainTextMode( ETrue );

#ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() )
        {
        CreateButtonL();
        }
#endif // RD_SCALABLE_UI_V2

    ResolveLayoutsL();
    
    iControlModeFlags |= EMsgControlModeForceFocusStop;
    
    CleanupStack::PopAndDestroy();  // reader
    }

// ---------------------------------------------------------
// CMsgAddressControl::NotifyViewEvent
//
// Builds up text fields for the control i.e. reads verified recipients and
// creates corresponding text fields for them.
// ---------------------------------------------------------
//
void CMsgAddressControl::NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam )
    {
    if ( iAddressControlEditor->AddressFieldAutoHighlight() )
        {
        aParam |= EMsgViewEventAutoHighlight;
        }

    CMsgExpandableControl::NotifyViewEvent( aEvent, aParam );

    if ( aEvent == EMsgViewEventPrepareForViewing )
        {
        if ( iAddressControlEditor->TextLength() && IsFocused() )
            {
            iAddressControlEditor->PrepareForViewing();
            
            }
        }
    }

// ---------------------------------------------------------
// ReadControlPropertiesFromResourceL
//
// Reads control properties from resource.
// ---------------------------------------------------------
//
void CMsgAddressControl::ReadControlPropertiesFromResourceL(
    TResourceReader& aReader )
    {
    CMsgExpandableControl::ReadControlPropertiesFromResourceL( aReader );
    }

// ---------------------------------------------------------
// CMsgAddressControl::CreateEditorFromResourceL
//
// Creates editor (CEikRichTextEditor) for the control from resource and
// returns pointer to it.
// ---------------------------------------------------------
//
CMsgExpandableControlEditor* CMsgAddressControl::CreateEditorFromResourceL(
    TResourceReader& aReader )
    {
    // Create an editor.
    CMsgExpandableControlEditor* editor =
        new ( ELeave ) CMsgAddressControlEditor(
            this, iControlModeFlags, iBaseControlObserver );

    CleanupStack::PushL( editor );
    editor->ConstructFromResourceL( aReader );
    CleanupStack::Pop();  // editor

    return editor;
    }

// ---------------------------------------------------------
// CMsgAddressControl::EditPermission
//
// Checks and returns control's edit permissions. Edit permissions are needed
// to check in order to know whether some key press is allowed to pass
// to the control or not.
// ---------------------------------------------------------
//
TUint32 CMsgAddressControl::EditPermission() const
    {
    return iAddressControlEditor->CheckEditPermission();
    }


// ---------------------------------------------------------
// CMsgAddressControl::OfferKeyEventL
//
// Handles key events. The most of the keys are passed to the control editor.
// ---------------------------------------------------------
//
TKeyResponse CMsgAddressControl::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    // reset "trap" for extraneous multi-tap TextUpdate edwin events as we are
    // now working on another input
    iDuplicateEvent = EFalse;
    
    TKeyResponse keyResp = EKeyWasNotConsumed;
    ResetParsingInfo();
    TInt  cursorPos = iAddressControlEditor->CursorPos();

    if ( aKeyEvent.iModifiers & EModifierShift )
        {
        iControlModeFlags |= EMsgControlModeShiftPressed;
        }
    else
        {
        iControlModeFlags &= ~EMsgControlModeShiftPressed;
        }

    switch ( aKeyEvent.iCode )
        {
        case EKeyHome:
        case EKeyEnd:
        case EKeyPageUp:
        case EKeyPageDown:
            {
            break;
            }
        case EKeyUpArrow:
        case EKeyDownArrow:
            {
            /*TInt fldpos = iAddressControlEditor->FirstFieldPos( cursorPos );
            TInt entryNumber = iAddressControlEditor->EntryNumber( cursorPos );

            if ( cursorPos != fldpos )
                {
                    iAddressControlEditor->MoveCursorToEntryPosL( entryNumber, 0 );
                }*/
            break;
            }
        case EKeyLeftArrow:
            {
            if ( iAddressControlEditor->IsPosAtEntryBeginning( cursorPos ) )
                {
                TInt entryNumber = iAddressControlEditor->EntryNumber( cursorPos );
                if ( iAddressControlEditor->IsEntryVerified( entryNumber ) )
                    {
                    if ( iAddressControlEditor->IsEntryRightToLeft( entryNumber ) )
                        {
                        iAddressControlEditor->MoveCursorToEntryPosL(
                            entryNumber,
                            iAddressControlEditor->EntryLength( entryNumber ) );
                        iAddressControlEditor->CheckHighlightingL();
                        return EKeyWasConsumed;
                        }
                    }
                }
                
            if ( !iAddressControlEditor->IsPosAtEntryBeginning( cursorPos ) )
                {
                TInt entryNumber = iAddressControlEditor->EntryNumber( cursorPos );
                if (iAddressControlEditor->IsEntryVerified( entryNumber ) )
                    {
                    if ( !iAddressControlEditor->IsEntryRightToLeft( entryNumber ) )
                        {
                        iAddressControlEditor->MoveCursorToEntryPosL( entryNumber, 0 );
                        iAddressControlEditor->CheckHighlightingL();
                        return EKeyWasConsumed;
                        }
                    }
                }
                
            break;
            }
        case EKeyRightArrow:
            {
            if ( iAddressControlEditor->IsPosAtEntryBeginning( cursorPos ) )
                {
                TInt entryNumber = iAddressControlEditor->EntryNumber( cursorPos );
                if (iAddressControlEditor->IsEntryVerified( entryNumber ) )
                    {
                    if( !iAddressControlEditor->IsEntryRightToLeft( entryNumber ) )
                        {
                        iAddressControlEditor->MoveCursorToEntryPosL(
                            entryNumber,
                            iAddressControlEditor->EntryLength( entryNumber ) );
                        iAddressControlEditor->CheckHighlightingL();
                        return EKeyWasConsumed;
                        }
                    }
                }
            if ( !iAddressControlEditor->IsPosAtEntryBeginning( cursorPos ) )
                {
                TInt entryNumber = iAddressControlEditor->EntryNumber( cursorPos );
                if (iAddressControlEditor->IsEntryVerified( entryNumber ) )
                    {
                    if( iAddressControlEditor->IsEntryRightToLeft( entryNumber ) )
                        {
                        iAddressControlEditor->MoveCursorToEntryPosL( entryNumber, 0 );
                        iAddressControlEditor->CheckHighlightingL();
                        return EKeyWasConsumed;
                        }
                    }
                }
            break;
            }
        case EKeyTab:
        case EKeyInsert:
            {
            return EKeyWasConsumed;
            }
        case EKeyEnter:
            {
            TUint32 editPermission = iAddressControlEditor->CheckEditPermission();
            if ( !( editPermission & EMsgEditParagraphDelimiter ) ||
                ( aKeyEvent.iModifiers & EModifierShift ) )
                {
                return EKeyWasConsumed;
                }
                
            iAddressControlEditor->TurnHighlightingOffL( iAddressControlEditor->Selection() );
                
            break;
            }
        case EKeyDelete:
            {
            TUint32 editPermission = iAddressControlEditor->CheckEditPermission();
            if ( editPermission & EMsgEditRemoveEntry &&
                 iAddressControlEditor->IsPosAtEntryBeginning( cursorPos ) )
                {
                RemoveHighlightedEntryL();
                return EKeyWasConsumed;
                }
            else if ( !( editPermission & EMsgEditDelete ) )
                {
                return EKeyWasConsumed;
                }
                
            iAddressControlEditor->TurnHighlightingOffL( iAddressControlEditor->Selection() );
            break;
            }
        case EKeyBackspace:
            {
            TUint32 editPermission = iAddressControlEditor->CheckEditPermission();
            
            if ( editPermission & EMsgEditRemoveEntry )
                {
                RemoveHighlightedEntryL();
                return EKeyWasConsumed;
                }
            else if ( editPermission & EMsgEditBackspaceMove )
                {
                TInt entryNumber = iAddressControlEditor->EntryNumber( cursorPos ) - 1;
                iAddressControlEditor->MoveCursorToEntryPosL(
                    entryNumber,
                    iAddressControlEditor->EntryLength( entryNumber ) );
                return EKeyWasConsumed;
                }
            else if ( !( editPermission & EMsgEditBackspace ) )
                {
                return EKeyWasConsumed;
                }
                
            iAddressControlEditor->TurnHighlightingOffL( iAddressControlEditor->Selection() );
            
            // This is for disabling paragraph adding when removing line feed after 
            // semicolon. OfferKeyEventL below will trigger EEventTextUpdate event.
            iDuplicateEvent = ETrue;
            
            break;
            }
        default:
            {
            if ( !iAddressControlEditor->IsHandleEditEvent() )
                {
                TUint32 editPermission = iAddressControlEditor->CheckEditPermission();
                if ( !( editPermission & EMsgEditCharInsert ) )
                    {
                    return EKeyWasConsumed;
                    }
                }
            iAddressControlEditor->TurnHighlightingOffL( iAddressControlEditor->Selection() );
            break;
            }
        }

    keyResp = iAddressControlEditor->OfferKeyEventL( aKeyEvent, aType );

    return keyResp;
    }

// ---------------------------------------------------------
// CMsgAddressControl::HandleEdwinEventL
//
//
// ---------------------------------------------------------
//
void CMsgAddressControl::HandleEdwinEventL(
    CEikEdwin* aEdwin, TEdwinEvent aEventType )
    {
    CMsgExpandableControl::HandleEdwinEventL( aEdwin, aEventType );

    if ( aEventType == MEikEdwinObserver::EEventNavigation )
        {
        if ( iAddressControlEditor->CursorPos() ==
            iAddressControlEditor->TextLength() )
            {
            iAddressControlEditor->TurnHighlightingOffFromPosL( 0 );
            }
        iAddressControlEditor->CheckHighlightingL();
        }
    if ( aEventType == MEikEdwinObserver::EEventTextUpdate )
        {
        iAddressControlEditor->PostInsertEditorFormattingL( iDuplicateEvent );
        
        // Reset duplicate event flag.
        iDuplicateEvent = EFalse;
        }
    }

// ---------------------------------------------------------
// CMsgAddressControl::PrepareForReadOnly
//
// Prepares read only or non read only state.
// ---------------------------------------------------------
//
void CMsgAddressControl::PrepareForReadOnly( TBool aReadOnly )
    {
    CMsgExpandableControl::PrepareForReadOnly( aReadOnly );
    
#ifdef RD_SCALABLE_UI_V2
    if ( !aReadOnly && 
         AknLayoutUtils::PenEnabled() )
        {
        if ( !iButton )
            {
            TRAPD( error, CreateButtonL() );
            if ( error == KErrNone )
                {
                LayoutButton();
                }
            else
                {
                delete iButton;
                iButton = NULL;
                }
            }
        }
    else 
        {
        // No button is created at the read only state or when pen is disabled
        delete iButton;
        iButton = NULL;
        }
#endif
    }

// ---------------------------------------------------------
// CMsgAddressControl::FocusChanged
//
// This is called when the focus of the control is changed.
// ---------------------------------------------------------
//
void CMsgAddressControl::FocusChanged( TDrawNow aDrawNow )
    {
    CMsgExpandableControl::FocusChanged( aDrawNow );

    }

// ---------------------------------------------------------
// CMsgAddressControl::HandleResourceChange
//
// This is called when the focus of the control is changed.
// ---------------------------------------------------------
//
void CMsgAddressControl::HandleResourceChange( TInt aType )
    {

    CMsgExpandableControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
// CMsgAddressControl::GetNextItemOnEntry
//
// Finds the next unverified string on entry and updates parsing info
// correspondingly. Returns ETrue if found.
// ---------------------------------------------------------
//
TInt CMsgAddressControl::GetNextItemOnEntry( TDes& aEntryBuf )
    {
    TInt entryNumber = iParsingInfo.iEntryNum;
    TInt startPos = iParsingInfo.iEndPos;
    TInt endPos = iParsingInfo.iEndPos;

    TPtrC entryString( iAddressControlEditor->ReadEntry( entryNumber ) );
    TInt ret = ParseString( aEntryBuf, entryString, startPos, endPos );
    SetParsingInfo( entryNumber, startPos, endPos );

    return ret;
    }

// ---------------------------------------------------------
// CMsgAddressControl::ParseString
//
// Finds and returns an unverified string aEntryItem from aString starting at
// position aStartPos. Returns a start position aStartPos and an end position
// aEndPos of the searched range. Returns KErrNotFound if the string cannot be
// found or KErrOverflow if a sting was too long.
// ---------------------------------------------------------
//
TInt CMsgAddressControl::ParseString(
    TDes& aEntryItem, const TPtrC& aString, TInt& aStartPos, TInt& aEndPos )
    {
    __ASSERT_DEBUG( aStartPos >= 0, Panic( EMsgInvalidStartPos ) );

    aEntryItem.Zero();
    aEndPos = aStartPos;
    TInt stringLength = aString.Length();
    TInt ret( KErrNone );

    while ( aEndPos < stringLength )
        {
        // finds suitable strings e.g. kalle;^   kalle^   ______;^
        if ( ( ( aString[aEndPos] == KMsgListSeparator ) ||
            ( aString[aEndPos] == KArabicSemicolon ) ||
            ( aString[aEndPos] == CEditableText::ELineBreak ) ||
            ( aString[aEndPos] == KDownwardsArrowWithTipLeftwards ) ||
            ( aString[aEndPos] == KDownwardsArrowWithTipRightwards ) )
            // SJK 25.04.2005: Fixing "HNUN-6BPAVU":
            /*&& ( aStartPos != aEndPos )*/ )
            {
            aEndPos++;
            break;
            }
        else if ( ( ( aString[aEndPos] != KMsgListSeparator ) ||
            ( aString[aEndPos] != KArabicSemicolon ) ) &&
            ( aEntryItem.Length() < aEntryItem.MaxLength() ) )
            {
            aEntryItem.Append( aString[aEndPos] );
            }
        else if ( aString[aEndPos] != CEditableText::ESpace )
            {
            // Do we ever get here? Even in theory!?
            // What is this branch for?!
            aEndPos++;
            break;
            }
        aEndPos++;
        }

    aEntryItem.TrimAll();

    if ( ( aEntryItem.Length() == 0 ) && ( aStartPos == aEndPos ) )
        {
        ret = KErrNotFound;
        }

    return ret;
    }

// ---------------------------------------------------------
// CMsgAddressControl::FormatAndSetCursorPosL
//
// Formats and sets the cursor to the beginning of the formatted band.
// ---------------------------------------------------------
//
void CMsgAddressControl::FormatAndSetCursorPosL( TInt aPos )
    {
    if ( iControlModeFlags & EMsgControlModeInitialized )
        {
        iAddressControlEditor->SetAmountToFormatL( ETrue );
        iAddressControlEditor->CheckHighlightingL();
        }

    // Formats and draws the content
    iAddressControlEditor->SetCursorPosAndCancelSelectionL( aPos );
    if ( iControlModeFlags & EMsgControlModeInitialized )
        {
        iAddressControlEditor->CheckHighlightingL();
        iBaseControlObserver->HandleBaseControlEventRequestL(
            this, EMsgEnsureCorrectFormPosition );
        }
    }

// ---------------------------------------------------------
// CMsgAddressControl::RemoveHightlightedEntryL
// ---------------------------------------------------------
//
void CMsgAddressControl::RemoveHighlightedEntryL( )
    {
    iAddressControlEditor->TurnHighlightingOffL();
    TInt cursorPos( iAddressControlEditor->CursorPos() );
    TInt entryNumber = iAddressControlEditor->EntryNumber( iAddressControlEditor->CursorPos() );
    //Count the paragraph delimiter while capturing length
    TInt entryLength = iAddressControlEditor->EntryLength( entryNumber )+1;
    //Clear the entry
    iAddressControlEditor->DeleteEntryCharsL( entryNumber, 0, entryLength );
    iAddressControlEditor->DeleteEntryL( entryNumber );
    FormatAndSetCursorPosL(iAddressControlEditor->CursorPos()); 

    iControlModeFlags |= EMsgControlModeModified;
    }

// ---------------------------------------------------------
// CMsgAddressControl::CreateButtonL
// ---------------------------------------------------------
//
void CMsgAddressControl::CreateButtonL()
    {
#ifdef RD_SCALABLE_UI_V2
    delete iButton;
    iButton = NULL;
    
    iButton = CAknButton::NewL( NULL,
                                NULL,
                                NULL,
                                NULL,
                                *iCaption->Text(),
                                KNullDesC,
                                KAknButtonSizeFitText | KAknButtonTextLeft,
                                0 );
    
    iButton->SetTextColorIds( KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG63 );
    iButton->SetObserver( this );
    
    if ( Parent() )
        {
        iButton->SetContainerWindowL( *Parent() );
        iButton->ActivateL();
        }
#endif // RD_SCALABLE_UI_V2
    }

// ---------------------------------------------------------
// CMsgAddressControl::LayoutButton
// ---------------------------------------------------------
//    
void CMsgAddressControl::LayoutButton()
    {
#ifdef RD_SCALABLE_UI_V2
    CMsgExpandableControl::LayoutButton();
    
    if ( iButton )
        {
        TMargins8 margins;
        TRect buttonRect = iButtonLayout.Rect();
        TRect textRect = iCaptionLayout.TextRect();
        margins.iTop = textRect.iTl.iY - buttonRect.iTl.iY;
        margins.iBottom = buttonRect.iBr.iY - textRect.iBr.iY;
        margins.iLeft = textRect.iTl.iX - buttonRect.iTl.iX;
        margins.iRight = buttonRect.iBr.iX - textRect.iBr.iX;
        
        iButton->SetMargins( margins );
        }
#endif // RD_SCALABLE_UI_V2
    }

//  End of File
