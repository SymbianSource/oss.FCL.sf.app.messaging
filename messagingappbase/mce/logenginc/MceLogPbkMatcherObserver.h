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
*     This class defines interface to inform, that matching with the phonebook
*       is completed
*
*/



#ifndef __Logs_Engine_MMceLogPbkMatcherObserver_H__
#define __Logs_Engine_MMceLogPbkMatcherObserver_H__

// CLASS DECLARATION

/**
 * This class defines interface to inform, that matching with the phonebook
 * is completed
 */
class MMceLogPbkMatcherObserver
    {
    public:
        virtual void PbkMatchingDoneL() = 0;
    };

#endif // __Logs_Engine_MMceLogPbkMatcherObserver_H__

// End of file
