/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       CPostcardOperationInsertImage, asynchronous operation for inserting image
*
*/




#ifndef __POSTCARDOPERATIONINSERTIMAGE_H
#define __POSTCARDOPERATIONINSERTIMAGE_H

// INCLUDES

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>

#include <uniimageprocessor.h>

#include <MIHLViewerObserver.h>

#include "PostcardOperation.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CMsgMediaInfo;
class MIHLFileImage;
class MIHLBitmap;
class MIHLImageViewer;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CPostcardOperationInsertImage 
*
* @lib postcardeditor.exe
* @since 3.0
*/
class CPostcardOperationInsertImage : public CPostcardOperation,
                                      public MUniImageProcessorCallback,
                                      public MIHLViewerObserver
    {
    
    public:  // New methods

        /**
        * Factory method that creates this object.
        * @since    3.0
        * @param aInfo - info object of the inserted image -> operation takes the ownership
        * @return   Pointer to instance
        */
        static CPostcardOperationInsertImage* NewL(
            MPostcardOperationObserver& aObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            CMsgImageInfo* aInfo,
            RFs& aFs );

        /**
        * Destructor
        */
        virtual ~CPostcardOperationInsertImage( );

        /**
        * Just initializes the iLaunchStep and completes
        */
		void Start( TInt aArgument );

	public: // From ImageProcessor
	
		/*
		* If process was not succesfull -> sets error and completes with it,
		* Otherwise start scaling to screen
		*/ 
	    void ImageProcessingReady( TSize aBitmapSize, TInt aFileSize, TBool aCompressed );

   protected: // From MIHLViewerObserver
        
		/**
        * From MIHLViewerObserver Notifies client when image handler content has changed.
		* Leave situation is handled in ViewerError() callback.
		*
		* Creates a duplicate of iDestinationBitmap, creates a CGulIcon of it and
		* passes it to AppUi class
        * @return void
		*/
		void ViewerBitmapChangedL();

        /**
        * From MIHLViewerObserver Notifies client if error occurs in viewer.
        * 
        * @param aError     IN  System wide error code.
        *
        * @return void
		*/
		void ViewerError( TInt aError );

    private:

        /**
        * Called by RunL.
        * Calls different functions according to iLaunchStep
        */
        void DoLaunchStepL( );

        /**
        * Removes possible old image.
        * Starts creating new attachment
        */
        void DoCreateAttaL( );

        /**
        * This functions starts the image process
        */
		void DoStartProcessImageL( );

        /**
        * Sets the mimetypes and content location.
        * Stores headers
        */
        void DoFinalizeSaveL( );

        /**
        * Starts the scaling process by using ImageHandler Library (scaling to bitmap, not file)
        */
        void DoCreateBitmapL( );

        /**
        * Renames the attachment.
        * Called when the image needs no other special handling than just renaming.
        */
		void DoRenameAttaL( );

        /**
        * Returns ETrue if compression is needed for the image
        */
        TBool NeedsCompress( );
        
        /**
        * From CActive
        */
        void DoCancel( );

        /**
        * From CActive
        */
        void RunL( );
    
        /**
        * From CActive
        */
        TInt RunError( TInt aError );

    protected: 

        /**
        * Constructor.
        */
        CPostcardOperationInsertImage(
            MPostcardOperationObserver& aObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            CMsgImageInfo* aInfo,
            RFs& aFs );

        /**
        * 2nd phase constructor.
        */
        void ConstructL( );

    protected: // data

		/**
		* Every operation starts with EPostcardInsertImageCreateAtta
		*
		* If image is only converted/scaled, this is needed:
		*	EPostcardInsertImageConvert (callback ScalingReady)
		*
		* If image is only compressed, this is needed:
		*	EPostcardInsertImageCompress
		*
		* If image is first scaled and then compressed
		*	EPostcardInsertImageConvert (callback ScalingReady)
		*	EPostcardInsertImageCreateAttaAfterConvertForCompress
		*	EPostcardInsertImageCompress (callback CompressionReady)
		*	EPostcardInsertImageDeleteTempAtta
		*
		* If image is not specifically processed, this is needed:
		*	EPostcardInsertImageRename
		*
		* In the end EPostcardInsertImageFinalizeSave where the changes are commited.
		* And EPostcardInsertImageCreateBitmap creates the bitmap to be shown in the screen
		*
		*/

        enum TPostcardInsertImageState
            {
            EPostcardInsertImageInitializing = 0,
            EPostcardInsertImageCheckAttas,
            EPostcardInsertImageCreateAtta,
            EPostcardInsertProcessImage,
			EPostcardInsertImageRename,
            EPostcardInsertImageFinalizeSave,
            EPostcardInsertImageCreateBitmap,
            EPostcardInsertImageFinished,
            EPostcardInsertImageError
            };

        enum TPostcardInsertImageFlags
            {
            EPostcardInsertImageConvertingImage = 1,
            EPostcardInsertImageRequestActive = 2
            };

        CMsgImageInfo*      iInfo;		// ImageInfo given when this instance is constructed

		RFile*				iCompressFile; // RFile created when the file is being compressed/processed
		CMsvAttachment*		iScaleAtta;		// Attachment file for scaled file
		CMsvAttachment*		iCompressAtta;	// Attachment file for compressed file
		CMsvAttachment*		iNoOpAtta;		// Attachment file for images that need no special handling

		CUniImageProcessor*	iImageProcessor;	// Image processor

        MIHLFileImage*      iSourceImage;       // source image for scaling to bitmap
        MIHLBitmap*         iDestinationBitmap; // scaled MIHL bitmap
        MIHLImageViewer*    iImageHandler;      // image handler for scaling

        TInt                iFlags;
    };

#endif // __PostcardOperationInsertImage_H
