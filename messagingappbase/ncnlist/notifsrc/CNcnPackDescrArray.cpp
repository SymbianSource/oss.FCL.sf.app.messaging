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
* Description:   Class for packaging a descriptor array for IPC.
*
*/



#include "CNcnPackDescrArray.h"
#include <s32mem.h>

namespace {

// Local utility functions:

TInt PackedLength( const MDesCArray& aArray )
    {
    const TInt count = aArray.MdcaCount();
    TInt result = count * sizeof( TInt32 );
    for ( TInt i = 0; i < count; ++i )
        {
        result += aArray.MdcaPoint( i ).Size();
        }
    return result;
    }

void PackArrayL( TDes8& aBuffer, const MDesCArray& aArray )
    {
    RDesWriteStream stream( aBuffer );
    CleanupClosePushL( stream );
    const TInt count = aArray.MdcaCount();
    for ( TInt i = 0; i < count; ++i )
        {
        TPtrC des( aArray.MdcaPoint( i ) );
        stream.WriteInt32L( des.Length() );
        stream.WriteL( des );
        }
    CleanupStack::PopAndDestroy();  // stream
    }

} // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CNcnPackDescrArray::NewLC
//
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CNcnPackDescrArray* CNcnPackDescrArray::NewL()
	{
	CNcnPackDescrArray* self = new( ELeave ) CNcnPackDescrArray;	
	return self;
	}
	
// ----------------------------------------------------------------------------
// CNcnPackDescrArray::NewLC
//
// Symbian two-phased constructor.
// ----------------------------------------------------------------------------
//
CNcnPackDescrArray* CNcnPackDescrArray::NewLC()
	{
	CNcnPackDescrArray* self = new( ELeave ) CNcnPackDescrArray;
	CleanupStack::PushL( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CNcnPackDescrArray::PackL
//
// Packages descriptor array to a buffer.
// ----------------------------------------------------------------------------
//
TDesC8* CNcnPackDescrArray::PackL( const MDesCArray& aSourceArray )
	{
	const TInt packedLength = PackedLength( aSourceArray );
	HBufC8* buffer = HBufC8::NewLC( packedLength );
	TPtr8 bufferPtr = buffer->Des();
	PackArrayL( bufferPtr, aSourceArray );	
	CleanupStack::Pop( buffer );
	return buffer;
	}

// ----------------------------------------------------------------------------
// CNcnPackDescrArray::~CNcnPackDescrArray
//
// Destructor.
// ----------------------------------------------------------------------------
//
CNcnPackDescrArray::~CNcnPackDescrArray()
    {
    }

// ----------------------------------------------------------------------------
// CNcnPackDescrArray::CNcnPackDescrArray
//
// Constructor.
// ----------------------------------------------------------------------------
//
CNcnPackDescrArray::CNcnPackDescrArray()
    {
    }

// End of file
