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
* Description:  MuiuDynamicSettingItemExtLink.h
*
*/


#ifndef __MUIUDYNSETTINGITEMEXTLINK_H__
#define __MUIUDYNSETTINGITEMEXTLINK_H__

// INCLUDES
#include <e32base.h>
#include "MuiuDynamicSettingItemLink.h"

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class CMuiuSettingsLinkExtended : public CMuiuSettingsLink
    {
        public: // Constructors and Destructor

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CMuiuSettingsLinkExtended();

        /**
        *
        * @since S60 3.0
        */
        static CMuiuSettingsLinkExtended* NewL();
        static CMuiuSettingsLinkExtended* NewLC();

    public: // Operators

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingsLinkExtended& operator=(
            const CMuiuSettingsLinkExtended& aBaseItem );

    public: // New Functions

        /**
        *
        * @since S60 3.0
        */
        virtual inline TInt64 Value() const;

        /**
        *
        * @since S60 3.0
        */
        virtual inline void SetValue( const TInt64 aValue );

    protected: // Constructors

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingsLinkExtended();

        /**
        *
        * @since S60 3.0
        */
        void ConstructL();

    protected: // Data

        TInt64                  iUserValue;
    };

#include "MuiuDynamicSettingItemExtLink.inl"

#endif //__MUIUDYNSETTINGITEMEXTLINK_H__

// End of file
