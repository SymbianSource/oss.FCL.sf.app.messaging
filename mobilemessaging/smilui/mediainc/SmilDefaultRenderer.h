/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       Implementation of SmilDefaultRenderer,
*       a renderer for not supported or corrupted media
*
*/



#ifndef DEFAULT_RENDERER_H
#define DEFAULT_RENDERER_H

//  INCLUDES
#include "SmilMediaRendererBase.h"

// DATA TYPES

// FORWARD DECLARATIONS
class MSmilFocus;
class CGulIcon;

// CLASS DECLARATION

/**
* CSmilDefaultRenderer provides SMIL media renderer interface.
* This class is used by SMIL Engine for rendering unknown /
* corrupted / DRM protected media items.
*
* @lib smilmediarenderer.lib
* @since 2.0
*/
NONSHARABLE_CLASS(CSmilDefaultRenderer) : public CSmilMediaRendererBase
    {
    public:  // Methods

    // Constructors and destructor

        /**
        * Static constructor.
        *
        * @since 2.0
        *
        * @param aFileHandle IN     File handle to the media file
        * @param aMedia      IN     SMIL media object.
        * @param aDrmCommon  IN     DRM Common
        * @param aDrmHelper  IN     DRM Helper
        * @param aMediaType  IN     Original media item type. 
        *                           Used to differentiate shown icon.
        * @param aDRMError   IN     DRM error flag.
        *                           Used to differentiate shown icon.
        *
        * @return Initialized default renderer object.
        */
        static CSmilDefaultRenderer* NewL( RFile& aFileHandle,
                                           MSmilMedia* aMedia,
                                           DRMCommon& aDrmCommon,
                                           CDRMHelper& aDrmHelper,
                                           TMsgMediaType aMediaType,
                                           TBool aDRMError = EFalse );

        /**
        * Destructor.
        *
        * @since 2.0
        */
        virtual ~CSmilDefaultRenderer();

    // New methods

    // Methods from base classes

    public: // From MSmilMediaRenderer

       /**
        * From MSmilMediaRenderer Called by the SMIL engine to signal
        * that the engine is no more referencing renderer.
        *
        * @since 2.0
        *
        * @return void
        */
        void Close();

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
        * From MSmilMediaRenderer Returns the duration of the media object.
        *
        * @since 2.0
        *
        * @return Returns zero (Note! zero duration means still
        * image).
        */
        TSmilTime IntrinsicDuration() const;
        
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

    protected:  // Methods

    // New Methods

    // Methods from base classes

    private: //Methods

        /**
        * C++ default constructor
        */
        CSmilDefaultRenderer();

        /**
        * Constructor.
        */
        CSmilDefaultRenderer( MSmilMedia* aMedia,
                              DRMCommon& aDrmCommon,
                              CDRMHelper& aDrmHelper );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RFile& aFileHandle,
                         TBool aDRMError, 
                         TMsgMediaType aMediaType );
        
        /**
        * Calculates correct icon size.
        */
        TSize CalculateIconFitSize( const TSize& aRegion,
                                    const TSize& aIcon ) const;

        /**
        * Calculates correct icon position.
        */
        TPoint CalculateIconCenterPosition( const TSize& aRegion,
                                            const TSize& aIcon ) const;

    public:     // Data

    protected:  // Data 

    private:    // Data

        /** Specifies if renderer is visible. */
        TBool iVisible;

        /** Icon to draw */
        CGulIcon* iIcon;
        
        /** Icon rectangle inside overall renderer region. */
        TRect iIconRect;
    };

#endif // DEFAULT_RENDERER_H
            
// End of File
