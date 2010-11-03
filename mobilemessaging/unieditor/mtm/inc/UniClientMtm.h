/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Client Mtm for uni messaging.
*     This is the API for accessing uni messages. An uni message ends up to be a sms or mms message.
*
*/



#ifndef __UNICLIENTMTM_H__
#define __UNICLIENTMTM_H__

//  INCLUDES
#include  <mtclbase.h> // base client mtm
#include  <e32std.h>   // TTimeInterval & TTime


// CONSTANTS

// MACROS

// DATA TYPES

enum TUniMessageTypeSetting
    {
    EUniMessageTypeSettingAutomatic = 0,
    EUniMessageTypeSettingSms, // This includes also fax & pager
    EUniMessageTypeSettingMms
    };

enum TUniMessageTypeLocking
    {
    EUniMessageTypeLockingNotSet = 0, // This means it's not handled yet by anyone
    EUniMessageTypeLocked,
    EUniMessageTypeNotLocked
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CUniHeaders;
class CMsvMimeHeaders;
class CMsvFindText;
class CMmsAttachmentWaiter;
class CMsgTextUtils;

// CLASS DECLARATION

/**
*  Client Mtm for unified messaging subsystem.
*  This class will be the interface to the UI component
*  and other messaging component that might want to handle unified messages
*  (For example SendAs interface).
*
*   Note: new functions has to be added as last virtual functions in order
*   not to to break the vtable 
*/
class CUniClientMtm :public CBaseMtm
    {
    public:  // Constructors and destructor
        // constructor is private!

        /**
        * Factory function.
        * The only function exported by this polymorphic interface dll.<br>
        * This function is not directly called by the application that needs
        * access, it is called by an instance of CClientMtmRegistry class.
        * @param  aRegisteredMtmDll Reference to Mtm Dll registry class
        * @param  aSession Reference to a Message Server session.
        * @return Pointer to CUniClientMtm class.
        *
        * <PRE>
        * Example of getting access to this class:
        *
        * // Called by a Uikon application that implements
        * // MMsvSessionObserver interface
        *
        * iSession = CMsvSession::OpenSyncL(*this);
        * CleanupStack::PushL(iSession);
        * iClientMtmRegistry = CClientMtmRegistry::NewL(*iSession);
        * CleanupStack::PushL(iClientMtmRegistry);
        * iClientMtm = (CUniClientMtm *) iClientMtmRegistry->
        *               NewMtmL(KUidUniMtm);
        * CleanupStack::PushL(iClientMtm);
        *
        * // - do other initialization -
        *
        * CleanupStack::Pop(3);    //iSession, iClientMtmRegistry, iClientMtm
        *
        * // - call any public functions in CUniClientMtm
        *
        * // When the application finishes,
        * // it must delete the objects in reverse order:
        * delete iClientMtm;
        * delete iClientMtmRegistry;
        * delete iSession;
        * </PRE>
        */
        IMPORT_C static CUniClientMtm* NewL(
            CRegisteredMtmDll& aRegisteredMtmDll,
            CMsvSession& aSession );
        
        /**
        * Destructor.
        */
        virtual ~CUniClientMtm();

    public:  // New functions

        // ----------------------------------------------------------
        // Functions to create and modify message entries

        /**
        * Create a new message entry
        *
        * @param aDestination target folder
        * @return Id of the created message
        */
        virtual TMsvId CreateNewEntryL( TMsvId aDestination );
        
        // -------------------------------------------------------------------
        // GENERAL MESSAGE INFORMATION METHODS 

        /**
        * Message size accessor. SaveMessageL and LoadMessageL updates the 
        * value.
        * @return size of all message parts in bytes including both
        * attachments and internal header structures. The actual message
        * size in transmission is smaller due to the header
        * binary encoding.
        */
        virtual TInt32 MessageSize();
              
        /**
        * Message description mutator. 
        * This provides a method to override the default message description.
        * The next SaveMessageL saves the description text in the 
        * TMsvEntry::iDescription.
        * Note that this method does not check the text length, so avoid long 
        * descriptions to minimize memory usage.
        * @param aText message description
        */
        virtual void SetMessageDescriptionL( const TDesC& aText );
       
        // -------------------------------------------------------------------
        // FUNCTIONS TO HANDLE MESSAGE ATTACHMENTS

        /**
        * Create text/plain attachment. <br>
        * Creates a text attachment from descriptor.
        * Has option to convert all unicode paragraph separator marks to
        *     line feeds.
        * Converts text from unicode (ucs-2) to utf-8 before storing it.
        * @param aStore an open EditStore. Caller must commit store
        *     (several attachments can be added before committing store)
        * @param aAttachmentId returned ID of the new attachment entry
        * @param aText UNICODE text to be added as a text/plain attachment.
        * @param aFile suggested filename for the attachment
        * @param aConvertParagraphSeparator flag to tell if the function 
        *     will search for all 0x2029 characters (Unicode paragraph
        *     separator) and replace them with 0x000a (line feed).
        *     aConvertParagraphSeparator == ETrue: convert
        *     aConvertParagraphSeparator == EFalse: do not convert
        */
        virtual void CreateTextAttachmentL(
            CMsvStore& aStore,
            TMsvAttachmentId& aAttachmentId,
            const TDesC& aText,
            const TDesC& aFile,
            TBool aConvertParagraphSeparator = ETrue );
       
        /**
        * Accessor for message type setting
        * @return Message type setting
        */
        virtual TUniMessageTypeSetting MessageTypeSetting() const;
        
        /**
        * Mutator for message type setting
        * @param aSetting Message type setting
        */
        virtual void SetMessageTypeSetting( TUniMessageTypeSetting aSetting );

        /**
        * Accessor for message type locking
        * @return Message type locking
        */
        virtual TUniMessageTypeLocking MessageTypeLocking() const;
        
        /**
        * Mutator for message type locking
        * @param aLocking Message type locking
        */
        virtual void SetMessageTypeLocking( TUniMessageTypeLocking aLocking );

        /**
        * Accessor for message root
        * @return Message root
        */
        virtual TMsvAttachmentId MessageRoot() const;
        
        /**
        * Mutator for message root
        * @param aLocking Message root
        */
        virtual void SetMessageRoot( TMsvAttachmentId aRoot );

        /**
        * From CBaseMtm: Store current entry data.
        * The caller must set the message to complete and visible when it is ready.
        */
        virtual void SaveMessageL( CMsvStore& aEditStore, TMsvEntry& aEntry ); 

    public:  // FUNCTIONS FROM BASE CLASSES

        /**
        * From CBaseMtm: Return type of this Mtm
        * @return Registered Mtm type
        */
        inline TUid Type() const;

        // Context specific functions

        /**
        * From CBaseMtm: Set current context
        * @param aEntry Pointer to entry instance
        */
        inline void SetCurrentEntryL( CMsvEntry* aEntry );        

        /**
        * From CBaseMtm: Switch context to entry defined by aId.
        * @param aId Entry id in message store.
        */
        inline void SwitchCurrentEntryL( TMsvId aId );
        
        /**
        * From CBaseMtm: return reference to current entry
        * @return reference to entry instance
        */
        inline CMsvEntry& Entry() const;
        
        /**
        * From CBaseMtm: Query if entry context has been set
        * @return ETrue, if context has been set, EFalse, if not
        */
        inline TBool HasContext() const;

        // Message specific functions

        /**
        * From CBaseMtm: Store current entry data.
        * The caller must set the message to complete and visible when it is ready.
        */
        void SaveMessageL(); 

        /**
        * From CBaseMtm: Restore current entry data.
        */
        void LoadMessageL();

        /**
        * From CBaseMtm: Validates that selected parts of current message are
        *     legal.
        * @param aPartList Flags specifying which parts to validate.
        *     (defined in MTMDEF.H).
        *     KMsvMessagePartPartBody is ignored.
        *     KMsvMessagePartRecipient is supported.
        *     KMsvMessagePartOriginator is supported.
        *     KMsvMessagePartDescription is ignored.
        *     KMsvMessagePartDate is ignored.
        *     KMsvMessagePartAttachments is supported.
        * @return Flags that specify which or the specified parts were valid.
        */
        TMsvPartList ValidateMessage( TMsvPartList aPartList );
        
        /**
        * From CBaseMtm: Searches for specified text in selected parts of
        * current message.
        * @param aPartList Flags specifying which parts to search.
        *     (defined in MTMDEF.H).
        *     KMsvMessagePartPartBody is ignored.
        *     KMsvMessagePartRecipient is supported.
        *     KMsvMessagePartOriginator is supported.
        *     KMsvMessagePartDescription is supported.
        *     KMsvMessagePartDate is ignored.
        *     KMsvMessagePartAttachments is ignored.
        * Following find attributes are supported:
        *     KMsvFindCaseSensitive
        *     KMsvFindWholeWord
        * @return Flags that specify in which of the specified parts the text
        *     was found.
        */
        TMsvPartList Find( const TDesC& aTextToFind, TMsvPartList aPartList );

        /**
        * From CBaseMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        CMsvOperation* ReplyL(
            TMsvId aDestination,
            TMsvPartList aPartlist,
            TRequestStatus& aCompletionStatus );
        
        /**
        * From CBaseMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        CMsvOperation* ForwardL(
            TMsvId aDestination,
            TMsvPartList aPartList,
            TRequestStatus& aCompletionStatus );

        /**
        * From CBaseMtm: Adds an addressee. Cannot distiguish To, Cc, and Bcc.
        * New addresses are handled as To type of addresses.
        * @param aRealAddress Reference to an address string
        */
        void AddAddresseeL( const TDesC& aRealAddress );
        
        /**
        * From CBaseMtm: Adds an addressee. Cannot distiguish To, Cc, and Bcc.
        * New addresses are handled as To type of addresses. 
        * @param aRealAddress Reference to an address string
        * @param aAlias Reference to a descriptive name (not a real address)
        */
        void AddAddresseeL( const TDesC& aRealAddress, const TDesC& aAlias );
        
        /**
        * From CBaseMtm: Adds a typed addressee (To, Cc or Bcc )
        * @param aType recipient type (EMsvRecipientTo, EMsvRecipientCc,
        *     or EMsvRecipientBcc)
        * @param aRealAddress Reference to an address string
        */
        virtual void AddAddresseeL(
            TMsvRecipientType aType,
            const TDesC& aRealAddress);
        
        /**
        * From CBaseMtm: Adds a typed addressee (To, Cc or Bcc )
        * @param aType recipient type (EMsvRecipientTo, EMsvRecipientCc,
        *     or EMsvRecipientBcc)
        * @param aRealAddress Reference to an address string
        * @param aAlias Reference to a descriptive name (not a real address)
        */
        virtual void AddAddresseeL(
            TMsvRecipientType aType,
            const TDesC& aRealAddress,
            const TDesC& aAlias);
        
        /**
        * From CBaseMtm: Removes an entry from addressee list.
        *     Cannot distinguish To, Cc and Bcc.
        * @param aIndex Index to the array of addresses.
        */
        void RemoveAddressee( TInt aIndex );

        /**
        * From CBaseMtm Rich text body accessor
        * @return reference to the rich text body
        */
        inline CRichText& Body();

        /**
        * From CBaseMtm Rich text body accessor
        * @return reference to the rich text body
        */
        inline const CRichText& Body() const;

        /**
        * From CBaseMtm: Sets the message subject
        * @param aSubject Subject as text string.
        */
        void SetSubjectL( const TDesC& aSubject );
        
        /**
        * From CBaseMtm: Subject accessor
        * @return Constant pointer descriptor pointing to subject
        */
        const TPtrC SubjectL() const;

        // General MTM-specific functionality

        /**
        * From CBaseMtm:
        * @param aCapability Uid specifying which capablity is queried
        * @param aResponse reference to an integer that will contain the value
        *     describing the capability at return.
        * @return error code, KErrNone, if specified capability is known
        *     to Mtm,
        *     KErrNotSupported, if capability Uid is unknown
        */
        TInt QueryCapability( TUid aCapability, TInt& aResponse );

        /**
        * From CBaseMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        void InvokeSyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TDes8& aParameter );
        
        /**
        * From CBaseMtm 
        * Not supported. 
        * Returns operation with status: KErrNotSupported
        */
        CMsvOperation*  InvokeAsyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TDes8& aParameter,
            TRequestStatus& aCompletionStatus );
        
        /**
        * From CBaseMtm: Return session that was set at initialization
        * @return Reference to session object
        */
        inline CMsvSession& Session();

        // Functions for SendAs support
        // These functions get the edit store from the current entry.
        // The caller should not hold the store open.
        // The store is committed and closed after each operation
        
        /**
        * From CBaseMtm Adds a file attachment to the current message entry.
        * The attachment is referenced by its file path and is copied into the
        *     message store. 
        * Only one asynchronous operation can be run at any one time.
        * @param aFilePath The full path specification of the attachment file.
        * @param aMimeType The mime type of the attachment file.
        * @param aCharset The character set in case the attachment is of text type
        *        if not relevant for current attachment type, set aCharset to 0
        * @param aStatus The request status to complete.
        * @leave System-wide error codes.
        */
        void AddAttachmentL( const TDesC& aFilePath,
            const TDesC8& aMimeType,
            TUint aCharset,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtm Adds a file attachment to the current message entry.
        * The attachment is referenced by an open file handle and is copied
        *     into the message store. 
        * Only one asynchronous operation can be run at any one time.
        * @param aFile An open file handle for the file attachment.
        *        The function closes the file handle when done.
        *        The caller should not attempt to close the handle afterwards
        *        Use as follows:
        *        CleanupStack::CleanupClosePush( aFile );
        *        AddAttachmentL( aFile, ... );
        *        CleanupStack::Pop(); // get handle off cleanup stack
        * @param aMimeType The mime type of the attachment file.
        * @param aCharset The character set in case the attachment is of text type
        *        if not relevant for current attachment type, set aCharset to 0
        * @param aStatus The request status to complete.
        * @leave System-wide error codes.
        */
        void AddAttachmentL( RFile& aFile,
            const TDesC8& aMimeType,
            TUint aCharset,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtm Adds a file attachment to the current message entry
        *     as a linked file.
        * The attachment is referenced by its file path and is not copied
        *     into the message store. The attachment file is always used from
        *     its original location on disk indicated by the aFilePath
        *     parameter. Only one asynchronous operation can be run at any
        *     one time.
        * @param aFilePath The full path specification of the attachment file.
        * @param aMimeType The mime type of the attachment file.
        * @param aCharset The character set in case the attachment is of text type
        *        if not relevant for current attachment type, set aCharset to 0
        * @param aStatus The request status to complete.
        * @leave System-wide error codes.
        */
        void AddLinkedAttachmentL( const TDesC& aFilePath,
            const TDesC8& aMimeType,
            TUint aCharset,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        void AddEntryAsAttachmentL( TMsvId aAttachmentId,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtm
        * @param aFileName suggested filename
        * @param aAttachmentFile An open file handle for the file attachment.
        * @param aMimeType The mime type of the attachment file.
        * @param aCharset The character set in case the attachment is of text type
        *        if not relevant for current attachment type, set aCharset to 0
        * @param aStatus The request status to complete.
        * @leave System-wide error codes.
        */
        void CreateAttachmentL( const TDesC& aFileName,
            RFile& aAttachmentFile,
            const TDesC8& aMimeType,
            TUint aCharset,
            TRequestStatus& aStatus);

        /**
        * From CBaseMtm Cancels the current attachment operation.
        */
        void CancelAttachmentOperation();
        
        // End of attachment funtions to support SendAs

        /**
        * From CBaseMtm: Create an empty entry in the outbox<br>
        * and set it as current context.<br>
        * The entry will be invisible and under construction.
        * @param aServiceId Service id for the new entry.
        */
        void CreateMessageL( TMsvId aServiceId );
        
        /**
        * From CBaseMtm:BIO functions to support the SendAs API.
        * The function is not supported.
        * Entry().Entry().iBioType will be set by SendAs if this function does
        *    not leave.
        * The default implementation in CBaseMtm is to do nothing.
        */
        void BioTypeChangedL( TUid aBioTypeUid );
    
        /**
        * From CBaseMtm: Return id of default service.
        * @return default service
        */
        TMsvId DefaultServiceL() const;
        
        /**
        * From CBaseMtm: Remove default service
        * Does nothing. Deletion of service not supported
        */
        void RemoveDefaultServiceL();
        
        /**
        * From CBaseMtm: Change default service
        * Does nothing. Changing of default service not supported
        */
        void ChangeDefaultServiceL(const TMsvId& aService);

    protected:  // New functions


    protected:  // Functions from base classes
        
        /**
        * From CBaseMtm: Called after the context of this instance
        * has been changed to another entry.
        */
        void ContextEntrySwitched();

        /**
        * From CBaseMtm: React to changes 
        * This functions does not do anything.
        * @param aEvent Code that tells which event has occurred.
        *     Event codes defined in MSVAPI.H
        * @param arg1 Depends on Event
        * @param arg2 Depends on Event
        * @param arg3 Depends on Event
        */
        void HandleEntryEventL(
            TMsvEntryEvent aEvent,
            TAny* arg1,
            TAny* arg2,
            TAny* arg3 );

        /**
        * By default Symbian OS constructor is private.
        * @param aRegisteredMtmDll Reference to Mtm Dll registry class
        * @param aSession Reference to a Message Server session.
        */
        CUniClientMtm(
            CRegisteredMtmDll& aRegisteredMtmDll,
            CMsvSession& aSession );

        void ConstructL();
        
    private:

        /**
        * This method builds the iAddresseeList from the iUniHeaders data.
        */
        void BuildAddresseeListL();

        /**
        * This methods adds entries from the the specified array to
        *     iAddresseeList.
        * @param aArray recipient array.
        */
        void BuildAddresseeListL(
            const CDesCArray& aArray, TMsvRecipientType aValue); 

        /**
        * Attachments size
        * @return size of all attachments, binary data + mime headers.
        */
        TInt32 AttachmentsSizeL();

        /**
        * Attachments size
        * @return size of all attachments, binary data + mime headers.
        */
        TInt32 AttachmentsSizeL( CMsvStore& aStore );

        /**
        * Find text from the given recipient list.
        * @param aTextToFind a text to be searched
        * @param aPartList message part list
        * @param aRecipients the recipient list
        * @param aFindText CMsvFindText object to help in the find
        * @return ETrue if match found.
        */
        TBool FindInRecipientL( 
            const TDesC& aTextToFind,
            TMsvPartList aPartlist,
            const CDesCArray& aRecipients,
            CMsvFindText& aFindText );
            
        /**
        * Add an attachment from public location either as copied file or
        *     linked file.
        * @param aFilePath The full path specification of the attachment file.
        * @param aMimeType The mime type of the attachment file.
        * @param aType CMsvAttachment::EMsvFile or
        *     CMsvAttachment::EMsvLinkedFile
        * @param aStatus The request status to complete when request has
        *     completed.
        * @param aCharacter set IANA MIBEnum of the character set for the
        *     attachment if needed.
        */
        void AddFilePathAttachmentL(const TDesC& aFilePath,
            const TDesC8& aMimeType,
            CMsvAttachment::TMsvAttachmentType aType,
            TRequestStatus& aStatus,
            const TUint aCharacterSet = 0 );
        
        /**
        * Tries to recognize character set of the given file. 
        * @since    3.0
        * @param    aFile       IN File to be recognized
        * @return   MIB id of the character set
        */
        TUint RecognizeCharSetL( RFile& aFile );
        
        /**
        * Sets the needed data fields for CMsvAttachment.
        * Creates and initializes CMsvMimeHeaders for the attachment.
        *
        * @return Size of the mime headers in bytes
        */
        TInt InitializeAttachmentL(
            MMsvAttachmentManager& aManager,
            CMsvAttachment& aAttachment,
            const TDesC& aFileName,
            const TDesC8& aMimeType,
            TInt aFileSize,
            TUint aCharset );
            
        /**
        * Sets the content location mime header.
        * This function needs to be called when attachments have been converted
        *    from ucs-2 to utf8.
        * Other mime headers have been correctly set by the conversion routine
        *
        * @param aStore edit store
        * @param aAttaId attchement if for the attachment to be handled
        * @param aFilename suggested filename
        */
        void SetContentLocationForConvertedAttL(
            CMsvStore& aStore,
            TMsvAttachmentId aAttaId,
            const TDesC& aFileName );
            
        
        /**
        * See function Find. DoFind is called in TRAP in Find function.
        */
        TMsvPartList DoFindL( const TDesC& aTextToFind, TMsvPartList aPartList );

            

    public:     // Data
    
    protected:  // Data
        
        CUniHeaders*  iUniHeaders;   // Unified message headers        
        TInt          iMessageDrive; // messages are on C: drive by default,
                                     // may be moved to other drive
        CMsgTextUtils* iTextUtils;
        
    private:    // Data
        // active object that commits the store when attachment operation
        // is complete
        CMmsAttachmentWaiter* iAttaWaiter;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#include "UniClientMtm.inl"

#endif      // __UNICLIENTMTM_H__   
            
// End of File

