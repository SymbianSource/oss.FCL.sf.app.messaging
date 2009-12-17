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
* Description: Extracts needed attributes from received EMN message.
*		
*
*/


#include "EMNXMLContentHandler.h"
#include "EMNLogging.h"

// Used in compares
_LIT8( KEmnElement, "emn" );
_LIT8( KMailboxAttribute, "mailbox");
_LIT8( KTimestampAttribute, "timestamp");

// Used for parsing timestamp attribute's value from ASCII message.
_LIT( KColonChar, ":" );
_LIT( KDashChar, "-" );
_LIT( KDateTimeSeparator, "T" );
_LIT( KTimezoneSeparator, "Z" );

// Received timestamp string length
const TInt KEMNLengthOfRecvTimeStamp = 20;

// Index of hour or minute in timestamp string
const TInt KEMNTimestampIncHour = 5;
const TInt KEMNTimestampIncHourMinute = 6;

// Constants for getting first or last four bits from byte
const TInt KEMNUpperBits = 4;
const TInt KEMNLowerBits = 0xf;

// Constants for date handling
const TInt KEMNAtLeastDate = 4;
const TInt KEMNIndexOfMilleniumAndCentury = 0;
const TInt KEMNIndexOfDecadeAndYear = 1;
const TInt KEMNIndexOfMonth = 2;
const TInt KEMNIndexOfDay = 3;

// Constants for time handling
const TInt KEMNIndexOfHour = 4;
const TInt KEMNIndexOfMinute = 5;

using namespace Xml;

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::CEMNXMLContentHandler
//-----------------------------------------------------------------------------
CEMNXMLContentHandler::CEMNXMLContentHandler(
    TEMNElement& aElement, TBool aIsAscii ) :
    iElement( aElement ), 
    iIsAscii( aIsAscii ), 
    iFoundMailboxAttribute( EFalse )
	{
	KEMNLOGGER_WRITE("CEMNXMLContentHandler::CEMNXMLContentHandler()");
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::NewL
//-----------------------------------------------------------------------------
CEMNXMLContentHandler* CEMNXMLContentHandler::NewL( 
    TEMNElement& aElement, 
    TBool aIsAscii )
	{
	CEMNXMLContentHandler* self = NewLC( aElement, aIsAscii );
	CleanupStack::Pop( self );
	return self;
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::NewLC
//-----------------------------------------------------------------------------
CEMNXMLContentHandler* CEMNXMLContentHandler::NewLC(
    TEMNElement& aElement, 
    TBool aIsAscii )
	{
	CEMNXMLContentHandler* self = 
	    new (ELeave) CEMNXMLContentHandler( aElement, aIsAscii );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::ConstructL
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::ConstructL()
    {
    }

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::~CEMNXMLContentHandler
//-----------------------------------------------------------------------------
CEMNXMLContentHandler::~CEMNXMLContentHandler()
	{
	KEMNLOGGER_WRITE("CEMNXMLContentHandler::~CEMNXMLContentHandler()");
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::OnStartDocumentL
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::OnStartDocumentL(
    const RDocumentParameters& /*aDocParam*/, 
    TInt /*aErrorCode*/ )
	{
	KEMNLOGGER_WRITE("CEMNXMLContentHandler::OnStartDocumentL()");
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::OnEndDocumentL
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::OnEndDocumentL( TInt /*aErrorCode*/ )
	{
	KEMNLOGGER_WRITE("<- EMN message data");
	
	// This EMN message will be discarded if no mailbox attribute found.
	if ( !iFoundMailboxAttribute ) 
	    {
	    KEMNLOGGER_WRITE("CEMNXMLContentHandler::OnEndDocumentL(): No mailbox attribute found, leaving.");
	    User::Leave( EEMNMissingMailboxAttribute );
	    }
	}
	
//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::OnStartElementL
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::OnStartElementL(
    const RTagInfo& aElement, 
    const RAttributeArray& aAttributes, 
    TInt aErrorCode )
	{
	KEMNLOGGER_WRITE("CEMNXMLContentHandler::OnStartElementL()");
	KEMNLOGGER_WRITE("EMN message data ->");
	
	// Element name
	const TDesC8& localPart8 = aElement.LocalName().DesC();
    KEMNLOGGER_WRITE_FORMAT8("<%s>", localPart8.Ptr() );
    
    // Attribute(s) of emn element
    if ( localPart8.Compare( KEmnElement ) == 0 )   // Is emn element
        {
        TInt attributeCount = aAttributes.Count();
        
        for ( TInt n = 0; n < attributeCount; n++ )
            {
            const RAttribute& attribute = aAttributes[n];
            const RTagInfo& nameInfo = attribute.Attribute();
            
            const TDesC8& localPart8 = nameInfo.LocalName().DesC();
    		const TDesC8& prefix8 = nameInfo.Prefix().DesC();
    		const TDesC8& value8 = attribute.Value().DesC();
    		
    		// Mailbox attribute is similar to both ascii and binary
    		if ( localPart8.Compare( KMailboxAttribute ) == 0)
    		    {
    		    KEMNLOGGER_WRITE_FORMAT8("mailbox=%s", value8.Ptr() );
    		    // Read the whole value of mailbox attribute and copy that
                // to iElement. Conversion happens from 8-bit to 16 bit
                // unicode.
                iElement.mailbox.Copy( value8.Left( KAOMailboxAttributeLength ) );
                iFoundMailboxAttribute = ETrue;
    		    }
    		// Timestamp attribute is in different form in ascii than in binary
    		// so we need to extract it with different methods.
    		else if ( localPart8.Compare( KTimestampAttribute ) == 0 )
    		    {
    		    KEMNLOGGER_WRITE_FORMAT8("timestamp=%s", value8.Ptr() );
      		    // NOTE: Currently timestamp attribute is omitted. 
                /* This code branch will be commented out when there is some nice
    	        use case for timestamp.
    		    if ( iIsAscii )
    		        {
    		        HandleXMLAttributesL( localPart8, value8 );
    		        }
                else
                    {
                    HandleWBXMLAttributesL( localPart8, value8 );
                    }
    	        */
    		    }
    		// Something else than mailbox or timestamp attribute
        	else
    		    {
    		    KEMNLOGGER_WRITE_FORMAT28("Unknown attribute: %s=%s", localPart8.Ptr(), value8.Ptr() );
    		    }  
            }
        }
    else    // No emn element found
        {
        KEMNLOGGER_WRITE_FORMAT8("%s\">", localPart8.Ptr() );
        User::Leave( EEMNMissingEMNElement );
        }
    
	User::LeaveIfError( aErrorCode );
	}
	
//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::HandleXMLAttributes
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::HandleXMLAttributesL(
    const TDesC8& aAttributeName8, 
    const TDesC8& aAttributeValue8 )
    {
    // Currently only timestamp attribute is handled here. Mailbox attribute
    // is same for both XML and WBXML.
    if ( aAttributeName8.Compare( KTimestampAttribute ) == 0 )
	    {
	    iElement.timestamp = NULL;
	    
	    TPtrC8 p8( aAttributeValue8.Ptr(), aAttributeValue8.Length() );
	    HBufC* tmpBuf = HBufC::NewLC( aAttributeValue8.Length() );
	    // Copies 8 bit to 16 bit and makes a new copy from heap
        tmpBuf->Des().Copy( p8 );
        TPtr ptr = tmpBuf->Des();
        
        if ( ptr.Length() == KEMNLengthOfRecvTimeStamp )
            {
            // Parse received timestamp to be in a form, which can be set to TTime 
            // "2010-12-31T00:01:00Z" != YYYYMMDD:HHMMSS
            TInt pos = ptr.Find( KColonChar );
            while ( pos != KErrNotFound )
                {
                ptr.Delete( pos, 1 );
                pos = ptr.Find( KColonChar );
                }
                
            // "2010-12-31T000100Z" != YYYYMMDD:HHMMSS
            pos = ptr.Find( KDashChar );
            while ( pos != KErrNotFound )
                {
                ptr.Delete( pos, 1 );
                pos = ptr.Find( KDashChar );
                }
            
            // "20101231T000100Z" != YYYYMMDD:HHMMSS
            pos = ptr.Find( KDateTimeSeparator );
            while ( pos != KErrNotFound )
                {
                ptr.Replace( pos, 1, KColonChar );
                pos = ptr.Find( KDateTimeSeparator );
                }
            
            // "20101231:000100Z" != YYYYMMDD:HHMMSS
            pos = ptr.Find( KTimezoneSeparator );
            while ( pos != KErrNotFound )
                {
                ptr.Delete( pos, 1 );
                pos = ptr.Find( KTimezoneSeparator );
                }
            
            // "20101231:000100" == YYYYMMDD:HHMMSS, but day and month must be 
            // decreased by one, because those are offset from zero and we 
            // received them in offset from one.
            
            /*
            * if MM == 09 -> MM = 08
            * if MM == 10 -> MM = 09
            * if MM == 11 -> MM = 10
            */
            if ( ( ptr[4] == '1' ) && (  ptr[5] == '0' ) )  // CSI: 47 # see comments
                {
                ptr[4] = ( ptr[4] - 0x1 );  // 1 -> 0
                ptr[5] = ( ptr[5] + 0x9 );  // 0 -> 9
                }
            else
                {
                ptr[5] = ( ptr[5] - 0x1 );  // n -> n-1
                }

            /*
            * if DD == 09 -> DD = 08
            * if DD == 13 -> DD = 12
            * if DD == 28 -> DD = 27
            * if DD == 31 -> DD = 30
            * if DD == 10 -> DD = 09
            * if DD == 20 -> DD = 19
            * if DD == 30 -> DD = 29
            */
            if ( ( ( ptr[6] == '1' ) ||                          // CSI: 47 # see comments
                   ( ptr[6] == '2' ) ||                          // CSI: 47 # see comments
                   ( ptr[6] == '3' ) ) && ( ptr[7] ) == '0' )    // CSI: 47 # see comments
                {
                ptr[6] = ( ptr[6] - 0x1 );  // n -> n-1
                ptr[7] = ( ptr[7] + 0x9 );  // 0 -> 9
                }
            else
                {
                ptr[7] = ( ptr[7] - 0x1 );  // n -> n-1
                }
            
            // Now the "20101231:000100" should be "20101130:000100"
            // Make a TTime object from it.
            TTime time;
            TInt err = time.Set( ptr );
            if ( err == KErrNone )
                {
                iElement.timestamp = time;
                }
            else
                {
                iElement.timestamp = NULL;
                }
            }
            
        CleanupStack::PopAndDestroy( tmpBuf );
	    }
    }

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::HandleWBXMLAttributes
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::HandleWBXMLAttributesL(
    const TDesC8& aAttributeName8, 
    const TDesC8& aAttributeValue8 )
    {
    // Currently only timestamp attribute is handled here. Mailbox attribute
    // is same for both XML and WBXML.
    if ( aAttributeName8.Compare( KTimestampAttribute ) == 0 )
	    {
	    iElement.timestamp = NULL;
	    
	    // Here is an example of received timestamp string: 20 05 11 12 13 14 01
	    // It is 12th of November 2005, 01:14:01 PM
	    
	    // Timestamp length must be atleast 4. It means that at least year, month
	    // and day must be given, because those can't be zeros. Hour, minute and 
	    // second can be zeros. But not all the Push Content GWs doesn't send those,
	    // so we must handle both situations.
	    TInt timestampLength = aAttributeValue8.Length();
	    if ( timestampLength >= KEMNAtLeastDate )
	        {
	        TDateTime datetime;
            TInt err;
            TInt temp1;
            TInt temp2;
            
            temp1 = aAttributeValue8[ KEMNIndexOfMilleniumAndCentury ];
            temp2 = aAttributeValue8[ KEMNIndexOfDecadeAndYear ];
            
            // Separately parse millenium, century, decade and year
            TInt year = 
                ( ( temp1 >> KEMNUpperBits ) * 1000 ) +      // CSI: 47 # see comments
                ( ( temp1 & KEMNLowerBits ) * 100 ) +        // CSI: 47 # see comments
                ( ( temp2 >> KEMNUpperBits ) * 10 ) +        // CSI: 47 # see comments
                ( temp2 & KEMNLowerBits );

            temp1 = aAttributeValue8[ KEMNIndexOfMonth ];
            TInt month = 
                ( ( temp1 >> KEMNUpperBits ) * 10 ) +        // CSI: 47 # see comments
                ( temp1 & KEMNLowerBits );

            temp1 = aAttributeValue8[ KEMNIndexOfDay ];
            TInt day = 
                ( ( temp1 >> KEMNUpperBits ) * 10 ) +        // CSI: 47 # see comments
                ( temp1 & KEMNLowerBits );

            err = datetime.SetYear( year );
            if ( err != KErrNone )
                {
                User::Leave( EEMNInvalidYear );
                }
                
            err = datetime.SetMonth( static_cast<TMonth>( month - 1  ) );
            if ( err != KErrNone )
                {
                User::Leave( EEMNInvalidMonth );
                }
                
            err = datetime.SetDay( day - 1 );
            if ( err != KErrNone )
                {
                User::Leave( EEMNInvalidDay );
                }
                
            // There might not be hour, minute and second in EMN message
            if ( timestampLength > KEMNAtLeastDate )
                {
                // At least hour                
                if ( timestampLength >= KEMNTimestampIncHour )
                    {
                    temp1 = aAttributeValue8[ KEMNIndexOfHour ];
                    TInt hour = ( ( temp1 >> KEMNUpperBits ) * 10 ) +    // CSI: 47 # see comments
                        ( temp1 & KEMNLowerBits );

                    err = datetime.SetHour( hour );
                    if ( err != KErrNone )
                        {
                        User::Leave( EEMNInvalidHour );
                        }
                    }
                
                // At least hour and minute
                if ( timestampLength >= KEMNTimestampIncHourMinute )
                    {
                    temp1 = aAttributeValue8[ KEMNIndexOfMinute ];
                    TInt minute = 
                        ( ( temp1 >> KEMNUpperBits ) * 10 ) +    // CSI: 47 # see comments
                        ( temp1 & KEMNLowerBits );
                            
                    err = datetime.SetMinute( minute );
                    if ( err != KErrNone )
                        {
                        User::Leave( EEMNInvalidMinute );
                        }
                    }
                }
            else
                {
                datetime.SetHour( 0 );
                datetime.SetMinute( 0 );
                }
                
            // At this point, datetime should be well formed, so no need to do extra
            // checks.
            TTime time( datetime );
            
            iElement.timestamp = time;
            KEMNLOGGER_WRITE_DATETIME("timestamp=", time );
	        }
	    else
	        {
	        KEMNLOGGER_WRITE("Timestamp provided, but not valid ==> EMN discarded!" );
	        KEMNLOGGER_WRITE_FORMAT("Timestamp length was %d", aAttributeValue8.Length() );
	        
	        // Timestamp attribute was found, but its format was not valid.
	        User::Leave( EEMNInvalidTimestampAttribute );
	        }
	    }
    }

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::CEMNXMLContentHandler
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::OnEndElementL(
    const RTagInfo& aElement, 
    TInt /*aErrorCode*/ )
	{
	const TDesC8& localPart8 = aElement.LocalName().DesC();
	KEMNLOGGER_WRITE_FORMAT8("</%s>", localPart8.Ptr() );
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::OnContentL
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::OnContentL(
    const TDesC8& /*aData8*/, 
    TInt /*aErrorCode*/ )
	{
	// Content of an EMN message should always be empty.
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::OnStartPrefixMappingL
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::OnStartPrefixMappingL(
    const RString& /*aPrefix*/,
    const RString& /*aUri*/,
    TInt /*aErrorCode*/ )
	{
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::OnEndPrefixMappingL
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::OnEndPrefixMappingL(
    const RString& /*aPrefix*/,
    TInt /*aErrorCode*/ )
	{
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::OnIgnorableWhiteSpaceL
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::OnIgnorableWhiteSpaceL(
    const TDesC8& /*aBytes*/, 
    TInt /*aErrorCode*/ )
	{
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::OnSkippedEntityL
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::OnSkippedEntityL(
    const RString& /*aName*/, 
    TInt /*aErrorCode*/ )
	{
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::OnProcessingInstructionL
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::OnProcessingInstructionL(
    const TDesC8& /* aTarget8*/,
    const TDesC8& /*aData8*/,
    TInt /*aErrorCode*/ )
	{
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::OnExtensionL
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::OnExtensionL(
    const RString& /*aData*/, 
    TInt /*aToken*/,
    TInt /*aErrorCode*/ )
	{
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::OnError
//-----------------------------------------------------------------------------
void CEMNXMLContentHandler::OnError( TInt __DEBUG_ONLY( aError ) )
	{
	KEMNLOGGER_WRITE_FORMAT("CEMNXMLContentHandler::OnError() aError = %d", aError );
	
#ifdef _DEBUG
	// No actions taken, if there is something wrong with elements or attributes
	// in received EMN message. This is because OMA EMN spesification says that
	// received message can be discarded if it is not valid or well-formed. This
	// error handling is just for debugging purposes.
	switch ( aError )
	    {
  	    case EEMNMissingEMNElement:
  	        KEMNLOGGER_WRITE("No EMN element found.");
  	        break;
  	        
	    case EEMNMissingMailboxAttribute:
	        KEMNLOGGER_WRITE("Mailbox attribute not found.");
	        break;
	        
	    case EEMNMissingTimestampAttribute:
	        KEMNLOGGER_WRITE("Timestamp attribute not found.");
	        break;
	    
	    // Received timestamp attribute did not contain proper date and time.
	    case EEMNInvalidTimestampAttribute:

	    // Following errors indicates that values were not in acceptable ranges
	    case EEMNInvalidYear:
	    case EEMNInvalidMonth:
	    case EEMNInvalidDay:
	    case EEMNInvalidMinute:
	    case EEMNInvalidHour:
            KEMNLOGGER_WRITE("Timestamp attribute found, but with invalid value.");
	        break;
	    default:
	        KEMNLOGGER_WRITE("Invalid error code, where we got it?.");
	    }
#endif
	}

//-----------------------------------------------------------------------------
// CEMNXMLContentHandler::GetExtendedInterface
//-----------------------------------------------------------------------------
TAny* CEMNXMLContentHandler::GetExtendedInterface( const TInt32 )
	{
	KEMNLOGGER_WRITE("CEMNXMLContentHandler::GetExtendedInterface()");
	return NULL;
	}

