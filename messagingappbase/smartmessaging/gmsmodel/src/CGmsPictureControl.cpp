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
*      GMS picture viewer control which is compatible with Editor Base.
*      It is based very much on the CMmsImageControl.
*  %version: bh1cfmsg#17 %
*
*/



// INCLUDE FILES
#include "CGmsPictureControl.h"

#include <coeutils.h>               // for ConeUtils::FileExists
#include <AknUtils.h>
#include <imageconversion.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <MIHLScaler.h>

// CONSTANTS

// Mime type of the image, required by the decoder.
_LIT8( KGmsOtaMimeType, "image/x-ota-bitmap");

_LIT(KSmEditorGmsPic,"smeditor gmsPic");

// ==========================================================

/**
* Helper class which allows CGmsPictureControl to receive a callback
* to it's callback method when an asynchronous operation has finished.
* CAsyncCallBack needed tweaking since it is ment for callback only, and
* it completes the task immediately.
*/
class CGmsAOCallBack: public CAsyncCallBack
    {
    public:
    
        CGmsAOCallBack( const TCallBack &aCallBack, TInt aPriority )
            : CAsyncCallBack( aCallBack, aPriority )
            {
            }
        
        void SetItActive() 
            {
            SetActive();
            }
        
    protected:         
       
        void RunL()
            {
            iCallBack.CallBack();
            }
    };


//  MEMBER FUNCTIONS

EXPORT_C CGmsPictureControl* CGmsPictureControl::NewL(
    const CCoeControl* aParent)
{
    CGmsPictureControl* self = new(ELeave) CGmsPictureControl();

    CleanupStack::PushL(self);
    self->ConstructL(aParent);
    CleanupStack::Pop(self);

    return self;
}

EXPORT_C CGmsPictureControl::~CGmsPictureControl()
{
    delete iBitmap;
    iBitmap = NULL;
    delete iScaledBitmap;
    iScaledBitmap = NULL;    
        
    delete iConverter;
    iConverter = NULL;
    delete iScaler;
    delete iConversionAO;
    delete iScalingAO;    
    iFs.Close();
}

EXPORT_C void CGmsPictureControl::LoadImageL(
    const TFileName& aImageFile,
    TUint            aMaxHeight /* = 0 */)
{
    __ASSERT_DEBUG((aImageFile.Length() > 0), Panic( ENoImageFile ) );

    if (!ConeUtils::FileExists(aImageFile))
    {
        User::Leave(KErrNotFound);
    }
    if (aMaxHeight)
    {
        iMaxHeight = aMaxHeight;
    }
    
    Reset();
        
    iConverter = CImageDecoder::FileNewL( iFs, aImageFile, KGmsOtaMimeType );
    __ASSERT_ALWAYS( iConverter->FrameCount() == 1, Panic(EBadImageContent) );
    TFrameInfo frameInfo = iConverter->FrameInfo( 0 );
    iBitmap = new( ELeave ) CFbsBitmap;
    User::LeaveIfError( iBitmap->Create( frameInfo.iOverallSizeInPixels,
                                         EGray2 ) );
                                             
    iConverter->Convert( &(iConversionAO->iStatus), *iBitmap );
	iConversionAO->SetItActive();
}

EXPORT_C TSize CGmsPictureControl::OriginalImageResolution() const
{
    TSize size(0,0);
    if ( iConverter )
        {
        size = iConverter->FrameInfo( 0 ).iOverallSizeInPixels;
        }
    return size;
}

TRect CGmsPictureControl::CurrentLineRect()
{
    return Rect();
}

void CGmsPictureControl::SetAndGetSizeL(TSize& aSize)
{
    // Size proposed in aSize parameter is used, only height is
    // proposed by this control.
    
    if (AknLayoutUtils::ScalableLayoutInterfaceAvailable())
    {        
        TAknWindowComponentLayout layout =
            AknLayoutScalable_Apps::mce_image_pane_g3();
        TAknLayoutRect layoutRect;
    	layoutRect.LayoutRect( Rect(), layout.LayoutLine() );
    	aSize.iHeight = layoutRect.Rect().Height();
    	
        // Adjust max height if needed
        if ( iMaxHeight > 0 && (TUint)aSize.iHeight > iMaxHeight )
        {
            aSize.iHeight = iMaxHeight;
        }    	
    }
    
    else
    { // use old sizing functionality if scalable IF isn't available:
        aSize = CalculateControlSize(aSize);
    }

    SetSizeWithoutNotification(aSize);
}

/**
* This internal function is the old version and is used ONLY if scalable
* layout interface is NOT available. This should be removed when it is always
* available.
*/
const TInt KGmsImageHeightMax = 58;
const TInt KBaseLineDelta = 17;

TSize CGmsPictureControl::CalculateControlSize(TSize& aProposedSize)
    {
    // These lines below, although they look a bit strange, are required to
    // conform with old sizing functionality when scalable layout IF is
    // not available:
    if ( iMaxHeight == 0 )
        { // max height hasn't been set by the client:
        iMaxHeight = KGmsImageHeightMax;
        }
    if ( (TUint)aProposedSize.iHeight > iMaxHeight )
        { // proposed size is greater than max size -> adjust:
        aProposedSize.iHeight = iMaxHeight;
        }    
    
    TSize trgSize(aProposedSize.iWidth, 0);
    
    if (!iConverter)
        {
        return trgSize;
        }
    
    TSize bmpSize = OriginalImageResolution();
    
    bmpSize.SetSize(bmpSize.iWidth*2, bmpSize.iHeight*2);
    
    if (bmpSize.iHeight < aProposedSize.iHeight)
        {
        trgSize.iHeight = bmpSize.iHeight;
        }
    else
        {
        trgSize.iHeight = aProposedSize.iHeight;
        }
    
    // For "wide" images height may be too big - check aspect ratio
    TReal widthRatio  = (TReal)bmpSize.iWidth  / (TReal)trgSize.iWidth;
    TReal heightRatio = (TReal)bmpSize.iHeight / (TReal)trgSize.iHeight;
    
    if (widthRatio > heightRatio)
        {
        // image must be scaled more in vertical size
        trgSize.iHeight = (TInt)(bmpSize.iHeight / widthRatio);
        }
    
    if (trgSize.iHeight%KBaseLineDelta)
        {
        trgSize.iHeight = trgSize.iHeight-trgSize.iHeight%KBaseLineDelta+KBaseLineDelta;
        }    
    return trgSize;
    }

void CGmsPictureControl::ClipboardL(TMsgClipboardFunc /*aFunc*/)
{
    User::Leave(KErrNotSupported);  // Image control does not support clipboard
}

void CGmsPictureControl::EditL(TMsgEditFunc /*aFunc*/)
{
    User::Leave(KErrNotSupported);  // Image control is not editable
}

TBool CGmsPictureControl::IsFocusChangePossible(TMsgFocusDirection /*aDirection*/) const
{
    return ETrue;
}

TBool CGmsPictureControl::IsCursorLocation(TMsgCursorLocation /*aLocation*/) const
{
    return ETrue;
}

TUint32 CGmsPictureControl::EditPermission() const
{
    return EMsgEditNone;
}

void CGmsPictureControl::Reset()
{
    delete iBitmap;
    iBitmap = NULL;
    delete iScaledBitmap;
    iScaledBitmap = NULL;    
        
    delete iConverter;
    iConverter = NULL;
            
    SetModified( ETrue );    
}

void CGmsPictureControl::NotifyViewEvent(TMsgViewEvent /*aEvent*/,
    TInt /*aParam*/)
{
}

TInt CGmsPictureControl::ConversionReady( TAny* aPtr )
{
	__ASSERT_ALWAYS(aPtr, User::Panic(KSmEditorGmsPic, ENullPtr) );
    static_cast<CGmsPictureControl*>( aPtr )->DoConversionReady();
    return 0;
}    

TInt CGmsPictureControl::ScalingReady( TAny* aPtr )
{
	__ASSERT_ALWAYS(aPtr, User::Panic(KSmEditorGmsPic, ENullPtr) );
    static_cast<CGmsPictureControl*>( aPtr )->DoScalingReady();
    return 0;
}    

void CGmsPictureControl::PrepareForReadOnly(TBool /*aReadOnly*/)
{
}

TInt CGmsPictureControl::CountComponentControls() const
    {
    // This method is required by the base class
    return 0;
    }

CCoeControl* CGmsPictureControl::ComponentControl(TInt /*aIndex*/) const
    {
    // This method is required by the base class
    return NULL;
    }
    
void CGmsPictureControl::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    // This method is required by the base class
    }

void CGmsPictureControl::SizeChanged()
{
    iBitmapRect = Rect();
    
    if (AknLayoutUtils::ScalableLayoutInterfaceAvailable())
    {        
        TAknWindowComponentLayout layout =
            AknLayoutScalable_Apps::mce_image_pane_g3();
        TAknLayoutRect layoutRect;
    	layoutRect.LayoutRect( Rect(), layout.LayoutLine() );
    	iBitmapRect = layoutRect.Rect();
    
        // Adjust max height if needed, this is not the usual case but
        // implemented here just because of supporting the existing
        // public LoadImageL() method. Usually the size of imageRect
        // should be fully determined by avkon layout!
        if ( iMaxHeight > 0 && (TUint)iBitmapRect.Size().iHeight > iMaxHeight )
        {
            iBitmapRect.SetHeight(iMaxHeight);
        }
        TInt yAxisOffset = Rect().iTl.iY - iBitmapRect.iTl.iY;
        iBitmapRect.Move(0,yAxisOffset);
        
        // If leave occurs then scaling is not performed
        TRAP_IGNORE( ScaleIfPossibleL( iBitmapRect.Size() ) );
    }
    
    else
    { // Use old sizing functionality if scalable IF isn't available:
      // Note that usage of image size and image rect is not identical
      // to scalable version!
        TSize imageSize(CalculateImageSize());
        
        // If leave occurs then scaling is not performed
        TRAP_IGNORE( ScaleIfPossibleL( imageSize ) );                
    }
}

void CGmsPictureControl::Draw(const TRect& /*aRect*/) const
{   
    // Ensure that scaled bitmap exists and that no scaling is
    // currently ongoing.    
    if ( iScaledBitmap &&
         iScaledBitmap->Handle() &&
         ! iScalingAO->IsActive() )
    {            
        CWindowGc& gc = SystemGc();
		gc.DrawBitmap(iBitmapRect,iScaledBitmap);
    }
}

/**
* This internal function is the old version and is used ONLY if scalable
* layout interface is NOT available. This should be removed when it is always
* available.
*/
TSize CGmsPictureControl::CalculateImageSize() const
{
    TSize trgSize(0,0);

    TSize bmpSize = OriginalImageResolution();
    bmpSize.SetSize(bmpSize.iWidth*2, bmpSize.iHeight*2);

    TRect rect(Rect());

    // Width is basically static during the session, but it's always
    // calculated anyway.
    if (bmpSize.iWidth < rect.Width())
    {
        // "narrow" image - request only what is needed.
        trgSize.iWidth = bmpSize.iWidth;
    }
    else
    {
        // "wide" image - request all we can get.
        trgSize.iWidth = rect.Width();
    }

    // Height is dynamic, i.e. changes when moving up&down in the message.
    // Request maximum height possible.
    // Note: Either max size allowed by spec or the size of the image itself
    // is considered here, i.e. not the given rect.
    if ( iMaxHeight > 0 && (TUint)bmpSize.iHeight > iMaxHeight )
    {
        trgSize.iHeight = iMaxHeight;
    }
    else
    {
        trgSize.iHeight = bmpSize.iHeight;
    }
    return trgSize;
}

TBool CGmsPictureControl::ScaleIfPossibleL( TSize aSize )
{
	if ( !iBitmap )    	     
    { 
        // Image hasn't been loaded, cannot scale
        return EFalse;
    }
	else if ( iConversionAO->IsActive() )
    { 
        // Conversion is ongoing, but not ready yet,
        // image will be scaled after conversion finishes.
        return EFalse;
    }
    
    else if( iScaledBitmap && 
             iScaledBitmap->SizeInPixels()==aSize )
    {
        return ETrue;
    }

    else if ( !iScalingAO->IsActive() )
    {
        // Scaling is possible, it is done asynchronously
        delete iScaledBitmap;
        iScaledBitmap = NULL;        
        iScaledBitmap = new( ELeave ) CFbsBitmap;
                
        User::LeaveIfError( iScaler->Scale( iScalingAO->iStatus, 
                                            *iBitmap, 
                                            TRect(iBitmap->SizeInPixels()),
                                            *iScaledBitmap,
                                            TRect(aSize) ) );
        iScalingAO->SetItActive();            
        return ETrue;
    }

    return EFalse;            
}

void CGmsPictureControl::DoConversionReady()
{    
    __ASSERT_DEBUG( iConversionAO->iStatus == KErrNone,
                    Panic(EConversionError) );
                    
    if ( iConversionAO->iStatus == KErrNone )
    	{        
        SetModified( ETrue );
        // We want to scale the image now:       
        SetRect( Rect() );
        
    	//we must delete the converter here because it is holding the
    	//converted imagefile so that it cant be replaced when FORWARD:ing
    	delete iConverter;
    	iConverter = NULL;
    	}
}

void CGmsPictureControl::DoScalingReady()
{    
    __ASSERT_DEBUG( iScalingAO->iStatus == KErrNone,
                    Panic(EScalingError) );
                    
    if ( iScalingAO->iStatus == KErrNone )
    {        
        SetModified( ETrue );
        // We want to draw the image now:       
        DrawNow();
    }
}


void CGmsPictureControl::Panic(TGmsPicControlPanic aCode)
    {
    User::Panic(KSmEditorGmsPic, aCode);
    }

void CGmsPictureControl::ConstructL(const CCoeControl* aParent)
{
    __ASSERT_DEBUG( aParent, Panic(ENoParent) );
    
    User::LeaveIfError( iFs.Connect() );

    iControlType = EMsgImageControl;    

    SetContainerWindowL(*aParent);
    
    TRect rect(aParent->Rect());
    if (rect.Height() == 0 || rect.Width() == 0)
    {
        // We have been called before view construction.
        // Set rect to screen size.
        rect.SetRect (
            TPoint(0,0),
            iEikonEnv->EikAppUi()->ClientRect().Size());
    }
    SetPosition(rect.iTl);
    SetSizeWithoutNotification(rect.Size());
    
    iConversionAO = new( ELeave ) CGmsAOCallBack(
        TCallBack( ConversionReady, this ), CActive::EPriorityStandard );    
    iScaler = IHLScaler::CreateL();
    iScalingAO = new( ELeave ) CGmsAOCallBack(
        TCallBack( ScalingReady, this ), CActive::EPriorityStandard );    
}

CGmsPictureControl::CGmsPictureControl()
{
    // By default iMaxHeight is not used:
    iMaxHeight = 0;
}

//  End of File
