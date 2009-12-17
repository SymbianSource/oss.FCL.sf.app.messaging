/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Defines implementation of CUniObjectsInfo class methods.
*
*/



// ========== INCLUDE FILES ================================

#include "UniObjectsInfo.h"

#include <MsgMediaInfo.h>
#include <MsgAttachmentInfo.h>

#include "uniobject.h"

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
// CUniObjectsInfo::NewLC
// ---------------------------------------------------------
//
CUniObjectsInfo* CUniObjectsInfo::NewLC( CMsgAttachmentModel& aAttachmentModel,
                                         CUniObject& aObject,
                                         TInt aDRMType,
                                         TBool aAttachmentObject,
                                         TInt aSlideNumber )
    {
    CUniObjectsInfo* self = new ( ELeave ) CUniObjectsInfo( aAttachmentModel, 
                                                            aObject,
                                                            aAttachmentObject,
                                                            aSlideNumber );
    
    CleanupStack::PushL( self );
    self->ConstructL( aDRMType );
    return self;
    }

// ---------------------------------------------------------
// CUniObjectsInfo::NewEmptySlideLC
// ---------------------------------------------------------
//
CUniObjectsInfo* CUniObjectsInfo::NewEmptySlideLC( CMsgAttachmentModel& aAttachmentModel,
                                                   const TDesC& aEmptySlideName )
    {
    CUniObjectsInfo* self = new ( ELeave ) CUniObjectsInfo( aAttachmentModel, 
                                                            aEmptySlideName );
    CleanupStack::PushL( self );
    self->ConstructL( CMsgAttachmentInfo::ENoLimitations );
    return self;
    }

// ---------------------------------------------------------
// CUniObjectsInfo::CUniObjectsInfo
// ---------------------------------------------------------
//
CUniObjectsInfo::CUniObjectsInfo( CMsgAttachmentModel& aAttachmentModel,
                                   CUniObject& aObject,
                                   TBool aAttachmentObject,
                                   TInt aSlideNumber ) :
    CMsgAttachmentInfo( aAttachmentModel,
                        aObject.MediaInfo()->FullFilePath(),
                        aObject.MediaInfo()->FileSize(),
                        ETrue,
                        ETrue ),
    iFlags( 0 ),
    iObject( &aObject ),
    iAttachmentObject( aAttachmentObject ),
    iSlideNumber( aSlideNumber )
    {
    this->SetSaved( aObject.IsSaved() );
    }

// ---------------------------------------------------------
// CUniObjectsInfo::CUniObjectsInfo
// ---------------------------------------------------------
//
CUniObjectsInfo::CUniObjectsInfo( CMsgAttachmentModel& aAttachmentModel,
                                 const TDesC& aEmptySlideName ) :
    CMsgAttachmentInfo( aAttachmentModel,
                        aEmptySlideName,
                        0,
                        ETrue,
                        ETrue ),
    iFlags( EMmsSlide ),
    iObject( NULL )
    {
    SetEmptyAttachment( ETrue );
    }

// ---------------------------------------------------------
// CUniObjectsInfo::ConstructL
// ---------------------------------------------------------
//
void CUniObjectsInfo::ConstructL( TInt aDRMType )
    {
    TDataType dataType;
    CMsgAttachmentInfo::TDRMDataType drmType( CMsgAttachmentInfo::ENoLimitations );
    TMsvAttachmentId attachmentId( 0 );
    
    if ( iObject )
        {
        dataType = iObject->MimeType();
        attachmentId = iObject->AttachmentId();
        drmType = static_cast<CMsgAttachmentInfo::TDRMDataType>( aDRMType );
        }
        
    CMsgAttachmentInfo::ConstructL( dataType, drmType );
    SetAttachmentId( attachmentId );
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CUniObjectsInfo::~CUniObjectsInfo()
    {
    }

// End of File
