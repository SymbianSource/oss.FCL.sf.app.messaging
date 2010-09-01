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



#ifndef CMAILBASECONTROLADAPTER_H
#define CMAILBASECONTROLADAPTER_H

//  INCLUDES
#include <MsgBaseControl.h>


// CLASS DECLARATION

/**
*  Base control adapter.
*
*  @since Series 60 3.0
*/
class CMailBaseControlAdapter : public CMsgBaseControl
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aParent Window owning parent control where 
        * aControl will be added. 
        * @param aControl a non window owning control to be added.
        */
        static CMailBaseControlAdapter* NewL(
            CCoeControl* aParent,
            CCoeControl& aControl);
        
        /**
        * Destructor.
        */
        virtual ~CMailBaseControlAdapter();
        
    public: // Functions from base classes
        
        /// From CMsgBaseControl
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
        CMailBaseControlAdapter(CCoeControl& aControl);
        void ConstructL();

    private:    // Data
        // Ref: to added control
        CCoeControl& iControl;
         
    };

#endif      // CMAILBASECONTROLADAPTER_H
            
// End of File
