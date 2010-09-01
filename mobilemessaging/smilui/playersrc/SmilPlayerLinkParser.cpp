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
* Description:  
*       Parses wanted string:
*           Phone numbers
*           E-mail addresses
*           URL addresses
*
*/



// INCLUDE FILES
#include "SmilPlayerLinkParser.h"

// Url Address' beginnings
_LIT( KHttpUrlAddress, "http://");
_LIT( KRtspUrlAddress, "rtsp://");
_LIT( KHttpsUrlAddress, "https://");
_LIT( KWwwUrlAddress, "www.");
_LIT( KWapUrlAddress, "wap.");

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSmilPlayerLinkParser::LinkType
// Returns given link type.
// ----------------------------------------------------------------------------
//
SmilPlayerLinkParser::TSmilPlayerLinkType
       SmilPlayerLinkParser::LinkType( const TDesC& aText )
    {
    TSmilPlayerLinkType result( ESmilPlayerNotSupported );
    if ( SearchMailAddress( aText ) )
        {
        result = ESmilPlayerMailAddress;
        }

    if ( SearchUrl( aText ) )
        {
        result = ESmilPlayerUrlAddress;
        }

    if ( SearchPhoneNumber( aText ) )
        {
        result = ESmilPlayerPhoneNumber; 
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerLinkParser::ParseUrl
// Parses URL from given text string.
// ----------------------------------------------------------------------------
//
TBool SmilPlayerLinkParser::ParseUrl( const TDesC& aType,
                                             const TDesC& aText )
    {
    if( aText.Length() >= aType.Length() ) 
        {
        if( aText.Left( aType.Length() ).MatchF( aType ) != KErrNotFound )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// SmilPlayerLinkParser::SearchMailAddress
// Returns whether given string contains mail address.
// ----------------------------------------------------------------------------
//
TBool SmilPlayerLinkParser::SearchMailAddress( const TDesC& aText )
    {
    if( aText.Length() >= KMailToString().Length() ) 
        {
        if( aText.Left( KMailToString().Length() ).MatchF(
                        KMailToString ) != KErrNotFound )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// SmilPlayerLinkParser::SearchPhoneNumber
// Returns whether given string contains phone number.
// ----------------------------------------------------------------------------
//
TBool SmilPlayerLinkParser::SearchPhoneNumber( const TDesC& aText )
    {
    if( aText.Length() >= KPhoneToString().Length() ) 
        {
        if( aText.Left( KPhoneToString().Length() ).MatchF( 
                        KPhoneToString ) != KErrNotFound )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// SmilPlayerLinkParser::SearchUrl
// Returns whether given string contains URL address.
// ----------------------------------------------------------------------------
//
TBool SmilPlayerLinkParser::SearchUrl( const TDesC& aText )
    {
    TBool wasValidUrl = EFalse;

    // Search for http://
    wasValidUrl = ParseUrl( KHttpUrlAddress, aText );

    if ( !wasValidUrl )
        { // Search for https://
        wasValidUrl = ParseUrl( KHttpsUrlAddress, aText );
        }

    if ( !wasValidUrl )
        { // Search for rtsp://
        wasValidUrl = ParseUrl( KRtspUrlAddress, aText );
        }

    if ( !wasValidUrl )
        { // Search for www.
        wasValidUrl = ParseUrl( KWwwUrlAddress, aText );
        }

    if ( !wasValidUrl )
        { // Search for wap.
        wasValidUrl = ParseUrl( KWapUrlAddress, aText );
        }

    return wasValidUrl;
    }

// End of File  
