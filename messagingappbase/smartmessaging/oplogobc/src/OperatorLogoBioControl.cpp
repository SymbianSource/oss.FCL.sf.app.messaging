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
*     Bio control for Operator Logos.
*
*/



// INCLUDE FILES
#include "OperatorLogoBioControl.h"     // COperatorLogoBioControl
#include "OperatorLogoOTAConv.h"        // converter

#include <centralrepository.h>
#include <settingsinternalcrkeys.h>
#include <StringLoader.h>               // StringLoader (iCoeEnv)
#include <msgbiocontrolObserver.h>      // MMsgBioControlObserver
#include <OpLogoBC.rsg>                 // resouce identifiers
#include <MsgBioUtils.h>                // MsgBioUtils
#include <bldvariant.hrh>
#include <featmgr.h>					// Feature manager
#include <csxhelp/smart.hlp.hrh>
#include <mmtsy_names.h>
#include <AknUtils.h>
#include <msvapi.h>                     // CMsvEntry
#include <aknlayoutscalable_apps.cdl.h>
#include <MIHLScaler.h>                 // MIHLScaler

#ifdef RD_PHONE_CLIENT_EXT
#include <CPhCltImageHandler.h>
#include <CPhCltImageParams.h>
#include <CPhCltBaseImageParams.h>
#else
#include <CPhCltExtImageHandler.h>      // CPhCltExtImageHandler
#include <CPhCltExtImageParams.h>       // CPhCltExtImageParams
#include <CPhCltExtBaseImageParams.h>
#include <PhCltExt.h>                   // CPhCltExtFactory
#endif

#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>



// CONSTANTS

const TInt KLogoMaxWidthPixels      = 97;
const TInt KLogoMaxHeightPixels     = 25;
const TInt KUseLogo                 = 0;
const TInt KFirstMenuItem           = 1;
const TInt KLineFeed                = 0x0a;
const TInt KOtaBitmapInfoField      = 0x00;
// for infofield containing number of animated icons one:
const TInt KOtaBitmapInfoFieldAnim  = 0x01;
const TInt KOtaBitmapColorDepth     = 0x01;
const TInt KVersionNumberZero       = 48;   // ASCII value for '0'
const TInt KNumOfBytesMccMnc        = 3;
const TInt KNumOfBytesExpDay        = 19;
const TInt KPixelsInByte            = 8;

_LIT(KOpLogoCResourceFileName, "oplogoBC.rsc");

_LIT(KTempOtaFileName,"oltmp.ota");

_LIT(KPanicOplogoBC, "OpLogo");

#ifndef RD_PHONE_CLIENT_EXT
// PhoneClient Extension library
_LIT( KDllPhoneClientExt, "PhoneClientExt.dll" );
#endif

enum TOpLogoPanics
    {
    EPanicBadImageContent = 1,
    EPanicNullFileName,
    EPanicScalingError,
    EPanicOnlyOneLibraryAllowed,
    EPanicNullPtr
    };

// ==========================================================

/**
* Helper class which allows COperatorLogoBioControl to receive a callback
* to it's callback method when an asynchronous operation has finished.
* CAsyncCallBack needed tweaking since it is ment for callback only, and
* it completes the task immediately.
*/
class COpLogoAOCallBack: public CAsyncCallBack
    {
    public:

        COpLogoAOCallBack( const TCallBack &aCallBack, TInt aPriority )
            : CAsyncCallBack( aCallBack, aPriority )
            {
            }

        ~COpLogoAOCallBack()
            {
            // Empty
            }

        void SetItActive()
            {
            SetActive();
            StartWait();
            }

    protected:

        void RunL()
            {
            StopWait();
            iCallBack.CallBack();
            }

        void StartWait()
            {
            if( !iWait.IsStarted() )
                {
                iWait.Start();
                }
            }

        void StopWait()
            {
            if( iWait.IsStarted() )
                {
                iWait.AsyncStop();
                }
            }

        void DoCancel()
            {
            }

    private:

        CActiveSchedulerWait iWait;
    };

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor.
COperatorLogoBioControl::COperatorLogoBioControl(
     MMsgBioControlObserver& aObserver,
     CMsvSession* aSession,
     TMsvId aId,
     TMsgBioMode aEditorOrViewerMode ):
     CMsgBioControl( aObserver,
                     aSession,
                     aId,
                     aEditorOrViewerMode,
                     NULL /* file handle */ )
    {
    }

// Symbian OS default constructor can leave.
void COperatorLogoBioControl::ConstructL()
    {
	
	//we take the file server session to own variable in order
	//to avoid constant use of CCoeEnv::Static() and to reduce the usage
	//of TLS
    iFs = CCoeEnv::Static()->FsSession();
    if ( !CheckMsgValidityL() )
        {
        User::Leave( KErrMsgBioMessageNotValid );
        }

	FeatureManager::InitializeLibL();

    if( IsEditor() ) //This control doesnt support editing mode.
        {
        User::Leave( KErrNotSupported );
        }

    User::LeaveIfError( iPhoneServer.Connect() );


    LoadStandardBioResourceL();
    LoadResourceL( KOpLogoCResourceFileName );

    iScaler = IHLScaler::CreateL();
    iScalingAO = new ( ELeave ) COpLogoAOCallBack(
        TCallBack( ScalingReady, this ), CActive::EPriorityStandard );
        
    CreateBitmapL();

    // We want to scale the image now:
    SetRect( Rect() );

    // logo will be shown after asynch. scaling,
    // see COperatorLogoBioControl::DoScalingReady()
    }

// Two-phased constructor.
EXPORT_C CMsgBioControl* COperatorLogoBioControl::NewL(
    MMsgBioControlObserver& aObserver,
    CMsvSession* aSession,
    TMsvId aId,
    TMsgBioMode aEditorOrViewerMode,
    const RFile* /* aFile */ )
    {
    COperatorLogoBioControl* self =
        new( ELeave ) COperatorLogoBioControl( aObserver,
                                               aSession,
                                               aId,
                                               aEditorOrViewerMode );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
COperatorLogoBioControl::~COperatorLogoBioControl()
    {
    FeatureManager::UnInitializeLib();

    iPhoneServer.Close();

    if( iTempFileExists )
        {
        //delete tempfile if exists
        iFs.Delete(iTempPathAndName);
        }

	delete iBitmap;
    delete iScaledBitmap;
    
    delete iScaler;
    delete iScalingAO;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::SetAndGetSizeL
// ---------------------------------------------------------
//
void COperatorLogoBioControl::SetAndGetSizeL( TSize& aSize )
    {
    // Size proposed in aSize parameter is used, only height is
    // proposed by this control.

    if (AknLayoutUtils::ScalableLayoutInterfaceAvailable())
        {
        TAknWindowComponentLayout layout =
            AknLayoutScalable_Apps::mce_image_pane_g2();
        TAknLayoutRect layoutRect;
    	layoutRect.LayoutRect( Rect(), layout.LayoutLine() );
    	aSize.iHeight = layoutRect.Rect().Height();
        }

    else
        { // use old sizing functionality if scalable IF isn't available:
        aSize.iHeight /= 2; // get in to the middle of pane
        }

    SetSizeWithoutNotification( aSize );
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::SetMenuCommandSetL
// ---------------------------------------------------------
//
void COperatorLogoBioControl::SetMenuCommandSetL( CEikMenuPane& aMenuPane )
    {
    AddMenuItemL( aMenuPane, R_USE_LOGO, KUseLogo, KFirstMenuItem );
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::CurrentLineRect
// ---------------------------------------------------------
//
TRect COperatorLogoBioControl::CurrentLineRect() const
    {
    return Rect();
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::IsFocusChangePossible
// ---------------------------------------------------------
//
TBool COperatorLogoBioControl::IsFocusChangePossible(
    TMsgFocusDirection /*aDirection*/ ) const
    {
    return EFalse;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::HeaderTextL
// ---------------------------------------------------------
//
HBufC* COperatorLogoBioControl::HeaderTextL() const
    {
    HBufC* titleText = StringLoader::LoadL( R_TITLE_OPERATOR_LOGO );
    return titleText;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::HandleBioCommandL
// ---------------------------------------------------------
//
TBool COperatorLogoBioControl::HandleBioCommandL(TInt aCommand)
    {
    aCommand -= iBioControlObserver.FirstFreeCommand();

    TBool retVal( EFalse );
    switch ( aCommand )
        {
        case KUseLogo:
            {
            TrySaveBitmapL();
            retVal = ETrue;
            break;
            }
        default:
            {
            retVal = EFalse;
            break;
            }
        }
    return retVal;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::OptionMenuPermissionsL
// ---------------------------------------------------------
//
TUint32 COperatorLogoBioControl::OptionMenuPermissionsL() const
    {
    return EMsgBioDelete
        | EMsgBioMessInfo
        | EMsgBioMove
        | EMsgBioHelp
        | EMsgBioExit;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse COperatorLogoBioControl::OfferKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aType*/ )
    {
	return EKeyWasConsumed;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::GetHelpContext
// ---------------------------------------------------------
//
void COperatorLogoBioControl::GetHelpContext(
    TCoeHelpContext& aHelpContext ) const
	{
	if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
		{
		const TUid KUidSmart = {0x101F4CDA};

		aHelpContext.iContext = KSMART_HLP_OPLOGOVIEWER();
		aHelpContext.iMajor = KUidSmart;
		}
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::CountComponentControls
// ---------------------------------------------------------
//
TInt COperatorLogoBioControl::CountComponentControls() const
    {
    return 0;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* COperatorLogoBioControl::ComponentControl(
    TInt /*aIndex*/ ) const
    {
    return NULL;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::SizeChanged
// ---------------------------------------------------------
//
void COperatorLogoBioControl::SizeChanged()
    {
    TRect imageRect( Rect() );

    if (AknLayoutUtils::ScalableLayoutInterfaceAvailable())
        {
        TAknWindowComponentLayout layout =
            AknLayoutScalable_Apps::mce_image_pane_g2();
        TAknLayoutRect layoutRect;
    	layoutRect.LayoutRect( Rect(), layout.LayoutLine() );
    	imageRect = layoutRect.Rect();

        // If leave occurs then scaling is not performed
        TRAP_IGNORE( ScaleIfPossibleL( imageRect.Size() ) );
        }

    else
        {
        TSize imageSize( KLogoMaxWidthPixels, KLogoMaxHeightPixels );
        if ( imageRect.Width() < KLogoMaxWidthPixels ||
             imageRect.Height() < KLogoMaxHeightPixels )
            { // size of the control is smaller than the maximum size we
              // would use otherwise.
            imageSize = imageRect.Size();
            }

        // If leave occurs then scaling is not performed
        TRAP_IGNORE( ScaleIfPossibleL( imageSize ) );
        }
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::FocusChanged
// ---------------------------------------------------------
//
void COperatorLogoBioControl::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::SetContainerWindowL
// ---------------------------------------------------------
//
void COperatorLogoBioControl::SetContainerWindowL(
    const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::CheckMsgValidityL
// ---------------------------------------------------------
//
TBool COperatorLogoBioControl::CheckMsgValidityL()
    {
    RFile file;
    TFileName fileName;
    TInt fileSize(0);
    HBufC8* fileBuffer;

    CMsvEntry* entry = this->MsvSession(). GetEntryL(iId);
    CleanupStack::PushL( entry );
	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);

	MMsvAttachmentManager& manager = store->AttachmentManagerL();
	file = manager.GetAttachmentFileL(0);

    CleanupClosePushL(file);

    // get the size of file for descriptor
    User::LeaveIfError(file.Size(fileSize));

    fileBuffer = HBufC8::NewLC(fileSize);
    TPtr8 filePtr = fileBuffer->Des();

    User::LeaveIfError(file.Read(filePtr));

    TInt ret(CheckMsgFormatL(file, filePtr));

    CleanupStack::PopAndDestroy(fileBuffer);
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( store );
    CleanupStack::PopAndDestroy( entry );

    return ret;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::CheckMsgFormatL
// ---------------------------------------------------------
//
TBool COperatorLogoBioControl::CheckMsgFormatL( RFile aFile, TPtr8 aPtr )
    {
    TInt len = aPtr.Length();
    TInt descPosition( 0 );
    TUint8 byte;

    // first byte
    if (len > descPosition)
        {
        byte = aPtr[descPosition];
        }
    else
        {
        return EFalse;
        }

    if (byte == KVersionNumberZero)
        {
        descPosition++;
        if (len > descPosition)
            {
            byte = aPtr[descPosition];
            }
        else
            {
            return EFalse;
            }

        TInt count( 1 );

        while (byte != KLineFeed && count <= KNumOfBytesExpDay )
            {
            // loop Mcc, Mnc and expiration date bytes off.
            descPosition++;
            if (len > descPosition)
                {
                byte = aPtr[descPosition];
                }
            else
                {
                return EFalse;
                }
            count ++;
            }

        if (count <= KNumOfBytesMccMnc || count > KNumOfBytesExpDay )
            {
            // there has to be at least 3 bytes for Mcc and Mnc between Version
            // number and linefeed. if there was an expiration date, the length
            // can't exceed 15 bytes.
            return EFalse;
            }
        }

    else
        {
        // first byte of message was first byte of Mcc
        descPosition++; // second byte for Mcc
        descPosition++; // byte for Mnc
        }

    descPosition++;
    if (len > descPosition)
        {
        byte = aPtr[descPosition]; // Ota bitmap Infofield
        }
    else
        {
        return EFalse;
        }

    if ( byte == KLineFeed )
        {
        // there was linefeed before ota bitmap
        descPosition++;
        if (len > descPosition)
            {
            byte = aPtr[descPosition]; // Ota bitmap Infofield
            }
        else
            {
            return EFalse;
            }

        iLineFeedExists = ETrue;
        }

    if ( byte != KOtaBitmapInfoField)
        {
        if ( byte == KOtaBitmapInfoFieldAnim)
            {
            // The byte descripting ota bitmap's infofield should be 0x00, but
            // there is existing formats where this byte is 0x01, meaning that
            // number of animated icons is one. There should not to be any
            // animated icons in operator logo. But if there is one animated
            // icon, that could be interpretted as static bitmap. There has
            // been misunderstandings with smart messaging spec.
            TBuf8<1> temp;
            temp.Append(0x00);
            User::LeaveIfError(aFile.Write(descPosition ,temp));
            }
        else
            {
            return EFalse;
            }
        }

    descPosition++;
    TInt imageWidth;
    if (len > descPosition)
        {
        imageWidth = aPtr[descPosition];  // Ota bitmap width
        }
    else
        {
        return EFalse;
        }

    descPosition++;
    TInt imageHeight;
    if (len > descPosition)
        {
        imageHeight = aPtr[descPosition];  // Ota bitmap height
        }
    else
        {
        return EFalse;
        }

    if (imageWidth <= 0 || imageHeight <= 0)
        {
        return EFalse;
        }

    // number of bytes in Bitmap's data
    TInt numOfImageBytes = (imageWidth * imageHeight) / KPixelsInByte;
    descPosition++;
    if (len > descPosition)
        {
        byte = aPtr[descPosition];  // Ota bitmap color depth
        }
    else
        {
        return EFalse;
        }

    if (byte != KOtaBitmapColorDepth) // color depth must be black and white
        {
        return EFalse;
        }

    descPosition++; // position to the first byte of otabitmap's data
    if ( (len - descPosition) < numOfImageBytes  )
        {
        return EFalse;
        }

    return ETrue;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl ::CreateBitmapL()
// ---------------------------------------------------------
//
void COperatorLogoBioControl::CreateBitmapL()
    {
    // Create a temporary ota-bitmap file...

    RFileReadStream readStream;
    OpenOperatorLogoL( readStream );
    CleanupClosePushL(readStream);
    WriteBitmapToTempFileL( readStream );
    CleanupStack::PopAndDestroy( &readStream );

    // ... Then convert it to a bitmap
    if( iBitmap )
        {
        iBitmap->Reset();
        delete iBitmap;
        iBitmap = NULL;
        }
    iBitmap = new ( ELeave ) CFbsBitmap();

    __ASSERT_DEBUG( iTempPathAndName.Length() > 0, Panic( EPanicNullFileName ) );
    COperatorLogoOTAConv* conv = COperatorLogoOTAConv::NewLC();
    conv->ConvertImageL( iTempPathAndName, *iBitmap );
    CleanupStack::PopAndDestroy( conv );
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::OpenOperatorLogoL()
// ---------------------------------------------------------
//
void COperatorLogoBioControl::OpenOperatorLogoL(
    RFileReadStream& aReadStream )
    {
	RFile file;
    CMsvEntry* entry = this->MsvSession(). GetEntryL(iId);
    CleanupStack::PushL( entry );

	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);

	MMsvAttachmentManager& manager = store->AttachmentManagerL();

	file = manager.GetAttachmentFileL(0);
	CleanupClosePushL( file );
	aReadStream.Attach(file,0);
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( store );
    CleanupStack::PopAndDestroy( entry );
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::WriteBitmapToTempFileL()
// Creates OTA bitmap file
// ---------------------------------------------------------
//
void COperatorLogoBioControl::WriteBitmapToTempFileL(
    RFileReadStream& aReadStream )
    {
    iTempFileExists = EFalse;
    //Takes header information away: readStream includes pure OTA-bitmap
    //after that.
    DetachHeadersFromOpLogoL( aReadStream );
    
    
    User::LeaveIfError(iFs.CreatePrivatePath(EDriveC));
	
	TChar driveChar;
    iFs.DriveToChar( EDriveC,driveChar);
    TDriveName driveName;
    driveName.Append(driveChar);
    driveName.Append(KDriveDelimiter);
    
    iFs.PrivatePath(iTempPathAndName);
    iTempPathAndName.Insert(0,driveName);
   	iTempPathAndName.Append(KTempOtaFileName);
    

    RFileWriteStream writeStream;

    TInt err = writeStream.Replace( iFs,
                                    iTempPathAndName,
                                    EFileStream );
    User::LeaveIfError( err );
	
    writeStream.PushL();
    writeStream.WriteL( aReadStream );
    writeStream.CommitL();
    writeStream.Pop();
    writeStream.Release();
    	
    iTempFileExists = ETrue;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl ::DetachHeadersFromOpLogoL
// ---------------------------------------------------------
//
void COperatorLogoBioControl::DetachHeadersFromOpLogoL(
    RFileReadStream& aReadStream )
    {
    TBool isVersion = EFalse;
    //Check if there is version number, which is always
    //zero in first octet. If not, the Mobile country code is first one.
    //It describes first part of Mobile country code, which is
    //encoded in little-endian BCD format.
    TUint8 firstMcc = aReadStream.ReadUint8L();
    if( firstMcc == KVersionNumberZero ) //version number found,
       //so the file is according to Smart Messaging specification (2.0.0 or 3.0.0)
        {
        //Mcc is the next octet after version number.
        firstMcc = aReadStream.ReadUint8L();
        isVersion = ETrue;
        }

    //Next one describes second part of Mobile country code
    TUint8 secondMcc = aReadStream.ReadUint8L();

    //Next one describes Mobile network code, which is
    //encoded in little-endian BCD format.
    TUint8 mnc = aReadStream.ReadUint8L();

    if( isVersion ) //Take all the rubbish away before OTA-bitmap
        {
        TInt next = aReadStream.ReadUint8L();

        while (next != KLineFeed)
            {
            next = aReadStream.ReadUint8L();
            }
        }

    if( iLineFeedExists )
        {
        // remove the linefeed before ota bitmap data
        aReadStream.ReadUint8L();
        }

    SetMccAndMnc( firstMcc, secondMcc, mnc );
    }

// ---------------------------------------------------------
// COperatorLogoBioControl ::SetMccAndMnc
// ---------------------------------------------------------
//
void COperatorLogoBioControl::SetMccAndMnc(
    TUint8 aFmcc,
    TUint8 aSmcc,
    TUint8 aMnc )
    {
    // First BCD coded mcc decimal
    TUint mask = 0x0F; // This is for first byte
    TUint num = aFmcc & mask;

    iMcc = num * 100; // first num is hundreds.

     // Second BCD coded mcc desimal
    mask = 0xF0; // This is for second byte
    num = aFmcc & mask;
    num = num >> 4;
    if (num != 0x0F)
        {
        iMcc += 10 * num; // second num is tens.
        }

    // Third BCD coded mcc desimal
    mask = 0x0F; // This is for first byte
    num = aSmcc & mask;
    if (num != 0x0F)
        {
        iMcc += num; // third num is ones.
        }

    // Fourth BCD coded mcc desimal
    mask = 0x000000F0; // This is for second byte
    num = aSmcc & mask;
    num = num >> 4;
    // First BCD coded mnc desimal
    mask = 0x0000000F; // This is for first byte
    num = aMnc & mask;
    if (num != 0x00000000)
        {
        iMnc = num * 10; // first of mnc is tens.
        }

    // Second BCD coded mnc desimal
    mask = 0x000000F0; // This is for second byte
    num = aMnc & mask;
    num = num >> 4;
    if (num != 0x0000000F)
        {
        iMnc += num; // second of mnc is ones.
        }
    }

// ---------------------------------------------------------
// COperatorLogoBioControl :: DetachSizeFromTempBitmapL
// ---------------------------------------------------------
//
TSize COperatorLogoBioControl::DetachSizeFromTempBitmapL()
    {
    RFileReadStream readStream;

    TInt err = readStream.Open( iFs,
        iTempPathAndName, EFileStream );

    User::LeaveIfError( err );

    CleanupClosePushL( readStream );

    TSize tempSize;

    readStream.ReadUint8L(); // infofield
    tempSize.iWidth = readStream.ReadUint8L(); // width
    tempSize.iHeight = readStream.ReadUint8L(); // height

    CleanupStack::PopAndDestroy(&readStream);

    return tempSize;
    }

#ifdef RD_PHONE_CLIENT_EXT
// ---------------------------------------------------------
// COperatorLogoBioControl ::TrySaveBitmapL
// ---------------------------------------------------------
//
void COperatorLogoBioControl::TrySaveBitmapL()
    {
    // Scaling not ready yet bitmaps unexistent for some other reason:
    User::LeaveIfNull( iBitmap );

    // Create image handler, factory will be deleted even in leave case:
    CPhCltImageHandler* imageHandler = CPhCltImageHandler::NewL();
    CleanupStack::PushL( imageHandler );

    CPhCltImageParams* logoParams =
        imageHandler->CPhCltBaseImageParamsL( EPhCltTypeOperatorLogo );
    CleanupStack::PushL( logoParams );
    
    CPhCltExtOperatorLogoParams* opLogoParams = 
        static_cast<CPhCltExtOperatorLogoParams*>( logoParams );

    TBool clearingMsg( IsClearingMessageL() );
    if ( clearingMsg )
        {
        if ( ConfirmationQueryL( R_SM_DELETE_ALL_LOGOS ) )
            {
            // Deletion happens by saving empty params
            opLogoParams->SetCodesL( KPhCltDeleteOperatorLogo, KPhCltDeleteOperatorLogo, EPhCltLogoTypeOTA );
            imageHandler->SaveImages( *logoParams );
            
            // Set oplogo setting to not show active in user's settings:
            CRepository* repository;
            repository = CRepository::NewLC( KCRUidPersonalizationSettings );
			TInt keyVal = 0;
			TInt err = repository->Get( KSettingsShowOperatorLogoSetting, keyVal );
			if ( err == KErrNone )
				{
				keyVal = 0;
				User::LeaveIfError( repository->Set( KSettingsShowOperatorLogoSetting, keyVal ));
				}
			else if ( err != KErrNotFound )
				{
				User::Leave( KErrCorrupt );	// Bad value in key
				}
			CleanupStack::PopAndDestroy(repository);
            }
            
        }
    else
        {
        opLogoParams->SetCodesL( iMcc, iMnc, EPhCltLogoTypeOTA );
        TBool replace( EFalse );
        TBool exists( LogoExistsL( *imageHandler, opLogoParams ) );

        if ( exists )
            {
            replace = ConfirmationQueryL( R_SM_REPLACE_LOGO );
            }

        if ( replace || !exists )
            {
            logoParams->AddImageL( iBitmap->Handle() );
            User::LeaveIfError( imageHandler->SaveImages( *logoParams ) );

            MsgBioUtils::ConfirmationNoteL( R_SM_NOTE_LOGO_COPIED );

            // Set oplogo showing active in user's settings:
            CRepository* repository;
            repository = CRepository::NewLC( KCRUidPersonalizationSettings );
			TInt keyVal = 0;
			TInt err = repository->Get( KSettingsShowOperatorLogoSetting, keyVal );
			if ( err == KErrNone )
				{
				keyVal = 1;
				User::LeaveIfError( repository->Set( KSettingsShowOperatorLogoSetting, keyVal ));
				}
			else if ( err != KErrNotFound )
				{
				User::Leave( KErrCorrupt );	// Bad value in key
				}
			CleanupStack::PopAndDestroy(repository);
            }
        }

    CleanupStack::PopAndDestroy( 2 ,imageHandler); // logoParams, imageHandler
    }
#else
// ---------------------------------------------------------
// COperatorLogoBioControl ::TrySaveBitmapL
// ---------------------------------------------------------
//
void COperatorLogoBioControl::TrySaveBitmapL()
    {
    // Scaling not ready yet bitmaps unexistent for some other reason:
    User::LeaveIfNull( iBitmap );
    
    // Load Phone Client Extension library
    // iControlDllLibrary is pushed to cleanup stack if leave should happen.
    LoadLibraryLC();

    TLibraryFunction entry = iControlDllLibrary.Lookup( 1 );
    CPhCltExtFactory* factory = reinterpret_cast< CPhCltExtFactory* >( entry() );
    // Factory cannot be pushed to cleanupstack because next method is LD.

    // Create image handler, factory will be deleted even in leave case:
    CPhCltExtImageHandler* imageHandler = factory->CPhCltExtImageHandlerLD();
    CleanupStack::PushL( imageHandler );

    User::LeaveIfError( imageHandler->Open( iPhoneServer ) );

    CPhCltExtImageParams* logoParams =
        imageHandler->CPhCltExtBaseImageParamsL( EPhCltTypeOperatorLogo );
    CleanupStack::PushL( logoParams );
    
    CPhCltExtOperatorLogoParams* opLogoParams = 
        static_cast<CPhCltExtOperatorLogoParams*>( logoParams );
    

    TBool clearingMsg( IsClearingMessageL() );
    if ( clearingMsg )
        {
        if ( ConfirmationQueryL( R_SM_DELETE_ALL_LOGOS ) )
            {
            // Deletion happens by saving empty params
            opLogoParams->SetCodesL( KPhCltDeleteOperatorLogo, KPhCltDeleteOperatorLogo, EPhCltLogoTypeOTA );
            imageHandler->SaveImages( *logoParams );
            
            // Set oplogo setting to not show active in user's settings:
            CRepository* repository;
            repository = CRepository::NewLC( KCRUidPersonalizationSettings );
			TInt keyVal = 0;
			TInt err = repository->Get( KSettingsShowOperatorLogoSetting, keyVal );
			if ( err == KErrNone )
				{
				keyVal = 0;
				User::LeaveIfError( repository->Set( KSettingsShowOperatorLogoSetting, keyVal ));
				}
			else if ( err != KErrNotFound )
				{
				User::Leave( KErrCorrupt );	// Bad value in key
				}
			CleanupStack::PopAndDestroy(repository);
            }
            
        }
    else
        {
        opLogoParams->SetCodesL( iMcc, iMnc, EPhCltLogoTypeOTA );
        TBool replace( EFalse );
        TBool exists( LogoExistsL( *imageHandler, opLogoParams ) );

        if ( exists )
            {
            replace = ConfirmationQueryL( R_SM_REPLACE_LOGO );
            }

        if ( replace || !exists )
            {
            logoParams->AddImageL( iBitmap->Handle() );
            User::LeaveIfError( imageHandler->SaveImages( *logoParams ) );

            MsgBioUtils::ConfirmationNoteL( R_SM_NOTE_LOGO_COPIED );

            // Set oplogo showing active in user's settings:
            CRepository* repository;
            repository = CRepository::NewLC( KCRUidPersonalizationSettings );
			TInt keyVal = 0;
			TInt err = repository->Get( KSettingsShowOperatorLogoSetting, keyVal );
			if ( err == KErrNone )
				{
				keyVal = 1;
				User::LeaveIfError( repository->Set( KSettingsShowOperatorLogoSetting, keyVal ));
				}
			else if ( err != KErrNotFound )
				{
				User::Leave( KErrCorrupt );	// Bad value in key
				}
			CleanupStack::PopAndDestroy(repository);
            }
        }

    CleanupStack::PopAndDestroy( 3 ,&iControlDllLibrary); // logoParams, imageHandler, iControlDllLibrary
    }

void COperatorLogoBioControl::LoadLibraryLC()
    {
    // If iControlDllLibrary has a handle value (!=0) then library
    // is already loaded. BioControlFactory supports only one loaded library
    // at a time.
    __ASSERT_ALWAYS( iControlDllLibrary.Handle() == 0,
                Panic( EPanicOnlyOneLibraryAllowed ) );

    TFileName fullname(NULL);
    fullname.Append( KDllPhoneClientExt );
   	User::LeaveIfError( iControlDllLibrary.Load( fullname ) );
   	CleanupClosePushL( iControlDllLibrary );
    }
#endif


// ---------------------------------------------------------
// COperatorLogoBioControl::LogoExistsL
// ---------------------------------------------------------
//
#ifdef RD_PHONE_CLIENT_EXT
TBool COperatorLogoBioControl::LogoExistsL(
    CPhCltImageHandler& aImageHandler,
    CPhCltImageParams* aLogoParams )
#else
TBool COperatorLogoBioControl::LogoExistsL(
    CPhCltExtImageHandler& aImageHandler,
    CPhCltExtImageParams* aLogoParams )
#endif    
    {

    TInt err = aImageHandler.LoadImages( aLogoParams );
    if( err != KErrNone && err != KErrNotFound )
        {
        User::Leave( err );
        }

    TBool retVal;
    if( err == KErrNone && aLogoParams->Count() > 0 )
        {
        retVal = ETrue;
        }
    else // KErrNotFound ||  oldLogoParams->Count() == 0
        {
        retVal = EFalse;
        }
    return retVal;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::CalculateCropRect
// ---------------------------------------------------------
//
TRect COperatorLogoBioControl::CalculateCropRect( TSize aSize )
    {
    // crops from right and bottom

    TRect cropperRect( 0, 0, aSize.iWidth, aSize.iHeight );

    if ( aSize.iHeight > KLogoMaxHeightPixels )
        {
        TInt heightDiff( aSize.iHeight - KLogoMaxHeightPixels );
        cropperRect.iBr.iY = aSize.iHeight - heightDiff;
        }
    if ( aSize.iWidth > KLogoMaxWidthPixels )
        {
        TInt widthDiff( aSize.iWidth - KLogoMaxWidthPixels );
        cropperRect.iBr.iX = aSize.iWidth - widthDiff;
        }

    return cropperRect;
    }


// ---------------------------------------------------------
// COperatorLogoBioControl ::VirtualHeight
// ---------------------------------------------------------
//
TInt COperatorLogoBioControl::VirtualHeight()
    {
    return 0;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl ::VirtualVisibleTop
// ---------------------------------------------------------
//
TInt COperatorLogoBioControl::VirtualVisibleTop()
    {
    return 0;
    }

// ---------------------------------------------------------
// COperatorLogoBioControl::IsClearingMessageL
// ---------------------------------------------------------
//
TBool COperatorLogoBioControl::IsClearingMessageL()
    {
    TBool ret( EFalse );
    RTelServer server;

    // connect to server
    TInt res = server.Connect();
    if ( res != KErrNone )
        { // cannot continue
        return ret;
        }

    // load phone module
    res=server.LoadPhoneModule( KMmTsyModuleName );
    if ( res != KErrNone )
        { // cannot continue, close the server
        server.Close();
        return ret;
        }

    // get phone info for phone name.
    RTelServer::TPhoneInfo phoneInfo;
    res = server.GetPhoneInfo( 0, phoneInfo );

    // open mobilephone subsession
    RMobilePhone mobilePhone;
    res = mobilePhone.Open( server, phoneInfo.iName );
    if (res != KErrNone )
        { // cannot continue, unload the phone module and close the server.
        server.UnloadPhoneModule( KMmTsyModuleName );
        server.Close();
        return ret;
        }

	// Get the current network MCC & MNC codes
	TRequestStatus status;
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg networkInfoPckg( networkInfo );

	mobilePhone.GetCurrentNetwork( status,
		                           networkInfoPckg,
								   area);
	User::WaitForRequest( status );

	// Parse the codes. Notice, this does not give correct results in WINS !!!
	TLex lexMcc( networkInfo.iCountryCode );
	TLex lexMnc( networkInfo.iNetworkId );
	TInt mcc;
    TInt mnc;
	if ( lexMcc.Val( mcc ) != KErrNone )
		{
		// cannot continue, unload the phone module and close the server.
        mobilePhone.Close();
		server.UnloadPhoneModule( KMmTsyModuleName );
        server.Close();
        return ret;
		}

	if ( lexMnc.Val( mnc ) != KErrNone )
		{
		// cannot continue, unload the phone module and close the server.
        mobilePhone.Close();
		server.UnloadPhoneModule( KMmTsyModuleName );
        server.Close();
        return ret;
		}

    // check are MCC and MNC same with codes came with message.
    ret = !( iMcc == mcc && iMnc == mnc );

    // close mobile phone
    mobilePhone.Close();

    // unload phone module
    server.UnloadPhoneModule( KMmTsyModuleName );

    // close server.
    server.Close();

    return ret;
    }

void COperatorLogoBioControl::Reset()
    {
    __ASSERT_DEBUG( iScalingAO, Panic( KErrArgument ) );

    if ( iScalingAO->IsActive() )
        {
        iScalingAO->Cancel();
        }
	
	if(iBitmap)
		{
		iBitmap->Reset();
    	delete iBitmap;
    	iBitmap = NULL;
		}

	if(iScaledBitmap)
		{
		iScaledBitmap->Reset();
    	delete iScaledBitmap;
    	iScaledBitmap = NULL;
		}
    }

TInt COperatorLogoBioControl::ScalingReady( TAny* aPtr )
    {
    __ASSERT_DEBUG(aPtr,User::Panic( KPanicOplogoBC, EPanicNullPtr));
    static_cast<COperatorLogoBioControl*>( aPtr )->DoScalingReady();
    return 0;
    }

void COperatorLogoBioControl::DoScalingReady()
    {
    __ASSERT_DEBUG( iScalingAO->iStatus == KErrNone,
                    Panic( EPanicScalingError ) );

    if ( iScalingAO->iStatus == KErrNone )
        {
        // We want to draw the image now:
        DrawNow();
        }
    }

void COperatorLogoBioControl::Panic( TInt aCode )
    {
    User::Panic( KPanicOplogoBC, aCode );
    }

void COperatorLogoBioControl::Draw( const TRect& aRect ) const
    {
    // Ensure that scaled bitmap exists and that no scaling is
    // currently ongoing.
    if ( iScaledBitmap &&
         iScaledBitmap->Handle() &&
         ! iScalingAO->IsActive() )
        {
        CWindowGc& gc = SystemGc();

        TRect bitmapRect;
        TAknWindowComponentLayout layout =
        	AknLayoutScalable_Apps::mce_image_pane_g2();
        TAknLayoutRect LayoutRect;
		LayoutRect.LayoutRect( aRect, layout.LayoutLine() );
		bitmapRect = LayoutRect.Rect();

		TInt yAxisOffset = aRect.iTl.iY - bitmapRect.iTl.iY;
		bitmapRect.Move(0,yAxisOffset);
		gc.DrawBitmap(bitmapRect,iScaledBitmap);
        }
    }

TBool COperatorLogoBioControl::ScaleIfPossibleL( TSize aSize )
    {
	if ( !iBitmap )
        {
        // Image hasn't been loaded, cannot scale
        return EFalse;
        }

    else
        {
        // Scaling is possible, it is done asynchronously

        if ( iScalingAO->IsActive() )
            { // In this case scaling was already ongoing, but it is cancelled
              // due to overriding request.
            iScalingAO->Cancel();
            }

        delete iScaledBitmap;
        iScaledBitmap = NULL;
        iScaledBitmap = new( ELeave ) CFbsBitmap;

        // Bitmap is scaled (cropped first if it exceeds 97*25 pixels).

        //get the current size of operator logo
        TSize logoSize = DetachSizeFromTempBitmapL();
        User::LeaveIfError( iScaler->Scale( iScalingAO->iStatus,
                                            *iBitmap,
                                            CalculateCropRect( logoSize ),
                                            *iScaledBitmap,
                                            TRect(aSize) ) );
        iScalingAO->SetItActive();
        return ETrue;
        }
    }

//  End of File
