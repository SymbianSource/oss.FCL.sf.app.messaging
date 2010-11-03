/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       MsgEditor Svgt media control - a Message Editor Base control
*
*/



#ifndef MSGSVGTCONTROL_H
#define MSGSVGTCONTROL_H

// INCLUDES
#include <msgmediacontrol.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMsgEditorView;
class TAknsItemID;
class CMsgBitmapControl;
class CMsgSvgUtils;

// CLASS DECLARATION

// ==========================================================

/**
* Message Editor Base control for handling svgt
*/
class CMsgSvgControl : public CMsgMediaControl
    {

    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        * @param aObserver Control state observer.
        * @param aParent   Parent control.
        *
        * @return new object
        */
        IMPORT_C static CMsgSvgControl* NewL( CMsgEditorView& aParent, 
                                              MMsgAsyncControlObserver* aObserver );

        /**
        * Destructor.
        */
        virtual ~CMsgSvgControl();

    public: // New functions
        
        /**
        * Loads SVG content indicator icon from specified file using bitmap id and mask id.
        * This indicator icon is superimposed on top of SVG icon.
        *
        * @param aId            Item Id.
        * @param aFileName      Name of the file that includes wanted icon bitmap
        * @param aFileBitmapId  ID of the icon bitmap
        * @param aFileMaskId    ID of the icon mask. -1 (default) if no mask needed.
        */
        IMPORT_C void LoadIndicatorIconL( const TAknsItemID& aId,
                                          const TDesC& aFileName,
                                          const TInt aFileBitmapId,
                                          const TInt aFileMaskId = -1 );
        
    public: 
        
        /**
        * From MMsgAsyncControl
        *
        * LoadL
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
        * From CMsgMediaControl    
        *
        * PlayL
        */  
        void PlayL();
        
        /**
        * From CMsgMediaControl    
        *
        * Stop
        */  
        void Stop();
        
        /**
        * From CMsgMediaControl    
        *
        * PauseL
        */  
        void PauseL();   
        
    public: // from CMsgBaseControl

        /**
        * Calculates and sets the size of the control and returns new size as
        * reference aSize.
        * From CMsgBaseControl
        */
        void SetAndGetSizeL( TSize& aSize );
    
    public: // from CCoeControl
        
        /**
        * Called when size is changed.
        * From CMsgBaseControl
        */
        void SizeChanged();
        
        /**
        * From CCoeControl. Handles resource change events.
        * @param aType
        */
        void HandleResourceChange( TInt aType );
        
        /**
        * From CCoeControl Return count of controls be included in this component
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl Return pointer to component in question.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
        /**
        * From CCoeControl,Draw.
        * @param aRect draw rect
        */
        void Draw( const TRect& aRect ) const;
    
    private:
        
        /**
        * C++ default constructor.
        */
        CMsgSvgControl();
                          
        /**
        * constructor
        * @param aFlags - CMsgImageControl::TFlags
        * @param aBaseControlObserver - observer to notify about changes.
        */
        CMsgSvgControl( MMsgBaseControlObserver& aBaseControlObserver );
                          
        /**
        * By default Symbian constructor is private.
        * @param aParent - parent control.
        */
        void ConstructL( CMsgEditorView& aParent,
                         MMsgAsyncControlObserver* aObserver );
        
        /**
        * Loads the thumbnail without setting the filehandle
        */
        void DoLoadL();
        
        /**
        * Calculates correct extent for icon control.
        */
        void SetIconExtent();
        
        /**
        * Calculates correct size for control.
        */
        TSize CalculateControlSize( TSize aProposedSize ) const;
               
    private: //Data

        RFile               iFileHandle;
        CMsgIconControl*    iSvgIndicator;
        CMsgSvgUtils*       iSvgUtils;
    };
    
#endif // MSGSVGTCONTROL_H
