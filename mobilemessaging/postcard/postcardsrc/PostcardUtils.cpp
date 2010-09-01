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
* Description:   Collection of utility functions used by Postcard classes
*
*/



// INCLUDE FILES
#include <e32base.h>

#include "PostcardUtils.h"
#include "Postcard.hrh"
#include <cntitem.h>
#include <cntfldst.h>
#include <centralrepository.h>
#include "PostcardPrivateCRKeys.h"  // cenrep keys
#include "PostcardPanic.h"        // Panic enums

// CONSTANTS

static const struct
    {
    const TText* iText;
    TInt iId;
    } ContactItemNames[] =
        {
        {_S("NAME"), EPostcardAddressName},
        {_S("INFO"), EPostcardAddressInfo},
        {_S("STREET"), EPostcardAddressStreet},
        {_S("ZIP"), EPostcardAddressZip},
        {_S("CITY"), EPostcardAddressCity},
        {_S("STATE"), EPostcardAddressState},
        {_S("COUNTRY"), EPostcardAddressCountry},
        };
const TInt KContactItemNames =
    sizeof(ContactItemNames) / sizeof(ContactItemNames[0]);

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PostcardUtils::ContactItemNameFromId
// -----------------------------------------------------------------------------
const TPtrC TPostcardUtils::ContactItemNameFromId( TInt aId )
    {
    for(TInt i = 0; i < KContactItemNames; i++)
        {
        if (ContactItemNames[i].iId == aId)
            {
            return ContactItemNames[i].iText;
            }
        }
	return KNullDesC();
	}

// -----------------------------------------------------------------------------
// PostcardUtils::IdFromContactItemName
// -----------------------------------------------------------------------------
TInt TPostcardUtils::IdFromContactItemName( const TDesC& aLabel )
	{
    for(TInt i = 0; i < KContactItemNames; i++)
        {
        TPtrC ptr(ContactItemNames[i].iText);
        if (aLabel.Compare(ptr) == 0)
            {
            return ContactItemNames[i].iId;
            }
        }
    return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// PostcardUtils::FieldOrNull
// -----------------------------------------------------------------------------
const CContactItemField* TPostcardUtils::FieldOrNull(
    const CContactItemFieldSet& aSet, const TDesC& aLabel )
    {
    for( TInt i = 0; i < aSet.Count( ); i++ )
        {
        const CContactItemField& field = aSet[i];
        if ( field.Label().Compare( aLabel ) == 0 )
            {
            return &field;
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// PostcardUtils::AddContactField
// -----------------------------------------------------------------------------
void TPostcardUtils::AddContactFieldL(CContactCard& aCard, const TDesC& aLabel,
    const TDesC& aFieldText)
    {
    // Create a new CContactCard field
    CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText);
    CContactTextField* itemText = newField->TextStorage();
    itemText->SetTextL(aFieldText);
    // Set the label in the field
    newField->SetLabelL(aLabel);
    // Add the field in the card
    aCard.AddFieldL(*newField);
    CleanupStack::Pop( newField );
    }

// -----------------------------------------------------------------------------
// PostcardUtils::ContactItemNameFromTag
// -----------------------------------------------------------------------------
const TPtrC TPostcardUtils::ContactItemNameFromTag( TInt aTag )
    {
    // Special format tags are following:
    //   0 = greeting text
    //   1 = name
    //   2 = address info
    //   3 = street
    //   4 = zip code
    //   5 = city
    //   6 = state
    //   7 = country

    static const TInt tagToControlId[ENumSpecFormatTags] = 
        {
	    EPostcardTextEditor, EPostcardAddressName,
	    EPostcardAddressInfo, EPostcardAddressStreet,
	    EPostcardAddressZip, EPostcardAddressCity,
	    EPostcardAddressState, EPostcardAddressCountry
        };

    __ASSERT_DEBUG( aTag < ENumSpecFormatTags, Panic( EPostcardPanicCoding ) );

    return ContactItemNameFromId( tagToControlId[aTag] );
    }

// -----------------------------------------------------------------------------
// PostcardUtils::NextTag
// -----------------------------------------------------------------------------
TInt TPostcardUtils::NextTag(const TDesC& aFormat, TInt& aTag)
    {
    // Find next tag from a special format string

    // We look for a tags like "%0U"
    _LIT(KTagFormat, "%0U");
    const TInt KTagLen = 3;
    TBuf<KTagLen> tag( KTagFormat );

    // Find first tag (minimum position) in the string
    TInt minPos = aFormat.Length();
    TInt i;
    for( i = 0; i < ENumSpecFormatTags; i++ )
        {
        TInt pos = aFormat.Find( tag );
        if ( pos != KErrNotFound && pos < minPos )
            {
            minPos = pos;
            aTag = i;
            }
        tag[1]++; // next tag to look for
        }
    return minPos < aFormat.Length() ? minPos : KErrNotFound;
    }
    
// ---------------------------------------------------------
// Panic
// Postcard Panic function
// ---------------------------------------------------------
void Panic( TPostcardPanic aPanic )
    {
    _LIT( KCategory, "Postcard" );
    User::Panic( KCategory, aPanic );
    }
