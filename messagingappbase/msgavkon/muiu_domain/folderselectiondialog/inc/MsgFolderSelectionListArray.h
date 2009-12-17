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
*     Array class for list of local folders under message centre.
*
*/



#ifndef MSGFOLDERSELECTIONLISTARRAY_H
#define MSGFOLDERSELECTIONLISTARRAY_H

//  INCLUDES
#include <bamdesca.h>   // MDesCArray
#include <msvapi.h>     // MMsvSessionObserver
#include <msvstd.h>     // TMsvId


// FORWARD DECLARATIONS
class CMsvSession;
class CMsvEntrySelection;

// CLASS DECLARATION

/**
*  Takes care of list of folders under msg.
*  To the list "Inbox" is always first on the list and then "Documents" and
*  then all the folders under "Documents".
*/
class CMsgFolderSelectionListArray :
    public CBase,
    public MDesCArray,
    public MMsvSessionObserver
    {


    public:
        /**
        * Two-phased constructor.
        * @param aCurrentFolder Id of the current folder (not shown in the list)
        */
        static CMsgFolderSelectionListArray* NewL( TMsvId aCurrentFolder );

        /**
        * Destructor.
        */
        virtual ~CMsgFolderSelectionListArray();

    public: // New functions

        /**
        * Returns id of the aIndex.
        * @return TMsvId of the aIndex item
        */
        TMsvId ItemId( TInt aIndex ) const;

    public: // Functions from base classes

        /**
        * From MDesCArray
        * @return Number of items in the list
        */
        TInt MdcaCount() const;

        /**
        * From MDesCArray
        * @param aIndex Printable text of the item
        * @return TPtrC
        */
        TPtrC MdcaPoint(TInt aIndex) const;

        /**
        * From MMsvSessionObserver
        */
        virtual void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3);

    private: // Constructors and destructor
        /**
        * C++ default constructor.
        */
        CMsgFolderSelectionListArray( );


    private:
        /**
        * By default Symbian OS constructor is private.
        * @param aCurrentFolder is not shown in the list.
        */
        void ConstructL( TMsvId aCurrentFolder );

    private: // Data
    CMsvEntrySelection*     iFolderList;
    CMsvSession*            iSession;
    HBufC*                  iMdcaPointText;
    };


#endif      // MSGFOLDERSELECTIONLISTARRAY_H

// End of File
