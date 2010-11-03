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
* Description:  Base email API factory
*
*/


#ifndef CIMUMINTERNALAPI_H
#define CIMUMINTERNALAPI_H

// INCLUDES
#include <e32base.h>

// Temporary
#include <ImumInMailboxServices.h>
#include <ImumInMailboxUtilities.h>
#include <ImumInHealthServices.h>

// CONSTANTS
// MACROS
// FORWARD DECLARATIONS
class CImumInternalApi;
class CMsvSession;
class MImumInMailboxServices;
class MImumInHealthServices;
class MImumInMailboxUtilities;

// LOCAL FUNCTION PROTOTYPES
IMPORT_C CImumInternalApi* CreateEmailApiL( CMsvSession* aMsvSession = NULL );
IMPORT_C CImumInternalApi* CreateEmailApiLC( CMsvSession* aMsvSession = NULL );

// @deprecated
typedef CImumInternalApi CIMAEmailApi;

// DATA TYPES

/**
* Client side center point of email API
*
*
* USAGE:
*   Create object to email API by calling either CreateEmailApiL or
*   CreateEmailApiLC. It's recommended, that the client provides the
*   session to Message Server. For example:
*       CImumInternalApi* emailApi = CreateEmailApiL( iMsvSession );
*
*   When finishing the usage of the API, simply delete the EmailApi object.
*   For example:
*       delete emailApi;
*       emailApi = NULL;
*
* @lib ImumUtils.lib
* @since S60 3.0
*/

class CImumInternalApi : public CBase
{
    public:

        enum TEmailFeatures
            {
            // Used when asking TARM status
            EMailLockedSettings = 0
            };

        virtual CMsvSession& MsvSession() = 0;
    
        /**
        * Interface to handle information regarding the healthiness of the mailboxes
        * @since S60 v3.2
        * @return Mailbox Doctor Interface
        */
        virtual const MImumInHealthServices& HealthServicesL() = 0;
        
        // @deprecated
        // For temporary use only
        virtual const MImumInHealthServices& HealthApiL() = 0;
                
        virtual const MImumInMailboxServices& MailboxServicesL() = 0;
        
        virtual const MImumInMailboxUtilities& MailboxUtilitiesL() = 0;       
        
        /**
         * Function for checking mailbox flags and show information note. 
         * If resource parameters are set blank no note will be displayed.
         * @since S60 v3.2
         * @param Flag from the list TEmailFeatures.
         * @param aFeatureOnResource resource to be displayed if feature is on.
         * @param aFeatureOffResource resource to be displayed if feature is off.
         * @return ETrue if asked flag is active.
         */
        virtual TBool IsEmailFeatureSupportedL( 
            const TEmailFeatures aFlag ,
            const TUint aFeatureOnResource = 0,
            const TUint aFeatureOffResource = 0 ) = 0;         
};



#endif /* CIMUMINTERNALAPI_H */
