/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgEditorAppUi  declaration
*
*/



#ifndef INC_MSGEDITORAPPUI_H
#define INC_MSGEDITORAPPUI_H

// ========== INCLUDE FILES ================================

#include <MsgEditorBaseUi.h>
#include <msvstore.h>                   // RMsvWriteStream

#include <MuiuMessageIterator.h>        // CMessageIterator
#include <AknSettingCache.h>
#include <MsgEditor.hrh>
#include <akninputblock.h>

#include <MsgEditorObserver.h>          // MMsgEditorObserver
#include <MsgEditorModelObserver.h>     // MMsgEditorModelObserver
#include <MsgEditorDocument.h>          // model
#include <MsgEditorCommon.h>
#include <MsgNaviPaneControlObserver.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgEditorView;
class CMsgBodyControl;
class CEikEdwin;
class CErrResolver;
class TApaTask;
class CAknNavigationDecorator;
class CApaMaskedBitmap;
class CFileMan;
class CAknNavigationControlContainer;
class CMsgEditorShutter;
class CMsgEditorAppUiExtension;
class CMsgNaviPaneControl;

// ========== CLASS DECLARATION ============================

/**
 * Base class for Message editor/viewer application ui module.
 *
 */
class CMsgEditorAppUi : public CMsgEditorBaseUi,
                        public MMsgEditorObserver,
                        public MMsgEditorModelObserver,
                        public MMessageIteratorObserver,
                        public MAknInputBlockCancelHandler,
                        public MMsgNaviPaneControlObserver
    {       
    public:
        
        /**
         * Constructor.
         */
        IMPORT_C CMsgEditorAppUi();

        /**
         * 2nd phase constructor.
         */
        IMPORT_C void ConstructL();

        /**
         * Destructor.
         */
        IMPORT_C ~CMsgEditorAppUi();

        /**
         * Route key events to view.
         * @param aKeyEvent key event
         * @param aType     event type
         * @return          response
         * from CCoeAppUi
         */
        IMPORT_C TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent, TEventCode aType);

        //
        IMPORT_C void HandleWsEventL(const TWsEvent& aEvent,CCoeControl* aDestination);
        //

        /**
         * Prepare to exit.
         * from CEikAppUi
         */
        IMPORT_C void PrepareToExit();

        /**
         * Asynchronous Mtm reply command.
         * @param aTarget           destination folder
         * @param aIncludeOriginal  include original text
         */
        IMPORT_C void ReplyL(TMsvId aTarget, TBool aIncludeOriginal = ETrue);

        /**
         * Asynchronous Mtm reply all command.
         * @param aTarget           destination folder
         * @param aIncludeOriginal  include original text
         */
        IMPORT_C void ReplyToAllL(TMsvId aTarget, TBool aIncludeOriginal = ETrue);

        /**
         * Asynchronous Mtm forward command.
         * @param aTarget           destination folder
         */
        IMPORT_C void ForwardL(TMsvId aTarget);

        /**
         * Inserts template text to body control from Notepad. Sets 'document
         * changed' flag to ETrue if inserting successful.
         * @param aMaxChars maximum number of characters allowed for insertion.
         * @return KErrNone if insertion successful. KErrCancel if template
         *         selection canceled. KErrOverflow if selected template text is
         *         longer than aMaxChars. In non-recoverable error situations it
         *         leaves.
         */
        IMPORT_C TInt InsertTemplateL(TInt aMaxChars = -1);
        
        /**
         * Inserts text memo to body control from Notepad. Sets 'document
         * changed' flag to ETrue if inserting successful.
         *
         * @param aMaxChars maximum number of characters allowed for insertion.
         *
         * @return KErrNone if insertion successful. KErrCancel if memo
         *         selection canceled. KErrOverflow if selected memo text is
         *         longer than aMaxChars. In non-recoverable error situations it
         *         leaves.
         */
        IMPORT_C TInt InsertTextMemoL( TInt aMaxChars = -1 );
        
        /**
         * Updates navi pane.
         * If no bitmaps given, navipane has only
         * left arrow + msg/folder amount + right arrow.
         */
        IMPORT_C void UpdateNaviPaneL(
            const CFbsBitmap* aBitmap     = NULL,
            const CFbsBitmap* aBitmapMask = NULL);

        /**
         * Shows Call to Sender query.
         * @param aNumber real address
         * @param aAlias  alias
         * @return ETrue if Yes, OK or Send key pressed. EFalse if No pressed.
         */
        IMPORT_C TBool CallToSenderQueryL(const TDesC& aNumber, const TDesC& aAlias);

        /**
         * Returns application menubar.
         * @return pointer to menubar.
         */
        IMPORT_C CEikMenuBar* MenuBar() const;

        /**
         *
         */
        IMPORT_C virtual void HandleResourceChangeL(TInt aType);

        /**
         *
         */
        IMPORT_C void LockEntryL();

        /**
         *
         */
        IMPORT_C void UnlockEntry();

        /**
         *
         */
        IMPORT_C TBool IsLockedEntry() const;

        /**
         * Return current document.
         * @return pointer to document.
         */
        inline CMsgEditorDocument* Document() const;

        /**
        * Shows "closing message" list query. The options
        * in the query are "Save to draft" and "Delete message".
        * Should be called when "Close" softkey is pressed in 
        * non-empty message.
        *
        * @return EMsgCloseCancel, if the query is cancelled
        *         EMsgCloseSave, if "Save to drafts" is selected
        *         EMsgCloseDelete, if "Delete message" is selected
        */
        IMPORT_C TInt ShowCloseQueryL();

        /**
        * Creates new subject string with "Re:" or "Fw:" prefix
        * 
        * @since 2.8
        * @param aSubject original subject string
        * @param aReply, if ETrue, for reply -> "Re:"
        *                if EFalse, for forward -> "Fw:"
        * @return Pointer to new string if prefix applied, ownership transferred
        *         NULL if prefix not applied
        */
        IMPORT_C HBufC* CreateSubjectPrefixStringL(
            const TDesC& aSubject,
            TBool aReply );
        
        /**
        * From MAknInputBlockCancelHandler. Called when input blocker is cancelled.
        *
        * @since 3.2
        */
    	IMPORT_C void AknInputBlockCancel();
        
        /**
        * From MMsgNaviPaneControlObserver.
        * Called by navigation pane control to notify an event.
        *
        * @param aEvent - New event 
        */
        IMPORT_C void HandleNavigationControlEventL( TMsgNaviControlEventId aEvent );
        
        /**
        * Enables/Disables closing with end key. Closing is enabled
        * by default.
        *
        * @param aCloseWithEndKey  ETrue if application should close with end key.
        *                          EFalse if application should ignore end key events.
        */
        inline void SetCloseWithEndKey( TBool aCloseWithEndKey );
        
        /**
         * Reacts to statuspane size change events.
         */
        IMPORT_C void HandleStatusPaneSizeChange();
        
    private:
    
        /**
         * Removes all prefixes from begining of the subject
         * 
         * @param aSubject original subject string
         * @return newSubject to new subject
         */
        HBufC* RemovePrefixesL(const TDesC& aSubject);
        
    public: // pure virtuals

        /**
         *
         */
        virtual void DoMsgSaveExitL() = 0;

    protected:    // from MMsgEditorModelObserver

        IMPORT_C void HandleEntryChangeL();
        IMPORT_C void HandleEntryDeletedL();
        IMPORT_C void HandleEntryMovedL(TMsvId aOldParent, TMsvId aNewParent);
        IMPORT_C void HandleMtmGroupDeinstalledL();
        IMPORT_C void HandleGeneralErrorL(TInt aErrorCode);
        IMPORT_C void HandleCloseSessionL();
        IMPORT_C void HandleServerFailedToStartL();
        IMPORT_C void HandleServerTerminatedL();
        IMPORT_C void HandleMediaChangedL();
        IMPORT_C void HandleMediaUnavailableL();
        IMPORT_C void HandleMediaAvailableL();
        IMPORT_C void HandleMediaIncorrectL();
        IMPORT_C void HandleCorruptedIndexRebuildingL();
        IMPORT_C void HandleCorruptedIndexRebuiltL();

    protected:

        /**
         * Parses command line parameter and launch model accoring to them.
         * @param aCommand
         * @param aDocumentName
         * @param aTail
         * @return
         * from CEikAppUi
         */
        IMPORT_C TBool ProcessCommandParametersL(
            TApaCommand aCommand, TFileName& aDocumentName, const TDesC8& aTail);

        /**
         * Launch editor/viewer for next/previous message.
         * @param aForward  direction: ETrue = next, EFalse = previous.
         */
        IMPORT_C void NextMessageL(TBool aForward);

        /**
         * Checks if next or previous message is available.
         * @param aForward  direction: ETrue = next, EFalse = previous.
         * @return ETrue if next message is available is desired direction.
         */
        IMPORT_C TBool IsNextMessageAvailableL(TBool aForward);

        /**
         * Returns index of current message and also total number of messages
         * in folder.
         * @param aCurrentIndex
         * @param aMsgsInFolder
         */
        IMPORT_C void MessageIndexInFolderL (
            TInt& aCurrentIndex, TInt& aMsgsInFolder);

        /**
         * Moves an entry to another location.
         * @param aTarget
         * @return
         */
        IMPORT_C TMsvId MoveMessageEntryL(TMsvId aTarget) const;

        /**
         * Deletes current entry and then exits.
         */
        IMPORT_C void DeleteAndExitL();

        /**
         * Deletes current entry.
         */
        IMPORT_C void DeleteCurrentEntryL();

        /**
         * Checks if document has finished launching.
         */
        IMPORT_C TBool IsLaunchedL() const;
        
        /**
        * Handles the changing of the local zoom. 
        * If the old zoom value equals to new zoom, nothing is done
        * @param aNewZoom is the new zoom level
        */
        IMPORT_C void HandleLocalZoomChangeL( TMsgCommonCommands aNewZoom );
        
        /**
        * Creates navigation pane for message viewers. 
        * Navigation pane can be later accessed through iNaviDecorator
        * pointer. Does not push control to navigation control stack.
        *
        * @since 5.0
        *
        * @para  aTime Time that is shown on the navigation pane
        * @param aPriority Message priority
        * @param aUtcTime ETrue if given time is UTC time,
        *                 EFalse if given time is local time. 
        */
        IMPORT_C void CreateViewerNaviPaneL( TTime aTime, 
                                             TMsgEditorMsgPriority aPriority,
                                             TBool aUtcTime = EFalse );
        
        /**
         * Sets the size of the title icon.
         * @param aTitleBitmap
         */
        IMPORT_C void SetTitleIconSizeL( CFbsBitmap* aTitleBitmap );

        
    protected:   // from MMsgEditorObserver

        /**
         *
         */
        IMPORT_C CMsgBaseControl* CreateCustomControlL(TInt aControlType);

        /**
         *
         *
         */
        IMPORT_C void EditorObserver(
            TMsgEditorObserverFunc aFunc, TAny* aArg1, TAny* aArg2, TAny* aArg3);

    protected:	// From CEikAppUi

        /**
         * Calls ServerApp's NotifyServerExit-function with parameter aReason
         * and basecalls CEikAppUi::Exit()
         */
    	IMPORT_C void Exit( TInt aReason = EAknCmdExit );
    	
    private:    // from MEikMenuObserver

        /**
         * From MEikMenuObserver (called when menu is opened).
         */
        IMPORT_C void SetEmphasis(CCoeControl* aMenuWindow, TBool aEmphasis);

    protected:  // from MMessageIteratorObserver

        IMPORT_C virtual void HandleIteratorEventL(TMessageIteratorEvent aEvent);

    protected: // enumerations

        enum TMsgCloseQueryOptions
            {
            EMsgCloseCancel,
            EMsgCloseSave,
            EMsgCloseDelete
            };

    protected:

        CMsgEditorView* iView;
        TBool           iMmcFeatureSupported;
        MApaEmbeddedDocObserver::TExitMode iExitMode;
        CMsgEditorAppUiExtension* iMsgEditorAppUiExtension; // owned


    private:

        /**
         * Updates navi pane.
         */
        void UpdateNaviPaneL(
            TBool             aUpdateExisting,
            const CFbsBitmap* aBitmap     = NULL,
            const CFbsBitmap* aBitmapMask = NULL);

        /**
         * Get services from mtm's and offers then to APP UI.
         * @param aQuery MTM's query capability.
         * @param aCmd   command enumeration..
         */
        void DoMtmCmdL(const TUid& aQuery, TInt aCmd) const;

        /**
         * Helper function for Handle... methods.
         * @param aTitle
         */
        void ViewErrorL(TInt aTitle);

        /**
         *
         */
        void SaveBodyL(CMsgBodyControl* aBody);
        
        /**
         * Inserts text to body control.
         *
         * @param aBuffer   text buffer
         * @param aMaxChars maximum number of characters allowed for insertion.
         *
         * @return KErrNone if insertion successful. KErrCancel if text buffer is
         *         invalid. KErrOverflow if given text is longer than aMaxChars. 
         *         In non-recoverable error situations it leaves.
         */
        TInt DoInsertTextL( HBufC* aBuffer, TInt aMaxChars = -1 );

        /**
        * Check if message can be fast opened. Derived class should override this
        * and return true if it supports fast opening of iterator's current message.
        * Default implementation returns false.
        * @param aIterator message iterator
        * @return true if iterator's current message can be fast opened
        */
        IMPORT_C virtual TBool CanFastOpenL( const CMessageIterator& aIterator );
        
    protected:

        /**
         * Notify derived class that app will exit.
         */
        virtual void TerminateTaskToExit() {}
        
    protected:
    
        CAknNavigationDecorator*           iNaviDecorator;
        CMessageIterator*                  iIterator;
        
    private:

        TInt                               iWaitedThings;
        TInt                               iResourceFileOffset;
        CFileMan*                          iFileMan;

        NONSHARABLE_CLASS( CLock ) : public CBase
            {
            public:
                static CLock* LockL(const CMsgEditorDocument& aDoc);
                ~CLock();
            private:
                CLock();
            private:
                CMsvStore* iStore;
                RMsvWriteStream iStream;
            };

        CLock* iLock;
        CAknNavigationControlContainer*    iOwnNaviPane;
        CMsgEditorShutter*                 iEditorShutter;
        
        CAknInputBlock*                     iInputBlocker;
        TBool                               iCloseWithEndKey;
        TInt                                iStatusPaneRes;
        
    protected :
        // SMS navigation direction(0:Prev, 1:Next, -1:None)
        TInt iMsgNaviDirection;
        
    protected:
        // MMS changing slide
        TBool iIsChangingSlide;

        };


//  INLINE FUNCTIONS

// ---------------------------------------------------------
// CMsgEditorAppUi::Document
//
// ---------------------------------------------------------
//
inline CMsgEditorDocument* CMsgEditorAppUi::Document() const
    {
    return static_cast<CMsgEditorDocument*>( CEikAppUi::Document() );
    }

// ---------------------------------------------------------
// CMsgEditorAppUi::SetCloseWithEndKey
//
// Currently disabled feature.
// ---------------------------------------------------------
//
inline void CMsgEditorAppUi::SetCloseWithEndKey( TBool aCloseWithEndKey )
    {
    iCloseWithEndKey = aCloseWithEndKey;
    }

#endif

// End of File
