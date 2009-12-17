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
*       Provides MMS access point selection dialog
*
*/



// INCLUDE FILES
#include <e32base.h>

#include <ApSelect.h>
#include <ApEngineConsts.h>
#include <ApSettingsHandlerUI.h>
#include <ApSettingsHandlerCommons.h>

#include <mmsapselect.h>

// -----------------------------------------------------------------------------
// MmsApSelect::SelectMmsAccessPointL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt MmsApSelect::SelectMmsAccessPointL(
        CCommsDatabase& aCommsDb,
        TUint32 aCurrent,
        TUint32& aSelected,
        TInt aBearerFilter )
    {
     TInt taskDone = 0; 
    //Check if there are any APs or not.
    CApSelect* apSelect = CApSelect::NewLC(
        aCommsDb,
        KEApIspTypeMMSMandatory,
        aBearerFilter,
        KEApSortNameAscending );

    TSelectionListType popupType = apSelect->Count() ?
        EApSettingsSelListIsPopUpWithNone :
        EApSettingsSelListIsPopUp;
    
    CleanupStack::PopAndDestroy( apSelect );
    
    CApSettingsHandler* apHandler = CApSettingsHandler::NewLC(
        ETrue,          // selecting, not editing
        popupType,
        EApSettingsSelMenuSelectNormal,
        KEApIspTypeMMSMandatory,
        aBearerFilter,
        KEApSortNameAscending ); // Not UI specified, but we can live with this.

    aSelected = KMaxTUint32; // invalid value
    TRAPD(err,taskDone = apHandler->RunSettingsL( aCurrent, aSelected ))
    CleanupStack::PopAndDestroy( apHandler );
	User::LeaveIfError(err);
    return taskDone;
    }

//  End of File
