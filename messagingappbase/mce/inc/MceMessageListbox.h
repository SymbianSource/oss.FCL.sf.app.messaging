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
*     Message list view's listbox to create object of
*     CMceMessageListBoxItemDrawer.
*
*/



#ifndef MCEMESSAGELISTBOX_H
#define MCEMESSAGELISTBOX_H

//  INCLUDES
#include <aknlists.h>

// FORWARD DECLARATIONS
class CMceMessageListContainer;

// CLASS DECLARATION

/**
*  This class is needed to create correct item drawer.
*/
class CMceMessageListBox : public CAknDoubleGraphicStyleListBox
    {
    public:
        /**
        * C++ constructor.
        */
        CMceMessageListBox(CMceMessageListContainer& aMessageListContainer);

        /**
        * Destructor.
        */
        ~CMceMessageListBox();

    protected:
        /**
        * From CAknDoubleGraphicStyleListBox
        */
        virtual void CreateItemDrawerL();

    private: // Data
        CMceMessageListContainer& iMessageListContainer;

    };


#endif      // MCEMESSAGELISTBOX_H

// End of File
