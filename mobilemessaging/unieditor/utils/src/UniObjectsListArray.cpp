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
* Description:  
*       Defines implementation of CUniObjectListItemArray class methods.
*
*/



// ========== INCLUDE FILES ================================

#include "UniObjectsListArray.h"

#include <MsgAttachmentModel.h>

#include <unidatautils.h>

#include "UniObjectsInfo.h"
#include "UniObjectsModel.h"
#include "UniObjectsViewDialog.h"

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
// CUniObjectListItemArray::CUniObjectListItemArray
// ---------------------------------------------------------
//
CUniObjectListItemArray::CUniObjectListItemArray( CMsgAttachmentModel& aAttachmentModel, 
                                                  CArrayPtr<CGulIcon>* aIconArray,
                                                  CUniDataUtils& aDataUtils ):
    CMsgAttaListItemArray( aAttachmentModel, aIconArray ),
    iDataUtils( aDataUtils )
    {
    }

// ---------------------------------------------------------
// CUniObjectListItemArray::NewL
// ---------------------------------------------------------
//
CMsgAttaListItemArray* CUniObjectListItemArray::NewL( CMsgAttachmentModel& aAttachmentModel, 
                                                      CArrayPtr<CGulIcon>* aIconArray,
                                                      CUniDataUtils& aDataUtils )
    {
    CMsgAttaListItemArray* self = new( ELeave ) CUniObjectListItemArray( aAttachmentModel, 
                                                                         aIconArray,
                                                                         aDataUtils );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CUniObjectListItemArray::MdcaPoint
// ---------------------------------------------------------
//
TPtrC CUniObjectListItemArray::MdcaPoint( TInt aIndex ) const
    {
    CMsgAttachmentInfo& attInfo = iAttachmentModel.AttachmentInfoAt( aIndex );
    CUniObjectsInfo& objInfo = static_cast<CUniObjectsInfo&>( attInfo );
    CUniObjectsModel& objModel = static_cast<CUniObjectsModel&>( iAttachmentModel );

    // if application icon has been inserted previously,
    // reset the array
    if ( iIconArray->Count() )
        {
        iIconArray->Reset();
        }

    TRAP_IGNORE (
        {
        if ( objInfo.Flags() & CUniObjectsInfo::EMmsSlide )
            {
            iIconArray->AppendL( objModel.BitmapForMmsSlideL() );
            }
        else
            {
            iIconArray->AppendL( attInfo.Icon() );
            }
        }
        );

    // then get the file name from atta info full path name.
    TParsePtrC parser( attInfo.FileName() );
    TFileName filename = parser.NameAndExt();
    
    HBufC* convertedFilename = NULL;
    
    if ( filename.Length() == 0 )
        {
        filename = iDataUtils.DefaultFileName();
        }
    else
        {
        TRAP_IGNORE( convertedFilename = AknTextUtils::ConvertFileNameL( filename ) );
        
        TPtr convertedFilenamePtr = convertedFilename->Des();
        AknTextUtils::StripCharacters( convertedFilenamePtr, KStripList );
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
    if ( !( objInfo.Flags() & CUniObjectsInfo::EMmsSlide ) )
        {
        iListItemText->Append(attInfo.SizeString());
        }

    return *iListItemText;
    }
            
// End of File
