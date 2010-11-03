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
* Description: 
*       Class declaration file
*
*/

#ifndef __EMAILPRECREATION_H__
#define __EMAILPRECREATION_H__

#include "ImumOnlineOperation.h"
#include "SelectionOperationBase.h"
#include <smtpset.h>
#include <ImumInternalApi.h>
class CMsvEmailConnectionProgressProvider;
class CMtmStore;

/**
* CEmailPreCreation
* This operation encapsulates an optimisation that, rather than forcing the
* editor to create a new message every time, we create one invisible blank
* message for each email account in the drafts folder, and tell the editor to
* edit this message.
*/
class CEmailPreCreation : public CBase
    {
    public:
        /**
        * NewL
        * @param CMsvSession&, session reference
        * @return CEmailPreCreation*, self pointer
        */
        static CEmailPreCreation* NewL( CImumInternalApi& aMailboxApi );

        /**
        * ~CEmailPreCreation
        * Destructior
        */
        virtual ~CEmailPreCreation();

    public:
        /**
        * PreCreateBlankEmailL
        * @param TRequestStatus&, request status
        * @param TMsvId, service id
        * @return CMsvOperation*, operation
        */
        CMsvOperation* PreCreateBlankEmailL(
            TMsvId aServiceId,
            TRequestStatus& aStatus );

        /**
        * DeleteAllPreCreatedEmailsL (synchronous)
        * @param TMsvId, service id
        */
        void DeleteAllPreCreatedEmailsL(
            TMsvId aServiceId );

        /**
        * DeleteAllPreCreatedEmailsL
        * Returns NULL if none found.
        * @param TRequestStatus&, request status
        * @param TMsvId, service id
        * @return CMsvOperation*, operation
        */
        CMsvOperation* DeleteAllPreCreatedEmailsL(
            TMsvId aServiceId,
            TRequestStatus& aStatus );
        
        /**
        * FindPreCreatedEmailL
        * @param TMsvId, service id
        * @param TMsvId, exclude id
        * @return TMsvId, id of found email
        */
        TMsvId FindPreCreatedEmailL(
            TMsvId aServiceId,
            TMsvId aExclude );

    private:
        /**
        * ConstructL
        * @param CMsvSession&, session reference
        */
        void ConstructL();
            
        /**
         * 
         * @since S60 3.2
         */
        CEmailPreCreation( CImumInternalApi& aMailboxApi );

        /**
        * ServiceUsesMHTMLEncodingL
        * @param TMsvId, service id
        * @return TBool, ETrue if uses HTML encoding
        */
        TBool ServiceUsesMHTMLEncodingL( TMsvId aServiceId );

        /**
        * DeleteEmailWithoutServiceL
        * Deletes all the invisible mail entries which service cannot be found
        */
        void DeleteEmailWithoutServiceL();

    private: // Data
        CImumInternalApi& iMailboxApi;
        CMsvEntry* iEntry;
    };



#endif
