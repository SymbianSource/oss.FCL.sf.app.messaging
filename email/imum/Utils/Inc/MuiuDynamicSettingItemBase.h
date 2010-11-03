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
* Description:  MuiuDynamicSettingItemBase.h
*
*/



#ifndef __MUIUDYNSETTINGITEMBASE_H__
#define __MUIUDYNSETTINGITEMBASE_H__

// INCLUDES
#include <e32base.h>
#include <muiuflags.h>
#include "MuiuDynamicSettingsArray.hrh"
#include "MuiuDynamicHeader.h"

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class CMuiuSettingBase : public CBase
    {
    public: // Constructors and Destructor

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CMuiuSettingBase();

        /**
        *
        * @since S60 3.0
        */
        static CMuiuSettingBase* NewL();
        static CMuiuSettingBase* NewLC();

    public: // Operators

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingBase& operator=(
            const CMuiuSettingBase& aBaseItem );

    public: // New virtual functions

        /**
        *
        * @since S60 3.0
        */
        virtual inline TMuiuDynSettingsType Type() const;

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

        /**
        *
        * @since S60 3.0
        * @return
        */
        virtual inline const TMuiuSettingsText* Text() const;

        /**
        * Returns the text value for the UI. Returns the user assigned value
        * or empty text. Empty text is given if user assigned value is not
        * defined
        *
        * @since S60 5.0
        * @return Visible text value.
        */
        virtual inline const TDesC& VisibleText() const;

        /**
        *
        * @since S60 3.0
        * @param aText
        */
        virtual inline void SetText( const TMuiuSettingsText* aText );

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
        CMuiuSettingBase();

        /**
        *
        * @since S60 3.0
        */
        void ConstructL();

    public: // Data

        TMuiuFlags              iItemFlags;
        TMuiuDynSettingsType    iItemType;
        TMuiuSettingsText*      iItemLabel;
        TUid                    iItemId;
        TInt                    iItemResourceId;
    };

#include "MuiuDynamicSettingItemBase.inl"

#endif //__MUIUDYNSETTINGITEMBASE_H__

// End of file
