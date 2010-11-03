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
* Description:  MuiuDynamicSettingItemEditText.h
*
*/



#ifndef __MUIUDYNSETTINGITEMEDITTEXT_H__
#define __MUIUDYNSETTINGITEMEDITTEXT_H__

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
class CMuiuSettingsEditText : public CMuiuSettingBase
    {
    public: // Constructors and Destructor

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CMuiuSettingsEditText();

        /**
        *
        * @since S60 3.0
        */
        static CMuiuSettingsEditText* NewL();
        static CMuiuSettingsEditText* NewLC();

    public: // Operators

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingsEditText& operator=(
            const CMuiuSettingsEditText& aBaseItem );

    public: // New virtual functions

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

        * @since S60 5.0
        * @return Text value or empty text
        */
        virtual const TDesC& VisibleText() const;

        /**
        *
        * @since S60 3.0
        * @param aText
        */
        virtual inline void SetText( const TMuiuSettingsText* aText );

        /**
        * Set empty text's value. Empty text is used when the Text is not
        * defined.
        *
        * @see VisibleText()
        * @param aEmptyText Empty text.
        * @since S60 5.0
        */
        virtual inline void SetEmptyText( const TDesC& aEmptyText );



    protected: // Constructors

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingsEditText();

        /**
        *
        * @since S60 3.0
        */
        void ConstructL();

    protected: // Data
        /// Own. Text value of this setting item
        TMuiuSettingsText*  iUserText;
        /// Own. Value that is shown when iUserText is undefined
        TMuiuSettingsText*  iEmptyText;

    public: // Data
        TInt                iItemMaxLength;
    };

#include "MuiuDynamicSettingItemEditText.inl"

#endif //__MUIUDYNSETTINGITEMEDITTEXT_H__

// End of file
