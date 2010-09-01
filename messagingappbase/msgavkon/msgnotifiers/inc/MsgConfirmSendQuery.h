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
*    CMsgConfirmSendQuery declaration file 
*
*/



#ifndef __MSGCONFIRMSENDQUERY_H
#define __MSGCONFIRMSENDQUERY_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CMsgConfirmSendNotifier;

// CLASS DECLARATION

/**
* CMsgConfirmSendQuery
*
* @lib MsgNotifiers.dll
* @since 2.0
*/
NONSHARABLE_CLASS( CMsgConfirmSendQuery ) : public CActive
    {
    public:     // construct / destruct

        /**
        * Two-phased constructor.
        *
        * @param aWatcher A reference pointer to CMsgConfirmSendWatcher
        */
        static CMsgConfirmSendQuery* NewL( CMsgConfirmSendNotifier& aNotifier );

        /**
        * Destructor.
        */
        virtual ~CMsgConfirmSendQuery();

        /**
        * Shows the global msg query
        *
        * @param aPrompt
        */
        void ShowQueryL( const TDesC8& aBuffer, TBool aSendAs );

    private:
    
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Constructor.
        *
        * @param aWatcher A reference pointer to CMsgConfirmSendNotifier
        */
        CMsgConfirmSendQuery( CMsgConfirmSendNotifier& aNotifier );

        /**
        * From CActive
        */
        void RunL() ;

        /**
        * From CActive
        */
        void DoCancel();
        
    private:
    
        void ResolveSendAsConfirmDetailsL( const TDesC8& aBuffer );
        void ResolveSendUiConfirmDetailsL( const TDesC8& aBuffer );
        void NameFromSecurityInfoL( TSecurityInfo& securityInfo );
        void NameFromMtmL( CMsvSession* aSession, TUid aMtmTypeUid );
        HBufC* CreateMessageTextLC();

    private:    // data

        CMsgConfirmSendNotifier& iNotifier;

    	CEikonEnv*      iCoe;
    	TInt            iResourceFileOffset;
    	
    	TSecurityInfo iSecurityInfo;
    	CMsvEntrySelection* iSelection;
        
        TBool iMultipleMessages;
        TBool iMultipleRecipients;
        HBufC* iAppName;
        HBufC* iMtmName;
        
    };

#endif // __MSGCONFIRMSENDQUERY_H

// End of File
