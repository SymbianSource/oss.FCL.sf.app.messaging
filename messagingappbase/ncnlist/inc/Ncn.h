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
* Description:   The header file for Ncn subsystem base class.
*
*/



#ifndef NCN_H
#define NCN_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CNcnModelBase;

// CLASS DECLARATION

/**
*  The class acting as a server framework for the NCN subsystem.
*/
class CNcn : public CPolicyServer
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static void NewLC();
        
        ~CNcn();
        
    public: // New functions

	    static TInt RunServer();
	
    protected: // From CServer2
    
        CSession2* NewSessionL( const TVersion& aVersion,
                                const RMessage2& aMessage ) const;
    
    protected: // New functions
    
	    static void RunServerL();

    private: // Constructors and destructor
    
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CNcn( TInt aPriority, const TPolicy& aPolicy );

    private: // Data

        // A pointer to CNcnModel instance. Owned.
        CNcnModelBase* iNcnModel;
    };

#endif   // NCN_H

// End of File
