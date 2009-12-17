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
*       Defines implementation of CUniObjectsModel class methods.
*
*/



// ========== INCLUDE FILES ================================
#include "UniObjectsModel.h"

#include <eikenv.h>
#include <gulicon.h>
#include <apgicnfl.h>           // apamasked bitmaps
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <data_caging_path_literals.hrh> 

#include <uniutils.mbg>

#include "UniUtils.h"
#include "UniObjectsInfo.h"
#include "UniObjectsModelObserver.h"

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
// CUniObjectsModel::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CMsgAttachmentModel* CUniObjectsModel::NewL( TBool aReadOnly )
    {
    CUniObjectsModel* self = new( ELeave ) CUniObjectsModel( aReadOnly );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CUniObjectsModel::~CUniObjectsModel()
    {
    delete iMmsSlideIcon;
    }

// ---------------------------------------------------------
// CUniObjectsModel::AddObjectL
//
// Adds an object into the internal array.
// ---------------------------------------------------------
//
CUniObjectsInfo* CUniObjectsModel::AddObjectL(
    CUniObject& aObject,
    TInt aDRMType,
    TBool aAttachmentObject,
    TInt aSlideNumber,
    TInt aIndex /*= KErrNotFound*/ )
    {
    CUniObjectsInfo* info = CUniObjectsInfo::NewLC( *this, 
                                                    aObject, 
                                                    aDRMType, 
                                                    aAttachmentObject, 
                                                    aSlideNumber );

    if ( aIndex == KErrNotFound )
        {
        iAttachInfoArray->AppendL( info );
        }
    else
        {
        iAttachInfoArray->InsertL( aIndex, info );
        }
        
    iModified = ETrue;

    CleanupStack::Pop( info );
    return info;
    }

// ---------------------------------------------------------
// CUniObjectsModel::AddEmptySlideL
//
// Adds an empty slide object into the internal array.
// ---------------------------------------------------------
//
CUniObjectsInfo* CUniObjectsModel::AddEmptySlideL(
    const TDesC& aEmptySlideName,
    TInt aIndex /*= KErrNotFound*/ )
    {
    CUniObjectsInfo* info = CUniObjectsInfo::NewEmptySlideLC(
        *this,
        aEmptySlideName );

    if ( aIndex == KErrNotFound )
        {
        iAttachInfoArray->AppendL( info );
        }
    else
        {
        iAttachInfoArray->InsertL( aIndex, info );
        }
        
    iModified = ETrue;

    CleanupStack::Pop( info );
    return info;
    }

// ---------------------------------------------------------
// CUniObjectsModel::DeleteObjectL
//
// Deletes attachment from internal array. Notifies attachment model
// observer with EMsgAttachmentRemoved parameter.
// ---------------------------------------------------------
//
void CUniObjectsModel::DeleteObjectL( TInt aIndex, 
                                      TBool aNotifyObserver )
    {
    if ( aIndex >= 0 && aIndex < iAttachInfoArray->Count() )
        {
        CMsgAttachmentInfo* info = iAttachInfoArray->At( aIndex );

        if ( iAttachmentModelObserver != NULL && aNotifyObserver )
            {
            MUniObjectsModelObserver* obs = 
                static_cast<MUniObjectsModelObserver*>( iAttachmentModelObserver );
                
            obs->NotifyChangesL( MMsgAttachmentModelObserver::EMsgAttachmentRemoved, 
                                 info );
            }

        // delete the information block.
        delete info;

        // delete the array entry.
        iAttachInfoArray->Delete( aIndex );

        iModified = ETrue;
        }
    }

// ---------------------------------------------------------
// CUniObjectsModel::CUniObjectsModel
//
// Constructor.
// ---------------------------------------------------------
//
CUniObjectsModel::CUniObjectsModel( TBool aReadOnly ) :
    CMsgAttachmentModel( aReadOnly )
    {
    }

// ---------------------------------------------------------
// CUniObjectsModel::CreateNewInfoL
// ---------------------------------------------------------
//
CMsgAttachmentInfo* CUniObjectsModel::CreateNewInfoL(
    const TDesC&    /*aFileName*/,
    TInt            /*aSize*/,
    TBool           /*aFetched*/,
    TBool           /*aAttachedThisSession*/,
    TMsvAttachmentId /*aAttachmentId*/,
    const TDataType& /*aDataType*/,
    CMsgAttachmentInfo::TDRMDataType /*aDRMDataType*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CUniObjectsModel::ChangeOrderL
// ---------------------------------------------------------
//
void CUniObjectsModel::ChangeOrderL( TInt aIndex, TInt aNewPosition )
    {
    if ( aIndex >= 0 && aIndex < iAttachInfoArray->Count() )
        {
        CMsgAttachmentInfo* info = iAttachInfoArray->At( aIndex );

        // delete object from array and insert it to new position.
        iAttachInfoArray->Delete( aIndex );
        iAttachInfoArray->InsertL( aNewPosition, info );

        iModified = ETrue;
        }
    }

// ---------------------------------------------------------
// CUniObjectsModel::NotifyChangeOrderL
// ---------------------------------------------------------
//
void CUniObjectsModel::NotifyChangeOrderL ( TUniLayout aLayout )
    {
    if ( iAttachmentModelObserver != NULL )
        {
        MUniObjectsModelObserver* obs = 
            static_cast<MUniObjectsModelObserver*>( iAttachmentModelObserver );
        obs->NotifyOrderChangeL( aLayout );
        }
    }

// ---------------------------------------------------------
// CUniObjectsModel::BitmapForMmsSlideL
//
// Loads system default bitmap.
// ---------------------------------------------------------
//
CGulIcon* CUniObjectsModel::BitmapForMmsSlideL()
    {
    if ( !iMmsSlideIcon )
        {
        TParse fileParse;
        fileParse.Set( KUniUtilsMBMFileName, &KDC_APP_BITMAP_DIR, NULL );
        iMmsSlideIcon = AknsUtils::CreateGulIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDQgnPropMmsEmptyPageSub,
            fileParse.FullName(),
            EMbmUniutilsQgn_prop_mms_empty_page_sub,
            EMbmUniutilsQgn_prop_mms_empty_page_sub_mask );
        }
    return iMmsSlideIcon;
    }

// ========== OTHER EXPORTED FUNCTIONS =====================

//  End of File
