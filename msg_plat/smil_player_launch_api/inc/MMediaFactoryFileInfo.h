/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*        Interface for the CSmilPlayerMediaFactory class.
*
*/



#ifndef MMEDIAFACTORYFILEINFO_H
#define MMEDIAFACTORYFILEINFO_H

// INCLUDES
#include <e32std.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RFile;

// CLASS DECLARATION

/**
*  Interface class for the CSmilPlayerMediaFactory class.
*
*  @since 2.0
*/
class MMediaFactoryFileInfo
    {
    public:
        
        /**
        * Implementation should return file handle matching given URL.
        *
        * @since 3.0
        *
        * @param aUrl   IN      URL that file handle is requested.
        * @param aFile  OUT     Should contain file handle that matches to the URL
        *                       when function returns. Ownership of the handle
        *                       is transferred to caller of the function.
        *
        * @return void
        */
        virtual void GetFileHandleL( const TDesC& aUrl, RFile& aFile ) = 0;
        
        /**
        * Implementation should return MIME type matching given URL.
        *
        * @since 2.0
        *
        * @param aUrl   IN      URL that MIME type is requested.
        *
        * @return Descriptor that contains matching MIME type.
        */
        virtual TPtrC8 GetMimeTypeL( const TDesC& aUrl ) = 0;
        
        /**
        * Implementation should return character set matching given URL.
        *
        * @since 2.0
        *
        * @param aUrl   IN      URL that character set is requested.
        *
        * @return IANA / WSP number for character set of the given URL.
        */
        virtual TUint GetCharacterSetL( const TDesC& aUrl ) = 0;
    };

#endif // MMEDIAFACTORYFILEINFO_H
