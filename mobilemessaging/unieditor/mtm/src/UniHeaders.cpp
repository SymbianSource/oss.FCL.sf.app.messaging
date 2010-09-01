/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: UniHeaders implementation
*
*/



// INCLUDE FILES
#include <msvstore.h>
#include <badesca.h>
#include <msvids.h>
#include <mtmdef.h>
#include <e32def.h>
#include <e32math.h>
#include <mmsvattachmentmanager.h>
#include <cmsvmimeheaders.h>
#include <mmsgenutils.h>
#include "UniHeaders.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KArrayGranularity = 4;
const TUid KUidUniHeaderStream={0x2000B00A}; 

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CUniHeaders::CUniHeaders()
// in a class derived from CBase all member variables are set initially to 0
    {
    }

// Symbian OS default constructor can leave.
void CUniHeaders::ConstructL()
    {
    iToArray = new ( ELeave )CDesCArrayFlat( KArrayGranularity );
    iCcArray = new ( ELeave )CDesCArrayFlat( KArrayGranularity );
    iBccArray = new ( ELeave )CDesCArrayFlat( KArrayGranularity );
    }

// Two-phased constructor.
CUniHeaders* CUniHeaders::NewL()
    {
    CUniHeaders* self = new ( ELeave ) CUniHeaders;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
CUniHeaders::~CUniHeaders()
    {
    delete iToArray;
    delete iCcArray;
    delete iBccArray;
    delete iSubject;
    }

// ---------------------------------------------------------
// CUniHeaders::RestoreL
//
// ---------------------------------------------------------
//

void CUniHeaders::RestoreL(
    CMsvStore& aStore )
    {

    RMsvReadStream stream;
    Reset(); // all old pointers are deleted here
    if ( aStore.IsPresentL( KUidUniHeaderStream ) )
        {
        stream.OpenLC( aStore, KUidUniHeaderStream ); // pushes 'stream' to the stack
        InternalizeL( stream );
        CleanupStack::PopAndDestroy( &stream );     
        }
    } 

// ---------------------------------------------------------
// CUniHeaders::StoreL
//
// ---------------------------------------------------------
//

void CUniHeaders::StoreL(
    CMsvStore& aStore )
    {    
    // caller must commit the store
    RMsvWriteStream stream;
    stream.AssignLC( aStore, KUidUniHeaderStream ); // pushes 'stream' to the stack
    ExternalizeL( stream );
    stream.CommitL();
    stream.Close();
    CleanupStack::PopAndDestroy( &stream );
    }

// ---------------------------------------------------------
// CUniHeaders::TypedAddresseeList
//
// ---------------------------------------------------------
//
const CDesCArray& CUniHeaders::TypedAddresseeList( 
    TMsvRecipientType aType ) 
    {
    if ( aType == EMsvRecipientTo )
        {
        return *iToArray;
        }
    else if ( aType == EMsvRecipientCc )
        {
        return *iCcArray;
        }
    else 
        {
        return *iBccArray;
        }
    }
   
// ---------------------------------------------------------
// CUniHeaders::AddTypedAddresseeL
//
// ---------------------------------------------------------
//
void CUniHeaders::AddTypedAddresseeL( 
    const TDesC&  aRealAddress, 
    TMsvRecipientType aType  )
    {
    if ( aType == EMsvRecipientTo )
        {
        iToArray->AppendL( aRealAddress );
        }
    if ( aType == EMsvRecipientCc )
        {
        iCcArray->AppendL( aRealAddress );
        }
    if ( aType == EMsvRecipientBcc ) 
        {
        iBccArray->AppendL( aRealAddress );
        }
    }

// ---------------------------------------------------------
// CUniHeaders::RemoveAddresseeL
//
// ---------------------------------------------------------
//
TBool CUniHeaders::RemoveAddressee( const TDesC& aRealAddress )
    {

    if ( RemoveAddressee( *iToArray, aRealAddress) )
        {
        return ETrue;
        }
    if ( RemoveAddressee( *iCcArray, aRealAddress) )
        {
        return ETrue;
        }
    if ( RemoveAddressee( *iBccArray, aRealAddress) )
        {
        return ETrue;
        }
      
    return EFalse;

    }

// ---------------------------------------------------------
// CUniHeaders::SetSubjectL
//
// ---------------------------------------------------------
//
void CUniHeaders::SetSubjectL( const TDesC& aSubject )
    {
    HBufC* newAttrib = aSubject.AllocL();
    delete iSubject;
    iSubject = newAttrib;
    }

// ---------------------------------------------------------
// CUniHeaders::Subject
//
// ---------------------------------------------------------
//
TPtrC CUniHeaders::Subject() const
    {
    return iSubject ? TPtrC( *iSubject ) : TPtrC(); 
    }

// ---------------------------------------------------------
// CUniHeaders::InternalizeL
//
// ---------------------------------------------------------
//
void CUniHeaders::InternalizeL( RMsvReadStream& aStream )
    {

    iSubject = HBufC::NewL( aStream, KMaxHeaderStringLength );
    iMessageTypeSetting = aStream.ReadInt32L();
    iMessageTypeLocking = aStream.ReadInt32L();
    iMessageRoot = aStream.ReadInt32L();
    
    // Internalize fields which may have multiple values.
    InternalizeArrayL( *iToArray, aStream ); //lint !e64 !e1514
    InternalizeArrayL( *iCcArray, aStream ); //lint !e64 !e1514
    InternalizeArrayL( *iBccArray, aStream ); //lint !e64 !e1514
    }

// ---------------------------------------------------------
// CUniHeaders::ExternalizeL
//
// ---------------------------------------------------------
//
void CUniHeaders::ExternalizeL( RMsvWriteStream& aStream ) const
    {

    aStream << Subject(); //lint !e1023 !e1703 not ambiguous
    aStream.WriteInt32L( iMessageTypeSetting );
    aStream.WriteInt32L( iMessageTypeLocking );
    aStream.WriteInt32L( iMessageRoot );

    // Externalize fields which may have multiple values.
    ExternalizeArrayL( *iToArray, aStream ); //lint !e64 !e1514
    ExternalizeArrayL( *iCcArray, aStream ); //lint !e64 !e1514
    ExternalizeArrayL( *iBccArray, aStream ); //lint !e64 !e1514
    }

// ---------------------------------------------------------
// CUniHeaders::RemoveAddressee
//
// ---------------------------------------------------------
//
TBool CUniHeaders::RemoveAddressee(
    CDesCArray& aList,
    const TDesC& aAddress )
    {
    
    TInt pos = 0; 

    // Find the match index from a given addressee list
    aList.Find( aAddress, pos );
    if (pos < aList.Count())
        {
        // remove entry from the heap
        aList.Delete( pos );
        aList.Compress();
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CUniHeaders::Reset
//
// ---------------------------------------------------------
//
void CUniHeaders::Reset()
    {
    
    iToArray->Reset();
    iCcArray->Reset(); 
    iBccArray->Reset(); 

    delete iSubject;   
    iSubject = NULL;
    }

// ---------------------------------------------------------
// CUniHeaders::Size
//
// ---------------------------------------------------------
//
TInt CUniHeaders::Size() const
    {

    TInt size = 0;

    TInt i = 0;

    TInt nbr = iToArray->MdcaCount();
    for ( i=0; i < nbr; ++i )
        {
        size += iToArray->MdcaPoint(i).Size();
        }

    nbr = iCcArray->MdcaCount();
    for ( i=0; i < nbr; ++i )
        {
        size+=iCcArray->MdcaPoint(i).Size();
        }

    nbr = iBccArray->MdcaCount();
    for ( i=0; i < nbr; ++i )
        {
        size+=iBccArray->MdcaPoint(i).Size();
        }

    size+= Subject().Size();
    size+= 2 * sizeof( TInt32 ); // iMessageTypeSetting, iMessageTypeLocking
    
    return size;
    }

// ---------------------------------------------------------
// CUniHeaders::ExternalizeArrayL
//
// ---------------------------------------------------------
//
void CUniHeaders::ExternalizeArrayL(
    CDesC16Array& anArray,
    RWriteStream& aStream ) const
    {

    TInt count=anArray.Count();
    aStream << TCardinality( count );       // compressed value
    for ( TInt ii = 0; ii < count; ++ii )
        {
        aStream << anArray[ii].Left( Min( anArray[ii].Length(), KMaxHeaderStringLength ) ) ;
        }
    }

// ---------------------------------------------------------
// CUniHeaders::InternalizeArrayL
//
// ---------------------------------------------------------
//
void CUniHeaders::InternalizeArrayL(
    CDesC16Array& anArray,
    RReadStream& aStream )
    {
    TCardinality card;
    aStream >> card;
    TInt count=card;
    anArray.Reset();
    for ( TInt ii = 0; ii < count; ++ii )
        {
        HBufC16* buf=HBufC16::NewLC( aStream,KMaxHeaderStringLength );
        anArray.CArrayFixBase::InsertL( ii,&buf );
        CleanupStack::Pop( buf );
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File  

