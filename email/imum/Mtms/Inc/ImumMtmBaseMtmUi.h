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
* Description: ImumMtmBaseMtmUi.h
*
*/


#ifndef CIMUMMTMBASEMTMUI_H
#define CIMUMMTMBASEMTMUI_H

// INCLUDES
#include <e32base.h>
#include <ImumInternalApi.h>    // CImumInternalApi
#include <ImumInMailboxUtilities.h>
#include <mtmuibas.h>           // CBaseMtmUi
#include <ErrorUI.h>            // CErrorUI
#include <muiuflagger.h>        // CMuiuFlags

#include "ImumMtmBaseMtmUi.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*
*  @lib
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CImumMtmBaseMtmUi ) : public CBaseMtmUi
    {
    public:  // Constructors and destructor

        /**
        * Destructor
        * @since S60 3.2
        */
        virtual ~CImumMtmBaseMtmUi();

    public: // New functions
    public: // Functions from base classes

        /**
         *
         *
         * @since S60 v3.2
         */
        CMsvOperation* DeleteServiceL(
            const TMsvEntry& aService,
            TRequestStatus& aStatus );

    protected:  // Constructors

        /**
        * Default constructor for classCImumMtmBaseMtmUi
        * @since S60 3.2
        * @return, Constructed object
        */
        CImumMtmBaseMtmUi(
            CBaseMtm& aBaseMtm,
            CRegisteredMtmDll& aRegisteredMtmDll );

        /**
        * Symbian 2-phase constructor
        * @since S60 3.2
        */
        void ConstructL();

    protected:  // New virtual functions

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TBool AcknowledgeReceiptsL(
            const TMsvId aMessageId,
            const MImumInMailboxUtilities::TImumInMboxRequest& aRequestType =
                MImumInMailboxUtilities::ERequestReceiving ) const;

        /**
         * Returns mail deletion mode from central repository, if repository
         * is unable to read retruns EIMASMailDeletionAlwaysAsk
         *
         * @since S60 3.2
         * @return TIMASMailDeletionMode
         */
        TInt GetMailDeletionModeL() const;

    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions
    private:  // Functions from base classes

    public:     // Data
    protected:  // Data

        // Own: Internal API
        CImumInternalApi*           iMailboxApi;
        // Shortcut to utilities
        MImumInMailboxUtilities*    iUtils;
        // Message server session
        CMsvSession*                iMsvSession;
        // Own: Error UI
        CErrorUI*                   iErrorResolver;
        // Own: Flags
        CMuiuFlags*                 iFeatureFlags;

    private:    // Data
    };

#endif //  CIMUMMTMBASEMTMUI_H

// End of File
