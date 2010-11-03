/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Includes real address, alias and direct/indirect info
*
*/




#ifndef __MUIUMSGUPLOADPARAM_H__
#define __MUIUMSGUPLOADPARAM_H__


//  INCLUDES
#include <e32std.h>
#include <e32def.h>

// CONSTANTS

const TUint KMaxMsgUploadAddressLength = 256;
const TUint KMaxMsgUploadAliasLength = 256;

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Includes real address, alias and direct/indirect info
*/
class TMsgUploadParameters
    {
    public:
        inline TMsgUploadParameters();
        inline TMsgUploadParameters( const TMsgUploadParameters& aParams );
    public:
        TBuf<KMaxMsgUploadAddressLength> iRealAddress;
        TBuf<KMaxMsgUploadAliasLength> iAlias;
        TBool iDirect;
    };

inline TMsgUploadParameters::TMsgUploadParameters()
    : iRealAddress( KNullDesC ),
      iAlias( KNullDesC ),
      iDirect( 0 )
    {}

inline TMsgUploadParameters::TMsgUploadParameters( const TMsgUploadParameters& aParams )
    : iRealAddress( aParams.iRealAddress ),
      iAlias( aParams.iAlias ),
      iDirect( aParams.iDirect )
    {}

#endif // __MUIUMSGUPLOADPARAM_H__

// End of file
