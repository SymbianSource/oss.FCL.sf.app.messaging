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



// INCLUDE FILES
#include "CGmsPictureControl.h"
#include "CGmsWrapper.h"

#include <gmsModel.h>
#include <s32file.h>                // RFileReadStream
#include <txtrich.h>                // CRIchText
#include <akntitle.h>
#include <StringLoader.h>
#include <DocumentHandler.h>        // CDocumentHandler
#include <aknnotewrappers.h>        // Avkon note wrappers
#include <mmsgbiocontrol.h>         // for KErrMsgBioMessageNotValid

// CONSTANTS

// The picture is written to this temporary file because Photoalbum wants it
// in a file. And the file ending has to be 'ota', otherwise it won't be
// recognised by the photoalbum viewer.
_LIT(KTempOtaFileName, "gmstmp.ota");

_LIT(KCGmsWrapper,"CGmsWrapper");

//  MEMBER FUNCTIONS

EXPORT_C CGmsWrapper* CGmsWrapper::NewL(RFs& aFs)
    {
    CGmsWrapper* self = new (ELeave) CGmsWrapper(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CGmsWrapper::~CGmsWrapper()
    {
    delete iModel;
    if (iExistsTmpFile)
        {
        __ASSERT_DEBUG(iFileName.Length() > 0, Panic(EGmsWrFilenameEmpty2));
        iFs.Delete(iFileName);
        }
    
    }

EXPORT_C void CGmsWrapper::LoadMsgL(const CRichText& aMsgRich)
    {
    iModel->ImportGMSL(aMsgRich);
    ExportBitmapToTempFileL();
    }

EXPORT_C void CGmsWrapper::AddPictureControlToViewL(CMsgEditorView& aView, TInt aId)
    {
    __ASSERT_DEBUG(iModel, Panic(EGmsWrNoModel1));
    __ASSERT_ALWAYS(iModel->HasPicture(), Panic(EGmsWrNoPicture));
    __ASSERT_ALWAYS(iFileName.Length() > 0, Panic(EGmsWrFilenameEmpty));
    CGmsPictureControl* imageControl = CGmsPictureControl::NewL(&aView);
    CleanupStack::PushL(imageControl);
    imageControl->SetControlId(aId);
    imageControl->LoadImageL(iFileName);
    aView.AddControlL(imageControl, imageControl->ControlId(),
    EMsgFirstControl, EMsgBody);
    CleanupStack::Pop(imageControl);
    }

EXPORT_C void CGmsWrapper::PictureSelectionPopupL(TInt /*aBadPicTextResId*/ )
    {
    // This method is deprecated
    User::Leave( KErrNotSupported );
    }

EXPORT_C CRichText* CGmsWrapper::GetTextLC()
    {
    CRichText* richText = CRichText::NewL(
        STATIC_CAST(CEikonEnv*, CCoeEnv::Static())->SystemParaFormatLayerL(),
        STATIC_CAST(CEikonEnv*, CCoeEnv::Static())->SystemCharFormatLayerL());
    CleanupStack::PushL(richText);
    HBufC16* text = iModel->TextAsHBufC16LC();
    richText->Reset(); // is this needed?
    richText->InsertL( 0, *text);
    CleanupStack::PopAndDestroy(text);
    return richText;
    }

EXPORT_C void CGmsWrapper::SetTextL(CRichText& aRich)
    {
    TInt docLength = aRich.DocumentLength();
    HBufC16* buf = HBufC16::NewLC(docLength);
    TPtr16 ptr16(buf->Des());
    aRich.Extract(ptr16, 0, docLength);
    iModel->SetTextL(ptr16);
    CleanupStack::PopAndDestroy(buf);
    }

EXPORT_C HBufC* CGmsWrapper::GetMessageLC()
    {
    return iModel->ExportGMS16LC();
    }

EXPORT_C void CGmsWrapper::SetPictureMsgTitleL(CEikStatusPane& aStatusPane,
    TInt aTitleTextResIs)
    {
    CEikStatusPane* sp = &aStatusPane;
    CAknTitlePane* title = STATIC_CAST(
        CAknTitlePane*,
        sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle )));
    HBufC* text = StringLoader::LoadLC(aTitleTextResIs);
    title->SetTextL(*text);
    CleanupStack::PopAndDestroy(text);
    }

EXPORT_C TInt CGmsWrapper::MsgSizeExcludingText()
    {
    return iModel->TotalSizeExcludingText();
    }

EXPORT_C void CGmsWrapper::AddToPhotoAlbumL(TInt aDefaultNameResIs,
    TInt /*aPictureCopiedResId*/)
    {
    CDocumentHandler* handler = CDocumentHandler::NewLC(NULL);
    TDataType nullType;
    TInt err = handler->CopyL(
        iFileName,
        *StringLoader::LoadLC(aDefaultNameResIs),
        nullType, NULL);
    CleanupStack::PopAndDestroy(2, handler); // (pic name) + handler
    if (err != KUserCancel && err != KErrNone)
        {
        User::Leave(err);
        }
    /* These are the other docHandler error codes:
    KBadMimeType         = 12002; // Mime type conflict etc.
    KMimeNotSupported    = 12003; // No handler found
    KNullContent         = 12005; // Empty content
    KDataDirNotFound     = 12006; // No data dir found
    KExecNotAllowed      = 12007; // Executables not allowed
    KNoHostProcess       = 12008; // Embedding without host
    KNotInitialized      = 12009; // Not initialized
    */
    }

EXPORT_C const CGmsModel& CGmsWrapper::Model() const
    {
    return *iModel;
    }

void CGmsWrapper::Panic(TGmsWrapperPanic aCode)
    {
    User::Panic(KCGmsWrapper, aCode);
    }

void CGmsWrapper::ExportBitmapToTempFileL()
    {
    __ASSERT_ALWAYS(iFileName.Length() == 0, Panic(EGmsWrTmpPicFileNotNeeded));
    __ASSERT_ALWAYS(!iExistsTmpFile, Panic(EGmsWrTmpFileAlreadyExists));

	User::LeaveIfError(iFs.CreatePrivatePath(EDriveC));
	
	TChar driveChar;
    iFs.DriveToChar( EDriveC,driveChar);
    TDriveName driveName;
    driveName.Append(driveChar);
    driveName.Append(KDriveDelimiter);
    
    iFs.PrivatePath(iFileName);
    iFileName.Insert(0,driveName);
   	iFileName.Append(KTempOtaFileName);
    RFileWriteStream writeStream;
    User::LeaveIfError(writeStream.Replace(
        iFs,
        iFileName,
        EFileStream ));
    iExistsTmpFile = ETrue;
    
    writeStream.PushL();
    iModel->ExportOTABitmapL(writeStream);
    writeStream.CommitL();
    writeStream.Close();
    CleanupStack::Pop(); // (writeStream cleanup item)
    }

void CGmsWrapper::ConstructL()
    {
    iModel = CGmsModel::NewL(iFs, KErrMsgBioMessageNotValid);
    }

CGmsWrapper::CGmsWrapper(RFs& aFs) : iFs(aFs), iExistsTmpFile(EFalse)
    {
    }

// end of file
