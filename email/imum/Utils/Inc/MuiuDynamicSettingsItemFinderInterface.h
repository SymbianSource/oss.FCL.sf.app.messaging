/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MuiuDynamicSettingsItemFinderInterface.h
*
*/



#ifndef MUIUDYNAMICSETTINGSITEMFINDERINTERFACE_H
#define MUIUDYNAMICSETTINGSITEMFINDERINTERFACE_H

// INCLUDES
#include <e32base.h>
#include "MuiuDynamicSettingItemBase.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

class TMuiuFinderItem
    {
    public: // Data
        CMuiuSettingBase* iItem;
        TInt              iIndex;
    };

typedef CArrayFixFlat<TMuiuFinderItem> CMuiuDynFinderItemArray;

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class MMuiuDynamicSettingsItemFinderInterface
    {
    public: // Constructors and destructor
    public: // New functions
    public: // Functions from base classes
    public: // New virtual functions

        virtual TBool SearchDoError() = 0;

        virtual TBool SearchDoContinuationCheck(
            const CMuiuSettingBase& aItem,
            const TInt aIndex ) = 0;

        virtual TBool SearchDoItemCheck(
            CMuiuSettingBase& aItem ) = 0;

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions
    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data
    };

#endif // MUIUDYNAMICSETTINGSITEMFINDERINTERFACE_H