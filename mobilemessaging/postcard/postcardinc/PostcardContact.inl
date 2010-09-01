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
*     Postcard application's interface to contacts (Phonebook2 and
*     virtual phonebook)
*
*/



// Inline Functions

// -----------------------------------------------------------------------------
// CPostcardContact::IsValid()
// -----------------------------------------------------------------------------
//
inline TBool CPostcardContact::IsValid() const
    {
    return iStoreContact != NULL;
    }

// -----------------------------------------------------------------------------
// CPostcardContact::LocToFieldTypeParam
// -----------------------------------------------------------------------------
//
inline TVPbkFieldTypeParameter CPostcardContact::LocToFieldTypeParam(
    TLocation aLocation )
    {
    return iLocToFieldTypeParam[aLocation];
    }

// -----------------------------------------------------------------------------
// CPostcardContact::AddrFieldToSubFieldType
// -----------------------------------------------------------------------------
//
inline TVPbkSubFieldType CPostcardContact::AddrFieldToSubFieldType(
    TAddressField aAddressField )
    {
    return iAddrFieldToSubFieldType[aAddressField];
    }

// -----------------------------------------------------------------------------
// CPostcardContact::ControlIdToAddrField
// -----------------------------------------------------------------------------
//
inline CPostcardContact::TAddressField CPostcardContact::ControlIdToAddrField(
    TInt aControlId )
    {
    aControlId -= EPostcardAddressInfo;
    __ASSERT_DEBUG(aControlId >= 0 && aControlId < KNumAddressFields,
        Panic(EPostcardPanicCoding));
    return iControlIdToAddrField[aControlId];
    }

// End of File
