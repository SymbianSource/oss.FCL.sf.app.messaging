/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class for unpacking a descriptor array received via IPC.
*
*/



#include "CNcnUnpackDescrArray.h"
#include <s32mem.h>

namespace {

// Local constants and definitions

const TInt KNcnPackedArrayGranularity = 4;

// Local utility functions:

void UnpackArrayL( const TDes8& aBuffer, CDesCArray& aArray )
    {
    RDesReadStream stream( aBuffer );
    CleanupClosePushL( stream );

    TInt status = KErrNone;
    do
        {
        TInt length( 0 );
        TRAP( status, length = stream.ReadInt32L() );
        if ( status == KErrNone && length > 0 )
            {
            HBufC* desBuf = HBufC::NewLC( length );
            TPtr desBufPtr = desBuf->Des();
            stream.ReadL( desBufPtr, length );
            aArray.AppendL( *desBuf );
            CleanupStack::PopAndDestroy( desBuf );
            }
        }
    while ( status == KErrNone );

    if ( status != KErrEof )
        {
        User::Leave( status );
        }
    CleanupStack::PopAndDestroy();  // stream
    }
    
} // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CNcnUnpackDescrArray::NewLC
//
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CNcnUnpackDescrArray* CNcnUnpackDescrArray::NewLC()
    {
	CNcnUnpackDescrArray* self = new( ELeave ) CNcnUnpackDescrArray;
	CleanupStack::PushL( self );
	return self;
    }

// ----------------------------------------------------------------------------
// CNcnUnpackDescrArray::UnpackL
//
// Unpacks descriptor array from IPC message.
// ----------------------------------------------------------------------------
//
void CNcnUnpackDescrArray::UnpackL( const RMessagePtr2& aMessage, TInt aParam )
    {
	const TInt packedLength = aMessage.GetDesLength( aParam );
	HBufC8* buffer = HBufC8::NewLC( packedLength );
	TPtr8 bufferPtr = buffer->Des();	
    aMessage.ReadL( aParam, bufferPtr );
	UnpackArrayL( bufferPtr, iArray );
	CleanupStack::PopAndDestroy( buffer );
    }

// ----------------------------------------------------------------------------
// CNcnUnpackDescrArray::~CNcnUnpackDescrArray
//
// Destructor.
// ----------------------------------------------------------------------------
//
CNcnUnpackDescrArray::~CNcnUnpackDescrArray()
    {
    }

// ----------------------------------------------------------------------------
// CNcnUnpackDescrArray::MdcaCount
//
// From MDesCArray.
// ----------------------------------------------------------------------------
//
TInt CNcnUnpackDescrArray::MdcaCount() const
    {
    return iArray.MdcaCount();
    }

// ----------------------------------------------------------------------------
// CNcnUnpackDescrArray::MdcaPoint
//
// From MDesCArray.
// ----------------------------------------------------------------------------
//
TPtrC16 CNcnUnpackDescrArray::MdcaPoint( TInt aIndex ) const
    {
    return iArray.MdcaPoint( aIndex );
    }

// ----------------------------------------------------------------------------
// CNcnUnpackDescrArray::CNcnUnpackDescrArray
//
// Constructor.
// ----------------------------------------------------------------------------
//
CNcnUnpackDescrArray::CNcnUnpackDescrArray()
    : iArray( KNcnPackedArrayGranularity )
    {
    }
    
// End of file
