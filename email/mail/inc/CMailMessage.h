/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Message container class for mail editor / viewer
*
*/



#ifndef CMAILMESSAGE_H
#define CMAILMESSAGE_H

//  INCLUDES
#include <msvstd.h>
#include <cmsvattachment.h>

// DATA TYPES

// FORWARD DECLARATIONS
class CMsvEntry;
class CImHeader;
class CImEmailMessage;
class CRichText;
class CParaFormatLayer;
class CCharFormatLayer;
class MMessageLoadObserver;
class MMsvAttachmentManager;
class TMsvEmailEntry;
class CMsvSession;
class CMsvAttachment;
class RFile;
class CMuiuOperationWait;
class CImMimeHeader;

// CLASS DECLARATION
/**
*  Internal helper class for linked html files.
*/
NONSHARABLE_CLASS(CLinkedHtmlItem): public CBase
    {
    public:
    inline CLinkedHtmlItem(const TDesC& aURI, TMsvAttachmentId aLinkedfileId )
        {
        delete iUriAddress;
        iUriAddress = NULL;
        iUriAddress = aURI.AllocL();
        iLinkedItemId = aLinkedfileId;
        };
    inline ~CLinkedHtmlItem()
        {
        delete iUriAddress;
        };
    public: // Data
        HBufC*      iUriAddress;
        TMsvAttachmentId      iLinkedItemId;
    };

/**
*  Mail message.
*  Mail message container
*
*  @lib MsgMailUtils.dll
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS(CMailMessage) : public CActive
    {
    public:  // Load state
        enum TLoadState
            {
            EHeaderReady = 0,
            EBodyTextReady,
            EAttachmentsReady,
            EAttachedMessagesReady,
            ELinkedFileReady,
            ELoadEnd
            };
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aEntry CMsvEntry that contais mail message.
        */
        IMPORT_C static CMailMessage* NewL(CMsvEntry& aEntry);

        /**
        * Destructor.
        */
        virtual ~CMailMessage();

    public: // New functions

        /**
        * Start Message loading.
        * @since Series 60 3.0
        * @param aMessageObserver loading observer
        */
        IMPORT_C void LoadMessageL(
            MMessageLoadObserver& aMessageObserver);

        /**
        * Start loading linked HtmlContent
        * @param aBase
        * @param aURI
        * Observer from LoadMessageL is Notified "ELinkedFileReady"
        * when file loaded.
        * *DEPRECATED*
        */
        IMPORT_C void LoadLinkedHtmlContentL(
        	const TDesC& aBase, const TDesC& aURI);

        /**
        * inline MessageHeader.
        * @since Series 60 3.0
        * @return pointer to message body text. Ownership not transferred.
        */
        CImHeader* MessageHeader();

        /**
        * HtmlCharsetIdL.
        * @since Series 60 3.1
        * @return character set id of message html part.
        */
        IMPORT_C TUint HtmlCharsetIdL();

        /**
        * CharsetIdL.
        * @since Series 60 3.0
        * @return character set id.
        */
        IMPORT_C TUint CharsetIdL();

        /**
        * ConvertToCharsetL.
        * @since Series 60 3.0
        * @param character set id.
        */
        IMPORT_C void ConvertToCharsetL(TUint aCharsetId);

        /**
        * inline MessageBody.
        * @since Series 60 3.0
        * @return pointer to message body text. Ownership not transferred.
        */
        CRichText* MessageBody();

        /**
        * AttachmentManager.
        * @since Series 60 3.0
        * @return attachment manager
        */
        IMPORT_C MMsvAttachmentManager& AttachmentManager() const;

		/**
		* LoadAttachments
		* @param aObserver load observer.
		* client will be notified trought aObserver when loading is done.
		*/
		IMPORT_C void LoadAttachmentsL(MMessageLoadObserver& aObserver);

		/**
		* LoadAttachedMessagesL
		* @param aObserver load observer.
		* client will be notified trought aObserver when loading is done.
		*/
		IMPORT_C void LoadAttachedMessagesL(MMessageLoadObserver& aObserver);

		/**
		* LoadHtmlContentL
		* @param aObserver load observer.
		* client will be notified trought aObserver when loading is done.
		*/
		IMPORT_C void LoadHtmlContentL(MMessageLoadObserver& aObserver);

        /**
        * LoadLinkedHtmlContentL.
        * @since Series 60 3.0
        * @param aBase. Base
        * @param aURI. Content to be searched.
        * @param aObserver load observer.
        * Client must call "LoadHtmlContentL" before calling this method.
        * Result can be read by calling "LinkedHtmlContent"
        * when observer is notified.
        */
        IMPORT_C void LoadLinkedHtmlContentL(
            const TDesC& aBase,
            const TDesC& aURI,
            MMessageLoadObserver& aObserver );

		/**
		* GetFileHandleL
		* Returns file handle to attachment.
		* @param aId attachment id.
		* Caller must close the handle.
		*/
        IMPORT_C RFile GetFileHandleL( TMsvAttachmentId aId );

        /**
        * Overrides the html character set
        */
        IMPORT_C void OverrideHtmlMessageCharset( TUint aCharsetId );

        /**
        * inline MessageEntry
        * @return message TMsvEmailEntry
        */
        TMsvEmailEntry MessageEntry();

        /**
        * inline MessageType
        * @return message type uid (iMtm)
        */
        TUid MessageType();

        /**
        * inline HtmlContent
        * @return File handle to HTML content.
        * Client must close the handle.
        */
        RFile HtmlContent();

        /**
        * inline LinkedHtmlContent
        * @return File handle to linked HTML content
        * Client must close the handle.
        */
        RFile LinkedHtmlContent();

        /**
        * inline Session
        * @return Messge server session. Ownership not transferred.
        */
        CMsvSession* Session();

        /**
        * inline Selection
        * @return Message selection
        */
        const CMsvEntrySelection& Selection();

        /**
        * inline LinkedHtmlId
        * @return Linked content id
        */
        TMsvId LinkedHtmlId();

        /**
        * inline HtmlFileId
        * @return Html file id
        */
        TMsvId MhtmlPartId();

        /**
        * inline SetNew
        * @param aNew ETrue if new message.
        */
        void SetNew( TBool aNew );

        /**
        * inline IsNew
        * @return ETrue if new message.
        */
        TBool IsNew() const;

    public: // Functions from base classes

    protected: // Functions from base classes
        // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private:  // Implementation
        void DoNextStateL();
        TUint DoGetMimeCharsetIdL( CMsvEntry& aEntry );
        TUint DetermineCharactersetIdFromMimeHeaderL( CMsvEntry& aEntry,
        									const CImMimeHeader* aMime );
        TBool NextStateL();
        void LoadBodyTextL();
        void LoadHTMLContentL();
        void LoadLinkedHTMLContentL(const TDesC& aBase, const TDesC& aURI);
        void LoadAttachmentsL();
        void LoadAttachedMessagesL();
        void NotifyObserverL(TInt aState);
        TBool IsLinkedItemL( RFile& aHandle, TMsvAttachmentId aId );
        void ShowErrorNoteL( TInt aError );
		// cleanups the array of active scheduler waits
        void CleanupWaitArray();
    private:
        /**
        * constructor.
        */
        CMailMessage();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CMsvEntry& aEntry);

    private:    // Data
        TBool iNewMessage;
        // message id
        TMsvId iMessageID;
        // own: copy of message entry
        CMsvEntry* iEntry;
        // Own: Mail message
        CImEmailMessage* iMessage;
        // Own: Message header
        CImHeader* iHeader;
        // Own: Message Body text
        CRichText* iBodyText;
        // Own: Para & Char layer for rich text body
        CParaFormatLayer* iGlobalParaLayer;
        CCharFormatLayer* iGlobalCharLayer;
        // Internal loading state
        TInt iState;
        // Html file handle
        RFile iHtmlFile;
        // html id
        TMsvId iMhtmlPartId;
        // linked content id
        TMsvId iLinkedfileId;
        // Linked html file
        RFile iLinkedHtmlFile;
        // messge flags
        TInt iFlags;
        // character set id
        TUint iCharacterSetId;
        // Ref: Mail viewer app ui
        MMessageLoadObserver* iMessageObserver;
        // Own: uri address
        HBufC* iURI;
        // Own: linked html items array
        RPointerArray<CLinkedHtmlItem>* iLinkedHtmlItems;
        // Own: object which converts asynchronous -> synchronous
        // must be a member to handle exiting during wait properly
        RPointerArray<CMuiuOperationWait> iWaitArray;
        CMuiuOperationWait* iAsyncWait;
    };

#include "CMailMessage.inl"

#endif      // CMAILMESSAGE_H

// End of File
