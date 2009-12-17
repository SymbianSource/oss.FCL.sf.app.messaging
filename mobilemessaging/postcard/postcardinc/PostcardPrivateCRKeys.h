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
* Description:  
*     Central repository UID and key definitions.
*
*/



#ifndef __POSTCARDPRIVATECRKEYS_H__
#define __POSTCARDPRIVATECRKEYS_H__

#include <e32base.h>

// Maximum length of strings in postcard repository
const TInt KCRPocaMaxString = 128;

// Postcard cenrep repository
const TUid KCRUidPostcard = {0x102824AB};

// Keys into repository

// Posrcard service provider address (number or email address)
const TUint32 KPocaKeyServiceProvider = 0x01;

// Mandatory postcard address fields
// Bits correspond the following fields
// 	1	Name (1)
//	2	Additional info (2)
//	3	Street (4)
//	4	Zip Code (8)
//	5	City (16)
//	6	State (32)
//	7	Country (64)
// default: 21
const TUint32 KPocaKeyMandatoryFields = 0x02;

// Max lengths of fields
// 0 hids the field

const TUint32 KPocaKeyMaxGreetingLength = 0x03;
const TUint32 KPocaKeyMaxNameLength = 0x04;
const TUint32 KPocaKeyMaxInfoLength = 0x05;
const TUint32 KPocaKeyMaxStreetLength = 0x06;
const TUint32 KPocaKeyMaxZipLength = 0x07;
const TUint32 KPocaKeyMaxCityLength = 0x08;
const TUint32 KPocaKeyMaxStateLength = 0x09;
const TUint32 KPocaKeyMaxCountryLength = 0x0a;

// Following numbers correspond the following fields
//  0   Greeting text
// 	1	Name
//	2	Additional info
//	3	Street
//	4	Zip Code
//	5	City
//	6	State
//	7	Country
// If this is left empty, postcard is created using OMA MMS Spec
const TUint32 KPocaKeyServiceSpecialFormat = 0x0b;

// This must be the same as used above
const TUint32 KPocaKeyServiceSeparator = 0x0c;

// This is used to replace the possible separator strings in the user added
// text. Note! This must be of the same length as r_postcard_service_separator
const TUint32 KPocaKeyServiceReplaceString = 0x0d;

// Number of keys
const TInt KPocaKeyCount = KPocaKeyServiceReplaceString;

#endif // __POSTCARDPRIVATECRKEYS_H__
