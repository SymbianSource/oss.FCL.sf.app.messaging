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
*       Message Editor Base Media control.
*
*/



#ifndef MSGMEDIACONTROL_H
#define MSGMEDIACONTROL_H

// INCLUDES
#include <AknsItemID.h>

#include <MsgBaseControl.h>         // for CMsgBaseControl
#include <MsgBaseControlObserver.h>
#include <msgasynccontrolobserver.h>
#include <msgasynccontrol.h>
#include <MsgEditor.hrh>            // NotifyViewEvent
#include <caf/caftypes.h>               // KMaxCafUniqueId

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CMsgFrameControl;
class CMsgIconControl;
class TMMSource;
class TMMFileHandleSource;
class TAknsItemID;
class CMsgMediaControl;
class CAknBitmapAnimation;

// DATA TYPES

// CLASS DECLARATION

// ==========================================================

/**
* Message Editor Base control handling images in MMS
*/
class CMsgMediaControl : public CMsgBaseControl,
                         public MCoeControlHitTest,
                         public MMsgAsyncControl
    {
    public:
    
        /**
        * Destructor.
        */
        virtual ~CMsgMediaControl();

        /**
        * Start playing media. Method does not start animation. 
        */         
        virtual void PlayL() = 0;

        /**
        * Stops playing media. 
        */         
        virtual void Stop() = 0;
        
        /**
        * Pauses playing media. 
        */         
        virtual void PauseL() = 0;
             
        /**
        * Return state
        */
        TMsgAsyncControlState State() const;

        /**
        * Return error code.
        */
        TInt Error() const;
        
        /**
        * Sets unique content ID used when loading/reloading media.
        * Default unique ID is used if not set.
        */
        inline void SetUniqueContentIdL( const TDesC& aUniqueId );
        
        /**
        * Returns current unique content ID.
        */
        inline const TDesC& UniqueContentId() const;
        
        /**
        *
        */
        void AddObserverL( MMsgAsyncControlObserver& aObserver );
        
        /**
        *
        */
        void RemoveObserver( MMsgAsyncControlObserver& aObserver );
        
        /**
        * Prepares control for viewing.
        * From CMsgBaseControl
        */
        void NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam );
        
        /**
        * Loads icon from specified file using given bitmap id and mask id. 
        *
        * @param aFileName      File to load icons from
        * @param aId            Item Id.
        * @param aFileBitmapId  ID of the icon bitmap. KErrNotFound accepted, in which
        *                       case nothing is loaded
        * @param aFileMaskId    ID of the icon mask. -1 (default) if no mask needed.
        *
        * @return ETrue:  icon has changed.
        *         EFalse: icon has not changed.
        */
        IMPORT_C TBool LoadIconL( const TDesC& aFileName,                                   
                                  const TAknsItemID& aId,
                                  const TInt aFileBitmapId,
                                  const TInt aFileMaskId = -1 );
        
        /**
        * Sets icon to specified size.
        *
        * @param aNewSize  New size.
        */
        IMPORT_C void SetIconSizeL( const TSize& aNewSize );
        
        /**
        * Sets icon visible/invisible.
        *
        * @param aVisibility ETrue sets icon visible
                             EFalse sets icon invisible
        */
        IMPORT_C void SetIconVisible( TBool aVisibility );
        
        /**
        * Sets animation control. Ownership transferred.
        *
        * @param aAnimationControl New animation control. Setting animation control
        *                          to NULL will remove animation.
        */
        IMPORT_C void SetAnimationL( CAknBitmapAnimation* aAnimationControl );
        
        /**
        * Sets animation to specified size.
        *
        * @param aNewSize  New size.
        */
        IMPORT_C void SetAnimationSizeL( const TSize& aNewSize );
        
        /**
        * Sets animation visible/invisible.
        *
        * @param aVisibility ETrue sets animation visible
                             EFalse sets animation invisible
        */
        IMPORT_C void SetAnimationVisibleL( TBool aVisibility );
        
        /**
        * Returns the current loaded icon's bitmap id.
        *
        * @return Current loaded icon's bitmap id. 
        *         If icon has not beed loaded returns KErrNotFound.
        */
        IMPORT_C TInt IconBitmapId() const;
        
        /**
        * Releases current animation
        */
        IMPORT_C void ReleaseAnimation();
        
    public: // from CMsgBaseControl
        
        /**
        * Returns the current control rect.
        * From CMsgBaseControl
        */
        TRect CurrentLineRect();
        
        /**
        * Handles clipboard operation.
        * From CMsgBaseControl
        */
        void ClipboardL( TMsgClipboardFunc aFunc );
        
        /**
        * Handles editing operation.
        * From CMsgBaseControl
        */
        void EditL( TMsgEditFunc aFunc );
        
        /**
        * Checks if the focus change up or down is possible and
        * returns ETrue if it is.
        * From CMsgBaseControl
        */
        TBool IsFocusChangePossible( TMsgFocusDirection aDirection ) const;
        
        /**
        * Checks if the cursor location is on the topmost or downmost position
        * and returns ETrue if it is.
        * From CMsgBaseControl
        */
        TBool IsCursorLocation( TMsgCursorLocation aLocation ) const;
        
        /**
        * Returns edit permission flags.
        * From CMsgBaseControl
        */
        TUint32 EditPermission() const;
    
    public: // From CCoeControl

        /**
        * Called when size is changed.
        * From CMsgBaseControl
        */
        void SizeChanged();
        
        /**
        * From CCoeControl,Draw.
        * @param aRect draw rect
        */
        void Draw( const TRect& aRect ) const;
        
        /**
        * Handles resource change events.
        * From CCoeControl
        */
        void HandleResourceChange( TInt aType );
        
         /**
        * From CCoeControl. See coecntrl.h
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
    public: // from MCoeControlHitTest
    
        /**
        * Evaluates whether control is "hit" by touch event.
        */
        TBool HitRegionContains( const TPoint& aPoint, const CCoeControl& aControl ) const;
            
    protected: 

        /**
        * C++ constructor.
        */
        CMsgMediaControl( MMsgBaseControlObserver& aBaseControlObserver,
                          TMsgControlId aMediaControlId,
                          TMsgControlType aMediaControlType );
            
        /**
        * Symbian constructor.
        */
        void BaseConstructL( const CCoeControl& aParent, MMsgAsyncControlObserver* aObserver );
        
        /**
        * Sets state and calls observer  
        */
        inline void SetState( TMsgAsyncControlState aState, 
                              TBool aDoNotCallObserver = EFalse );
        
        /**
        *
        */
        void ResetIconId();
        
        /**
        *
        */
        void DoUpdateFocus( TDrawNow aDrawNow );
        
        /**
        * Calculates correct extent for component controls.
        */
        void SetComponentControlExtents();
        
        /**
        * Set background frame for animation.
        */
        void SetAnimationBackGroundFrameL();
        
        /**
        * Tests whether animation is currently visible.
        *
        * @return ETrue     if animation is visible
        *         EFalse    otherwise.
        */
        TBool AnimationVisible() const;
        
        /**
        * Returns visible placeholder (i.e. icon or animation) control.
        *
        * @return NULL          if no placeholder is visible
        *         CCoeControl*  otherwise currently visible placeholder.
        */
        CCoeControl* VisiblePlaceholder() const;
                
    protected: // from CMsgBaseControl

        /**
        * Prepares for read only or non read only state.
        * @param aReadOnly
        */
        void PrepareForReadOnly( TBool aReadOnly );
    
    protected: // from CCoeControl
        
        /**
        * This is called when the focus of the control is changed.
        * @param aDrawNow
        */
        void FocusChanged( TDrawNow aDrawNow );
        
        /**
        * From CCoeControl Return count of controls be included in this component
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl Return pointer to component in question.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
    protected: //Data

        CArrayPtrFlat<MMsgAsyncControlObserver>* iObservers;
        
        TMsgAsyncControlState iState;
        
        TInt iError;
        TInt iBaseLine;
        
        CMsgIconControl* iIconControl;
        CMsgFrameControl* iFrame;
    
        TBuf<ContentAccess::KMaxCafUniqueId> iUniqueContentId;
        
        CAknBitmapAnimation* iAnimation;
    };


#include <msgmediacontrol.inl>

#endif // MSGMEDIACONTROL_H
