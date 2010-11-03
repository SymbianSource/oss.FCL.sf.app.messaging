/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Encapsulates the message data for MeetingRequestSender service.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include <s32mem.h>

#include <MeetingRequestData.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMeetingRequestData::NewL
// Two-phased constructor. aDataType should be KSenduiMsgDataMeetingRequest
// when message is sent using MeetingRequestSender. 
// -----------------------------------------------------------------------------
//
EXPORT_C CMeetingRequestData* CMeetingRequestData::NewL()
    {
    CMeetingRequestData* self = CMeetingRequestData::NewLC();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CMeetingRequestData::NewLC
// Two-phased constructor. aDataType should be KSenduiMsgDataMeetingRequest
// when message is sent using MeetingRequestSender. 
// -----------------------------------------------------------------------------
//
EXPORT_C CMeetingRequestData* CMeetingRequestData::NewLC( )
    {
    CMeetingRequestData* self = new( ELeave ) CMeetingRequestData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CMeetingRequestData::~CMeetingRequestData
// Destructor.
// -----------------------------------------------------------------------------
//
CMeetingRequestData::~CMeetingRequestData()
    {
    }

// -----------------------------------------------------------------------------
// CMeetingRequestData::CMeetingRequestData
// Constructor. 
// -----------------------------------------------------------------------------
//
CMeetingRequestData::CMeetingRequestData() 
    {
    }

// -----------------------------------------------------------------------------
// CMeetingRequestData::CMeetingRequestData
// 2nd phase constructor. 
// -----------------------------------------------------------------------------
//
void CMeetingRequestData::ConstructL() 
    {
    // Nothing at the moment.
    }

// -----------------------------------------------------------------------------
// CMeetingRequestData::UseMailbox
// Define which mailbox should be used to send an email. aMailboxId is the 
// same as mail client service id. If not set, default mailbox is used. 
// -----------------------------------------------------------------------------
//   
EXPORT_C void CMeetingRequestData::UseMailbox( const TMsvId aMailboxId )
    {
    iMailboxId = aMailboxId;
    }

// -----------------------------------------------------------------------------
// CMeetingRequestData::Mailbox
// Get mailbox id. The same as mail client service id. 
// -----------------------------------------------------------------------------
//   
EXPORT_C TMsvId CMeetingRequestData::Mailbox( ) const
    {
    return iMailboxId;
    }

// -----------------------------------------------------------------------------
// CMeetingRequestData::SetDatabaseFileId
// Set id of calendar database file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
EXPORT_C void CMeetingRequestData::SetDatabaseFileId( const TUint64 aFileId )
    {
    iDatabaseFileId = aFileId;
    }

// -----------------------------------------------------------------------------
// CMeetingRequestData::DatabaseFileId
// Returns id of calendar database file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
EXPORT_C TUint64 CMeetingRequestData::DatabaseFileId( ) const
    {
    return iDatabaseFileId;
    }

// -----------------------------------------------------------------------------
// CMeetingRequestData::AppendEntryLocalId
// Append local id of calendar entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
EXPORT_C void CMeetingRequestData::AppendEntryLocalId(
    const TUint32 aEntryLocalId )
    {
    iEntryLocalIdArray.Append( aEntryLocalId );
    }

// -----------------------------------------------------------------------------
// CMeetingRequestData::DatabaseFileId
// Returns array of local ids of calendar entries.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
EXPORT_C const RArray<TUint32>& CMeetingRequestData::EntryLocalIdArray( ) const
    {
    return iEntryLocalIdArray;
    }

// -----------------------------------------------------------------------------
// CMeetingRequestData::SetMailHeaderMethodL
// Set method type for mail header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
EXPORT_C void CMeetingRequestData::SetMailHeaderMethodL(
    const TDesC8& aMethodType )
    {
    iMethodType.Append( aMethodType );
    }

// -----------------------------------------------------------------------------
// CMeetingRequestData::MailHeaderMethod
// Returns method type for mail header.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
EXPORT_C TPtrC8 CMeetingRequestData::MailHeaderMethod( ) const
    {
    return iMethodType.Ptr();
    }


// -----------------------------------------------------------------------------
// CMeetingRequestData::WriteToBufferLC
// -----------------------------------------------------------------------------
//   
EXPORT_C HBufC8* CMeetingRequestData::WriteToBufferLC() const
    {
    // Lenght for the buffer:
    //
    // 32-bit       iMailboxId
    // 64-bit       iDatabaseFileId
    // 50 x 8bit    iMehtodType
    // 16-bit       iEntryLocalIdArray.Count()
    // N x 32-bit   N x iEntryLocalIdArray[i]
    //
    TInt bufLength = 0;
    bufLength += sizeof( iMailboxId );
    bufLength += sizeof( iDatabaseFileId );
    bufLength += KMethodTypeMaxLength;
    bufLength += sizeof( TInt );
    bufLength += iEntryLocalIdArray.Count() * sizeof( TUint32 );

    HBufC8* buf = HBufC8::NewLC( bufLength );
    TDes8 des = buf->Des();
    RDesWriteStream stream( des );
    
    stream << iMailboxId;
    stream.WriteReal64L( iDatabaseFileId );
    
    stream << iMethodType;
    
    TInt count = iEntryLocalIdArray.Count();
    stream.WriteInt16L(count);
        
    for ( TInt i = 0; i < count; ++i )
        {
        stream.WriteUint32L(iEntryLocalIdArray[i]);
        }
    stream.Close();
    return buf;
    }
    
// -----------------------------------------------------------------------------
// CMeetingRequestData::ReadFromBufferL
// -----------------------------------------------------------------------------
//   
EXPORT_C void CMeetingRequestData::ReadFromBufferL( const TDesC8& aData )
    {
    RDesReadStream stream( aData );
    
    stream >> iMailboxId;
    iDatabaseFileId = stream.ReadReal64L( );
    
    stream >> iMethodType;
    
    
    TInt count = stream.ReadInt16L();
    
    for ( TInt i = 0; i < count; ++i )
        {
        iEntryLocalIdArray[i] = stream.ReadUint32L();
        }
    stream.Close();
    }

//  END OF FILE
