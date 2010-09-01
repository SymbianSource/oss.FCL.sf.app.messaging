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
* Description:   Provides MMS indirect upload service.
*                Creates MMS message and launches the MMS editor.
*
*/




#ifndef CMMSINDIRECTUPLOAD_H
#define CMMSINDIRECTUPLOAD_H

// INCLUDES
#include    <CSendingService.h>
#include    <sendas2.h>
#include    <centralrepository.h>
#include    <MessagingInternalCRKeys.h>
#include    <MmsEngineInternalCRKeys.h>
#include    <coemain.h>

class CSendUiAttachment;
class CSendUiSingleton;

// CLASS DECLARATION

/**
 * CMmsIndirectUpload provides MMS Indirect Upload service.
 * Creates MMS message and launches MMS editor.
 * @lib MmsIndirectUpload.dll
 * @since Series 60 2.8
 */
class CMmsIndirectUpload : public CSendingService
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMmsIndirectUpload* NewL( TSendingServiceParams* aParams );

        /**
        * Destructor.
        */
        virtual ~CMmsIndirectUpload();

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
        * Creates message to be sent and calls MTM to edit the message.
        * @since Series 60 2.8
        * @param aServiceUid Uid of the sending service.
        * @param aMessageData Data for the message.
        * @return none
        */
        void CreateAndSendMessageL(
            TUid aServiceUid,
            const CMessageData* aMessageData,
            TBool aLaunchEditorEmbedded = ETrue);

        /**
        * From CSendingService
        * Returns the id of the service provider.
        * @since Series 60 2.8
        * @return Id of the service provider.
        */
        TUid ServiceProviderId() const;

        TUid TechnologyTypeId( ) const;


    private:

        /**
        * C++ default constructor.
        */
        CMmsIndirectUpload( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:  // New functions

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
        * @return ETrue if sending can continue, otherwise EFalse.
        */
        TBool ValidateAttachmentsL( CArrayPtrFlat<CSendUiAttachment>* aAttachments );

        /**
        * Checks MMS service availability.
        * @since Series 60 3.0
        * @return ETrue when MMS service is available, otherwise EFalse.
        */
        TBool CheckMmsServiceAvailabilityL( RSendAs& aSendAsSession );

        /**
        * Reads service data from resources and from shared data or from
        * central repository.
        * @since Series 60 2.8
        * @return None.
        */
        void ReadServiceDataL();

    private:  // Data

        // Contains pointers to the data of the services provided by this plugin.
        // Pointed service data objects are owned.
        RPointerArray<CSendingServiceInfo>          iServiceArray;

    };

#endif      // CMMSINDIRECTUPLOAD_H

// End of File
