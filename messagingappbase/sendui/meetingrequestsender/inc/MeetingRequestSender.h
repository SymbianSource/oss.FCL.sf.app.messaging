/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides meeting request sending service.
*                Service expects to received messages of type CMimeMessageData.
*                Implements CSendingService. 
*
*/



#ifndef CMEETINGREQUESTSENDER_H
#define CMEETINGREQUESTSENDER_H

//  INCLUDES
#include    <e32base.h>
#include    <e32std.h>              // RPointerArray
#include    <msvapi.h>              // MMsvSessionObserver
#include    <f32file.h>             // RFs
#include    <coemain.h>
#include    <CSendingService.h>
#include    <CMessageAddress.h>
#include    <SenduiMtmUids.h>
#include    <CMsvRecipientList.h>
#include    "MeetingRequestSenderUids.hrh"

//  FORWARD DECLARATIONS
class CMsvSession;
class CClientMtmRegistry;
class CBaseMtm;
class TDataType;
class CMeetingRequestData;
class CSendUiAttachment;
class CSendUiSingleton;

//  CLASS DEFINITION
/**
 *
 * CMeetingRequestSender in an ECOM plug-in to Sendui.
 * Plug-in is used to send meeting requests.
 * Sendui is responsible of instantiating and calling the
 * service when needed.
 * @lib MeetingRequestSender.dll
 * @since Series 60 3.0
 */
class CMeetingRequestSender : public CSendingService
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMeetingRequestSender* NewL( TSendingServiceParams* aParams );

        /**
        * Two-phased constructor.
        */
        static CMeetingRequestSender* NewLC( TSendingServiceParams* aParams );

        /**
        * Destructor.
        */
        virtual ~CMeetingRequestSender();

    private:    // Constructors

        /**
        * C++ default constructor.
        */
        CMeetingRequestSender( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton );

        /**
        * By default Symbian 2nd phase constructor is private.
        */  
        void ConstructL( );

    public:     // Methods from CSendingService
        
        void PopulateServicesListL(
            RPointerArray<CSendingServiceInfo>& aList );

        void CreateAndSendMessageL(
            TUid aServiceUid,
            const CMessageData* aMessageData,
            TBool aLaunchEditorEmbedded = ETrue);

        TUid ServiceProviderId() const;
        
        /**
        * Return technology type of a given service
        *
        * @since Series 60 v3.1
        * @return TUid Technology type id
        */ 
        TUid TechnologyTypeId( ) const;

    private:    // New methods

        /**
        * Creates and sends message by using client MTM.
        *
        * @since Series 60 3.0
        * @param aMessageData Data for the meeting request message.
        * @return None.
        */
        void DoCreateAndSendMessageL( const CMessageData& aMessageData,
                                      CMeetingRequestData* aMeetingRequestData );

        /**
        * Transfer iCal IDs to client MTM.
        *
        * @since Series 60 3.0
        * @param aMessageData Data for the meeting request message.
        * @return None.
        */
        void TransferIdsToClientL( const CMessageData& aMessageData );

        /**
        * Creates new message. Index entry is initialized and 
        * new message is created in drafs folder. 
        * @since Series 60 3.0
        * @param aMessageData Message containing subject.
        * @return None.
        */
        void CreateMessageL( const CMessageData& aMessageData );

        /**
        * Add addressees.
        * @since Series 60 3.0
        * @param aRecipientType Recipient type.
        * @param aAddressees Array of addressees.
        * @return None.
        */        
        void AddAddresseesL(
            TMsvRecipientType aRecipientType, 
            const CMessageAddressArray& aAddressees );

        /**
        * Add attachment to email. 
        * @since Series 60 3.0
        * @param Attachment.
        * @return None
        */                                
        void AddAttachmentL( 
            CSendUiAttachment* aAttachment, 
            TInt aLinksSupported );

        /**
        * Remove illegal characters from string. Illegal chars
        * are replaced with spaces. String is trimmed before it is 
        * returned. 
        * @since Series 60 3.0
        * @param aCheckedString String where chars are removed
        * @return None
        */                                                                            
        void RemoveIllegalChars( HBufC* aCheckedString );

        /**
        * Initialize plug-in service info. 
        * @since Series 60 3.0
        * @return None
        */                                                                                    
        void InitializeServiceL();

        /**
        * Set message body.
        * @since Series 60 3.0
        * @param aMessageData Message data which contains the body. 
        * @return None. 
        */                                                                                    
        void SetBodyL( const CMessageData& aMessageData );

        /**
        * Create email headers ("standard" and MIME)
        * @since Series 60 3.0
        * @param aMessageData Message which contains attendees and MIME type.  
        * @return None. 
        */                                                                                    
        void CreateHeaderL( const CMessageData& aMessageData, CMeetingRequestData* aMeetingRequestData );

        /**
        * Add attachments to the email.
        * @since Series 60 3.0
        * @param aMessageData Message which contains attachments.
        * @return None. 
        */                                                                                    
        void AddAttachmentsL( const CMessageData& aMessageData );

        /**
        * Check if there are enough disk space.
        * @since Series 60 3.0
        * @param aMsgSize Total message size including attachments. 
        * @return None. 
        */                                                                                    
        void CheckDiskSpaceL( TInt aMsgSize );

        /**
        * Initialize standard email header. 
        * @since Series 60 3.0
        * @param aMessageData Message containing addressees.
        * @return None. 
        */                                                                                    
        void InitStdHeaderL( const CMessageData& aMessageData );

        /**
        * Initialize MIME email header. 
        * @since Series 60 3.0
        * @param aMessageData Message containing MIME type.
        * @return None. 
        */                                                                                    
        void InitMimeHeaderL( CMeetingRequestData* aMeetingRequestData );

        /**
        * Validate email.
        * @since Series 60 3.0
        * @return ETrue, if email is valid. 
        */                                                                                    
        TBool ValidateEmail( );

        /**
        * Send email.
        * @since Series 60 3.0
        * @return None. 
        */
        void SendEmailL( );

        /**
        * Initialize and create MTM client resources.
        * @since Series 60 3.0
        * @param aMessageData 
        * @return None.
        */                                                                       
        void InitResourcesL( CMeetingRequestData* aMeetingRequestData );

        /**
        * Move message to specified folder (usually outbox). 
        * @since Series 60 3.0
        * @param aTarget Id of target folder. 
        * @return None. 
        */                                                                                    
        void MoveMessageEntryL( TMsvId aTarget );

        /**
        * Resolves the client MTM type by using service id.
        * @since Series 60 3.0
        * @param aServiceId Mailbox id
        * @return None. 
        */                                                                                    
        void ResolveClientMTMTypeL( TMsvId aServiceId );
        
        /**
        * Returns a new descriptor where "MAILTO:" prefix 
        * is removed if it existed.
        * @since Series 60 3.0
        * @param aAddress Text string to be parsed.
        * @return TPtrC A new string which has no "MAILTO:" prefix. 
        */
        TPtrC AttendeeAdressWithoutPrefix( const TDesC& aAddress );

    private:    // Data

        // Contains pointers to the data of the services provided by this plugin.
        // Pointed service data objects are owned.
        RPointerArray<CSendingServiceInfo>      iServiceArray;

        CStreamStore*           iStore;     //Owns
        CBaseMtm*               iClientMtm; //Owns
        TMsvId                  iMailServiceId;
        TUid                    iClientMtmType;
    };


#endif      //  CMEETINGREQUESTSENDER_H

// End of File
