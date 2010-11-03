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
*  Handles mce uid name array   
*
*/



// INCLUDE FILES

#include <AknUtils.h> // AknTextUtils
#include "MceSettingsArrays.h"


// LOCAL CONSTANTS AND MACROS

const TInt KMceArraysGranularity = 4;
const TInt KMceArraysTextLength  = 100;
const TInt KMceSettingsMailboxNameTextLength = 20;

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CMceUidNameArray::NewL
// ----------------------------------------------------
EXPORT_C CMceUidNameArray* CMceUidNameArray::NewL( CMceUidNameArray::TMceUidNameArrayType aType )
    {
    CMceUidNameArray* self = new (ELeave) CMceUidNameArray( aType );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self ); 
    return self;
    }

// ----------------------------------------------------
// CMceUidNameArray::CMceUidNameArray
// ----------------------------------------------------
CMceUidNameArray::CMceUidNameArray( TMceUidNameArrayType aType) :
    CUidNameArray( KMceArraysGranularity ),
    iListType( aType )
    {
    }

// ----------------------------------------------------
// CMceUidNameArray::~CMceUidNameArray
// ----------------------------------------------------
EXPORT_C CMceUidNameArray::~CMceUidNameArray()
    {
    delete iText;
    }

// ----------------------------------------------------
// CMceUidNameArray::ConstructL
// ----------------------------------------------------
void CMceUidNameArray::ConstructL()
    {
    iText = HBufC::NewL( KMceArraysTextLength );
    }


// ----------------------------------------------------
// CMceUidNameArray::MdcaPoint
// ----------------------------------------------------
TPtrC CMceUidNameArray::MdcaPoint( TInt aIndex ) const
    {
    TPtr tempText = iText->Des();
    tempText.Zero();
    if ( iListType == EMtmList )
        {
        tempText.Append( KColumnListSeparator );
        tempText.Append( At( aIndex ).iName );
        }
    else
        {
        tempText.Append( KColumnListSeparator );
        TBuf<KMceSettingsMailboxNameTextLength> tempBuffer;
        if ( At( aIndex ).iName.Length() > 
            At( aIndex ).iName.Left( KMceSettingsMailboxNameTextLength ).Length() )
            {
            tempBuffer.Copy( At( aIndex ).iName.Left( KMceSettingsMailboxNameTextLength - 1 ) );
            tempBuffer.Append( CEditableText::EEllipsis );
            }
        else
            {
            tempBuffer.Copy( At( aIndex ).iName.Left( KMceSettingsMailboxNameTextLength ) );
            }
        tempText.Append( tempBuffer );
        }
    return tempText;
    }




//  End of File
