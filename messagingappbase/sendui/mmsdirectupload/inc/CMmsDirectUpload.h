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
* Description:   Provides MMS direct upload service.
*                Creates and sends as many MMS messages as there are files
*                to be send.
*
*/



#ifndef CMMSDIRECTUPLOAD_H
#define CMMSDIRECTUPLOAD_H

// INCLUDES
#include <CSendingService.h>
#include <ConeResLoader.h>

#include <centralrepository.h>
#include <MessagingInternalCRKeys.h>
#include <MmsEngineInternalCRKeys.h>

// DATA TYPES
enum TCompressionResult
    {
    ECompressionOk = 0,
    ECompressionNok,
    EIndexLast
    };

// FORWARD DECLARATIONS
class CCoeEnv;
class CSendingServiceInfo;
class CMessageData;
class CSendUiAttachment;
class CSendUiSingleton;

// CLASS DECLARATION

/**
 * CMmsDirectUpload provides MMS direct upload service.
 * Creates and sends as many MMS messages as there are files to be send.
 *
 * @lib CMmsDirectUpload.dll
 * @since Series 60 2.8
 */ 
class CMmsDirectUpload : public CSendingService
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMmsDirectUpload* NewL( TSendingServiceParams* aParams );
        
        /**
        * Destructor.
        */
        virtual ~CMmsDirectUpload();

    public: // Functions from base classes

        /**
        * From CSendingService
        * Populates given list with the provided services.
        * @since Series 60 2.8
        * @param aList Service list to be populated. 
        * @return None
        */
        void PopulateServicesListL(
            RPointerArray<CSendingServiceInfo>& aList );

        /**
        * From CSendingService
        * Creates to outbox as many MMS messages as there are files to be send.
        * @since Series 60 2.8
        * @param aServiceUid Uid of the sending service.
        * @param aMessageData Data for the message.
        * @return none
        */
        void CreateAndSendMessageL(
            TUid aServiceUid,
            const CMessageData* aMessageData,
            TBool aLaunchEditorEmbedded = ETrue );

        /**
        * From CSendingService
        * Returns the id of the service provider.
        * @since Series 60 2.8
        * @return Id of the service provider.
        */
        TUid ServiceProviderId() const;

       /**
        * Return technology type of a given service
        *
        * @since Series 60 v3.1
        * @return TUid Technology type id
        */ 
        TUid TechnologyTypeId( ) const;

    private: // Constructors
    
        /**
        * C++ default constructor.
        */
        CMmsDirectUpload( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

    private: // New functions

        /**
        * Reads service data from resources and from shared data or from
        * central repository.
        * @since Series 60 2.8
        * @return None.
        */
        void ReadServiceDataL();

        /**
        * Validates attachments for sending.
        * DRM protection and MMS validity are checked.
        * DRM query and error note is shown if needed.
        * Return value of attachment arrays contains valid attachments.
        * @since Series 60 3.0
        * @param aMessageData Message data package containing original
        *        attachments.
        * @param aValidAttachmentPaths Return value contains valid
        *        attachment paths.
        * @param aValidAttachmentHandles Return value contains valid
        *        attachment handles.
        * @return EFalse, if user cancels sending, otherwise ETrue.
        */
        TBool ValidateAttachmentsL( CArrayPtrFlat<CSendUiAttachment>* aAttachments );

        /**
        * Shows confirmation query about sending of files in separate messages
        * to user.
        * @since Series 60 2.8
        * @param aQueryId Resource ID for the query type.
        * @param aQueryTextId Resource ID for the query text.
        * @param aQueryValue Value to be added to query text.
        * @return The return value returned by CAknQueryDialog::ExecuteLD.
        */
        TBool ShowConfirmationQueryL(
            const TUint& aQueryId,
            const TUint& aQueryTextId,
            TInt aQueryValue = 0 ) const;

void ErrorHandlerL( TInt aErr, TInt aFileCount ) const;

        /**
        * Show confirmation query to the user if images have been compressed. 
        * @since Series 60 2.8
        * @param aFileCount Total amount of files to be send.
        * @param
        * @param
        * @return None.
        */
        void ResolveCompressionQueryL(
            TInt aFileCount,
            TInt aCompressionNok ) const;

        /**
        * Show error note to the user.
        * @since Series 60 2.8
        * @param aResourceId Id of the resource for the error note text.
        * @return None.
        */      
        void ShowErrorNoteL( TInt aResourceId ) const;


    private:  // Data


        // For UI resource loading
        RConeResourceLoader                     iResourceLoader;
        

        // Contains pointers to the data of the services provided by this plugin.
        // Pointed service data objects are owned.
        RPointerArray<CSendingServiceInfo>      iServiceArray;

        // Maximum message size. (Same as maximum size for original MMS message)
        TInt                                    iMaxMsgSize;
        
    };

#endif      // CMMSDIRECTUPLOAD_H

// End of File
