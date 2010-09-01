/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       SMIL Player media renderer for displaying text files
*
*/



#ifndef SMILTEXTRENDERER_H
#define SMILTEXTRENDERER_H

//  INCLUDES
#include "SmilMediaRendererBase.h"

#include <e32base.h>
#include <frmtlay.h>
#include <AknPictographDrawerInterface.h>

#include <eiksbfrm.h>

#include <xhtmlparser.h> 

// FORWARD DECLARATIONS
class CRichText;
class CSmilTransitionFilter;
class CFbsBitmap;
class CSmilTextRendererCustomDraw;
class CAknPictographInterface;  //pictograph support
class CGraphicsContext;

// CLASS DECLARATION

/**
* SMIL Player media renderer for displaying text files
*
* @lib smilmediarenderer.lib
* @since 2.0
*/
NONSHARABLE_CLASS(CSmilTextRenderer) : public CSmilMediaRendererBase,
                                       public MEikScrollBarObserver,
                                       public MXhtmlParserObserver
    {

    public: // Constructors and destructor
        
        /**
        * Factory method that creates this object.
        *
        * @param    aFileHandle IN File handle to the media file
        * @param    aMedia      IN SMIL engine side object representing
        *                          this media file
        * @param    aDrmCommon  IN DRM Common
        * @param    aDrmHelper  IN DRM Helper
        * @param    aCharset    IN IANA character set MIB enumeration
        *                          of the media file
        * @param    aGc         IN graphics context
        *
        * @return   pointer to instance
        */
        static CSmilTextRenderer* NewL( RFile& aFileHandle,
                                        MSmilMedia* aMedia,
                                        DRMCommon& aDrmCommon,
                                        CDRMHelper& aDrmHelper,
                                        TUint aCharset,
                                        CGraphicsContext& aGc,
                                        TMsgMediaType aMediaType 
										);
        /**
        * Destructor.
        */
        virtual ~CSmilTextRenderer();

    public: // Functions from base classes
    
        /**
        * From MSmilMediaRenderer Returns if renderer is visual 
        * (i.e. draws something to the screen).
        *
        * @since 2.0
        * 
        * @return Returns ETrue since the media type is visual.
        */
        TBool IsVisual() const;
        
        /**
        * From MSmilMediaRenderer Returns if renderer is scrollable.
        *
        * @since 2.0
        *
        * @return Returns ETrue if text is scrollable and EFalse if
        * it is not.
        */
        TBool IsScrollable() const;

        /**
        * From MSmilMediaRenderer Move scrollable media to given position.
        *
        * @since 2.0
        *
        * @param aDirX  IN  Amount of pixels to move on X direction.
        * @param aDirY  IN  Amount of pixels to move on Y direction.
        *
        * @return void
        */
        void Scroll( TInt aDirX, TInt aDirY );
        
        /**
        * From MSmilMediaRenderer Returns the unscalable size of the
        * visual media object.
        *
        * @since 2.0
        * 
        * @return Width in pixels.
        */
        TInt IntrinsicWidth() const;

        /**
        * From MSmilMediaRenderer Returns the unscalable size of the
        * visual media object.
        *
        * @since 2.0
        * 
        * @return Height in pixels.
        */
        TInt IntrinsicHeight() const;   
        
        /**
        * From MSmilMediaRenderer Called by the engine to seek the media
        * to given position in its local timeline.
        *
        * @since 2.0
        * 
        * @param aTime  IN  Seek position.
        *
        * @return void
        */
        void SeekMediaL( const TSmilTime& aTime );
        
        /**
        * From MSmilMediaRenderer Set volume of the audio media.
        *
        * @since 2.0
        *
        * @param aVolume    IN  A percentage value from maximum value that
        *                       the volume should be set.
        * @return void
        */
        
        /**
        * From MSmilMediaRenderer Called by the engine to signal that
        * the media is going to be played soon.
        *
        * @since 2.0
        *
        * @return void
        */
        void PrepareMediaL();       
        
        /**
        * From MSmilMediaRenderer Called by the engine to start media
        * playback and to make visual media visible.
        *
        * @since 2.0
        * 
        * @return void
        */
        void ShowMediaL();
        
        /**
        * From MSmilMediaRenderer Called by the engine to stop media
        * playback and remove visual media from the screen.
        *
        * @since 2.0
        *
        * @return void
        */
        void HideMedia();
        
        /**
        * From MSmilMediaRenderer Called by the engine to pause media playback.
        *
        * @since 2.0
        *
        * @return void
        */
        void FreezeMedia();
        
        /**
        * From MSmilMediaRenderer Called by the engine to continue paused
        * media playback.
        *
        * @since 2.0
        *
        * @return void
        */
        void ResumeMedia();
        
        /**
        * From MSmilMediaRenderer Called by the engine to make a media
        * renderer draw its content.
        *
        * @since 2.0
        *
        * @param aGc    IN      Graphical context performing the drawing.
        * @param aRect  IN      Target rectangle.
        * @param aTransitionFilter IN Transition effects if present.
        * @param aFocus IN      Focus indication if focussed.
        *
        * @return void
        */
        void Draw( CGraphicsContext& aGc,
                   const TRect& aRect,
                   CSmilTransitionFilter* aTransitionFilter,
                   const MSmilFocus* aFocus );

        /**
        * Return text of the renderer
        *
        * @since 2.0
        *
        * @return Pointer to plain text object
        */
        IMPORT_C CPlainText* GetText();

        /**
        * Redraw pictographarea in animation callback
        *
        * @since 2.0
        */
        void DrawPictoCallback() const;
        
        /**
        * from MEikScrollBarObserver
        *   Handles the scrollevents
        */
        void HandleScrollEventL( CEikScrollBar* aScrollBar, 
                        TEikScrollEvent aEventType );
                        


    private:

        /**
        * C++ default constructor.
        */
        CSmilTextRenderer();
        
        /**
        * Constructor.
        */
        CSmilTextRenderer( MSmilMedia* aMedia, 
                           DRMCommon& aDrmCommon,
                           CDRMHelper& aDrmHelper,
                           TUint aCharset,
                           TMsgMediaType aMediaType 
						   );
        /**
        * 2nd phase constructor
        */
        void ConstructL( RFile& aFileHandle, CGraphicsContext& aGc ); 

        /**
        * Draws scroll bar
        */
        void DrawScrollBarL( CGraphicsContext& aGc,
                             const TRect& aRect ) ;

        /**
        * Draws transition
        */
        void DrawTransitionL( CGraphicsContext& aGc,
                              CSmilTransitionFilter* aTransitionFilter,
                              const TRect& aRect );
        
        /**
        * Creates text layout.
        */
        void CreateLayoutL();

        /**
        * Opens text file pointed by file handle.
        */
        void OpenTextL( RFile& aFileHandle );

        /**
        * Setup correct format for text
        */
        void FormatTextL( const CGraphicsContext& aGc );

        /**
        * Resolves text color.
        */
        TRgb ResolveTextColorL( TRgb aDefaultColor ) const;
        
        /**
        * Scrolls the text without changing manually the thumb
        *   position.
        */                        
        void ScrollWithOutThumbUpdate( TInt aDirY );
        
        /**
        * from MXhtmlParserObserver
        *   Handles the parse events
     	* Call back function used to inform a client of the Parser
	    * when a parsing operation completes.
	    *
	    * @since S60 v3.2
	    */
		void ParseCompleteL();
		
		/**
        * from MXhtmlParserObserver
        *   Handles the parse events
     	* Call back function used to inform a client of the Parser
	    * about error.
	    *
	    * @since S60 v3.2
	    */
		void ParseError( TInt aError );
    private: // data
        
        // Character set
        TUint iCharset;

        // Rich text object
        CRichText* iRichText;
        
        // Paragraph formatter
        CParaFormatLayer* iParaFormatLayer;
        
        // Character formatter
        CCharFormatLayer* iCharFormatLayer;
        
        // Text layout
        CTextLayout* iLayout;
        
        // Text draw context
        TDrawTextLayoutContext iDrawContext;
        
        // Specifies whether text renderer is visible
        TBool iShowing;
        
        // Specifies whether scroll bar is enabled
        TBool iScrollBar;
        
        // Text Y position
        TInt iYPos;
        
        // Custom text drawer
        CSmilTextRendererCustomDraw* iCustomDraw;
        
        // Buffer bitmap
        CFbsBitmap* iBufferBitmap;
        
        // Buffer bitmap device
        CFbsBitmapDevice* iBufferDevice;
        
        // Mask bitmap
        CFbsBitmap* iMaskBitmap;
        
        // Mask bitmap device
        CFbsBitmapDevice* iMaskDevice;

        // Line length
        TInt iLineHeight;

#ifdef RD_MSG_XHTML_SUPPORT
		// XHTML parser
		CXhtmlParser* iXhtmlParser;
#endif
				
		// Parser error
		TInt iError;
		        
#ifdef RD_SCALABLE_UI_V2        
        // Scrollbar frame
        CEikScrollBarFrame* iSBFrame;
        
        //does the scrollbar need to be initialised and drawn
        TBool iNotYetDrawn;
        
        //save the thumb position if replay is needed
        TInt iThumbPos;
        
        //from scrollbar, member because maxvalue of scrollbar 
        // is uninitialised and anyhow this is more efficient
        TInt iScrollSpan;
#endif // RD_SCALABLE_UI_V2       
        };

/**
*  Implementation of MFormCustomDraw
*       Class is used to draw transparent background
*       by empty implementation of DrawBackground(...)
*       In addition class is used to get mask by using iWhite -member.
*
* @lib smilmediarenderer.lib
* @since 2.0
*/
NONSHARABLE_CLASS(CSmilTextRendererCustomDraw): public CBase,
                                    public MFormCustomDraw,
                                    public MAknPictographAnimatorCallBack
    {

    public:

        /**
        * Static constructor.
        *
        * @param    aParent IN Parent text renderer
        * @param    aGc     IN Graphic context.
        *
        * @return   pointer to instance
        */
        static CSmilTextRendererCustomDraw* NewL( const CSmilTextRenderer& aParent,
                                                  CGraphicsContext& aGc );

        /**
        * Destructor
        */
        ~CSmilTextRendererCustomDraw();

        /**
        * SetWhite
        *
        * @since 2.0
        * 
        * @param aWhite IN Specifies whether white is enabled.
        */
        void SetWhite( TBool aWhite );

    public: // Functions from base classes
        
        /**
        * From MFormCustomDraw
        * See frmtlay.h for more information.
        */
        void DrawBackground( const TParam& aParam,
                             const TRgb& aRgb,
                             TRect& aDrawn ) const;

        /**
        * From MFormCustomDraw
        * See frmtlay.h for more information.
        */
        void DrawText( const TParam& aParam,
                       const TLineInfo& aLineInfo,
                       const TCharFormat& aFormat,
                       const TDesC& aText,
                       const TPoint& aTextOrigin,
                       TInt aExtraPixels ) const;

        /**
        * From MAknPictographAnimatorCallBack
        * See AknPictographDrawerInterface.h for more information.
        */
        void DrawPictographArea();

    private:

        /**
        * C++ contructor
        */
        CSmilTextRendererCustomDraw( const CSmilTextRenderer& aParent,
                                     CGraphicsContext& aGc );
                                     
        /**
        * 2nd phase constructor
        */
        void ConstructL();

    private: // data

        TBool iWhite;
        const CSmilTextRenderer& iParent;
        CAknPictographInterface* iPictographDrawer;
        CGraphicsContext& iGc; 

        };

#endif // SMILTEXTRENDERER_H
