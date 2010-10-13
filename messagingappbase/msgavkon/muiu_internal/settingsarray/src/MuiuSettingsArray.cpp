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
*     Text and listbox settings
*
*/




// INCLUDE FILES
#include <coemain.h>
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include <eikrutil.h>
#include <aknsettingpage.h>
#include <aknradiobuttonsettingpage.h>
#include <aknpasswordsettingpage.h>
#include <akntextsettingpage.h>
#include <muiu_internal.rsg>
#include "MuiuSettingsArray.h"
#include <e32property.h>
#include <messaginginternalpskeys.h>
#include <StringLoader.h>

// CONSTANTS
const TInt KMuiuSettingsArrayGranularity    = 4;
const TInt KMuiuSettingsArrayVisibleTextLength = 30;
const TInt KPaswordStrLength = 32;
const TInt KTwoItems    = 2;
const TInt KValue = 32;
_LIT(KMuiuSpaceString, " ");
// this is added at the end of the compulsory setting item string
_LIT(KMuiuSettingsArrayCompulsoryAdditionString, "\t*"); 
_LIT(KDirAndMuiuResFileName,"muiu_internal.rsc");





// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------
// CMuiuSettingsArray::NewL
// ---------------------------------------------------------
//
EXPORT_C CMuiuSettingsArray* CMuiuSettingsArray::NewL( TInt aResourceId )
    {
    CMuiuSettingsArray* self = new (ELeave) CMuiuSettingsArray();
    CleanupStack::PushL( self );
    self->ConstructL( aResourceId );
    CleanupStack::Pop( self ); // self
    return self;
    }

// ---------------------------------------------------------
// CMuiuSettingsArray::CMuiuSettingsArray
// ---------------------------------------------------------
//
EXPORT_C CMuiuSettingsArray::CMuiuSettingsArray()
: CArrayFixFlat<TMuiuSettingsItem> ( KMuiuSettingsArrayGranularity ),
iResources( *CCoeEnv::Static() )
    {
    }

// ---------------------------------------------------------
// CMuiuSettingsArray::ConstructL
// ---------------------------------------------------------
//
EXPORT_C void CMuiuSettingsArray::ConstructL( TInt aResourceId )
    {
    TParse parse;

    CEikonEnv* env = CEikonEnv::Static();
    if ( !env )
        {
        User::Leave( KErrGeneral );
        }
    parse.Set( KDirAndMuiuResFileName, &KDC_RESOURCE_FILES_DIR, NULL ); 
    TFileName fileName( parse.FullName() );

    iResources.OpenL( fileName );

    iTempText = HBufC::NewL( KMuiuMaxSettingsTextLength );
    
    if ( aResourceId )
        {
        TResourceReader reader;
        env->CreateResourceReaderLC( reader, aResourceId );
        
        const TInt count = reader.ReadInt16();
        for ( TInt loop = 0; loop < count; loop++ )
            {
            TMuiuSettingsItem item;
            item.iLangSpecificNumConv = ETrue;
            item.iCurrentNumber = 0;
            item.iMuiuSettingsItemArray = 0;
            item.iType = EMuiuSettingsUnknown;
            HBufC* txt = reader.ReadHBufCL();   // message text
            item.iLabelText.Copy( *txt );
            TBool flag = EFalse;
            //If string retrived from resource is "Message sent as" 
            flag = IsMessageSentStringL( txt );

            delete txt;
            
            const TMuiuSettingsType type = ( TMuiuSettingsType ) reader.ReadInt16();
            item.iType = type;
            const TInt maxLength = reader.ReadInt16();
            item.iUserTextMaxLength = maxLength < KMuiuMaxSettingsTextLength ?
                                      maxLength : KMuiuMaxSettingsTextLength;
            
            item.iCompulsory = reader.ReadInt8() != EMuiuSettingsCompulsoryNo;
            
            const TInt array_id = reader.ReadInt32();
            
            if ( type == EMuiuSettingsSelectionList && array_id )
                {
                CDesCArrayFlat* array = 
                                new( ELeave ) CDesCArrayFlat( KMuiuSettingsArrayGranularity );
                CleanupStack::PushL( array );
                TResourceReader reader2;
                env->CreateResourceReaderLC( reader2, array_id );
                
                const TInt count = reader2.ReadInt16();
                for ( TInt loop = 0; loop < count; loop++ )
                    {
                    HBufC* txt = reader2.ReadHBufCL();
                    CleanupStack::PushL( txt );
                    array->AppendL( *txt );
                    CleanupStack::PopAndDestroy(); // txt
                    }
                CleanupStack::PopAndDestroy(); //reader2
                item.iMuiuSettingsItemArray = array;
                if(!flag)
                AppendL( item );
                CleanupStack::Pop( array ); //array
                }
            else
                {
                AppendL( item );
                }
            }
        
        CleanupStack::PopAndDestroy(); //reader
        }
    
    iPasswordFill = EikResourceUtils::ReadTInt16L(
        R_MUIU_SETTINGS_DIALOG_PASSWORD_FILL, env );
    iCompulsoryText = env->AllocReadResourceL( R_MUIU_SETTINGS_DIALOG_COMPULSORY_FILL );
    }

// ---------------------------------------------------------
// CMuiuSettingsArray::~CMuiuSettingsArray
// ---------------------------------------------------------
//
EXPORT_C CMuiuSettingsArray::~CMuiuSettingsArray()
    {
    delete iTempText;
    delete iCompulsoryText;
    for ( TInt loop = Count()-1; loop >= 0; loop--)
        {
        delete At( loop ).iMuiuSettingsItemArray;
        }
    delete iSettingPage;
    iResources.Close();
    }

// ---------------------------------------------------------
// CMuiuSettingsArray::EditItemL
// ---------------------------------------------------------
//
EXPORT_C TBool CMuiuSettingsArray::EditItemL( TInt aIndex, TBool aEnterPressed )
    {
    TBool retValue = EFalse;
    switch ( At( aIndex ).iType )
        {
        case EMuiuSettingsSelectionList:
            retValue = HandleEditListBoxSettingsL( aIndex, aEnterPressed );
            break;
        case EMuiuSettingsEditableText:
            retValue = HandleEditTextSettingsL( aIndex );
            break;
        case EMuiuSettingsNumber:
            retValue = HandleEditNumberL( aIndex );
            break;
        case EMuiuSettingsPassword:
            retValue = HandleEditPasswordL( aIndex );
            break;
        case EMuiuSettingsScNumber:
            retValue = HandleEditScNumberL( aIndex );
            break;
        default:
            break;
        }
    return retValue;
    }

// ---------------------------------------------------------
// CMuiuSettingsArray::HandleEditListBoxSettingsL
// ---------------------------------------------------------
//
TBool CMuiuSettingsArray::HandleEditListBoxSettingsL( TInt aIndex, TBool aEnterPressed )
    {
    TInt currentItem = At( aIndex ).iCurrentNumber;
    TBool langSpecificNumConv = At( aIndex ).iLangSpecificNumConv;
    TBool retValue = EFalse;
    
    if ( aEnterPressed && At( aIndex ).iMuiuSettingsItemArray->Count() == KTwoItems )
        {
        currentItem = !currentItem;
        At( aIndex ).iCurrentNumber = currentItem;
        At( aIndex ).iUserText.Copy( (*At( aIndex ).iMuiuSettingsItemArray)[currentItem] );
        retValue = ETrue;
        }
    else
        {
        const CDesCArrayFlat& origArray = (*At( aIndex ).iMuiuSettingsItemArray);
        const TInt count = origArray.Count();
        CDesCArrayFlat* tempArray = new (ELeave ) CDesCArrayFlat( KMuiuSettingsArrayGranularity );
        CleanupStack::PushL( tempArray );
        for ( TInt loop = 0; loop < count; loop++ )
            {
            HBufC* tempBuf = ( origArray )[loop].AllocL();
            CleanupStack::PushL( tempBuf );
            TPtr tempBufPtr = tempBuf->Des();
            if ( langSpecificNumConv )
                {
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tempBufPtr );
                }
            tempArray->AppendL( *tempBuf );
            CleanupStack::PopAndDestroy( tempBuf );
            }
        iSettingPage = new ( ELeave )CAknRadioButtonSettingPage( 
                             R_MUIU_SETTINGS_DIALOG_LISTBOX, currentItem, tempArray );
        iSettingPage->SetSettingTextL( At( aIndex ).iLabelText );
        if ( iSettingPage->ExecuteLD(CAknSettingPage::EUpdateWhenChanged) )
            {
            At( aIndex ).iCurrentNumber = currentItem;
            At( aIndex ).iUserText.Copy( (*At( aIndex ).iMuiuSettingsItemArray)[currentItem]  );
            retValue = ETrue;
            }
        CleanupStack::PopAndDestroy( tempArray ); 
        iSettingPage = NULL;
        }
    return retValue;
    }

// ---------------------------------------------------------
// CMuiuSettingsArray::HandleEditTextSettingsL
// ---------------------------------------------------------
//
TBool CMuiuSettingsArray::HandleEditTextSettingsL( TInt aIndex )
    {
    TBuf<KMuiuMaxSettingsTextLength> textToEdit =
        At( aIndex ).iUserText.Left(At( aIndex ).iUserTextMaxLength );
    TBool retValue = EFalse;
    iSettingPage = new( ELeave )CAknTextSettingPage(R_MUIU_SETTINGS_DIALOG_TEXT, textToEdit );
    iSettingPage->SetSettingTextL( At( aIndex ).iLabelText );
    iSettingPage->ConstructL( );
    STATIC_CAST( CAknTextSettingPage*, iSettingPage)->TextControl()
        ->SetTextLimit( At( aIndex ).iUserTextMaxLength );
    
    if ( iSettingPage->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        At( aIndex ).iUserText.Copy( textToEdit );
        retValue = ETrue;
        }
    iSettingPage = NULL;
    return retValue;
    }

// ---------------------------------------------------------
// CMuiuSettingsArray::HandleEditNumberL
// ---------------------------------------------------------
//
TBool CMuiuSettingsArray::HandleEditNumberL( TInt aIndex )
    {
    TInt numberToEdit = At( aIndex ).iCurrentNumber;
    TBool retValue = EFalse;
    iSettingPage = new( ELeave )CAknIntegerSettingPage( 
                   R_MUIU_SETTINGS_DIALOG_NUMBER, 
                   numberToEdit, 
                   CAknIntegerSettingPage::EInvalidValueNotOffered );
    iSettingPage->SetSettingTextL( At( aIndex ).iLabelText );
    
    iSettingPage->ConstructL();
    static_cast<CAknIntegerSettingPage*>(iSettingPage)->IntegerEditorControl()->
                 SetAknEditorNumericKeymap( EAknEditorPlainNumberModeKeymap );
    
    if ( iSettingPage->ExecuteLD(CAknSettingPage::EUpdateWhenChanged) )
        {
        At( aIndex ).iCurrentNumber = numberToEdit;
        At( aIndex ).iUserText.Num( numberToEdit );
        retValue = ETrue;
        }
    iSettingPage = NULL;
    return retValue;
    }

// ---------------------------------------------------------
// CMuiuSettingsArray::HandleEditScNumberL
// ---------------------------------------------------------
//
TBool CMuiuSettingsArray::HandleEditScNumberL( TInt aIndex )
    {
    TBuf<KMuiuMaxSettingsTextLength> textToEdit = At( aIndex ).iUserText;
    TBool retValue = EFalse;
    if ( At( aIndex ).iLangSpecificNumConv )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( textToEdit );
        }
    iSettingPage = new( ELeave )CAknTextSettingPage( 
                   R_MUIU_SETTINGS_DIALOG_SC_NUMBER, textToEdit );
    iSettingPage->SetSettingTextL( At( aIndex ).iLabelText );
    
    if ( iSettingPage->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        AknTextUtils::ConvertDigitsTo( textToEdit , EDigitTypeWestern );
        At( aIndex ).iUserText.Copy( textToEdit );
        retValue = ETrue;
        }
    iSettingPage = NULL;
    return retValue;
    }

// ---------------------------------------------------------
// CMuiuSettingsArray::HandleEditPasswordL
// ---------------------------------------------------------
//
TBool CMuiuSettingsArray::HandleEditPasswordL( TInt aIndex )
    {
    TBuf<KPaswordStrLength> newPassword = At( aIndex ).iUserText.Left( KValue );
    TBuf<KPaswordStrLength> oldPassword;
    oldPassword.Copy ( newPassword );
    TBool retValue = EFalse;
    
    iSettingPage = new( ELeave )CAknAlphaPasswordSettingPage(
        R_MUIU_SETTINGS_DIALOG_PASSWORD, newPassword, oldPassword );
    iSettingPage->SetSettingTextL( At( aIndex ).iLabelText );
    iSettingPage->ConstructL( );
    STATIC_CAST( CAknAlphaPasswordSettingPage*, iSettingPage )->AlphaPasswordEditor()
        ->SetMaxLength( At( aIndex ).iUserTextMaxLength );
    
    if ( iSettingPage->ExecuteLD(CAknSettingPage::EUpdateWhenChanged) )
        {
        At( aIndex ).iUserText.Copy( newPassword );
        retValue = ETrue;
        }
    iSettingPage = NULL;
    return retValue;
    }


// ---------------------------------------------------------
// CMuiuSettingsArray::MdcaCount
// ---------------------------------------------------------
//
EXPORT_C TInt CMuiuSettingsArray::MdcaCount() const
    {
    return Count();
    }


// ---------------------------------------------------------
// CMuiuSettingsArray::MdcaPoint
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMuiuSettingsArray::MdcaPoint( TInt aIndex ) const
    {
    TPtr tempText = iTempText->Des();
    TBuf<KMuiuMaxSettingsTextLength> tempSettingText;
    tempSettingText.Zero();
    
    tempText.Zero();
    tempText.Append( KMuiuSpaceString );
    tempText.Append( KColumnListSeparator );
    tempText.Append( At( aIndex ).iLabelText );
    tempText.Append( KColumnListSeparator );
    tempText.Append( KColumnListSeparator );
    
    if ( At( aIndex ).iType == EMuiuSettingsSelectionList )
        {
        tempSettingText = At( aIndex ).iMuiuSettingsItemArray->MdcaPoint(
            At( aIndex ).iCurrentNumber ).Left( KMuiuSettingsArrayVisibleTextLength );
        }
    else if ( At( aIndex ).iType == EMuiuSettingsPassword )
        {
        TInt length = At( aIndex ).iUserText.Length();
        length = length > KMuiuSettingsArrayVisibleTextLength ?
        KMuiuSettingsArrayVisibleTextLength : length;
        if ( length > 0 )
            {
            TChar passFill = TChar( iPasswordFill );
            tempSettingText.AppendFill( passFill, length );
            }
        }
    else 
        {
        if ( At( aIndex ).iUserText.Length() )
            {
            tempSettingText = At( aIndex ).iUserText.Left( KMuiuSettingsArrayVisibleTextLength );
            }
        }
    
    if ( tempSettingText.Length() )
        {
        tempText.Append( tempSettingText );
        }
    else
        {
        if ( At( aIndex ).iCompulsory )
            {
            tempText.Append( *iCompulsoryText );
            }
        else
            {
            tempText.Append( KMuiuSpaceString );
            }
        }
    
    if ( At( aIndex ).iCompulsory )
        {
        tempText.Append( KMuiuSettingsArrayCompulsoryAdditionString );
        }

    if ( At( aIndex ).iLangSpecificNumConv )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tempText );
        }

    return tempText;
    }

// ---------------------------------------------------------
// CMuiuSettingsArray::IsMessageSentStringL
// ---------------------------------------------------------
//
TBool CMuiuSettingsArray::IsMessageSentStringL(HBufC* aString)
    {
    TInt flag = EFalse;
    HBufC* string = StringLoader::LoadLC( R_MUIU__MCE_SETTINGS_SMS_CONVERSION);
    if (aString->Compare(string->Des()) == 0)
        {
         TInt val = 0;
         TInt ret = RProperty::Get( KPSUidMuiu, KMuiuRemoveMsgSentSettings, val );
         if ( ret == KErrNone && val != 0 )
             {
             flag = ETrue;
             RProperty::Set( KPSUidMuiu, KMuiuRemoveMsgSentSettings, 0) ;
             }
         }
    CleanupStack::PopAndDestroy( string );
    return flag;
    }
//  End of File
