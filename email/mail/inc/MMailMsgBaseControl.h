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
* Description:  Adapter API between Editor base and plug-in control
*
*/



#ifndef _MMAILMSGBASECONTROL_H
#define _MMAILMSGBASECONTROL_H

//  INCLUDES
#include <MsgBaseControl.h>
#include <MsgEditor.hrh>
#include <coedef.h>
#include <w32std.h>

// CLASS DECLARATION
class CCoeControl;
/**
*  Mail viewer internal API for Base control adapter.
*
*  @since Series 60 3.0
*/
class MMailMsgBaseControl
    {
    public:  // Constructors and destructor
                
        /**
        * Destructor.
        */
        virtual ~MMailMsgBaseControl(){};
        
    public: // New methods See <MsgBaseControl.h>
        
        virtual TRect CurrentLineRect() = 0;
        virtual void SetAndGetSizeL(TSize& aSize) = 0;        
        virtual void ClipboardL(TMsgClipboardFunc aFunc) = 0;
        virtual void EditL(TMsgEditFunc aFunc) = 0;
        virtual TBool IsFocusChangePossible(
            TMsgFocusDirection aDirection) const = 0;
        virtual TBool IsCursorLocation(TMsgCursorLocation aLocation) const = 0;
        virtual TUint32 EditPermission() const = 0;
        virtual TInt VirtualHeight() = 0;
        virtual TInt VirtualVisibleTop() = 0;
        virtual void PrepareForReadOnly(TBool aReadOnly) = 0;
        
        
        virtual TInt CountPluginComponentControls() const = 0;
        virtual CCoeControl* PluginComponentControl(TInt aIndex) const = 0;
        virtual void PluginSizeChanged() = 0;
        virtual void PluginFocusChanged(TDrawNow aDrawNow) = 0;
		virtual void PluginHandleResourceChange(TInt aType) = 0;
        virtual TKeyResponse PluginOfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType) = 0;
        /**
        * Set plugin extent
        */    
        virtual void SetPluginExtent(const TPoint& aPosition,const TSize& aSize) = 0;    
        
    };

#endif      // _MMAILMSGBASECONTROL_H  
            
// End of File
