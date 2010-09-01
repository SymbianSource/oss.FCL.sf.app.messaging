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
* Description: MuiuMessageIterator  declaration
*
*/



#ifndef __MUIUMESSAGEITERATOR_H__
#define __MUIUMESSAGEITERATOR_H__

//  INCLUDES
#include <e32base.h>
#include <msvapi.h>         // MMsvEntryObserver, CMsvSession...



// CLASS DECLARATION

/**
* 
* 
*/
class MMessageIteratorObserver
    {
    public:
        // Notify events
        enum TMessageIteratorEvent
            {
            EFolderCountChanged = 1
            };

    /**
     *
     */
    virtual void HandleIteratorEventL( TMessageIteratorEvent aEvent ) = 0;
};


/**
* Message Iterator Utility Class
* Typically used by message editors to implement next and previous message
* menu options. Class is an observer of the parent of the current entry,
* so it does handle changes in the children list.
* Class does not handle other external events e.g. deletion of parent.
*/
class CMessageIterator : public CBase, public MMsvEntryObserver
    {
    public:
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMessageIterator* NewL(
            CMsvSession& aSession,
            const TMsvEntry& aCurrentMessage);

        /**
        * Destructor
        */
        IMPORT_C ~CMessageIterator();

        /**
        * @return TMsvEntry of the current message
        */
        IMPORT_C const TMsvEntry& CurrentMessage() const;

        /**
        * @return TMsvEntry of the current message's parent
        */
        IMPORT_C const TMsvEntry& CurrentParent() const;

        /**
        * @return EFalse if next message does not exist, ETrue if next message exists
        */
        IMPORT_C TBool NextMessageExists() const;

        /**
        * @return EFalse if previous message does not exist, ETrue if previous message exists
        */
        IMPORT_C TBool PreviousMessageExists() const;

        /**
        * @return Index of the current message in currently opened folder
        */
        IMPORT_C TInt CurrentMessageIndex() const;

        /**
        * @return Number of messages in currently opened folder.
        */
        IMPORT_C TInt MessagesInFolder() const;


        /**
        * @param aCurrentMessage: changes current message
        */
        IMPORT_C void SetCurrentMessageL(const TMsvEntry& aCurrentMessage);

        /**
        * Changes current message to next
        * @return KErrNotFound if next message does not exist, otherwise KErrNone
        */
        IMPORT_C TInt SetNextMessage();

        /**
        * Changes current message to previous
        * @return KErrNotFound if previous message does not exist, otherwise KErrNone
        */
        IMPORT_C TInt SetPreviousMessage();


        /**
        * Opens current message
        * @param aStatus: request status
        * @param aEmbed: Should be usually ETrue
        * @return CMsvOperation: operation of the opening of the editor.
        */
        IMPORT_C CMsvOperation* OpenCurrentMessageL(
            TRequestStatus& aStatus,
            TBool aEmbed = ETrue );

        /**
        * Set iterator observer.
        */
        IMPORT_C void SetMessageIteratorObserver(MMessageIteratorObserver* aObserver);

        /**
        * From MMsvEntryObserver
        */
        void HandleEntryEventL(
            TMsvEntryEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );

    private:
        /**
        * C++ constuctor
        */
        CMessageIterator();

        /**
        * Symbian OS constructor
        */
        void ConstructL(
            CMsvSession& aSession,
            const TMsvEntry& aStartMessage );

        /**
        * internal
        */
        void UpdateChildrenOfParentL( TMsvId aCurrentMessageId );

    private:
        // --- Data ---
        TInt iCurrentIndex;
        CMsvEntry* iParentOfCurrent;
        CMsvEntrySelection* iChildrenOfParent;
        TBool iEntryObserver;

        MMessageIteratorObserver* iMessageIteratorObserver;

    };

#endif //__MUIUMESSAGEITERATOR_H__

// End of file
