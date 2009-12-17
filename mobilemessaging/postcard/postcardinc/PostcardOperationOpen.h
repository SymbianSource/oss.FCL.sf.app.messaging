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
*       CPostcardOperationOpen, asynchronous operation for postcard opening
*
*/




#ifndef __POSTCARDOPERATIONOPEN_H
#define __POSTCARDOPERATIONOPEN_H

// INCLUDES

#include <e32base.h>
#include <e32std.h>

#include <uniimageprocessor.h>

#include <MIHLViewerObserver.h>

#include "PostcardOperation.h"
#include "PostcardContact.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class MIHLFileImage;
class MIHLBitmap;
class MIHLImageViewer;
class CContactCard;
class CFbsBitmap;
class CMsgMediaInfo;
class CMsvAttachment;
class CPlainText;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CPostcardOperationOpen 
*
* @lib postcardeditor.exe
* @since 3.0
*/
class CPostcardOperationOpen : public CPostcardOperation,
                               public MUniImageProcessorCallback,
                               public MIHLViewerObserver

    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        * @since    3.0
        * @return   Pointer to instance
        */
        static CPostcardOperationOpen* NewL(
            MPostcardOperationObserver& aObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            RFs& aFs );

        /**
        * Destructor
        */
        virtual ~CPostcardOperationOpen( );

        /**
        * Just initializes the iLaunchStep and completes
        */
		void Start( TInt aArgument );

    private: // Constants

        enum TPostcardOpenState
            {
            EPostcardOpenInitializing = 0,
            EPostcardOpenCheckAttas,
            EPostcardOpenHandleImage,
            EPostcardOpenProcessImage,
			EPostcardOpenUpdateSendasImageFile,
            EPostcardOpenScaleImage,
            EPostcardOpenHandleText,
            EPostcardOpenHandleRecipient,
            EPostcardOpenFinished,
            EPostcardOpenError
            };

        enum TPostcardOpenFlags
            {
            EPostcardOpenRequestActive = 1,
            EPostcardOpenConvertingPNG = 2
            };

    private: // Methods

        /**
        * Called by RunL.
        * Calls different functions according to iLaunchStep
        */
        void DoLaunchStepL( );

        /**
        * If image is present and postcard type is sendui 
        * -> creates a media info object and checks the validity
        * If image needs compressing -> starts to create a new attachment
        * Otherwise starts scaling the image to bitmaps
        */
        void DoHandleImageL( );

        /**
        * This functions starts the image process
        */
		void DoStartProcessImageL( );

        /**
        * In case of image, adds CMsvHeaders to the attachment
        */
        void DoUpdateSendasAttachmentL( const TDataType& aMimeType );

        /*
        * Parse address and greeting out of special format text
        */
        void ParseSpecialFormatTextL( CPlainText& aText, CContactCard& aCard, CPlainText& aGreeting );

        /**
        * Reads the possible text from the message store.
        * Check if this is special phonebook created text where the contact id is stored
        * If it is, DoHandlePhonebookSendAsL is called.
        * Otherwise if this is SendUi originated file, just passes the text into AppUi object.
        * If opened for example from Drafts or Sent folder check if special text format is used
        * If it is, DoHandleSpecialFormatOpenL is called.
        * Otherwise just passes the text to AppUi object.
        */
        void DoHandleTextL( );

        /**
        * Reads the VCard from the message store. If this is forward, it deletes the VCard.
        * Otherwise creates new CContactCard from the store and passes it to AppUi object.
        */
        void DoHandleRecipientL( );

        /**
        * Parses recipient information from the store and adds to contact card
        */
        void ParseRecipientL( CContactCard& aContactCard );

        /**
        * Creates a CContactCard from a contact. CPostcardRecipientWrapper is
        * used to decide which address location should be used. If there is
        * more than one location available CPostcardRecipientWrapper shows
        * a dialog to user. New CContactCard is passed to AppUi object.
        * @param aContact Contact to create a contact card from
        */
		void DoHandlePhonebookSendAsL( CPostcardContact& aContact );

        /**
        * Updates contact card with name and address information from the contact.
        * @param aCard Contact card to update
        * @param aContact Contact to get information from
        * @param aLocation Address location to update contact card with
        */
        void UpdateContactCardL( CContactCard& aCard,
            CPostcardContact& aContact, CPostcardContact::TLocation aLocation );

        /**
        * Add text field to a contact card
        * @param aCard Contact card to update
        * @param aText Field text
        * @param aFieldSelect Field to update
        */
        static void AddContactCardFieldL( CContactCard& aCard,
            const TPtrC& aText, TInt aFieldSelect );

        /**
        * Text is read from the message store. Special format syntax is read from the ini resource file.
        * The text is checked field by field and new text and CContactCard objects are created out of it.
        * The new objects are passes to AppUi object.
        * When this function is called, DoHandleRecipientL is not used at all as the recipient
        * information was externalized from the greeting text.
        */
		void DoHandleSpecialFormatOpenL( CPlainText& aText );

        /**
        * Move operation to a new state
        */
        void ToState( TPostcardOpenState aState );

        /**
        * Move operation to error state
        */
        void ToErrorState( TInt aResourceId );

        /**
        * Leaving version of ScalingReady()
        */    
        void ScalingReadyL();
		
        /**
        * From CActive
        */
        void DoCancel( );

        /**
        * From CActive
        * Calls DoLaunchStepL
        */
        void RunL( );

	public: // From ImageProcessor
	
		/*
		* If process was not succesfull -> sets error and completes with it,
		* Otherwise calls DoProcessImageFinishedL to start scaling to screen
		*/ 
	    void ImageProcessingReady( TSize aBitmapSize, TInt aFileSize, TBool aCompressed );


    private: // called by the two functions above
    
        /**
        * Removes the original RFile because either compressor or converter created a new one.
        * Creates ImageHandler and starts scaling the new image to screen (to bitmap)
        * 
        * Creates ImageHandler and starts scaling the image to bitmap to be displayed on the screen
        */    
        void StartScalingToScreenL( );

	
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

    protected:

        /**
        * From CActive
        */
        TInt RunError( TInt aError );

        /**
        * Constructor.
        */
        CPostcardOperationOpen(
            MPostcardOperationObserver& aObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            RFs& aFs );

        /**
        * 2nd phase constructor.
        */
        void ConstructL( );

    protected: // data

        CMsgMediaInfo*      iImageInfo;
		CUniImageProcessor*	iImageProcessor;		// Image processor

        const TDesC&        iSpecialFormat;
        const TDesC&        iSeparatorChar;

        RFile*				iEditFile;
        CMsvAttachment*     iEditAtta;
        CFbsBitmap*			iBitmap;
        CFbsBitmap*			iBitmapMask;

        MIHLFileImage*      iSourceImage;       // source image for scaling to bitmap
        MIHLBitmap*         iDestinationBitmap; // scaled MIHL bitmap
        MIHLImageViewer*    iImageHandler;      // image handler for scaling

        TInt                iFlags;
    };

#endif // __PostcardOperationOpen_H
