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
* Description:  
*       MMS viewer header
*
*/



// INCLUDE FILES

#include <eikrted.h>

#include <commonphoneparser.h>

#include <mmsgenutils.h>
#include <mmsmsventry.h>

#include <messagingvariant.hrh>

#include <MsgExpandableControl.h>
#include <MsgAddressControl.h>

#include <MmsViewer.rsg>
#include "MmsViewer.hrh"
#include "MmsViewerHeader.h"

#include <mmsclient.h>
#include <MsgMediaInfo.h>
#include <MsgAttachmentControl.h>
#include <uniaddresshandler.h>
#include <uniobjectlist.h>
#include "UniMmsViewerDocument.h"


// ==========================================================

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
GLREF_C void Panic( TMmsViewerExceptions aPanic ); 

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsViewerHeader::CMmsViewerHeader
//
// Constructor.
// ---------------------------------------------------------
//
CMmsViewerHeader::CMmsViewerHeader( CMmsClientMtm& aMtm,
                                    CUniObjectList& aAttachmentList,
                                    CMsgEditorView& aView,
                                    RFs& aFs )
    : CUniBaseHeader( aMtm, aView, aFs ),
    iAttachmentList( aAttachmentList )
    {
    }


// ---------------------------------------------------------
// CMmsViewerHeader::ConstructL
//
//
// ---------------------------------------------------------
//
void CMmsViewerHeader::ConstructL( )
    {
    
    AddHeadersVariationL( EFalse );
    AddHeadersConfigL( );
             
        // subject is shown, if configured or contains data
        // (even if variated off )     
    const TPtrC subject = iMtm.SubjectL();
    if (    iAddHeadersConfig & EUniFeatureSubject
        ||  subject.Length() )
        {
        iAddDelayed |= EUniFeatureSubject;
        }
    iAddDelayed |= EUniFeatureAttachment;
    TBool mobileTerminated( ETrue );
    TInt addressesInStore = MtmAddressTypes();
    
    TMmsMsvEntry mmsEntry =
        static_cast<TMmsMsvEntry>( iMtm.Entry().Entry() );
    mobileTerminated = mmsEntry.IsMobileTerminated();
    if ( mobileTerminated )
        {
        iAddDelayed |= EUniFeatureFrom;
        }
    else
        {
        // see also below necessity of the To: field   
        if ( addressesInStore & EUniFeatureTo )
            {
            iAddDelayed |= EUniFeatureTo;
            }
               
        if (    iAddHeadersVariation & EUniFeatureCc
            &&  addressesInStore & EUniFeatureCc )
            {
                // variated on and contains data
            iAddDelayed |= EUniFeatureCc;
            }
            // show in the other field if variated off
        else if (   ! ( iAddHeadersVariation & EUniFeatureCc ) 
                &&  addressesInStore & EUniFeatureCc )
            {
            // Cc: data exists, add it into To:
            iAddDelayed |= EUniFeatureTo;
            iAddIntoOtherCc |= EUniFeatureTo;
            }
        
        if (    iAddHeadersVariation & EUniFeatureBcc
            &&  addressesInStore & EUniFeatureBcc )
            {
            // variated on and contains data
            iAddDelayed |= EUniFeatureBcc;
            }
            // show in the other field if variated off
        else if (   ! ( iAddHeadersVariation & EUniFeatureBcc ) 
                &&  addressesInStore & EUniFeatureBcc )
            {
            if ( iAddHeadersVariation & EUniFeatureCc )
                {
                // Bcc: data exists, add it into Cc:
                iAddDelayed |= EUniFeatureCc;
                iAddIntoOtherBcc |= EUniFeatureCc;
                }
            else
                {
                // No Cc:, must be added into To:
                iAddDelayed |= EUniFeatureTo;
                iAddIntoOtherBcc |= EUniFeatureTo;
                }
            }
            
        }        
    DoAddToViewL( ETrue );          
    }

// ---------------------------------------------------------
// CMmsViewerHeader::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMmsViewerHeader* CMmsViewerHeader::NewL( CMmsClientMtm& aMtm,
                                          CUniObjectList& aAttachmentList,
                                          CMsgEditorView& aView,
                                          RFs& aFs )
    {
    CMmsViewerHeader* self = new ( ELeave ) CMmsViewerHeader(
        aMtm, aAttachmentList, aView, aFs );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMmsViewerHeader::~CMmsViewerHeader
// ---------------------------------------------------------
CMmsViewerHeader::~CMmsViewerHeader()
    {
    delete iAlias;
    }

// ---------------------------------------------------------
// InsertRecipientL
// ---------------------------------------------------------
void CMmsViewerHeader::InsertRecipientL(TAddressData&   aData,
                                        THeaderFields   aRecipientType,
                                        TBool           aReadContent)
    {
    TInt res( R_MMSVIEWER_TO );
    TInt flag( EUniFeatureTo );
    
    if ( aRecipientType == EHeaderAddressCc )
        {
        res = R_MMSVIEWER_CC;
        flag = EUniFeatureCc;
        }
    else if ( aRecipientType == EHeaderAddressBcc )
        {
        res = R_MMSVIEWER_BCC;
        flag = EUniFeatureBcc;
        }
    else if ( aRecipientType != EHeaderAddressTo )
        {
        return;
        }

    DoInsertRecipientL( aData,
                        res,
                        aReadContent,
                        flag );          // read from MTM
    }

// ---------------------------------------------------------
// DoInsertRecipientL
// ---------------------------------------------------------
void CMmsViewerHeader::DoInsertRecipientL(  TAddressData&   aData,
                                            TInt            aResource,
                                            TBool           /*aReadContent*/,
                                            TInt            aAddHeadersRecipientType )
    {    
            // cannot add existing control
    __ASSERT_DEBUG( !aData.iControl, Panic( EMmsViewerNullPointer ) );
    
    const CMsvRecipientList& addresses = iMtm.AddresseeList();
    iView.AddControlFromResourceL(  aResource, 
                                    EMsgAddressControl, 
                                    EMsgAppendControl, 
                                    EMsgHeader );
    aData.iControl = static_cast<CMsgAddressControl*>
        ( iView.ControlById( aData.iControlType ) );
    aData.iOwned = EFalse;

    CMsgRecipientList* recipientList = CMsgRecipientList::NewL();
    CleanupStack::PushL( recipientList );

    TInt count = addresses.Count();
    TMsvRecipientTypeValues msvRecipientType( aData.iRecipientTypeValue );
    for (TInt k = aData.iRecipientTypeValue; k <= EMsvRecipientBcc ; k++ )
        {
        // on the first round copy 'native' addresses then additional headers
        if (    k == 1 
            && iAddIntoOtherCc & aAddHeadersRecipientType )
            {
            msvRecipientType = EMsvRecipientCc;
            }      
        else if (   k == 2
                &&  iAddIntoOtherBcc & aAddHeadersRecipientType )
            {
            msvRecipientType = EMsvRecipientBcc;
            }      
        if (    k > aData.iRecipientTypeValue 
            &&  msvRecipientType == aData.iRecipientTypeValue )
            {
            // no update in  the initial value
            continue;
            }

        for ( TInt i = 0; i < count; i++)
            {
            if ( addresses.Type(i) == msvRecipientType )
                {            
                CMsgRecipientItem* recipient = CMsgRecipientItem::NewLC(
                    TMmsGenUtils::Alias( addresses[i] ),
                    TMmsGenUtils::PureAddress( addresses[i] ) );
                recipient->SetVerified( ETrue );
                recipientList->AppendL( recipient );
                CleanupStack::Pop( recipient );
                recipient = NULL;
                }
            }
        }
    static_cast<CMsgAddressControl*>( aData.iControl )->AddRecipientsL( *recipientList );
    aData.iControl->Editor().SetCursorPosL( 0, EFalse );

    CleanupStack::PopAndDestroy( recipientList );
    }
        
// ---------------------------------------------------------
// InsertFromL
// ---------------------------------------------------------
void CMmsViewerHeader::InsertFromL( TBool /*aReadContent*/ )
    {
    iView.AddControlFromResourceL( R_MMSVIEWER_FROM, EMsgAddressControl, EMsgAppendControl, EMsgHeader );
    iHeaders[EHeaderAddressFrom].iControl = static_cast<CMsgAddressControl*>
        ( iView.ControlById( EMsgComponentIdFrom ) );
    iHeaders[EHeaderAddressFrom].iOwned = EFalse;

    const TPtrC sender = static_cast<CMmsClientMtm&>(iMtm).Sender();
    const TPtrC details = iMtm.Entry().Entry().iDetails;
    
    if ( sender.Length() )
        {
        iSenderType = MsvUiServiceUtilitiesInternal::ResolveAddressTypeL(    
            TMmsGenUtils::PureAddress( sender ),
                        CommonPhoneParser::ESMSNumber );
        
        if ( iSenderType != EMuiuAddressTypePhoneNumber )
            {
            //We trust on that there won't be illegal addresses
            //in received messages
            iSenderType = EMuiuAddressTypeEmail;
            }
        
        //Check if MMS engine has put an alias into iDetails field
        if ( sender.Compare( details ) != 0 )
            {
            TPtrC alias = KNullDesC();
            if ( details.Length() && !iAlias )
                {
                iAlias = details.AllocL( );
                alias.Set( *iAlias );
                }

            // Resolve whether this is remote alias. Local alias takes precedence
            TPtrC tempAlias( TMmsGenUtils::Alias( sender ) );
            if (    iAlias
                &&  tempAlias.Length()
                &&  !tempAlias.CompareF( *iAlias ) )
                
                {
                iRemoteAlias = ETrue;
                }

            MVPbkContactLink* link = NULL;
            static_cast<CMsgAddressControl*>( iHeaders[EHeaderAddressFrom].iControl )
                ->AddRecipientL( alias,
                TMmsGenUtils::PureAddress( sender ),
                ETrue,
                link );
            }
        else
            {
            MVPbkContactLink* link = NULL;
            static_cast<CMsgAddressControl*>( iHeaders[EHeaderAddressFrom].iControl )
                ->AddRecipientL( TMmsGenUtils::Alias( sender ),
                TMmsGenUtils::PureAddress( sender ),
                ETrue,
                link );
            }
        }
    }

// ---------------------------------------------------------
// RemoveSubjectContentL
// ---------------------------------------------------------
//
void CMmsViewerHeader::RemoveSubjectContentL( )
    {
    if (    iHeaders[EHeaderSubject].iControl 
        &&  iHeaders[EHeaderSubject].iControl->Editor().TextLength() )
        {
        // without this change Japanese pictographs in subject field 
        // appear on slides 2,...
        iHeaders[EHeaderSubject].iControl->Editor().SetCursorPosL( 0, EFalse );
        TPtrC nullPtrC( KNullDesC() );
        iHeaders[EHeaderSubject].iControl->Editor().SetTextL( &nullPtrC );
        }
    }


// ---------------------------------------------------------
// RestoreSubjectContentL
// ---------------------------------------------------------
//
void CMmsViewerHeader::RestoreSubjectContentL()
    {
    InsertSubjectL( ETrue );
    }

// ---------------------------------------------------------
// InsertSubjectL
// ---------------------------------------------------------
//
void CMmsViewerHeader::InsertSubjectL( TBool /*aReadContent*/ )
    {
    HBufC* subject = iMtm.SubjectL().AllocLC();
    TPtr subjectPtr = subject->Des();
    subjectPtr.Trim();
    if (    subjectPtr.Length() 
        &&  !iHeaders[EHeaderSubject].iControl )
        {
        iView.AddControlFromResourceL( R_MMSVIEWER_SUBJECT, EMsgExpandableControl, EMsgAppendControl, EMsgHeader );
        iHeaders[EHeaderSubject].iControl = static_cast<CMsgExpandableControl*>
            ( iView.ControlById( EMsgComponentIdSubject ) );
        iHeaders[EHeaderSubject].iOwned = EFalse;

        iHeaders[EHeaderSubject].iControl->Editor().SetTextL( subject );
        }
    else if (   subjectPtr.Length() 
            &&  iHeaders[EHeaderSubject].iControl
            &&  !iHeaders[EHeaderSubject].iControl->Editor().TextLength() )
        {
        iHeaders[EHeaderSubject].iControl->Editor().SetTextL( subject );
        }
    CleanupStack::PopAndDestroy(); //subject
    }

// ---------------------------------------------------------
// CUniEditorHeader::InsertAttachmentL
// ---------------------------------------------------------
//
void CMmsViewerHeader::InsertAttachmentL( TBool /*aReadContent*/)
    {
    if ( iAttachmentList.Count() )
        {        
        CMsgAttachmentControl* attachmentControl = CMsgAttachmentControl::NewL( iView, iView );
        CleanupStack::PushL( attachmentControl );
        iView.AddControlL( attachmentControl,    
                           EMsgComponentIdAttachment,    
                           EMsgAppendControl, 
                           EMsgHeader );
        CleanupStack::Pop( attachmentControl );
    
        iHeaders[EHeaderAttachment].iControl = 
            static_cast<CMsgAttachmentControl*>( iView.ControlById( EMsgComponentIdAttachment ) );
        
        iHeaders[EHeaderAttachment].iOwned = EFalse;

        for( TInt index = 0; index < iAttachmentList.Count(); index++ )
            {
            TParsePtrC fileParser( iAttachmentList.GetByIndex( index )->MediaInfo()->FullFilePath() );                          
            attachmentControl->AppendAttachmentL( fileParser.NameAndExt() );
            }
        }
            
    }


// ---------------------------------------------------------
// RemoveAttachmentL
// ---------------------------------------------------------
//
void CMmsViewerHeader::RemoveAttachmentL( )
    {
    RemoveFromViewL( EUniFeatureAttachment );
    }

//  End of File
