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
*     Declares storage class for mail preferences
*
*/


#ifndef MSGMAILPREFERENCES_H
#define MSGMAILPREFERENCES_H

//  INCLUDES
#include <e32base.h>
#include <msvstd.h>
#include <MsgEditor.hrh>

// FORWARD DECLARATIONS
class CMsvSession;
class TMsvEmailEntry;
class CImSmtpSettings;

// CONSTANTS
enum EHeaderStatus
	{
	EHeaderHidden,
	EHeaderVisible,
	EHeaderOnlySave
	};

class TAdditionalHeaderStatus
    {
    public:
    TAdditionalHeaderStatus();
    TAdditionalHeaderStatus(TMsgControlId aControlID);
    TMsgControlId iHeaderValue;
	EHeaderStatus iStatus;
	TBool iHasContent;
    };

// CLASS DECLARATION

/**
*  Storage class for sending related mail settings. 
*  This class is used to store user-configurable information about sending.
*  These include the remote mailbox to be used, whether the message should
*  be sent immediately or upon next connection. 
*/
class CMsgMailPreferences : public CBase
    {
    public: // Enumerations

        /** 
        * Possible mail scheduling values. 
        */
        enum TMsgMailScheduling {
            EMsgMailSchedulingNow,
            EMsgMailSchedulingNextConn
        }; 

    public: // Constructors and destructor
        /**
        * Constructor.
        * Sets default values.
        */
        IMPORT_C CMsgMailPreferences();

    public: // New functions
        /**
        * Retrieves Character set ID for outgoing mails.
        * @return Character set ID.
        */
        TUid SendingCharacterSet() const;
        
        /**
        * Set Sending character set id
        * @param aSendingCharacterSet Character set id
        */
        void SetSendingCharacterSet(TUid aSendingCharacterSet);
        
        /**
        * Retrieves remote mailbox ID.
        * @return Service ID of the remote mailbox.
        */
        TMsvId ServiceId() const;

        /**
        * Retrieves mail scheduling value.
        * @return Mail scheduling value.
        */
        TMsgMailScheduling MessageScheduling() const;

        /**
        * Sets remote mailbox service ID.
        * @param Remote mailbox service ID.
        */
        void SetServiceId(TMsvId aServiceId);

        /**
        * Sets mail scheduling value.
        * @param aScheduling New mail scheduling value.
        */
        void SetMessageScheduling(TMsgMailScheduling aScheduling);

        /**
        * inline
        * Sets encoding supported flag.
        * @param aSupported ETrue if encoding setting is supported.
        */
        void SetEncodingSupport( TBool aSupported );
                
        /**
        * Gets default preferences from account settings.
        * @param aServiceId Remote mailbox service ID from where the settings
        * will be retrieved. If it is KMsvUnknownServiceIndexEntryId, class
        * defaults will be used. 
        * @param aSession Session to Message Server for fetching settings
        */
        IMPORT_C void DefaultAccountSettingsL(const TMsvId aServiceId, 
                                     CMsvSession& aSession);

        /**
        * Exports current preferences.
        * @param aEntry Email service entry where the 
		* current setting will be exported.
        */
        IMPORT_C void ExportSendOptionsL(const TMsvEmailEntry& aEntry);

        /**
        * Imports current preferences.
        * @param aEntry Email service entry from 
		* where the settings will be read.
        */
        IMPORT_C void ImportSendOptionsL(TMsvEmailEntry& aEntry);

        IMPORT_C void SetAdditionalHeaders(TInt aValue);

        IMPORT_C EHeaderStatus GetAdditionalHeaderVisibility( TMsgControlId aHeader);
    
    private: // implementation
        TUid SolveCharacterSet(const CImSmtpSettings& aSmtpSettings ) const;

    private: // Data   
        /// The message scheduling type
        TMsgMailScheduling      iScheduling;       

        /// The service ID of remote mailbox to be used in sending.
        TMsvId                  iServiceId;
		
		/// Additional headers settings
     	TInt iHeaderResults;
	
	    // Character set for outgoing mails
	    TUid iSendingCharacterSet;
	    
	    // Encoding setting supported
	    TBool iEncodingSupported;

    };

#include "MsgMailPreferences.inl" // inline functions

#endif // MSGMAILPREFERENCES_H   
            
// End of File
