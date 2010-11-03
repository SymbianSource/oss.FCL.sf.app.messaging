/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Array of notifications
*
*/



#ifndef __MMBOXARRAY_H__
#define __MMBOXARRAY_H__

#include <eikdialg.h>// CEikDialog
#include <msvstd.h>  // TMsvId

#include <bldvariant.hrh>

// FORWARD DECLARATIONS
class CMsvSession;
class CNotMtmUiData;

enum TMMBoxSortType
    {
    EMMBoxSortByDate = 0,
    EMMBoxSortByName,
    EMMBoxSortBySubject
    };

//  CLASS DEFINITIONS

/**
* CMMBoxArray
* Helper array class used by CMMBoxDialog
*/
class CMMBoxArray : 
    public CBase, 
    public MDesCArray
    {
    public:
        /**
        * Two phase constructor
        */
        static CMMBoxArray* NewL( CMsvSession& aSession, CNotMtmUiData& aUiData );

        /**
        * Destructor
        */
        ~CMMBoxArray();

        /**
        * Update notifications array
        * @param aFolderId: id of the folder where notifications are stored
        */
        void RefreshArrayL( TMsvId aFolderId );

        /**
        * Returns entry id 
        * @param aIndex: index of the entry
        * @return TMsvId: id of the entry
        */
        TMsvId EntryId( TInt aIndex ) const;

        /**
        * Returns TMsvEntry 
        * @param aIndex: index of the entry
        * @return TMsvEntry: TMsvEntry of the entry
        */
        TMsvEntry Entry( TInt aIndex ) const;

        /**
        * Returns current sorting type 
        * @return TMMBoxSortTypes: type of current sorting
        */
        TMMBoxSortType SortType( ) const;

        /**
        * Sets new sorting type
        * @param aType: new sorting type
        */
        TMsvSelectionOrdering SetSortType( const TMMBoxSortType& aType );

        /**
        * Reverses the current sorting type
        */
        void ReverseSortingL( );


    public: // Functions from base classes

        /**
        * from MDesCArray
        */
        virtual TInt MdcaCount() const;

        /**
        * from MDesCArray
        */
        virtual TPtrC MdcaPoint(TInt aIndex) const;

    private:
        /**
        * C++ constructor
        */
        CMMBoxArray( CMsvSession& aSession, CNotMtmUiData& aUiData );

        /**
        * EPOC constructor
        */
        void ConstructL();

    private:
        CMsvSession&    iSession;
        CMsvEntry*      iFolderEntry;
        HBufC*          iListBoxText;        
        CNotMtmUiData&  iUiData;

    };


#endif // __MMBOXARRAY_H__

//  End of File
