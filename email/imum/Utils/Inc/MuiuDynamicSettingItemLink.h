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
* Description:  MuiuDynamicSettingItemLink.h
*
*/



#ifndef __MUIUDYNSETTINGITEMLINK_H__
#define __MUIUDYNSETTINGITEMLINK_H__

// INCLUDES
#include <e32base.h>
#include "MuiuDynamicSettingItemBase.h"

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class CMuiuSettingsLink : public CMuiuSettingBase
    {
    public: // Constructors and Destructor

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CMuiuSettingsLink();

        /**
        *
        * @since S60 3.0
        */
        static CMuiuSettingsLink* NewL();
        static CMuiuSettingsLink* NewLC();

    public: // Operators

        /**
        *
        * @since S60 3.0
        */
        virtual CMuiuSettingsLink& operator=(
            const CMuiuSettingsLink& aBaseItem );

    public: // Functions from base classes

        /*
        *
        * @since S60 3.0
        */
        virtual inline TBool HasLinkArray() const;

    protected: // Constructors

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingsLink();

        /**
        *
        * @since S60 3.0
        */
        void ConstructL();

    public: // Data

        TMuiuDynSettingsType    iItemSubType;
        CMuiuDynSetItemArray*   iItemLinkArray;
        TMuiuSettingsText*      iItemSettingText;
    };

#include "MuiuDynamicSettingItemLink.inl"

#endif //__MUIUDYNSETTINGITEMLINK_H__

// End of file
