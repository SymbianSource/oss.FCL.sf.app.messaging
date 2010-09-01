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
*       Base class for SMIL Player media renderers
*
*/




#ifndef CSMILMEDIARENDERERBASE_H
#define CSMILMEDIARENDERERBASE_H

#include <coecntrl.h>
#include <smilmediarendererinterface.h>
#include <caf/caftypes.h>
#include <DRMCommon.h>

#include <MsgMedia.hrh>

// CONSTANTS
const TInt KFocusSize = 2;
const TInt KScrollBarSize = 5;
const TInt KScrollElevatorSize = 3;

// MACROS

// FORWARD DECLARATIONS
class DRMCommon;
class CDRMHelper;
class MSmilMedia;

/**
* Base class for SMIL Player media renderers
*
* @lib smilmediarenderer.lib
* @since 2.0
*/
NONSHARABLE_CLASS(CSmilMediaRendererBase) : public CCoeControl,
                                            public MSmilMediaRenderer
    {
    public:

        /**
        * Destructor.
        *
        * @since 2.0
        */
        virtual ~CSmilMediaRendererBase();

    public: // New functions

        /**
        * Return renderer type.
        *
        * @since 2.0
        *
        * @return Media type of the class. 
        */
        IMPORT_C TMsgMediaType MediaType() const;
        
        /**
        * Called by the UI to activate renderer.
        *
        * @since 3.0
        *
        * @return void
		*/
        IMPORT_C virtual void ActivateRendererL();
    
    public: // Methods from base classes
    
        /**
        * From MSmilMediaRenderer Called by the SMIL engine to signal
        * that the engine is no more referencing renderer. Default
        * implementation deletes the renderer.
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
        * @return Default implementation returns EFalse.
        */
        TBool IsVisual() const;

        /**
        * From MSmilMediaRenderer Returns if renderer is non-transparent
        * (i.e. fills all the pixels on specied region).
        *
        * @since 2.0
        *
        * @return Default implementation returns EFalse.
        */
        TBool IsOpaque() const;

        /**
        * From MSmilMediaRenderer Returns if renderer is scrollable.
        *
        * @since 2.0
        *
        * @return Default implementation returns EFalse.
        */
        TBool IsScrollable() const;

        /**
        * From MSmilMediaRenderer Returns if renderer is Symbian control.
        *
        * @since 2.0
        *
        * @return Default implementation returns ETrue since all the 
        * renderers are implemented as controls.
        */
        TBool IsControl() const;

        /**
        * From MSmilMediaRenderer Returns the unscalable size of the
        * visual media object.
        *
        * @since 2.0
        * 
        * @return Default implementation returns zero.
        */
        TInt IntrinsicWidth() const;

        /**
        * From MSmilMediaRenderer Returns the unscalable size of the
        * visual media object.
        *
        * @since 2.0
        * 
        * @return Default implementation returns zero.
        */
        TInt IntrinsicHeight() const;

        /**
        * From MSmilMediaRenderer Returns the duration of the media object.
        *
        * @since 2.0
        *
        * @return Default implementation returns zero.
        */
        TSmilTime IntrinsicDuration() const;

        /**
        * From MSmilMediaRenderer Called by the engine to make a media
        * renderer draw its content. Default implementation is empty.
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
        * From MSmilMediaRenderer Move scrollable media to given position.
        * Default implementation is empty.
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
        * From MSmilMediaRenderer Set volume of the audio media.
        * Default implmenetation is empty.
        *
        * @since 2.0
        *
        * @param aVolume    IN  A percentage value from maximum value that
        *                       the volume should be set.
        * @return void
        */
        void SetVolume( TInt aVolume );
    
        /**
        * From CCoeControl. Handles key events.
        *
        * @param aKeyEvent  IN The key event.
        * @param aType      IN The type of key event
        *
        * @return Indicates whether or not the key event was used by this control.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );
        
    protected:
        
        /**
        * C++ default contsructor.
        */
        CSmilMediaRendererBase();
        
        /**
        * Constructor.
        *
        * @param aMediaType  IN     Renderer's media type.
        * @param aMedia      IN     SMIL media object.
        * @param aDrmCommon  IN     DRM Common
        * @param aDrmHelper  IN     DRM Helper
        *
        * @since 2.0
        */
        CSmilMediaRendererBase( TMsgMediaType aMediaType,
                                MSmilMedia* aMedia, 
                                DRMCommon& aDrmCommon, 
                                CDRMHelper& aDrmHelper );
        
        /**
        * Symbian 2nd phase base class constructor.
        *
        * @param aFileHandle IN  Source file handle.
        *
        * @return void
        */
        void BaseConstructL( RFile& aFileHandle );

        /**
        * Common focus drawing function for all renderers.
        *
        * @param aGc    IN  Reference to graphics context
        * @param aFocus IN  Pointer to focus object
        *
        * @return void
        */
        void DrawFocus( CGraphicsContext& aGc,
                        const MSmilFocus* aFocus ) const;

        /**
        * Common scroll bar drawing function for all renderers
        * Draws scroll bar(s) only if necessary.
        *
        * @param aGc         IN  Reference to graphics context
        * @param aRegionRect IN  Rect of the region
        * @param aMedaRect   IN  Rect of the full media bitmap
        *
        * @return void
        */
        void DrawScrollBars( CGraphicsContext& aGc,
                             const TRect& aRegionRect,
                             const TRect& aMediaRect ) const;
        
        /**
        * Starts CActiveSchedulerWait
        *
        * @return void
        */
        void BeginActiveWait();
        
        /**
        * Stops CActiveSchedulerWait
        *
        * @return void
        */
        void EndActiveWait();
        
        /**
        * Returns correct access intent for the specified media type.
        *
        * @return ContentAccess::EPlay,
        *         ContentAccess::EView, 
        *         ContentAccess::EExecute, 
        *         ContentAccess::EPrint or
        *         ContentAccess::EInstall 
        */
        ContentAccess::TIntent Intent() const;
        
        /**
        * Consumes DRM rights from media object if it is DRM protected.
        *
        * @return EOk if rights were consumed ok. Otherwise error value.
        *         For error values see DRMCommon.h.
        */
        TInt ConsumeDRMRightsL();
        
        /**
        * Checks DRM rights from media object if it is DRM protected.
        *
        * @return EOk if rights were checked ok. Otherwise error value.
        *         For error values see DRMCommon.h.
        */
        TInt CheckDRMRights();
        
        /**
        * Retrieves correct icon size from LAF. 
        */
        TSize RetrieveIconSize() const;
        
        /**
        * Returns if sending DRM file is allowed
        *
        * @since 3.0
        *
        * @param aFileHandle     IN  Handle to file that preview is wanted.
        * 
        * @return ETrue if sending is allowed. Otherwise EFalse.
        */
        TBool SendingDrmFileAllowedL( RFile& aFileHandle );

    protected: //data
        
        /** Wait loop for active wait. */
        CActiveSchedulerWait iWait;

        /** Renderer type. */
        TMsgMediaType iMediaType;

        /** DRM common. */
        DRMCommon& iDrmCommon;
        
        /** DRM Helper. */
        CDRMHelper& iDrmHelper;
        
        /** DRM file URI. */
        HBufC8* iDrmUri;
        
        /** Specifies if DRM right has been consumed. */
        TBool iDrmRightConsumed;
        
        /** Specifies what drm protectoin is used on media object. */
        DRMCommon::TContentProtection iDrmProtection;
        
        // Event array to store key events.
        CArrayFixFlat<TKeyEvent>* iEventArray;
        
        // SMIL media object.
        MSmilMedia* iMedia;
        
        // Focus rectangle.
        TRect iFocusRect;
    };

#endif // CSMILMEDIARENDERERBASE_H
