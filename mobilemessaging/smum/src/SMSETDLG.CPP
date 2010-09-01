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
*     Help classes to handle Service Centres and settings.
*
*/



// INCLUDES
#include <eikclb.h>                     // CEikColumnListBox
#include <StringLoader.h>               // StringLoader
#include <smutset.h>                    // CSmsSettings
#include <SMUM.rsg>                     // resource ids
#include "SMSETDLG.H"                   // dialogs
#include "SmumSettingsDialogGSM.h"         //ESmumSendOptServiceCentreInUseLBI
//  LOCAL CONSTANTS AND MACROS
const TInt KSmumNumberOfSeparatingColumnCharacters = 2;
const TInt KSmumNumberOfEndColumnCharacters = 1;
const TInt KMaxLengthOfNameAndNumberString = 100;
const TUint KSpaceChar = ' ';
_LIT(KSmumSpaceString, " ");

//  MEMBER FUNCTIONS


/**
*  TSmumSCNameInfo - class
*/

// ---------------------------------------------------------
// TSmumSCNameInfo::TSmumSCNameInfo
//
// ---------------------------------------------------------
TSmumSCNameInfo::TSmumSCNameInfo( const TDesC& aName, const TDesC& aAddress )
:iNameAndAddress( aName ), iNameLength( 0 )
	{
	iNameAndAddress.Append( KColumnListSeparator );
    iNameAndAddress.Append( KColumnListSeparator );
    iNameAndAddress.Append( aAddress );
    iNameAndAddress.Append( KColumnListSeparator );
    iNameLength = aName.Length();
	}

// ---------------------------------------------------------
// TSmumSCNameInfo::~TSmumSCNameInfo
//
// ---------------------------------------------------------
TSmumSCNameInfo::~TSmumSCNameInfo()
    {
    }

// ---------------------------------------------------------
// TSmumSCNameInfo::TSmumSCNameInfo
//
// ---------------------------------------------------------
TSmumSCNameInfo::TSmumSCNameInfo( const TDesC& aName )
:iNameAndAddress( aName ), iNameLength ( 0 )
	{
    iNameLength = aName.Length();
	}

// ---------------------------------------------------------
// TSmumSCNameInfo::Name
//
// ---------------------------------------------------------
TPtrC TSmumSCNameInfo::Name() const
	{
	return iNameAndAddress.Left( iNameLength );
	}

// ---------------------------------------------------------
// TSmumSCNameInfo::Address
//
// ---------------------------------------------------------
TPtrC TSmumSCNameInfo::Address() const
	{
	const TInt nameAndAddressLength = iNameAndAddress.Length();
	if ( iNameLength == nameAndAddressLength )
        {
		return TPtrC( KNullDesC );
        }

	return iNameAndAddress.Mid(
				iNameLength + KSmumNumberOfSeparatingColumnCharacters, 
				nameAndAddressLength - iNameLength - 
                KSmumNumberOfEndColumnCharacters - KSmumNumberOfSeparatingColumnCharacters );
	}

/**
*  CSmumSCArray - class
*/

// ---------------------------------------------------------
// CSmumSCArray::CSmumSCArray
//
// ---------------------------------------------------------
CSmumSCArray::CSmumSCArray( TInt aGranularity )
    :CArrayFixFlat<TSmumSCNameInfo>( aGranularity )
    {
    }

// ---------------------------------------------------------
// CSmumSCArray::~CSmumSCArray
//
// ---------------------------------------------------------
CSmumSCArray::~CSmumSCArray()
    {
    delete iStringForSCName;
    }

// ---------------------------------------------------------
// CSmumSCArray::NewL
//
// ---------------------------------------------------------
CSmumSCArray* CSmumSCArray::NewL( TInt aGranularity )
    {
    CSmumSCArray* self = new ( ELeave ) CSmumSCArray( aGranularity );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------
// CSmumSCArray::ConstructL
//
// ---------------------------------------------------------
void CSmumSCArray::ConstructL()
    {
    iStringForSCName = HBufC::NewL( KMaxLengthOfNameAndNumberString );
    }

// ---------------------------------------------------------
// CSmumSCArray::MdcaCount
//
// ---------------------------------------------------------
TInt CSmumSCArray::MdcaCount() const
    {
    return Count();
    }

// ---------------------------------------------------------
// CSmumSCArray::MdcaPoint
//
// ---------------------------------------------------------
TPtrC CSmumSCArray::MdcaPoint( TInt aIndex ) const
    {
    TPtr tempText = iStringForSCName->Des();
    tempText.Zero();

    if ( Count())
        {
        tempText.Append( KSpaceChar );
        tempText.Append( KColumnListSeparator );
        tempText.Append( At( aIndex ).Name());
        }

    return tempText;
    }

// ---------------------------------------------------------
// CSmumSCArray::SCNameInfo
//
// ---------------------------------------------------------
TSmumSCNameInfo& CSmumSCArray::SCNameInfo( TInt aIndex )
    {
    return At( aIndex );
    }


/**
*  CSmumSettingsArray - class
*/

// ---------------------------------------------------------
// CSmumSettingsArray::NewL
//
// ---------------------------------------------------------
CSmumSettingsArray* CSmumSettingsArray::NewL( TInt aResourceId )
    {
    CSmumSettingsArray* self = new ( ELeave ) CSmumSettingsArray();
    CleanupStack::PushL( self );
    self->ConstructL( aResourceId ); 
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------
// CSmumSettingsArray::ConstructL
//
// ---------------------------------------------------------
void CSmumSettingsArray::ConstructL( TInt aResourceId )
    {
    iReplyPathString = StringLoader::LoadL( R_QTN_SMS_REPLY_PATH );
    CMuiuSettingsArray::ConstructL( aResourceId );
    }

// ---------------------------------------------------------
// CSmumSettingsArray::CSmumSettingsArray
//
// ---------------------------------------------------------
CSmumSettingsArray::CSmumSettingsArray()
    {
    }

// ---------------------------------------------------------
// CSmumSettingsArray::~CSmumSettingsArray
//
// ---------------------------------------------------------
CSmumSettingsArray::~CSmumSettingsArray()
    {
    delete iReplyPathString;
    }

// ---------------------------------------------------------
// CSmumSettingsArray::SetReplyPath
//
// ---------------------------------------------------------
void CSmumSettingsArray::SetReplyPath( TBool aReplyPathProvided )
    {
    iReplyPathProvided = aReplyPathProvided;
    }
    
// ---------------------------------------------------------
// CSmumSettingsArray::ReplyPathProvided
//
// ---------------------------------------------------------
TBool CSmumSettingsArray::ReplyPathProvided()
    {
    return iReplyPathProvided;
    }

// ---------------------------------------------------------
// CSmumSettingsArray::MdcaPoint
//
// ---------------------------------------------------------
TPtrC CSmumSettingsArray::MdcaPoint( TInt aIndex ) const
    {
    if ( iReplyPathProvided && ESmumSendOptServiceCentreInUseLBI == aIndex )
        {
        TPtr tempText = iTempText->Des();
        tempText.Zero();
        tempText.Append( KSmumSpaceString );
        tempText.Append( KColumnListSeparator );
        tempText.Append( At( aIndex ).iLabelText );
        tempText.Append( KColumnListSeparator );
        tempText.Append( KColumnListSeparator );
        tempText.Append( *iReplyPathString );
        return tempText;
        }
    else
        {
        return CMuiuSettingsArray::MdcaPoint( aIndex );
        }
    }

//  End of File
