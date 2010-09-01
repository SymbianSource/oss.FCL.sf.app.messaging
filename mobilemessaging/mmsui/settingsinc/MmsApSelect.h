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
*           MMS access point selection dialog
*
*/



#ifndef MMSAPSELECT_H
#define MMSAPSELECT_H

//  INCLUDES

#include <e32def.h>
#include <ApEngineConsts.h> // for bearer filter values
#include <ApSettingsHandlerCommons.h> // for return values

// FORWARD DECLARATIONS

class CCommsDatabase;

// CLASS DECLARATIONS

/**
* Class MmsApSelect
* Provides static method for showing MMS Access Point selection
* dialog.
*
* @since 3.1
*/
class MmsApSelect
    {
    public:

        /**
        * Show MMS specific access point selection dialog.
        *
        * @param aCommsDb Reference to CommsDB (should be of type
        *                 "EDatabaseTypeIAP"
        * @param aCurrent The id of the AP to highlight by default
        * @param aSelected The id of the access point selected 
        *                  "KMaxTUint32" if nothing was selected
        * @param aBearerFilter Filtering criteria on bearer type,
        *                      see <ApEngineConsts.h>.
        * @return Value returned by CApSettingsHandler::RunSettingsL(...),
        *               defined in <ApSettingsHandlerCommons.h>,
        *               see also <ApSettingsHandlerUi.h>.
        */
        IMPORT_C static TInt SelectMmsAccessPointL(
            CCommsDatabase& aCommsDb,
            TUint32 aCurrent,
            TUint32& aSelected,
            TInt aBearerFilter );

    };

#endif    // MMSAPSELECT_H
            
// End of File
