/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Adapter between CMsgBaseControl and CCoeControl
*
*/



#ifndef CMAILMSGBASECONTROLADAPTER_H
#define CMAILMSGBASECONTROLADAPTER_H

//  INCLUDES
#include <MsgBaseControl.h>


// CLASS DECLARATION
class MMailMsgBaseControl;

/**
*  Base control adapter.
*
*  @since Series 60 3.0
*/
class CMailMsgBaseControlAdapter : public CMsgBaseControl
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aParent Window owning parent control where 
        * aControl will be added. 
        * @param aControl a non window owning control to be added.
        */
        static CMailMsgBaseControlAdapter* NewL(
            CCoeControl* aParent,
            MMailMsgBaseControl& aBaseControl);
        
        /**
        * Destructor.
        */
        virtual ~CMailMsgBaseControlAdapter();
        
    public: // Functions from base classes
        
        /// From CMsgBaseControl
        void NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam );
        TRect CurrentLineRect();
        void SetAndGetSizeL(TSize& aSize);        
        void ClipboardL(TMsgClipboardFunc aFunc);
        void EditL(TMsgEditFunc aFunc);
        TBool IsFocusChangePossible(TMsgFocusDirection aDirection) const;
        TBool IsCursorLocation(TMsgCursorLocation aLocation) const;
        TUint32 EditPermission() const;
        TInt VirtualHeight();
        TInt VirtualVisibleTop();
        
		// From CCoeControl
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(TInt aIndex) const;
        void SizeChanged();
        void SetExtent(const TPoint& aPosition,const TSize& aSize);
        void SetPosition(const TPoint& aPosition);
        void FocusChanged(TDrawNow aDrawNow);
		void HandleResourceChange(TInt aType);
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

    protected:  // Functions from base classes
        
        /// From CMsgBaseControl
        void PrepareForReadOnly(TBool aReadOnly);

    private:  // Implementation
       
    private:

        /**
        * Constructor.
        */
        CMailMsgBaseControlAdapter(MMailMsgBaseControl& aBaseControl);
        void ConstructL();

    private:    // Data
        // Ref: to added control
        MMailMsgBaseControl& iBaseControl;
         
    };

#endif      // CMAILMSGBASECONTROLADAPTER_H  
            
// End of File
