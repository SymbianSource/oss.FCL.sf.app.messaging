/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ImumInSettingsDataCollection.h
*
*/
#ifndef M_IMUMINSETTINGSDATACOLLECTION_H
#define M_IMUMINSETTINGSDATACOLLECTION_H

#include <msvstd.h>

#include <ImumDaSettingsDataCollection.h>

class CImumInSettingsData;

NONSHARABLE_CLASS( MImumInSettingsDataCollection ) :
    public MImumDaSettingsDataCollection
    {
    public:

        virtual TBool operator!=( const MImumInSettingsDataCollection& aSettingsData ) const = 0;
        virtual TBool operator==( const MImumInSettingsDataCollection& aSettingsData ) const = 0;
        virtual TInt Copy( const MImumInSettingsDataCollection& aSettingsData ) = 0;

        /**
         * Creates an identical object of the item.
         *
         * @since S60 v3.2
         * @return A new cloned object
         */
        virtual MImumInSettingsDataCollection* CloneL() const = 0;
    };



#endif /* M_IMUMINSETTINGSDATACOLLECTION_H */
