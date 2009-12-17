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
*     Phonenumber matching wrapper for contact model MatchPhoneNumberL
*
*/



#ifndef __Logs_Engine_CPhoneNumberMatcher_H__
#define __Logs_Engine_CPhoneNumberMatcher_H__

#include <e32base.h>

// DATA TYPES

// FORWARD DECLARATIONS
class CContactMatcher;
class CContactIdArray;

// CLASS DECLARATION

class CPhoneNumberMatcher : public CBase

    {
    public:     // Constructors.
        /**
         * Standard creation function. Creates and returns a new object of this
         * class.
         *
         * @return Pointer to the new instance of this class.
         */
        IMPORT_C static CPhoneNumberMatcher* NewL( 
            RFs& aFsSession );
                                
        /**
         * Destructor.
         */
        IMPORT_C ~CPhoneNumberMatcher();


    private: 
        /**
         * Constructor, second phase.
         */
        void ConstructL( RFs& aFsSession );

        /**
         * Constructor
         *
         */
        CPhoneNumberMatcher(); 

    public: 
        /**
         * Match count, Series 60 specific matching rules.
         *
         * @return count of matches or KErrNone if number size is 0.
         */
        IMPORT_C TInt MatchCountL( const TDesC& aNumber );

        /**
         * Get unique match from the phonebook if MatchCountL returned 1 
         *
         * @return result name or null string.
         */
        IMPORT_C TPtrC GetUniqueName();

        /**
         * Field id of unique contact.
         *
         * @return field id of matched contact field or KErrNotFound
         */
        IMPORT_C TInt GetFieldId();

        /**
         * Contact id of found contact
         *
         * @aContactId set value if unique contact id is found.
         * @return KErrNone if match found or KErrNotFound.
         */
        IMPORT_C TInt GetContactId(TContactItemId& aContactId);
        
        
        /**
         * Uses async function from CContactMatcher to open match stores.
         */
        void OpenDefaultMatchStoresL( TRequestStatus& aStatus );
        
        /**
         * Cancel operation
         */
        void CancelOperation();

    private: // data

        /// 
        CContactMatcher* iContactMatcher;

        /// The amount of digits to be used in contact matching
        TInt iMatchDigitCount;

        /// Own: Name used in DB update
        HBufC* iName;

        /// Own: Contact field id 
        TInt iFieldId;
        
        /// Own: contact id
        TContactItemId iContactId;
    };

#endif // __Logs_Engine_CPhoneNumberMatcher_H__

// End of File
