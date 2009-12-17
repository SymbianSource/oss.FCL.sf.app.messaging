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
* Description:  IMSSettingsAccessPointItem.h
*
*/


#ifndef CIMSSettingsItemAccessPoint_H
#define CIMSSettingsItemAccessPoint_H

// INCLUDES
#include <e32base.h>
#include "MuiuDynamicSettingItemLink.h"     // CMuiuSettingsLink
#include "MuiuDynamicSettingsArray.hrh"     // TMuiuDynSettingsType
#include <cdbcols.h>                        // TCommDbDialogPref
#include <cmapplicationsettingsui.h>		// TCmSettingSelection

// CONSTANTS
// MACROS
// DATA TYPES
enum TIMSettingsItemTypes
    {
    EIMSSetItemAccessPoint = EMuiuDynSetItemLastUnusedItem
    };

// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*
*  @lib
*  @since S60 3.0
*/
class CIMSSettingsAccessPointItem : public CMuiuSettingsLink
    {
    public:  // Constructors and destructor

        /**
        * Creates object from CIMSSettingsItem and leaves it to cleanup stack
        * @since S60 3.0
        * @return, Constructed object
        */
        static CIMSSettingsAccessPointItem* NewLC();

        /**
        * Create object from CIMSSettingsItem
        * @since S60 3.0
        * @return, Constructed object
        */
        static CIMSSettingsAccessPointItem* NewL();

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CIMSSettingsAccessPointItem();

    public: // Operators

        /**
        *
        * @since S60 3.0
        */
        CIMSSettingsAccessPointItem& operator=(
            const CIMSSettingsAccessPointItem& aApItem );

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

    protected:  // Constructors

        /**
        * Default constructor for classCIMSSettingsItem
        * @since S60 3.0
        * @return, Constructed object
        */
        CIMSSettingsAccessPointItem();

        /**
        * Symbian 2-phase constructor
        * @since S60 3.0
        */
        void ConstructL();

    public:     // Data

        // Slot to store the radio button of the selected item
        TInt64               iIapRadioButton;

        // Flag to determine the setting mode
        TBool                iIsWizard;

        // Slot to store the iap selection mode and the iap id
        TCmSettingSelection  iIap;
    };

#include "IMSSettingsItemAccessPoint.inl"

#endif //  CIMSSettingsItemAccessPoint_H

// End of File
