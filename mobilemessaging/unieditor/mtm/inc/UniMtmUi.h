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
*       CUniMtmUi - UI part of the Unified Message MTM type to Symbian OS Messaging
*
*/



#ifndef __UNIMTMUI_H
#define __UNIMTMUI_H

//  INCLUDES
#include <mtmuibas.h>
#include <MuiuMsgEditorLauncher.h> // for launch flags

// CONSTANTS

// MACROS

// DATA TYPES
// Editor types used to launch the correct application
enum TEditorType
    {
    EReadOnly,
    EEditExisting,
    ECreateNewMessage,
    ESpecialEditor
    };

// FUNCTION PROTOTYPES


// FORWARD DECLARATIONS
class CUniClientMtm;


// CLASS DECLARATION

/**
* CUniMtmUi - UI part of the unified message MTM type to Symbian OS Messaging
*
* @since 0.9
*/
class CUniMtmUi : public CBaseMtmUi
    {
    public:  // Constructors and destructor
        
        /**
        * Symbian OS constructor
        * @param aBaseMtm client MTM
        * @param aRegisteredMtmDll registry dll
        * @return a new unified MTM UI object.
        */
        static CUniMtmUi* NewL(
            CBaseMtm& aBaseMtm,
            CRegisteredMtmDll& aRegisteredMtmDll );
        
        /**
        * Destructor.
        */
        ~CUniMtmUi();
        
    public:  // New functions
        
        
    public: // Functions from base classes
        
        /**
        * From CBaseMtmUi: Capability check.
        */
        TInt QueryCapability( TUid aCapability, TInt& aResponse );
        
        /**
        * From CBaseMtmUi: not supported in Series 60
        */
        CMsvOperation* DeleteServiceL(
            const TMsvEntry& aService,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: Create a new entry. Launches unified editor.
        * Asynchronous.
        * Settings are not checked here.
        * @param aEntry entry to be created
        * @param aParent parent entry
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* CreateL(
            const TMsvEntry& aEntry,
            CMsvEntry& aParent,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: not supported in Series 60
        */
        CMsvOperation* DeleteFromL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi:
        * This method is not supported
        */
        CMsvOperation* CancelL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
        * From CBaseMtmUi: Handles open request to the message
        * entry having context. Asynchronous.
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* OpenL( TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: As the other OpenL() but gets the first entry
        * in a selection and opens it.
        */
        CMsvOperation* OpenL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
        * From CBaseMtmUi: not supported in Series 60
        */
        CMsvOperation* CloseL( TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: not supported in Series 60
        */
        CMsvOperation* CloseL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
        * From CBaseMtmUi: Opens UniEditor for editing a message. 
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* EditL( TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: As the other EditL(), but gets the first entry in
        * a selection and edits it.
        */
        CMsvOperation* EditL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
        * From CBaseMtmUi: 
        * This method is not supported
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* ViewL( TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: 
        * This method is not supported
        */
        CMsvOperation* ViewL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
        * From CBaseMtmUi: Opens UniEditor for message Reply.
        * This method is not supported
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* ReplyL(
            TMsvId aDestination,
            TMsvPartList aPartlist,
            TRequestStatus& aCompletionStatus );
        
        /**
        * From CBaseMtmUi: Opens UniEditor for forwarding message.
        * This method is not supported
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* ForwardL(
            TMsvId aDestination,
            TMsvPartList aPartList,
            TRequestStatus& aCompletionStatus );
        
        /**
        * From CBaseMtmUi: Copy a message from outbox to remote service (=send)
        * Context should be in MTM folder/service.
        * @param aSelection messages to be copied/sent
        * @param aStatus follows the operation progress
        * @return operation started
        */
        CMsvOperation* CopyToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: Move a message from outbox to remote service (=send)
        * Context should be in MTM folder/service.
        * @param aSelection messages to be moved/sent
        * @param aStatus follows the operation progress
        * @return operation started
        */
        CMsvOperation* MoveToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: Copy a message from remote service to inbox.
        * Just leaves with "not supported".
        */
        CMsvOperation* CopyFromL(
            const CMsvEntrySelection& aSelection,
            TMsvId aTargetId,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: Move a message from remote service to inbox.
        * Just leaves with "not supported".
        */
        CMsvOperation* MoveFromL(
            const CMsvEntrySelection& aSelection,
            TMsvId aTargetId,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: Perform an asynchronous operation
        */
        CMsvOperation* InvokeAsyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus,
            TDes8& aParameter );
        
        /**
        * From CBaseMtmUi: Perform a synchronous operation
        */
        void InvokeSyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TDes8& aParameter );
        
        /**
        * From CBaseMtmUi: Used for display operation summary (usually completed operations).
        * Not in use.
        */
        TInt DisplayProgressSummary( const TDesC8& aProgress ) const;
        
        static void ConvertUtcToLocalTime( TTime& aTime );
        
       
    protected:  // New functions
    
        /**
        * Handles the actual launching of editor or viewer when viewing or editing a 
        * message.
        * @param aStatus follows the operation progress.
        * @param aSession Message server session to be used.
        * @param aEditorType Editor type used to launch the correct application. 
        *                    Defaults to EEditExisting.
        * @return started operation
        */        
        CMsvOperation* LaunchEditorApplicationL(
            TRequestStatus& aStatus,
            CMsvSession& aSession,
            TEditorType aEditorType = EEditExisting );
    
        /**
        * Performs CopyToL or MoveToL operation
        * @param aSelection the entries to be sent
        * @param aStatus follows the operation progress
        * @param aCopyOnly defines is it copy or move
        * @return started operation
        */
        CMsvOperation* CopyMoveToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus,
            TBool aCopyOnly );
    
    
    protected:  // Functions from base classes
    
        /**
        * From CBaseMtmUi: Returns resource file
        * @param aFileName Store resource file here
        */
        void GetResourceFileName( TFileName& aFileName ) const;
    
    
    private:
    
        /**
        * From CBaseMtmUi: construction.
        * @param
        * @return
        */
        void ConstructL();
    
        /**
        * From CBaseMtmUi: constructor.
        * @param aBaseMtm client MTM reference
        * @param aRegisteredMtmDll Registry DLL
        * @return
        */
        CUniMtmUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll );
   
        /**
        * Opens message info popup.
        * @param aStatus follows the operation progress
        * @param aParameter contains message size from Editor, otherwise empty
        * @return started operation
        */
        CMsvOperation* OpenMessageInfoL(
            TRequestStatus& aCompletionStatus,
            TDes8& aParameter );
        
        /**
        * Creates To:, Cc, and Bcc recipient lists for Message Info.
        * Creation of Cc and Bcc recipients list are created, if 
        * these fields are variated On.
        * Every created recipient buffer object increases pushed object count 
        * by one. Thus pushed object count is 1-3 depending on variation.
        * If a field is variated Off, recipients are appended to 
        * To: or Cc: fields.
        * @param aRecip  IN  Recipient array
        * @param aTo     OUT To recipients array
        * @param aCc     OUT Cc recipients array
        * @param aBcc    OUT Bcc Recipient array
        * @param aAddHeadersVariation variation bit flags
        */
        void TurnRecipientsArrayIntoStringsLC(
                            const   CMsvRecipientList&  aRecip,
                            HBufC*&                     aTo,
                            HBufC*&                     aCc,
                            HBufC*&                     aBcc, 
                            TInt                        aAddHeadersVariation ) const; 
    
        /**
        * Message size - integer to string converter
        * Converts message size in bytes to a kilobyte string. Rounds the bytes up to the
        * next full kilo. I.e:
        * 0 -> 0KB
        * 1 -> 1KB
        * 1024 -> 1KB
        * 1025 -> 2KB
        * @param aTarget returned size string
        * @param aFileSize source size
        */
        void MessageSizeToStringL( TDes& aTarget, TInt aFileSize ) const; 
    
        /**
        * Checks if the message is effectively empty
        * @return boolean - empty or not empty
        */
        TBool IsMessageEmptyL() const;

    private:    // Data
    
        
        
        
    };
    
#endif      // __UNIMTMUI_H
    
    // End of File
    
