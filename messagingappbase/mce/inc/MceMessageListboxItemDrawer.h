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
*     Needed because we have to override function Properties so that
*     folders cannot be selected.
*
*/



#ifndef MCEMESSAGELISTBOXITEMDRAWER_H
#define MCEMESSAGELISTBOXITEMDRAWER_H

//  INCLUDES
#include <eikfrlb.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
* Needed because have to override function Properties so that
* folders cannot be selected.
*/
class CMceMessageListBoxItemDrawer : public CFormattedCellListBoxItemDrawer
    {
    public:
        /**
        * C++ default constructor.
        */
        CMceMessageListBoxItemDrawer(
            MTextListBoxModel* aTextListBoxModel,
            const CFont* aFont,
            CFormattedCellListBoxData* aFormattedCellData,
            CMceMessageListContainer& aMessageListContainer );

        /**
        * Destructor.
        */
        ~CMceMessageListBoxItemDrawer();

    public: // Functions from base classes

        /**
        * From CFormattedCellListBoxItemDrawer
        * Needs to be overriden to disable folder selection
        */
        virtual TListItemProperties Properties(TInt aItemIndex) const;

    private: // Data
        CMceMessageListContainer& iMessageListContainer;

    };


#endif      // MCEMESSAGELISTBOXITEMDRAWER_H

// End of File
