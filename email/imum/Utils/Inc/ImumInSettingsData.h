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
* Description:  ImumInSettingsData.h
*
*/


#ifndef C_IMUMINSETTINGSDATA_H
#define C_IMUMINSETTINGSDATA_H

// INCLUDES
#include <msvstd.h>
#include <ImumDaSettingsData.h>
#include <ImumDaSettingsKeys.h>
#include <ImumInSettingsKeys.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class MImumInSettingsDataCollection;

// CLASS DECLARATION


class CImumInSettingsData : public CImumDaSettingsData
    {
    public:

        virtual ~CImumInSettingsData() {};
        virtual MImumInSettingsDataCollection& AddInSetL( const TUid& aProtocol ) = 0;
        virtual MImumInSettingsDataCollection& GetInSetL( const TUint aIndex ) const = 0;
        virtual TBool operator!=( const CImumInSettingsData& aSettingsData ) const = 0;
        virtual TBool operator==( const CImumInSettingsData& aSettingsData ) const = 0;
        virtual TInt Copy( const CImumInSettingsData& aSettingsData ) = 0;
        virtual void Log() const = 0;

        /**
         * Creates an identical object of the item.
         *
         * @since S60 v3.2
         * @return A new cloned object
         */
        virtual CImumInSettingsData* CloneL() const = 0;
    };

#endif /* C_IMUMINSETTINGSDATA_H */
