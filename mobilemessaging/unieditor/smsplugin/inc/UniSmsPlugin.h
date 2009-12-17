/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*       UniEditor SMS Plugin definition.
*
*/



#ifndef __UNISMSPLUGIN_H
#define __UNISMSPLUGIN_H

// INCLUDES
#include "UniPluginApi.h"

// DATA TYPES

// FORWARD DECLARATIONS

//class CSmsSettings;
//class CSmsNumber;
//class CSmsMessage;
class CClientMtmRegistry;
class CSmsClientMtm;
class CSmsHeader;
class CParaFormatLayer;
class CCharFormatLayer;
class CRichText;
class CMsgSmsEmailOverSmsFunc;
class CSmsNumber;
class CSmsMessage;

// CLASS DECLARATION

/**
* CUniSmsPlugin
*/ 
class CUniSmsPlugin : public CUniEditorPlugin
	{
	public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CUniSmsPlugin* NewL( TAny* aConstructionParameters );
 		
        /**
        * Destructor.
        */
        virtual ~CUniSmsPlugin();

    public: // From CUniEditorPlugin

        void LoadHeadersL( CMsvStore* aStore );

        void SaveHeadersL( CMsvStore& aStore );

        TMsvId ConvertFromL( TMsvId aId );

        TMsvId ConvertToL( TMsvId aId );
        
        TMsvId CreateReplyL( TMsvId aSrc, TMsvId aDest, TMsvPartList aParts );

        TMsvId CreateForwardL( TMsvId aSrc, TMsvId aDest, TMsvPartList aParts );
        
        void SendL( TMsvId aId );

        TBool ValidateServiceL( TBool aEmailOverSms = EFalse );

        void GetSendingSettingsL( TUniSendingSettings& aSettings );

        void SetSendingSettingsL( TUniSendingSettings& aSettings );
        
        TBool IsServiceValidL();
        
        inline void SetUnicodeMode( TBool aUnicodeMode );
        
        //Turkish SMS(PREQ2265) specific...
        void SetEncodingSettings(TBool aUnicodeMode, TSmsEncoding aAlternativeEncodingType, TInt charSupportType);
       
        void GetNumPDUsL(
               TDesC& aBuf,
               TInt& aNumOfRemainingChars,
               TInt& aNumOfPDUs,
               TBool& aUnicodeMode, 
               TSmsEncoding& aAlternativeEncodingType);

	private: // Constructors
	
	    /**
        * C++ default constructor.
        */
        CUniSmsPlugin( CMsvSession& aSession, CUniClientMtm& aUniMtm );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();
		
		/**
		* Returns pointer to iSmsMtm member variable
		*/
		CSmsClientMtm* SmsMtmL();

        /**
        * Creates reply or forward message
        */
        TMsvId DoCreateReplyOrForwardL( 
            TBool aReply, 
            TMsvId aSrc, 
            TMsvId aDest, 
            TMsvPartList aParts );

        /**
        * Moves message into outbox for sending and creates copies of SMS messages
        * into outbox in case of multiple recipients
        */
        void MoveMessagesToOutboxL();

        /**
        * Called in MoveMessagesToOutboxL, calls the actual moving
        * function
        */
        TMsvId MoveMessageEntryL( TMsvId aTarget );

        
        /**
        * Creates messages into the Outbox if there are several recipients
        * @param TMsvEntry& a reference to entry to be copied
        * @param CSmsNumber& a reference to CSmsNumber to be copied
        * @param CRichText& a reference to msg body contents
        * @return TMsvId of the created msg entry
        */
        TMsvId CreateMessageInOutboxL(
            const TMsvEntry& aEntry, 
            const CSmsNumber& aRecipient, 
            const CRichText& aBody );
            
        /**
        * Creates messages into the Outbox if there are several recipients
        * and the message is E-Mail over SMS message
        * @param aEntry a reference to entry to be copied
        * @param aAddress a reference to address to be copied
        * @return TMsvId of the created msg entry
        */
        virtual TMsvId CreateMessageInOutboxL(
            const TMsvEntry& aEntry, 
            const TDesC& aAddress );    
        

        /**
        * Calls the actual sending method of SMS Client MTM
        */
        void SetScheduledSendingStateL( CMsvEntrySelection* aSelection );
        
        /**
        * Extracts name and address
        */
        void NameAndAddress( const TDesC& aMsvAddress, TPtrC& aName, TPtrC& aAddress );

        /**
        * Creates text for TMsvEntry::iDescription. It could be subject or
        * if subject does not exist a beginning of the message body 
        */
        void ExtractDescriptionFromMessageL(
            const CSmsMessage& aMessage, 
            TDes& aDescription, 
            TInt aMaxLength);
            
        /**
        * Copies text from attachment into the body of the SMS
        */
        void CreatePlainTextSMSL( RFile& aFile );

        /** 
        * Insert subject into the body of the SMS
        */
        void InsertSubjectL( CSmsHeader& aHeader, CRichText& aText );
        
        /**
        * Copies data from VCard attachment into body of the SMS
        */
        void CreateVCardSMSL( RFile& aFile );
        
        /**
        * Copies data from VCCalendar attachment into body of the SMS
        */
        void CreateVCalSMSL( RFile& aFile );
                
        /**
        * Checks service center address 
        */
        TBool ValidateSCNumberL();
        
        /**
        * Checks service center address for E-Mail over SMS messages
        */
        TBool ValidateSCNumberForEmailOverSmsL();
        
        /**
        * Executes service setting query dialog
        */
        TBool SmsScDialogL();
        
        /**
        * Saves E-Mail specific information in header
        */
        void FillEmailInformationDataL( CSmsHeader& aHeader,  const TPtrC& aAddress );
    
        /**
        * Checks if the address is E-Mail address
        */
        TBool IsEmailAddress( const TPtrC& aAddress ) const;
            
        /**
        * Converts sms message to uni message format.
        */
		TMsvId DoConvertFromL( TMsvId aId, TBool aIsForward );
        

    private:  // Data    

        CMsvSession& iSession;
        CUniClientMtm& iUniMtm;
        
        CClientMtmRegistry* iMtmRegistry;
        CSmsClientMtm* iSmsMtm;
        
        TMsvId iSmsServiceId;
        CSmsHeader* iSmsHeader;
        
        CParaFormatLayer* iParaFormatLayer;
        CCharFormatLayer* iCharFormatLayer;
        CRichText* iRichText;
        
        CDesCArray* iRecipients;
              
        TFileName iAttachmentFileName;    	 
       	 
       	TBool iBioMsg;
        
        enum TUniPluginPanic
            {
            EIllegalArguments  
            }; 
    
        // EmailOverSms specific ...
		TInt                    iLongestEmailAddrLength ;
		TUint32                 iMsgLengthFlags;
		TBool                   iFullyConstructed;
		
        CSmsNumber*				iEmailOverSmsC;
        TBool                   iUnicodeMode;
        TBool                   iOfflineSupported; //is offline feature supported
        
        //Turkish SMS(PREQ2265) specific...
        TInt iCharSupportType;
        TSmsEncoding  iAlternativeEncodingType;
        TBool iNLTFeatureSupport;
    };

inline void CUniSmsPlugin::SetUnicodeMode( TBool aUnicodeMode )
    {
    iUnicodeMode = aUnicodeMode;
    }

#endif      // __UNISMSPLUGIN_H

// End of File
