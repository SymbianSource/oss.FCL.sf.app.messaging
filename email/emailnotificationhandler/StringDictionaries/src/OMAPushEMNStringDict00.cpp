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
* Description: COMAPushEMNStringDict00
*		
*
*/


#include <e32std.h>
#include <ecom/implementationproxy.h>

#include <xml/plugins/dictionarycodepage.h>

#include <xml/xmlframeworkerrors.h>

#include "OMAPushEMNStringDict00.h"

// These headers are automatically generated from \StringDictionaries\src\*.st 
// files.
#include "omapushemntagtable.h"
#include "omapushemnattributetable.h"
#include "omapushemnattributevaluetable.h"

using namespace Xml;

//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::NewL
//-----------------------------------------------------------------------------
MStringDictionary* COMAPushEMNStringDict00::NewL( TAny* aStringPool )
    {
    COMAPushEMNStringDict00* self = 
        new(ELeave) COMAPushEMNStringDict00(
            reinterpret_cast<RStringPool*>( aStringPool ) );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return ( static_cast<MStringDictionary*>( self ) );
    }

//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::COMAPushEMNStringDict00
//-----------------------------------------------------------------------------
COMAPushEMNStringDict00::COMAPushEMNStringDict00( 
    RStringPool* aStringPool ) :  iStringPool(*aStringPool), iCodePage(0)
    {
    // do nothing;
    }

//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::ConstructL
//-----------------------------------------------------------------------------
void COMAPushEMNStringDict00::ConstructL()
    {
    // we don't own this stringpool
    iStringPool.OpenL(OMAPushEMNStringDict00TagTable::Table);
    iStringPool.OpenL(OMAPushEMNStringDict00AttributeTable::Table);
    iStringPool.OpenL(OMAPushEMNStringDict00AttributeValueTable::Table);

    iCodepage00Table = CDictionaryCodePage::NewL(
    &OMAPushEMNStringDict00TagTable::Table, 
    &OMAPushEMNStringDict00AttributeTable::Table, 
    &OMAPushEMNStringDict00AttributeValueTable::Table, 
    0); // codepage

    // Construct the correlation mapping
    iCodepage00Table->ConstructIndexMappingL(
    iTagCodePage00, CDictionaryCodePage::EStringTypeElement );
    iCodepage00Table->ConstructIndexMappingL(
    iAttributeCodePage00, CDictionaryCodePage::EStringTypeAttribute );
    iCodepage00Table->ConstructIndexMappingL(
    iAttributeValueCodePage00, CDictionaryCodePage::EStringTypeAttributeValue );
    }

//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::Release
//-----------------------------------------------------------------------------
void COMAPushEMNStringDict00::Release()
    {
    delete( this );
    }

//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::~COMAPushEMNStringDict00
//-----------------------------------------------------------------------------
COMAPushEMNStringDict00::~COMAPushEMNStringDict00()
    {
    if ( iCodepage00Table )
        {
        delete iCodepage00Table;
        }
    }

//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::ElementL
//-----------------------------------------------------------------------------
void COMAPushEMNStringDict00::ElementL(
    TInt aToken, RString& aElement) const
    {
    TInt index = iCodepage00Table->StringPoolIndexFromToken(
          aToken, CDictionaryCodePage::EStringTypeElement );

    if ( index == KErrXmlStringPoolTableNotFound )
        {
        User::Leave( KErrXmlUnsupportedElement );
        }

    // when we have multiple codepages per string dictionary we'd do something 
    // like iCodepageTable[n]->StringTable()
    aElement = iStringPool.String(
        index,
        *( iCodepage00Table->StringTable(
            CDictionaryCodePage::EStringTypeElement ) ) );
    }

//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::AttributeL
//-----------------------------------------------------------------------------
void COMAPushEMNStringDict00::AttributeL(
    TInt aToken, RString& aAttribute ) const
    {
    TInt index = iCodepage00Table->StringPoolIndexFromToken(
        aToken, CDictionaryCodePage::EStringTypeAttribute );

    if ( index == KErrXmlStringPoolTableNotFound )
        {
        User::Leave( KErrXmlUnsupportedAttribute );
        }

    // when we have multiple codepages per string dictionary we'd do something
    // like iCodepageTable[n]->StringTable()
    aAttribute = iStringPool.String(
        index,
        *( iCodepage00Table->StringTable(
            CDictionaryCodePage::EStringTypeAttribute ) ) );
    };

//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::AttributeValuePairL
//-----------------------------------------------------------------------------
void COMAPushEMNStringDict00::AttributeValuePairL(
    TInt aToken, RString& aAttribute, RString& aValue ) const
    {
    AttributeL( aToken, aAttribute );
    AttributeValueL( aToken, aValue );
    };
  
//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::AttributeValueL
//-----------------------------------------------------------------------------
void COMAPushEMNStringDict00::AttributeValueL(
    TInt aToken, RString& aValue ) const
    {
    TInt index = iCodepage00Table->StringPoolIndexFromToken(
        aToken, CDictionaryCodePage::EStringTypeAttributeValue );

    if ( index == KErrXmlStringPoolTableNotFound )
        {
        User::Leave( KErrXmlUnsupportedAttributeValue );
        }

    // when we have multiple codepages per string dictionary we'd do something
    // like iCodepageTable[n]->StringTable()
    aValue = iStringPool.String(
        index,
        *( iCodepage00Table->StringTable(
            CDictionaryCodePage::EStringTypeAttributeValue ) ) );
    };

//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::CompareThisDictionary
//-----------------------------------------------------------------------------
TBool COMAPushEMNStringDict00::CompareThisDictionary(
    const RString& aDictionaryDescription ) const
    {
    // If this string dictionary has many codepages then all these comparisons should go here.
    // Remember, the string dictionary loads up all the RStringTables into its RStringPool
    // on construction. So if the comparison fails we do not have it.
    return (
        ( aDictionaryDescription == iStringPool.String( 
            OMAPushEMNStringDict00TagTable::EUri, 
            OMAPushEMNStringDict00TagTable::Table ) ) ||
        ( aDictionaryDescription == iStringPool.String( 
            OMAPushEMNStringDict00TagTable::EPublicId, 
            OMAPushEMNStringDict00TagTable::Table ) ) ||
        ( aDictionaryDescription == iStringPool.String( 
            OMAPushEMNStringDict00TagTable::EFormalPublicId, 
            OMAPushEMNStringDict00TagTable::Table ) ) );
    }

//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::SwitchCodePage
//-----------------------------------------------------------------------------
TInt COMAPushEMNStringDict00::SwitchCodePage( TInt aCodePage )
    {
    // We only have one codepage sp can't switch
    if ( aCodePage != iCodePage )
        {
        return KErrXmlMissingStringDictionary;
        }
    return iCodePage;
    }

//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::PublicIdentifier
//-----------------------------------------------------------------------------
void COMAPushEMNStringDict00::PublicIdentifier( RString& aPubId )
    {
    aPubId = iStringPool.String(
        OMAPushEMNStringDict00TagTable::EFormalPublicId,
        *( iCodepage00Table->StringTable(
            CDictionaryCodePage::EStringTypeElement ) ) );
    }

//-----------------------------------------------------------------------------
// COMAPushEMNStringDict00::NamespaceUri
//-----------------------------------------------------------------------------
void COMAPushEMNStringDict00::NamespaceUri( RString& aUri )
    {
    aUri = iStringPool.String(
        OMAPushEMNStringDict00TagTable::EUri,
        *( iCodepage00Table->StringTable(
            CDictionaryCodePage::EStringTypeElement ) ) );
    }

//-----------------------------------------------------------------------------
// ECOM PLUGIN LOADING STUFF
//-----------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = {
  IMPLEMENTATION_PROXY_ENTRY( 0x1027508B, COMAPushEMNStringDict00::NewL )
};

//-----------------------------------------------------------------------------
// ImplementationGroupProxy
//-----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / 
        sizeof( TImplementationProxy );

    return ImplementationTable;
    }

