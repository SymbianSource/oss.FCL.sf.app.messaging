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
*     CMsgEditorAppUiExtension declaration file
*
*/



#ifndef __CMSGEDITORAPPUIEXTENSION_H
#define __CMSGEDITORAPPUIEXTENSION_H

//  INCLUDES
#include <bldvariant.hrh>

#include <e32base.h>
#include "MsgEditorSettingCacheUpdatePlugin.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
// ========== CLASS DECLARATION ============================
/**
* message editor appui extension object
* @since 3.2
*/
class CMsgEditorAppUiExtension : public CBase
    {
    public:
       IMPORT_C CMsgEditorAppUiExtension( CAknAppUi* iAppUi );
    public:
        TMsgEditorSettingCacheUpdatePlugin iSettingCachePlugin;
    };


#endif      // __CMSGEDITORAPPUIEXTENSION_H
            
// End of File
