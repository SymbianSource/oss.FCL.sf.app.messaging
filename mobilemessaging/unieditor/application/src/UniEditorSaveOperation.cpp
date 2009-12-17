/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides CUniEditorSaveOperation methods.
*
*/



// ========== INCLUDE FILES ================================

#include <cmsvattachment.h>

#include <txtrich.h>
#include <eikrted.h>

#include <MuiuMsvUiServiceUtilities.h>
#include <StringLoader.h>

#include <MsgEditorView.h>
#include <MsgBodyControl.h>
#include <MsgExpandableControl.h>
#include <MsgBaseControl.h>
#include <msgimagecontrol.h>

#include <unimsventry.h>

// MsgMedia stuff
#include <MsgMimeTypes.h>
#include <MsgMediaInfo.h>
#include <MsgTextInfo.h>
#include <MsgImageInfo.h>

// UniModel + UniUtils
#include <unidatautils.h>
#include <unidatamodel.h>
#include <unismilmodel.h>
#include <unismillist.h>
#include <uniobjectlist.h>
#include <uniaddresshandler.h>

#include <UniEditor.rsg>

#include "UniPluginApi.h"
#include "UniClientMtm.h"

#include "UniSendingSettings.h"
#include "UniEditorHeader.h"
#include "UniEditorDocument.h"
#include "UniEditorSaveOperation.h"
#include "UniEditorAppUi.h" 
// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

const TInt  KMaxDetailsLength   = 64;   // Copy max this many chars to TMsvEntry iDetails
const TInt  KUniMaxDescription  = 64;

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniEditorSaveOperation::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CUniEditorSaveOperation* CUniEditorSaveOperation::NewL( 
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        CUniEditorHeader& aHeader,
        CMsgEditorView& aView,
        RFs& aFs )
    {
    CUniEditorSaveOperation* self = 
        new ( ELeave ) CUniEditorSaveOperation( aObserver, aDocument, aHeader, aView, aFs );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::CUniEditorSaveOperation
//
// Constructor.
// ---------------------------------------------------------
//
CUniEditorSaveOperation::CUniEditorSaveOperation(
        MUniEditorOperationObserver& aObserver,
        CUniEditorDocument& aDocument,
        CUniEditorHeader& aHeader,
        CMsgEditorView& aView,
        RFs& aFs ) :
    CUniEditorOperation( aObserver, aDocument, aFs, EUniEditorOperationSave ),
    iHeader( aHeader ),
    iView( aView )
    {
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::ConstructL()
    {
    BaseConstructL();
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::~CUniEditorSaveOperation
//
// Destructor.
// ---------------------------------------------------------
//
CUniEditorSaveOperation::~CUniEditorSaveOperation()
    {
    Cancel();
    delete iDom;
    delete iEditStore;
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::Save
//
// SaveL
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::Save( TUniSaveType aSaveType )
    {
    ResetErrors();
    
    iSavedObject = NULL;
    iSaveType = aSaveType;
    
    iOperationState = EUniEditorSaveCheck;
        
    CompleteSelf( KErrNone );
    }


// ---------------------------------------------------------
// CUniEditorSaveOperation::SaveL
//
// SaveL
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::SaveL( CUniObject& aObject )
    {
    if ( !MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL( iDocument.Session(),
                                                               aObject.Size( ETrue ) ) )
        {
        iSavedObject = &aObject;
        iSavedObject->Save( *this, CMsvAttachment::EMsvFile );
        SetPending();
        }
    else
        {
        User::Leave( KErrDiskFull );
        }
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::CheckDiskSpaceL
// ---------------------------------------------------------
//
TBool CUniEditorSaveOperation::CheckDiskSpaceL()
    {
    // First calculate disk space needed for new attas
    TInt spaceNeeded = iDocument.DataModel()->ObjectList().SpaceNeededForSaveAll();
    spaceNeeded += iDocument.DataModel()->AttachmentList().SpaceNeededForSaveAll();

    if ( iComposeSmil )
        {
        spaceNeeded += iDocument.DataModel()->SmilModel().SmilComposeSize();
        }

    // Disk space check
    return !( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL( 
                iDocument.Session(),
                spaceNeeded ) );
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::DoSaveStepL
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::DoSaveStepL()
    {
    switch ( iOperationState )
        {
        case EUniEditorSaveCheck:
            {
            DoSaveChecksL();
            break;
            }
        case EUniEditorSaveObjects:
            {
            DoSaveObjectL();
            break;
            }
        case EUniEditorSaveAttachment:
            {
            DoSaveAttachments();
            break;
            }
        case EUniEditorSaveSmilCompose:
            {
            DoComposeSmilL();
            break;
            }
        case EUniEditorSaveFinalize:
            {
            DoFinalizeSaveL();
            break;
            }
        case EUniEditorSavePlugins:
            {
            DoSavePluginsL();
            break;
            }
        case EUniEditorSaveEnd:
            {
            if ( iEditStore )
                {
                iEditStore->CommitL();
                delete iEditStore;
                iEditStore = NULL;
                }
            iObserver.EditorOperationEvent(
                EUniEditorOperationSave,
                EUniEditorOperationComplete );
            break;
            }
       case EUniEditorSaveError:
	       {
       		if(iEditStore)
       			{
       			delete iEditStore;
	            iEditStore = NULL;
	            }
	            static_cast<CUniEditorAppUi*>(CEikonEnv::Static()->AppUi())->ExitWithoutSave();
	       	}
        default:
            {
            // Shouldn't be here!
            delete iEditStore;
            iEditStore = NULL;
            iObserver.EditorOperationEvent(
                EUniEditorOperationSave,
                EUniEditorOperationError );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::DoSaveChecksL
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::DoSaveChecksL()
    {
    if ( !iDocument.Modified() &&
        iSaveType <= iDocument.PrevSaveType() )
        {
        iOperationState = EUniEditorSaveEnd;
        CompleteSelf( KErrNone );
        //nothing to do!
        return;
        }

    iOperationState = EUniEditorSaveEnd; // initialize state in case CheckDiskSpaceL leaves
    if ( CheckDiskSpaceL() )
        {
        iOperationState = EUniEditorSaveObjects;
        // TODO: Open edit store already here
        //iEditStore = iDocument.Mtm().Entry().EditStoreL();

        iHeader.CopyHeadersToMtmL( iDocument.HeaderModified(),
                                   EFalse );

        iComposeSmil = EFalse;
        if ( iDocument.DataModel()->SmilType() == EMmsSmil )
            {
            //ReleaseImage( ETrue );
            if ( /* iDocument.BodyModified() */
                !iDocument.DataModel( )->ObjectList().Count() <= 0 ||
                ( !IsForward() && iSaveType == ESendingSave ) )
                {
                iComposeSmil = ETrue;
                }
            if ( iComposeSmil ||
                iSaveType > iDocument.PrevSaveType() )
                {
                if ( IsForward() )
                    {
                    iDocument.DataModel()->ObjectList().EnsureAllObjectsHaveContentLocationL();
                    // TODO: Pass edit store as a parameter to objectlist
                    //iDocument.DataModel()->ObjectList().EnsureAllObjectsHaveContentLocationL( *iEditStore );
                    }
                }
            }
        }
    else
        {
        SetError( KErrDiskFull );
        iOperationState = EUniEditorSaveEnd;
        }
      
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::DoSaveObjectL
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::DoSaveObjectL()
    {
    if ( iDocument.BodyModified() )
        {
        // Processing will continue from CUniEditorSaveOperation::ObjectSaveReady when
        // save has been performed
        iDocument.DataModel()->ObjectList().SaveAll( *this, CMsvAttachment::EMsvFile );    
        // TODO: Pass edit store as a parameter to objectlist
        //iDocument.DataModel()->ObjectList().SaveAll( *this, *iEditStore, CMsvAttachment::EMsvFile );    
        SetPending();
        }
    else
        {
        iOperationState = EUniEditorSaveAttachment;
        CompleteSelf( KErrNone );
        }
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::DoSaveAttachments
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::DoSaveAttachments()
    {
    // Processing will continue from CUniEditorSaveOperation::ObjectSaveReady when
    // save has been performed
    iDocument.DataModel()->AttachmentList().SaveAll( *this, CMsvAttachment::EMsvFile );    
    // TODO: Pass edit store as a parameter to objectlist
    //iDocument.DataModel()->AttachmentList().SaveAll( *this, *iEditStore, CMsvAttachment::EMsvFile );    
    SetPending();
    }

// ---------------------------------------------------------
// CMmsDocument::DoSaveStepL
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::DoComposeSmilL()
    {
    // TODO: Open edit store already in "save checks"
    // Open store even if we didn't need it here.
    // -> We don't need to check later whether the store is open or not.
    iEditStore = iDocument.Mtm().Entry().EditStoreL();
    if ( iComposeSmil )
        {
        iDocument.DataModel()->SmilList().RemoveSmilL( *iEditStore );
        if ( iSaveType == ESendingSave )
            {
            iDocument.DataModel()->SmilModel().RemoveEmptySlides();
            }
            
        if ( iDocument.DataModel()->SmilModel().SlideCount() != 0 )
            {
            // Model is not empty -> Compose new SMIL
            iDom = iDocument.DataModel()->SmilModel().ComposeL();
            iDocument.DataModel()->SmilList().CreateSmilAttachmentL( *this, *iEditStore, iDom );
            SetPending();
            return;
            }
        }
        
    iOperationState = EUniEditorSaveFinalize;
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CMmsDocument::DoFinalizeSaveL
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::DoFinalizeSaveL()
    {
    // Set new state immediately in case some function leaves.
    iOperationState = EUniEditorSavePlugins;

    // Set message root after composing SMIL but before calling SaveMessageL
    TMsvAttachmentId root( 0 );  
    if ( iDocument.DataModel()->SmilList().Count() )
        {
        root = iDocument.DataModel()->SmilList().GetSmilAttachmentByIndex( 0 );
        }
    iDocument.Mtm().SetMessageRoot( root );


    TMsvEntry tEntry = iDocument.Entry();
    iDocument.Mtm().SaveMessageL( *iEditStore, tEntry );
    
    HBufC* description = CreateDescriptionL();
    if ( description )
        {
        tEntry.iDescription.Set( *description );
        CleanupStack::PushL( description );
        }

    // Set details
    TBuf<KMaxDetailsLength> detailsBuf; 
    iHeader.MakeDetailsL( detailsBuf );
    tEntry.iDetails.Set( detailsBuf );

    if ( iComposeSmil && IsForward() )
        {
        //New SMIL composed. This is a normal message from now on.
        TUniMsvEntry::SetForwardedMessage( tEntry, EFalse );
        }

    TUniMsvEntry::SetEditorOriented( tEntry, ETrue );
    TUniMsvEntry::SetCharSetSupport( tEntry, iDocument.CharSetSupport() );

    TUniMessageCurrentType curType = EUniMessageCurrentTypeSms;
    
    if( iDocument.UniState() == EUniMms )
        {
        curType = EUniMessageCurrentTypeMms;
        
        TUniSendingSettings settings;
        
        if( iDocument.UniState() == EUniMms )
            {
            CUniEditorPlugin* mmsPlugin = iDocument.MmsPlugin();
            
            if ( mmsPlugin )
                {
                mmsPlugin->GetSendingSettingsL( settings );
                switch ( settings.iPriority )
                    {
                    case TUniSendingSettings::EUniPriorityHigh:
                        {
                        tEntry.SetPriority( EMsvHighPriority );
                        break;
                        }
                    case TUniSendingSettings::EUniPriorityLow:
                        {
                        tEntry.SetPriority( EMsvLowPriority );
                        break;
                        }
                    default:
                        {
                        tEntry.SetPriority( EMsvMediumPriority );
                        break;
                        }
                    }
                }
            else
                {
                // Should not happen but better to handle anyway.
                tEntry.SetPriority( EMsvMediumPriority );
                }
            }
        }
    else
        { 
        // Message is sms -> remove priority
        tEntry.SetPriority( EMsvMediumPriority );
        }

    TUniMsvEntry::SetCurrentMessageType( tEntry, curType );

    tEntry.SetAttachment( EFalse );

    if ( iSaveType >= EClosingSave )
        {
        // Update timestamp  
        tEntry.iDate.UniversalTime();

        if ( !tEntry.Visible() )
            {
            // Save from close or exit save.
            // Message should be visible after save
            tEntry.SetVisible( ETrue );
            tEntry.SetInPreparation( EFalse );
            }
        }
    else 
        {
        // Save made because user launched Message Info, Preview or Objects view 
        // Message should stay invisible and in preparation if it was.
        if ( !tEntry.Visible() )
            {
            // Guarantee that when Visible is ETrue InPreparation is EFalse 
            tEntry.SetInPreparation( ETrue );
            }
        }



    iDocument.CurrentEntry().ChangeL( tEntry );
    
    if ( description )
        {
        // This must not be popped before ChangeL
        CleanupStack::PopAndDestroy( description );
        }

    iDocument.SetHeaderModified( EFalse );
    iDocument.SetBodyModified( EFalse );
    iDocument.SetPrevSaveType( iSaveType );

    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::CreateDescriptionL
//
// Creates description buffer. Returns NULL if description is not
// wanted to be updated.
// ---------------------------------------------------------
//
HBufC* CUniEditorSaveOperation::CreateDescriptionL()
    {
    CUniDataModel* model = iDocument.DataModel();

    if( iDocument.UniState() == EUniSms )
        {
        if( model->ObjectList().Count() == 0 &&
            model->AttachmentList().Count() == 1 )
            { 
            // No objects and one attachment
            CUniObject* obj = model->AttachmentList().GetByIndex( 0 );
            if (    obj &&  obj->MimeType().Length() > 0 )
                {
                TInt res = 0;
                if( obj->MimeType().CompareF( KMsgMimeVCard ) == 0 )
                    { 
                    // There's only one VCard
                    res = R_UNIEDITOR_BIOTYPE_VCARD;
                    }
                else if( obj->MimeType().CompareF( KMsgMimeVCal ) == 0 ||
                         obj->MimeType().CompareF( KMsgMimeICal ) == 0 )
                    { 
                    // There's only one VCal
                    res = R_UNIEDITOR_BIOTYPE_VCAL;
                    }
                if( res )
                    {
                    return StringLoader::LoadL( res, iView.ControlEnv() );
                    }
                }
            }
        }
    
    TPtrC subject = iDocument.Mtm().SubjectL();
    TInt firstTextSlide = -1;
    for ( TInt i = 0; i < model->SmilModel().SlideCount() && firstTextSlide == -1; i++ )
        {
        CUniObject* obj = model->SmilModel().GetObject( i, EUniRegionText );
        if ( obj && obj->MediaType() == EMsgMediaText )
            {
            firstTextSlide = i;
            }
        }
        
    if ( subject.Length() <= 0 &&
         firstTextSlide != -1 &&
         iDocument.CurrentSlide() != firstTextSlide )
        {
        return NULL;
        }
        
    TInt length = 0;
    if ( subject.Length() <= 0 )
        {
        if ( model->SmilType() == EMmsSmil )
            {
            CMsgBodyControl* body = static_cast<CMsgBodyControl*>( iView.ControlById( EMsgComponentIdBody ) );
            if ( body )
                {
                length = body->TextContent().DocumentLength();
                
                // No subject. Change description.
                if ( !length )
                    {
                    return KNullDesC().AllocL();
                    }
                else
                    {
                    TBuf<KUniMaxDescription> description;
                    description.Zero();
                    length = ( length < KUniMaxDescription ? length : KUniMaxDescription );
                   
                    // 107-24185 : Emoticon support for SMS and MMS
                    
                    HBufC *text = body->Editor().GetTextInHBufL();
                    CleanupStack::PushL( text );
                    description.Copy( text->Ptr(), length );
                    CleanupStack::PopAndDestroy( text );
                    while( length-- )
                        {
                        if (    description[length] == CEditableText::ETabCharacter
                            ||  description[length] == CEditableText::EPageBreak
                            ||  description[length] == CEditableText::ENonBreakingSpace
                            ||  description[length] == CEditableText::EHyphen
                            ||  description[length] == CEditableText::ENonBreakingHyphen
                            ||  description[length] == CEditableText::ELeftSingleQuote
                            ||  description[length] == CEditableText::ERightSingleQuote
                            ||  description[length] == CEditableText::ELeftDoubleQuote
                            ||  description[length] == CEditableText::ERightDoubleQuote
                            ||  description[length] == CEditableText::EBullet
                            ||  description[length] == CEditableText::EEllipsis
                            ||  description[length] == CEditableText::ELineBreak
                            ||  description[length] == CEditableText::EParagraphDelimiter 
                            ||  description[length] == CEditableText::EPictureCharacter
                            ||  description[length] == CEditableText::EZeroWidthNoBreakSpace
                            ||  description[length] == CEditableText::EByteOrderMark
                            ||  description[length] == CEditableText::EReversedByteOrderMark            
                            ||  description[length] == '\n' )
                            {
                            description[length] = ' ';
                            }
                        }
                    description.Trim();
                    return description.AllocL();
                    }
                }
            }
        }
    else
        {
        
        TBuf<KUniMaxDescription> description;
        description.Zero();
        length = subject.Length();
        CMsgExpandableControl* header = static_cast<CMsgExpandableControl*>( iView.ControlById( EMsgComponentIdSubject ) );
        
        // Saving as pure text from the editor which may contain emoticons.
        HBufC *text = header->Editor().GetTextInHBufL();
        CleanupStack::PushL( text );
        description.Copy( text->Ptr(), length );
        CleanupStack::PopAndDestroy( text );
       

        while( length-- )
            {
            if ( description[length] == CEditableText::EParagraphDelimiter ||
                description[length] == '\n' )
                description[length] = ' ';
            }
        description.Trim();
        return description.AllocL();
        }
        
    return NULL;
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::DoSavePluginsL
//
// 2nd phase constructor.
// ---------------------------------------------------------
void CUniEditorSaveOperation::DoSavePluginsL()
    {
    iOperationState = EUniEditorSaveEnd;
    
    if( iDocument.SmsPlugin() )
        {
        iDocument.SmsPlugin()->SaveHeadersL( *iEditStore );
        }
        
    if( iDocument.MmsPlugin() )
        {
        iDocument.MmsPlugin()->SaveHeadersL( *iEditStore );
        }
                
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::DoCancelCleanup
//
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::DoCancelCleanup()
    {    
    if ( iSavedObject )
        {
        iSavedObject->Cancel();
        }
    
    iDocument.DataModel()->SmilList().Cancel();
    
    delete iEditStore;
    iEditStore = NULL;
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::RunL
//
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::RunL()
    {
    PrintOperationAndState();
    
    if ( !SetErrorAndReport( iStatus.Int() ) )
        {
        DoSaveStepL();
        }
    }
// ---------------------------------------------------------
// CUniEditorSaveOperation::RunError
// ---------------------------------------------------------
//
TInt CUniEditorSaveOperation::RunError( TInt aError )
    {
   
   if(aError == KErrDiskFull)
        {
            iOperationState = EUniEditorSaveError;  
            CompleteSelf( KErrNone );
            return KErrNone;               
        }
   if ( aError == KLeaveExit )
        {
        return KLeaveExit;
        }
    else
        {
        CompleteSelf( aError );
        return KErrNone;
        }
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::ObjectSaveReady
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::ObjectSaveReady( TInt aError )
    {
    if ( iSavedObject )
        {
        iSavedObject = NULL;
        iOperationState = EUniEditorSaveEnd;    
        }
    else if ( iOperationState == EUniEditorSaveAttachment )
        {
        iOperationState = EUniEditorSaveSmilCompose;
        }
    else if ( iOperationState == EUniEditorSaveObjects )
        {
        iOperationState = EUniEditorSaveAttachment; 
        }
        
    CompleteOperation( aError );
    }

// ---------------------------------------------------------
// CUniEditorSaveOperation::SmilComposeEvent
// ---------------------------------------------------------
//
void CUniEditorSaveOperation::SmilComposeEvent( TInt aError )
    {
    iOperationState = EUniEditorSaveFinalize;
    CompleteOperation( aError );
    }

// EOF
