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
* Description:  IMSSoftkeyControl.h
*
*/


#ifndef CIMSSOFTKEYCONTROL_H
#define CIMSSOFTKEYCONTROL_H

// INCLUDES
#include <e32base.h>
#include <eikbtgpc.h>           // CEikButtonGroupContainer
#include <eikcmobs.h>           // MEikCommandObserver
#include <muiuflags.h>          // TMuiuFlags

// CONSTANTS
// MACROS
// DATA TYPES
enum TISCFlags
    {
    // Keep these flags and TISCObservedKeys in same order!
    EISCObservingLeftSoftkey = 0,
    EISCObservingUnknownKey,
    EISCObservingRightSoftkey,
    EISCObservingMiddleSoftkey,
    // Allow changing the middle soft key label
    EISCAllowMiddleSoftkeyLabelChange,
    EISCLastFlag
    };

enum TISCObservedKeys
    {
    // Keep these keys and related flags in same order!
    EISCLeftSoftkey = 0,
    EISCUnknownKey,
    EISCRightSoftkey,
    EISCMiddleSoftkey
    };

// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*
*  @lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CIMSSoftkeyControl ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Creates object from CIMSSoftkeyControl and leaves it to cleanup stack
        * @since S60 3.1
        * @return, Constructed object
        */
        static CIMSSoftkeyControl* NewLC(
            CEikButtonGroupContainer& aSoftkeys );

        /**
        * Create object from CIMSSoftkeyControl
        * @since S60 3.1
        * @return, Constructed object
        */
        static CIMSSoftkeyControl* NewL(
            CEikButtonGroupContainer& aSoftkeys );

        /**
        * Destructor
        * @since S60 3.1
        */
        virtual ~CIMSSoftkeyControl();

    public: // New operators

        /**
        * Get a direct access to softkeys
        * @since S60 3.1
        * @return Reference to softkey container
        */
        CEikButtonGroupContainer& operator()();

    public: // New functions


        /**
        * Sets the observer to the specific key
        * @since S60 3.1
        * @param aSoftkey Key to be observed
        * @param aCommandObserver The observer for the key events
        * @return KErrNone, when setting the observer is succesful
        */
        TInt SetObserver(
            const TISCObservedKeys aSoftkey,
            MEikCommandObserver& aCommandObserver );

        /**
        * Sets/Removes the observers for the softkeys
        * @since S60 3.1
        * @param aLeftSoftkey Set/Remove left softkey observer
        * @param aRightSoftkey Set/Remove right softkey observer
        * @param aMiddleSoftkey Set/Remove middle softkey observer
        * @param aCommandObserver The observer for the key events
        * @return KErrNone, when setting the observer is succesful
        */
        TInt SetObserver(
            const TBool aLeftSoftkey,
            const TBool aRightSoftkey,
            const TBool aMiddleSoftkey,
            MEikCommandObserver& aCommandObserver );

        /**
        * Removes the observer
        * @since S60 3.1
        * @param aSoftkey
        */
        void RemoveObserver(
            const TISCObservedKeys aSoftkey );

        /**
        * Defines a new softkey
        * @since S60 3.1
        * @param aLabelResource Resource of the text for the softkey label
        * @param aSoftkeyPos Position of the softkey
        * @param aSoftkeyCmd Command executed by pressing the softkey
        * @param aCommandObserver Observer to receive the events
        */
        void DefineCustomSoftkeyL(
            const TInt aLabelResource,
            const TISCObservedKeys aSoftkeyPos,
            const TInt aSoftkeyCmd,
            MEikCommandObserver* aCommandObserver );

        /**
        * Defines a new softkey
        * @since S60 3.1
        * @param aSoftkeyLabel Text for the softkey label
        * @param aSoftkeyPos Position of the softkey
        * @param aSoftkeyCmd Command executed by pressing the softkey
        * @param aCommandObserver Observer to receive the events
        */
        void DefineCustomSoftkeyL(
            const TDesC& aSoftkeyLabel,
            const TISCObservedKeys aSoftkeyPos,
            const TInt aSoftkeyCmd,
            MEikCommandObserver* aCommandObserver );

        /**
        * Checks the current status of MSK label updating availability
        * @since S60 3.1
        * @return ETrue, when MSK label change is allowed
        * @return EFalse, when MSK label change is not allowed
        */
        TBool MSKLabelling();

        /**
        * Sets the status of MSK label updating
        * @since S60 3.1
        * @param aNewState Allow/Disallow MSK label change
        */
        void SetMSKLabelling( const TBool aNewState );

        /**
        * Changes the label of the middle softkey
        * @since S60 3.1
        * @param aLabelResource Resource of the text for the softkey label
        * @param aCommandId Command executed by pressing the softkey
        * @param aCommandObserver Observer to receive the events
        */
        void UpdateMiddleSoftkeyLabelL(
            const TInt aLabelResource,
            const TInt aCommandId,
            MEikCommandObserver* aCommandObserver );

        /**
        * Hides softkeys. Preserves the possibility to show softkeys again,
        * functionality is to be coded
        * @since S60 3.2
        * @param aVisible tells if buttons should be showed
        * @param aCommandObserver Observer to receive the events
        */
        void SetVisibility(
            const TBool aVisible,
            MEikCommandObserver* aCommandObserver );

    public: // Functions from base classes

    protected:  // Constructors

        /**
        * Default constructor for classCIMSSoftkeyControl
        * @since S60 3.1
        * @return, Constructed object
        */
        CIMSSoftkeyControl(
            CEikButtonGroupContainer& aSoftkeys );

        /**
        * Symbian 2-phase constructor
        * @since S60 3.1
        */
        void ConstructL();

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions

        /**
        *
        * @since S60 3.1
        */
        TInt ChangeObserver(
            const TBool aOn,
            const TISCObservedKeys aSoftkey,
            MEikCommandObserver& aCommandObserver );

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        // Softkey container
        CEikButtonGroupContainer&   iSoftkeys;
        // Container for the flags
        TMuiuFlags                  iFlags;
        // Container for the last used resource for MSK
        TInt                        iPreviousMskLabelResource;

    };

#endif //  CIMSSOFTKEYCONTROL_H

// End of File

