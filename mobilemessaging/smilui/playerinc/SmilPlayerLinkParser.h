/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SmilPlayerLinkParser  declaration
*
*/


#ifndef SMILPLAYERLINKPARSER_H
#define SMILPLAYERLINKPARSER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
_LIT( KMailToString, "mailto:");
_LIT( KPhoneToString, "phoneto:");

// CLASS DECLARATION

/**
*  Parser the link
*
* @lib smilplayer.lib
* @since 3.0
*/
NONSHARABLE_CLASS(SmilPlayerLinkParser)
    {

    public:

        /**
        * Enumeration to define the type of the parsed link
        */
        enum TSmilPlayerLinkType
            {
            ESmilPlayerNotSupported,
            ESmilPlayerPhoneNumber,
            ESmilPlayerMailAddress,
            ESmilPlayerUrlAddress
            };

    public:  // New functions

        /**
        * Returns link type for a given link
        *
        * @since 3.0
        *
        * @param aText          Text that will be parsed
        * @return link type
        */
        static TSmilPlayerLinkType LinkType( const TDesC& aText );

    private:

        /**
        * Search algorithm for phone number
        */
        static TBool SearchPhoneNumber( const TDesC& aText);
        
        /**
        * Search algorithm for email address
        */
        static TBool SearchMailAddress( const TDesC& aText);
        
        /**
        * Search algorithm for URL address
        */
        static TBool SearchUrl( const TDesC& aText);

        /**
        * Parses URL. Is used by SearchUrlL method and if a url
        * was found it returns ETrue, otherwise EFalse.
        */
        static TBool ParseUrl( const TDesC& aType, const TDesC& aText );

    };

#endif // SMILPLAYERLINKPARSER_H

// End of File
