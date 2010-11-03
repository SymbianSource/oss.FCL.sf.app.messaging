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
* Description:  MsgAddressControlEditor  declaration
*
*/



#ifndef INC_MSGADDRESSCONTROLEDITOR_H
#define INC_MSGADDRESSCONTROLEDITOR_H

// ========== INCLUDE FILES ================================
#include <txtrich.h>
#include "MsgExpandableControlEditor.h"    // for CMsgExpandableControlEditor
#include "MsgAddressControlEditorField.h"

// ========== CONSTANTS ====================================

// ========== MACROS =======================================
#define KMaxContactLength 102 //(first name (50) + last name(50) + spaces(2))

#define iRichText ( static_cast<CRichText*>( iText ) )

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgEditorKeyCatcher;
class MVPbkContactLink;

// ========== CLASS DECLARATION ============================

/**
 * Defines a rich text editor for CMsgAddressControl.
 *
 */
class CMsgAddressControlEditor : public CMsgExpandableControlEditor
    {
    public:

        /**
         * Constructor.
         * @param aParent
         * @param aFlags
         * @param aBaseControlObserver
         */
        CMsgAddressControlEditor(
            const CCoeControl*       aParent,
            TUint32&                 aFlags,
            MMsgBaseControlObserver* aBaseControlObserver);

        /**
         * Destructor.
         */
        virtual ~CMsgAddressControlEditor();

        // Entry handling functions

        /**
         * Extracts an entry according to aEntryNumber and returns it in
         * aEntryString.
         * @param aEntryString
         * @param aEntryNumber
         */
        void ExtractEntry(TDes& aEntryString, TInt aEntryNumber);

        /**
         * Reads an entry aEntryNumber and returns pointer to it. If such an
         * entry does not exist, returns null string.
         * @param aEntryNumber
         * @return
         */
        TPtrC ReadEntry(TInt aEntryNumber) const;

        /**
         * Returns number of entries.
         * @return
         */
        TInt EntryCount() const;

        /**
         * Deletes an entry aEntryNumber.
         * @param aEntryNumber
         */
        void DeleteEntryL(TInt aEntryNumber);

        /**
         * Clears a text of an entry aEntryNumber.
         * @param aEntryNumber
         * @param aUpdate
         */
        void ClearEntryL(TInt aEntryNumber, TBool aUpdate = EFalse);

        /**
         * Clears the content of the editor.
         */
        void DeleteAllL();

        /**
         * Deletes characters from aStartPos to aEndPos at entry aEntryNumber.
         * @param aEntryNumber
         * @param aStartPos
         * @param aEndPos
         */
        void DeleteEntryCharsL(TInt aEntryNumber, TInt aStartPos, TInt aEndPos);

        /**
         * Sets cursor position to aPos and cancels selection.
         * @param aPos
         */
        void SetCursorPosAndCancelSelectionL(TInt aPos);

        /**
         * Inserts an entry to the location aEntryNumber with strings aName
         * and aAddress. Makes the entry as verified if aVerified = ETrue.
         */
        void InsertEntryL(
            TInt           aEntryNumber,
            const TDesC&   aName,
            const TDesC&   aAddress,
            TBool          aVerified,
            TBool          aValidated,
            const MVPbkContactLink* aContactLink );

        /**
         * Inserts the paragraph delimiter at aPos on entry aEntryNumber.
         * @param aEntryNumber
         * @param aPos
         */
        void InsertEntryBreakL(TInt aEntryNumber, TInt aPos);

        /**
         * Returns entry number where cursor is currently on.
         * @param aPos
         * @return
         */
        TInt EntryNumber(TInt aPos) const;

        /**
         * Returns length of the entry aEntryNumber.
         * @param aEntryNumber
         * @return
         */
        TInt EntryLength(TInt aEntryNumber) const;

        /**
         * Returns first character position on the entry aEntryNumber.
         * @param aEntryNumber
         */
        TInt EntryStartPos(TInt aEntryNumber) const;

        /**
         * Return start position of field under aCursorPos.
         * @param aCursorPos
         */
        TInt FirstFieldPos(TInt aCursorPos) const;

        /**
         * Moves cursor at the end of the entry aEntryNumber.
         * @param aEntryNumber
         * @param aPos
         */
        void MoveCursorToEntryPosL(TInt aEntryNumber, TInt aPos);

        /**
         * Checks whether the cursor is at the beginning of the entry.
         * @param aPos
         * @return ETrue if it is, otherwise EFalse.
         */
        TBool IsPosAtEntryBeginning(TInt aPos) const;

        /**
         * Checks whether an entry aEntryNumber is verified or not.
         * @param aEntryNumber
         * @return ETrue if it is, otherwise EFalse.
         */
        TBool IsEntryVerified(TInt aEntryNumber) const;

        /**
         * Checks whether an entry aEntryNumber - 1 is verified.
         * @param aEntryNumber
         * @return ETrue if it is, otherwise EFalse.
         */
        TBool IsPriorEntryVerified(TInt aEntryNumber) const;

        /**
         * Checks whether an entry aEntryNumber is validated or not.
         * @param aEntryNumber
         * @return ETrue if it is, otherwise EFalse.
         */
        TBool IsEntryValidated(TInt aEntryNumber) const;

        /**
         * Checks if there is a text field on the left hand side of the cursor at
         * position aPos.
         * @param aPos
         * @return ETrue if it is, otherwise EFalse.
         */
        TBool IsFieldOnLeft(TInt aPos) const;

        /**
         * Checks if there is a text field on the right hand side of the cursor at
         * position aPos.
         * @param aPos
         * @return ETrue if it is, otherwise EFalse.
         */
        TBool IsFieldOnRight(TInt aPos) const;

        /**
         * Checks if there is a text field under the cursor at position aPos.
         * @param aPos
         * @return ETrue if it is, otherwise EFalse.
         */
        TBool IsFieldOnPos(TInt aPos) const;

        /**
         * Checks whether an entry aEntryNumber is empty.
         * @param aEntryNumber
         * @return
         */
        TBool IsEntryEmpty(TInt aEntryNumber) const;

        /**
         * Turns entry as highlighted if the cursor is on the top of the verified
         * entry, otherwise turns highlighting off if there were any somewhere else.
         * @param aCancelFep
         */
        void CheckHighlightingL(TBool aCancelFep = ETrue);

        /**
         * Turns highlighting off if such a highlighted text field is somewhere.
         * @param aSelection
         */
        void TurnHighlightingOffL(TCursorSelection aSelection);

        /**
         * Turns highlighting off if such a highlighted text field is somewhere.
         * @param aCancelFep
         */
        void TurnHighlightingOffL(TBool aCancelFep = ETrue);

        /**
         * Turns highlighting off from position aPos.
         * @param aPos
         */
        void TurnHighlightingOffFromPosL(TInt aPos);

        /**
         * Creates a suitable text string for the text field using strings aName
         * and aAddress and return pointer to it. If aUpdate == EFalse, this
         * function does not format the string but takes only aAddress.
         * @param aName
         * @param aAddress
         * @param aVerified
         * @return
         */
        HBufC* CreateEntryStringLC(
            const TDesC& aName,
            const TDesC& aAddress,
            TBool        aVerified = ETrue);

        /**
         * Re-builds all the text strings.
         */
        void ReBuildEntryStringsL();

        /**
         * Returns a field of the entry aEntryNumber. If there is no field on that
         * entry, returns NULL.
         * @param aEntryNumber
         * @return
         */
        const CMsgAddressControlEditorField* Field(TInt aEntryNumber) const;

        /**
         * Returns character at position aPos.
         * @param aPos
         * @return
         */
        TChar CharAtPos (TInt aPos) const;

        /**
         * Prepares control for viewing.
         */
        void PrepareForViewing();

        /**
         * Inserts ; and line feeds prior to inline edits
         */
        void PreInsertEditorFormattingL( const TKeyEvent& aKeyEvent );

        /**
         * Inserts ; and line feeds prior to inline edits
         */
        void PostInsertEditorFormattingL( TBool aDuplicateEvent );

        /**
        * Determine the overall text direction for the text for an entry 
        * 
        * @param    aEntryNumber    a valid entry number
        * @return   EFalse iff the text for the given entry is not right to left text
        */
        TBool IsEntryRightToLeft(TInt aEntryNumber) const;

        /**
        * Sets address highlighting for automatic find function on/off. 
        * 
        * @param    aValidHighlightable    ETrue for on / EFalse for off.
        * @return   none
        */
        void SetAddressFieldAutoHighlight( TBool aValidHighlightable = EFalse );

        /**
        * Returns address highlighting value for automatic find function. 
        * 
        * @param
        * @return   aValidHighlightable    ETrue for on / EFalse for off.
        */
        inline TBool AddressFieldAutoHighlight();

    public:  // from CMsgExpandableControlEditor

        /**
         * Checks and returns control's edit permissions.
         * @return
         */
        TUint32 CheckEditPermission() const;

        /**
         * Creates this control from resource.
         * @param aReader
         */
        void ConstructFromResourceL(TResourceReader& aReader);

        /**
         * Calculates and sets the size of the control and returns new size as
         * reference aSize.
         * @param aSize
         */
        void SetAndGetSizeL(TSize& aSize);

        /**
         *
         */
        TBool IsHandleEditEvent() const { return iHandleEditEvent; }
        
        /**
        * Called by the parent control to notify editor about
        * read only state change. Includes address field and
        * expandable control specific behaviours. 
        * Base class implementation should be called by the derived class.
        *
        * @param aReadOnly ETrue if read only editor. Otherwise EFalse.
        */
        void PrepareForReadOnlyL( TBool aReadOnly );
        
        /**
         *   returns the current cursor position
         */
        TInt GetCursorpos() const;
    public:  // from CEikRichTextEditor

        /**
         * Handles key events.
         * @param aKeyEvent
         * @param aType
         * @return
         */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);    
    
        /**
        * From CCoeControl. See coecntrl.h
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
        /**
        * From CCoeControl. See coecntrl.h
        */
        void HandleResourceChange( TInt aType );
        
    public:  // from MEikCcpuEditor

        /**
         *
         */
        void CcpuPasteL();

    private:

        /**
         * Constructor (not available).
         */
        CMsgAddressControlEditor();

        /**
         * 2nd phase constructor.
         */
        void ConstructL();

        /**
         * Insert a semicolon at the specified position.
         *
         * @param aPos intertion point
         */
        void InsertSemicolonL(TInt aPos);

        /**
         * Insert a paragraph delimiter at the specified position.
         *
         * @param aPos intertion point
         */
        void InsertParagraphL(TInt aPos);
        
        /**
        * Check that contact underlining is correct.
        */
        void CheckUnderliningL();
        
        /**
        * Returns field from given position.
        */
        inline const CMsgAddressControlEditorField* FieldFromPos( TInt aPos ) const;
        
    private:

        TMsgAddressControlEditorFieldFactory* iAddressControlEditorFieldFactory;

        // this is used to indicate where higlighting starts.
        TInt                    iHighLightedFieldPos;

        // Indicates current entry number if length of the text has been changed
        // due to RebuildEntryString.
        TInt                    iCurrentEntry;

        TBool                   iHandleEditEvent;
        TBool                   iValidHighlightable; // for automatic highlight

        CMsgEditorKeyCatcher*   iKeyCatcher;
        TBool                   iHandlingPointerEvent;
        
        const CMsgAddressControlEditorField* iPreviousField;
        TInt iCusPos;
    };

inline TBool CMsgAddressControlEditor::AddressFieldAutoHighlight( )
    {
    return iValidHighlightable;
    }

inline const CMsgAddressControlEditorField* CMsgAddressControlEditor::FieldFromPos( TInt aPos ) const
    {
    return static_cast<const CMsgAddressControlEditorField*>( iRichText->TextField( aPos ) );
    }

#endif

// End of File
