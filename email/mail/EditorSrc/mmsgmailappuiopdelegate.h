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
* 			Interface class for delegating tasks from CMsgMailBaseOp subclasses.
*
*/



#ifndef MMSGMAILOPDELEGATE_H
#define MMSGMAILOPDELEGATE_H

#include <msvapi.h>

// FORWARD DECLARATIONS
class CMsgBodyControl;

// CLASS DECLARATION

/**
* Interface class for delegating tasks from CMsgMailBaseOp subclasses.
* This class is mainly needed since there exists tight coupling between
* operations and UI classes that cannot be completely loosen at the moment,
* this class allows us to delegate the difficult operation steps to another
* class (in practice to CMsgMailAppEditorUi). Due to simplicity delegation of
* all subclasses is concentrated to this one interface.
*  
*/
NONSHARABLE_CLASS( MMsgMailAppUiOpDelegate )
    {
    public:  // destructor        
    
    // send message -related:
    
        /**
        * Method for saving a message.
        */
        virtual void DelegateSaveMsgL() = 0;
        
        /**
        * Method for moving message to outbox.
        * @return new message id of the entry
        */
        virtual TMsvId DelegateMoveMsgToOutboxL() = 0;
                
    // launch view -related:

        /**
        * Method for initiating view launch.
        */
        virtual CMsgBodyControl& DelegateInitLaunchL() = 0;

        /**
        * Method for executing view.
        */
        virtual void DelegateExecuteViewL() = 0;
        
        /**
        * Method for finalizing view after it has been launched.
        */
        virtual void DelegateFinalizeLaunchL() = 0;
    };

#endif      // MMSGMAILOPDELEGATE_H

// End of File
