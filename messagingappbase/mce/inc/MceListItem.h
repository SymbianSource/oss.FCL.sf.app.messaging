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
*     Item class for list array. Encapsulates message id, description of the item.
*
*/



#ifndef MCELISTITEM_H
#define MCELISTITEM_H

// CONSTANTS
const TInt  KMceListGranuality = 4;
const TInt  KMcePrintableTextLength = 150;
const TInt  KMceVisibleTextLength = 50; // max characters to show from iDescription and iDetails

// DATA TYPES
typedef TBuf<KMcePrintableTextLength> TMcePrintableText;

// CLASS DECLARATION

/**
*  Item class for list array. Encapsulates message id, description of the item.
*/
class TMceListItem
    {
    public:                     // Data
        TBool                   iExtraItem;
        TMsvId                  iMsvId;
        TMcePrintableText       iPrintableText;
        TInt                    iIconIndex;
        TMcePrintableText       iSecondaryText;
#ifdef RD_MSG_NAVIPANE_IMPROVEMENT
        TInt                    iMessageCount;
#endif // RD_MSG_NAVIPANE_IMPROVEMENT        
    };
    
    
class TMceMainViewExtraItem
    {
    public:                     // Data
        TInt                    iCommandId;
        TInt                    iIconIndex;
        TInt                    iApplicationUid;
        TMcePrintableText       iPrintableText;
        TMcePrintableText       iCommandText;
        TMcePrintableText       iSecondaryText;
    };
    
typedef CArrayFixFlat<TMceMainViewExtraItem> CMceMainViewExtraItemArray;

#endif // MCELISTITEM_H

// End of file
