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
* Description:   Provides DRM link sender service.
*                Adds DRM link to SMS message and opens SMS editor.
*
*/




#ifndef DRMLINKSENDER_H
#define DRMLINKSENDER_H

// INCLUDES
#include    <CSendingService.h>
#include    <ConeResLoader.h>
#include    <msvapi.h>
#include    <mtmuibas.h>
#include "CSendUiAttachment.h"

// FORWARD DECLARATIONS

class CSendingServiceInfo;
class CMessageData;
class CSmsClientMtm;
class CClientMtmRegistry;

// CLASS DECLARATION

/**
 * DrmLinkSender provides DRM link sender service.
 * Adds DRM link to SMS message and opens SMS editor.
 *
 * @lib DrmLinkSender.dll
 * @since Series 60 3.0
 */ 
class CDrmLinkSender : public CSendingService
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CDrmLinkSender* NewL( TSendingServiceParams* aParams );
        
        /**
        * Destructor.
        */
        virtual ~CDrmLinkSender();

    public: // Functions from base classes

        /**
        * From CSendingService
        * Populates given list with the provided services.
        * @since Series 60 3.0
        * @param aList Service list to be populated. 
        * @return None
        */
        void PopulateServicesListL(
            RPointerArray<CSendingServiceInfo>& aList );

        /**
        * From CSendingService
        * Adds DRM link to SMS message and opens SMS editor.
        * @since Series 60 3.0
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
        * @since Series 60 3.0
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
        CDrmLinkSender( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );

   private: // New functions

        HBufC8* DrmContentUrlLC( const TDesC& aFilePath, RFs& aFs );

        HBufC8* DrmContentUrlLC( const RFile& aFile );

        void InitializeServiceL();
        
        void CreateMessageAndLaunchEditorL( HBufC8& aContentUrl );

        /**
        * From CSendingService
        * Adds DRM link to MessageData body text and creates as an attachment
        * @since Series 60 5.0
        * @param aContentUrl content url extracted from the DRM protected file.
        * @return none
        */
        void AddAttachmentsL( HBufC8& aContentUrl );

        void AddAttachmentL( CSendUiAttachment* aAttachment);

    private:  // Data

        CBaseMtm* iClientMtm;
        CBaseMtmUi* iMtmUi;

        // Contains pointers to the data of the services provided by this plugin.
        // Pointed service data objects are owned.
        RPointerArray<CSendingServiceInfo>      iServiceArray;

         // For UI resource loading
        RConeResourceLoader*                    iResourceLoader;

        // Maximum message size. (Same as maximum size for original MMS message)
        TInt                                    iMaxMsgSize;

    };

#endif      // DRMLINKSENDER_H

// End of File
