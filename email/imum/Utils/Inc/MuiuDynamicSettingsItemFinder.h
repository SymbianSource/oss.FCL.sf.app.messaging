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
* Description:  MuiuDynamicSettingsItemFinder.h
*
*/



#ifndef MUIUDYNAMICSETTINGSITEMFINDER_H
#define MUIUDYNAMICSETTINGSITEMFINDER_H

// INCLUDES
#include <e32base.h>
#include <muiuflags.h>
#include "MuiuDynamicSettingItemBase.h"
#include "MuiuDynamicSettingsItemFinderInterface.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CMuiuSettingBase;
class MMuiuDynamicSettingsItemFinderInterface;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class CMuiuDynamicSettingsItemFinder : public CBase
    {
    public: // Constructors and destructor

        /**
        * Destructor
        * @since S60
        */
        ~CMuiuDynamicSettingsItemFinder();

        /**
        *
        * @since S60
        */
        static CMuiuDynamicSettingsItemFinder* NewL(
            MMuiuDynamicSettingsItemFinderInterface& aObserver );

        /**
        *
        * @since S60
        */
        static CMuiuDynamicSettingsItemFinder* NewLC(
            MMuiuDynamicSettingsItemFinderInterface& aObserver );


    public: // New functions

        /**
        * Set method to member iSearchFlags
        * @since S60 3.0
        * @param aSearchFlags, Sets value of iSearchFlags
        */
        void SetSearchFlags( const TMuiuFlags& aSearchFlags );

        /**
        * Get function for member iSearchFlags
        * @since S60 3.0
        * @return value of iSearchFlags
        */
        TMuiuFlags SearchFlags() const;

        /**
        *
        * @since S60 3.0
        */
        void StartSearchL(
            CMuiuDynSetItemArray& aItemArray,
            const TUid& aId );

        /**
        *
        * @since S60 3.0
        */
        void StartSearchL(
            CMuiuDynSetItemArray& aItemArray,
            const TInt aItem );

        /**
        * Get function for member iFinderArray
        * @since S60 3.0
        * @return value of iFinderArray
        */
        CMuiuDynFinderItemArray* FinderArray() const;

        /**
        * Gets the copy of finder array and saves it to stack
        * @since S60 3.0
        * @return value of iFinderArray
        */
        CMuiuDynFinderItemArray* FinderArrayLC() const;

    public: // Functions from base classes

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private: // Constructors

        /**
        *
        * @since S60
        */
        CMuiuDynamicSettingsItemFinder(
            MMuiuDynamicSettingsItemFinderInterface& aObserver );

        /**
        *
        * @since S60
        */
        void ConstructL();

    private:  // New virtual functions
    private:  // New functions

        /**
        *
        * @since S60 3.0
        */
        void InitializeSearchL( const TInt aNth );

        /**
        *
        * @since S60 3.0
        */
        void FinalizeSearchL();

        /**
        *
        * @since S60 3.0
        */
        TBool ContinueSearch(
            const TInt aCurrentItem,
            const TInt aMaxItems );

        /**
        *
        * @since S60 3.0
        */
        TBool SearchingItem();

        /**
        *
        * @since S60 3.0
        */
        void ItemToArrayL();

        /**
        *
        * @since S60 3.0
        */
        void ItemCheckL( const TUid& aId );

        /**
        *
        * @since S60 3.0
        */
        void ItemCheckL();

        /**
        *
        * @since S60 3.0
        */
        TBool ItemSubArrayCheck();

        /**
        *
        * @since S60 3.0
        */
        TBool ContinueSubSearch();

        /**
        *
        * @since S60 3.0
        */
        void DoIdSearchSubArraysL( const TUid& aId );

        /**
        *
        * @since S60 3.0
        */
        void DoIndexSearchSubArraysL();

        /**
        *
        * @since S60 3.0
        */
        void DoEndSearch();

        /**
        *
        * @since S60 3.0
        */
        void DoIdSearchL(
            CMuiuDynSetItemArray& aItemArray,
            const TUid& aId );

        /**
        *
        * @since S60 3.0
        */
        void DoIndexSearchL( CMuiuDynSetItemArray& aItemArray );

        /**
        *
        * @since S60 3.0
        */
        void SearchIdFromTreeL(
            CMuiuDynSetItemArray& aItemArray,
            const TUid& aId );

        /**
        *
        * @since S60 3.0
        */
        void SearchIndexFromTreeL(
            CMuiuDynSetItemArray& aItemArray );

        /**
        *
        * @since S60 3.0
        */
        TBool IsHidden();

        /**
        *
        * @since S60 3.0
        */
        void SearchShouldStop();

    private:  // Functions from base classes

    public:     // Data

        enum TMuiuDynFinderFlags
            {
            // Following flags are for set up the search

            // When enabled, user defined check is made
            EFinderCustomCheck = 0,
            // Includes subfolders in search
            EFinderSearchSubFolders,
            // Overrides the item spesific tests for one round
            EFinderOverrideChecks,
            // Hidden items will not be included into the search
            EFinderDoHideCheck,
            // Searches for resources instead of item
            EFinderResourceSearch,
            // Goes through whole array regardless of hits
            EFinderSearchAll,
            // Keeps the last search result in memory
            EFinderKeepLastResult,
            // Searches items that doesn't match the set
            EFinderExlusiveSearch,
            // The result tree's ownership is moved to client
            EFinderMoveOwnership,

            // Following flags are mainly for internal use
            EFinderItemFound = 16,
            EFinderItemFindError,
            EFinderExit
            };

    protected:  // Data
    private:    // Data

        //
        MMuiuDynamicSettingsItemFinderInterface& iObserver;
        // Flags to control the finder
        TMuiuFlags              iSearchFlags;
        //
        CMuiuDynSetItemArray*   iSubArrays;
        //
        CMuiuSettingBase*       iTempItem;
        //
        TInt                    iSearchItem;
        // Current item index, used for passing forward
        TInt                    iCurrentItem;
        //
        CMuiuDynFinderItemArray* iFinderArray;
    };

#endif      // MUIUDYNAMICSETTINGSITEMFINDER_H

// End of File
