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
*     Implements character converter for application.
*
*/


// INCLUDE FILES
#include <bldvariant.hrh>
#include <eikenv.h>
#include <miutconv.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include <cimconvertcharconv.h>
#endif
#include "MsgMailViewerCharConv.h"

// CONSTANTS
const TInt KBlockSize = 64;


// ================= MEMBER FUNCTIONS =======================

// Default constructor
CMsgMailViewerCharConv::CMsgMailViewerCharConv()
: iState( CCnvCharacterSetConverter::KStateDefault )
    { 
    }

// destructor
CMsgMailViewerCharConv::~CMsgMailViewerCharConv()
    {
    delete iCharConv;
    delete iImCharConv;
    delete iTarget8;
    delete iTarget16;
    }

// Symbian OS default constructor can leave.
void CMsgMailViewerCharConv::ConstructL( TUint aCharSet )
    {
    if (aCharSet > 0)
        {
        iCharConv = CCnvCharacterSetConverter::NewL();
        iImCharConv = CImConvertCharconv::NewL(
            *iCharConv, CEikonEnv::Static()->FsSession());			// CSI: 27 # Must be used because of iEikEnv 
																	// is not accessible.
        TBool found = iImCharConv->PrepareToConvertToFromOurCharsetL(aCharSet);
        if (!found)
            {
            User::Leave( KErrNotFound );
            }
        }
    }

// Two-phased constructor.
CMsgMailViewerCharConv* CMsgMailViewerCharConv::NewL(
        TUint aCharSet )     // CMsgMailViewerApp reference
    {
    CMsgMailViewerCharConv* self = new (ELeave) CMsgMailViewerCharConv();
    CleanupStack::PushL( self );
    self->ConstructL( aCharSet );
    CleanupStack::Pop();

    return self;
    }

void CMsgMailViewerCharConv::PrepareToConvertFromUnicodeL( const TDesC16& aSource )
    {
    iSource16.Set( aSource );

    delete iTarget8;
    iTarget8 = NULL;
    iTarget8 = HBufC8::NewL( iSource16.Length()*2 );		// CSI: 47 # For conversion.

    iFromUnicode = ETrue;
    iFailCount = 0;
    }

void CMsgMailViewerCharConv::PrepareToConvertToUnicodeL( const TDesC8& aSource )
    {
    iSource8.Set( aSource );

    delete iTarget16;
    iTarget16 = NULL;
    iTarget16 = HBufC16::NewL( iSource8.Length() );

    iFromUnicode = EFalse;
    }

HBufC16* CMsgMailViewerCharConv::GetUnicodeText()
    {
    HBufC16* result = iTarget16;
    iTarget16 = NULL;

    return result;
    }
    
HBufC8* CMsgMailViewerCharConv::GetForeignText()
    {
    HBufC8* result = iTarget8;
    iTarget8 = NULL;

    return result;
    }

TInt CMsgMailViewerCharConv::GetFailedCount() const
    {
    return iFailCount;
    }
    
void CMsgMailViewerCharConv::AppendTextL( const TDesC16& aData )
    {
    TPtr target( iTarget16->Des() );
    TInt length = target.Length() + aData.Length();
    if( length > target.MaxLength() )
        {
        iTarget16 = iTarget16->ReAllocL( length );
        target.Set( iTarget16->Des() );
        }

    target.Append( aData );
    }

void CMsgMailViewerCharConv::AppendTextL( const TDesC8& aData )
    {
    TPtr8 target( iTarget8->Des() );
    TInt length = target.Length() + aData.Length();
    if( length > target.MaxLength() )
        {
        iTarget8 = iTarget8->ReAllocL( length );
        target.Set( iTarget8->Des() );
        }

    target.Append( aData );
    }

void CMsgMailViewerCharConv::StepL()
    {
    if( iFromUnicode )
        {
        StepFromUnicodeL();
        }
    else
        {
        StepToUnicodeL();
        }
    }

void CMsgMailViewerCharConv::StepFromUnicodeL()
    {
    if (iImCharConv)
        {
        TBuf8<KBlockSize> buf;
        TInt firstUnconvertdChar;
        TInt numUnconvertedChars;
        
        TInt length (iImCharConv->ConvertFromOurCharsetL( iSource16, buf, 
            numUnconvertedChars, 
            firstUnconvertdChar));
        
        iFailCount += numUnconvertedChars;
        User::LeaveIfError( length );
        
        iSource16.Set( iSource16.Right( length ) );
        
        AppendTextL( buf );
        }
    else
        {
        // No converter, just copy source to target
        iTarget8->Des().Copy(iSource16);
        iSource16.Set( KNullDesC() );
        }
    }

void CMsgMailViewerCharConv::StepToUnicodeL()
    {
    if (iImCharConv)
        {
        TBuf16<KBlockSize> buf;
        TInt firstUnconvertdChar;
        TInt numUnconvertedChars;
        
        TInt length (iImCharConv->ConvertToOurCharsetL( iSource8, buf, 
            numUnconvertedChars, 
            firstUnconvertdChar));
        
        User::LeaveIfError( length );
        
        iSource8.Set( iSource8.Right( length ) );
        
        AppendTextL( buf );
        }
    else
        {
        // just copy source to target
        iTarget16->Des().Copy(iSource8);
        iSource8.Set( KNullDesC8() );
        }
    }


TBool CMsgMailViewerCharConv::IsProcessDone() const
    {
    TBool ret( EFalse );

    if( iFromUnicode )
        {
        ret = iSource16.Length() == 0;
        }
    else
        {
        ret = iSource8.Length() == 0;
        }

    return ret;
    }

// End of File  
