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
* Description:   Collection of utility functions used by Postcard class
*
*/



#ifndef POSTCARDUTILS_H
#define POSTCARDUTILS_H

// INCLUDES

// FORWARD DECLARATIONS
class CContactItemFieldSet;
class CContactItemField;
class CContactCard;

// CLASS DECLARATION

/**
* Collection of static utility functions used by Postcard classes
*
* @lib postcard.exe
* @since 3.0
*/
class TPostcardUtils
    {
    public: // Constants
        enum
            {
            // There are 8 different tags (%0U...%7U) that may be embedded into
            // special format string. 6 address fields + name + greeting.
            ENumSpecFormatTags = 8
            };

    public: // Methods

        /**
        * Returns the label of the field aId.
        * This is used innerly in Postcard application to
        * distinguish fields from each other in CContactItem object.
        */
	    static const TPtrC ContactItemNameFromId( TInt aId );

        /**
        * Returns the id of the label aLabel.
        * This is used innerly in Postcard application to
        * distinguish fields from each other in CContactItem oject.
        */
        static TInt IdFromContactItemName( const TDesC& aLabel );

        /**
        * Converts a label to a contact item field
        */
        static const CContactItemField* FieldOrNull(
            const CContactItemFieldSet& aSet, const TDesC& aLabel );
        /**
        * Adds a field to a contact
        */
        static void AddContactFieldL( CContactCard& aCard, const TDesC& aLabel,
            const TDesC& aFieldText );
        /**
        * Converts special format string tag to contact item name
        */
        static const TPtrC ContactItemNameFromTag( TInt aTag );
        /**
        * Finds next special format tag from a string
        */
        static TInt NextTag(const TDesC& aFormat, TInt& aTag);
	};

#endif // POSTCARDUTILS_H

// End of file
