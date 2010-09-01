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
*     This class implements interface to inform that Mce Log engine state
*     has changed.
*
*/




#ifndef __Mce_App_MMceLogEngObserver_H__
#define __Mce_App_MMceLogEngObserver_H__

// CLASS DECLARATION

/**
 * This class implements interface to inform that Mce Log engine state has
 * changed.
 */
class MMceLogEngObserver
    {
    public:
        /**
         * Use this to inform "Delivery Report" view to get new listbox content
         */
        virtual void MceLogEngineListBoxStateHasChangedL() = 0;
    };

#endif // __Mce_App_MMceLogEngObserver_H__

// End of File
