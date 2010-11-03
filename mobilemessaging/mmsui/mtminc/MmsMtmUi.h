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
*       CMmsMtmUi - UI part of the MMS MTM type to Symbian OS Messaging
*
*/



#ifndef MMSMTMUI_H
#define MMSMTMUI_H

//  INCLUDES
#include <mtmuibas.h>
#include <MuiuMsgEditorLauncher.h>  // for launch flags

#include <mmsconst.h>

#include "MmsSettingsHandler.h"
#include "MmsSettingsDialog.h"

// CONSTANTS
// Expose, if needed:
// #define KMtmUiFirstFreeMmsUiFunctionId (KMtmUiFirstFreePlatformMTMFunctionId + 0x1000)


// MACROS


// DATA TYPES


   // Editor types used to launch the correct application
enum TEditorType
    {
    EReadOnly,
    EEditExisting,
    ECreateNewMessage,
    ESpecialEditor,
    EReadOnlyAudioMessage,
	ECreateNewAudioMessage
    };

// FUNCTION PROTOTYPES


// FORWARD DECLARATIONS
class CMmsClientMtm;
class CMmsMtmDeliveryDialogLauncher;

// CLASS DECLARATION

/**
* CMmsMtmUi - UI part of the MMS MTM type to Symbian OS Messaging
*
* @since 0.9
*/
class CMmsMtmUi : public CBaseMtmUi
    {
    public:  // Constructors and destructor
        
        /**
        * Symbian OS constructor
        * @param aBaseMtm client MTM
        * @param aRegisteredMtmDll registry dll
        * @return a new MMS MTM UI object.
        */
        static CMmsMtmUi* NewL(
            CBaseMtm& aBaseMtm,
            CRegisteredMtmDll& aRegisteredMtmDll );
        
        /**
        * Destructor.
        */
        ~CMmsMtmUi();
        
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
        * From CBaseMtmUi: Create a new entry. Launches MMS editor.
        * Asynchronous.
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
        * Suspends sending of the selected messages (if possible)
        */
        CMsvOperation* CancelL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
        * From CBaseMtmUi: Handles open request to the message or service
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
        * From CBaseMtmUi: Opens MmsEditor for editing a message. If
        * service entry opens MMS Settings dialog for editing MMS settings.
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
        * From CBaseMtmUi: Views the message or service entry having context.
        * Asynchronous.
        * Opens MMS Viewer or Settings dialog depending of the entry.
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* ViewL( TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: As the other ViewL(), but gets the first entry in
        * a selection ands views it.
        */
        CMsvOperation* ViewL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
        * From CBaseMtmUi: Opens MmsEditor for message Reply.
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* ReplyL(
            TMsvId aDestination,
            TMsvPartList aPartlist,
            TRequestStatus& aCompletionStatus );
        
        /**
        * From CBaseMtmUi: Opens MmsEditor for forwarding message.
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
        * This does nothing, because MsgErrorWatcher shows all MMS error messages.
        * Furthermore currently MMS engine does not return any sensible progress to report.
        */
        TInt DisplayProgressSummary( const TDesC8& aProgress ) const;
        
        /**
        * From CBaseMtmUi: Get progress information. Not used in Series 60.
        */

        /**
        * From CBaseMtmUi: Get user confirmation for sending.
        */
		CMsvOperation* ConfirmSendL(
			TRequestStatus& aStatus, 
			const CMsvEntrySelection& aSelection, 
			const TSecurityInfo& aClientInfo ); 
			
	    /**
	    * Check whether the given TMsvEntry represents an upload message
	    * @param aEntry TMsvEntry of the message to be checked
	    */
	    static inline TBool IsUpload( const TMsvEntry& aEntry );
			
	    /**
	    * Check whether the given TMsvEntry represents a MMS Postcard message
	    * @param aEntry TMsvEntry of the message to be checked
	    */
	    static inline TBool IsPostcard( const TMsvEntry& aEntry );

		/**
	    * Check whether the given TMsvEntry represents a AMS message
	    * @param aEntry TMsvEntry of the message to be checked
	    */
	    static inline TBool IsAudioMessage( const TMsvEntry& aEntry );

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
        CMmsMtmUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll );
    
        /**
        * Does nothing - done in engine by FactorySettings
        */
        void CreateDefaultSettingsL();
    
        /**
        * Opens settings dialog for the MMS service.
        * @param aCompletionStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* OpenServiceSettingsDialogL(
            TRequestStatus& aCompletionStatus );
    
        /**
        * Checks whether or not MMS service settings are OK. Open settings
        * dialog if not.
        * @param aExitCode settings dialog exit code
        * @return are settings ok or not
        */
        TBool CheckSettingsL( CMmsSettingsDialog::TMmsExitCode& aExitCode ) const;
    
        /**
        * Creates completed operation after settings dialog closing with
        * appropriate parameters.
        * @param aCompletionStatus the operation progress
        * @param aExitCode settings dialog exit code
        * @return completed operation
        */
        CMsvOperation* CompleteSettingsOperationL(
            TRequestStatus& aCompletionStatus,
            const CMmsSettingsDialog::TMmsExitCode& aExitCode = CMmsSettingsDialog::EMmsExternalInterrupt );
    
        /**
        * Actual settings dialog launcher
        * @param aExitCode out: the exit code causing dialog to close
        */
        void LaunchSettingsDialogL(
            CMmsSettingsDialog::TMmsExitCode& aExitCode ) const;
    
        /**
        * Opens message info popup.
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* OpenMessageInfoL(
            TRequestStatus& aCompletionStatus );

        /**
        * Opens postcard info popup.
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* OpenPostcardInfoL(
            TRequestStatus& aCompletionStatus );
            
        /**
        * Opens delivery statys popup.
        * @param aStatus follows the operation progress
        * @param aParameter contains message size from Editor, otherwise empty
        * @return started operation
        */
        CMsvOperation* OpenDeliveryPopupL(
            TRequestStatus& aCompletionStatus,
            TDes8& aParameter ); 
    
        /**
        * Creates a copy from the current message to be sent to Upload service
        * either directly (sent immediately) or indirectly (open to MMS Editor)
        *
        * @param aSelection The entry to be uploaded
        *                   Notice: Only one entry supported!
        * @param aCompletionStatus follows the operation progress
        * @param aParameter Parameter package of type TMsgUploadParameters
        *                   Defined in "MuiuMsgUploadParam.h"
        *                   Includes real address, alias and direct/indirect info
        * @return started operation
        */
        CMsvOperation* UploadL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus,
            TDes8& aParameter );

        /**
        * Performs DRM checks for Upload messages
        * @return ETrue, if message is allowed to be sent,
        *         EFalse, otherwise
        */
        TBool UploadDRMChecksL();

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
        * @param aMobileTerminated whether this is sent or received message 
        */
        void TurnRecipientsArrayIntoStringsLC(
                            const   CMsvRecipientList&  aRecip,
                            HBufC*&                     aTo,
                            HBufC*&                     aCc,
                            HBufC*&                     aBcc, 
                            TInt                        aAddHeadersVariation,
                            TBool                       aMobileTerminated ) const; 

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
    
        /**
        * @return TInt message size including size of attachment files and subject.
        *         Neither any kind of headers nor recipients fields are calculated ínto figure.
        */
        TInt MessageSizeL() const;
    
        /**
        * Factory settings initializing function
        * @param aLevel normal or deep
        */
        void HandleFactorySettingsL( TDes8& aLevel );
    
        /**
        * Checks if ap exists
        * @param aAp, access point id
        */
        TBool ApExistsL( TInt32 aAp );

    private:    // Data
        CMmsSettingsHandler*    iSettingsHandler;
        CMmsMtmDeliveryDialogLauncher* iLauncher;
        TBool                   iPriorityFeatureSupported;
    };

inline TBool CMmsMtmUi::IsUpload( const TMsvEntry& aEntry )
    {
    if ( aEntry.iBioType == KUidMsgSubTypeMmsUpload.iUid )
        {
        return ETrue;
        }
    return EFalse;
    }

inline TBool CMmsMtmUi::IsPostcard( const TMsvEntry& aEntry )
    {
    if ( aEntry.iBioType == KUidMsgSubTypeMmsPostcard.iUid )
        {
        return ETrue;
        }
    return EFalse;
    }

inline TBool CMmsMtmUi::IsAudioMessage( const TMsvEntry& aEntry )
    {
    if ( aEntry.iBioType == KUidMsgSubTypeMmsAudioMsg.iUid )
        {
        return ETrue;
        }
    return EFalse;    
    }
    
#endif      // MMSMTMUI_H
    
    // End of File
    
