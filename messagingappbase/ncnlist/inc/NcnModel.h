/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines concrete non-CDMA class CNcnModel.
*
*/


#ifndef NCNMODEL_H
#define NCNMODEL_H

// INCLUDES
#include    "NcnModelBase.h"
#include    "MNcnUI.h"
#include    "MNcnNotifier.h"
#include	<msatrefreshobserver.h>

// CLASS DECLARATION
class RSatSession;
class RSatRefresh;

/**
*  The main Ncn subsystem class
*/
class CNcnModel : public CNcnModelBase, public MSatRefreshObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNcnModelBase* NewL();

        /**
        * Destructor.
        */
        virtual ~CNcnModel();

    private:

        /**
        * C++ default constructor.
        */
        CNcnModel();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    private: //From base class

        /**
        * Create Central repository connection
        * @since Series60 3.0
        */
        void CreateCRConnectionL();     

    public: // From base class

        /**
        * From CNcnModelBase. Sim changed status is given by this method
        * @since Series60 2.6
        */
        void SetSimChanged( const TBool aSimChanged );

        /**
        * From CNcnModelBase. Sets the current SMS initialisation phase.
        * @since Series60 2.6
        */
        void SetSmsInitialisationState( const TInt aSmsInitialisationPhase );

        /**
        * From CNcnModelBase. This method performs the SMSC fetching operation.
        * @since Series60 2.6
        */
        void PerformSimServiceCentreFetching();

        /**
        * From CNcnModelBase. This method forces the SMSC fetching operation.
        * @since Series60 2.6
        */
        void ForceToPerformSimServiceCentreFetching();

        /**
        * From CNcnModelBase. This method checks if the SMSC fetching is needed
        * @since Series60 2.6
        */
        void CheckIfSimSCOperationRequired();
		
    public: //From MSatRefreshObserver
    
        /**
        * Refresh query. Client should determine whether it allow the
        * refresh to happen. In this method the client should not close or
        * cancel the RSatRefresh subsession.
        * @since 3.0
        * @param aType Refresh type.
        * @param aFiles Elementary files which are to be changed. 
        * May be zero length. 
        * @return ETrue to allow refresh, EFalse to decline refresh.
        */
        TBool AllowRefresh( TSatRefreshType aType, const TSatRefreshFiles& aFiles );

         /**
        * Notification of refresh. In this method the client should not close 
        * or cancel the RSatRefresh subsession.
        * @since 3.0
        * @param aType Type of refresh which has happened.
        * @param aFiles List of elementary files which has been changed. 
        * May be zero length.
        */
        void Refresh( TSatRefreshType aType, const TSatRefreshFiles& aFiles );
        
    private: // Data
        
        /**
         * Enumeration for SMS initialisation phases.
         */
        enum TSMSInitialisationPhase
            {
            ESMSInitNone = 0,
            ESMSInitInProgress = 1,
            ESMSInitComplete = 2,
            ESMSInitReadServiceCentre = 3            
            };
            
       	// Sat session connection.
		RSatSession* iSatSession;
		
		// Sat refresh connection
		RSatRefresh* iSatRefresh;
    };

#endif      // NCNMODEL_H

// End of File
