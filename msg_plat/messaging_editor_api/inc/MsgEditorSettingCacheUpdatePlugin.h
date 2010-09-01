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
*     TMsgEditorSettingCacheUpdatePlugin declaration file
*
*/



#ifndef __MSGEDITORSETTINGCACHEUPDATEPLUGIN_H
#define __MSGEDITORSETTINGCACHEUPDATEPLUGIN_H

//  INCLUDES
#include <bldvariant.hrh>
#include <AknSettingCache.h>

#include <MsgEditorAppUi.h>


// FORWARD DECLARATIONS

// CLASS DECLARATION
// ========== CLASS DECLARATION ============================

/**
* Implementation of setting cache plugin. 
* @internal
* @since 3.1
*/
NONSHARABLE_CLASS(TMsgEditorSettingCacheUpdatePlugin) : public MAknSettingCacheUpdatePlugin
    {
    public: // from MAknSettingCacheUpdatePlugin
        /**
        * handles local zoom layout switch event
        * @param aEventId event id to be queried
        * @return ETrue if the event id is handled
        */
        virtual TBool HandlesEvent(TInt aEventId) const;
    
        /**
        * Update the cached values of local zoom.
        * @param aEventId event id to be processed
        * @return ETrue if the event has caused an update, EFalse otherwise
        */
        virtual TBool Update(TInt aEventId);
        
        /**
        * Return the local zoom value
        * @param aEventId the event id
        * @param aValue output the value
        * @return KErrNone if there was no error.
        */
        virtual TInt GetValue(TInt aEventId, TInt& aValue) const;
        
        
    public: // constructors etc
        IMPORT_C TMsgEditorSettingCacheUpdatePlugin( CAknAppUi* aAppUi);
        IMPORT_C virtual ~TMsgEditorSettingCacheUpdatePlugin();
       
    private:
        TAknUiZoom iLocalZoomCache;
        CAknAppUi* iAppUi;// not owned
    };


    
#endif      // __MSGEDITORSETTINGCACHEUPDATEPLUGIN_H
            
// End of File
