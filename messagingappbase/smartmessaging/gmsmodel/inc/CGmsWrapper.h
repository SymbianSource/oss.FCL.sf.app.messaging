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
*     Wrapper for handling GMS message. It wraps a CGmsModel.
*
*/



#ifndef __GMSWRAPPER_H_
#define __GMSWRAPPER_H_

// INCLUDES

#include <e32base.h>

// DATA TYPES

enum TGmsWrapperPanic
    {
    EGmsWrNoModel1,
    EGmsWrNoPicture,
    EGmsWrNoPicture2,
    EGmsWrFilenameEmpty,
    EGmsWrTmpPicFileNotNeeded,
    EGmsWrTmpFileAlreadyExists,
    EGmsWrFilenameEmpty2,
    EGmsWrNoPic1
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CMsgEditorView;
class CEikStatusPane;
class CGmsModel;
class RFs;
class CRichText;

// CLASS DECLARATION

/**
 * Class for handling GMS data and UI operations.
 */
class CGmsWrapper : public CBase
    {
    public: // construction and destruction

        /**
         * Two phased constructor.
         * @param aFs Reference to RFs.
         * @return The newly constructed object.
         */
        IMPORT_C static CGmsWrapper* NewL(RFs& aFs);

        /// Destructor.
        IMPORT_C ~CGmsWrapper();

    public: // new functions

        /**
         * Loads the message data from the rich text. It also creates a
         * temporary file for the picture.
         * @param aMsgRich The message data in a CRichText.
         */
        IMPORT_C void LoadMsgL(const CRichText& aMsgRich);

        /**
         * Creates a picture control, loads it with picture data, and adds the
         * new control to the view. The LoadMsgL() or PictureSelectionPopupL()
         * must have been called before this.
         * @param aView Reference to the view that gets the new control.
         * @param aId The ID that is assigned to the new image control.
         */
        IMPORT_C void AddPictureControlToViewL(CMsgEditorView& aView, TInt aId);

        /**
         * This method is deprecated, do not use.
         */
        IMPORT_C void PictureSelectionPopupL(TInt aBadPicTextResId);

        /**
         * Gives the body user text as a CRichText which is on the
         * cleanup stack.
         * @return The body text as CRichText.
         */
        IMPORT_C CRichText* GetTextLC();

        /**
         * This sets the text.
         * @param aRich The text.
         */
        IMPORT_C void SetTextL(CRichText& aRich);

        /**
         * Gets the entire message as HBufC.
         * @return The entire message as HBufC on CleanupStack.
         */
        IMPORT_C HBufC* GetMessageLC();

        /**
         * Sets the Picture Message title to the status pane.
         * @param aStatusPane Reference to the status pane.
         * @param aTitleTextResIs title resource id
         */
        IMPORT_C void SetPictureMsgTitleL(CEikStatusPane& aStatusPane,
            TInt aTitleTextResIs);

        /**
         * Size of message minus the space taken up by the text payload.
         * @return Message size excluding text.
         */
        IMPORT_C TInt MsgSizeExcludingText();

        /**
         * Adds the picture to Photo Album.
         * @param aDefaultNameResIs default name resource
         * @param aPictureCopiedResId resource for copied note
         */
        IMPORT_C void AddToPhotoAlbumL(TInt aDefaultNameResIs,
            TInt aPictureCopiedResId);

        /**
         * Accessor for iModel.
         * @return Constant reference to iModel.
         */
        IMPORT_C const CGmsModel& Model() const;

    private:

        /// Panic wrapper.
        void Panic(TGmsWrapperPanic aCode);

        /**
         * Creates a temporary bitmap from the data that has been loaded.
         * The LoadMsgL() has been called before this. The file is deleted
         * in the destructor. A temp file is not needed if
         * PictureSelectionPopupL() has been used, because in that case there
         * exists the original picture file in Photoalbum that we can use.
         */
        void ExportBitmapToTempFileL();

    private: // construction

        /// Second phase constructor
        void ConstructL();

        /**
         * Constructor.
         * @param aFs Reference to RFs.
         */
        CGmsWrapper(RFs& aFs);

    private: // prohibited

        /// Default constructor prohibited.
        CGmsWrapper();

        /// Copy contructor prohibited.
        CGmsWrapper(const CGmsWrapper& aSource);

        /// Assignment operator prohibited.
        const CGmsWrapper& operator=(const CGmsWrapper& aSource);

    private: // data

        /// Own: picture
        CGmsModel* iModel;

        mutable RFs& iFs;

        /// picture file name
        TFileName iFileName;

        /// if a temporary ota file exists
        TBool iExistsTmpFile;

    };


#endif //__GMSWRAPPER_H_

// end of file
