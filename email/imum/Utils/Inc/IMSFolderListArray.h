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
*       folder subscription dialog
*
*/


#ifndef IMSFOLDERLISTARRAY_H
#define IMSFOLDERLISTARRAY_H

// INCLUDE FILES
#include <e32base.h>
#include <msvstd.h>
#include <msvapi.h>
#include <bamdesca.h>

#include "IMSFolderListArray.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

/**
* class CImapFolderListArray
* Array class to be used by CImapSubscriptionDialog
*/
class CImapFolderListArray :
    public CBase,
    public MDesCArray
    {
    public:

        /**
        * Two phased constructor NewL
        * @param CMsvSession&
        * @param TMsvId mailbox id which folders we are handling
        * @return CImapFolderListArray*
        */
        static CImapFolderListArray* NewL(
            CMsvSession& aSession,
            TMsvId aMailboxId );


        /**
        * Destructor
        * ~CImapFolderListArray()
        */
        ~CImapFolderListArray();

        /**
        * Id()
        * @param TInt, index which Id is to be fetched
        * @return TMsvId, the id of folder identified by parameter
        */
        TMsvId Id( TInt aIndex );

        /**
        * ChangeFolderL
        * @param TMsvId, id of folder to be changed into
        */
        void ChangeFolderL( TMsvId aFolderId );

        /**
        * FoldersUpdated
        * @return TBool, ETrue if folders have been updated
        */
        TBool FoldersUpdated() const;

        /**
        * ContextHasChildren
        * @param TMsvId, id of folder to be checked for children
        * @TBool ETrue if folder has children
        */
        TBool ContextHasChildren( TMsvId aId ) const;

        /**
        * GetContextChildrenL
        * This is a recursive function, which will get all folder entries under
        * the entry given in as the first parameter
        * @param TMsvId, id of the folder which children we want
        * @return CMsvEntrySelection*, array of TMsvIds of the children
        */
        CMsvEntrySelection* GetContextChildrenL( TMsvId aId, CMsvEntry& aEntry ) const;

        /**
        * RefreshFolderListArrayL()
        *
        */
        void RefreshFolderListArrayL();

    private:

        /**
        * C++ constructor
        * @param CMsvSession&
        * @param TMsvId
        */
        CImapFolderListArray(
            CMsvSession& aSession,
            TMsvId aMailboxId );

        /**
        * Second phase constructor
        * ConstructL()
        */
        void ConstructL();


        /**
        * from MDesCArray
        * MdcaCount()
        * @return TInt, count of items to be displayed
        */
        virtual TInt MdcaCount() const;

        /**
        * from MDesCArray
        * MdcaPoint()
        * @param TInt
        * @return TPtrC
        */
        virtual TPtrC MdcaPoint(TInt aIndex) const;

        /**
        * ContextHasChildFolders
        * @param TMsvId, id of folder to be checked for children
        * @return TBool, ETrue if folder has children
        */
        TBool ContextHasChildFolders( TMsvId aId ) const;

        /**
        * DoContextHasChildFoldersL
        * @param TMsvId, id of folder to be checked for children
        * @return TBool ETrue if folder has children
        */
        TBool DoContextHasChildFoldersL( TMsvId aId ) const;

    private: // data
        CMsvSession&    iSession;
        TMsvId          iMailboxId;
        CMsvEntry*      iEntry;
        HBufC*          iText;
        TInt            iCount;
    };

#endif

// End of File