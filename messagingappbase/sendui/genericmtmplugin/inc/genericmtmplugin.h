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
* Description:   This class offers single ECOM interface to all SendUi MTM plugins.
*
*/




#ifndef C_GENERICMTMPLUGIN_H
#define C_GENERICMTMPLUGIN_H


#include <ConeResLoader.h>
#include <msvapi.h>
#include <txtmrtsr.h>
#include <cmsvrecipientlist.h>
#include <TSendingCapabilities.h>

#include <CMessageAddress.h>
#include <CSendingService.h>

class CSendingServiceInfo;
class CMessageData;
class CClientMtmRegistry;
class TSendingCapabilities;
class CAknContextPane;
class CAknWaitDialog;
class CBaseMtmUi;
class CBioMsgBlacklist;
class CBaseMtmUiData;
class CMtmUiDataRegistry;
class CMtmUiRegistry;
class CSendUiAttachment;
class CSendUiSingleton;

/**
 *  Offers single ECOM interface to all SendUi MTM plugins.
 *
 *  @lib genericmtmplugin
 *  @since S60 S60 v3.2
 */
NONSHARABLE_CLASS( CGenericMtmPlugin ) : public CSendingService,
                                         public MRichTextStoreResolver
    {

public:

    static CGenericMtmPlugin* NewL( TSendingServiceParams* aParams );

    virtual ~CGenericMtmPlugin();

    /**
     * Initializes sending.
     *
     * @since S60 v.3.2
     * @return None.
     */
    TBool InitializeSendingL();

// from base class CSendingService

    /**
    * From CSendingService
    * Populates given list with the provided services.
    *
    * @since Series 60 v3.2
    * @param aList Service list to be populated. 
    * @return None
    */
    void PopulateServicesListL( RPointerArray<CSendingServiceInfo>& aList );

    /**
    * From CSendingService
    * Creates and sends message via selected MTM
    *
    * @since Series 60 v3.2
    * @param aServiceUid Uid of the sending service.
    * @param aMessageData Data for the message.
    * @param aLaunchEditorEmbedded True, if editor is to be launched embedded
    */
    void CreateAndSendMessageL(
        TUid                            aMtmUid,
        const CMessageData*             aMessageData,
        TBool                           aLaunchEditorEmbedded );
        
    /**
    * From CSendingService
    * Returns the id of the service provider.
    *
    * @since Series 60 3.0
    * @return Id of the service provider.
    */
    TUid ServiceProviderId() const;

    
private:

    CGenericMtmPlugin( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton );

    void ConstructL( );
    
// from base class MRichTextResolver
        
   /**
    * Returns pointer to stream store
    *
    * @since Series 60 v3.2
    * @param Not used.
    */
    const CStreamStore& StreamStoreL(TInt) const;


// New functions
private: 

    /**
    * Start syncronous MTM scanning process
    *
    * @since Series 60 3.0
    */  
    void ScanMtmsL( RPointerArray<CSendingServiceInfo>& aList );

    
   /**
    * Validates attachments
    *
    * Checks relevant validity information depended on the file type and
    * sending service. Possible checks are file integrity, MMS and DRM 
    * validity checks
    *
    * @since Series 60 2.8
    * @param aMtmUid Chosen sending service
    * @param aMessageData Message data
    * @param aValidAttachmentPaths Array of file paths
    * @param aValidAttachments Array of open file handles
    * @return ETrue, if all attachments are valid or user has accepted sending
    *         possibly corrupted files or files that receiver cannot open.
    */
    TBool ValidateAttachmentsL( 
	    TUid                    aMtmUid,
	    CBaseMtmUiData&         aMtmUiData,
	    CArrayPtrFlat<CSendUiAttachment>* aAttachments );
        
    /**
    * Determines if a wait note is needed
    *
    * If there are attachments or body text, then wait note is required.
    *
    * @since Series 60 2.8
    * @param aMtmUid Chosen sending service
    * @param aMessageData Message data
    * @param aAttachmentCount Number of attachments including file paths and file handles
    * @return ETrue, if note required. Otherwise EFalse.
    */
    TBool IsWaitNoteNeededL(
        TUid                    aMtmUid ,
        const CMessageData&     aMessageData,
        TInt                    aAttachmentCount );

    /**
    * Shows wait note
    *
    * @since Series 60 2.8
    */
    void ShowWaitNoteLC();
    
    /**
    * Resets sending state of SendUi by cleaning the cleanupitem
    *
    * @since Series 60 2.8
    * @param aAny Sending state
    * @return EFalse Sending state
    */
    static void ResetIsSending(TAny* aAny);

    /**
    * Destroys wait dialog.
    *
    * Called from CleanupStack.
    *
    * @since Series 60 2.8
    * @param aAny CAknWaitDialog pointer
    */
    static void CleanupWaitDialog(TAny* aAny);
    void CreateMessageL( TUid&    aMtmUid, 
        		         TMsvId  aService );
    
    /**
    * Creates message
    *
    * Validates the given MTM and creates a message to the MTM
    *
    * @since Series 60 2.8
    * @param aMtmUid Chosen sending service
    * @param aLaunchEmbedded ETrue, if editor is to be launched embedded
    */
    TBool CreateMessageL(    
        TUid&    aMtmUid,        
        TBool&   aLaunchEmbedded );
    
    /**
    * Sets recipients
    *
    * @since Series 60 2.8
    * @param aMtmUid Selected MTM
    * @param aMessageData Message data
    */  
    void SetRecipientsL(
        TUid                    aMtmUid,
        const CMessageData&     aMessageData );
        
    /**
    * Adds addressees
    *
    * @since Series 60 2.8
    * @param aRecipientType Type of message: To,Cc,Bcc
    * @param aAddressees Address data
    */
     void AddAddresseesL(
        TMsvRecipientType               aRecipientType,
        const CMessageAddressArray&     aAddressees );
    
    /**
    * Inserts multiple attachments to sending MTM
    *
    * @since Series 60 2.8
    * @param aMtmUid Chosen sending service
    * @param aAttaFilePaths Attachment path array
    * @param aAttaFileHandles Attachment file handle array
    * @return EFalse, if attachment addition was not successful. Normally ETrue.
    */  
    TBool AddAttachmentsL(
        CBaseMtmUiData& aMtmUiData,
        CArrayPtrFlat<CSendUiAttachment>* aAttachments );

   /**
    * Inserts one attachment to sending MTM
    *
    * @since Series 60 2.8
    * @param aAttachment Attachment file path
    * @return EFalse, if attachment addition was not successful. Normally ETrue.
    */      
   TBool AddAttachmentL( CSendUiAttachment* aAttachment, TInt aSupportsLinks  );
   
   /**
    * Sets body text of the message to MTM
    *
    * @since Series 60 2.8
    * @param aMessageData Message data containing body text
    */  
    void SetBodyL( 
        const CBaseMtmUiData& aMtmUiData, 
        const CMessageData& aMessageData,
        CArrayPtrFlat<CSendUiAttachment>* aAttachments );
        
   /**
    * Launches editor
    *
    * @since Series 60 v3.2
    * @param aTempFileName Name of a temporary file
    */  
    void LaunchEditorL(
        TInt aAttachmentCount,
        TBool aLaunchEditorEmbedded );
    
   /**
    * Returns default Email service
    * 
    * Leaves without error note, if email service cannot be found even after running email 
    * setup wizard.
    *
    * @param aServiceId Default Email service
    * @since Series 60 v3.2
    */
    TMsvId EmailDefaultServiceL();

   /**
    * Removes illegal characters.
    * 
    * Removes < and > characters.
    *
    * @since Series 60 v2.8
    */  
    void RemoveIllegalChars( TPtr& aCheckedString);
    
    /**
    * Check for feature support
    *
    * Relevant feature support flags are temporary saved from FeatureManager
    * during object construction
    * 
    * @since Series 60 v2.8
    */  
    TBool IsSupported( const TUint& aFeature );
 
   /**
    * Check for ongoing call
    *
    * @since Series 60 v2.8
    * @param aPhoneNumberString Returns the number of the caller
    * @param aName Returns the name of the caller
    * @return ETrue, if call is going
    */
    TBool IsCallOngoing(
        TDes& aPhoneNumberString,
        TDes& aName );

   /**
    * Sends files over Bluetooth
    *
    * @since Series 60 v2.8
    * @param aAttaFilePaths Filepaths to be send
    * @param aAttaFileHandles Filehandles to be send
    */
    void SendBtMessageL( CArrayPtrFlat<CSendUiAttachment>* aAttachments );
        
    /**
    * Calls MTM UI to validate service function  
    *
    * @since Series 60 v3.1
    * @return ETrue, if service is valid
    */  
     TBool IsValidServiceL( );
    
    /**
    * Validates MTM
    *
    * Checks if service supports validation and validates the service if it does. 
    *
    * @since Series 60 v3.2
    * @param aMtmUid Service identification
    * @return ETrue, if service is valid or does not support validation.
    */ 
     TBool ValidateMtmL( );

    /**
    * Retrieve and validate a service
    *
    * @since Series 60 v3.2
    * @param aMtmUid Service identification
    * @return TMsvId Id of a found service
              KMsvUnknownServiceIndexEntryId if no valid service found.
    */ 
     TMsvId GetAndValidateServiceL(
         TUid& aMtmUid, 
        TBool aLaunchEmbedded,
        TBool& aContinue );
    
    /**
    * Validates an account
    *
    * @since Series 60 v3.2
    * @param aMtmUiData Reference to Mtm ui data
    * @param aMtmUid Service identification
    */  
     TBool ValidateAccountL( CBaseMtmUiData& aMtmUiData, TUid aMtmUid );
     
     void LoadMtmL( 
         TUid aMtmType, 
         RPointerArray<CSendingServiceInfo>& aList );
     
     void GetMtmSendingCapabilitiesL( CBaseMtmUiData& mtmUiData, TSendingCapabilities& cap );

     static TBool IsPostcardContactLink( const CArrayPtrFlat<CSendUiAttachment>* aAttachments);

private: // data

    /**
     * Stores locally required feature flags from FeatureManager. 
     */
    TUint iSendUiFeatureFlags;

    /**
     * Contains pointers to the data of the services provided by this plugin.
     * Pointed service data objects are owned.
     */
    RPointerArray<CSendingServiceInfo>      iServiceArray;

    /**
     * Temporary stream store
     * Own.
     */
    CStreamStore* iStore;
        
    /**
     * Selected MTM sending service
     * Own.
     */    
     CBaseMtm* iClientMtm;
     
     /**
     * Selected MTM sending service UI
     * Own.
     */    
     CBaseMtmUi* iMtmUi;
        
    /**
     * Waitdialog
     *
     * Note. This is destroyed using CleanupStack.
     * Own.
     */
     CAknWaitDialog* iWaitDialog;
    
    /**
     * The required capabilities for the sending service
     */        
     TSendingCapabilities iRequiredCapabilities;
    
    /**
     * Index entry at the Message Server 
     */    
    TMsvEntry iIndexEntry;
    
    /**
     * Sending state
     */    
    TBool iIsSending;

    };

#endif // C_GENERICMTMPLUGIN_H
