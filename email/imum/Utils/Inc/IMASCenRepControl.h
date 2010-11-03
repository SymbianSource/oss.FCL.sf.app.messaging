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
* Description:  IMASCenRepControl.h
*
*/


#ifndef CIMASCENREPCONTROL_H
#define CIMASCENREPCONTROL_H

// INCLUDES
#include <e32base.h>

#include "IMASAccountControl.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION

/**
*
*  @lib
*  @since S60 3.0
*/
class CIMASCenRepControl : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Creates object from CIMASCenRepControl and leaves it to cleanup stack
        * @since S60 3.0
        * @return, Constructed object
        */
        static CIMASCenRepControl* NewLC();

        /**
        * Create object from CIMASCenRepControl
        * @since S60 3.0
        * @return, Constructed object
        */
        static CIMASCenRepControl* NewL();

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CIMASCenRepControl();

    public: // New functions

        /**
        *
        * @since S60 3.0
        */
        void SetActiveCentralRepository(
            CRepository& aRepository );

        /**
        *
        * @since S60 3.0
        */
        void SetActiveBaseKey(
            const TUint32 aAccountId,
            const TUid& aProtocol );

        /**
        *
        * @since S60 3.0
        */
        void SetActiveBaseKey(
            const TUint32 aExtendedAccountId );

        /**
        *
        * @since S60 3.0
        */
        void SetActiveDefaultDataKey();

        /**
        *
        * @since S60 3.0
        */
        void CreateToAccountL(
            const TUint32 aSetting,
            const TDesC& aString );

        /**
        *
        * @since S60 3.0
        */
        void CreateToAccountL(
            const TUint32 aSetting,
            const TInt aValue );

        /**
        *
        * @since S60 3.0
        */
        void CreateToAccountL(
            const TInt aSettingHigh,
            const TInt aSettingLow,
            const TInt64 aValue );

        /**
        *
        * @since S60 3.0
        */
        void DeleteFromAccountL(
            const TInt aSetting ) const;

        /**
        *
        * @since S60 3.0
        */
        void DeleteFromAccountL(
            const TInt aSettingHigh,
            const TInt aSettingLow ) const;

        /**
        *
        * @since S60 3.0
        */
        void SetToAccountL(
            const TUint32 aSetting,
            const TDesC& aString );
        /**
        *
        * @since S60 3.0
        */
        void SetToAccountL(
            const TUint32 aSetting,
            const TInt aValue );

        /**
        *
        * @since S60 3.0
        */
        void SetToAccountL(
            const TInt aSettingHigh,
            const TInt aSettingLow,
            const TInt64 aSetting );

        /**
        *
        * @since S60 3.0
        */
        TInt64 GetFromAccountL(
            const TInt aSettingHigh,
            const TInt aSettingLow ) const;

        /**
        *
        * @since S60 3.0
        */
        TInt GetFromAccountL(
            const TInt aSetting ) const;

        /**
        *
        * @since S60 3.0
        */
        void GetTextFromAccountL(
            TDes& aText,
            const TInt aSetting ) const;

        /**
        * Get 8-bit text from central repository
        * @since S60 3.2
        * @param cent-rep key for value, reference for modifiable descriptor
        * @return system wide error code
        */

        void GetTextFromAccountL(
            TDes8& aText,
            const TInt aSetting ) const;

        // non-leaving versions of GetFromAccounts

        /**
        * Get one 64-bit value from central repository
        * @since S60 3.2
        * @param lower and higher part keys of 64-bit values,
        *        reference to TInt64 value
        * @return system wide error code
        */

        TInt GetFromAccount(
            const TInt aSettingHigh,
            const TInt aSettingLow,
            TInt64& aSettingValue ) const;

        /**
        * Get one 32-bit value from central repository
        * @since S60 3.2
        * @param cent-rep key for value, reference to value
        * @return system wide error code
        */
        TInt GetFromAccount(
            const TInt aSetting, TInt& aSettingValue ) const;

        /**
        * Get text from central repository
        * @since S60 3.2
        * @param cent-rep key for value, reference for modifiable descriptor
        * @return system wide error code
        */
        TInt GetTextFromAccount(
            TDes& aText,
            const TInt aSetting ) const;

        /**
        * Get 8-bit text from central repository
        * @since S60 3.2
        * @param cent-rep key for value, reference for modifiable descriptor
        * @return system wide error code
        */
        TInt GetTextFromAccount(
            TDes8& aText,
            const TInt aSetting ) const;

        /**
        * Get one 32-bit value from central repository. Creates the settings
        * with default value if it does not exist. Will leave with generic 
        * Symbian error code if creation fails or unexpected error is 
        * encountered when reading setting.
        * 
        * @param aSetting CenRep key for value.
        * @param aDefaultValue Default value for settings.
        * @return Value from CenRep.
        */
        TInt GetFromAccountOrCreateL(
            const TUint32 aSetting, 
            TInt aDefaultValue );

        /**
        * Get one 64-bit value from central repository. Creates the settings
        * with default value if it does not exist. Will leave with generic 
        * Symbian error code if creation fails or unexpected error is 
        * encountered when reading setting.
        * 
        * @param aSettingHigh CenRep key for high 32bit value.
        * @param aSettingLow  CenRep key for low 32bit value.
        * @param aDefaultValue Default value for settings.
        * @return Value from CenRep.
        */
        TInt64 GetFromAccountOrCreateL(
            const TUint32 aSettingHigh,
            const TUint32 aSettingLow,
            TInt64 aDefaultValue );
        
    public: // Functions from base classes

    protected:  // Constructors

        /**
        * Default constructor for classCIMASCenRepControl
        * @since S60 3.0
        * @return, Constructed object
        */
        CIMASCenRepControl();

        /**
        * Symbian 2-phase constructor
        * @since S60 3.0
        */
        void ConstructL();

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions
    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        // Pointer to active Central Repository
        CRepository* iCenRep;
        // Currently active account
        TUint32 iBaseKey;
    };

#endif //  CIMASCENREPCONTROL_H

// End of File
