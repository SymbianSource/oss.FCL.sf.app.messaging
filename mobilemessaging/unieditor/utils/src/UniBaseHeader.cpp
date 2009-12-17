/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Defines implementation of CUniBaseHeader class methods.
*
*/



// INCLUDE FILES
#include "UniBaseHeader.h"

#include <eikrted.h>
#include <txtrich.h>                // CRichText
#include <mtclbase.h>

#include <centralrepository.h>    // link against centralrepository.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys

#include <mmsgenutils.h>

#include <messagingvariant.hrh>

#include <MsgExpandableControl.h>
#include <MsgAddressControl.h>

#include <uniaddresshandler.h>
#include <uniobjectlist.h>


// ==========================================================

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CUniBaseHeader::CUniBaseHeader
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CUniBaseHeader::CUniBaseHeader( CBaseMtm& aMtm,
                                         CMsgEditorView& aView,
                                         RFs& aFs ) : 
    iMtm( aMtm ),
    iView( aView ),
    iFs( aFs )
    {
    iHeaders[EHeaderAddressFrom].iControlType = EMsgComponentIdFrom;
    iHeaders[EHeaderAddressTo].iControlType = EMsgComponentIdTo;
    iHeaders[EHeaderAddressCc].iControlType = EMsgComponentIdCc;
    iHeaders[EHeaderAddressBcc].iControlType = EMsgComponentIdBcc;
    iHeaders[EHeaderSubject].iControlType = EMsgComponentIdSubject;
    iHeaders[EHeaderAttachment].iControlType = EMsgComponentIdAttachment;
    iHeaders[EHeaderAddressTo].iRecipientTypeValue = EMsvRecipientTo;
    iHeaders[EHeaderAddressCc].iRecipientTypeValue = EMsvRecipientCc;
    iHeaders[EHeaderAddressBcc].iRecipientTypeValue = EMsvRecipientBcc;
    }

// ---------------------------------------------------------
// CUniBaseHeader::~CUniBaseHeader
// ---------------------------------------------------------
//
EXPORT_C CUniBaseHeader::~CUniBaseHeader()
    {
    for (TInt i = EHeaderAddressFrom; i <= EHeaderAttachment; i++ )
        {
        if ( iHeaders[i].iOwned )
            {
            delete iHeaders[i].iControl;
            }
        delete iHeaders[i].iAddressHandler;
        }
    }

// ---------------------------------------------------------
// CUniBaseHeader::AddToViewL
//
// Assumes that slide is automatically correct. Caller's responsibility
// ---------------------------------------------------------
// 
EXPORT_C void CUniBaseHeader::AddToViewL()
    {    
    DoAddToViewL( EFalse ); // do not read data
    }

// ---------------------------------------------------------
// CUniBaseHeader::DoAddToViewL
// ---------------------------------------------------------
// 
EXPORT_C void CUniBaseHeader::DoAddToViewL( TBool aReadContent )
    {
    TInt flag = EUniFeatureFrom;
    
    // Headers must be in right order. Remove additional first
    RemoveFromViewL( EUniFeatureCc | EUniFeatureBcc | EUniFeatureSubject | EUniFeatureAttachment );
    
    for ( TInt i = EHeaderAddressFrom; i <= EHeaderAttachment; i++ )
        {
        if ( i == EHeaderAddressTo )
            {
            flag = EUniFeatureTo;
            }
        else if ( i == EHeaderAddressCc )
            {
            flag = EUniFeatureCc;
            }
        else  if ( i == EHeaderAddressBcc )
            {
            flag = EUniFeatureBcc;
            }
        else  if ( i == EHeaderSubject )
            {
            flag = EUniFeatureSubject;
            }
        else if ( i == EHeaderAttachment )
            {
            flag = EUniFeatureAttachment;
            }
            
        if ( iHeaders[i].iControl &&  
             !iView.ControlById( iHeaders[i].iControlType ) )
            {
            iView.AddControlL( iHeaders[i].iControl,
                               iHeaders[i].iControlType,
                               EMsgAppendControl,
                               EMsgHeader );
            iHeaders[i].iOwned = EFalse;
            }
        else if ( iAddDelayed & EUniFeatureFrom &&  
                  i == EHeaderAddressFrom )
            {
            InsertFromL( aReadContent );          // read from MTM
            iAddDelayed &= ( ~EUniFeatureFrom );
            }
        else if ( iAddDelayed & EUniFeatureSubject &&  
                  i == EHeaderSubject )
            {
            InsertSubjectL( aReadContent );
            iAddDelayed &= ( ~EUniFeatureSubject );
            }
        else if ( iAddDelayed & EUniFeatureAttachment &&
                  i == EHeaderAttachment )
            {
            InsertAttachmentL( aReadContent );
            iAddDelayed &= ( ~EUniFeatureAttachment );
            }
        else if ( iAddDelayed & flag &&  
                  ( i == EHeaderAddressTo ||  
                    i == EHeaderAddressCc ||  
                    i == EHeaderAddressBcc ) )
            {
            InsertRecipientL( iHeaders[i],
                              static_cast<THeaderFields> ( i ),
                              aReadContent);          // read from MTM
            iAddDelayed &= ( ~flag );
            }
        }
    iAddDelayed = 0;
    }
    
// ---------------------------------------------------------
// CUniBaseHeader::InsertFromL
// ---------------------------------------------------------
// 
EXPORT_C void CUniBaseHeader::InsertFromL( TBool /*aReadContent*/) // read from MTM
    {
    }

// ---------------------------------------------------------
// CUniBaseHeader::RemoveFromViewL
//
// Assumes that slide is automatically correct. Caller's responsibility
// ---------------------------------------------------------
// 
EXPORT_C void CUniBaseHeader::RemoveFromViewL( TInt aFlags )
    {
    for ( TInt i = EHeaderAddressFrom; i <= EHeaderAttachment; i++ )
        {      
        TInt flag( EUniFeatureTo );
        if ( i == EHeaderAddressFrom )
            {
            flag = EUniFeatureFrom; 
            }
        else if ( i == EHeaderAddressCc )
            {
            flag = EUniFeatureCc; 
            }
        else if ( i == EHeaderAddressBcc )
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
            
        if ( aFlags & flag &&  
             iHeaders[i].iControl )
            {   
            if ( iView.RemoveControlL( iHeaders[i].iControlType ) )
                {
                iHeaders[i].iOwned = ETrue;
                }
            // else - not removed
            }
        }        
    }
    
// ---------------------------------------------------------
// MtmAddressTypes
// ---------------------------------------------------------
//
EXPORT_C TInt CUniBaseHeader::MtmAddressTypes()
    {
    TInt ret(0);
    const CMsvRecipientList& addresses = iMtm.AddresseeList();

    TInt addrCnt = addresses.Count();        
    for ( TInt i = 0; i < addrCnt; i++ )
        {          
        if ( addresses.Type(i) == EMsvRecipientTo )
            {
            ret |= EUniFeatureTo;
            }
        else if ( addresses.Type(i) == EMsvRecipientCc )
            {
            ret |= EUniFeatureCc;
            }
        else if ( addresses.Type(i) == EMsvRecipientBcc )
            {
            ret |= EUniFeatureBcc;
            }
        }
    return ret;
    }

// ---------------------------------------------------------
// CUniBaseHeader::FocusedAddressField
// ---------------------------------------------------------
//
EXPORT_C TInt CUniBaseHeader::FocusedAddressField( CMsgBaseControl* aFocusedControl )
    {
    TInt ret = KErrNotFound;
    for ( TInt i = EHeaderAddressFrom; i <= EHeaderAttachment ; i++ )
        {    
        if ( iHeaders[i].iControl == aFocusedControl )
            {
            return iHeaders[i].iControlType;
            }
        }
    return ret;
    }

// ---------------------------------------------------------
// CUniBaseHeader::AddHeadersVariation
// ---------------------------------------------------------
//
EXPORT_C void CUniBaseHeader::AddHeadersVariationL( TBool aIsUpload )
    {
    TInt featureBitmask = 0;
    
    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, featureBitmask );
    delete repository;
    
    // bit 0x2
    if ( featureBitmask & KMmsFeatureIdEditorSubjectField )
        { // Subject is supported
        iAddHeadersVariation |= EUniFeatureSubject;
        if( featureBitmask & KMmsFeatureIdEditorSubjectConfigurable )
            { // It is also configurable
            iAddHeadersVariation |= EUniFeatureSubjectConfigurable;
            }
        }
    // bit 0x400
    // do not show for upload messages
    if ( featureBitmask & KMmsFeatureIdEditorCcField && 
         !aIsUpload )
        {
        iAddHeadersVariation |= EUniFeatureCc;
        }
    // bit 0x800
    // do not show for upload messages
    if ( featureBitmask & KMmsFeatureIdEditorBccField &&  
         !aIsUpload )
        {
        iAddHeadersVariation |= EUniFeatureBcc;
        }        
    }

// ---------------------------------------------------------
// CUniBaseHeader::AddHeadersConfig
// ---------------------------------------------------------
//
EXPORT_C TInt CUniBaseHeader::AddHeadersConfigL()
    {
    TInt featureBitmask = 0;
    TInt ret = 0;
    CRepository* repository = CRepository::NewL( KCRUidMmsui );
    repository->Get( KMmsuiAdditionalHeaders, featureBitmask );
    delete repository;

    // set bit to null if variation is OFF
    if ( featureBitmask & KMmsuiHeaderSubject &&  
         iAddHeadersVariation & EUniFeatureSubjectConfigurable )
        {
        ret |= EUniFeatureSubject;
        }
    if ( featureBitmask & KMmsuiHeaderCC &&  
         iAddHeadersVariation & EUniFeatureCc )
        {
        ret |= EUniFeatureCc;
        }
    if ( featureBitmask & KMmsuiHeaderBCC &&  
         iAddHeadersVariation & EUniFeatureBcc )
        {
        ret |= EUniFeatureBcc;
        }
    iAddHeadersConfig = ret;
    return ret;
    }

// ---------------------------------------------------------
// CUniBaseHeader::SaveAddHeadersConfigL
// ---------------------------------------------------------
//
EXPORT_C void CUniBaseHeader::SaveAddHeadersConfigL(TInt aFlags)
    {
    TInt featureBitmask = 0;    
    CRepository* repository = CRepository::NewL( KCRUidMmsui  );
    
    // Do not set bit if variation is OFF
    if ( aFlags & EUniFeatureSubject &&  
         iAddHeadersVariation & EUniFeatureSubjectConfigurable )
        {
        featureBitmask |= KMmsuiHeaderSubject;
        }
    if ( aFlags & EUniFeatureCc &&  
         iAddHeadersVariation & EUniFeatureCc )
        {
        featureBitmask |= KMmsuiHeaderCC;
        }
    if ( aFlags & EUniFeatureBcc &&  
         iAddHeadersVariation & EUniFeatureBcc )
        {
        featureBitmask |= KMmsuiHeaderBCC;
        }
    
    // Don't save anything, which is variated off
    repository->Set( KMmsuiAdditionalHeaders, featureBitmask );
    delete repository;
    }

// ---------------------------------------------------------
// CUniBaseHeader::HasRecipients
// ---------------------------------------------------------
//
EXPORT_C TBool CUniBaseHeader::HasRecipients( )
    {
    TBool result( EFalse );
    
    for ( TInt i = EHeaderAddressTo; i <= EHeaderAddressBcc ; i++ )
        {    
        if ( iHeaders[i].iControl )
            {
            CMsgRecipientArray* recipients = NULL;
            TRAP_IGNORE( recipients = 
                            static_cast<CMsgAddressControl*>( iHeaders[i].iControl )->GetRecipientsL() );
            if ( recipients &&  
                 recipients->Count() > 0 )
                {
                result = ETrue;
                break;
                }
            }
        }
        
    return result;
    }

// ---------------------------------------------------------
// CUniBaseHeader::IsAddressControl
// ---------------------------------------------------------
//
EXPORT_C TBool CUniBaseHeader::IsAddressControl (TInt aId )
    {
    TBool result( EFalse );
    
    for ( TInt i = EHeaderAddressTo; i <= EHeaderAddressBcc ; i++ )
        {    
        if ( iHeaders[i].iControl &&  
             iHeaders[i].iControlType == aId )
            {
            result = ETrue;
            break;
            }
        }
        
    return result;
    }

// ---------------------------------------------------------
// CUniBaseHeader::HandleResourceChange
// ---------------------------------------------------------
//
EXPORT_C void CUniBaseHeader::HandleResourceChange( TInt aType )
    {
    for ( TInt i = EHeaderAddressFrom; i <= EHeaderAttachment ; i++ )
        {    
        if ( iHeaders[i].iControl &&  
             iHeaders[i].iOwned )
            {
            iHeaders[i].iControl->HandleResourceChange( aType );
            }
        }

    }


//  End of File
