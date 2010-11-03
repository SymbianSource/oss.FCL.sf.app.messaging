/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Message Editor interface class for asynchronous controls.
*
*/



#ifndef MMSGASYNCCONTROL_H
#define MMSGASYNCCONTROL_H

// INCLUDES
#include <e32base.h>
#include <msgasynccontrolobserver.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RFile;

// DATA TYPES

// CLASS DECLARATION

// ==========================================================

/**
* Message Editor Base common interface class for asynchronous controls
*
* @since 3.2
* @lib MsgEditorView.lib
*/
class MMsgAsyncControl
    {
    public:
        
        /**
        * Load media to from given file handle to media control.
        *
        * @param aFileHandle Valid file handle.
        */
        virtual void LoadL( RFile& aFileHandle ) = 0;
        
        /**
        * Closes control. Closing means freeing file
        * specific resources that other application can use the file.
        * This function also takes care of stopping playin, if needed
        */         
        virtual void Close() = 0;
        
        /**
        * Cancels the asyncronous operation if it is pending.
        */         
        virtual void Cancel() = 0;
             
        /**
        * Return current state.
        */
        virtual TMsgAsyncControlState State() const = 0;

        /**
        * Return error code of the operation.
        */
        virtual TInt Error() const = 0;
        
        /**
        * Adds observer.
        */
        virtual void AddObserverL( MMsgAsyncControlObserver& aObserver ) = 0;
        
        /**
        * Removes observer.
        */
        virtual void RemoveObserver( MMsgAsyncControlObserver& aObserver ) = 0;        
    };

#endif // MMSGASYNCCONTROL_H
