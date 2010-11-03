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
* Description:  Static helper methods related to CMailMessage.
*
*/


#ifndef MAILMESSAGEUTILS_H
#define MAILMESSAGEUTILS_H

#include <e32std.h>

// FORWARD DECLARATIONS
class CMailMessage;

/**
* Class offers static helper methods related to CMailMessage.
* This class contains utility methods that are not CMailMessage's core
* functionality and which we want to keep internal to Mail subsystem.
*/
class MailMessageUtils
    {
    public:
    
        /**
        * Method for checking wheather a message is outgoing or incoming.
        * @param aMessage message to evaluate
        * @return ETrue if message is outgoing
        */
        IMPORT_C static TBool IsOutgoingL( CMailMessage& aMessage );
	
    };

#endif // MAILMESSAGEUTILS_H

// End of file
