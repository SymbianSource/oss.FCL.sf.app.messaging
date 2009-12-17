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
* Description:  
*  .
*
*/




// INCLUDE FILES
#include "CMsgDeliveryItem.h"


// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CMsgDeliveryItem::CMsgDeliveryItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgDeliveryItem::CMsgDeliveryItem()
    {
    }

// -----------------------------------------------------------------------------
// CMsgDeliveryItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMsgDeliveryItem::ConstructL()
    {
    iNumber = HBufC::NewL(0);
    iRecipient = HBufC::NewL(0);
    }

// -----------------------------------------------------------------------------
// CMsgDeliveryItem::CMsgDeliveryItem
// Destructor
// -----------------------------------------------------------------------------
// 
CMsgDeliveryItem::~CMsgDeliveryItem()
    {
    delete iRecipient;    
    delete iNumber;
    }

// -----------------------------------------------------------------------------
// CMmsMTMLogData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMsgDeliveryItem* CMsgDeliveryItem::NewL()
    {
    CMsgDeliveryItem* self = new( ELeave ) CMsgDeliveryItem( );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

 // ---------------------------------------------------------
// CMsgDeliveryItem::GetTime
// ---------------------------------------------------------
//  
TTime CMsgDeliveryItem::GetTime()
    {
    return iTime;
    }
 // ---------------------------------------------------------
// CMsgDeliveryItem::SetTime
// ---------------------------------------------------------
//  
void CMsgDeliveryItem::SetTime(const TTime aTime)
    {
    iTime = aTime;
    }

 // ---------------------------------------------------------
// CMsgDeliveryItem::GetNumber
// ---------------------------------------------------------
//  
TDesC& CMsgDeliveryItem::GetNumber()
    {
    return *iNumber;
    }

 // ---------------------------------------------------------
// CMsgDeliveryItem::SetNumberL
// ---------------------------------------------------------
//  
void CMsgDeliveryItem::SetNumberL(const TDesC& aText)
    {
    delete iNumber;
    iNumber = NULL;
    iNumber = aText.AllocL();
    }

 // ---------------------------------------------------------
// CMsgDeliveryItem::GetRecipient
// ---------------------------------------------------------
//  
TDesC& CMsgDeliveryItem::GetRecipient()
    {
    return *iRecipient;
    }

 // ---------------------------------------------------------
// CMsgDeliveryItem::SetRecipientL
// ---------------------------------------------------------
//  
void CMsgDeliveryItem::SetRecipientL(const TDesC& aText)
    {
    delete iRecipient;
    iRecipient = NULL;
    iRecipient = aText.AllocL();
    }

 // ---------------------------------------------------------
// CMsgDeliveryItem::GetStatus
// ---------------------------------------------------------
//  
TInt CMsgDeliveryItem::GetStatus()
    {
    return iStatus;
    }

 // ---------------------------------------------------------
// CMsgDeliveryItem::SetStatus
// ---------------------------------------------------------
//  
void CMsgDeliveryItem::SetStatus(const TMmsStatus aStatus)
    {
    iStatus = aStatus;
    }

// End of File
