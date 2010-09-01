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
* Description:  MsgEditorSettingCacheUpdatePlugin implementation
*
*/



// ========== INCLUDE FILES ================================



#include "MsgEditorSettingCacheUpdatePlugin.h"

#include <e32std.h>                 // for GLDEF_C
#include <centralrepository.h>
#include <MessagingSDKCRKeys.h>

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
// TMsgEditorSettingCacheUpdatePlugin::TMsgEditorSettingCacheUpdatePlugin
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C TMsgEditorSettingCacheUpdatePlugin::TMsgEditorSettingCacheUpdatePlugin(CAknAppUi* aAppUi)    
    :
    iLocalZoomCache(EAknUiZoomAutomatic),
    iAppUi(aAppUi)
    {
    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL(KCRUidMessagingUiSettings) );
    if (ret == KErrNone)
        {
        TInt cenRepZoomValue = 0;
        ret = repository->Get(KMessagingEditorLocalUiZoom, cenRepZoomValue);
        if( ret == KErrNone )
            {
            iLocalZoomCache = static_cast<TAknUiZoom>(cenRepZoomValue);
            }
        }
    delete repository;          
    }


// ---------------------------------------------------------
// TMsgEditorSettingCacheUpdatePlugin::~TMsgEditorSettingCacheUpdatePlugin
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C TMsgEditorSettingCacheUpdatePlugin::~TMsgEditorSettingCacheUpdatePlugin()
    {
    }

// ---------------------------------------------------------
// TMsgEditorSettingCacheUpdatePlugin::HandlesEvent
//
// check for the KAknLocalZoomLayoutSwitch event
// ---------------------------------------------------------
//
TBool TMsgEditorSettingCacheUpdatePlugin::HandlesEvent(TInt aEventId) const
    {
    return (aEventId == KAknLocalZoomLayoutSwitch);
    }

// ---------------------------------------------------------
// TMsgEditorSettingCacheUpdatePlugin::Update
//
// Update the cached value of the local messaging Ui Local Zoom value.
// Recognises layout switch events and checks for changes
// ---------------------------------------------------------
//
TBool TMsgEditorSettingCacheUpdatePlugin::Update(TInt aEventId)
    {
    bool changed = EFalse;
    if (aEventId == KAknLocalZoomLayoutSwitch)
        {
        if(iAppUi)
            {
            TInt oldLocalZoom = iLocalZoomCache;
            CRepository* repository = NULL;
            TRAPD( ret, repository = CRepository::NewL(KCRUidMessagingUiSettings) );
            if (ret == KErrNone)
                {
                TInt cenRepZoomValue = 0;
                ret = repository->Get(KMessagingEditorLocalUiZoom, cenRepZoomValue);
                 if( ret == KErrNone )
                    {
                    iLocalZoomCache = static_cast<TAknUiZoom>(cenRepZoomValue);
                    changed = (iLocalZoomCache != oldLocalZoom);
                    }
                }
            delete repository;
            }
        }
    return changed;
    }

// ---------------------------------------------------------
// TMsgEditorSettingCacheUpdatePlugin::GetValue
// ---------------------------------------------------------
EXPORT_C TInt TMsgEditorSettingCacheUpdatePlugin::GetValue(TInt aEventId, TInt& aValue) const
    {
    if(aEventId == KAknLocalZoomLayoutSwitch)
        {
        aValue = iLocalZoomCache;
        return KErrNone;
        }
    return KErrNotSupported;
    }
 


//  End of File
