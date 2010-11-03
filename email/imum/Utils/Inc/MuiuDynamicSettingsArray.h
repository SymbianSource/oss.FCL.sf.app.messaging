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
* Description:  This file contains declearation of settings array handler
*
*/



#ifndef MUIUDYNAMICSETTINGSARRAY_H
#define MUIUDYNAMICSETTINGSARRAY_H

// INCLUDES
#include <e32base.h>                // Base symbian include
#include <e32debug.h>               // RDebug
#include <bamdesca.h>                // MDesCArray
#include <eikenv.h>                 // CEikonEnv
#include "MuiuDynamicSettingsArray.hrh" // CMuiuSettingsType
#include "MuiuDynamicSettingItemBase.h"      // CMuiuSettingBase
#include "MuiuDynamicSettingItemEditValue.h" // CMuiuSettingEditValue
#include "MuiuDynamicSettingItemEditText.h"  // CMuiuSettingEditText
#include "MuiuDynamicSettingItemLink.h"      // CMuiuSettingLink
#include "MuiuDynamicSettingItemExtLink.h"   // CMuiuSettingExtLink
#include <ConeResLoader.h>              // RConeResourceLoader
#include <muiuflags.h>
#include "MuiuDynamicSettingsItemFinderInterface.h"

// CONSTANTS
enum TMuiuStackResult
    {
    EMuiuStackResultOk,
    EMuiuStackResultSubMenuOpen,
    EMuiuStackResultInvalid
    };

// MACROS
// DATA TYPES
typedef RArray<TInt> RResourceStack;

// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class TResourceReader;
class CMuiuSettingBase;
class CMuiuDynamicSettingsItemFinder;
class TMuiuFinderItem;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.0
*/
class CMuiuDynamicSettingsArray :
    public CBase, public MDesCArray,
    public MMuiuDynamicSettingsItemFinderInterface
    {
    public: // Constructors and destructor

        virtual ~CMuiuDynamicSettingsArray();

    public: // New functions

        /**
        * Forces the redraw
        * @since S60 3.0
        */
        void Refresh();

        /**
        * ?member_description
        * @since S60 3.0
        * @param aResource Resource to be used to add the objects to the array
        * @return Result of the operation
        */
        TInt AddItemsL(
            const TInt aResource );

        TInt AddItemsL(
            const TInt aResource,
            const TInt aIndex );

        TInt AddItemL(
            const CMuiuSettingBase& aBaseItem,
            const TInt aIndex = KErrNotFound );

        TInt DelItems( const TInt aResource );
        TInt DelItem( const TInt aIndex );
        TInt DelItem( const TUid& aId );

        void SetItemValue(
            const TUid& aId,
            const TInt aValue,
            const TBool aUpdate = EFalse );

        void SetItemValue(
            const TInt aIndex,
            const TInt aValue,
            const TBool aUpdate = EFalse );

        void SetItemValue(
            CMuiuSettingBase& aBaseItem,
            const TInt aValue,
            const TBool aUpdate = EFalse );

        void SetItemText(
            const TUid& aId,
            const TMuiuSettingsText& aText,
            const TBool aUpdate = EFalse );

        void SetItemText(
            const TInt aIndex,
            const TMuiuSettingsText& aText,
            const TBool aUpdate = EFalse );

        void SetItemText(
            CMuiuSettingBase& aBaseItem,
            const TMuiuSettingsText& aText,
            const TBool aUpdate = EFalse );

        const TMuiuSettingsText* ItemText(
            CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60
        * @return
        */
        TMuiuStackResult HandleStackForwardL(
            const TInt aIndex = KErrNotFound );
        TMuiuStackResult HandleStackForwardL( const TUid& aId );
        TMuiuStackResult HandleStackForwardL( const CMuiuSettingBase& aBase );
        TMuiuStackResult HandleStackBackwardL( TInt aCount = 1 );
        TMuiuStackResult HandleStackChangeL(
            const TInt aForward,
            const TInt aResourceId = KErrNotFound );

        /**
        * Checks if item is hidden
        * @since S60
        * @param aIndex, index of the item
        * @param aId, id of the item
        * @param aItem, the item
        * @return KErrNone, if item is visible
        * @return KErrNotFound, if item is hidden
        * @return KErrNotSupported, if item is permanently hidden
        */
        TInt IsHidden( const TInt aIndex ) const;
        TInt IsHidden( const TUid& iId ) const;
        TInt IsHidden( const CMuiuSettingBase& aItem ) const;

        /**
        * Item hide functions
        */
        void SetHideAll(
            const TBool aHide,
            const TBool aUpdate = EFalse );
        void SetHideItem(
            const TBool aHide,
            const TInt aIndex,
            const TBool aUpdate = EFalse );
        void SetHideItem(
            const TBool aHide,
            const TUid& aId,
            const TBool aUpdate = EFalse );
        void SetHideItem(
            const TBool aHide,
            const CMuiuSettingBase& aItem,
            const TBool aUpdate = EFalse );
        void SetHideItems(
            const TBool aHide,
            const TInt aResource,
            const TBool aUpdate = EFalse );
        void SetHideItemsExcept(
            const TBool aHide,
            const TInt aIndex,
            const TBool aUpdate = EFalse );
        void SetHideItemsExcept(
            const TBool aHide,
            const TUid& aId,
            const TBool aUpdate = EFalse );
        void SetHideItemsExcept(
            const TBool aHide,
            const CMuiuSettingBase& aItem,
            const TBool aUpdate = EFalse );
        void SetHideItemsExceptIn(
            const TBool aHide,
            const TInt aResource,
            const TBool aUpdate = EFalse );

        CMuiuSettingBase* GetItem(
            const TUid& aId,
            const TBool aExcludeHidden = ETrue ) const;

        CMuiuSettingBase* GetItem(
            const TInt aIndex,
            const TBool aExcludeHidden = ETrue ) const;

    public: // Functions from base classes
    protected:  // Constructors

        CMuiuDynamicSettingsArray();
        void BaseConstructL( const TBool aLockedSettings );
        void BaseConstructL( 
            const TInt aResourceId, 
            const TBool aLockedSettings );

    protected:  // Data

        enum TMuiuArrayEvents
            {
            EMuiuDynArrayAdded = 0,
            EMuiuDynArrayRemoved,
            EMuiuDynArrayChanged,
            EMuiuDynArrayStackForward,
            EMuiuDynArrayStackBackward
            };

    protected:  // New virtual functions

        virtual TInt EventArrayChangedL(
            const TMuiuArrayEvents aEvent ) = 0;

        virtual TBool EventCustomMdcaPoint(
            const TUid& aId,
            TPtr& aString ) const = 0;

        /**
        *
        * @since S60 3.0
        */
        virtual CMuiuSettingBase* CreateCustomItemToArrayLC(
            const TUid& aId );

        /**
        *
        * @since S60 3.0
        */
        virtual void ListboxItemFillMultiLineLabel(
            const CMuiuSettingBase& aBaseItem,
            TMuiuSettingsText& aText ) const;

        /**
        *
        * @since S60 3.0
        */
        virtual void ListboxItemFillEditTextLabel(
            const CMuiuSettingBase& aBaseItem,
            TMuiuSettingsText& aText ) const;

        /**
        *
        * @since S60 3.0
        */
        virtual void ListboxItemFillEditValueLabel(
            const CMuiuSettingBase& aBaseItem,
            TMuiuSettingsText& aText ) const;

        /**
        *
        * @since S60 3.0
        */
        virtual void ListboxItemFillEditTimeLabel(
            const CMuiuSettingBase& aBaseItem,
            TMuiuSettingsText& aText ) const;

        /**
        *
        * @since S60 3.0
        */
        virtual void ListboxUndefinedFillLabel(
            const CMuiuSettingBase& aBaseItem,
            TMuiuSettingsText& aText ) const;

        /**
        *
        * @since S60 3.0
        */
        virtual void ListboxRadioButtonFillArrayLabel(
            const CMuiuSettingBase& aBaseItem,
            TMuiuSettingsText& aText ) const;

        /**
        *
        * @since S60 3.0
        */
        virtual void ListboxCheckboxFillArrayLabel(
            const CMuiuSettingBase& aBaseItem,
            TMuiuSettingsText& aText ) const;

    protected:  // New functions

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingBase* GetSubItem(
            const CMuiuSettingBase& aBaseItem,
            const TUid& aId,
            const TBool aExcludeHidden = ETrue ) const;

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingBase* GetSubItem(
            const CMuiuSettingBase& aBaseItem,
            const TInt aIndex,
            const TBool aExcludeHidden = ETrue ) const;

        /**
        *
        * @since S60 3.0
        */
        TInt GetSubItemIndex(
            const CMuiuSettingsLink& aLinkItem,
            const TUid& aId,
            const TBool aExcludeHidden = ETrue ) const;

        /**
        *
        * @since S60 3.0
        * @param aActualIndex
        *    ETrue  : aIndex is from visible list and index
        *             from the actual resource is required
        *    EFalse : aIndex is from resource and visible
        *             index is required
        */
        TInt GetSubItemIndex(
            const CMuiuSettingsLink& aLinkItem,
            const TInt aIndex,
            const TBool aActualIndex ) const;

        CMuiuDynFinderItemArray* GetResource(
            const TInt aResourceId,
            const TBool aExcludeHidden = ETrue ) const;
        CMuiuDynFinderItemArray* GetResourceLC(
            const TInt aResourceId,
            const TInt aExcludeHidden = ETrue ) const;

        TInt ItemIndex(
            const TUid& aId,
            const TBool aExcludeHidden = ETrue ) const;

        void SetBetweenValues(
            TInt& aValue,
            TInt aMin,
            TInt aMax ) const;

        TBool ItemSubArrayCheck(
            const CMuiuSettingBase& aBaseItem ) const;

        TBool ItemSubItemCheck(
            const CMuiuSettingBase& aBaseItem ) const;

        TInt StackActiveResource() const;

        /**
        *
        * @since S60 3.0
        */
        TBool IsVisibleCheck(
            const CMuiuSettingBase& aBaseItem,
            const TBool aExcludeHidden ) const;

    protected:  // Functions from base classes

        TInt MdcaCount() const;

        // From MMuiuDynamicSettingsItemFinderInterface
        virtual TBool SearchDoError();

        virtual TBool SearchDoContinuationCheck(
            const CMuiuSettingBase& aItem,
            const TInt aIndex );

        virtual TBool SearchDoItemCheck(
            CMuiuSettingBase& aItem );

    private:  // New virtual functions
    private:  // New functions

        /**
        *
        * @since S60 3.0
        */
        void CleanArrayItems();

        /**
        *
        * @since S60 3.0
        */
        void LoadSettingArrayFromResourceL( const TInt aResourceId );

        /**
        *
        * @since S60 3.0
        */
        TInt LoadSettingItemsToArrayL(
            const TInt aResourceId,
            CMuiuDynSetItemArray& aItemArray );


        void UpdateActiveItemsCount( const TInt aNewCount = KErrNotFound );

        /**
        *
        * @since S60
        */
        void HandleDynamicSettingsItemL(
            CMuiuSettingBase& aBaseItem,
            const TInt aMaxLength,
            const TMuiuDynSettingsType aSubType,
            const TInt aLinkResourceId,
            const TDesC* aEmptyText );

        /**
        *
        * @since S60
        */
        void LoadDynamicSettingItemL(
            TResourceReader& aReader,
            CMuiuSettingBase& aBaseItem );

        /**
        *
        * @since S60 3.0
        */
        TPtrC16 ListboxItemLabelFillL(
            CMuiuSettingBase& aBaseItem ) const;

        /**
        *
        * @since S60 3.0
        */
        CMuiuSettingBase* CreateItemToArrayL(
            const TMuiuDynSettingsType aType,
            const TUid& aId,
            CMuiuDynSetItemArray& aItemArray );

        /**
        * Functions to load special item settings
        * @since S60 3.0
        * @param aReader, Reader to be used to read the resource
        * @param aBaseItem, Item to be used as base to settings
        * @param aItemArray, Array that item should be stored
        */
        void LoadDynamicSettingsItemLinkItemL(
            CMuiuSettingBase& aBaseItem,
            const TMuiuDynSettingsType aSubType,
            const TInt aLinkResourceId );

        void LoadDynamicSettingsItemRadioButtonArrayL(
            CMuiuSettingBase& aBaseItem,
            CMuiuDynSetItemArray& aItemArray );

        void LoadDynamicSettingsItemCheckBoxArrayL(
            CMuiuSettingBase& aBaseItem,
            CMuiuDynSetItemArray& aItemArray );

        void UpdateItemEditTextL(
            CMuiuSettingBase& aBaseItem,
            const TInt aMaxLength,
            const TDesC* aEmptyText );

        void UpdateItemEditValueL(
            CMuiuSettingBase& aBaseItem,
            const TInt aMaxLength );

        void LoadDynamicSettingItemFlags(
            const TUint32 aFlags );

        // SEARCHING

        /**
        *
        * @since S60 3.0
        */
        void SearchResourceL(
            const TInt aResourceId,
            const TBool aExcludeHidden ) const;

        /**
        *
        * @since S60 3.0
        */
        void DoHideItems( const TBool aHide );
        void InitDefaultSearch(
            const TBool aExcludeHidden = ETrue ) const;

        /**
        *
        * @since S60 3.0
        */
        TInt StackCountItems(
            const TBool aExcludeHidden = ETrue ) const;

    private:  // Functions from base classes

        // from MDesCArray
        TPtrC16 MdcaPoint( TInt aIndex ) const;

    public:     // Data
    protected:  // Data

        // Flags to control the array behaviour
        enum TMuiuDynArrayFlags
            {
            // Allows updating of the array items
            EMuiuDynArrayAllowRefresh = 0,
            // Indicates if email settings are locked
            EMuiuEMailLockedSettings
            };
            
        // Text array for temporary usage, should be
        // deleted after the use
        HBufC*                  iTempText;
        // Flags for array handling
        TMuiuFlags              iArrayFlags;
        // String containing text from resource file
        HBufC*                  iResourceText;

    private:    // Data

        // Object to load resources
        RConeResourceLoader     iResourceLoader;
        // Item finder object
        CMuiuDynamicSettingsItemFinder* iFinder;
        // Stack to track the active resources
        RResourceStack*         iResourceStack;
        // Array of items to be shown
        CMuiuDynSetItemArray*   iItemArray;
        // String to fill empty compulsary fields
        HBufC*                  iCompulsoryText;
        // Char to replace character in password fields
        TUint16                 iPasswordFill;
        // Count of active items in array
        TInt                    iActiveCount;
        // Error state
        TInt                    iError;
        // Not own:
        CEikonEnv*              iEnv;
    };

#endif      // MUIUDYNAMICSETTINGSARRAY_H

// End of File
