/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plug-in view manager
*
*/



#ifndef CMAILVIEWMANAGER_H
#define CMAILVIEWMANAGER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CMailMessage;
class CMailMessageView;
class CMailBaseControlAdapter;
class CCoeControl;
class CMsgEditorView;
class MMailMsgBaseControl;
class CMsgBaseControl;

// CLASS DECLARATION

/**
*  Manages mail viewer ECOM plug-in view components.
*
*  @lib MailUtils.lib
*  @since Series 60 3.0
*/
class CMailViewManager : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMailViewManager* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMailViewManager();

    public: // New functions
        
        /**
        * MessageLoadL.
        * @since Series 60 3.0
        * @param aStatus loading status
        * @param aMessage message to be loaded.
        */
        void MessageLoadL( TInt aStatus, CMailMessage& aMessage );
        
        /**
        * AddBodyControlL.
        * @since Series 60 3.0
        * @param aParent Mail parent view.
        * @param aControl Control to be set as message body.
        */        
        void AddBodyControlL( 
				CMsgEditorView* aParent, CCoeControl& aControl );

        /**
        * AddBodyControlL.
        * @since Series 60 3.0
        * @param aParent Mail parent view.
        * @param aControl Control to be set as message body.
        */        
        void AddBodyControlL( 
				CMsgEditorView* aParent, MMailMsgBaseControl& aControl );				
				
		/**
		* ViewPlugin
		* @since Series 60 3.0
		* @return Mail view plug-in or NULL if can't load any plug-in. 
		*/				
		CMailMessageView* ViewPlugin();
		
		/**
		* PluginCount
		* @since Series 60 3.0
		* @return number of loaded plugins.
		*/
		TInt PluginCount();
		
		/**
		* RemovePlugin
		* @since Series 60 3.0
		* Removes first plugin from the array.
		*/
		void RemovePlugin();
    
    private: // implementation
        void AddBodyControlL( 
            CMsgEditorView* aParent, 
            CMsgBaseControl& aControl );
				
    private:

        /**
        * C++ default constructor.
        */
        CMailViewManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        
        /// Own: message view array
        CArrayPtr<CMailMessageView>* iMailViewArray;
        /// Own: View adapter
        CMailBaseControlAdapter* iAdapter; 

    };

#endif      // CMAILVIEWMANAGER_H   
            
// End of File
