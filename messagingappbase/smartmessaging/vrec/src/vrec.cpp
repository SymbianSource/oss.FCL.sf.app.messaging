/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*    Recognizer for vCalendar and vCard.
*
*/



// INCLUDE FILES
#include <apmrec.h>
#include <apmstd.h>
#include "vrec.h"
#include "VRecLog.h"

#include <e32std.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

// CONSTANTS
const TUid KUidVREC   = {0x101F4CF5};

// maximum amount of buffer space we will ever use
// Value estimated on observation of iCal userid lengths
const TInt KMaxBufferLength        = 255;

const TText8* const mimeTypes[] =
    {
	_S8("text/X-vCard"),
    _S8("text/calendar"),   // iCalendar MIME type (RFC 2445)
    _S8("text/x-vCalendar") // vCalendar v1.0 MIME type
    };

_LIT8(KDataTypeVCard,      "text/X-vCard");
_LIT8(KDataTypeVCalendar2, "text/x-vCalendar"); // vCalendar v1.0 MIME type
_LIT8(KDataTypeVCalendar3, "text/calendar"); // vCalendar v2.0 MIME type

_LIT8(KBegin,          "BEGIN");
_LIT8(KVCalendar,      "VCALENDAR");
_LIT8(KBeginVCard,      "BEGIN:VCARD");
_LIT8(KVersionICal,		"VERSION:2.0");

/**
* VCalendar definition from vCalendar spec v1.0
* ws = 1*(SPACE / HTAB)
* wsls = 1*(SPACE / HTAB / CRLF)
* vcal_file = [wsls] vcal [wsls]
* vcal = "BEGIN"[ws]":"[ws] "VCALENDAR" [ws] 1*CRLF
*        calprop calenties [ws] *CRLF
*       "END" [ws] ":" [ws] "VCALENDAR" [ws] 1*CRLF
*/
// ============================ MEMBER FUNCTIONS ===============================
CVRec::CVRec()
    :CApaDataRecognizerType(KUidVREC, CApaDataRecognizerType::ELow)
    {
    iCountDataTypes = sizeof(mimeTypes)/sizeof(TText*);
    LOG1("CVRec::iCountDataTypes: %d", iCountDataTypes);
    }

TUint CVRec::PreferredBufSize()
    {
    return KMaxBufferLength;
    }

TDataType CVRec::SupportedDataTypeL(TInt aIndex) const
    {
    __ASSERT_DEBUG(aIndex>=0 && aIndex<iCountDataTypes, User::Invariant());
    TPtrC8 ptr;
    ptr.Set(mimeTypes[aIndex]);
	return TDataType(ptr);
    }

void CVRec::DoRecognizeL(const TDesC& /*aName*/, const TDesC8& aBuffer)
    {
    LOG1("CVRec::DoRecognizeL: %S", &aBuffer);
    TInt length = aBuffer.Length();
    
    // The vCalendar header is longer than the vCard header.
    TInt lengthNeeded = KBeginVCard().Length();
    if (length < lengthNeeded)
        {
        // not enough data for it to be a vCard or vCalendar or iCalendar
        return;
        }
        
    TPtrC8 data = aBuffer.Left(KMaxBufferLength);
    
    TInt posVCard = data.FindF( KBeginVCard );
    
    //the position must be at the beginning of the file
    if (posVCard == 0)
    	{
        iDataType   = TDataType(KDataTypeVCard);
        iConfidence = ECertain;
        return;
        }
    else if (posVCard != KErrNotFound)
    	{
    	iConfidence = ENotRecognized;
    	return;
    	}
    	
    if (data.FindF(KBegin) != KErrNotFound && 
    	data.FindF(KVCalendar) != KErrNotFound && 
    	data.FindF(KVersionICal) != KErrNotFound)
		{
	    iDataType   = TDataType(KDataTypeVCalendar3);
	    iConfidence = ECertain;
	    return;
        }
    else if (data.FindF(KBegin) != KErrNotFound && 
    		 data.FindF(KVersionICal) == KErrNotFound && 
    		 data.FindF(KVCalendar) != KErrNotFound)
        {
        iDataType   = TDataType(KDataTypeVCalendar2);
        iConfidence = ECertain;
        return;
        }
    else
        {
        iConfidence = ENotRecognized;
        return;
        }
    }
    
CApaDataRecognizerType* CVRec::CreateRecognizerL()
	{
	return new (ELeave) CVRec ();
	}

const TImplementationProxy ImplementationTable[] =
	{
#ifdef __EABI__
        IMPLEMENTATION_PROXY_ENTRY( 0x101F879A, CVRec::CreateRecognizerL )
#else
		{ { 0x101F879A }, CVRec::CreateRecognizerL }
#endif
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }





// end of file
