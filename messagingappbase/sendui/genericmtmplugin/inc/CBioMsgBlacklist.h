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
* Description:   Used to check if sending of bio message is supported.
*
*/



#ifndef __CBIOMSGBLACKLIST_H__
#define __CBIOMSGBLACKLIST_H__


//  INCLUDES
#include <ConeResLoader.h>


// CLASS DECLARATION

/**
* Class which can be used to check if sending of bio message is supported. 
* Normally used before calling DisplaySendMenuItemL.
*/
NONSHARABLE_CLASS (CBioMsgBlacklist) : public CBase
    {
    public:
        
        /**
        * Two-phased constructor.
        */   
        static CBioMsgBlacklist* NewL( CCoeEnv* aCoeEnv );

        /**
        * Destructor.
        */
        virtual ~CBioMsgBlacklist();

        /**
        * Can be used to check if sending of bio message is supported. Normally used before
        * calling DisplaySendMenuItemL.
        * @since 2.5
        * @param aBioMessageId: uid of the bio message
        * @return: ETrue if it is ok to send bio message
        */
        TBool CanSendBioMsg( TUid aBioMessageUid );

    private:
        /**
        * Constructor.
        */
        CBioMsgBlacklist(CCoeEnv* aCoeEnv);

        /**
        * Symbian OS constructor.
        */
        void ConstructL();

    private:

        CArrayFix<TInt32>*  iBioMsgBlacklistArray;

        CCoeEnv*            iCoeEnv;
    };


#endif //__CBIOMSGBLACKLIST_H__

// End of file
