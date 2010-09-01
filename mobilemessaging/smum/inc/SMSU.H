/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Sms Mtm Ui.
*
*/



#if !defined(__SMSU_H__)
#define __SMSU_H__

#if !defined(__MTMUIBAS_H__)
#include <mtmuibas.h>                    // CBaseMtmUi
#endif
#if !defined(__SMSCLNT_H__)
#include <barsc.h>                       // RResourceFile
#include <smsclnt.h>                     // CSmsClientMtm
#endif

#include <centralrepository.h>
#include <etelmm.h>
#include <smutset.h>

// FORWARD DECLARATIONS
class CMtmStore;
class TCharFormatMask;
class TCharFormat;
class CPlainText;
class CSmsNumber;
class CTextResolver;
class CSimDlgPluginInterface;
// CONSTANTS
const TInt KTBuf80Length = 80;
// CLASS DECLARATION
/**
*  CSmsMtmUi
*  Inherited from CBaseMtmUi
*/
NONSHARABLE_CLASS( CSmsMtmUi ) : public CBaseMtmUi
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSmsMtmUi* NewL(CBaseMtm& aBaseMtm,CRegisteredMtmDll& aRegisteredMtmDll);
        
        /**
        * Destructor.
        */
        virtual ~CSmsMtmUi();

    public: // Functions from base classes

        /**
        * From CBaseMtmUi
        * Function NOT dependent on the current context
        * Context set to new entry
        */
        virtual CMsvOperation* CreateL(const TMsvEntry& aEntry, CMsvEntry& aParent, TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* OpenL(TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* CloseL(TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* EditL(TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* ViewL(TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* CancelL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);
        
        /**
        * From CBaseMtmUi
        * Selections must be in same folder and all of the correct MTM type 
        * Context may change after calling these functions
        */
        virtual CMsvOperation* OpenL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);
        
        /**
        * From CBaseMtmUi
        * Selections must be in same folder and all of the correct MTM type 
        * Context may change after calling these functions
        */
        virtual CMsvOperation* CloseL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);
        
        /**
        * From CBaseMtmUi
        * Selections must be in same folder and all of the correct MTM type 
        * Context may change after calling these functions
        */
        virtual CMsvOperation* EditL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);  
        
        /**
        * From CBaseMtmUi
        * Selections must be in same folder and all of the correct MTM type 
        * Context may change after calling these functions
        */
        virtual CMsvOperation* ViewL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);

        /**
        * From CBaseMtmUi
        * Deletes entries from the current context, which must be a folder or service of the relevant MTM 
        */
        virtual CMsvOperation* DeleteFromL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
        
        /**
        * From CBaseMtmUi
        * Deletes service, which need not be the current context
        */
        virtual CMsvOperation* DeleteServiceL(const TMsvEntry& aService, TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        * Takes ownership of the passed entry (which the context is switched to )
        */
        virtual CMsvOperation* ReplyL(TMsvId aDestination, TMsvPartList aPartlist, TRequestStatus& aCompletionStatus);
        
        /**
        * From CBaseMtmUi
        * Context-specific
        * Takes ownership of the passed entry (which the context is switched to )
        */
        virtual CMsvOperation* ForwardL(TMsvId aDestination, TMsvPartList aPartList, TRequestStatus& aCompletionStatus);

        /**
        * From CBaseMtmUi
        * Context should be MTM folder/service to copy to
        */
        virtual CMsvOperation* CopyToL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context should be MTM folder/service to move to
        */
        virtual CMsvOperation* MoveToL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
        
        /**
        * From CBaseMtmUi
        * Context should be MTM folder/service to copy from
        */
        virtual CMsvOperation* CopyFromL(const CMsvEntrySelection& aSelection, TMsvId aTargetId, TRequestStatus& aStatus);
        
        /**
        * From CBaseMtmUi
        * Context should be MTM folder/service to move from
        */
        virtual CMsvOperation* MoveFromL(const CMsvEntrySelection& aSelection, TMsvId aTargetId, TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Interpret transfer progress 
        */
        virtual TInt DisplayProgressSummary(const TDesC8& aProgress) const;

        /**
        * From CBaseMtmUi
        */
        virtual TInt QueryCapability(TUid aCapability, TInt& aResponse);

        /**
        * From CBaseMtmUi
        */
        virtual void InvokeSyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TDes8& aParameter);

        /**
        * From CBaseMtmUi
        */
        virtual CMsvOperation* InvokeAsyncFunctionL(TInt aFunctionId, const CMsvEntrySelection& aSelection, TRequestStatus& aCompletionStatus, TDes8& aParameter);

     public: // new functions

        /**
        * Asyncronous function for displaying message info
        * @param aCompletionStatus
        * @param aParameter
        * @return pointer to started operation
        */
        virtual CMsvOperation* ShowMessageInfoL(TRequestStatus& aCompletionStatus, TDes8& aParameter) const;
        
        /**
        * Syncronous function for displaying message info
        * @return
        */
        virtual void ShowMessageInfoL() const;
        
        /**
        * Enumerations for different editor types
        * (used below)
        */
        enum TEditorType
         {
         EReadOnly,
         EEditExisting,
         ECreateNewMessage
         };

        /**
        * Returns a pointer to current textbody
        * @param aStatus
        * @param aSession 
        * @param aEditorType
        * @param aParentId
        * @return pointer to started operation
        */
        virtual CMsvOperation* LaunchEditorApplicationL(
            TRequestStatus& aStatus, 
            CMsvSession& aSession, 
            TEditorType aEditorType = EEditExisting, 
            TMsvId aParentId = KMsvDraftEntryIdValue );
       
    protected: // Functions from base classes
    
        /**
        * from CBaseMtmUi
        */
        virtual void GetResourceFileName( TFileName& aFileName ) const;

        /**
        * Performs CopyToL or MoveToL operation
        * @param aSelection the entries to be sent
        * @param aStatus follows the operation progress
        * @param aCopyOnly defines is it copy or move
        * @return pointer to started operation
        */
        virtual CMsvOperation* CopyMoveToL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus, TBool aCopyOnly);
        
    protected: // New functions

        /**
        * Ensures that the TMsvEntry is a suitable candidate for treatment by
        * this MtmUi layer.
        * @param aEntry as to be checked entry
        * @return boolean-answer
        */
        virtual TBool CheckEntry( const TMsvEntry& aEntry ) const;

        /**
        * Launch the service settings dialog on the current entry
        * @param aStatus follows the operation progress
        * @return pointer to started operation
        */
        virtual CMsvOperation* LaunchSettingsDialogL( TRequestStatus& aStatus );

        /**
        * Creates Sms Service
        * @param aNoServiceNoCheckNeeded is check needed or not
        * @return id of service
        */
        virtual TMsvId CreateSmsServiceL( TBool aNoServiceNoCheckNeeded );

        /**
        * Returns service's id
        * @return id of service
        */
        virtual TMsvId ServiceIdL();
        
        /**
        * Actual Sms service default creator 
        * @param aServiceId of service
        */
        virtual void CreateDefaultSettingsL( TMsvId aServiceId, TBool aMailInit = EFalse );

        /**
        * Reads default / variated settings from shared data 
        * @param aServiceSettings pointer to service
        */
        virtual void ReadDefaultSettingsFromSharedDataL( CSmsSettings* aServiceSettings );

        /**
        * Copies service centres from old to new settings
        * @param aServiceSettings pointer to service
        */
        virtual void CopySCentresFromOldToNewSettingsL( CSmsSettings* aServiceSettings );

        /**
        * Take the name and number fields from the recipients array and concaternate them
        * together, putting <> around the numbers and separating recipients with a comma
        * @return pointer to descriptor
        */
        virtual HBufC* TurnRecipientsArrayIntoStringLC( 
            const CArrayPtrFlat<CSmsNumber>& aRecip, 
            const TBool aEmailMessage = EFalse ) const;

        /**
        * Extracts title from GMS body NOTE : IS THIS STILL NEEDED?
        * Extracts up to aMaxLength of the text from a picture message.
        * the format of the body will be blocks of the following:
        * <type> <length> <contents> 
        * where type is a code indicating text or picture.
        * this code is based on functions in CGmsModel from gmsbc which 
        * unfortunately aren't exported, so I've had to copy bits.
        * @param aBody textbody descriptor
        * @param aMaxLength tells how long description is wanted
        * @return pointer to descriptor
        */
        virtual HBufC* ExtractTitleFromGMSBodyL( const CRichText& aBody, TInt aMaxLength ) const;

        /**
        * Returns a pointer to Sms Client Mtm
        * @return reference to Sms Client Mtm
        */
        inline CSmsClientMtm& SmsClientMtm() const { return STATIC_CAST( CSmsClientMtm&, BaseMtm());};

        /**
        * Function to set shared data variables up to date for other apps to observe for example 
        * creation of sms service
        * @param afile shared data file which is modified
        * @param aKey key in file which is modified
        * @param aValue new value for the key
        */
        virtual void SetSharedDataL( const TUint32 aId, const TInt aVal );
        /**
        * Enums for return values of CheckAndEnsureSmsServiceOkL
        */
        enum TSmumSmsReCreation
            {
            ESmsReCreatedNone = 0,
            ESmsReCreatedService,
            ESmsReCreatedServicesStore,
            ESmsReCreatedBoth
            };
                
        /**
        * Checks do we have sms service and store for it, if not creates (if wanted)
        * one with default settings
        * @param aCreateService if missing create one
        * @param aCreateStore if missing create one
        * @param aFetchSMSCs if something recreated fetch SMSCs from SIM
        * @return TInt enum TSmumSmsReCreation tells what was done 
        */
        virtual TInt CheckAndEnsureSmsServiceOkL( 
            TBool aCreateService, 
            TBool aCreateStore, 
            TBool aFetchSMSCs );

        /**
        * Reads SMSCs from SIM. A bit slow operation.
        * @return TInt errorcode 
        */
        virtual TInt ReadSMSCsFromSIML();
        
        /**
        * Checks the SIM status
        * @return TInt, value of sim state
        */
        virtual TInt CheckSimStateL();
        
        /**
        * Extracts all the required data from the mtm, populates a message info array
        * and passes this array to the message info dialog class which displays the
        * dialog.
        * This is used for sms's and also created bio messages (which use sms as the
        * bearer).
        * @return
        */
        virtual void ConstructAndShowInfoDialogL() const;
        
        /**
        * Extracts subject text from message body text
        * @since Series 60 2.8
        * @param aTEntry, entry which to determine the right message type
        * @param aSmsClientMtm, mtm from which the subject is extract
        * @param aSubject buffer to hold the extracted subject text
        * @param aIsEmailMessage True if message TP-PID is set to interworking 
        * with email
        */
        virtual void GetSubjectTextL( 
            const TMsvEntry& aTEntry, 
            const CSmsClientMtm& aSmsClientMtm,
            HBufC*& aSubject,
            TBool aIsEmailMessage = EFalse ) const;

        /**
        * Returns the resource id according to TMsvEntry type
        * @since Series 60 2.8
        * @param aTEntry, entry which is used to find the correct resource id
        * @return, resource id for TMsvEntry type
        */
        virtual TInt GetBioResourceId( const TMsvEntry& aTEntry ) const;
        
        /**
        * Checks if the message bio type is supported
        * @since Series 60 2.8
        * @param aTEntry, entry which is used to check if type is supported or not
        * @return, ETrue if supported otherwise EFalse
        */
        virtual TBool IsBioTypeSupported( const TMsvEntry& aTEntry ) const;
       
    protected:

        /**
        * By default Symbian OS constructor is private.
        */        
        virtual void ConstructL();

        /**
        * Constructor (accessible only through factory function)
        */
        CSmsMtmUi(CBaseMtm& aBaseMtm,CRegisteredMtmDll& aRegisteredMtmDll);

    protected: // Data 
        TBool               iNewSmsSettings;
        CTextResolver*      iErrorResolver;
        // Central Repository session
        CRepository*        iCenRepSession;
        CSimDlgPluginInterface* iSimDlgPluginIntf;
    	RTelServer          iServer;
	    RMobilePhone        iPhone;  
  };





// CLASS DECLARATION
/**
*  TSmsSmumProgress
*/

NONSHARABLE_CLASS( TSmsSmumProgress )// : public TSmsProgress
    {
    public :
    
    	/**
        * Enumerations for different progress types
        * (used below)
        */
        enum TSmsSmumProgressType
            {
            ESmsSmumFirstProgType = TSmsProgress::ESmsProgressTypeDefault+200,
            ESmsSmumSmsOpening,
            ESmsSmumLastProgType
            };
            
    public : //data
        TInt iError;
        TSmsSmumProgressType iType;
    };
    
typedef TPckgBuf<TSmsSmumProgress> TSmsSmumProgressBuf;  

#endif // __SMSU_H__



//  End of File
