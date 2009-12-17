/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Bio control for Business Cards.
*
*/



// INCLUDE FILES
#include "VCardBioControl.h"        // for CVCardBioControl
#include "vcardvpbutil.h"
#include "VCardLog.h"				// Logging functionality

#include <eikon.hrh>                // for EikEdwin defines
#include <eiklabel.h>               // for CEikLabel
#include <eikenv.h>                 // for eikenv
#include <eikbtgpc.h>               // for CEikButtonGroupContainer
#include <e32base.h>
#include <eikmenup.h>
#include <eikmenub.h>
#include <avkon.hrh>
#include <aknnotedialog.h>          // for CAknNoteDialog
#include <gulicon.h>                // for CGulIcon
#include <eikclbd.h>
#include <bioscmds.h>
#include <msvapi.h>                 // for CMsvSession
#include <s32file.h>                // for RFileReadStream
#include <msgbiocontrolObserver.h>  // for MMsgBioControlObserver
#include <StringLoader.h>           // StringLoader
#include <vcardbc.rsg>              // resouce identifiers
#include <CRichBio.h>               // CRichBio
#include <CPbkFieldInfo.h>
#include <MsgBioUtils.h>
#include <MsgSmsViewer.hrh> // ESmsViewerCreateCCNew, ESmsViewerCreateCCExisting
#include <sysutil.h>
#include <featmgr.h>
#include <bldvariant.hrh>
#include <csxhelp/smart.hlp.hrh>
#include <mmsvattachmentmanager.h>
#include <AknDef.h>
#include <AknUtils.h>
#include <PbkView.rsg>
#include <akninputblock.h>

//Phonebook 2 API
#include <CPbk2StoreConfiguration.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactManager.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkVCardEng.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <VPbkContactStoreUris.h>

// LOCAL CONSTANTS AND MACROS

_LIT(KResourceFile, "vcardbc.rsc");
_LIT(KPbk2CommonUi,"Pbk2CommonUi.rsc");
_LIT(KPbk2UIControls,"Pbk2UIControls.rsc");
_LIT(KClassName, "CVCardBioControl");

const TInt KMenuPos = 1; //The inserting position of menu item.

// Compact Business Card Bio UID
const TUid KUidBioBusinessCard = {0x10005531};

const TInt KVcdBcHeightReductionBva = 9;

// CLASS-SPECIFIC PANIC CODES
enum TPanicCode
    {
    ENullObject1,
    ENullObject2,
    ENullObject3,
    ENullObject4,
    ENullObject5,
    ENullObject6,
    ENullObject7,
    EUndefinedEnum,
    EInvalidIndex,
    EPanicPostCond_ReallocBufferL
    };

//  MEMBER FUNCTIONS

// Two-phased constructor.
EXPORT_C CMsgBioControl* CVCardBioControl::NewL(
    MMsgBioControlObserver& aObserver,
    CMsvSession* aSession,
    TMsvId aId,
    TMsgBioMode aEditorOrViewerMode,
    const RFile* aFile)
    {
    LOG("CVCardBioControl::NewL begin");
    CVCardBioControl* self = new(ELeave) CVCardBioControl(
        aObserver,
        aSession,
        aId,
        aEditorOrViewerMode,
        aFile);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    LOG("CVCardBioControl::NewL end");
    return self;
    }

CVCardBioControl::~CVCardBioControl()
    {
    FeatureManager::UnInitializeLib();
    delete iViewer;
    delete iVpbUtil;
    }

void CVCardBioControl::SetAndGetSizeL(TSize& aSize)
    {
    LOG("CVCardBioControl::SetAndGetSizeL begin");
    __ASSERT_DEBUG(iViewer, Panic(ENullObject1));

    if(iIsFileBased)
        {
        SetPosition(TPoint(0,KVcdBcHeightReductionBva));
        aSize.iHeight -= KVcdBcHeightReductionBva;
        iViewer->SetAndGetSizeL(aSize);
        SetSizeWithoutNotification(aSize);
        }
    else
        {
        iViewer->SetAndGetSizeL(aSize);

        //the following compares the window size against the
        //"virtual" height of the viewer. This way there is no
        //scrolling if virtualHeight is less than window size
        if(iViewer->VirtualHeight() <= Window().Size().iHeight)
            {
            aSize.iHeight = iViewer->VirtualHeight();
            SetSizeWithoutNotification(aSize);
            }
        else
            {
            SetSizeWithoutNotification(aSize);
            }
        }

    LOG("CVCardBioControl::SetAndGetSizeL end");
    }

void CVCardBioControl::SetMenuCommandSetL(CEikMenuPane& aMenuPane)
    {
    LOG("CVCardBioControl::SetMenuCommandSetL begin");
    if (!IsEditor())
        {
        if (iIsFileBased)
            {
            FileBasedAddMenuItemL(aMenuPane, R_QTN_SM_ADD_PHONEBOOK,
                EAddToPhonebook);
            }
        else
            {
            AddMenuItemL(aMenuPane, R_QTN_SM_ADD_PHONEBOOK,
            EAddToPhonebook, KMenuPos);
            }
        }
    LOG("CVCardBioControl::SetMenuCommandSetL end");
    }

TBool CVCardBioControl::HandleBioCommandL(TInt aCommand)
    {
    if (aCommand == iBioControlObserver.FirstFreeCommand()
        + EAddToPhonebook)
        {
        CAknInputBlock* blocker = CAknInputBlock::NewLC();
        AddToPhoneBookL();
        CleanupStack::PopAndDestroy( blocker );
        return ETrue;
        }
    return EFalse;
    }

TRect CVCardBioControl::CurrentLineRect() const
    {
    return iViewer->CurrentLineRect();
    }

TBool CVCardBioControl::IsFocusChangePossible(
    TMsgFocusDirection aDirection) const
    {
    if (aDirection == EMsgFocusUp)
        {
        return iViewer->IsCursorLocation(EMsgTop);
        }
    return EFalse;
    }

HBufC* CVCardBioControl::HeaderTextL() const
    {
    return StringLoader::LoadL(R_QTN_SM_TITLE_BUSINESS_CARD, iCoeEnv);
    }

TInt CVCardBioControl::VirtualHeight()
    {
    __ASSERT_DEBUG(iViewer, Panic(ENullObject2));
    return iViewer->VirtualHeight();
    }

TInt CVCardBioControl::VirtualVisibleTop()
    {
    __ASSERT_DEBUG(iViewer, Panic(ENullObject3));
    return iViewer->VirtualVisibleTop();
    }

TBool CVCardBioControl::IsCursorLocation(TMsgCursorLocation aLocation) const
    {
    return iViewer->IsCursorLocation(aLocation);
    }

TKeyResponse CVCardBioControl::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    __ASSERT_DEBUG(iViewer, Panic(ENullObject4));
    return iViewer->OfferKeyEventL(aKeyEvent, aType);
    }

void CVCardBioControl::GetHelpContext(TCoeHelpContext& aHelpContext) const
    {
    LOG("CVCardBioControl::GetHelpContext begin");
    const TUid KUidSmart = {0x101F4CDA};
    if (!iIsFileBased)
        {
        aHelpContext.iContext=IsEditor()?
            KSMART_HLP_BUSINESSEDITOR():KSMART_HLP_BUSINESSVIEWER() ;
        aHelpContext.iMajor=KUidSmart;
        }
    else
        {
        if ( !IsEditor() )
            {
            aHelpContext.iContext=KSMART_HLP_BUSPULLMESVIEWER;
            aHelpContext.iMajor=KUidSmart;
            }
        }
    LOG("CVCardBioControl::GetHelpContext end");
    }

void  CVCardBioControl::SetDimmed(TBool aDimmed)
    {
    LOG("CVCardBioControl::SetDimmed begin");
    CCoeControl::SetDimmed(aDimmed);
    LOG("CVCardBioControl::SetDimmed end");
    }

TInt CVCardBioControl::CountComponentControls() const
    {
    return 1; // just the iViewer
    }

CCoeControl* CVCardBioControl::ComponentControl(TInt aIndex) const
    {
    if (aIndex == 0)
        {
        return iViewer;
        }
    return NULL;
    }

void CVCardBioControl::SizeChanged()
    {
    LOG("CVCardBioControl::SizeChanged begin");
    __ASSERT_DEBUG(iViewer, Panic(ENullObject5));
    iViewer->SetExtent(Position(), iViewer->Size());
    LOG("CVCardBioControl::SizeChanged end");
    }

void CVCardBioControl::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    LOG("CVCardBioControl::FocusChanged begin");
    __ASSERT_DEBUG(iViewer, Panic(ENullObject6));
    iViewer->SetFocus(IsFocused());
    LOG("CVCardBioControl::FocusChanged end");
    }

void CVCardBioControl::SetContainerWindowL(const CCoeControl& aContainer)
    {
    LOG("CVCardBioControl::SetContainerWindowL begin");
    CCoeControl::SetContainerWindowL(aContainer);

    // The reason for creating the viewer control here is that the
    // construction requires a parent with a window. So it cannot be done in
    // ConstructL().

    iViewer = CRichBio::NewL(this, ERichBioModeEditorBase);
    AddVPbkFieldsToViewerL(*iVpbContact);
    LOG("CVCardBioControl::SetContainerWindowL end");
    }

CVCardBioControl::CVCardBioControl(
    MMsgBioControlObserver& aObserver,
    CMsvSession* aSession,
    TMsvId aId,
    TMsgBioMode aEditorOrViewerMode,
    const RFile* aFile
    ):
        CMsgBioControl(aObserver,
            aSession,
            aId,
            aEditorOrViewerMode,
            aFile)
    {
    }

// Symbian OS default constructor can leave.
void CVCardBioControl::ConstructL()
    {
    LOG("CVCardBioControl::ConstructL begin");

    SetExtension( this );
    iIsFileBased = IsFileBased();

    LoadResourceL(KResourceFile);
    LoadResourceL(KPbk2CommonUi);
    LoadResourceL(KPbk2UIControls);
    LoadStandardBioResourceL();

    TVCardBCBusinessCardType type;

    // file handle to the vcard attachment in msv store
    RFile fileHandle;
    ResolveFileHandleAndTypeL( fileHandle, type );
    CleanupClosePushL( fileHandle );

    iVpbUtil = CVCardVpbUtil::NewL();

    FeatureManager::InitializeLibL();

    // Now do the importing
    //
    if (type == EVCard )
        {
        CleanupStack::Pop( &fileHandle );
        //after this call the fileHandle will no longer be valid
        TRAPD(err,iVpbUtil->ImportVCardL(EVCard,fileHandle));
        if (err != KErrNone)
            {
            User::Leave(KErrMsgBioMessageNotValid);
            }

        if (iVpbUtil->IsContactItemEmpty())
            {
            User::Leave(KErrMsgBioMessageNotValid);
            }

        iVpbContact = iVpbUtil->ContactData();
        }
    else if(type == ECompactBusinessCard)
        {
        CleanupStack::Pop( &fileHandle );
        //after this call the fileHandle will no longer be valid
        TRAPD(err,iVpbUtil->ImportVCardL(ECompactBusinessCard,fileHandle));
        if(err != KErrNone)
            {
            User::Leave(KErrMsgBioMessageNotValid);
            }

        iVpbContact = iVpbUtil->ContactData();

        if (iVpbUtil->IsContactItemEmpty())
            {
            User::Leave(KErrMsgBioMessageNotValid);
            }
        }
    else
        {
        __ASSERT_DEBUG(EFalse, Panic(EUndefinedEnum));
        }
    LOG("CVCardBioControl::ConstructL end");
    }

void CVCardBioControl::AddToPhoneBookL()
    {
    LOG("CVCardBioControl::AddToPhoneBookL begin");
    RFs fs;
    TInt connVal = fs.Connect();
    User::LeaveIfError(connVal);
    CleanupClosePushL( fs );

	// Check whether there's free space in flash mem to save the contact
    if ( SysUtil::FFSSpaceBelowCriticalLevelL( &fs ))
        {
        User::Leave( KErrDiskFull );
        }
    CleanupStack::PopAndDestroy(); // fs

    TBool vcardStored(EFalse);
    vcardStored = iVpbUtil->CommitVCardToStoreL();

    if( vcardStored )
        {
        MsgBioUtils::ConfirmationNoteL( R_QTN_BCARD_SAVED_TO_PB_AS_NEW );
        }
    LOG("CVCardBioControl::AddToPhoneBookL end");
    }

void CVCardBioControl::ResolveFileHandleAndTypeL(
    RFile& aFile,
    TVCardBCBusinessCardType& aBCType )
    {
    LOG("CVCardBioControl::ResolveFileHandleAndTypeL begin");
    if (iIsFileBased)
        {
        // Note that file based Compact Business Cards are not supported.
        // (The stand alone Bio Viewer Application will not be registered to
        // handle CBC mime types)
        aBCType = EVCard;
        aFile.Duplicate(FileHandle());
        }
    else
        {
        CMsvEntry* entry = MsvSession().GetEntryL( iId );
        if (entry->Entry().iBioType == KUidBioBusinessCard.iUid)
	        {
            aBCType = ECompactBusinessCard; //Bio type of COMPACT Business card
            }
        else
            {
            aBCType = EVCard; // It is a vCard
            }

        CleanupStack::PushL( entry );
        CMsvStore* store = entry->ReadStoreL();
        CleanupStack::PushL(store);
        MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
        aFile = attachMan.GetAttachmentFileL( 0 ); //card is the firstattachment
        CleanupStack::PopAndDestroy( 2 ); // store, entry
        }
    LOG("CVCardBioControl::ResolveFileHandleAndTypeL end");
    }

void CVCardBioControl::AddVPbkFieldsToViewerL(const MVPbkStoreContact& aContact)
    {
    LOG("CVCardBioControl::AddPbkFieldsToViewerL begin");

    const MVPbkStoreContactFieldCollection& fields = aContact.Fields();
    TInt count = fields.FieldCount();

    for (TInt n(0); n < count; n++)
        {
        const MVPbkStoreContactField& field = fields.FieldAt(n);
        if (FieldToBeShown(field))
            {
            TPtrC text(iVpbUtil->FormatFieldContentL(field));
            if (text.Length() && field.FieldLabel().Length())
                {
                iViewer->AddItemL(field.FieldLabel(), text);
                }
            }
        }
    LOG("CVCardBioControl::AddPbkFieldsToViewerL end");
    }

TBool CVCardBioControl::FieldToBeShown(const MVPbkStoreContactField& aField)
    {
    LOG("CVCardBioControl::FieldToBeShown begin");
    if (aField.FieldData().DataType() == EVPbkFieldStorageTypeBinary ||
        aField.FieldData().DataType() == EVPbkFieldStorageTypeNull)
        {
        return EFalse;
        }
    LOG("CVCardBioControl::FieldToBeShown end");
    return ETrue;
    }

void CVCardBioControl::FileBasedAddMenuItemL(CEikMenuPane& aMenuPane,
    TInt aStringRes, TInt aCommandOffset)
    {
    LOG("CVCardBioControl::FileBasedAddMenuItemL begin");
    CEikMenuPaneItem::SData menuItem;
    menuItem.iCascadeId = NULL;
    menuItem.iFlags = NULL;
    HBufC* menuItemText(StringLoader::LoadLC(aStringRes, iCoeEnv));
    TPtr textPtr = menuItemText->Des();
    if(textPtr.Length() > menuItem.iText.MaxLength())
        {
        menuItem.iText.Format(textPtr.MidTPtr(0,menuItem.iText.MaxLength()) );
        }
    else
        {
        menuItem.iText.Format(textPtr);
        }
    CleanupStack::PopAndDestroy(menuItemText);
    menuItem.iCommandId = iBioControlObserver.FirstFreeCommand()
        + aCommandOffset;
    aMenuPane.InsertMenuItemL(menuItem, 0);
    LOG("CVCardBioControl::FileBasedAddMenuItemL end");
    }

void CVCardBioControl::OpenStreamLC(RFileReadStream& aStream,
    TFileName& aFileName)
    {
    LOG("CVCardBioControl::OpenStreamLC begin");
    User::LeaveIfError( aStream.Open(
        CCoeEnv::Static()->FsSession(),
        aFileName,
        EFileStream ));
    aStream.PushL();
    LOG("CVCardBioControl::OpenStreamLC end");
    }

void CVCardBioControl::HandleResourceChange( TInt aType )
    {
    LOG("CVCardBioControl::HandleResourceChange begin");
    CMsgBioControl::HandleResourceChange( aType );
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iViewer->Reset();
        }
    LOG("CVCardBioControl::HandleResourceChange end");
    }

TAny* CVCardBioControl::BioControlExtension(TInt aExtensionId)
    {
    if ( aExtensionId == KMsgBioControlScrollExtension )
        {
        return static_cast<MMsgBioControlScrollExtension*> (this);
        }
    else
        {
        return NULL;
        }
    }

TInt CVCardBioControl::ExtScrollL( TInt aPixelsToScroll,
                                        TMsgScrollDirection aDirection )
    {
    return iViewer->ScrollL(aPixelsToScroll, aDirection);
    }

void CVCardBioControl::ExtNotifyViewEvent( TMsgViewEvent aEvent, TInt aParam )
    {
    iViewer->NotifyViewEvent( aEvent, aParam );
    }

void CVCardBioControl::Panic(TInt aReason)
    {
    User::Panic(KClassName, aReason);
    }
//  End of File
