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
* Description:  ImumDiskSpaceObserver.h
*
*/


#ifndef CIMUMDISKSPACEOBSERVER_H
#define CIMUMDISKSPACEOBSERVER_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>        // RFs

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION


/**
*
*  @lib
*  @since S60 3.0
*/
class MImumDiskSpaceObserver
    {
    // New virtual functions
    public:
        virtual void HandleDiskSpaceEvent() = 0;
    };

/**
*
*  @lib
*  @since S60 3.0
*/
class CImumDiskSpaceObserver : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Creates object from CImumDiskSpaceObserver and leaves it to cleanup stack
        * @since S60 3.0
        * @return, Constructed object
        */
        static CImumDiskSpaceObserver* NewLC(
            MImumDiskSpaceObserver& aObserver,
            RFs& aFileSession );

        /**
        * Create object from CImumDiskSpaceObserver
        * @since S60 3.0
        * @return, Constructed object
        */
        static CImumDiskSpaceObserver* NewL(
            MImumDiskSpaceObserver& aObserver,
            RFs& aFileSession );

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CImumDiskSpaceObserver();

    public: // New functions

        /**
        *
        * @since S60 3.0
        */
        void NotifyDiskSpace( const TInt64 aThreshold, const TInt aDrive );

    public: // Functions from base classes

    protected:  // Constructors

        /**
        * Default constructor for classCImumDiskSpaceObserver
        * @since S60 3.0
        * @return, Constructed object
        */
        CImumDiskSpaceObserver(
            MImumDiskSpaceObserver& aObserver,
            RFs& aFileSession );

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

        /**
        *
        * @since S60 3.0
        */
        virtual void DoCancel();

        /**
        *
        * @since S60 3.0
        */
        virtual void RunL();

    public:     // Data
    protected:  // Data
    private:    // Data

        // Observer class to observer the disk space events
        MImumDiskSpaceObserver& iDiskObserver;
        // File session
        RFs&                    iFileSession;
    };

#endif //  CIMUMDISKSPACEOBSERVER_H

// End of File
