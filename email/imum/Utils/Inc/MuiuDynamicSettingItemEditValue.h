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
* Description:  MuiuDynamicSettingItemEditValue.h
*
*/



#ifndef __MUIUDYNSETTINGITEMEDITVALUE_H__
#define __MUIUDYNSETTINGITEMEDITVALUE_H__

// INCLUDES
#include <e32base.h>
#include "MuiuDynamicSettingsArray.hrh"
#include <muiuflags.h>

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class CMuiuSettingsEditValue : public CMuiuSettingBase
    {
    public: // Constructors and Destructor

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CMuiuSettingsEditValue();

        /**
        *
        * @since S60 3.0
        */
        static CMuiuSettingsEditValue* NewL();
        static CMuiuSettingsEditValue* NewLC();

    public: // Operators

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingsEditValue& operator=(
            const CMuiuSettingsEditValue& aBaseItem );

    public: // New Functions

        /**
        *
        * @since S60 3.0
        */
        inline TInt64 Value() const;

        /**
        *
        * @since S60 3.0
        */
        inline void SetValue( const TInt64 aValue );

    private: // Constructors

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingsEditValue();

        /**
        *
        * @since S60 3.0
        */
        void ConstructL();

    private: // Data
        TInt64              iUserValue;

    public: // Data
        TInt                iItemMaxLength;
        TMuiuSettingsText*  iItemSettingText;
    };

#include "MuiuDynamicSettingItemEditValue.inl"

#endif //__MUIUDYNSETTINGITEMEDITVALUE_H__

// End of file
