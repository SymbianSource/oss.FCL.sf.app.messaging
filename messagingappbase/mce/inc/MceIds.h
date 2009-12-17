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
*     Constants for Mce
*
*/




#ifndef _MCEIDS_H
#define _MCEIDS_H

#include <msvstd.h> // TMsvId
#include "MceIds.hrh"


const TMsvId KMceDocumentsEntryId = KMceDocumentsEntryIdValue;
const TMsvId KMceTemplatesEntryId = KMceTemplatesEntryIdValue;

const TMsvId KMceWriteMessageId = KMceWriteMessageListIdValue;
const TMsvId KMceDeliveryReportsId = KMceDeliveryReportsListIdValue;
const TMsvId KMceNoMailboxesListId = KMceNoMailboxesListIdValue;
const TMsvId KMceFirstExtraItemId = KMceNoMailboxesListIdValue+1;

const TInt KMceHideInBackground = KMceHideInBackgroundValue;

const TUid KMceMainViewListViewId   = {KMceMainViewListViewIdValue};
const TUid KMceMessageListViewId    = {KMceMessageListViewIdValue};
const TUid KMceDeliveryReportViewId = {KMceDeliveryReportViewIdValue};

const TUid KMceApplicationUid       = {KMceApplicationUidValue};

const TUid KMceMtmMailForExchangeUid = { KMceMtmMailForExchangeValue };


#endif // _MCEIDS_H

// End of File
