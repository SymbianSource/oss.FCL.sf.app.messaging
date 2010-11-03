/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: smiltime  declaration
*
*/



#ifndef SMILTIME_H
#define SMILTIME_H

//  INCLUDES
#include <e32base.h>
#include <e32math.h>
#include <e32std.h>

// FORWARD DECLARATIONS
class TInstanceTime;
class CSmilObject;

// CLASS DECLARATION

/**
 * Time class. Special time values INDEFINITE, UNRESOLVED and UNKNOWN are also possible.
 * Includes basic math operators.
 */
class TSmilTime
    {
    public:  // Constructors and destructor

		enum {
		// for style properties
			KUnspecified = KMaxTInt,
		// for calculated values
			KUnresolved = KMaxTInt,

			KIndefinite = KMaxTInt - 1,
			KMedia = KMaxTInt - 2
			};
		
		/**
        * C++ default constructor.
        */
        IMPORT_C TSmilTime();

		/**
        * C++ constructor with initialisation.
        */
		IMPORT_C TSmilTime( TInt aValue );

		IMPORT_C TSmilTime ( const TDesC& aValue );

    public: // New functions
		
		/**
        * Converts from global to local time 
        */
		IMPORT_C static TSmilTime ToLocal( CSmilObject* aObject, TSmilTime aTime );

		/**
        * Multiply operator (indef*def=indef, unres*def=unres)
        */
		IMPORT_C TSmilTime operator*( const TSmilTime& aA ) const; 
		
		/**
        * Multiply operator (indef*def=indef, unres*def=unres)
        */
		IMPORT_C TSmilTime operator*( const TReal32 aValue ) const; 

		/**
        * Substract operator (indef-def=indef, unres-def=unres, def-indef=indef,...)
        */
		IMPORT_C TSmilTime operator-( const TSmilTime& aA ) const; 

		/**
        * Sum operator (indef+def=indef, unres+def=unres)
        */
		IMPORT_C TSmilTime operator+( const TSmilTime& aA ) const; 

		/**
        * Comparison operator (unres>indef>def)
        */
		IMPORT_C TBool operator>( const TSmilTime& aA ) const; 

		/**
        * Comparison operator (def<indef<unres)
        */
		IMPORT_C TBool operator<( const TSmilTime& aA ) const; 

		/**
        * Assignment operator
        */
		IMPORT_C TSmilTime& operator=( const TInt aValue ); 

		/**
        * Assignment operator
        */
		IMPORT_C TSmilTime& operator=( const TSmilTime& aTime ); 

		/**
        * Comparison operator (indef!=unres)
        */
		IMPORT_C TBool operator==( const TSmilTime& aTime ) const; 

		/**
        * Comparison operator (indef!=unres)
        */
		IMPORT_C TBool operator!=( const TSmilTime& aTime ) const; 

		/**
        * Comparison operator 
        */
		IMPORT_C TBool operator>=( const TSmilTime& aTime ) const; 

		/**
        * Comparison operator 
        */
		IMPORT_C TBool operator<=( const TSmilTime& aTime ) const; 

		/**
		* Increment operator (indef++ == indef)
        */
		IMPORT_C const TSmilTime& operator++( TInt /*aX*/ ); 

		TBool IsFinite() const		{ return iValue < KIndefinite; }
		TBool IsUnresolved() const	{ return iValue == KUnresolved; }
		TBool IsUnspecified() const { return iValue == KUnspecified; }
		TBool IsIndefinite() const	{ return iValue == KIndefinite; }
		TBool IsMedia() const		{ return iValue == KMedia; }
		TInt Value() const			{ return iValue; }


		/**
		 * Converts (finite) time value to microseconds
		 */
		IMPORT_C TTimeIntervalMicroSeconds ToMicroSeconds() const;
		/**
		 * Creates time value from a microsecond value
		 */
		IMPORT_C static TSmilTime FromMicroSeconds(TTimeIntervalMicroSeconds aMicro);
		/**
		 * Creates time value from an integer value
		 */
		IMPORT_C static TSmilTime FromMicroSeconds(TInt aMicro);

    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @param ?arg1 ?description
        * @return ?description
        */
        // ?type ?member_function( ?type ?arg1 );
        
    protected:  // New functions
        
        /**
        * ?member_description.
        * @param ?arg1 ?description
        * @return ?description
        */

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:    // Data
		TInt			iValue;
	};

#endif      // ?INCLUDE_H   
            
// End of File
