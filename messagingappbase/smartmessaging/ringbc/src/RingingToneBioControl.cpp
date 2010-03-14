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
* Description:   BIO control for Ringing Tones.
*
*/



// INCLUDE FILES

#include "RingingToneBioControl.h"      // for CRingingToneBioControl
#include "RingBCToneConverter.h"             // for CComNSMConverter
#include "RingBC.hrh"
#include "RingBCLog.h"

#include <biocmtm.h>                    // for CBIOClientMtm
#include <coeutils.h>                   // for ConeUtils
#include <msgbiocontrolObserver.h>      // for MMsgBioControlObserver
#include <StringLoader.h>               // for StringLoader
#include <AknsUtils.h>
#include <RingBC.rsg>                   // for resouce identifiers
#include <ringbc.mbg>                   // for icons indexes
#include <ProfileEngineSDKCRKeys.h>
#include <MsgBioUtils.h>                // for MsgBioUtils
#include <eikrted.h>                    // for CEikRichTextEditor
#include <txtrich.h>                    // for CRichText
#include <aknnotewrappers.h>            // for CAknInformationNote
#include <Profile.hrh>                  // for EProfileRingingTypeSilent
#include <bldvariant.hrh>
#include <pathinfo.h>
#include <featmgr.h>					// Feature manager
#include <csxhelp/smart.hlp.hrh>
#include <data_caging_path_literals.hrh>
#include <applayout.cdl.h>				// LAF
#include <aknlayoutscalable_apps.cdl.h>
#include <mmsvattachmentmanager.h>

#include <msgaudiocontrol.h>	//AudioControl
#include <MsgEditorCommon.h>	//MsgEditorCommons utilities

// CONSTANTS

_LIT(KRingingToneFileExtension,".rng");
_LIT(KResourceName, "ringbc.rsc" );
_LIT(KRingBCMbmFileName, "ringbc.mif");

// LOCAL CONSTANTS AND MACROS
const TInt KComponentCountZero = 0;
const TInt KNoOverWriteOrRecurse = 0;
const TInt KLengthOfTitleRow = 100;
const TInt KTextNameQueryMaxLength = 50;


const TInt KRichEditor              = 1;
const TInt KAudioControl            = 0;

const TInt KRingVolumeZero = 0;

const TInt KShowNoNotes = 0;
const TInt KShowNotes = 1;

const TInt KFrameExpansion = 10;
const TInt KMaxVolumeLevel = 10;

// MODULE DATA STRUCTURES

enum TOptionListLocation
    {
    EFirstMenuItem  = 0,
    ESecondMenuItem
    };

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
EXPORT_C CMsgBioControl* CRingingToneBioControl::NewL(
            MMsgBioControlObserver& aObserver,
            CMsvSession* aSession,
            TMsvId aId,
            TMsgBioMode aEditorOrViewerMode,
            const RFile* aFile)
    {
    CRingingToneBioControl* self =
    	new( ELeave ) CRingingToneBioControl(
    		aObserver,
            aSession, aId, aEditorOrViewerMode,
            aFile);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self); // self

    return self;

    }

// Destructor
CRingingToneBioControl::~CRingingToneBioControl()
    {
    FeatureManager::UnInitializeLib();
    delete iRichEditor;
    delete iAudioControl;    
    delete iToneTitle;
    }

// C++ default constructor.
CRingingToneBioControl::CRingingToneBioControl(
        MMsgBioControlObserver& aObserver,
        CMsvSession* aSession,
        TMsvId aId,
        TMsgBioMode aEditorOrViewerMode,
        const RFile* aFile):
        CMsgBioControl(aObserver,
            aSession,
            aId,
            aEditorOrViewerMode,
            aFile)
    {
    }

// Symbian OS default constructor can leave.
void CRingingToneBioControl::ConstructL()
    {
    if( IsEditor() )   //Editor mode is not supported
        {
        User::Leave( KErrNotSupported );
        }

	FeatureManager::InitializeLibL();

    LoadStandardBioResourceL();
    LoadResourceL( KResourceName );

    // checks if the message is valid
    CheckMsgValidityL();
    TakeTitleL();

    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::CheckMsgValidityL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::CheckMsgValidityL()
    {
    RFile file;

    CMsvEntry* entry = this->MsvSession(). GetEntryL(iId);
    CleanupStack::PushL( entry );

    CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);

	MMsvAttachmentManager& manager = store->AttachmentManagerL();
	file = manager.GetAttachmentFileL(0);

    CleanupClosePushL(file);

    TInt size;
    User::LeaveIfError(file.Size(size));
    if (!size)
        {
        //the empty ringing tone is considered a corrupted message
        User::Leave(KErrMsgBioMessageNotValid);
        }
	
	HBufC8* dataBuf = HBufC8::NewLC(size);
    TPtr8 data(dataBuf->Des());
    User::LeaveIfError(file.Read(data, size));

    CRingBCNSMConverter* conv = CRingBCNSMConverter::NewL();
    CleanupStack::PushL( conv );
    TBool res = conv->IsRingToneMimeTypeL(data);

    if (!res)
        {
        User::Leave(KErrMsgBioMessageNotValid);
        }
        
    CleanupStack::PopAndDestroy(5,entry); // entry,conv,store,file,dataBuf
    }


// ----------------------------------------------------------------------------
// CRingingToneBioControl::ProcessCommandL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::ProcessCommandL( TInt aCommandId )
    {
    if ( aCommandId == ESoftkeyStop )
		{
		StopPlayback();
        }
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::SetAndGetSizeL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::SetAndGetSizeL( TSize& aSize )
    {
	//the heightadjustment calculates the height for the header elements and 
	//the space between components
   	TInt heightAdjustment = (MsgEditorCommons::MsgBaseLineDelta()*2)+
   							MsgEditorCommons::MsgBaseLineOffset();
   	
   	aSize.iHeight -= heightAdjustment;
   	SetSizeWithoutNotification( aSize  );
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::SetMenuCommandSetL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::SetMenuCommandSetL( CEikMenuPane& aMenuPane )
    {
    // if playing, do not show context sensitive menu.
    if ( iPlayerState != EMsgAsyncControlStatePlaying )
        {
		//removing the playback option from options menu, single click will do the playing
        //AddMenuItemL(aMenuPane, R_SM_PLAYBACK, ERingtPlayback, EFirstMenuItem);
        AddMenuItemL(aMenuPane,
        	R_SM_ADD_COMPOSER, ERingtAddComposer, ESecondMenuItem);
        }
	}

// ----------------------------------------------------------------------------
// CRingingToneBioControl::CurrentLineRect
// ----------------------------------------------------------------------------
//
TRect CRingingToneBioControl::CurrentLineRect() const
    {
    return iAudioControl->CurrentLineRect();
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::IsFocusChangePossible
// ----------------------------------------------------------------------------
//
TBool CRingingToneBioControl::IsFocusChangePossible(
        TMsgFocusDirection aDirection ) const
    {
    return iAudioControl->IsFocusChangePossible( aDirection );
	}

// ----------------------------------------------------------------------------
// CRingingToneBioControl::HeaderTextL
// ----------------------------------------------------------------------------
//
HBufC* CRingingToneBioControl::HeaderTextL() const
    {
    return iCoeEnv->AllocReadResourceL( R_SM_TITLE_RINGING_TONE );
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::HandleBioCommandL
// ----------------------------------------------------------------------------
//
TBool CRingingToneBioControl::HandleBioCommandL(TInt aCommand)
    {
    aCommand -= iBioControlObserver.FirstFreeCommand();
    TBool retValue(EFalse);
    switch ( aCommand )
        {
        case ERingtPlayback:
            PlaybackL();
            retValue = ETrue;
            break;
        case ERingtAddComposer:
            TrySaveToneL();
            retValue = ETrue;
            break;
        default:
            retValue = EFalse;
            break;
        }
	return retValue;
    }

// ----------------------------------------------------------------------------
// CRingingToneLogoBioControl::OptionMenuPermissionsL
// ----------------------------------------------------------------------------
//
TUint32 CRingingToneBioControl::OptionMenuPermissionsL() const
    {
    if ( iPlayerState != EMsgAsyncControlStatePlaying )
        {
        return EMsgBioDelete
        | EMsgBioMessInfo
        | EMsgBioMove
        | EMsgBioHelp
        | EMsgBioExit;
        }
    else 
        {
        // do not show context sensitive menu.
        return 0;
        }
    }



// ----------------------------------------------------------------------------
// CRingingToneBioControl::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CRingingToneBioControl::OfferKeyEventL(
        const TKeyEvent& /*aKeyEvent*/,
        TEventCode /*aType*/ )
    {
    return EKeyWasConsumed;
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::GetHelpContext
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::GetHelpContext(
	TCoeHelpContext& aHelpContext) const
	{
	if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
		{
		const TUid KUidSmart = {0x101F4CDA};

		aHelpContext.iContext = KSMART_HLP_RINGVIEWER();
		aHelpContext.iMajor = KUidSmart;
		}
	}

// ----------------------------------------------------------------------------
// CRingingToneBioControl::CountComponentControls
// ----------------------------------------------------------------------------
//
TInt CRingingToneBioControl::CountComponentControls() const
    {
    TInt componentCount( KComponentCountZero );
    if ( iAudioControl )
        {
        componentCount++;
        }
	if ( iRichEditor )        
		{
		componentCount++;
		}
    return componentCount;
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::ComponentControl
// ----------------------------------------------------------------------------
//
CCoeControl* CRingingToneBioControl::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == KRichEditor )
    	{
    	return iRichEditor;
    	}
    if ( aIndex == KAudioControl )
        {
        return iAudioControl;
        }
    else
    	{
    	return NULL;
    	}
	}

// ----------------------------------------------------------------------------
// CRingingToneBioControl::HandleResourceChange
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::HandleResourceChange( TInt aType )
    {
    if(aType == KEikDynamicLayoutVariantSwitch)
    	{
        TRAP_IGNORE(
       		CorrectFormatToRichEditorL();
        	iRichEditor->SetTextL( iToneTitle );
        	FillBitmapMaskL();
        	);
        }
    iAudioControl->HandleResourceChange( aType );
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::SizeChanged
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::SizeChanged()
    {
    TRect rect = Rect();
    TRect bitmapRect;
    TAknWindowComponentLayout layout =
	   	AknLayoutScalable_Apps::mce_image_pane_g5();
    TAknLayoutRect LayoutRect;
	LayoutRect.LayoutRect( rect, layout.LayoutLine() );
	bitmapRect = LayoutRect.Rect();
	TInt formattedHeight = iRichEditor->TextLayout()->FormattedHeightInPixels();
	bitmapRect.Move(0,formattedHeight);

	//the rectangle is grown to so that focused frame is visible
	bitmapRect.Grow( KFrameExpansion, KFrameExpansion );
	
	// rect for richtext is positioned on the top of audio control
	rect.iBr.iY = bitmapRect.iTl.iY;
	iRichEditor->SetRect( rect );
    iAudioControl->SetRect( bitmapRect );
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::FocusChanged
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::FocusChanged( TDrawNow aDrawNow )
    {
	iAudioControl->SetFocus(IsFocused());
	iRichEditor->FocusChanged(aDrawNow);
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::SetContainerWindowL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::SetContainerWindowL(
	const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );

    //The reason for creating the audio control here is that the construction
    //requires a parent with a window. So it cannot be done in ConstructL()
    iAudioControl = CMsgAudioControl::NewL( *reinterpret_cast<CMsgEditorView*>(this), this, EMdaPriorityNormal, EMdaPriorityPreferenceQuality );
	FillBitmapMaskL();

    CreateRichEditorL();
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl ::Draw()
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::Draw(const TRect& /*aRect*/) const
    {
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl ::CorrectFormatToRichEditorL()
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::CorrectFormatToRichEditorL()
    {
    CRichText* rich = iRichEditor->RichText();
    TInt pos(rich->DocumentLength());

    // font correction.
    TCharFormat format;
    TCharFormatMask mask;

	TAknTextLineLayout editorLayout = AppLayout::Smart_Messages_Line_2( 0 );
	const CFont* editorFont = AknLayoutUtils::FontFromId( editorLayout.FontId() );
	format.iFontSpec = editorFont->FontSpecInTwips();

    TRgb textColor;
    if ( AknsUtils::GetCachedColor(
	    AknsUtils::SkinInstance(),
        textColor,
        KAknsIIDQsnTextColors,
        EAknsCIQsnTextColorsCG6 ) != KErrNone )
	    {
	    textColor = AKN_LAF_COLOR_STATIC( 215 );
	    }

    format.iFontPresentation.iTextColor = textColor;

    mask.ClearAll();
    mask.SetAttrib( EAttColor );
    mask.SetAttrib(EAttFontStrokeWeight);
    mask.SetAttrib(EAttFontHeight);
    mask.SetAttrib(EAttFontTypeface);

    rich->SetInsertCharFormatL(format, mask, pos);

    // this is needed to set ringing tone title centered.
    CParaFormat* paraFormat = CParaFormat::NewLC();
    TParaFormatMask paraMask;

    paraFormat->Reset();
    paraFormat->iHorizontalAlignment = CParaFormat::ECenterAlign;
    paraFormat->iVerticalAlignment = CParaFormat::ECenterAlign;

    paraMask.SetAttrib(EAttAlignment);
    paraMask.SetAttrib(EAttFontHeight);
    paraMask.SetAttrib(EAttColor);

    rich->ApplyParaFormatL(paraFormat, paraMask, 0, pos);

    CleanupStack::PopAndDestroy(paraFormat);
    }

void CRingingToneBioControl::OpenFileL()
	{
	RFile file;

	CMsvEntry* entry = this->MsvSession(). GetEntryL(iId);
	CleanupStack::PushL( entry );

	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);

	MMsvAttachmentManager& manager = store->AttachmentManagerL();
	file = manager.GetAttachmentFileL(0);
	CleanupClosePushL(file);

	//asynchronous loading of file
	//the audio control takes a copy of the file
	iAudioControl->LoadL(file);

	//wait for the opening of file to complete
	//in MsgMediaControlStateChanged
    CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(3, entry);
	}

// ----------------------------------------------------------------------------
// CRingingToneBioControl::PlaybackL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::PlaybackL()
    {
    if( iPlayerState != EMsgAsyncControlStateOpening ||
		iPlayerState != EMsgAsyncControlStateAboutToPlay ||
		iPlayerState != EMsgAsyncControlStatePlaying ||
		iPlayerState != EMsgAsyncControlStateBuffering )
    	{
    	
    	OpenFileL();
    	
    	//any other state than EMsgAsyncControlStateReady
    	//constitutes a failed opening of media file
    	if(iPlayerState == EMsgAsyncControlStateReady)
    		{
    		GetAndSetRingingToneVolumeL();
		    iAudioControl->PlayL();

		    iStopCba = CEikButtonGroupContainer::NewL (
		    CEikButtonGroupContainer::ECba, CEikButtonGroupContainer::EHorizontal,
		        this, R_RINGCONTROL_SOFTKEYS_STOP );

		    const TSize screenSize = CCoeEnv::Static()->ScreenDevice()->SizeInPixels();
		    iStopCba->SetBoundingRect( TRect( screenSize ) );
    		}
    	}
    }

void CRingingToneBioControl::StopPlayback()
	{
	iAudioControl->Stop();
	
	//after playback the file must be closed so that moving the ring tone
	//message to another folder is possible 
	iAudioControl->Close();
	}

// ----------------------------------------------------------------------------
// CRingingToneBioControl::GetAndSetRingingToneVolumeL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::GetAndSetRingingToneVolumeL()
    {
    TInt ringingVol;
    TInt ringingType;
    TInt aMaxVolume;
	aMaxVolume = iAudioControl->MaxVolume();
	CRepository* repository = CRepository::NewLC( KCRUidProfileEngine );

	User::LeaveIfError( repository->Get( KProEngActiveRingingVolume, ringingVol ));
	User::LeaveIfError(	repository->Get( KProEngActiveRingingType, ringingType ));

	CleanupStack::PopAndDestroy(repository); //repository

	//Some emulator versions crash in volume setting
#ifndef __WINS__
    if (ringingType == EProfileRingingTypeSilent)
        {
        RDebug::Print(_L("Profile: Ring type silent"));
        iAudioControl->SetVolume(KRingVolumeZero);
        }
    else
        {
        TInt result( aMaxVolume * ringingVol / KMaxVolumeLevel ); 
        RDebug::Print(_L("Ringing volume : %d"), ringingVol);
        iAudioControl->SetVolume(result);
        }
#endif
    }



// ----------------------------------------------------------------------------
// CRingingToneBioControl::CreateRichEditorL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::CreateRichEditorL()
    {
    iRichEditor = new( ELeave ) CEikRichTextEditor(TGulBorder::ENone);

    TInt edwinFlags =
    	CEikEdwin::ENoHorizScrolling |
    	CEikEdwin::ENoAutoSelection |
        CEikEdwin::EWidthInPixels |
        CEikEdwin::EReadOnly |
        CEikEdwin::EEdwinAlternativeWrapping;

    iRichEditor->ConstructL(this,2 , 0, edwinFlags);

    iRichEditor->SetMaximumHeightInLines(2);

    // corrects font and alignment to centered.
    CorrectFormatToRichEditorL();

    iRichEditor->SetTextL( iToneTitle );    
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::FillBitmapMaskL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::FillBitmapMaskL()
    {
    
    TParse *fp = new(ELeave) TParse();
	fp->Set( KRingBCMbmFileName, &KDC_APP_BITMAP_DIR, NULL );
	CleanupStack::PushL( fp );
    
    iAudioControl->LoadIconL(  fp->FullName(),
                                KAknsIIDQgnPropOtaTone,
                                EMbmRingbcQgn_prop_ota_tone,
                                EMbmRingbcQgn_prop_ota_tone_mask);
    
    CleanupStack::PopAndDestroy(fp);
                                    
    TAknWindowComponentLayout layout =
        		AknLayoutScalable_Apps::mce_image_pane_g5();
    TAknLayoutRect LayoutRect;
	LayoutRect.LayoutRect( Rect(), layout.LayoutLine() );
	TRect bitmapRect;
	bitmapRect = LayoutRect.Rect();                                
	iAudioControl->SetIconSizeL(bitmapRect.Size());
	iAudioControl->SetIconVisible(ETrue);
    }


// ----------------------------------------------------------------------------
// CRingingToneBioControl::AddSongTitleToArrayL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::AddSongTitleToArrayL(CDesCArray& aItemArray) const
    {
    //Add titlerow to array
    HBufC* dataRow = HBufC::NewLC(KLengthOfTitleRow);
    TPtr rowPtr = dataRow->Des();
    HBufC* baseString = StringLoader::LoadLC( R_BASE_STRING );

    TInt maxLength(KLengthOfTitleRow - baseString->Length());

    HBufC* tempName = HBufC::NewLC(KMaxFileName);

    TPtr tempTPtr = tempName->Des();
    tempTPtr.Copy(iToneTitle->Des());

    if (iToneTitle->Length() > maxLength)
        {
        tempTPtr = iToneTitle->Des().Left(maxLength);
        }

    rowPtr.Format(*baseString, tempName);
    CleanupStack::PopAndDestroy( 2, baseString );  //tempName, baseString
    aItemArray.AppendL( rowPtr );
    CleanupStack::PopAndDestroy(dataRow);  //dataRow
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl ::IsOnlyDots
// ----------------------------------------------------------------------------
//
TBool CRingingToneBioControl::IsOnlyDots(TDes& aTitle)
    {
    const TChar KSingleDot = '.';
	_LIT(KDoubleDots,"..");

    if(aTitle.Length()==1)
        {

        if( aTitle.Locate(KSingleDot) != KErrNotFound )
            {
            return ETrue;
            }
        }

    else if(aTitle.Length()==2)
        {

        if( aTitle.Find(KDoubleDots) != KErrNotFound )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::IsValidL
// ----------------------------------------------------------------------------
//
TBool CRingingToneBioControl::IsValidL(TDes& aTitle, TBool aNotes)
    {
    RFs fileServer;
    User::LeaveIfError( fileServer.Connect() );

    //If name includes solely single dot or double dot, this note should be
    //showed (Folders spec)
    if( IsOnlyDots(aTitle) )
        {

        fileServer.Close();
        if(aNotes)
            {
            InformationNoteL(R_FLDR_BAD_FILE_NAME);
            }
        return EFalse;

        }

    //only because of IsValidName does not check backslash, but file
    //is not valid with it.
    if( aTitle.Locate('\\') != KErrNotFound )
        {

        fileServer.Close();
        if(aNotes)
            {
            InformationNoteL(R_FLDR_ILLEGAL_CHARACTERS);
            }
        return EFalse;

        }
    if( aTitle.Locate('\n') != KErrNotFound )
        {
        fileServer.Close();
        if(aNotes)
            {
            InformationNoteL(R_FLDR_ILLEGAL_CHARACTERS);
            }
        return EFalse;
        }

    // check is there any special chars between 00h to 1Fh
    for (TInt i=0; i<32; i++)
        {
        if (aTitle.Locate(i) !=KErrNotFound )
            {
            fileServer.Close();
            if(aNotes)
                {
                InformationNoteL(R_FLDR_ILLEGAL_CHARACTERS);
                }
            return EFalse;

            }

        }

    //Checking if filename is valid for saving purpose
    if( !fileServer.IsValidName(aTitle) )
        {

        fileServer.Close();
        if(aNotes)
            {
            InformationNoteL(R_FLDR_ILLEGAL_CHARACTERS);
            }
        return EFalse;

        }

    fileServer.Close();
    return ETrue; //File name is valid

    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::InformationNoteL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::InformationNoteL(TInt aStringResource)
    {
    HBufC* buf = StringLoader::LoadLC(aStringResource);

    CAknInformationNote* note = new (ELeave) CAknInformationNote(ETrue);
    note->ExecuteLD(*buf);

    CleanupStack::PopAndDestroy(buf); //buf
    }


// ----------------------------------------------------------------------------
// CRingingToneBioControl::ExistsL
// ----------------------------------------------------------------------------
//
TBool CRingingToneBioControl::ExistsL(TDes& aTitle)
    {
    HBufC* path = CreatePathForSavingLC(aTitle);

    if ( ConeUtils::FileExists(*path) )
        {
        CleanupStack::PopAndDestroy(path); //path;

        return ETrue;
        }

    CleanupStack::PopAndDestroy(path); //path;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::AskNameL
// ----------------------------------------------------------------------------
//
TBool CRingingToneBioControl::AskNameL(TDes& aTitle)
    {
    IncrementFileNameL( aTitle );

    if (!NameQueryL(aTitle))
        {
        return EFalse;
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::AskNameAndValidNameL
// ----------------------------------------------------------------------------
//
TBool CRingingToneBioControl::AskAndValidNameL(TDes& aTitle)
    {
    do
        {
        if(!AskNameL(aTitle))
            {
            return EFalse;
            }

        } while (!IsValidL(aTitle,KShowNotes)) ;

    return ETrue;
    }


// ----------------------------------------------------------------------------
// CRingingToneBioControl::TrySaveToneL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::TrySaveToneL()
    {
    HBufC* saveTitle = HBufC::NewLC(KMaxFileName);
    TPtr saveTitlePtr(saveTitle->Des());

    saveTitlePtr.Copy(*iToneTitle); //initialize saveTitle

    TBool okSave = ETrue; //No cancel pressed

    //First case is special, cause if message have wrong kind of title,
    //there will be default name in data query and no rename query.
    if( !IsValidL(saveTitlePtr,KShowNoNotes) )
        {
        SetDefaultNameL(saveTitlePtr);

        okSave = AskAndValidNameL(saveTitlePtr);
        }

    //Have to check if title already exists. If not, then save. But if
    //it exists already, should ask if rename (rename query) and then ask
    //for new name until user press cancel or give valid name. After that
    //we should check if that already exists...
    if(okSave)
        {
        while (ExistsL(saveTitlePtr))
            {
            if (!RenameQueryL(saveTitlePtr))
                {
                okSave = EFalse;
                break;
                }

            if(!AskAndValidNameL(saveTitlePtr))
                {
                okSave = EFalse;
                break;
                }
            };
        }

    //User did not press cancel in any place and it is saving time.
    if(okSave)
        {
        SaveToneL(saveTitlePtr);
        MsgBioUtils::ConfirmationNoteL(R_SM_NOTE_TONE_COPIED);
        }

    CleanupStack::PopAndDestroy(saveTitle); //saveTitle
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::CreatePathForSavingLC
// ----------------------------------------------------------------------------
//
HBufC* CRingingToneBioControl::CreatePathForSavingLC(const TDesC& aName)
    {
    HBufC* targetPathAndName = TargetPathLC();

    TPtr targetPathAndNamePtr = targetPathAndName->Des();

    targetPathAndNamePtr.Append(aName);
    targetPathAndNamePtr.Append(KRingingToneFileExtension);

    return targetPathAndName;
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::RenameQueryL
// ----------------------------------------------------------------------------
//
TBool CRingingToneBioControl::RenameQueryL(TDes& aTitle)
    {
    if ( !MsgBioUtils::ConfirmationQueryOkCancelL(
    	*StringLoader::LoadLC( R_FLDR_RENAME_QUERY, aTitle ) ) )
        {

        CleanupStack::PopAndDestroy(); //StringLoader;
        return EFalse;

        }
    CleanupStack::PopAndDestroy(); //StringLoader;

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::SaveToneL
// ----------------------------------------------------------------------------
//
TBool CRingingToneBioControl::SaveToneL(TDes& aTitle)
    {
    CFileMan* files = CFileMan::NewL(CCoeEnv::Static()->FsSession());
    CleanupStack::PushL( files );

    HBufC* targetPathAndName = CreatePathForSavingLC(aTitle);

    CMsvEntry* entry = this->MsvSession(). GetEntryL(iId);
    CleanupStack::PushL( entry );

    CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);

	MMsvAttachmentManager& manager = store->AttachmentManagerL();

	RFile ringFile;
	ringFile = manager.GetAttachmentFileL(0);

    CleanupStack::PopAndDestroy(2, entry);
    CleanupClosePushL(ringFile);

    TInt err = files->Copy(
    	ringFile, *targetPathAndName, KNoOverWriteOrRecurse );
    User::LeaveIfError(err);

    CleanupStack::PopAndDestroy(3, files); //targetPathAndName, ringFile, files
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::SetDefaultNameL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::SetDefaultNameL(TDes& aName)
    {
    aName.Copy(*StringLoader::LoadLC( R_SM_TONE_NAME ));
    CleanupStack::PopAndDestroy(); //stringloader
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::NameQueryL
// ----------------------------------------------------------------------------
//
TBool CRingingToneBioControl::NameQueryL(TDes& aName)
    {
    if (aName.Length() > KTextNameQueryMaxLength)
        {
        User::Leave(KErrOverflow);
        }

    CAknTextQueryDialog* queryDlg = new ( ELeave )
                                     CAknTextQueryDialog( aName );
    return queryDlg->ExecuteLD( R_SM_QUERY_RINGING_TONE );
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl::TakeTitleL
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::TakeTitleL()
    {
    //Read stream from file

    RFile ringFile;

    CMsvEntry* entry = this->MsvSession(). GetEntryL(iId);
    CleanupStack::PushL( entry );

    CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);

	MMsvAttachmentManager& manager = store->AttachmentManagerL();
	ringFile = manager.GetAttachmentFileL(0);

    CleanupStack::PopAndDestroy(2,entry);    //entry, store
    CleanupClosePushL(ringFile);


    TInt filelen;
    User::LeaveIfError(ringFile.Size(filelen));

    HBufC8* filedata8 = HBufC8::NewMaxLC(filelen);
    TPtr8 dataPtr8 = filedata8->Des();
    User::LeaveIfError(ringFile.Read(dataPtr8,filelen));

    CleanupStack::Pop( filedata8 ); // pop the filedata temporarily
    CleanupStack::PopAndDestroy(); // close and destory ringFile
    CleanupStack::PushL( filedata8 ); // put the filedata back

    CRingBCNSMConverter* conv = CRingBCNSMConverter::NewL();
    CleanupStack::PushL( conv );

	if ( iToneTitle )
		{
		delete iToneTitle;
		iToneTitle = NULL;
		}
	iToneTitle = conv->TitleLC(dataPtr8); //The ownership changes here

    CleanupStack::Pop(iToneTitle);
    CleanupStack::PopAndDestroy(2, filedata8);  //conv, filedata8

    // checks if there is no name for ringing tone
    if (iToneTitle->Length() == 0)
        {
        delete iToneTitle;
        iToneTitle = NULL;

        // sets default name
        iToneTitle = StringLoader::LoadL( R_SM_TONE_NAME );

        // check that default name is not too long.
        if (iToneTitle->Length() > KMaxFileName )
            {

            HBufC* tempName = HBufC::NewLC(KMaxFileName);
            TPtr tempTPtr = tempName->Des();

            tempTPtr.Copy(iToneTitle->Des().Left(KMaxFileName));

            CleanupStack::Pop(tempName);

	        delete iToneTitle;
            iToneTitle = tempName;
            }
        }
    }
// ----------------------------------------------------------------------------
// CRingingToneBioControl::IncrementFileName
// ----------------------------------------------------------------------------
//
void CRingingToneBioControl::IncrementFileNameL(TDes& aFileName)
    {
    HBufC* path = CreatePathForSavingLC(aFileName);

    while (ConeUtils::FileExists(*path))
        {

        MsgBioUtils::IncrementNameL(aFileName,KMaxFileName);

        CleanupStack::PopAndDestroy(path);  //path
        path = CreatePathForSavingLC(aFileName);

        }

    CleanupStack::PopAndDestroy(path); //path;
    }


// ----------------------------------------------------------------------------
// CRingingToneBioControl ::TargetPathLC
// ----------------------------------------------------------------------------
//
HBufC* CRingingToneBioControl::TargetPathLC()
    {
	TFileName path = PathInfo::PhoneMemoryRootPath();
	path.Append( PathInfo::SimpleSoundsPath() );
    ConeUtils::EnsurePathExistsL(path);
    HBufC* savePath = HBufC::NewLC(KMaxPath + KMaxFileName);
	TPtr ptr = savePath->Des();
	ptr = path;
	return savePath;
    }

// ----------------------------------------------------------------------------
// CRingingToneBioControl ::VirtualHeight
// ----------------------------------------------------------------------------
//
TInt CRingingToneBioControl::VirtualHeight()
	{
    return 0;
	}

// ----------------------------------------------------------------------------
// CRingingToneBioControl ::VirtualVisibleTop
// ----------------------------------------------------------------------------
//
TInt CRingingToneBioControl::VirtualVisibleTop()
	{
    return 0;
	}

void CRingingToneBioControl::MsgAsyncControlStateChanged( CMsgBaseControl& /*aControl*/,
                                              TMsgAsyncControlState aNewState,
                                              TMsgAsyncControlState aOldState )
    {
    iPlayerState = aNewState;
    
    if(aOldState == EMsgAsyncControlStateOpening)
		{
		//last state was opening so we go ahead with playing by stopping 
		//the active scheduler that was started in OpenFileL -function
		CActiveScheduler::Stop();
		}
    
    switch( aNewState )
    	{
			//intentional fallthrough    			
			case EMsgAsyncControlStateIdle:
    		case EMsgAsyncControlStateStopped:
    		case EMsgAsyncControlStateCorrupt:
    		case EMsgAsyncControlStateNoRights:
    			{
    			//the Stop CBA has to be removed here because
    			//playing could be stopped when the tune has finished or
    			//the tune is corrupt
    			delete iStopCba;
    			iStopCba = NULL;
    			break;
    			}
    	}
    }


#ifdef RD_SCALABLE_UI_V2    
void CRingingToneBioControl::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {


    if ( AknLayoutUtils::PenEnabled() )
        {
        switch ( aPointerEvent.iType )
            {
            case TPointerEvent::EButton1Down:
            	{
            	//nothing to do here
            	}
            break;
            case TPointerEvent::EButton1Up:
                {
				TBool hitted = iAudioControl->HitRegionContains( aPointerEvent.iPosition, *this );
                if( hitted )
                    {
                        if ( iPlayerState != EMsgAsyncControlStatePlaying )
                            {
                            //playback is triggered when focused control is touched with stylus
                            iAudioControl->SetFocus(EFalse);
                            PlaybackL();
                            }
                        else if ( iPlayerState == EMsgAsyncControlStatePlaying )
                            {
                            //stylus hit to audio control when playback is in 
                            //progress stops the playing 
                            StopPlayback();
                            }
                    }
            	}
              break;
            default:
                break;
            }
        }
    }
#else   
void CRingingToneBioControl::HandlePointerEventL( const TPointerEvent& /*aPointerEvent*/ )
    {
    //no op
    }
#endif // RD_SCALABLE_UI_V2    

void CRingingToneBioControl::MsgAsyncControlResourceChanged( CMsgBaseControl& /*aControl*/,
											 				 TInt /*aType*/ )
	{
	//no op
	}
//  End of File
