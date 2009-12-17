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
*     Item class for remote folders. Encapsulates folder id and sort order of folder.
*
*/



#ifndef MCEFOLDERITEM_H
#define MCEFOLDERITEM_H


// CLASS DECLARATION

/**
*  Item class for remote folders; encapsulates folder id and sort order
*/
class TMceFolderItem
    {
    public:                     // Data        
        TMsvId                  iFolderId;        
        TInt                    iSortOrder;
        TBool                   iOrdering;
    };

#endif // MCEFOLDERITEM_H

// End of file
