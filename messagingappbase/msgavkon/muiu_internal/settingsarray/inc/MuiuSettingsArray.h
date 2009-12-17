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
*     Text, number, pasword, sc number and listbox settings
*
*/



#ifndef MUIUSETTINGSARRAY_H
#define MUIUSETTINGSARRAY_H

//  INCLUDES
#include <MsgArrays.h> // CUidNameArray
#include <ConeResLoader.h>
#include <muiusettingsarray.hrh>

// CONSTANTS
const TInt KMuiuMaxSettingsTextLength = 100;

// DATA TYPES
typedef TBuf <KMuiuMaxSettingsTextLength> TMuiuSettingsText;

// FORWARD DECLARATIONS
class CEikTextListBox;
class CUidNameArray;
class MMceAccountManager;
class CMuiuMtmSettingsArray;
class CAknSettingPage;

// CLASS DECLARATION
struct TMuiuSettingsItem
    {
    TMuiuSettingsText   iLabelText;
    TMuiuSettingsType   iType;
    TBool               iCompulsory;			//Default value EFalse
    TMuiuSettingsText   iUserText;
    TInt                iUserTextMaxLength;
    TInt                iCurrentNumber;
    CDesCArrayFlat*     iMuiuSettingsItemArray;
    TBool               iLangSpecificNumConv;		//Default value ETrue
    };



/**
* Text, number, pasword, sc number and listbox settings.
*/
class CMuiuSettingsArray :
    public CArrayFixFlat<TMuiuSettingsItem>,
    public MDesCArray
    {
    public:
       /*
        * Two phase Symbian OS constructor
        * @param aResourceId: resource id. Resource structure defined by MUIU_SETTINGS_ARRAY.
        */
        IMPORT_C static CMuiuSettingsArray* NewL( TInt aResourceId );

       /*
        * destructor
        */
        IMPORT_C virtual ~CMuiuSettingsArray();

       /*
        * Lauches avkon setting page for item aIndex (list setting page or text setting page)
        * @param aIndex: item to be edited
        * @param aEnterPressed: ETrue if enter pressed. Used only in list setting page if two
        *        items then toggle between two items.
        * @return ETrue if edited, EFalse if no changes.
        */
        IMPORT_C TBool EditItemL( TInt aIndex, TBool aEnterPressed = EFalse );

    public:
       /*
        * from MDesCArray
        */
        IMPORT_C virtual TInt MdcaCount() const;

       /*
        * from MDesCArray
        */
        IMPORT_C virtual TPtrC MdcaPoint(TInt aIndex) const;

    private:

       /*
        * Opens list box setting page
        */
        TBool HandleEditListBoxSettingsL( TInt aIndex, TBool aEnterPressed = EFalse );

       /*
        * Opens text setting page
        */
        TBool HandleEditTextSettingsL( TInt aIndex );

       /*
        * Opens text setting page
        */
        TBool HandleEditNumberL( TInt aIndex );

       /*
        * Opens text setting page
        */
        TBool HandleEditPasswordL( TInt aIndex );

        /*
        * Opens Sc number edit setting page
        */
        TBool HandleEditScNumberL( TInt aIndex );

    protected:
       /*
        * C++ default constructor
        */
        IMPORT_C CMuiuSettingsArray();

       /*
        * Symbian OS constructor
        */
        IMPORT_C void ConstructL( TInt aResourceId );

    protected:
        HBufC*                  iTempText; // use this if you override MdcaPoint
    private:
        HBufC*                  iCompulsoryText;
        CAknSettingPage*        iSettingPage;
        TUint16                 iPasswordFill;
        RConeResourceLoader     iResources;
    };


#endif // MUIUSETTINGSARRAY_H

// End of file
