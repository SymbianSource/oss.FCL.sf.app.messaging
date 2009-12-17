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
* Description:  MuiuEmailTools  declaration
*
*/



#ifndef CIMASETTINGSUTILS_H
#define CIMASETTINGSUTILS_H

// INCLUDES
#include <e32base.h>
#include <e32cmn.h>             
#include <msvstd.h>                 // TEntry        
#include <mtmuibas.h>               // CBaseMtmUi
#include <cemailaccounts.h>         // CEmailAccounts
#include <SendUiConsts.h>           // Mtm's

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  This class contains simple tools for mailbox handling
*  @lib muiu
*  @since S60 3.0
*/
class MuiuEmailTools
    {
    public:  // Constructors and destructor
    public: // New functions             
                        
        /**
        * Checks the current default mailbox
        * @since S60 3.0
        * @param aMsvSession Message server session
        * @param aMtmId 
        *   Mail Mtm Id to be used for searching default mailbox. If Pop3 or
        *   Imap4 mtm is offered, the value of aMtmId is changed to 
        *   corresponding Smtp mtm id.
        * @return KMsvUnknownServiceIndexEntryId if no default mailbox
        * @return Default mailbox id
        */
        IMPORT_C static TMsvId DefaultSendingMailboxL( 
            CMsvSession& aMsvSession,            
            TUid& aMtmId );               
                            
        /**
        * Sets the default sending mailbox
        * @since S60 3.0
        * @param aSendingMailboxId 
        */
        IMPORT_C static void SetDefaultSendingMailboxL(
            const TMsvId aSendingMailboxId );                
            
        /**
        * Searches the correct entry of specified protocol for required mailbox
        * @since S60 3.0
        * @param aMsvSession Session to be used in search
        * @param aEntry Entry for result to be stored
        * @param aMailboxId Id of the required mailbox
        * @param aGetSendingService Get either receiving or sending type
        * @return TInt KErrNone if search is successful
        */
        IMPORT_C static TInt GetMailboxServiceId(
            CMsvSession& aMsvSession,
            TMsvEntry& aEntry,
            const TMsvId aMailboxId,            
            const TBool aGetSendingService );              
            
        /**
        * Finds the first valid mailbox with spesific mtm        
        * @since S60 3.0
        * @param aMsvSession Message server session
        * @param a
        */
        static TMsvId FindFirstValidMailboxL( 
            CMsvSession& aMsvSession,
            const CMsvEntrySelection& aSelection,
            const TUid& aMtmId );            
        
        /**
        * Checks if given mtm-type matches known mail mtm's 
        * @since S60 3.0
        * @param aMtm Mtm to be tested
        * @param aAllowExtended Other than basic mail mtm's are accepted also
        * @return EFalse if the mtm is not recognized
        * @return ETrue if the mtm is supported type (Imap4 / Pop3 / Smtp )
        */
       IMPORT_C static TBool IsMailMtm( 
            const TUid& aMtm,
            const TBool aAllowExtended = EFalse );          
    
        /**
        * Verifies that the mailbox can be used in the current phone.
        * @since S60 3.0
        * @return ETrue/EFalse, Based on the validity of the mailbox
        */
        static TBool ValidateMailboxL( 
            CMsvSession& aMsvSession,
            const TUid& aMtmId,
            const TMsvId aMailboxId );

        /**
        * Simplifies the call of MtmUi's QueryCapability.
        * @since S60 3.0
        * @param aQuery Capability to be queried
        * @param aMtmUi MtmUi to be used for the query
        * @return ETrue, when no error and capability exist        
        * @return EFalse, when the capability is rejected or error has occurred
        */
        static TBool QueryCapability( 
            const TInt aQuery, 
            CBaseMtmUi& aMtmUi ); 
            
        /**
        * Queries default service from specific MTM
        * @since S60 3.0
        * @param aMtmId Mtm type to be queried
        * @return Id of the default service
        */
        static TMsvId QueryDefaultServiceL( 
            CMsvSession& aMsvSession, 
            const TUid& aMtmId );          
    
    public: // Functions from base classes

    protected:  // Constructors
    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions
    
        /**
        * Simplifies the MtmUi's query Mtm
        * @since S60 3.0
        * @param aMailboxId, Mailbox id to be checked
        * @param aMtmUi, MtmUi to be used for the query      
        * @return The result of the query
        */
        static TInt QueryMailboxValidityL( 
            const TMsvId aMailboxId,
            CBaseMtmUi& aMtmUi );            
            
    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data
    };

#endif //  CIMASETTINGSUTILS_H

// End of File