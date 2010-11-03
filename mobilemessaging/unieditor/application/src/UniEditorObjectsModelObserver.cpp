/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides UniEditor Object Model Observer methods.
*
*/



// Symbian OS
#include <e32std.h>

// Symbian OS Messaging
#include <msvapi.h>                 // MTM server API

// Base editor
#include <MsgAttachmentUtils.h>

// Uni Engine
#include "UniClientMtm.h"              // Client Mtm API

// UniModel
#include <unismilmodel.h>           // SMIL api
#include <uniobject.h>
// UniUtils
#include <uniobjectsmodel.h>        // Objects view model part
#include <uniobjectsinfo.h>         // Objects view attachment info class

// Uni Editor
#include "UniEditorAppUi.h"         // UniEditorAppUi class
#include "UniEditorObjectsModelObserver.h" // CUniEditorObjectsModelObserver
#include "UniEditorDocument.h"      // UniEditorDocument class

// ----------------------------------------------------
// CUniEditorObjectsModelObserver::CUniEditorObjectsModelObserver
// ----------------------------------------------------
//
CUniEditorObjectsModelObserver::CUniEditorObjectsModelObserver( CUniEditorDocument& aDocument,
                                                                CUniEditorAppUi& aAppUi ) :
    iDocument( aDocument ),
    iAppUi( aAppUi )
    {
    }

// ----------------------------------------------------
// CUniEditorObjectsModelObserver::~CUniEditorObjectsModelObserver
// ----------------------------------------------------
//
CUniEditorObjectsModelObserver::~CUniEditorObjectsModelObserver()
    {
    }

// ----------------------------------------------------
// CUniEditorObjectsModelObserver::NotifyChanges
// ----------------------------------------------------
//
void CUniEditorObjectsModelObserver::NotifyChanges ( TMsgAttachmentCommand /* aCommand */, 
                                                     CMsgAttachmentInfo* /* aAttachmentInfo*/ )
    {
    // this shouldn't be called in unieditor but just in case.
    }

// ----------------------------------------------------
// CUniEditorObjectsModelObserver::NotifyChangesL
// ----------------------------------------------------
//
void CUniEditorObjectsModelObserver::NotifyChangesL ( TMsgAttachmentCommand aCommand, 
                                                      CMsgAttachmentInfo* aAttachmentInfo )
    {
    CUniEditorDocument* editorDoc = static_cast<CUniEditorDocument*>( &iDocument );
    CUniSmilModel& smilModel = editorDoc->DataModel()->SmilModel();

    CUniObjectsInfo* objInfo = static_cast<CUniObjectsInfo*>( aAttachmentInfo );

    switch (aCommand)
        {
        case EMsgAttachmentRemoved:
            {
            CUniObject* obj = static_cast<CUniObject*>( objInfo->Object() );
            
            if ( objInfo->AttachmentObject() )
                {
                editorDoc->DeleteAttachmentL( objInfo->SlideNumber(), obj, ETrue );
                editorDoc->SetHeaderModified( ETrue );
                }
            else
                {
                TUniLayout layout = smilModel.Layout();

                // Store the region type
                TUniRegion region = obj->Region();

                if ( obj &&
                     obj->MediaType() == EMsgMediaImage )
                    {
                    iAppUi.ReleaseImage( ETrue );
                    }
                //Coverty fix -- Forward NULL, http://ousrv057/cov.cgi?cid=36246    
                if(obj)
                    {
                    editorDoc->DeleteAttachmentL( objInfo->SlideNumber(), obj, EFalse );
              		}
              
                
                // Only call AppUI to remove object if it is located on the
                // current slide.
                if ( editorDoc->CurrentSlide() == objInfo->SlideNumber() )
                    {
                    iAppUi.RemoveObjectL( region, layout );
                    }
                    
                editorDoc->SetBodyModified( ETrue );
                }
            }
            break;

        default:
            // Enum out of bounds
            User::Leave( KErrArgument );
            break;
        }
    }

// ----------------------------------------------------
// CUniEditorObjectsModelObserver::GetAttachmentFileL
// ----------------------------------------------------
//
RFile CUniEditorObjectsModelObserver::GetAttachmentFileL( TMsvAttachmentId aId )
    {
    return iDocument.GetAttachmentFileL( aId );
    }

// ----------------------------------------------------
// CUniEditorObjectsModelObserver::NotifyOrderChangeL
// ----------------------------------------------------
//
void CUniEditorObjectsModelObserver::NotifyOrderChangeL ( TUniLayout aLayout )
    {
    CUniEditorDocument* editorDoc = static_cast<CUniEditorDocument*>(&iDocument );
    CUniSmilModel& smilModel = editorDoc->DataModel()->SmilModel();

    // Item was moved first -> Layout changes
    iAppUi.ChangeOrderL( aLayout );
    
    smilModel.SetLayoutL( aLayout );
    editorDoc->SetBodyModified( ETrue );
    }
    
// End of File

