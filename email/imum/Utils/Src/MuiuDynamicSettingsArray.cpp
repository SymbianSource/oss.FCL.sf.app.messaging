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
* Description:  MuiuDynamicSettingsArray.cpp
*
*/



// INCLUDE FILES
#include <e32base.h>            // Base symbian include
#include <gulutil.h>            // KColumnListSeparator
#include <eikrutil.h>           // EikResourceUtils
#include <AknUtils.h>           // AknTextUtils
#include <barsread.h>           // TResourceReader
#include "MuiuDynamicSettingsArray.h"
#include "MuiuDynamicSettingItemBase.h"      // CMuiuSettingBase
#include "MuiuDynamicSettingItemEditValue.h" // CMuiuSettingEditValue
#include "MuiuDynamicSettingItemEditText.h"  // CMuiuSettingEditText
#include "MuiuDynamicSettingItemLink.h"      // CMuiuSettingLink
#include "MuiuDynamicSettingItemExtLink.h"   // CMuiuSettingExtLink
#include <StringLoader.h>
#include <ImumUtils.rsg>
#include <data_caging_path_literals.hrh>
#include <muiu_internal.rsg>

#include "EmailFeatureUtils.h"
#include "MuiuDynamicSettingsItemFinder.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS

// This is added at the end of the locked setting item string
_LIT( KMuiuDynStrProtected, "\t\t\t\t\t0" );
// This is added at the end of the compulsory setting item string
_LIT( KMuiuDynStrCompulsory, "\t*" );
// Muiu resource file
_LIT( KMuiuResFileName, "z:muiu_internal.rsc" );
// The number of characters that can be shown in the screen
const TInt KMuiuDynVisibleTextLength = 30;
// The number of characters that is shown in password field
const TInt KMuiuDynVisiblePasswordChars = 4;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::CMuiuDynamicSettingsArray()
// ----------------------------------------------------------------------------
//
CMuiuDynamicSettingsArray::CMuiuDynamicSettingsArray()
    :
    iResourceLoader( *CCoeEnv::Static() ),
    iFinder( NULL ),
    iItemArray( NULL ),
    iCompulsoryText( NULL ),
    iPasswordFill( 0x00 ),
    iActiveCount( 0 ),
    iError( KErrNone ),
    iEnv( CEikonEnv::Static() )
    {
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::~CMuiuDynamicSettingsArray()
// ----------------------------------------------------------------------------
//
CMuiuDynamicSettingsArray::~CMuiuDynamicSettingsArray()
    {
    CleanArrayItems();

    if ( iResourceStack )
        {
        iResourceStack->Reset();
        delete iResourceStack;
        iResourceStack = NULL;
        }
    delete iItemArray;
    iItemArray = NULL;
    delete iTempText;
    iTempText = NULL;
    delete iResourceText;
    iResourceText = NULL;
    delete iCompulsoryText;
    iCompulsoryText = NULL;
    delete iFinder;
    iFinder = NULL;
    iResourceLoader.Close();

    iEnv = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::BaseConstructL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::BaseConstructL( const TBool aLockedSettings )
    {
    // Prepare the arrays
    iItemArray = new ( ELeave ) CMuiuDynSetItemArray( KMuiuDynArrayGranularity );
    iResourceStack = new ( ELeave ) RResourceStack( KMuiuDynArrayGranularity );
    iFinder = CMuiuDynamicSettingsItemFinder::NewL( *this );

    // Define resource file and open it
    TParse parse;
    parse.Set( KMuiuResFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName fileName( parse.FullName() );
    iResourceLoader.OpenL( fileName );
    iArrayFlags.SetFlag( EMuiuDynArrayAllowRefresh );
    iArrayFlags.ChangeFlag( EMuiuEMailLockedSettings, aLockedSettings );

    // Prepare strings
    iTempText = HBufC::NewL( KMuiuDynMaxSettingsTextLength );
    iResourceText = iEnv->AllocReadResourceL(
        R_IMUM_SELEC_SETT_VAL_FIELD_NONE );
    iPasswordFill = EikResourceUtils::ReadTInt16L(
        R_MUIU_SETTINGS_DIALOG_PASSWORD_FILL, iEnv );
    iCompulsoryText = iEnv->AllocReadResourceL(
        R_MUIU_SETTINGS_DIALOG_COMPULSORY_FILL );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::BaseConstructL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::BaseConstructL(
    const TInt aResourceId,
    const TBool aLockedSettings )
    {
    // Initialize the members
    BaseConstructL( aLockedSettings );

    // If resource provided add it into the setting array
    LoadSettingArrayFromResourceL( aResourceId );

    UpdateActiveItemsCount();
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::CleanArrayItems()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::CleanArrayItems()
    {
    if ( iItemArray )
        {
        // Delete items from the array
        TInt index = iItemArray->Count() - 1;

        // Search through all items in the array
        for ( ; index >= 0; index-- )
            {
            // Only certain type of items can contain subarrays
            CMuiuSettingBase* base = ( *iItemArray )[index];
            delete base;
            base = NULL;
            }
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::CreateCustomItemToArrayLC()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuDynamicSettingsArray::CreateCustomItemToArrayLC(
    const TUid& /* aId */ )
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::ListboxItemFillMultiLineLabel()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::ListboxItemFillMultiLineLabel(
    const CMuiuSettingBase& aBaseItem,
    TMuiuSettingsText& aText ) const
    {
    aText = static_cast<const CMuiuSettingsLink*>(
        &aBaseItem )->iItemSettingText->Left( KMuiuDynVisibleTextLength );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::ListboxItemFillEditTextLabel()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::ListboxItemFillEditTextLabel(
    const CMuiuSettingBase& aBaseItem,
    TMuiuSettingsText& aText ) const
    {
    aText.Copy( aBaseItem.VisibleText().Left( KMuiuDynVisibleTextLength ) );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::ListboxItemFillEditValueLabel()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::ListboxItemFillEditValueLabel(
    const CMuiuSettingBase& aBaseItem,
    TMuiuSettingsText& aText ) const
    {
    aText.Append( aBaseItem.Value() );
    aText.SetLength( KMuiuDynVisibleTextLength );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::ListboxItemFillEditTimeLabel()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::ListboxItemFillEditTimeLabel(
    const CMuiuSettingBase& aBaseItem,
    TMuiuSettingsText& aText ) const
    {
    aText.Append( aBaseItem.Value() );
    aText.SetLength( KMuiuDynVisibleTextLength );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::ListboxUndefinedFillLabel()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::ListboxUndefinedFillLabel(
    const CMuiuSettingBase& aBaseItem,
    TMuiuSettingsText& aText ) const
    {
    aText = static_cast<const CMuiuSettingsLinkExtended*>(
        &aBaseItem )->iItemSettingText->Left( KMuiuDynVisibleTextLength );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::ListboxRadioButtonFillArrayLabel()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::ListboxRadioButtonFillArrayLabel(
    const CMuiuSettingBase& aBaseItem,
    TMuiuSettingsText& aText ) const
    {
    aText = static_cast<const CMuiuSettingsLinkExtended*>(
        &aBaseItem )->iItemSettingText->Left( KMuiuDynVisibleTextLength );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::ListboxCheckboxFillArrayLabel()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::ListboxCheckboxFillArrayLabel(
    const CMuiuSettingBase& aBaseItem,
    TMuiuSettingsText& aText ) const
    {
    aText = static_cast<const CMuiuSettingsLinkExtended*>(
        &aBaseItem )->iItemSettingText->Left( KMuiuDynVisibleTextLength );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::ListboxItemLabelFillL()
// ----------------------------------------------------------------------------
//
TPtrC16 CMuiuDynamicSettingsArray::ListboxItemLabelFillL(
    CMuiuSettingBase& aBaseItem ) const
    {
    // Get the type of the setting item
    TMuiuDynSettingsType type = aBaseItem.iItemType;
    TMuiuSettingsText tempSettingText;
    tempSettingText.Zero();

    switch ( type )
        {
        case EMuiuDynSetItemMultiLine:
            ListboxItemFillMultiLineLabel( aBaseItem, tempSettingText );
            break;

        case EMuiuDynSetUndefined:
            ListboxUndefinedFillLabel( aBaseItem, tempSettingText );
            break;

        case EMuiuDynSetRadioButtonArray:
            ListboxRadioButtonFillArrayLabel( aBaseItem, tempSettingText );
            break;

        case EMuiuDynSetCheckBoxArray:
            ListboxCheckboxFillArrayLabel( aBaseItem, tempSettingText );
            break;

        case EMuiuDynSetItemEditText:
            ListboxItemFillEditTextLabel( aBaseItem, tempSettingText );
            break;

        case EMuiuDynSetItemEditValue:
            ListboxItemFillEditValueLabel( aBaseItem, tempSettingText );
            break;

        case EMuiuDynSetItemEditTime:
            ListboxItemFillEditTimeLabel( aBaseItem, tempSettingText );
            break;

        default:
            // Nothing to show, send empty text
            break;
        }

    // If the setting is marked as password, fill special character
    // instead of readable text
    TInt length = tempSettingText.Length();
    if ( length > 0 && aBaseItem.iItemFlags.Flag( EMuiuDynItemPassword ) )
        {
        // Add special chars to all type characters in setting field
        TChar passFill = TChar( iPasswordFill );
        tempSettingText.Zero();
        tempSettingText.AppendFill( passFill, KMuiuDynVisiblePasswordChars );
        }

    return tempSettingText;
    }

/******************************************************************************

    Item property editors

******************************************************************************/

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SetItemValue()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SetItemValue(
    const TUid& aId,
    const TInt aValue,
    const TBool aUpdate )
    {
    CMuiuSettingBase* baseItem = GetItem( aId, EFalse );

    if ( baseItem )
        {
        SetItemValue( *baseItem, aValue, aUpdate );
        }

    baseItem = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SetItemValue()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SetItemValue(
    const TInt aIndex,
    const TInt aValue,
    const TBool aUpdate )
    {
    CMuiuSettingBase* baseItem = iItemArray->At( aIndex );

    if ( baseItem )
        {
        SetItemValue( *baseItem, aValue, aUpdate );
        }

    baseItem = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SetItemValue()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SetItemValue(
    CMuiuSettingBase& aBaseItem,
    const TInt aValue,
    const TBool aUpdate )
    {
    aBaseItem.SetValue( aValue );

    // Issue update when needed
    if ( aUpdate )
        {
        Refresh();
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SetItemText()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SetItemText(
    const TUid& aId,
    const TMuiuSettingsText& aText,
    const TBool aUpdate )
    {
    CMuiuSettingBase* baseItem = GetItem( aId, EFalse );

    if ( baseItem )
        {
        SetItemText( *baseItem, aText, aUpdate );
        }

    baseItem = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SetItemText()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SetItemText(
    const TInt aIndex,
    const TMuiuSettingsText& aText,
    const TBool aUpdate )
    {
    CMuiuSettingBase* baseItem = iItemArray->At( aIndex );

    if ( baseItem )
        {
        SetItemText( *baseItem, aText, aUpdate );
        }

    baseItem = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SetItemText()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SetItemText(
    CMuiuSettingBase& aBaseItem,
    const TMuiuSettingsText& aText,
    const TBool aUpdate )
    {
    switch ( aBaseItem.iItemType )
        {
        case EMuiuDynSetItemEditValue:
            static_cast<CMuiuSettingsEditValue*>(
                &aBaseItem )->iItemSettingText->Copy( aText );
            break;

        case EMuiuDynSetRadioButtonArray:
        case EMuiuDynSetCheckBoxArray:
            static_cast<CMuiuSettingsLinkExtended*>(
                &aBaseItem )->iItemSettingText->Copy( aText );
            break;

        default:
            aBaseItem.SetText( &aText );
            break;
        }

    // Issue update when needed
    if ( aUpdate )
        {
        Refresh();
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::ItemText()
// ----------------------------------------------------------------------------
//
const TMuiuSettingsText* CMuiuDynamicSettingsArray::ItemText(
    CMuiuSettingBase& aBaseItem )
    {
    switch ( aBaseItem.iItemType )
        {
        case EMuiuDynSetItemEditValue:
            return static_cast<CMuiuSettingsEditValue*>(
                &aBaseItem )->iItemSettingText;

        case EMuiuDynSetRadioButtonArray:
        case EMuiuDynSetCheckBoxArray:
            return static_cast<CMuiuSettingsLinkExtended*>(
                &aBaseItem )->iItemSettingText;
        }

    return aBaseItem.Text();
    }

/******************************************************************************

    Base class function definitions

******************************************************************************/

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SearchDoError()
// ----------------------------------------------------------------------------
//
TBool CMuiuDynamicSettingsArray::SearchDoError()
    {
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SearchDoContinuationCheck()
// ----------------------------------------------------------------------------
//
TBool CMuiuDynamicSettingsArray::SearchDoContinuationCheck(
    const CMuiuSettingBase& /* aItem */,
    const TInt /* aIndex */ )
    {
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SearchDoItemCheck()
// ----------------------------------------------------------------------------
//
TBool CMuiuDynamicSettingsArray::SearchDoItemCheck(
    CMuiuSettingBase& /* aItem */ )
    {
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::MdcaCount()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsArray::MdcaCount() const
    {
    return iActiveCount;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::MdcaPoint()
// ----------------------------------------------------------------------------
//
TPtrC16 CMuiuDynamicSettingsArray::MdcaPoint( TInt aIndex ) const
    {
    // First search the currently active resource to array and check
    // the given index is in the current array
    __ASSERT_DEBUG( aIndex < StackCountItems(),
        User::Panic( KMuiuDynSettingsArrayPanic, KErrOverflow ) );

    // Search for item from the active array
    CMuiuSettingBase* setting = iFinder->FinderArray()->At( aIndex ).iItem;
    __ASSERT_DEBUG( setting != NULL,
        User::Panic( KMuiuDynSettingsArrayPanic, KErrNotFound ) );

    // Prepare strings
    TPtr tempText = iTempText->Des();

    // Send custom event and if not used, prepare default
    if ( !EventCustomMdcaPoint( setting->iItemId, tempText ) )
        {
        // Initialize string
        tempText.Zero();
        tempText.Append( KMuiuDynStrSpace );
        tempText.Append( KColumnListSeparator );
        tempText.Append( *setting->iItemLabel );

        // Submenus doesn't contain other information
        if ( !ItemSubArrayCheck( *setting ) )
            {
            tempText.Append( KColumnListSeparator );
            tempText.Append( KColumnListSeparator );

            TMuiuSettingsText tempSettingText;
            // Try to add the text, error detected in next if
            TRAP_IGNORE( tempSettingText =
                ListboxItemLabelFillL( *setting ) );

            // Fill the setting text with string
            if ( tempSettingText.Length() )
                {
                tempText.Append( tempSettingText );
                }
            else
                {
                // Write compulsory text to required fields
                if ( setting->iItemFlags.Flag( EMuiuDynItemCompulsory ) )
                    {
                    tempText.Append( *iCompulsoryText );
                    }
                else if( setting->iItemFlags.Flag( EMuiuDynItemPassword ) )
                    {
                    tempText.Append( KMuiuDynStrSpace );
                    }
                else
                    {
                    tempText.Append( *iResourceText );
                    }
                }

            // Add the special string after compulsory fields
            if ( setting->iItemFlags.Flag( EMuiuDynItemCompulsory ) )
                {
                tempText.Append( KMuiuDynStrCompulsory );
                }

            // Do the number conversion, if allowed
            if ( setting->iItemFlags.Flag( EMuiuDynItemLangSpecificNumConv ) )
                {
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
                    tempText );
                }
            }
        else if( iArrayFlags.Flag( EMuiuEMailLockedSettings ) &&
            setting->iItemFlags.Flag( EMuiuDynItemLockableSetting ) )
            {
            tempText.Append( KMuiuDynStrProtected );
            }
        }

    setting = NULL;
    return tempText;
    }

/******************************************************************************

    Resource loading functions

******************************************************************************/

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::LoadDynamicSettingItemL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::LoadDynamicSettingItemL(
    TResourceReader& aReader,
    CMuiuSettingBase& aBaseItem )
    {
    // Read rest of the settings first
    // STRUCT MUIU_DYNAMIC_SETTINGS_ITEM : LTEXT itemlabel
    HBufC* txt = aReader.ReadHBufCL();   // message text

    if ( txt )
        {
        aBaseItem.iItemLabel->Copy( *txt );
        }

    delete txt;
    txt = NULL;

    // STRUCT MUIU_DYNAMIC_SETTINGS_ITEM : WORD itemmaxlength
    TInt maxLength = aReader.ReadInt16();

    // STRUCT MUIU_DYNAMIC_SETTINGS_ITEM : LTEXT itememptytext
    HBufC* empty = aReader.ReadHBufCL();
    CleanupStack::PushL( empty );

    // STRUCT MUIU_DYNAMIC_SETTINGS_ITEM : LONG itemflags
    aBaseItem.iItemFlags = MAKE_TUINT64( aReader.ReadInt32(), 0 );

    // STRUCT MUIU_DYNAMIC_SETTINGS_ITEM : LLINK itemarray
    TInt linkResourceId = aReader.ReadInt32();

    // STRUCT MUIU_DYNAMIC_SETTINGS_ITEM : WORD itemsubtype
    TMuiuDynSettingsType subtype =
        static_cast<TMuiuDynSettingsType>( aReader.ReadInt16() );

    // Finally add the item to list
    HandleDynamicSettingsItemL(
        aBaseItem, maxLength, subtype, linkResourceId, empty );

    CleanupStack::PopAndDestroy( empty );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::UpdateItemEditTextL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::UpdateItemEditTextL(
    CMuiuSettingBase& aBaseItem,
    const TInt aMaxLength,
    const TDesC* aEmptyText )
    {
    CMuiuSettingsEditText* text =
        static_cast<CMuiuSettingsEditText*>( &aBaseItem );
    text->iItemMaxLength = aMaxLength;
    if( aEmptyText )
        {
        text->SetEmptyText( *aEmptyText );
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::UpdateItemEditValueL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::UpdateItemEditValueL(
    CMuiuSettingBase& aBaseItem,
    const TInt aMaxLength )
    {
    CMuiuSettingsEditValue* value =
        static_cast<CMuiuSettingsEditValue*>( &aBaseItem );
    value->iItemMaxLength = aMaxLength;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::HandleDynamicSettingsItemL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::HandleDynamicSettingsItemL(
    CMuiuSettingBase& aBaseItem,
    const TInt aMaxLength,
    const TMuiuDynSettingsType aSubType,
    const TInt aLinkResourceId,
    const TDesC* aEmptyText )
    {
    switch ( aBaseItem.Type() )
        {
        // Load dynamically linked items or submenus
        case EMuiuDynSetMenuArray:
        case EMuiuDynSetRadioButtonArray:
        case EMuiuDynSetCheckBoxArray:
        case EMuiuDynSetItemMultiLine:
            LoadDynamicSettingsItemLinkItemL(
                aBaseItem, aSubType, aLinkResourceId );
            break;

        // Finish text item
        case EMuiuDynSetItemEditText:
            UpdateItemEditTextL( aBaseItem, aMaxLength, aEmptyText );
            break;

        // Finish number item
        case EMuiuDynSetItemEditValue:
        case EMuiuDynSetItemEditTime:
            UpdateItemEditValueL( aBaseItem, aMaxLength );
            break;

        // Other items are already finished
        case EMuiuDynSetItemRadioButton:
        case EMuiuDynSetItemCheckBox:
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::CreateItemToArrayL()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuDynamicSettingsArray::CreateItemToArrayL(
    const TMuiuDynSettingsType aType,
    const TUid& aId,
    CMuiuDynSetItemArray& aItemArray )
    {
    CMuiuSettingBase* base = NULL;

    switch ( aType )
        {
        // Create array item
        case EMuiuDynSetMenuArray:
            base = CMuiuSettingsLink::NewLC();
            break;

        // Force manual item creation
        case EMuiuDynSetUndefined:
            base = CreateCustomItemToArrayLC( aId );
            __ASSERT_DEBUG( base != NULL,
                User::Panic( KMuiuDynSettingsArrayPanic, KErrUnknown ) );
            break;

        // Create radiobutton array item
        case EMuiuDynSetRadioButtonArray:
        case EMuiuDynSetCheckBoxArray:
        case EMuiuDynSetItemMultiLine:
            base = CMuiuSettingsLinkExtended::NewLC();
            break;

        // Create edit text item
        case EMuiuDynSetItemEditText:
            base = CMuiuSettingsEditText::NewLC();
            break;

        // Create edit number item
        case EMuiuDynSetItemEditValue:
        case EMuiuDynSetItemEditTime:
            base = CMuiuSettingsEditValue::NewLC();
            break;

        // Create normal base item
        case EMuiuDynSetItemRadioButton:
        case EMuiuDynSetItemCheckBox:
        default:
            base = CMuiuSettingBase::NewLC();
            break;
        }

    // Add to array
    aItemArray.AppendL( base );
    CleanupStack::Pop( base );

    return base;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::LoadDynamicSettingsItemLinkItemL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::LoadDynamicSettingsItemLinkItemL(
    CMuiuSettingBase& aBaseItem,
    const TMuiuDynSettingsType aSubType,
    const TInt aLinkResourceId )
    {
    // Prepare the item
    CMuiuSettingsLink* linkItem = static_cast<CMuiuSettingsLink*>( &aBaseItem );
    linkItem->iItemSubType = aSubType;
    linkItem->iItemFlags.SetFlag( EMuiuDynItemUpdateOnChange );

    // Start reading only if resource is provided
    if ( aLinkResourceId )
        {
        linkItem->iItemLinkArray =
            new ( ELeave ) CMuiuDynSetItemArray(
                KMuiuDynArrayGranularity );

        // Start loading the settings
        LoadSettingItemsToArrayL( aLinkResourceId, *linkItem->iItemLinkArray );
        }

    linkItem = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::LoadSettingArrayFromResource()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::LoadSettingArrayFromResourceL(
    const TInt aResourceId )
    {
    // Start reading only if resource is provided
    if ( aResourceId )
        {
        iResourceStack->AppendL( aResourceId );
        const TInt items = iItemArray->Count();

        // Begin to read the items from the resource
        TInt count = LoadSettingItemsToArrayL( aResourceId, *iItemArray );

        // Update the item count
        UpdateActiveItemsCount( count );
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::LoadSettingItemsToArrayL()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsArray::LoadSettingItemsToArrayL(
    const TInt aResourceId,
    CMuiuDynSetItemArray& aItemArray )
    {
    // Define new read based on last item
    TResourceReader reader;
    iEnv->CreateResourceReaderLC(
        reader, aResourceId );

    // Get the count of items in resource
    TInt count = reader.ReadInt16();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        // Read the settings from the resource

        // STRUCT MUIU_DYNAMIC_SETTINGS_ITEM : WORD itemtype
        TMuiuDynSettingsType type =
            static_cast<TMuiuDynSettingsType>( reader.ReadInt16() );

        // STRUCT MUIU_DYNAMIC_SETTINGS_ITEM : WORD itemid
        TUid id = TUid::Uid( reader.ReadInt16() );

        // Create the setting item
        CMuiuSettingBase* base =
            CreateItemToArrayL( type, id, aItemArray );
        base->iItemId = id;
        base->iItemType = type;
        base->iItemResourceId = aResourceId;

        // Start loading the item, based on item type
        LoadDynamicSettingItemL( reader, *base );
        base = NULL;
        }

    CleanupStack::PopAndDestroy(); // reader

    return count;
    }

/******************************************************************************

    Item addition and deletion functions

******************************************************************************/

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::AddItemsL()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsArray::AddItemsL( const TInt aResource )
    {
    // Load the resource and add it to array
    LoadSettingArrayFromResourceL( aResource );

    iError = EventArrayChangedL( EMuiuDynArrayAdded );

    // return the error state
    return iError;
    }

/******************************************************************************

    Item hide functions

******************************************************************************/

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::IsHidden()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsArray::IsHidden( const TUid& aId ) const
    {
    return IsHidden( *GetItem( aId ) );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::IsHidden()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsArray::IsHidden(
    const CMuiuSettingBase& aItem ) const
    {
    if ( aItem.iItemFlags.Flag( EMuiuDynItemPermanentlyHidden ) )
        {
        return KErrNotSupported;
        }
    else if ( aItem.iItemFlags.Flag( EMuiuDynItemHidden ) )
        {
        return KErrNotFound;
        }
    else
        {
        return KErrNone;
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::IsHidden()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsArray::IsHidden( const TInt aIndex ) const
    {
    return IsHidden( *GetItem( aIndex ) );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::DoHideItems()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::DoHideItems( const TBool aHide )
    {
    // All items collected, set their flags hidden flags
    CMuiuDynFinderItemArray& array = *iFinder->FinderArray();
    TInt item = array.Count();
    while ( --item >= 0 )
        {
        CMuiuSettingBase* base = array[item].iItem;
        base->iItemFlags.ChangeFlag( EMuiuDynItemHidden, aHide );
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::HideItems()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SetHideItems(
    const TBool aHide,
    const TInt aResourceId,
    const TBool aUpdate )
    {
    // When hiding all, search through all subfolders and allow the state
    // change
    TMuiuFlags search;
    search.SetFlag( CMuiuDynamicSettingsItemFinder::EFinderSearchSubFolders );
    search.SetFlag( CMuiuDynamicSettingsItemFinder::EFinderResourceSearch );
    iFinder->SetSearchFlags( search );

    // Try to hide items and panic in any error
    TRAPD( error, iFinder->StartSearchL( *iItemArray, aResourceId ) );
    if ( error != KErrNone )
        {
        __ASSERT_DEBUG( EFalse,
            User::Panic( KMuiuDynSettingsArrayPanic, error ) );
        }

    DoHideItems( aHide );

    // Handle the array change
    if ( aUpdate )
        {
        Refresh();
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SetHideAll()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SetHideAll(
    const TBool aHide,
    const TBool aUpdate )
    {
    // When hiding all, search through all subfolders and allow the state
    // change
    TMuiuFlags search;
    search.SetFlag( CMuiuDynamicSettingsItemFinder::EFinderSearchSubFolders );
    search.SetFlag( CMuiuDynamicSettingsItemFinder::EFinderSearchAll );
    iFinder->SetSearchFlags( search );

    // Try to hide items and panic in any error
    TRAPD( error, iFinder->StartSearchL( *iItemArray, KErrNotFound ) );
    if ( error != KErrNone )
        {
        __ASSERT_DEBUG( EFalse,
            User::Panic( KMuiuDynSettingsArrayPanic, error ) );
        }

    // Do hide action
    DoHideItems( aHide );

    // Handle the array change
    if ( aUpdate )
        {
        Refresh();
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SetHideItem()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SetHideItem(
    const TBool aHide,
    const TInt aIndex,
    const TBool aUpdate )
    {
    GetItem( aIndex )->iItemFlags.ChangeFlag( EMuiuDynItemHidden, aHide );

    // Handle the array change
    if ( aUpdate )
        {
        Refresh();
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SetHideItem()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SetHideItem(
    const TBool aHide,
    const TUid& aId,
    const TBool aUpdate )
    {
    GetItem( aId )->iItemFlags.ChangeFlag( EMuiuDynItemHidden, aHide );

    // Handle the array change
    if ( aUpdate )
        {
        Refresh();
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SetHideItemsExceptIn()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SetHideItemsExceptIn(
    const TBool aHide,
    const TInt aResource,
    const TBool aUpdate )
    {
    // When hiding all, search through all subfolders and allow the state
    // change
    TMuiuFlags search;
    search.SetFlag( CMuiuDynamicSettingsItemFinder::EFinderSearchSubFolders );
    search.SetFlag( CMuiuDynamicSettingsItemFinder::EFinderExlusiveSearch );
    search.SetFlag( CMuiuDynamicSettingsItemFinder::EFinderResourceSearch );
    iFinder->SetSearchFlags( search );

    // Try to hide items and panic in any error
    TRAPD( error, iFinder->StartSearchL( *iItemArray, aResource ) );
    if ( error != KErrNone )
        {
        __ASSERT_DEBUG( EFalse,
            User::Panic( KMuiuDynSettingsArrayPanic, error ) );
        }

    // Do hide action
    DoHideItems( aHide );

    // Handle the array change
    if ( aUpdate )
        {
        Refresh();
        }
    }

/******************************************************************************

    Item search functions

******************************************************************************/

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::InitDefaultSearch()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::InitDefaultSearch(
    const TBool aExcludeHidden ) const
    {
    // In default search, subfolders are included and hidden items are not
    // included into the search
    TMuiuFlags search;
    search.SetFlag( CMuiuDynamicSettingsItemFinder::EFinderSearchSubFolders );
    search.ChangeFlag(
        CMuiuDynamicSettingsItemFinder::EFinderDoHideCheck, aExcludeHidden );
    iFinder->SetSearchFlags( search );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::ItemIndex()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsArray::ItemIndex(
    const TUid& aId,
    const TBool aExcludeHidden ) const
    {
    InitDefaultSearch( aExcludeHidden );

    // Try to make the search, but if the search fails, allow caller
    // to handle the result
    TRAP_IGNORE( iFinder->StartSearchL( *iItemArray, aId ) );

    // Return the found item
    return iFinder->FinderArray()->At( 0 ).iIndex;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::GetItem()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuDynamicSettingsArray::GetItem(
    const TInt aIndex,
    const TBool aExcludeHidden ) const
    {
    // First search the currently active resource to array and check
    // the given index is in the current array
    TBool ok = aIndex < StackCountItems( aExcludeHidden );
    __ASSERT_DEBUG( ok,
        User::Panic( KMuiuDynSettingsArrayPanic, KErrOverflow ) );
    __ASSERT_DEBUG( aIndex >= 0,
        User::Panic( KMuiuDynSettingsArrayPanic, KErrUnderflow ) );

    // Return the found item
    return ok ? iFinder->FinderArray()->At( aIndex ).iItem : NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::GetItem()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuDynamicSettingsArray::GetItem(
    const TUid& aId,
    const TBool aExcludeHidden ) const
    {
    InitDefaultSearch( aExcludeHidden );

    // Try to make the search, but if the search fails, allow caller
    // to handle the result
    TRAP_IGNORE( iFinder->StartSearchL( *iItemArray, aId ) );

    // Return the found item
    return iFinder->FinderArray()->At( 0 ).iItem;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::GetSubItem()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuDynamicSettingsArray::GetSubItem(
    const CMuiuSettingBase& aBaseItem,
    const TUid& aId,
    const TBool aExcludeHidden ) const
    {
    // Make sure the item has subitems
    if ( ItemSubItemCheck( aBaseItem ) )
        {
        // Convert the item to link item
        const CMuiuSettingsLink* link =
            static_cast<const CMuiuSettingsLink*>( &aBaseItem );

        // Get item count
        TInt count = link->iItemLinkArray ?
            link->iItemLinkArray->Count() : KErrNotFound;

        // Find the item from the array
        while ( --count >= 0)
            {
            CMuiuSettingBase* base = ( *link->iItemLinkArray )[count];

            if ( IsVisibleCheck( aBaseItem, aExcludeHidden ) &&
                 base->iItemId == aId )
                {
                return base;
                }
            }
        }

    return NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::GetSubItem()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuDynamicSettingsArray::GetSubItem(
    const CMuiuSettingBase& aBaseItem,
    const TInt aIndex,
    const TBool aExcludeHidden ) const
    {
    CMuiuSettingBase* base = NULL;

    // Make sure the item has subitems
    if ( ItemSubItemCheck( aBaseItem ) )
        {
        // Convert the item to link item
        const CMuiuSettingsLink* link =
            static_cast<const CMuiuSettingsLink*>( &aBaseItem );

        // Get the item count and make sure the required index is
        // below the number of items
        TInt count = link->iItemLinkArray ?
            link->iItemLinkArray->Count() : KErrNotFound;
        count = ( aIndex >= 0 && aIndex < count ) ? count : KErrNotFound;
        TInt index = KErrNotFound;
        TInt item = 0;

        for ( ; index != aIndex && item < count; item++ )
            {
            base = ( *link->iItemLinkArray )[item];

            if ( IsVisibleCheck( *base, aExcludeHidden ) )
                {
                index++;
                }
            }

        // At the end of the items and still not found
        if ( item == count && index != aIndex )
            {
            base = NULL;
            }
        }

    return base;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::GetSubItemIndex()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsArray::GetSubItemIndex(
    const CMuiuSettingsLink& aLinkItem,
    const TUid& aId,
    const TBool aExcludeHidden ) const
    {
    TBool found = EFalse;
    TInt index = KErrNotFound;

    if ( aLinkItem.iItemLinkArray )
        {
        CMuiuSettingBase* subItem = NULL;

        const TInt items = aLinkItem.iItemLinkArray->Count();
        TInt button = KErrNotFound;

        // Seek the item id and return its index
        while ( !found && ++button < items )
            {
            subItem = ( *aLinkItem.iItemLinkArray )[button];

            // Ignore hidden items AND item is not hidden OR
            // check all the items
            if ( IsVisibleCheck( *subItem, aExcludeHidden ) )
                {
                ++index;

                found = ( subItem->iItemId == aId );
                }
            }
        }

    return found ? index : KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::GetSubItemIndex()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsArray::GetSubItemIndex(
    const CMuiuSettingsLink& aLinkItem,
    const TInt aIndex,
    const TBool aActualIndex ) const
    {
    TInt result = KErrNotFound;

    if ( aLinkItem.iItemLinkArray )
        {
        CMuiuSettingBase* radioButton = NULL;
        TInt visibleCount = KErrNotFound;
        TInt resourceCount = KErrNotFound;

        TInt items = aLinkItem.iItemLinkArray->Count();

        for ( TInt index = 0; result < 0 && index < items; index++ )
            {
            radioButton = ( *aLinkItem.iItemLinkArray )[index];

            resourceCount++;
            if ( !IsHidden( *radioButton ) )
                {
                visibleCount++;
                }

            if ( aActualIndex )
                {
                if ( aIndex == visibleCount )
                    {
                    result = resourceCount;
                    }
                }
            else
                {
                if ( aIndex == resourceCount )
                    {
                    result = visibleCount;
                    }
                }
            }
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SearchResourceL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SearchResourceL(
    const TInt aResourceId,
    const TBool aExcludeHidden ) const
    {
    // When hiding all, search through all subfolders and allow the state
    // change
    TMuiuFlags search;
    search.SetFlag( CMuiuDynamicSettingsItemFinder::EFinderSearchSubFolders );
    search.SetFlag( CMuiuDynamicSettingsItemFinder::EFinderResourceSearch );
    search.ChangeFlag(
        CMuiuDynamicSettingsItemFinder::EFinderDoHideCheck, aExcludeHidden );
    iFinder->SetSearchFlags( search );

    // Try to hide items and panic in any error
    TRAPD( error, iFinder->StartSearchL( *iItemArray, aResourceId ) );
    if ( error != KErrNone )
        {
        __ASSERT_DEBUG( EFalse,
            User::Panic( KMuiuDynSettingsArrayPanic, error ) );
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::GetResource()
// ----------------------------------------------------------------------------
//
CMuiuDynFinderItemArray* CMuiuDynamicSettingsArray::GetResource(
    const TInt aResourceId,
    const TBool aExcludeHidden ) const
    {
    // Start the resource search
    TRAP_IGNORE( SearchResourceL( aResourceId, aExcludeHidden ) );

    // Return the pointer to array
    return iFinder->FinderArray();
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::GetResourceLC()
// ----------------------------------------------------------------------------
//
CMuiuDynFinderItemArray* CMuiuDynamicSettingsArray::GetResourceLC(
    const TInt aResourceId,
    const TBool aExcludeHidden ) const
    {
    // Start the resource search
    SearchResourceL( aResourceId, aExcludeHidden );
    TMuiuFlags search = iFinder->SearchFlags();
    search.SetFlag( CMuiuDynamicSettingsItemFinder::EFinderMoveOwnership );
    iFinder->SetSearchFlags( search );

    // Return the copy of the array
    return iFinder->FinderArrayLC();
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::ItemSubArrayCheck()
// ----------------------------------------------------------------------------
//
TBool CMuiuDynamicSettingsArray::ItemSubArrayCheck(
    const CMuiuSettingBase& aBaseItem ) const
    {
    // Check if the item type is menu array
    return aBaseItem.Type() == EMuiuDynSetMenuArray;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::ItemSubItemCheck()
// ----------------------------------------------------------------------------
//
TBool CMuiuDynamicSettingsArray::ItemSubItemCheck(
    const CMuiuSettingBase& aBaseItem ) const
    {
    // if item is not any of the following types it cannot contain subitems
    if ( aBaseItem.iItemType != EMuiuDynSetMenuArray &&
         aBaseItem.iItemType != EMuiuDynSetRadioButtonArray &&
         aBaseItem.iItemType != EMuiuDynSetCheckBoxArray &&
         aBaseItem.iItemType != EMuiuDynSetUndefined &&
         aBaseItem.iItemType != EMuiuDynSetItemMultiLine )
        {
        return EFalse;
        }

    const CMuiuSettingsLink* link =
        static_cast<const CMuiuSettingsLink*>( &aBaseItem );

    // If the item has an array and it contains items, return true
    return ( link->iItemLinkArray != NULL &&
             link->iItemLinkArray->Count() > 0 );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::SetBetweenValues()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::SetBetweenValues(
    TInt& aValue,
    TInt aMin,
    TInt aMax ) const
    {
    // Make a sanity check
    if ( aMin > aMax )
        {
        // Swap the values
        TInt temp = aMin;
        aMin = aMax;
        aMax = temp;
        }

    // Make sure that we are above the minimum
    if ( aValue < aMin )
        {
        aValue = aMin;
        }

    // Make sure that we are below the maximum
    if ( aValue > aMax )
        {
        aValue = aMax;
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::Refresh()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::Refresh()
    {
    if ( iArrayFlags.Flag( EMuiuDynArrayAllowRefresh ) )
        {
        UpdateActiveItemsCount();
        TRAP_IGNORE( EventArrayChangedL( EMuiuDynArrayChanged ) );
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::UpdateActiveItemsCount()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsArray::UpdateActiveItemsCount(
    const TInt aNewCount )
    {
    // If count not provided, update it manually
    if ( aNewCount == KErrNotFound )
        {
        iActiveCount = StackCountItems();
        }
    // Just update the count
    else
        {
        iActiveCount = aNewCount;
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::HandleStackForwardL()
// ----------------------------------------------------------------------------
//
TMuiuStackResult CMuiuDynamicSettingsArray::HandleStackForwardL(
    const TInt aIndex )
    {
    // If index is provided, append the resource to stack immediately
    if ( aIndex != KErrNotFound )
        {
        TInt resource = iItemArray->At( aIndex )->iItemResourceId;

        if ( resource )
            {
            iResourceStack->AppendL( resource );
            }
        else
            {
            return EMuiuStackResultInvalid;
            }
        }
    // Otherwise find the last submenu item in the array
    else
        {
        TInt index = iItemArray->Count();

        while ( --index >= 0 )
            {
            // Check the item is menu item
            if ( iItemArray->At( index )->iItemType == EMuiuDynSetMenuArray )
                {
                iResourceStack->AppendL(
                    iItemArray->At( index )->iItemResourceId );
                index = EMuiuStackResultInvalid;
                }
            }
        }

    return HandleStackChangeL( ETrue );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::HandleStackForwardL()
// ----------------------------------------------------------------------------
//
TMuiuStackResult CMuiuDynamicSettingsArray::HandleStackForwardL(
    const TUid& aId )
    {
    TInt index = iItemArray->Count();

    // Search through the list
    while ( --index >= 0 )
        {
        CMuiuSettingBase* base = iItemArray->At( index );

        // Check the item is menu item
        if ( base->iItemId == aId &&
             base->iItemType == EMuiuDynSetMenuArray )
            {
            iResourceStack->AppendL( base->iItemResourceId );
            index = KErrNotFound;
            }
        else
            {
            return EMuiuStackResultInvalid;
            }
        }

    return HandleStackChangeL( ETrue );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::HandleStackForwardL()
// ----------------------------------------------------------------------------
//
TMuiuStackResult CMuiuDynamicSettingsArray::HandleStackForwardL(
    const CMuiuSettingBase& aBase )
    {
    // Make sure the item is subarray item
    if ( ItemSubArrayCheck( aBase ) )
        {
        // Make sure the array exists and it contains items
        const CMuiuSettingsLink* link =
            static_cast<const CMuiuSettingsLink*>( &aBase );
        __ASSERT_DEBUG( link->iItemLinkArray != NULL,
            User::Panic( KMuiuDynSettingsArrayPanic, KErrNotFound ) );
        const CMuiuSettingBase* nextItem = link->iItemLinkArray->At( 0 ) ;
        __ASSERT_DEBUG( nextItem != NULL,
            User::Panic( KMuiuDynSettingsArrayPanic, KErrNotFound ) );

        // new item has been found, check the resource is not same
        __ASSERT_DEBUG( nextItem->iItemResourceId != StackActiveResource(),
            User::Panic( KMuiuDynSettingsArrayPanic, KErrAlreadyExists ) );

        HandleStackChangeL( ETrue, nextItem->iItemResourceId );

        return EMuiuStackResultSubMenuOpen;
        }

    return EMuiuStackResultOk;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::HandleStackBackwardL()
// ----------------------------------------------------------------------------
//
TMuiuStackResult CMuiuDynamicSettingsArray::HandleStackBackwardL( TInt aCount )
    {
    // Remove the last item from stack number of times
    while ( aCount-- && iResourceStack->Count() )
        {
        iResourceStack->Remove( iResourceStack->Count() - 1 );
        }

    return HandleStackChangeL( EFalse );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::HandleStackChangeL()
// ----------------------------------------------------------------------------
//
TMuiuStackResult CMuiuDynamicSettingsArray::HandleStackChangeL(
    const TInt aForward,
    const TInt aResourceId )
    {
    // If resource id not provided
    if ( aResourceId != KErrNotFound )
        {
        iResourceStack->AppendL( aResourceId );
        }

    // Update the active count
    UpdateActiveItemsCount();

    // Inform of the array change
    EventArrayChangedL(
        aForward ? EMuiuDynArrayStackForward : EMuiuDynArrayStackBackward );

    return EMuiuStackResultOk;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::StackCountItems()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsArray::StackCountItems(
    const TBool aExcludeHidden ) const
    {
    // Set the array finder to contain the current resource in array
    GetResource( StackActiveResource(), aExcludeHidden );

    return iFinder->FinderArray()->Count();
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::StackActiveResource()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsArray::StackActiveResource() const
    {
    return ( *iResourceStack )[iResourceStack->Count() - 1];
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsArray::IsVisibleCheck()
// ----------------------------------------------------------------------------
//
TBool CMuiuDynamicSettingsArray::IsVisibleCheck(
    const CMuiuSettingBase& aBaseItem,
    const TBool aExcludeHidden ) const
    {
    return ( aExcludeHidden && !IsHidden( aBaseItem ) || !aExcludeHidden );
    }

//  End of File

