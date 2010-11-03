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
* Description:  MsgAddressControl  declaration
*
*/



#ifndef CMSGADDRESSCONTROL_H
#define CMSGADDRESSCONTROL_H

// ========== INCLUDE FILES ================================
#include <cntdef.h>             // for TContactItemId

#include "MsgExpandableControl.h"
#include "MsgRecipientItem.h"              // for CMsgRecipientItem

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

#define iAddressControlEditor ( static_cast<CMsgAddressControlEditor*>( iEditor ) )

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================
class MVPbkContactLink;
class CAknButton;

// ========== DATA TYPES ===================================

// ========== CLASS DECLARATION ============================

/**
* Defines a parsing info class that is used for storing information about a
* found entry string.
*/
class TParsingInfo
    {
    public:

        /**
        * Constructor.
        */
        inline TParsingInfo();

        /**
        * Constructor.
        * @param aEntryNum
        * @param aStartPos
        * @param aEndPos
        */
        inline TParsingInfo( TInt aEntryNum, TInt aStartPos, TInt aEndPos );

        /**
        *
        * @param aEntryNum
        * @param aStartPos
        * @param aEndPos
        */
        inline void SetInfo( TInt aEntryNum, TInt aStartPos, TInt aEndPos );

    private:

        /**
        * Constructor (not available).
        */
        TParsingInfo( const TParsingInfo& aSource );

        /**
        * Assignment operator (not available).
        */
        const TParsingInfo& operator=( const TParsingInfo& aSource );

    public:

        TInt iEntryNum;
        TInt iStartPos;
        TInt iEndPos;
    };

/**
* Defines an address control for message editors/viewers.
*/
class CMsgAddressControl : public CMsgExpandableControl
    {
    public:

        enum TMsgAddressControlEditPermissionFlags
            {
            EMsgEditCharInsert         = EMsgFirstReservedFlag << 1,    // Inserting a character is possible
            EMsgEditParagraphDelimiter = EMsgFirstReservedFlag << 2,    // Enter key press is possible
            EMsgEditBackspace          = EMsgFirstReservedFlag << 3,    // Pressing Backspace is possible
            EMsgEditDelete             = EMsgFirstReservedFlag << 4,    // Pressing Delete is possible
            EMsgEditBackspaceMove      = EMsgFirstReservedFlag << 5,    // Pressing Backspace is possible but it does not remove a char
            EMsgEditRemoveEntry        = EMsgFirstReservedFlag << 6     // Removing entry is possible
            };

    public:

        /**
        * Constructor.
        * @param aBaseControlObserver
        */
        CMsgAddressControl( MMsgBaseControlObserver& aBaseControlObserver );

        /**
        * Destructor.
        */
        virtual ~CMsgAddressControl();

    public:

        /**
        * Adds one recipient to the address control.
        * @param aName
        * @param aAddress
        * @param aId
        * @param aVerified
        */
        IMPORT_C void AddRecipientL( const TDesC& aName,
                                     const TDesC& aAddress,
                                     TBool aVerified,
                                     MVPbkContactLink* aContactLink );        

        /**
        * Adds list of recipients to the address control.
        * @param aRecipients
        */
        IMPORT_C void AddRecipientsL( const CMsgRecipientList& aRecipients );

        /**
        * Returns recipients from the address control.
        * @return
        */
        IMPORT_C CMsgRecipientArray* GetRecipientsL();

        /**
        * Clears content of the address control.
        * @return
        */
        IMPORT_C TInt ResetL();

        /**
        * Clears content of the address control (non-leaving version; from CMsgBaseControl).
        */
        IMPORT_C void Reset();

        /**
        * Finds the first unverified string and updates also the iParserInfo
        * correspondingly.
        * @param aString
        * @return
        */
        IMPORT_C TInt GetFirstUnverifiedStringL( HBufC*& aString );

        /**
        * Finds the next unverified string.
        * @param aString
        * @return
        */
        IMPORT_C TInt GetNextUnverifiedStringL( HBufC*& aString );

        /**
        * Refreshes the contents of address control.
        * OBSOLETE! User RefreshL instead.
        *
        * @param aArray
        * @return
        */
        IMPORT_C TInt ReplaceUnverifiedStringL( CMsgRecipientArray& aArray );

        /**
        * Refreshes the contents of address control.
        *
        * @param aArray
        * @return
        */
        IMPORT_C TInt RefreshL( const CMsgRecipientArray& aArray );

        /**
        * Highlights the found unverified entry.
        * @return
        */
        IMPORT_C TInt HighlightUnverifiedStringL();

        /**
        * Returns size of addresses.
        * @param aEntryCount      entry count
        * @param aSizeOfAddresses total size of addresses.
        * @return
        */
        IMPORT_C void GetSizeOfAddresses( TInt& aEntryCount, TInt& aSizeOfAddresses );

        /**
        * Highlight unvalidated entry.
        */
        IMPORT_C TInt HighlightUnvalidatedStringL();

        /**
        * Checks whether prior character is semicolon. Needed to variable
        *  selection key functionality in editors.
        * @param none
        * @return ETrue if it is, otherwise EFalse.
        */
        IMPORT_C TBool IsPriorCharSemicolonL() const;

        /**
       * Sets address highlighting for automatic find function on/off. 
       * 
       * @param    aValidHighlightable    ETrue for on / EFalse for off.
       * @return   none
       */
        IMPORT_C void SetAddressFieldAutoHighlight( TBool aValidHighlightable = EFalse );

        /**
       * Returns address highlighting value for automatic find function. 
       * 
       * @param
       * @return   aValidHighlightable    ETrue for on / EFalse for off.
       */
        IMPORT_C TBool AddressFieldAutoHighlight();

    public:   // from CMsgExpandableControl

        /**
        * Creates this control from resource.
        * @param aResourceId
        */
        void ConstructFromResourceL( TInt aResourceId );

        /**
        * Prepares control for viewing.
        * @param aEvent
        * @param aParam
        */
        void NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam );

        /**
        * Reads control properties from resource.
        * @param aReader
        */
        void ReadControlPropertiesFromResourceL( TResourceReader& aReader );

        /**
        * Creates editor (CEikRichTextEditor) for the control from resource and returns
        * pointer to it.
        * @param aReader
        * @return
        */
        CMsgExpandableControlEditor* CreateEditorFromResourceL( TResourceReader& aReader );

        /**
        * Checks and returns control's edit permissions. Edit permissions are needed
        * to check in order to know whether some key press is allowed to pass
        * to the control or not.
        * @return
        */
        TUint32 EditPermission() const;

    public:   // from CCoeControl

        /**
        * Handles key events.
        * @param aKeyEvent
        * @param aType
        * @return
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
        * For handling dynamic layout switch.
        */
        void HandleResourceChange( TInt aType );
    
    public:  // from MEikEdwinObserver

        /**
        * Handle edwin events.
        * @param aEdwin
        * @param aEventType
        */
        void HandleEdwinEventL( CEikEdwin* aEdwin, TEdwinEvent aEventType );
        
    protected:   // from CMsgExpandableControl
    
        /**
        * Sets the correct layout for button.
        */    
        void LayoutButton();       
        
    protected:  // from CMsgBaseControl

        /**
        * Prepares for read only or non read only state.
        * @param aReadOnly
        */
        void PrepareForReadOnly( TBool aReadOnly );

    protected:  // from CCoeControl

        /**
        * This is called when the focus of the control is changed.
        * @param aDrawNow
        */
        void FocusChanged( TDrawNow aDrawNow );
        
    private:

        /**
        * Finds the next unverified string on entry and updates parsing info
        * correspondingly. Returns ETrue if found.
        * @param aEntryBuf
        * @return
        */
        TInt GetNextItemOnEntry( TDes& aEntryBuf );

        /**
        * Finds and returns an unverified string aEntryItem from aString starting at
        * position aStartPos. Returns a start position aStartPos and an end position
        * aEndPos of the searched range. Returns KErrNotFound if a string cannot be found.
        * @param aEntryItem
        * @param aEntryItem
        * @param aStartPos
        * @param aEndPos
        * @return
        */
        TInt ParseString( TDes& aEntryItem, const TPtrC& aString, TInt& aStartPos, TInt& aEndPos );

        /**
        * Formats and sets the cursor to the beginning of the formatted band.
        */
        void FormatAndSetCursorPosL( TInt aPos );

        /**
        *
        */
        inline void ResetParsingInfo();

        /**
        *
        * @param aEntryNum
        * @param aStartPos
        * @param aStartPos
        */
        inline void SetParsingInfo( TInt aEntryNum, TInt aStartPos, TInt aEndPos );
        
        /**
        * Removes highlighted entry.
        */
        void RemoveHighlightedEntryL();
        
        /**
        * Creates button with label text.
        */
        void CreateButtonL();
        
    private:

        TParsingInfo iParsingInfo;
        TBool iDuplicateEvent;

        CMsgRecipientArray* iRecipientArray;
    };

#include <MsgAddressControl.inl>

#endif // CMSGADDRESSCONTROL_H

// End of File
