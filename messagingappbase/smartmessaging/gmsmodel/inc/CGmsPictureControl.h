/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      GMS picture viewer control - compatible with Editor Base.
*      It is based very much on the CMmsImageControl.
*
*/



#ifndef __GMSPICTURECONTROL_H_
#define __GMSPICTURECONTROL_H_

// INCLUDES

#include <MsgBaseControl.h>         // for CMsgBaseControl

// DATA TYPES

enum TGmsPicControlPanic
    {
    ENoParent,
    ENoImageFile,
    EBadImageContent,
    EConversionError,
    EScalingError,
    ENullPtr
    };

// FORWARD DECLARATIONS

class CImageDecoder;
class CFbsBitmap;
class CGmsAOCallBack;
class MIHLScaler;

// CLASS DECLARATION

/**
 * Message Editor Base compatible control for handling GMS pictures.
 */
class CGmsPictureControl : public CMsgBaseControl
{

public:  // Constructor and destructor

    /**
     * Two-phased constructor.
     * @param aParent parent control
     * @return new object
     */
    IMPORT_C static CGmsPictureControl* NewL(const CCoeControl* aParent);

    /**
     * Destructor.
     */
    IMPORT_C ~CGmsPictureControl();

public: // new functions

    /**
     * Loads the image into display.
     * After calling LoadImageL and adding image control to view using
     * iView->AddControlL call you must call imagecontrol->SizeChanged()
     * to make image appear in view.
     * @param aImageFile image file name - expected to be valid image
     * @param aMaxHeight maximum height of the displayed image - default is
     * 0 = {max display size or max thumbnail size}.
     */
    IMPORT_C void LoadImageL(
        const TFileName& aImageFile, TUint aMaxHeight = 0);

    /**
     * Gives the resolution of the original image read from a file.
     * @return Original image resolution
     */
    IMPORT_C TSize OriginalImageResolution() const;

public: // from CMsgBaseControl

    /**
     * Returns the current control rect.
     * @return Current control rect.
     */
    TRect CurrentLineRect();

    /**
     * Calculates and sets the size of the control and returns new size as
     * reference aSize.
     * @param aSize Reference to TSize which gives and gets the size.
     */
    void SetAndGetSizeL(TSize& aSize);

    /**
     * Handles clipboard operation.
     * @param aFunc The clipboard function.
     */
    void ClipboardL(TMsgClipboardFunc aFunc);

    /**
     * Handles editing operation.
     * @param aFunc The edit operation.
     */
    void EditL(TMsgEditFunc aFunc);

    /**
     * Checks if the focus change up or down is possible and
     * returns ETrue if it is.
     * @param aDirection The direction of focus change.
     * @return ETrue if focus change is possible.
     */
    TBool IsFocusChangePossible(TMsgFocusDirection aDirection) const;

    /**
     * Checks if the cursor location is on the topmost or downmost position
     * and returns ETrue if it is.
     * @param aLocation Topmost or bottom.
     * @return ETrue if cursor is in the specified part.
     */
    TBool IsCursorLocation(TMsgCursorLocation aLocation) const;

    /**
     * Returns edit permission flags.
     * @return Edit permission flags.
     */
    TUint32 EditPermission() const;

    /**
     * Resets (= clears) contents of the control.
     */
    void Reset();

    /**
     * Prepares control for viewing.
     * @param aEvent Event
     * @param aParam Parameter
     */
    void NotifyViewEvent(TMsgViewEvent aEvent, TInt aParam);

public: // TCallBack

   /**
    * TCallBack function for image conversion.
    * @param aPtr this class instance
    * @return always zero
    */
    static TInt ConversionReady( TAny* aPtr );

    static TInt ScalingReady( TAny* aPtr );

protected: // from CMsgBaseControl

    /**
     * Prepares for read only or non read only state.
     * @param aReadOnly Whether to prepare for read only or non-read only.
     */
    void PrepareForReadOnly(TBool aReadOnly);

protected: // from CCoeControl

    /**
     * CMsgBaseControl requires that this method is implemented.
     */
    TInt CountComponentControls() const;

    /**
     * CMsgBaseControl requires that this method is implemented.
     */
    CCoeControl* ComponentControl(TInt /*aIndex*/) const;

    /**
     * CMsgBaseControl requires that this method is implemented.
     */
    void FocusChanged( TDrawNow /*aDrawNow*/ );

    /**
     * Called when size is changed.
     */
    void SizeChanged();

    /**
     * Handles bitmap drawing.
     */
    void Draw(const TRect& aRect) const;

private: // New functions

   /**
    * Helper method which creates or rescales iScaledBitmap unless
    * image hasn't been loaded or conversion is currently ongoing.
    * @param aSize size for which to scale
    * @return ETrue if scaling was possible
    */
    TBool ScaleIfPossibleL( TSize aSize );

   /**
    * Callback method called when image conversion is ready.
    */
    void DoConversionReady();

   /**
    * Callback method called when image scaling is ready.
    */
    void DoScalingReady();

   /**
    * Panic wrapper.
    * @param aCode panic code
    */
    void Panic(TGmsPicControlPanic aCode);

    /**
     * Calculates the size for the image control.
     * NOTE: This method is old and used only if scalable layout IF is not available.
     * @param aProposedSize size given by the framework. Our size is either this or smaller.
     * @return new size for the control
     */
    TSize CalculateControlSize(TSize& aProposedSize);

    /**
     * Calculates the size for the image itself according to control rect.
     * NOTE: This method is old and used only if scalable layout IF is not available.
     * @return image size
     */
    TSize CalculateImageSize() const;

private: // Constructors

    /**
     * Second phase constructor.
     * @param aParent The parent.
     */
    void ConstructL(const CCoeControl* aParent);

   /**
    * Constructor
    */
    CGmsPictureControl();

private: //Data

    /// File server session
    RFs iFs;

    /// Maximum height.
    TUint iMaxHeight;

    /// Own: Image converter
    CImageDecoder* iConverter;

    /// Own: Active object which calls callback method when
    /// conversion has finished
    CGmsAOCallBack* iConversionAO;

    /// Own: Active object which calls callback method when
    /// scaling has finished
    CGmsAOCallBack* iScalingAO;

    /// Own: Bitmap, unscaled original
    CFbsBitmap* iBitmap;

    /// Own: Bitmap, scaled for current layout size
    CFbsBitmap* iScaledBitmap;

    /// Own: Bitmap scaler
    MIHLScaler* iScaler;

    /// Own: The rect of the bitmap
    TRect iBitmapRect;
};

#endif // __GMSPICTURECONTROL_H_

// end of file
