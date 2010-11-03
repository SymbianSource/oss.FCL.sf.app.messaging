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
*     Item class for mail accounts. 
*
*/



#ifndef MCEMAILACCOUNTITEM_H
#define MCEMAILACCOUNTITEM_H


// CLASS DECLARATION

/**
*  Item class for remote folders; encapsulates folder id and 
*  the information, if remote connection has been started in mce, not by always
*  online
*/
class TMceMailAccountItem
    {
    public:                     // Data        
        TMsvId                  iServiceId;
        TBool                   iMceConnection;
        TMsvOp                  iOp;
        
    };


#endif // MCEMAILACCOUNTITEM_H

// End of file
