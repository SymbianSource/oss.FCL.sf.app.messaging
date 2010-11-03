/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SmilPlayerPresentationController  declaration
*
*/


#ifndef SMILPLAYERPRESENTATIONCONTROLLER_H
#define SMILPLAYERPRESENTATIONCONTROLLER_H

// INCLUDES
#include <coecntrl.h>

#include <smilplayerinterface.h>
#include <remconcoreapi.h> // TRemConCoreApiOperationId
#include "SmilPlayer.hrh"
#include <AknUtils.h>

#include <MsgMedia.hrh>


// FORWARD DECLARATIONS
class MMediaFactoryFileInfo;
class CSmilPlayerMediaFactory;
class CSmilTransitionFactory;
class CSmilPlayerDialog;
class CAknWaitDialog;
class CMDXMLDocument;
class CMDXMLNode;
class CMDXMLElement;
class CSmilPlayerPresentationControl;
class CAknNavigationDecorator;
class CSmilPlayerIndicatorController;
class CSendUi;

class CAiwServiceHandler;

// CLASS DECLARATION

/**
* Controller class of the presentation.
*/
NONSHARABLE_CLASS(CSmilPlayerPresentationController) : public CCoeControl,
                                                       public MSmilPlayer
    {
    public: // Constructors and destructor

        /**
        * Static constructor.
        *
        * @param aRect            Control rectangle.
        * @param aBaseUrl         Base URL used to resolve relative URLs.
        * @param aFileInfo        Handle to the media objects.
        * @param aParent          Dialog class of this controller
        * @param aDOM             SMIL Document Object Model
        * @param aVolumeEnabled   Specified whether audio is enabled.
        *
        * @return pointer to the object
        */
        static CSmilPlayerPresentationController* NewL( const TRect& aRect,
                                                        const TDesC& aBaseUrl,
                                                        MMediaFactoryFileInfo* aFileInfo,
                                                        CSmilPlayerDialog* aParent,
                                                        CMDXMLDocument* aDOM,
                                                        TBool aVolumeEnabled );
        /**
        * Constructor.
        *
        * @param aRect            Control rectangle.
        * @param aBaseUrl         Base URL used to resolve relative URLs.
        * @param aFileInfo        Handle to the media objects.
        * @param aParent          Dialog class of this controller
        * @param aFileHandle      File handle to .smil file
        * @param aVolumeEnabled   Specified whether audio is enabled.
        *
        * @return pointer to the object
        */
        static CSmilPlayerPresentationController* NewL( const TRect& aRect,
                                                        const TDesC& aBaseUrl,
                                                        MMediaFactoryFileInfo* aFileInfo,
                                                        CSmilPlayerDialog* aParent,
                                                        RFile& aFileHandle,
                                                        TBool aVolumeEnabled );

        /**
        * Destructor.
        */
        virtual ~CSmilPlayerPresentationController();

    public: // New functions
        
        /**
        * Returns buffer bitmap
        *
        * @since 2.0
        *
        * @return Pointer to buffer bitmap.
        */
        inline CFbsBitmap* BufferBitmap();

        /**
        * Returns buffer bitmap context
        *
        * @since 2.0
        *
        * @return Pointer to buffer bitmap context..
        */
        inline CFbsBitGc* BufferContext();

        /**
        * Prepares presentation controller to dialog close.
        *
        * @since 2.0
        */
        void PrepareExitL();

        /**
        * Controller states
        */
        enum TSmilPresControllerStates
            {
            EStatePlay,
            EStateStop,
            EStatePause,
            EStateResume
            };
            
        /**
        * Changes presentation state. 
        *
        * @since 3.0
        *
        * @param aState New state.
        */
        void ChangeStateL( TSmilPresControllerStates aState );

        /**
        * Stops timers for pause and volume indicator.
        *
        * @since 2.0
        */
        void StopTimer();

        /**
        * Invalidates the redraw area
        *
        * @since 2.0
        *
        * @param aArea  Value of the invalidated area
        */
        void RedrawArea( const TRect& aArea ) const;
        
        /**
        * Returns the Presentation object of the player.
        *
        * @since 2.0
        *
        * @return CSmilPresentation
        */
        inline CSmilPresentation* Presentation();

        /**
        * Set the visibility of the status pane
        *
        * @since 2.0
        *
        * @param aValue Boolean value whether to make normal statuspane visible or not.
        */
        void SetStatusPaneVisibilityL( TBool aValue );

        /**
        * Returns whether achor exists.
        *
        * @since 2.0
        * 
        * @return ETrue if anchor exists otherwise EFalse is returned.
        */
        TBool AnchorExist() const;

        /**
        * Returns whether achor is to internal link.
        *
        * @since 2.0
        *
        * @return ETrue if anchor is to internal link otherwise EFalse is returned.
        */
        TBool AnchorIsInternalLink() const;

        /**
        * Returns whether focused object scrolling activation is enabled.
        *
        * @since 2.0
        * 
        * @return ETrue if scrolling activation is enabled otherwise EFalse is returned.
        */
        TBool ScrollingActivationEnabled() const;

        /**
        * Returns whether focused object is scrollable
        *
        * @since 2.0
        * 
        * @return ETrue if focused object is scrollable otherwise EFalse is returned.
        */
        TBool FocusedObjectScrollable() const;

        /**
        * Returns whether focused object scrolling deactivation is enabled.
        *
        * @since 2.0
        *
        * @return ETrue if scrolling deactivation is enabled otherwise EFalse is returned.
        */
        inline TBool ScrollingDeactivationEnabled() const;

        /**
        * Activates the focused object for scrolling
        *
        * @since 2.0
        */
        void ActivateScrollingL();

        /**
        * Deactivates the focused object from scrolling
        *
        * @since 2.0
        */
        void DeactivateScrollingL();

        /**
        * Activates the link defined in the focused object
        *
        * @since 2.0
        */
        inline void OpenLinkL();

        /**
        * Returns whether some object is focused.
        *
        * @since 2.0
        *
        * @return ETrue if object is focused otherwise EFalse is returned.
        */
        inline TBool ObjectFocused() const;

        /**
        * Switches status pane to the given one.
        *
        * @since 3.0
        *
        * @param aNewPaneResourceId New statuspane's resource ID
        * @param aSetOwnIndicators  Specifies whether own indicators should be used
        * @param aOldPaneResourceId Old statuspane resource ID for storing.
        *
        */
        void SwitchStatusPaneL( TInt aNewPaneResourceId,
                                TBool aSetOwnIndicators, 
                                TInt& aOldPaneResourceId );

        /**
        * Called just before activating
        *
        * @since 2.0
        *
        * @return ETrue if presentationcontroller is initialized ok otherwise EFalse.
        */
        TBool InitializeL();
        
        /**
        * Sets correct layout for the presentation controller.
        *
        * @since 3.0
        *
        */
        void LayoutPresentationL();
        
        /**
        * Indicates whether the presentation is overlapped by another
        * dialog (e.g. FindUi).
        * 
        * @return ETrue if overlapped, otherwise EFalse.
        */   
        TBool IsPresentationOverlapped();

        /**
        * Sets flag to indicate that the presentation is overlapped.
        * 
        * @param aOverlapped ETrue / EFalse for setting the flag likewise.
        */   
        void SetPresentationOverlapped( TBool aOverlapped );
        
        /**
        * Sets the correct presentation layout, when a dialog (e.g. FindUi)
        * is overlapping the presentation.
        */ 
        void LayoutOverlappedPresentation();
        
        /**
        * Sends an activation event to focused media.
        *
        * @since 3.0
        */
        void ActivateFocusedMediaL();
        
        /**
        * Handles Remote Control Server Operations.
        *
        * @since 3.0
        *
        * @param aOperation An operation ID to handle.
        */
        void HandleRCSOperation( TRemConCoreApiOperationId aOperation );
        
        /**
        * Handles scroll key event.
        *
        * @since 3.0
        *
        */
        TKeyResponse HandleScrollKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Handles volume key event.
        *
        * @since 3.0
        *
        */
        TKeyResponse HandleVolumeKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
        * Returns all presentation's text on single buffer. Individual texts are separated
        * with two paragraph separators from each others.
        *
        * @since 3.0
        *
        * @return Text buffer. Buffer may be NULL and it must be otherwise freed by client.
        */
        HBufC* PresentationTextL();
        
        /**
        * Returns whether presentation have asked media type.
        *
        * @since 3.0
        
        * @param aType Enumeration of TSmilMediaType
        *
        * @return ETrue if presentation have asked media type otherwise EFalse.
        */
        TBool PresentationContains( const TMsgMediaType& aType ) const;
        
        /**
        * Handles the pointer-events. One tap activates area and
        *  second tap enables scrolling
        */                
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
        /**
        * Controls the taps on the corresponding area, 
        *   either playing or pausing the presentation after the tap
        */
        void PlayPauseTappedL();
        
        /**
        * Returns whether asked media type if focused.
        * 
        * @since 3.2
        * 
        * @param aMediaType Enumeration of TSmilMediaType
        *
        * @return ETrue if asked media type is focused otherwise EFalse.
        *
        */
        TBool IsFocused( const TMsgMediaType& aMediaType ) const;

        /**
        * Activates focused media object.
        */
        void ActivateFocusedL();
        
        /**
         * Returns SMIL players own statuspane current visibility.
         * 
         * @return ETrue if own statuspane is visible and EFalse if it is not.
         */
        TBool OwnStatusPaneVisible();
        
        /**
         * Returns whether wide screen statuspane should be used.
         * 
         * @return ETrue/EFalse Use/Don't use
         */
         static TBool UseWidescreenStatusPane();

        
    public: // Functions from base classes

        /** 
        * From MSmilPlayer
        * Returns the media factory object that is responsible of 
        * creating the media renderers
        *
        * @since 2.0
        *
        */
        inline virtual MSmilMediaFactory* GetMediaFactory(
                        const CSmilPresentation* aPres ) const;        

        /**
        * From MSmilPlayer  
        * Returns the transition factory object that is responsible of 
        * creating the transition renderers
        *
        * @since 2.0
        *
        */
        inline virtual MSmilTransitionFactory* GetTransitionFactory( 
                        const CSmilPresentation* aPres ) const;
        
        /** 
        * From MSmilPlayer
        * Evaluate a content control attribute for true or false. 
        * Attributes 
        *       systemOperatingSystem
        *       systemCPU
        *       systemScreenSize 
        *       systemScreenDepth 
        * are handled inside the SMIL engine, the rest are passed to this method.
        *
        * @since 2.0
        *
        */
        inline virtual TBool EvaluateContentControlAttribute(
                        const TDesC& aName, const TDesC& aValue );

        /** 
        * From MSmilPlayer
        * Return the dimensions of the drawing surface
        *
        * @since 2.0
        *
        */
        inline virtual TRect GetDimensions(const CSmilPresentation* aPres );

        /**
        * From MSmilPlayer  
        * Return the EIKON GUI control that is used as the view for the given 
        * presentation.
        *
        * SMIL Engine never calls this method. It might be used by 
        * implementations of MSmilMediaRenderer interface.
        *
        * @since 2.0
        *
        */
        inline virtual CCoeControl* GetControl(const CSmilPresentation* aPres );

        /** 
        * From MSmilPlayer
        * The engine calls this method to initiate redrawing of the given area. 
        * Implementation of this method should in turn call Draw method of the 
        * given presentation object.
        *
        * @since 2.0
        *
        */
        inline virtual void Redraw(TRect aArea, CSmilPresentation* aPres );

        /** 
        * This method is called by the engine as a result of hyperlink
        * activation. Player should open the given (relative to the
        * presentation base) url. However, the current presentation MUST NOT
        * be deleted during execution of this method, as this method is
        * invoked by the current presentation,
        * 
        * aAnchor - contains addition information about the link. can be 0.
        *
        * source/destination states: 
        * EPlay - (continue) play the presentation
        * EStop - stop the source presentation or open destination presentation
        *   in stopped state
        * EPause - pause the source presentation, continue it after,
        *   destination finishes
        *
        * 3GPP profile does not require respecting these states,
        *   aAnchor may be ignored
        *
        * @since 2.0
        *
        */
        virtual void OpenDocumentL(
                        const TDesC& aUrl,
                        const CSmilAnchor* aAnchor,
                        const CSmilPresentation* aPres );       

        /** 
        * Invoked when a presentation state changes. aEvent is 
        * EStarted - presentation started playing,
        * EStopped - presentation was stopped,
        * EPaused - presentation was paused
        * EResumed - paused presentation was resumed
        * ESeeked - presentation was seeked to a new position on timeline
        * EMediaNotFound - opening media failed, aText contains the media url
        * or
        * EEndReached - presentation has reached its end time. whether it is
        * paused or stopped depends on EndAction setting.
        * one of the negative system wide error codes (from a trapped leave
        * inside engine RunL() method). 
        * In this case the playing presentation is stopped automatically and
        * the presentation can not be restarted.
        *
        * @since 2.0
        *
        */
        virtual void PresentationEvent(
                        TInt aEvent,
                        const CSmilPresentation* aPres,
                        const TDesC& aText );

    private:

        /**
        * C++ constructor.
        */
        CSmilPlayerPresentationController( CMDXMLDocument* aDOM,
                                           MMediaFactoryFileInfo* aFileInfo,
                                           CSmilPlayerDialog* aParent,
                                           RFile& aFileHandle );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TRect& aRect, const TDesC& aBaseUrl, TBool aVolumeEnabled );

    private: // New functions

        /**
        * Parses the presentation file
        */
        void DoParsingL();

        /**
        * Calls to references phone number
        */
        void CallL( const TDesC& aString );

        /**
        * Sends message (SMS, MMS or e-mail) to selected item
        */
        void SendMessageL( const TUid& aMtmUid, const TDesC& aString );

        /**
        * Goes to referenced URL
        */
        void GoToUrlL( const TDesC& aString );

        /**
        * Load text string and shows a confirm dialog
        */
        TBool LoadTextAndConfirmL( TInt aResourceId, const TDesC& aString ) const;

        /**
        * Shows confirm query to the user
        */
        TBool ShowConfirmQueryL( TInt aResource, const TDesC& aText ) const;
        
        /**
        * Resets the control state.
        */
        void Reset();
        
        /**
        * Sets correct layout for indicators.
        */  
        void LayoutIndicators();
        
        /**
        * Sets correct layout for presentation control.
        */
        void LayoutPresentationControl();
        
        /**
        * Performs volume change key handling.
        */ 
        TKeyResponse DoHandleVolumeKeyL( const TKeyEvent& aKeyEvent, TEventCode aType );
        
        /**
        * Performs Remote Control Operation handling.
        */
        void DoHandleRCSOperationL( TRemConCoreApiOperationId aOperation );
        
        /**
        * Initializes double buffer.
        */
        void InitializeDoubleBufferL();
        
    private: // Functions from base classes

        /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl,Draw.
        */
        void Draw( const TRect& aRect ) const;

    private: // data
            
        // iDOM
        CMDXMLDocument* iDOM;

        // A Font and Bitmap server, which managed bitmap as a graphics device.
        CFbsBitmapDevice*               iBufferDevice;

        // Implementation of a bitmapped graphics context.
        CFbsBitGc*                      iBufferContext;

        // Presentation class to the engine
        CSmilPresentation*              iPresentation;

        // Factory class for the media
        CSmilPlayerMediaFactory*        iMediaFactory;

        // Factory class for the transitions
        CSmilTransitionFactory*         iTransitionFactory;

        // Store class for the medias
        MMediaFactoryFileInfo*          iFileInfo;

        // Dialog class for this class
        CSmilPlayerDialog*              iParent;

        // A bitmap managed by the Font and Bitmap server.
        // Provided to the CSmilTransitionFactory class.
        CFbsBitmap*                     iBufferBitmap;

        // Wait dialog.
        CAknWaitDialog* iWaitDialog;
        
        // SendUi 
        CSendUi* iSendUi;

        CAiwServiceHandler* iServiceHandler;

        //
        TBool iScrollingTogglePerformedWhileLinkExist;

        //
        TInt iLayoutResId;

        // Presentation control of the player
        CSmilPlayerPresentationControl* iPresentationControl;
        
        // Controller class for this class
        CAknNavigationDecorator* iIndicatorContainer;
        
        // Indicator controller.
        CSmilPlayerIndicatorController* iIndicatorController;
        
        //
        TBool iDOMInternallyOwned;
        
        //
        TRect iPresentationRect;
        
        // .smil file handle (can be KNullHandle).
        RFile iFileHandle;
        
        // Base URL that is used for resolving relative URLs.
        HBufC* iBaseUrl;
        
        // Flag to indicate that layout change is needed.
        TBool iChangeLayout;
        
        // Last selected object
        CSmilObject* iTappedObject;
        
        // Current visibility of own statuspane.
        TBool iOwnStatusPaneVisible;    
        
        // Flag to indicate if the presentation is overlapped by another dialog (e.g. FindUi)
        TBool iPresentationOverlapped;
    };

#include "SmilPlayerPresentationController.inl"

#endif // SMILPLAYERPRESENTATIONCONTROLLER_H

// End of File
