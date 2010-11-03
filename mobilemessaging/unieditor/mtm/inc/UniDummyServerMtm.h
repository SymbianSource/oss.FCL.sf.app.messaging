/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Dummy Server Mtm
*
*/



#ifndef __UNIDUMMYSERVERMTM_H
#define __UNIDUMMYSERVERMTM_H

//  INCLUDES
#include <mtsr.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Uni Dummy Server Mtm 
*  This class exists only because of client server architecture. 
*  The class is dummy class for purpose, it does not do anything. 
*  All inherited functions are implemented as KErrNotSupported.
*/
class CUniDummyServerMtm : public CBaseServerMtm
    {
    public:  // Constructors and destructor
        // constructor is private!

        /**
        * Factory function.
        * The only function exported by this polymorphic interface dll.<br>
        * This function is called by message server when a client asks for
        *     some service from this mtm type
        * @param  aRegisteredMtmDll Reference to Mtm Dll registry class
        * @param  aInitialEntry Service entry. Can be the first entry in
        *     CMsvSelection array in the call from the client, or if
        *     there is no service entry as the first item of the selection,
        *     this will be the default service entry for this type of mtm.
        *     If no default entry has been specified, and the first entry
        *     in the selection is not a service entry, the creation of 
        *     server mtm will fail. 
        * @return Pointer to CUniDummyServerMtm class. 
        */
        IMPORT_C static CUniDummyServerMtm* NewL(
            CRegisteredMtmDll& aRegisteredMtmDll,
            CMsvServerEntry* aInitialEntry );

        /**
        * Destructor.
        */
        ~CUniDummyServerMtm();

    public: // Functions from base classes

        /**
        * From CBaseServerMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        void CopyToLocalL(
            const CMsvEntrySelection& aSelection,
            TMsvId aDestination,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        void CopyFromLocalL(
            const CMsvEntrySelection& aSelection,
            TMsvId aDestination,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        void CopyWithinServiceL(
            const CMsvEntrySelection& aSelection,
            TMsvId aDestination,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        void MoveToLocalL(
            const CMsvEntrySelection& aSelection,
            TMsvId aDestination,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        void MoveFromLocalL(
            const CMsvEntrySelection& aSelection,
            TMsvId aDestination,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        void MoveWithinServiceL(
            const CMsvEntrySelection& aSelection,
            TMsvId aDestination,
            TRequestStatus& aStatus );

        // Create, change, delete
        
        /**
        * From CBaseServerMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        void DeleteAllL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        void CreateL( TMsvEntry aNewEntry, TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        void ChangeL( TMsvEntry aNewEntry, TRequestStatus& aStatus );
        
        // command and progress

        /**
        * From CBaseServerMtm 
        * Not supported. 
        * @leave KErrNotSupported
        */
        void StartCommandL(
            CMsvEntrySelection& aSelection,
            TInt aCommand,
            const TDesC8& aParameter,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseServerMtm 
        * Not supported. 
        * Returns EFalse
        */
        TBool CommandExpected();
        
        /**
        * From CBaseServerMtm 
        * Not supported. 
        * Returns KNullDesc
        */
        const TDesC8& Progress();

        
    protected:  // Functions from base classes
        
        /**
        * From CActive: Cancel current operation.
        * The function is not supported.
        */
        void DoCancel();

        /**
        * From CBaseServerMtm: Do work.
        * The function is not supported.
        */
        void DoRunL();

        /**
        * From CBaseServerMtm: Complete current operation.
        * The function is not supported.
        * @param aError Error code to be returned as final status
        */
        void DoComplete( TInt aError );

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        CUniDummyServerMtm(
            CRegisteredMtmDll& aRegisteredMtmDll,
            CMsvServerEntry* aInitialEntry );

        // By default, prohibit copy constructor
        CUniDummyServerMtm( const CUniDummyServerMtm& );
        // Prohibit assigment operator
        CUniDummyServerMtm& operator= ( const CUniDummyServerMtm& );

    private:    // Data
    
    };

#endif      // __UNIDUMMYSERVERMTM_H
            
// End of File
