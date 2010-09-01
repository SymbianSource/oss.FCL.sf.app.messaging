/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of SmilSVGRenderer.
*
*/




#ifndef CSMILSVGRENDERER_H
#define CSMILSVGRENDERER_H

//  INCLUDES
#include <SVGTAppObserver.h>
#include <SVGRequestObserver.h>
#include "SmilMediaRendererBase.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CGulIcon;
class MMediaFactoryFileInfo;
class MSvgError;
class CSVGTAppObserverUtil;

// CLASS DECLARATION

/**
*  CSmilSVGRenderer provides SMIL media renderer interface to render
*  SVG-T files.
*
*  This class is used by SMIL engine to perform the SVG presentation
*  playback.
*
*  @lib mediarenderer.lib
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CSmilSVGRenderer ) : public CSmilMediaRendererBase,
                                        public MSvgtAppObserver,
                                        public MSvgRequestObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aFileHandle IN Source file handle.
        * @param aMedia      IN SMIL media object.
        * @param aFileInfo   IN File information interface.
        *
        * @return Initialized SVG renderer.
        */
        static CSmilSVGRenderer* NewL( RFile& aFileHandle,
                                       MSmilMedia* aMedia,
                                       MMediaFactoryFileInfo* aFileInfo,
                                       DRMCommon& aDrmCommon,
                                       CDRMHelper& aDrmHelper );
        
        /**
        * Destructor.
        */
        virtual ~CSmilSVGRenderer();

    public: // Functions from base classes
        
        /**
        * From MSmilMediaRenderer Returns if renderer is visual 
        * (i.e. draws something to the screen).
        *
        * @since S60 3.2
        * 
        * @return Returns ETrue since the media type is visual.
        */
		TBool IsVisual() const;
		
		/**
        * From MSmilMediaRenderer Returns the unscalable size of the
        * visual media object.
        *
        * @since S60 3.2
        * 
        * @return Width in pixels of SVG thumbnail.
        */
		TInt IntrinsicWidth() const;
		
		/**
        * From MSmilMediaRenderer Returns the unscalable size of the
        * visual media object.
        *
        * @since S60 3.2
        * 
        * @return Height in pixels of SVG thumbnail.
        */
		TInt IntrinsicHeight() const;
		
		/**
        * From MSmilMediaRenderer Returns the duration of the media object.
        *
        * @since S60 3.2
        *
        * @return Returns duration of SVG thumbnail.
        */
		TSmilTime IntrinsicDuration() const;
		
		/**
        * From MSmilMediaRenderer Called by the engine to signal that
        * the media is going to be played soon.
        *
        * @since S60 3.2
        *
        * @return void
		*/
		void PrepareMediaL();
		
		/**
        * From MSmilMediaRenderer Called by the engine to seek the media
        * to given position in its local timeline.
        *
        * @since S60 3.2
        * 
        * @param aTime  IN  Seek position.
        *
        * @return void
		*/
		void SeekMediaL( const TSmilTime& aTime );
		
		/**
        * From MSmilMediaRenderer Called by the engine to start media
        * playback and to make visual media visible.
        *
        * @since S60 3.2
        * 
        * @return void
		*/
		void ShowMediaL();

        /**
        * From MSmilMediaRenderer Called by the engine to stop media
        * playback and remove visual media from the screen.
        *
        * @since S60 3.2
        *
        * @return void
		*/
		void HideMedia();
		
		/**
        * From MSmilMediaRenderer Called by the engine to pause media playback.
        *
        * @since S60 3.2
        *
        * @return void
		*/
		void FreezeMedia();

        /**
        * From MSmilMediaRenderer Called by the engine to continue paused
        * media playback.
        *
        * @since S60 3.2
        *
        * @return void
		*/
		void ResumeMedia();
		
		/**
        * From MSmilMediaRenderer Called by the engine to make a media
        * renderer draw its content.
        *
        * @since S60 3.2
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
        * From MSmilMediaRenderer Called by the UI to activate renderer.
        *
        * @since S60 3.2
        *
        * @return void
		*/
        IMPORT_C void ActivateRendererL();
        
        /**
        * From MSvgtAppObserver. See SVGTAppObserver.h for more information.
        */
        TInt FetchImage( const TDesC& aUri, RFs& aSession, RFile& aFileHandle );
        
        /**
        * From MSvgtAppObserver. See SVGTAppObserver.h for more information.
        */
        TBool LinkActivated( const TDesC& aUri );

        /**
        * From MSvgtAppObserver. See SVGTAppObserver.h for more information.
        */
        TBool LinkActivatedWithShow( const TDesC& aUri, const TDesC& aShow );
        
        /**
        * From MSvgtAppObserver. See SVGTAppObserver.h for more information.
        */
        void DisplayDownloadMenuL();
        
        /**
        * From MSvgtAppObserver. See SVGTAppObserver.h for more information.
        */
        void GetSmilFitValue( TDes& aSmilValue );
                                                     
        /**
        * From MSvgtAppObserver. See SVGTAppObserver.h for more information.
        */
        TBool CanShowSave();
        
        /**
        * From MSvgtAppObserver. See SVGTAppObserver.h for more information.
        */
        TBool IsSavingDone();
        
        /**
        * From MSvgtAppObserver. See SVGTAppObserver.h for more information.
        */
        TBool CanShowDownload() const;
        
        /**
        * From MSvgtAppObserver. See SVGTAppObserver.h for more information.
        */
        void DoSaveL( TInt aButtonid );

        /**
        * From MSvgtAppObserver. See SVGTAppObserver.h for more information.
        */
        TInt NewFetchImageData( const TDesC& aUri );

        /**
        * From MSvgtAppObserver. See SVGTAppObserver.h for more information.
        */
        void AssignEmbededDataL( const TDesC& aUri );
        
        /**
        * From MSvgRequestObserver. See SVGRequestObserver.h for more information.
        */
        void UpdateScreen();

        /**
        * From MSvgRequestObserver. See SVGRequestObserver.h for more information.
        */
        TBool ScriptCall( const TDesC& aScript, CSvgElementImpl* aCallerElement );

		/**
        * From MSvgRequestObserver. See SVGRequestObserver.h for more information.
        */
		TInt FetchFont( const TDesC& aUri, RFs& aSession, RFile& aFileHandle );

        /**
        * From MSvgRequestObserver. See SVGRequestObserver.h for more information.
        */
        void UpdatePresentation( const TInt32&  aNoOfAnimation );
        /**
         * From MSvgtAppObserver. See SVGTAppObserver.h for more information.
         */
        TBool ExitWhenOrientationChange();      
    private:

        /**
        * C++ default constructor.
        */
        CSmilSVGRenderer();
        
        /**
        * C++ constructor.
        */
        CSmilSVGRenderer( MSmilMedia* aMedia,
                          MMediaFactoryFileInfo* aFileInfo,
                          DRMCommon& aDrmCommon,
                          CDRMHelper& aDrmHelper );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RFile& aFileHandle );

        /**
        * Create thumbnail from SVG file.
        */
        void GenerateThumbnailL();
        
        /**
        * Performs SVG engine error handling.
        */
        void HandleSVGEngineErrorL( MSvgError* aError ) const;
        
    private:    // Data
        
        // Presentation thumbnail bitmap.
        CFbsBitmap* iThumbnailBitmap;
        
        // Presentation thumbnail mask.
        CFbsBitmap* iThumbnailMask;
        
        // Presentation thumbnail indicator icon.
        CGulIcon* iIndicatorIcon;
        
        // Indicates whether renderer is visible.
        TBool iVisible;
        
        // Handle to source file.
        RFile iFileHandle;
        
        // File information interface.
        MMediaFactoryFileInfo* iFileInfo;
        
        // Size of the SVG thumbnail.
        TSize iThumbnailSize;
        
        // Size of the superimposed indicator.
        TSize iIndicatorSize;
        
        // Helper class for SVG-T dialog observer.
        CSVGTAppObserverUtil* iObserverUtil;
    };

#endif      // CSMILSVGRENDERER_H   
            
// End of File
