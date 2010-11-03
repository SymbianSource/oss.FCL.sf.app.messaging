/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgAttaListItemArray implementation
*
*/



// ========== INCLUDE FILES ================================
#include <msvapi.h>
#include <eikspane.h>
#include <eikmenup.h>
#include <AknDef.h>
#include <aknappui.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <aknlists.h>
#include <gulicon.h>
#include <coeutils.h>               // for ConeUtils::FileExists
#include <bautils.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>          // CAknWaitDialog
#include <DocumentHandler.h>
#include <StringLoader.h>           // StringLoader
#include <AiwCommon.h>
#include <NpdApi.h>

#include <mmsvattachmentmanager.h>

#include <avkon.hrh>

#include <aknlayout.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include <AknUtils.h>
#include <AknsUtils.h>          // skinned icons
#include <AknsConstants.h>      // skinned icon ids

#include "MsgEditorCommon.h"        // for KMsgEditorMbm
#include "MsgEditorDocument.h"
#include "MsgAttachmentInfo.h"
#include "MsgAttachmentModel.h"
#include "MsgViewAttachmentsDialog.h"
#include "MsgAttachmentUtils.h"
#include "MsgAttaListItemArray.h"

#include <msgeditor.mbg>
#include "MsgEditorAppUi.hrh"
#include <MsgEditorAppUi.rsg>       // resouce identifiers

#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

_LIT( KStripList, "\x000A\x000D\x2029\x2028\x0085" );

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgAttaListItemArray::NewL
//
//
// ---------------------------------------------------------
//
EXPORT_C CMsgAttaListItemArray* CMsgAttaListItemArray::NewL(
    CMsgAttachmentModel& aAttachmentModel,
    CArrayPtr<CGulIcon>* aIconArray )
    {
    CMsgAttaListItemArray* self = new ( ELeave ) CMsgAttaListItemArray(
        aAttachmentModel,
        aIconArray );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self

    return self;
    }

// ---------------------------------------------------------
// CMsgAttaListItemArray::~CMsgAttaListItemArray
//
//
// ---------------------------------------------------------
//
EXPORT_C CMsgAttaListItemArray::~CMsgAttaListItemArray()
    {
    delete iListItemText;
    }

// ---------------------------------------------------------
// CMsgAttaListItemArray::ConstructL
//
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgAttaListItemArray::ConstructL()
    {
    iListItemText = new ( ELeave ) TBuf<KMsgAttaItemTextLength>;
    }

// ---------------------------------------------------------
// CMsgAttaListItemArray::MdcaCount
//
//
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgAttaListItemArray::MdcaCount() const
    {
    return iAttachmentModel.NumberOfItems();
    }

// ---------------------------------------------------------
// CMsgAttaListItemArray::MdcaPoint
//
//
// ---------------------------------------------------------
//
TPtrC CMsgAttaListItemArray::MdcaPoint(TInt aIndex) const
    {
    CMsgAttachmentInfo& attInfo = iAttachmentModel.AttachmentInfoAt( aIndex );

    // if application icon has been inserted previously, delete it from index 0
    // and insert new app icon to index 0.
    iIconArray->Reset();
    TRAP_IGNORE( iIconArray->AppendL( attInfo.Icon() ) );

    // then get the file name from atta info full path name.
    TParsePtrC parser( attInfo.FileName() );
    TFileName filename( parser.NameAndExt() );
    
    HBufC* convertedFilename = NULL;
    
    if ( filename.Length() == 0 )
        {
        CEikonEnv::Static()->ReadResource( filename, R_QTN_MMS_NO_NAME_FOR_FILE );
        }
    else
        {
        TRAP_IGNORE( convertedFilename = AknTextUtils::ConvertFileNameL( filename ) );
        
        TPtr convertedFileNamePtr = convertedFilename->Des();
        AknTextUtils::StripCharacters( convertedFileNamePtr, KStripList );
        }
        
    // generate item text for list item by combining
    // icon index (always zero), file name and file size.
    iListItemText->Zero();

    iListItemText->AppendNum( 0 );
    iListItemText->Append( KColumnListSeparator );
    
    if ( convertedFilename )
        {
        iListItemText->Append( *convertedFilename );
        delete convertedFilename;
        }
    else
        {
        iListItemText->Append( filename );
        }
    
    iListItemText->Append( KColumnListSeparator );
    iListItemText->Append( attInfo.SizeString() );

    return *iListItemText;
    }

// ---------------------------------------------------------
// CMsgAttaListItemArray::CMsgAttaListItemArray
//
//
// ---------------------------------------------------------
//
EXPORT_C CMsgAttaListItemArray::CMsgAttaListItemArray(
    CMsgAttachmentModel& aAttachmentModel,
    CArrayPtr<CGulIcon>* aIconArray )
    : iAttachmentModel( aAttachmentModel ),
      iIconArray( aIconArray )
    {
    }

// End of File
