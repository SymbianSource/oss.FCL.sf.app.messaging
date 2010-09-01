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
* Description:  MsgAttachmentInfo implementation
*
*/



// ========== INCLUDE FILES ================================

#include <eikenv.h>
#include <gulicon.h>

#include "MsgAttachmentInfo.h"
#include "MsgAttachmentModel.h"
#include "MsgAttachmentUtils.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgAttachmentInfo::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CMsgAttachmentInfo* CMsgAttachmentInfo::NewL(
    CMsgAttachmentModel& aAttachmentModel,
    const TFileName& aFileName,
    TInt aSize,
    TBool aFetched,
    TBool aAttachedThisSession,
    TMsvAttachmentId aAttachmentId,
    const TDataType& aDataType,
    TDRMDataType aDRMDataType )
    {
    CMsgAttachmentInfo* self = new ( ELeave ) CMsgAttachmentInfo(
        aAttachmentModel,
        aFileName,
        aSize,
        aFetched,
        aAttachedThisSession );

    CleanupStack::PushL( self );
    
    // Must be set before ConstructL is called as might be used on
    // during CMsgAttachmentModel::BitmapForFileL function call.
    self->SetAttachmentId( aAttachmentId ); 
    self->ConstructL( aDataType, aDRMDataType );
    CleanupStack::Pop(); // self

    return self;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::~CMsgAttachmentInfo
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgAttachmentInfo::~CMsgAttachmentInfo()
    {
    delete iIcon;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::FileName
//
// Returns reference to the filename.
// ---------------------------------------------------------
//
EXPORT_C const TFileName& CMsgAttachmentInfo::FileName() const
    {
    return iFileName;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::SetFileName
//
// Set the filename of this attachment.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentInfo::SetFileName( const TFileName& aFileName )
    {
    iFileName = aFileName;

    // find file size
    RFile file;
    RFs& fs = CEikonEnv::Static()->FsSession();

    TInt err = file.Open( fs, iFileName, EFileShareAny );
    if ( err == KErrAccessDenied || err == KErrInUse )
        {
        err = file.Open( fs, iFileName, EFileShareReadersOnly );
        }
    if ( err == KErrNone )
        {
        file.Size( iSize );
        }
    file.Close();

    TRAP_IGNORE( ChangeSizeAndIconL() );
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::Size
//
// Returns the size.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgAttachmentInfo::Size() const
    {
    return iSize;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::SizeString
//
// Returns the size string.
// ---------------------------------------------------------
//
EXPORT_C TBuf<KMsgAttaSizeStringLength> CMsgAttachmentInfo::SizeString() const
    {
    return iSizeString;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::AttachmentId
//
// Returns the attachment id.
// ---------------------------------------------------------
//
EXPORT_C TMsvAttachmentId CMsgAttachmentInfo::AttachmentId() const
    {
    return iAttachmentId;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::SetAttachmentId
//
// Sets the attachment id.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentInfo::SetAttachmentId( TMsvAttachmentId aId )
    {
    iAttachmentId = aId;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::IsFetched
//
// Returns ETrue if this attachment is fetched.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgAttachmentInfo::IsFetched() const
    {
    return iAttachmentFlags & EAttachmentFetched;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::SetFetched
//
// Sets the fetched attribute of this attachment.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentInfo::SetFetched( TBool aFetched )
    {
    TBool updateIcon( EFalse );
    
    if ( aFetched && !IsFetched() )
        {
        iAttachmentFlags |= EAttachmentFetched;
        updateIcon = ETrue;
        }
    else if ( !aFetched && IsFetched() )
        {
        iAttachmentFlags &= ~EAttachmentFetched;
        updateIcon = ETrue;
        }
        
    if ( iIcon && updateIcon )
        {
        // No need to update icon if it's not yet initialized.
        TRAP_IGNORE( DoUpdateIconL() );
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::DataType
//
// Returns the datatype (MIME type) of this attachment.
// ---------------------------------------------------------
//
EXPORT_C const TDataType& CMsgAttachmentInfo::DataType() const
    {
    return iDataType;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::SetDataType
//
// Sets the datatype (MIME type) of this attachment.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentInfo::SetDataType( const TDataType& aDataType )
    {
    if ( iDataType.Des8().CompareF( aDataType.Des8() ) != 0 )
        {
        iDataType = aDataType;
        
        TRAP_IGNORE( DoUpdateIconL() );
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::SetDRMDataType(TDRMDataType aDRMDataType)
//
// Sets the DRMtype of this attachment.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentInfo::SetDRMDataType( TDRMDataType aDRMDataType )
    {
    if ( iDRMDataType != aDRMDataType )
        {
        iDRMDataType = aDRMDataType;
        
        TRAP_IGNORE( DoUpdateIconL() );
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::DRMDataType
//
// Returns the DRMtype of this attachment.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgAttachmentInfo::DRMDataType() const
    {
    return iDRMDataType;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::IsAttachedThisSession
//
// Returns ETrue if this attachment was attached in this session.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgAttachmentInfo::IsAttachedThisSession() const
    {
    return iAttachmentFlags & EAttachedThisSession;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::SetAttachedThisSession
//
// Sets flag which tells if this attachment is attached in this session.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentInfo::SetAttachedThisSession( TBool aFlag )
    {
    if ( aFlag )
        {
        iAttachmentFlags |= EAttachedThisSession;
        }
    else
        {
        iAttachmentFlags &= ~EAttachedThisSession;
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::IsSupported
//
// Returns supported flag.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgAttachmentInfo::IsSupported() const
    {
    return iAttachmentFlags & EAttachmentSupported;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::SetSupported
//
// Sets supported flag.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentInfo::SetSupported( TBool aFlag )
    {
    if ( aFlag )
        {
        iAttachmentFlags |= EAttachmentSupported;
        }
    else
        {
        iAttachmentFlags &= ~EAttachmentSupported;
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::Icon
//
// Returns the icon.
// ---------------------------------------------------------
//
EXPORT_C CGulIcon* CMsgAttachmentInfo::Icon() const
    {
    return iIcon;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::SetSeparator
//
// Returns separator flag.
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentInfo::SetSeparator( TBool aFlag )
    {
    if ( aFlag )
        {
        iAttachmentFlags |= ESeparatorBefore;
        }
    else
        {
        iAttachmentFlags &= ~ESeparatorBefore;
        }
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::IsSeparator
//
// Sets separator flag.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgAttachmentInfo::IsSeparator() const
    {
    return iAttachmentFlags & ESeparatorBefore;;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::CMsgAttachmentInfo
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgAttachmentInfo::CMsgAttachmentInfo( CMsgAttachmentModel& aAttachmentModel,
                                                 const TFileName& aFileName,
                                                 TInt aSize,
                                                 TBool aFetched,
                                                 TBool aAttachedThisSession ) :
    iAttachmentModel( aAttachmentModel ),
    iFileName( aFileName ),
    iSize( aSize ),
    iCharset( KErrNotFound )
    {
    SetFetched( aFetched );
    SetAttachedThisSession ( aAttachedThisSession );
    SetSupported( ETrue );
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::ConstructL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentInfo::ConstructL( const TDataType& aDataType,
                                              TDRMDataType aDRMDataType )
    {
    iDataType = aDataType;
    iDRMDataType = aDRMDataType;
    ChangeSizeAndIconL();
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::ChangeSizeAndIconL
//
//
// ---------------------------------------------------------
//
void CMsgAttachmentInfo::ChangeSizeAndIconL()
    {
    MsgAttachmentUtils::FileSizeToStringL( iSizeString, iSize );

    DoUpdateIconL();
    }


// ---------------------------------------------------------
// CMsgAttachmentInfo::SetSaved
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttachmentInfo::SetSaved( TBool aSaved )
    {
    if ( aSaved )
        {
        iAttachmentFlags |= EAttachmentSaved;
        }
    else
        {
        iAttachmentFlags &= ~EAttachmentSaved;
        }
    }
    
// ---------------------------------------------------------
// CMsgAttachmentInfo::IsSaved
//
//
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgAttachmentInfo::IsSaved( ) const
    {
    return iAttachmentFlags & EAttachmentSaved;
    }

// ---------------------------------------------------------
// CMsgAttachmentInfo::DoUpdateIconL
//
//
// ---------------------------------------------------------
//
void CMsgAttachmentInfo::DoUpdateIconL()
    {
    CGulIcon* resultIcon = iAttachmentModel.BitmapForFileL( *this );
        
    // icons are owned by atta info object.
    delete iIcon;
    iIcon = resultIcon;
    }

// ========== OTHER EXPORTED FUNCTIONS =====================

//  End of File
