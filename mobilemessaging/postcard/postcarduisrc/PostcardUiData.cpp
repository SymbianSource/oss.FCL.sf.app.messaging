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
* Description:  
*       Provides Postcard UI data methods.
*
*/



// INCLUDE FILES
#include "PostcardUiData.h"

#include <mtclbase.h>
#include <eikon.rsg>
#include <data_caging_path_literals.hrh> 
#include <bautils.h>						// FileExists

#include <coemain.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <fbs.h>
#include <featmgr.h>

#include <mtmuids.h>
#include <msvids.h>
#include <msvuids.h>
#include <mtmuidef.hrh>

#include <PostcardUi.rsg>
#include <avkon.rsg>

// MACROS
// LOCAL CONSTANTS AND MACROS
_LIT( KPostcardUiResourceFile,"PostcardUi.rsc");
_LIT( KPostcardDriveZ,"z:"); 
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// NewPostcardUiDataL
// Factory function
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUiData* NewPostcardUiDataL( CRegisteredMtmDll& aRegisteredDll )
    {
    // Get supported features from feature manager.
    FeatureManager::InitializeLibL();
    if ( !FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) )
        {
        FeatureManager::UnInitializeLib();
        User::Leave( KErrNotSupported );
        }
    FeatureManager::UnInitializeLib();
    
    return CPostcardUiData::NewL( aRegisteredDll );
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPostcardUiData::CPostcardUiData
// C++ constructor
// ---------------------------------------------------------
//
CPostcardUiData::CPostcardUiData( CRegisteredMtmDll& aRegisteredDll )
    :   CBaseMtmUiData( aRegisteredDll )
    {
    }

// ---------------------------------------------------------
// CPostcardUiData::ConstructL
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CPostcardUiData::ConstructL()
    {
    CBaseMtmUiData::ConstructL();
    }


// ---------------------------------------------------------
// CPostcardUiData::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CPostcardUiData* CPostcardUiData::NewL( CRegisteredMtmDll& aRegisteredDll )
    {
    CPostcardUiData* self = new ( ELeave ) CPostcardUiData( aRegisteredDll );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// ---------------------------------------------------------
// CPostcardUiData::CPostcardUiData
// Destructor.
// ---------------------------------------------------------
//
CPostcardUiData::~CPostcardUiData()
    {
    }


// ---------------------------------------------------------
// CPostcardUiData::OperationSupportedL
// ---------------------------------------------------------
//
TInt CPostcardUiData::OperationSupportedL(
    TInt aFunctionId,
    const TMsvEntry& aContext ) const
    {
    // Only Message Info for messages is a supported operation
    if ( ( aFunctionId == KMtmUiFunctionMessageInfo ) &&
        ( aContext.iType == KUidMsvMessageEntry ) )
        {
        return 0;
        }

    return R_EIK_TBUF_NOT_AVAILABLE;
    }


// ---------------------------------------------------------
// CPostcardUiData::QueryCapability
// The default UI capability checker (UI MTM may have also minimal
// UI specific checks also)
// ---------------------------------------------------------
//
TInt CPostcardUiData::QueryCapability(
    TUid aFunctionId,
    TInt& aResponse ) const
    {
    TInt error = KErrNone;

    switch ( aFunctionId.iUid )
        {
        // Supported:
        case KUidMtmQueryCanSendMsgValue:
        case KUidMtmQueryCanCreateNewMsgValue:
        case KUidMsvMtmQueryMessageInfo:
            break;

        case KUidMsvMtmQuerySupportEditor:
        case KUidMtmQuerySupportAttachmentsValue: // For SendUi
            aResponse = ETrue;
            break;

        default:
            // All others - Not Supported. At least these:
            //KUidMtmQuerySupportedBodyValue: text part(s) are attachments!
            //KUidMtmQueryMaxBodySizeValue: texts part(s) are attachments!
            //KUidMtmQuerySupportSubjectValue: subject is supported in Viewer, but not in Editor
            //KUidMsvMtmUiQueryCanPrintPreviewMsgValue: no printing in Series 60
            //KUidMsvMtmUiQueryCanPrintMsgValue: no printing in Series 60
            //KUidMtmQueryMaxRecipientCountValue
            //KUidMtmQuerySupportsBioMsgValue
            //KUidMtmQuerySendAsRequiresRenderedImageValue
            //KUidMtmQuerySendAsRenderingUidValue
            //KUidMsvMtmUiQueryExtendedOpenProgress
            //KUidMsvMtmUiQueryExtendedGetMailProgress
            //KUidMsvMtmUiQueryConnectionOrientedServices
            //KUidMsvMtmUiQueryServiceAttributes
            //KUidMsvMtmUiQueryCanGetNew
            //KUidMsvMtmUiQueryCanOpenServiceOnAction
            //KUidMtmQueryOffLineAllowedValue
            //KUidMtmQuerySupportsSchedulingValue
            //KUidMtmQuerySupportsFolderValue

	    //KUidMtmQueryMaxTotalMsgSizeValue:
            //KUidMsvMtmQueryEditorUidValue:
            //KUidMsvMtmQueryViewerUidValue:
            //KUidMsvMtmQuerySupportLinks: // flow through
            //KUidMsvMtmUiQueryMessagingInitialisation: // flow through. This is also in UI MTM!
            //KUidMsvMtmQueryFactorySettings: // This is also in UI MTM!

            error = KErrNotSupported;
        }
    return error;   
    }



// ---------------------------------------------------------
// CPostcardUiData::ContextIcon
// Returns the array of icons relevant to given entry
// ---------------------------------------------------------
//

const CBaseMtmUiData::CBitmapArray& CPostcardUiData::ContextIcon(
    const TMsvEntry& /*aContext*/,
    TInt /*aStateFlags*/ ) const
    {
    CArrayPtrFlat<CFbsBitmap>* array (NULL); 
    TRAP_IGNORE( array = new (ELeave) CArrayPtrFlat<CFbsBitmap>( 0 ) );
    return *array;
    }

// ---------------------------------------------------------
// CPostcardUiData::CanCreateEntryL
// ---------------------------------------------------------
//
TBool CPostcardUiData::CanCreateEntryL(
    const TMsvEntry& /*aParent*/,
    TMsvEntry& /*aNewEntry*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CPostcardUiData::CanOpenEntryL
// ---------------------------------------------------------
//
TBool CPostcardUiData::CanOpenEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CPostcardUiData::CanCloseEntryL
// ---------------------------------------------------------
//
TBool CPostcardUiData::CanCloseEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId=R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CPostcardUiData::CanViewEntryL
// ---------------------------------------------------------
//
TBool CPostcardUiData::CanViewEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CPostcardUiData::CanEditEntryL
// ---------------------------------------------------------
//
TBool CPostcardUiData::CanEditEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CPostcardUiData::CanDeleteFromEntryL
// ---------------------------------------------------------
//
TBool CPostcardUiData::CanDeleteFromEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CPostcardUiData::CanDeleteServiceL
// ---------------------------------------------------------
//
TBool CPostcardUiData::CanDeleteServiceL(
    const TMsvEntry& /*aService*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CPostcardUiData::CanCopyMoveToEntryL
// ---------------------------------------------------------
//
TBool CPostcardUiData::CanCopyMoveToEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CPostcardUiData::CanCopyMoveFromEntryL
// ---------------------------------------------------------
//
TBool CPostcardUiData::CanCopyMoveFromEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CPostcardUiData::CanReplyToEntryL
// ---------------------------------------------------------
//
TBool CPostcardUiData::CanReplyToEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CPostcardUiData::CanForwardEntryL
// ---------------------------------------------------------
//
TBool CPostcardUiData::CanForwardEntryL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CPostcardUiData::CanCancelL
// ---------------------------------------------------------
//
TBool CPostcardUiData::CanCancelL(
    const TMsvEntry& /*aContext*/,
    TInt& aReasonResourceId ) const
    {
    aReasonResourceId = R_EIK_TBUF_NOT_AVAILABLE;
    return EFalse;
    }

// ---------------------------------------------------------
// CPostcardUiData::StatusTextL
// Shows the text in outbox
// ---------------------------------------------------------
//
HBufC* CPostcardUiData::StatusTextL( const TMsvEntry& /*aContext*/ ) const
    {
    return HBufC::NewL( 0 );
    }

void CPostcardUiData::PopulateArraysL( )
    {
    ReadFunctionsFromResourceFileL( R_POCA_MTM_FUNCTION_ARRAY );    
    }

void CPostcardUiData::GetResourceFileName( TFileName& aFileName ) const
    {
    TFileName tmpName;
    
    // Append the Resource Files Directory
    tmpName.Append( KDC_MTM_RESOURCE_DIR );     
    // Append the Ressource File Name
    tmpName.Append( KPostcardUiResourceFile );
    
    // Obtain the drive where the DLL is installed
    TFileName dllDrive;
    Dll::FileName( dllDrive );
    
    // Obtain the Complete path for the Resource File
    TParse parse;
    parse.Set( dllDrive, NULL, NULL );
    parse.Set( parse.Drive(), &tmpName, NULL );
    TFileName fileName;
    fileName.Append( parse.FullName());
    
    // Get language of resource file
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession( ), fileName );

    if( !BaflUtils::FileExists( iCoeEnv->FsSession( ), fileName) )
        { // If it's not on dll drive -> try Z
        fileName.Replace( 0, 2, KPostcardDriveZ );
        BaflUtils::NearestLanguageFile( iCoeEnv->FsSession( ), fileName );
        }
    
    aFileName = fileName;
    
    return;
    }


// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File
