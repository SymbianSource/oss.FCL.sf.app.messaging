/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CMmsMtmDeliveryDialogLauncher implementation
*
*/




// INCLUDE FILES

#include <mmsmsventry.h>
#include <mmsclient.h>
#include "CMmsMtmDeliveryDialogLauncher.h"
#include "MmsMtmDeliveryPopup.h"
#include "CMsgDeliveryItem.h"
#include "MMSMTMLogData.h"

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================





// -----------------------------------------------------------------------------
// CMmsMtmDeliveryDialogLauncher::CMmsMtmDeliveryDialogLauncher
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsMtmDeliveryDialogLauncher::CMmsMtmDeliveryDialogLauncher() :
    iLogData( NULL ),
    iDialog( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CMmsMtmDeliveryDialogLauncher::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMmsMtmDeliveryDialogLauncher* CMmsMtmDeliveryDialogLauncher::NewL(TMmsMsvEntry aEntry, CMmsClientMtm* aMmsClient)
    {
    CMmsMtmDeliveryDialogLauncher* self = new ( ELeave ) CMmsMtmDeliveryDialogLauncher( );
    
    CleanupStack::PushL( self );
    self->ConstructL(aEntry, aMmsClient);
    CleanupStack::Pop( self ); //self

    return self;
    }

// -----------------------------------------------------------------------------
// CMmsMtmDeliveryDialogLauncher::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsMtmDeliveryDialogLauncher::ConstructL(TMmsMsvEntry aEntry, CMmsClientMtm* aMmsClient)
    {
    iEntry = aEntry;
    iMmsClient = aMmsClient;
    iDialog = CMsgInfoDeliveryStatusPopup::NewL();
    }

// -----------------------------------------------------------------------------
// CMmsMtmDeliveryDialogLauncher::~CMmsMtmDeliveryDialogLauncher
// Destructor
// -----------------------------------------------------------------------------
// 
CMmsMtmDeliveryDialogLauncher::~CMmsMtmDeliveryDialogLauncher()
    {
    if (iLogData)
        {
        delete iLogData;
        }
    if (iDialog)
        {
        delete iDialog;
        }
    }

// -----------------------------------------------------------------------------
// CMmsMtmDeliveryDialogLauncher::SetDataArray
// "Callback" method, which the CMmsLogData calls after retrieving the data.
// Derived from the MMmsMtmDeliveryDataContainer
// -----------------------------------------------------------------------------
// 
void CMmsMtmDeliveryDialogLauncher::SetDataArrayL(CArrayPtrFlat< CMsgDeliveryItem >* aDataArray )
    {
    iDialog->SetDataArray(aDataArray);
    /*TInt answer = */iDialog->ExecuteL(); // Commented answer out as it's not used
    }
	
// -----------------------------------------------------------------------------
// CMmsMtmDeliveryDialogLauncher::StartDialogL
// Exported method, used to start the CMmsMTMLogData active object
// -----------------------------------------------------------------------------
// 
EXPORT_C TBool CMmsMtmDeliveryDialogLauncher::StartDialogL()
    {
    TBool success(EFalse);

    iLogData = CMmsMTMLogData::NewL(this, iEntry);
    success = iLogData->DeliveryDataL(iMmsClient);

    return success;
    }
//  End of File
