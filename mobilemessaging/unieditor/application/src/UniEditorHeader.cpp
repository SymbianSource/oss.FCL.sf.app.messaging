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
* Description:   UniEditor header.
*
*/



// INCLUDE FILES

#include <eikrted.h>
#include <txtrich.h>                // CRichText

#include <centralrepository.h>    // link against centralrepository.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys
#include <MuiuMsvUiServiceUtilities.h> 
 
#include <mmsgenutils.h>

#include <messagingvariant.hrh>

#include <MsgExpandableControl.h>
#include <MsgAddressControl.h>
#include <MsgAttachmentControl.h>
#include <MsgRecipientItem.h>

#include <MsgMediaInfo.h>

#include <unimsventry.h>
#include <UniEditor.rsg>
#include <unidatautils.h>
#include <uniaddresshandler.h>

#include "UniEditorEnum.h"     // Panic
#include "UniEditorDocument.h"
#include "UniEditorHeader.h"


// ==========================================================

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KUniCharAt = '@';
const TInt  KUniMaxSubject  = 40;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CUniEditorHeader::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CUniEditorHeader* CUniEditorHeader::NewL( CUniClientMtm& aMtm,
                                            CUniEditorDocument& aDoc,
                                          CMsgEditorView& aView,
                                          RFs& aFs )
    {
    CUniEditorHeader* self = new ( ELeave ) CUniEditorHeader( aMtm, aDoc, aView, aFs );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CUniEditorHeader::CUniEditorHeader
//
// Constructor.
// ---------------------------------------------------------
//
CUniEditorHeader::CUniEditorHeader( CUniClientMtm& aMtm,
                                    CUniEditorDocument& aDoc,
                                    CMsgEditorView& aView,
                                    RFs& aFs ): 
    CUniBaseHeader( aMtm, aView, aFs ),
    iDoc( aDoc),
    iLongestEmail( 0 )
    {
    }

// ---------------------------------------------------------
// CUniEditorHeader::ConstructL
// ---------------------------------------------------------
//
void CUniEditorHeader::ConstructL()
    {
    AddHeadersVariationL( TUniMsvEntry::IsMmsUpload( iMtm.Entry().Entry() ) );
    AddHeadersConfigL();
     
    iAddDelayed = EUniFeatureTo;
    
    iDoc.DataModel()->AttachmentList().SetListObserver( this );
        
    const TPtrC subject = iMtm.SubjectL();
    
    TBool lockedSMS = EFalse;
    if ( iDoc.Mtm().MessageTypeSetting() == EUniMessageTypeSettingSms )
        {
        lockedSMS = ETrue;
        }
    
    if ( iAddHeadersVariation & EUniFeatureSubject )
        { 
        // Subject is supported
        if ( !( iAddHeadersVariation & EUniFeatureSubjectConfigurable  ) )
            { 
            // and it's not configurable -> it's always on
            iAddDelayed |= EUniFeatureSubject;
            }
        else if ( iAddHeadersConfig & EUniFeatureSubject &&
                  !lockedSMS )
            { 
            // or it's currently configured on and we are not on locked SMS mode.
            iAddDelayed |= EUniFeatureSubject;
            }
        else if ( subject.Length() )
            { 
            // or there's something in the subject
            iAddDelayed |= EUniFeatureSubject;
            }
        }

    // check adding into other field than 'home' field
    TInt addIntoOther( 0 );
    TInt addressesInStore = MtmAddressTypes();
    
    // configured or variated on but contains data
    if ( !lockedSMS &&
         ( iAddHeadersConfig & EUniFeatureCc ||  
           ( iAddHeadersVariation & EUniFeatureCc &&  
             addressesInStore & EUniFeatureCc ) ) )
        {
        iAddDelayed |= EUniFeatureCc;
        }
    else if ( !( iAddHeadersVariation & EUniFeatureCc ) &&  
              addressesInStore & EUniFeatureCc )
        {
        // show in the other field if variated off
        addIntoOther |= EUniFeatureCc;
        }
    
    // configured or variated on but contains data
    if ( !lockedSMS &&
         ( iAddHeadersConfig & EUniFeatureBcc ||  
           ( iAddHeadersVariation & EUniFeatureBcc &&  
             addressesInStore & EUniFeatureBcc ) ) )
        {
        iAddDelayed |= EUniFeatureBcc;
        }
    else if ( !( iAddHeadersVariation & EUniFeatureBcc ) &&  
              addressesInStore & EUniFeatureBcc )
        {
        // show in the other field if variated off
        addIntoOther |= EUniFeatureBcc;
        }
    
    if ( iDoc.DataModel()->AttachmentList().Count() > 0 )
        {
        iAddDelayed |= EUniFeatureAttachment;
        }
    
    DoAddToViewL( ETrue );          
    
    // Add data to those which are not configured but are variated off and contains data
    // CMsgEditorAppUi must set Document always modified!
    // Its is done in CUniEditorLaunchOperation::DoHandleMessageL()
    // Variation has changed On->Off and message opened from Drafts
    if ( addIntoOther & EUniFeatureCc )
        {
        iHeaders[EHeaderAddressTo].iAddressHandler->CopyAddressesFromMtmL( EMsvRecipientCc, ETrue ); 
        }
    if ( addIntoOther & EUniFeatureBcc && 
         iHeaders[EHeaderAddressCc].iAddressHandler )
        {
        iHeaders[EHeaderAddressCc].iAddressHandler->CopyAddressesFromMtmL( EMsvRecipientBcc, ETrue );
        }
    else if ( addIntoOther & EUniFeatureBcc )
        {
        iHeaders[EHeaderAddressTo].iAddressHandler->CopyAddressesFromMtmL( EMsvRecipientBcc, ETrue );        
        } 
    
    if ( iDoc.MessageType() == EUniReply )
        {
        RemoveDuplicateAddressesL();
        }    
    }

// ---------------------------------------------------------
// CUniEditorHeader::~CUniEditorHeader
// ---------------------------------------------------------
//
CUniEditorHeader::~CUniEditorHeader()
    {
    }

// ---------------------------------------------------------
// CUniEditorHeader::AddHeadersAddL
// ---------------------------------------------------------
//
void CUniEditorHeader::AddHeadersAddL(TInt aFlags)
    {    
    if ( !aFlags )
        {
        // nothing to add
        return;
        }
        
    // don't use anything, which is variated off
    aFlags &= iAddHeadersVariation;

    // do not add anything existing
    if ( iHeaders[EHeaderAddressCc].iControl )
        {
        aFlags &= ( ~EUniFeatureCc );
        }
    if ( iHeaders[EHeaderAddressBcc].iControl )
        {
        aFlags &= ( ~EUniFeatureBcc );
        }
    if ( iHeaders[EHeaderSubject].iControl )
        {
        aFlags &= ( ~EUniFeatureSubject );
        }
    if ( iHeaders[EHeaderAttachment].iControl )
        {
        aFlags &= ( ~EUniFeatureAttachment );
        }
    
    iAddDelayed = aFlags;

    // Test whether on the first slide.
    // Check whether we can add immediately.
    // Adding can be done only when on first slide.
    if ( iHeaders[EHeaderAddressTo].iOwned )
        {
        return;
        }
        
    DoAddToViewL(EFalse);    // do no read data
    }

// ---------------------------------------------------------
// CUniEditorHeader::AddHeadersDeleteL
// ---------------------------------------------------------
//
void CUniEditorHeader::AddHeadersDeleteL( TInt aFlags,
                                          TBool aContentModified)
    {
    if ( !aFlags ) 
        {
        return;
        }
    
    // Test whether on the first slide.
    if ( !iHeaders[EHeaderAddressTo].iOwned )
        {
        RemoveFromViewL( aFlags );
        }

    for (TInt i = EHeaderAddressCc; i <= EHeaderAttachment; i++ )
        {
        TInt flag = EUniFeatureCc;
        if ( i == EHeaderAddressBcc )
            {
            flag = EUniFeatureBcc; 
            }
        else if ( i == EHeaderSubject )
            {
            flag = EUniFeatureSubject; 
            }
        else if ( i == EHeaderAttachment )
            {
            flag = EUniFeatureAttachment;
            }
            
        if ( iHeaders[i].iOwned &&  
             aFlags & flag )
            {
            delete iHeaders[i].iControl;
            iHeaders[i].iControl = NULL;
            
            delete iHeaders[i].iAddressHandler;
            iHeaders[i].iAddressHandler = NULL;
            }
        }
    if ( aContentModified )
        {
        iView.SetControlsModified ( ETrue );
        }
    }

// ---------------------------------------------------------
// CUniEditorHeader::InsertRecipientL
// ---------------------------------------------------------
//
void CUniEditorHeader::InsertRecipientL(TAddressData&   aData,
                                        THeaderFields   aRecipientType,
                                        TBool           aReadContent)
    {
    // cannot add existing control
    __ASSERT_DEBUG( !aData.iControl, Panic( EUniIllegalArgument ) );

    TInt res( R_UNIEDITOR_TO );

    if ( aRecipientType == EHeaderAddressCc )
        {
        res = R_UNIEDITOR_CC;
        }
    else if ( aRecipientType == EHeaderAddressBcc )
        {
        res = R_UNIEDITOR_BCC;
        }
    else if ( aRecipientType != EHeaderAddressTo )
        {
        return;
        }
    
    iView.AddControlFromResourceL(  res, 
                                    EMsgAddressControl, 
                                    ControlIndexForAdding( aRecipientType ), 
                                    EMsgHeader );
                                    
    aData.iControl = static_cast<CMsgAddressControl*>
        ( iView.ControlById( aData.iControlType ) );
    aData.iOwned = EFalse;

    TBool addInvalidRecipient = EFalse;

    if( TUniMsvEntry::IsMmsUpload( iMtm.Entry().Entry() ) )
        {
        aData.iControl->SetReadOnly( ETrue );
        addInvalidRecipient = ETrue;
        }

    if( TUniMsvEntry::IsEditorOriented( iMtm.Entry().Entry() ) )
        {
        addInvalidRecipient = ETrue;
        }

    aData.iAddressHandler = CUniAddressHandler::NewL(
        iMtm,
        *static_cast<CMsgAddressControl*>(aData.iControl),
        *( iView.ControlEnv() ) );

    if ( aReadContent )
        {
        aData.iAddressHandler->CopyAddressesFromMtmL(aData.iRecipientTypeValue, addInvalidRecipient );
        }
    }

// ---------------------------------------------------------
// CUniEditorHeader::InsertSubjectL
// ---------------------------------------------------------
//
void CUniEditorHeader::InsertSubjectL( TBool aReadContent )
    {
    iView.AddControlFromResourceL(  R_UNIEDITOR_SUBJECT, 
                                    EMsgExpandableControl, 
                                    ControlIndexForAdding( EHeaderSubject ), 
                                    EMsgHeader );
    iHeaders[EHeaderSubject].iControl = static_cast<CMsgExpandableControl*>
        ( iView.ControlById( EMsgComponentIdSubject ) );
    iHeaders[EHeaderSubject].iOwned = EFalse;

    if ( aReadContent )
        {
        const TPtrC subject = iMtm.SubjectL();
        if ( subject.Length() )
            {
            iHeaders[EHeaderSubject].iControl->Editor().SetTextL( &subject );
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorHeader::InsertAttachmentL
// ---------------------------------------------------------
//
void CUniEditorHeader::InsertAttachmentL( TBool aReadContent )
    {
    CMsgAttachmentControl* attachmentControl = CMsgAttachmentControl::NewL( iView, iView );
    
    if ( iDoc.CurrentSlide() == 0 )
        {
        CleanupStack::PushL( attachmentControl );
        iView.AddControlL( attachmentControl,    
                           EMsgComponentIdAttachment,    
                           ControlIndexForAdding( EHeaderAttachment ), 
                           EMsgHeader );
        CleanupStack::Pop( attachmentControl );
    
        iHeaders[EHeaderAttachment].iControl = 
            static_cast<CMsgAttachmentControl*>( iView.ControlById( EMsgComponentIdAttachment ) );
        
        iHeaders[EHeaderAttachment].iOwned = EFalse;
        }
    else
        {
        iHeaders[EHeaderAttachment].iControl = attachmentControl;
        iHeaders[EHeaderAttachment].iOwned = ETrue;
        }

    if ( aReadContent )
        {
        CUniObjectList& attaList = iDoc.DataModel()->AttachmentList();
            
        for( TInt index = 0; index < attaList.Count(); index++ )
            {
            TParsePtrC fileParser( attaList.GetByIndex( index )->MediaInfo()->FullFilePath() );
                                        
            attachmentControl->AppendAttachmentL( fileParser.NameAndExt() );
            }
        
        }
    }

// ---------------------------------------------------------
// CUniEditorHeader::RemoveAttachmentL
// ---------------------------------------------------------
//
void CUniEditorHeader::RemoveAttachmentL()
    {
    if ( iDoc.CurrentSlide() == 0 )
        {
        iView.DeleteControlL( EMsgComponentIdAttachment );
        }
    else
        {
        delete iHeaders[EHeaderAttachment].iControl;
        }
    
    iHeaders[EHeaderAttachment].iControl = NULL;
    iHeaders[EHeaderAttachment].iOwned = EFalse;
    }
    
// ---------------------------------------------------------
// CUniEditorHeader::CopyAddressesToMtmL
// ---------------------------------------------------------
//
TBool CUniEditorHeader::CopyAddressesToMtmL( TBool aDocumentHeaderModified)
    {
    TBool modified = EFalse;
    if ( aDocumentHeaderModified )
        {
        modified = ETrue;
        }
    if ( modified )
        {
        // variation has changed On->Off and message opened from Drafts
        iHeaders[EHeaderAddressTo].iAddressHandler->
            RemoveAddressesFromMtmL( iHeaders[EHeaderAddressTo].iRecipientTypeValue );
        iHeaders[EHeaderAddressTo].iAddressHandler->
            RemoveAddressesFromMtmL( iHeaders[EHeaderAddressCc].iRecipientTypeValue );
        iHeaders[EHeaderAddressTo].iAddressHandler->
            RemoveAddressesFromMtmL( iHeaders[EHeaderAddressBcc].iRecipientTypeValue );
        }
    for (TInt i = EHeaderAddressTo; i <= EHeaderAddressBcc ; i++ )
        {    
        // all or nothing
        if (    iHeaders[i].iControl
            &&  modified )
            {
            if ( iHeaders[i].iAddressHandler )
                {
                iHeaders[i].iAddressHandler->CopyAddressesToMtmL(
                    iHeaders[i].iRecipientTypeValue);
                }
            modified = ETrue;
            }
        }
    return modified;
    }

// ---------------------------------------------------------
// CUniEditorHeader::CopyHeadersToMtmL
// ---------------------------------------------------------
//
TBool CUniEditorHeader::CopyHeadersToMtmL(  TBool aDocumentHeaderModified,
                                            TBool aSaveToMtm )
    {
    TBool modified = EFalse;
    if ( aDocumentHeaderModified )
        {
        modified = ETrue;
        }
    CMsgExpandableControl* subj = iHeaders[EHeaderSubject].iControl;
    
    if ( subj && modified )
        {
        TInt length = subj->TextContent().DocumentLength();
        
        if( length )
            {
            TBuf<KUniMaxSubject> description;
            description.Zero();
                  
            // Copying as pure text from the editor which may contain emoticons.
            
            HBufC *text = subj->Editor().GetTextInHBufL();
            CleanupStack::PushL( text );
            description.Copy( text->Ptr(), length );
            CleanupStack::PopAndDestroy( text );
            iMtm.SetSubjectL( description );
            modified = ETrue;
            }
        else
            {
            iMtm.SetSubjectL( KNullDesC() );
            }
        }
    // variation has changed On->Off and message opened from Drafts
    else if ( modified )
        {
        iMtm.SetSubjectL( KNullDesC() );
        }
        
    modified = CopyAddressesToMtmL (modified);    
    if (    modified 
        &&  aSaveToMtm )
        {
        iMtm.SaveMessageL();
        }
    
    return modified;
    }

// ---------------------------------------------------------
// CUniEditorHeader::VerifyAddressesL
// ---------------------------------------------------------
//
TBool CUniEditorHeader::VerifyAddressesL( TBool& aModified, TBool aAcceptEmails )
    {
    aModified = EFalse;
    TBool modified = EFalse;
    TBool ret = ETrue;
    for (TInt i = EHeaderAddressTo; i <= EHeaderAddressBcc ; i++ )
        {    
        // all or nothing
        if ( iHeaders[i].iAddressHandler )
            {
            CMsgCheckNames::TMsgAddressSelectType 
                msgType = aAcceptEmails?CMsgCheckNames::EMsgTypeMms:CMsgCheckNames::EMsgTypeSms;
            iHeaders[i].iAddressHandler->SetValidAddressType( msgType );
            TBool retTemp = iHeaders[i].iAddressHandler->VerifyAddressesL( modified );
            if ( modified )
                {
                aModified = ETrue;
                }
            if ( !retTemp )
                {
                // unvalidated addresses
                ret = EFalse;
                break;
                }
            }
        }
    return ret;
    }

// ---------------------------------------------------------
// CUniEditorHeader::NeedsVerificationL
// ---------------------------------------------------------
//
TBool CUniEditorHeader::NeedsVerificationL()
    {
    CMsgRecipientArray* recipients = NULL;
    for (TInt currentHeader = EHeaderAddressTo; currentHeader <= EHeaderAddressBcc ; currentHeader++ )
        {    
        // all or nothing
        if ( iHeaders[currentHeader].iAddressHandler )
            {
            recipients = 
                static_cast<CMsgAddressControl*>( iHeaders[currentHeader].iControl )->GetRecipientsL();
            
            for( TInt currentRecipient = 0; currentRecipient < recipients->Count(); currentRecipient++ )
                {
                if( !(recipients->At( currentRecipient )->IsVerified() ) )
                    {
                    return ETrue;
                    }
                }
            }
        }
    return EFalse;    
    }

// ---------------------------------------------------------
// CUniEditorHeader::AddRecipientL
// ---------------------------------------------------------
//
TBool CUniEditorHeader::AddRecipientL(  CMsgBaseControl* aFocusedControl,
                                        MObjectProvider* /*aParent*/,
                                        TBool            aIncludeEmailAddresses,
                                        TBool&           aInvalid )
    {
    TBool added = EFalse;
    
    // note order of for ()
    for ( TInt i = EHeaderAddressBcc; i >= EHeaderAddressTo ; i-- )
        {
        CMsgCheckNames::TMsgAddressSelectType msgType = aIncludeEmailAddresses ? CMsgCheckNames::EMsgTypeMms:
                                                                                 CMsgCheckNames::EMsgTypeSms;
        if ( iHeaders[i].iControl == aFocusedControl &&  
             iHeaders[i].iAddressHandler )
            {
            iHeaders[i].iAddressHandler->SetValidAddressType( msgType );
            added = iHeaders[i].iAddressHandler->AddRecipientL( aInvalid );
            break;
            }
            
        // if focus on any other control, add to To: recipient list
        if ( i == EHeaderAddressTo &&  
             iHeaders[EHeaderAddressTo].iAddressHandler )
            {
            iHeaders[i].iAddressHandler->SetValidAddressType( msgType );
            added = iHeaders[i].iAddressHandler->AddRecipientL( aInvalid );
            }
        }
        
    return added;
    }

// ---------------------------------------------------------
// CUniEditorHeader::RemoveDuplicateAddressesL
// ---------------------------------------------------------
//
TBool CUniEditorHeader::RemoveDuplicateAddressesL()
    {
    TBool retVal = EFalse;
    CArrayPtrFlat<CMsgAddressControl>* addressControls = new ( ELeave ) CArrayPtrFlat<CMsgAddressControl>( 3 );
    CleanupStack::PushL( addressControls );

    if ( AddressControl( CUniBaseHeader::EHeaderAddressTo ) )
        {
        addressControls->AppendL( AddressControl( CUniBaseHeader::EHeaderAddressTo ) );
        }
    if ( AddressControl( CUniBaseHeader::EHeaderAddressCc ) )
        {
        addressControls->AppendL( AddressControl( CUniBaseHeader::EHeaderAddressCc ) );
        }
    if ( AddressControl( CUniBaseHeader::EHeaderAddressBcc ) )
        {
        addressControls->AppendL( AddressControl( CUniBaseHeader::EHeaderAddressBcc ) );
        }
        
    retVal = CUniAddressHandler::RemoveDuplicateAddressesL( *addressControls );
    CleanupStack::PopAndDestroy( addressControls );
    return retVal;
    }

// ---------------------------------------------------------
// CUniEditorHeader::IsHeaderSmsL
// ---------------------------------------------------------
//
TBool CUniEditorHeader::IsHeaderSmsL()
    {
    // Is one of the following visible and do they have content
    CMsgRecipientArray* recipients = NULL;
    
    if ( iHeaders[EHeaderAddressCc].iControl )
        { 
        // CC is visible
        recipients = static_cast<CMsgAddressControl*>( iHeaders[EHeaderAddressCc].iControl )->GetRecipientsL();
            
        if ( recipients &&  
             recipients->Count() > 0 )
            {   
            // There are recipients -> this is not sms
            iLongestEmail = 0;
            return EFalse;
            }
        }
        
    if ( iHeaders[EHeaderAddressBcc].iControl )
        { 
        // BCC is visible
        recipients = static_cast<CMsgAddressControl*>( iHeaders[EHeaderAddressBcc].iControl )->GetRecipientsL();
            
        if ( recipients &&  
             recipients->Count() > 0 )
            {   // there are recipients -> this is not sms
            iLongestEmail = 0;
            return EFalse;
            }
        }
        
    // Are there too many recipients in To field?        
    if ( iHeaders[CUniEditorHeader::EHeaderAddressTo].iControl )
        {
        recipients = static_cast<CMsgAddressControl*>( iHeaders[EHeaderAddressTo].iControl )->GetRecipientsL();
        
        if ( recipients &&  
             recipients->Count() > iDoc.MaxSmsRecipients() )
            {
            iLongestEmail = 0;
            return EFalse;
            }
            
        // Refresh the longest email address length
        RefreshLongestEmailAddressL();
        
        if ( !iDoc.EmailOverSmsSupported() )
            {
            for ( TInt i = 0; i < recipients->Count(); i++ )
                {
                if ( IsValidEmailAddress( recipients->At( i )->Address()->Des() ) )
                    {
                    return EFalse;
                    }
                }
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CUniEditorHeader::ObjectAddedL
// ---------------------------------------------------------
//
void CUniEditorHeader::ObjectAddedL( CUniObjectList* aList, 
                                     CUniObject* aObject, 
                                     TInt aIndex )
    {                        
    if ( aObject &&
         aList == &iDoc.DataModel()->AttachmentList() )
        {               
        if ( !iHeaders[CUniEditorHeader::EHeaderAttachment].iControl )
            {
            InsertAttachmentL( EFalse );
            }
        
        CMsgAttachmentControl* attachmentControl = 
                static_cast<CMsgAttachmentControl*>( 
                    iHeaders[CUniEditorHeader::EHeaderAttachment].iControl );
        
        TParsePtrC fileParser( aObject->MediaInfo()->FullFilePath() );
        
        attachmentControl->InsertAttachmentL( fileParser.NameAndExt(), aIndex );
        }
    }
        
// ---------------------------------------------------------
// CUniEditorHeader::ObjectRemovedL
// ---------------------------------------------------------
//
void CUniEditorHeader::ObjectRemovedL( CUniObjectList* aList, 
                                       CUniObject* aObject,
                                       TInt aIndex )
    {
    if ( aObject &&
         aList == &iDoc.DataModel()->AttachmentList() )
        {
        CMsgAttachmentControl* attachmentControl = 
                static_cast<CMsgAttachmentControl*>( 
                    iHeaders[CUniEditorHeader::EHeaderAttachment].iControl );
                        
        if ( attachmentControl )
            {
            TParsePtrC fileParser( aObject->MediaInfo()->FullFilePath() );
            TPtrC plainFileName = fileParser.NameAndExt();
            
            HBufC* attachmentName = attachmentControl->Attachment( aIndex ).AllocLC();
            
            if ( plainFileName.Match( *attachmentName ) )
                {
                attachmentControl->RemoveAttachmentL( aIndex );
                }
            else 
                {
                attachmentControl->RemoveAttachmentL( plainFileName );
                }
            
            CleanupStack::PopAndDestroy( attachmentName );
            }
        
        if ( aList->Count() == 0 )
            {
            RemoveAttachmentL();
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorHeader::RefreshLongestEmailAddressL
// ---------------------------------------------------------
//
void CUniEditorHeader::RefreshLongestEmailAddressL()
    {
    TInt longestOne = 0;
    if ( iDoc.EmailOverSmsSupported() && iHeaders[CUniEditorHeader::EHeaderAddressTo].iControl )
        {
        CMsgRecipientArray* recipients = NULL;
        recipients = static_cast<CMsgAddressControl*>
            ( iHeaders[CUniEditorHeader::EHeaderAddressTo].iControl )->GetRecipientsL();
        for( TInt i = 0; i < recipients->Count(); i++ )
            {
            if( IsValidEmailAddress( recipients->At( i )->Address()->Des() ) )
                {
                if( recipients->At( i )->Address()->Length() > longestOne )
                    {
                    longestOne = recipients->At( i )->Address()->Length();
                    }
                }
            }
        }
    iLongestEmail = longestOne;
    }

// ---------------------------------------------------------
// CUniEditorHeader::ControlIndexForAdding
// ---------------------------------------------------------
//
TInt CUniEditorHeader::ControlIndexForAdding( TInt aControlToAdd )
    {
    TInt index = 0;
    switch( aControlToAdd )
        {
        case EHeaderAttachment:
            {
            if ( iHeaders[EHeaderSubject].iControl &&  
                 iHeaders[EHeaderSubject].iControl->IsVisible() )
                {
                index++;
                }
            } // Ok to flow thru
        case EHeaderSubject:
            {
            if ( iHeaders[EHeaderAddressBcc].iControl &&  
                 iHeaders[EHeaderAddressBcc].iControl->IsVisible() )
                {
                index++;
                }
            } // Ok to flow thru
        case EHeaderAddressBcc:
            {
            if ( iHeaders[EHeaderAddressCc].iControl &&  
                 iHeaders[EHeaderAddressCc].iControl->IsVisible() )
                {
                index++;
                }
            } // Ok to flow thru
        case EHeaderAddressCc:
            {
            if ( iHeaders[EHeaderAddressTo].iControl &&  
                 iHeaders[EHeaderAddressTo].iControl->IsVisible() )
                {
                index++;
                }
                
            break;
            }
        default:
            {
            break;
            }
        }
        
    return index;
    }
    
// ---------------------------------------------------------
// CUniEditorHeader::ShowInvalidContactNotesL
// ---------------------------------------------------------
//
void CUniEditorHeader::ShowInvalidContactNotesL()
    {
    for (TInt i = EHeaderAddressTo; i <= EHeaderAddressBcc ; i++ )
        {    
        // all or nothing
        if ( iHeaders[i].iAddressHandler )
            {
            iHeaders[i].iAddressHandler->ShowInvalidRecipientInfoNotesL();
            }
        }
    }

// ---------------------------------------------------------
// CUniEditorHeader::IsValidEmailAddress
// ---------------------------------------------------------
//
TBool CUniEditorHeader::IsValidEmailAddress( const TDesC& aAddress )
    { 
    // valid email address contains at least 3 characters
    if( aAddress.Length() >= 3 )
        {
        // search for @ from the address. however, it can't be the first or the last item
        for ( TInt i = 1; i < aAddress.Length() - 1; i++ )
            {
            if ( aAddress[ i ] == KUniCharAt )
                {
                return ETrue;
                }
            }
        }
    return EFalse;
    }
    
    
    

//  End of File
