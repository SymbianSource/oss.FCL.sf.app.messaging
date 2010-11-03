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
* Description:  MsgXhtmlBodyControl  declaration
*
*/



#ifndef CMSGXHTMLBODYCONTROL_H
#define CMSGXHTMLBODYCONTROL_H

// ========== INCLUDE FILES ================================

#include <xhtmlparser.h>
#include <MsgBodyControl.h>               // for CMsgBodyControl
#include <msgasynccontrol.h>              // for MMsgAsynchControl

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgEditorView;
class CXhtmlParser;

// ========== CLASS DECLARATION ============================

/**
* Defines a XHTML body control for message editors/viewers.
*
* @since 3.2
* @lib MsgEditorView.lib
*/
NONSHARABLE_CLASS( CMsgXhtmlBodyControl ): public CMsgBodyControl,
                                           public MMsgAsyncControl,
                                           public MXhtmlParserObserver
    {
    public:
        
        /**
        * Factory method that creates this control.
        *
        * @param aParent Parent control
        *
        * @return Pointer to creates CMsgXhtmlBodyControl object
        */
        IMPORT_C static CMsgXhtmlBodyControl* NewL( CMsgEditorView& aParent,
                                                    MMsgAsyncControlObserver* aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CMsgXhtmlBodyControl();
    
    public:
        
        /**
        * From MMsgAsyncControl
        *
        * Load content to from given file handle to control asynchronously.
        *
        * @param aFileHandle Valid file handle.
        */
        void LoadL( RFile& aFileHandle );
        
        /**
        * From MMsgAsyncControl
        *
        * Cancels the asyncronous operation if it is pending.
        */         
        void Cancel();
        
        /**
        * From MMsgAsyncControl
        *
        * Closes control. Closing means freeing file
        * specific resources that other application can use the file.
        */         
        void Close();
        
        /**
        * From MMsgAsyncControl
        *
        * Return current state.
        */
        TMsgAsyncControlState State() const;

        /**
        * From MMsgAsyncControl
        *
        * Return error code of the operation.
        */
        TInt Error() const;
        
        /**
        * From MMsgAsyncControl
        *
        * Adds observer.
        */
        void AddObserverL( MMsgAsyncControlObserver& aObserver );
        
        /**
        * From MMsgAsyncControl
        *
        * Removes observer.
        */
        void RemoveObserver( MMsgAsyncControlObserver& aObserver );

        /**
        * From MXhtmlParserObserver
        *
        * Call back function used to inform a client of the Parser
        * when a parsing operation completes.
        */
    	void ParseCompleteL();
	    
    	/**
    	* From MXhtmlParserObserver
        *
        * Call back function used to inform a client of the Parser
        * about error.
        */
    	void ParseError( TInt aError );
    	
	public:

        /**
        * From CCoeControl
        *
        * For handling dynamic layout switch.
        */
        void HandleResourceChange( TInt aType );
	
    private:
        
        /**
        * C++ default constructor.
        */
        CMsgXhtmlBodyControl();
        
        /**
        * C++ constructor.
        */
        CMsgXhtmlBodyControl( MMsgBaseControlObserver& aBaseControlObserver );

        /**
        * 2nd phase constructor.
        */
        void ConstructL( MMsgAsyncControlObserver* aObserver );
        
        /**
        * Sets state and calls observer  
        */
        void SetState( TMsgAsyncControlState aState );
        
        /**
        * Reads layout data from "LAF".
        */
        void ResolveLayout();
        
        /**
        * Performs XHTML parsing from DOM
        */ 
        void ParseXhtmlL();

        /**
        * CleanupLinkArray() cleanup for automatic variable
        * CArrayPtrFlat<CItemFinder::CFindItemExt>*.
        * @param aAny Array
        */
        static void CleanupLinkArray( TAny* aAny );

    private: // Data
        
        CArrayPtrFlat<MMsgAsyncControlObserver>* iObservers;
        
        TMsgAsyncControlState iState;
        
        TInt iError;
        
        CXhtmlParser* iParser;
    };

#endif // CMSGXHTMLBODYCONTROL_H

// End of File
