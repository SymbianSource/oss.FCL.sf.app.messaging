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
*     Utility class for resolving DRM protection.
*
*/


// INCLUDE FILES
#include "MsgMailDRMHandler.h"

#include <e32base.h>
#include <DRMHelper.h>
#include <caf/data.h>
#include <MsgAttachmentInfo.h>
#include <MsgAttachmentModel.h>
#include <caf/caf.h>
#include "MailLog.h"

// LOCAL CONSTANTS AND MACROS


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor cannot contain any code that might leave
MsgMailDRMHandler::MsgMailDRMHandler()
    {
    }

// -----------------------------------------------------------------------------
// CMsgMailEditorAppUi::FileSize()
// -----------------------------------------------------------------------------
//
void MsgMailDRMHandler::ConstructL()
    {
    iDRMHelper = CDRMHelper::NewL();
    }

// Symbian OS default constructor can leave.
EXPORT_C MsgMailDRMHandler* MsgMailDRMHandler::NewL()
    {
    MsgMailDRMHandler* self = new (ELeave) MsgMailDRMHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();// self
    return self;
    }
    
// Destructor
EXPORT_C MsgMailDRMHandler::~MsgMailDRMHandler()
    {
    delete iDRMHelper;
    }

// -----------------------------------------------------------------------------
// MsgMailDRMHandler::IsForwardLockedFileL()
// -----------------------------------------------------------------------------
//  
EXPORT_C TBool MsgMailDRMHandler::IsForwardLockedFileL ( 
    RFile& aFileHandle )
    {
    
    TBool isForwardable;
    TBool valid;
    TBool drmProtected;
    GetFileInfoL ( aFileHandle, isForwardable, valid, drmProtected);
    
    
    if(drmProtected)
        {
        return !isForwardable;    
        }
    else
        {
        //No drm protection, so can't be forward locked.
        return EFalse;
        }
    
    }
    
// -----------------------------------------------------------------------------
// MsgMailDRMHandler::IsSeparateDeliveryFileL()
// -----------------------------------------------------------------------------
//  
EXPORT_C TBool MsgMailDRMHandler::IsSuperDistributableFileL ( 
    RFile& aFileHandle )
    {
    
    TBool isForwardable;
    TBool valid;
    TBool drmProtected;
    
    GetFileInfoL ( aFileHandle, isForwardable, valid, drmProtected);

    if(drmProtected)
        {
        return isForwardable;    
        }
    else
        {
        //No drm protection, so can't be super distributable.
        return EFalse;
        }
    } 

// -----------------------------------------------------------------------------
// MsgMailDRMHandler::SetDRMDataTypeL()
// -----------------------------------------------------------------------------
//  
EXPORT_C void MsgMailDRMHandler::SetDRMDataTypeL ( 
    CMsgAttachmentInfo& aAttachmentInfo, RFile& aFileHandle )
    {
    LOG(">MsgMailDRMHandler::SetDRMDataTypeL");
    TBool isForwardable;
    TBool valid;
    TBool drmProtected;

    GetFileInfoL ( aFileHandle, isForwardable, valid, drmProtected);
   
    if ( drmProtected && isForwardable )
        {
        LOG("MsgMailDRMHandler::SetDRMDataTypeL - drm protected");        
        CMsgAttachmentInfo::TDRMDataType drmType = valid ? 
            CMsgAttachmentInfo::ESeparateDeliveryValidRights : 
            CMsgAttachmentInfo::ESeparateDeliveryInvalidRights;
            
        aAttachmentInfo.SetDRMDataType( drmType );
        }
    else if( drmProtected && !isForwardable )
        {
        LOG("MsgMailDRMHandler::SetDRMDataTypeL - forward locked");
        aAttachmentInfo.SetDRMDataType( 
            CMsgAttachmentInfo::EForwardLockedOrCombinedDelivery );            
        }
    LOG("<MsgMailDRMHandler::SetDRMDataTypeL");
    }

// -----------------------------------------------------------------------------
// MsgMailDRMHandler::GetDRMDataTypeL()
// -----------------------------------------------------------------------------
// 
EXPORT_C CMsgAttachmentInfo::TDRMDataType MsgMailDRMHandler::GetDRMDataTypeL( 
    RFile aFileHandle )
    {
    
    TBool isForwardable;
    TBool valid;
    TBool drmProtected;
    
    GetFileInfoL ( aFileHandle, isForwardable, valid, drmProtected);
    
    CMsgAttachmentInfo::TDRMDataType drmType(
        CMsgAttachmentInfo::ENoLimitations );
    
    if ( drmProtected && isForwardable )
        {
        drmType = valid ? 
            CMsgAttachmentInfo::ESeparateDeliveryValidRights : 
            CMsgAttachmentInfo::ESeparateDeliveryInvalidRights;
        }
    else if(drmProtected && !isForwardable)
        {
        drmType = CMsgAttachmentInfo::EForwardLockedOrCombinedDelivery;
        }
    return drmType;     
    }
    
// -----------------------------------------------------------------------------
// MsgMailDRMHandler::GetFileInfo()
// -----------------------------------------------------------------------------
//  
void MsgMailDRMHandler::GetFileInfoL ( RFile& aFileHandle, 
    TBool& aIsForwardable, TBool& aValidRights, TBool& aProtection)
    {
    LOG(">MsgMailDRMHandler::GetFileInfoL");
    aProtection = EFalse;
    
    //These two are only valid if file is drm protected.
    aValidRights = EFalse;
    aIsForwardable = EFalse;
    
    LOG("MsgMailDRMHandler::GetFileInfoL - CContent");
    CContent* content = CContent::NewLC(aFileHandle);
    
    TInt isDRMProtected = 0;
    
    //Check if file is drm protected. If not no need for further checks.
    LOG("MsgMailDRMHandler::GetFileInfoL - CContent::GetAttribute protected");
    User::LeaveIfError(content->GetAttribute(EIsProtected, isDRMProtected));
    
    if (isDRMProtected)
        {
        aProtection = ETrue;
        
        //Check if forwardable or superdistributable.
        TInt isForwardable;
        LOG("MsgMailDRMHandler::GetFileInfoL - CContent::GetAttribute forwardable");
        User::LeaveIfError(content->GetAttribute(EIsForwardable, isForwardable));
        
        aIsForwardable = isForwardable;
        
        CData* data = NULL;
  
        // In the case of invalid rights, CData::NewLC will leave with KErrCANoRights,
        // KErrCANoPermission, or KErrCAPendingRights
        TRAPD(rightscheck, data = CData::NewL( aFileHandle, KDefaultContentObject, EUnknown ));
        
        if(rightscheck == KErrNone)
            {
            aValidRights = ETrue;
            delete data;
            }
        }
    CleanupStack::PopAndDestroy(content);
    LOG("<MsgMailDRMHandler::GetFileInfoL");
    }

//  End of File  
