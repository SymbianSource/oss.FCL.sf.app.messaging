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
* Description: SMSICDMA.CPP*
*/




// Standard includes
#include <mtmdef.h>     // MTM specific definitions for constants
#include <eikon.rsg>    // Standard resources
// Messaging includes
#include <msvuids.h>    // Msv UIDs
// Specific includes
#include <smum.rsg>                     // for resource IDs
#include <muiu.mbg>                     // bitmaps
#include <avkon.rsg>                    // for resource IDs
#include <StringLoader.h>               // StringLoader
#include "smsicdma.h"                       
#include "smsu.hrh"                     // for KMtmUiFunctionSimDialog
#include "smsui.pan"                    // for panics
#include <aknsutils.h>
#include <aknsconstants.h>              // skinned icon ids
#include <data_caging_path_literals.hrh>// Path for Bitmap 

//  LOCAL CONSTANTS AND MACROS
const TInt KSmsiNumberOfZoomStates = 2; // Nothing to do with zoomstates, picture + mask = 2
const TUid KUidMsgTypeSMS = {0x1000102C};

_LIT(KBitmapFile, "muiu.MBM");

//  MEMBER FUNCTIONS

// Factory constructor function
CSmsMtmUiDataCDMA* CSmsMtmUiDataCDMA::NewL( CRegisteredMtmDll& aRegisteredDll )
    {
    CSmsMtmUiDataCDMA* smsmtmuidata=new ( ELeave ) CSmsMtmUiDataCDMA( aRegisteredDll );
    CleanupStack::PushL( smsmtmuidata );
    smsmtmuidata->ConstructL();
    CleanupStack::Pop();
    return smsmtmuidata;
    }

// C++ constructor can NOT contain any code, that
// might leave.
CSmsMtmUiDataCDMA::CSmsMtmUiDataCDMA( CRegisteredMtmDll& aRegisteredDll ):
    CSmsMtmUiData( aRegisteredDll )
    {
    }

// Destructor
CSmsMtmUiDataCDMA::~CSmsMtmUiDataCDMA()
    {
    }

// ---------------------------------------------------------
// CSmsMtmUiData::ContextIcon
//
// ---------------------------------------------------------
const CBaseMtmUiData::CBitmapArray& CSmsMtmUiDataCDMA::ContextIcon(
    const TMsvEntry& aContext, 
    TInt /*aStateFlags*/ ) const
    {
    __ASSERT_DEBUG(aContext.iMtm==KUidMsgTypeSMS, Panic(ESmsiWrongMtm));
    __ASSERT_DEBUG(aContext.iType.iUid!=KUidMsvFolderEntryValue, Panic(ESmsiFoldersNotSupported));
    __ASSERT_DEBUG(aContext.iType.iUid!=KUidMsvAttachmentEntryValue, Panic(ESmsiNoIconForAttachment));


    // Return the set of icons appropriate to the current context
	// BIO type messages
    TInt bmpIndex; 
    if (aContext.iBioType!=0)
        {
        bmpIndex = aContext.Unread() ? 
            EMbmMuiuQgn_prop_mce_smart_unread : EMbmMuiuQgn_prop_mce_smart_read;
        }
	// Other SMS Messages
    else
        {
		TUint sendingState = aContext.SendingState();
		switch( sendingState )
			{
			case KMsvSendStateFailed:
				
				if(aContext.Priority() == EMsvHighPriority)
					{
					// Message priority is "urgent"
					bmpIndex = EMbmMuiuQgn_prop_sms_failed_urgent_cdma;
					}
				else
					{
					// Message priority is "normal"
					bmpIndex = EMbmMuiuQgn_prop_sms_failed_cdma;
					}

				break;

			case KMsvSendStateUponRequest: // Mapped to be the same
			case KMsvSendStateSuspended:
				if(aContext.Priority() == EMsvHighPriority)
					{
					// Message priority is "urgent"
					bmpIndex = EMbmMuiuQgn_prop_sms_pending_urgent_cdma;
					}
				else
					{
					// Message priority is "normal"
					bmpIndex = EMbmMuiuQgn_prop_sms_pending_cdma;
					}
				break;
        
			case KMsvSendStateScheduled: // Mapped to be the same
			case KMsvSendStateWaiting:
				if(aContext.Priority() == EMsvHighPriority)
					{
					// Message priority is "urgent"
					bmpIndex = EMbmMuiuQgn_prop_sms_waiting_urgent_cdma;
					}
				else
					{
					// Message priority is "normal"
					bmpIndex = EMbmMuiuQgn_prop_sms_waiting_cdma;
					}
				break;
            
			case KMsvSendStateResend:
				if(aContext.Priority() == EMsvHighPriority)
					{
					// Message priority is "urgent"
					bmpIndex = EMbmMuiuQgn_prop_sms_pending_urgent_cdma;
					}
				else
					{
					// Message priority is "normal"
					bmpIndex = EMbmMuiuQgn_prop_sms_pending_cdma;
					}
				break;

			case KMsvSendStateSent:
				if(aContext.Priority() == EMsvHighPriority)
					{
					// Message priority is "urgent"
					bmpIndex = EMbmMuiuQgn_prop_sms_sent_urgent_cdma;
					}
				else
					{
					// Message priority is "normal"
					bmpIndex = EMbmMuiuQgn_prop_sms_sent_cdma;
					}
				break;

			case KMsvSendStateNotApplicable: // MT
				// Check message priority to return the correct set of icons		
				if(aContext.Priority() == EMsvHighPriority)
					{
					// Message priority is "urgent"
					bmpIndex = aContext.Unread() ?
							   EMbmMuiuQgn_prop_mce_sms_unread_urgent : EMbmMuiuQgn_prop_mce_sms_read_urgent;
					}
				else
					{
					// Message priority is "normal"
					bmpIndex = aContext.Unread() ?
							   EMbmMuiuQgn_prop_mce_sms_unread : EMbmMuiuQgn_prop_mce_sms_read;
					}
				break;
				
			default:
			    {
			    bmpIndex = ( aContext.Priority() == EMsvHighPriority ) ?
			        EMbmMuiuQgn_prop_mce_sms_unread_urgent : EMbmMuiuQgn_prop_mce_sms_unread;
			    break;
			    }
			}
		}
	// Bitmaps are no longer in sequential order, need a switch case to get the correct index
	TInt i;
	switch( bmpIndex )
			{
			// Add to iIconArrays in this order
			case EMbmMuiuQgn_prop_mce_sms_read:
				i=0;
				break;
			case EMbmMuiuQgn_prop_mce_sms_unread:
				i=1;
				break;
			case EMbmMuiuQgn_prop_mce_smart_read:
				i=2;
				break;
			case EMbmMuiuQgn_prop_mce_smart_unread:
				i=3;
				break;
			case EMbmMuiuQgn_prop_mce_sms_read_urgent:
				i=4;
				break;
			case EMbmMuiuQgn_prop_mce_sms_unread_urgent:
				i=5;
				break;
			case EMbmMuiuQgn_prop_sms_waiting_cdma:
				i=6;
				break;
			case EMbmMuiuQgn_prop_sms_failed_cdma:
				i=7;
				break;
			case EMbmMuiuQgn_prop_sms_sent_cdma:
				i=8;
				break;
			case EMbmMuiuQgn_prop_sms_pending_cdma:
				i=9;
				break;
			case EMbmMuiuQgn_prop_sms_delivered_cdma:
				i=10;
				break;
			case EMbmMuiuQgn_prop_sms_waiting_urgent_cdma:
				i=11;
				break;
			case EMbmMuiuQgn_prop_sms_failed_urgent_cdma:
				i=12;
				break;
			case EMbmMuiuQgn_prop_sms_sent_urgent_cdma:
				i=13;
				break;
			case EMbmMuiuQgn_prop_sms_pending_urgent_cdma:
				i=14;
				break;
			case EMbmMuiuQgn_prop_sms_delivered_urgent_cdma:
				i=15;
				break;
			default:
				i=0;
				break;
			}
		return *iIconArrays->At(i);
	}

// ---------------------------------------------------------
// CSmsMtmUiDataCDMA::QueryCapability
//
// ---------------------------------------------------------
TInt CSmsMtmUiDataCDMA::QueryCapability( TUid aFunctionId, TInt& aResponse ) const
    {
    TInt retVal = CSmsMtmUiData::QueryCapability( aFunctionId, aResponse );
    switch ( aFunctionId.iUid )
        {
        case KUidMtmQueryCanSendMsgValue:        
        case KUidMtmQueryCanReceiveMsgValue:        
        case KUidMtmQueryCanCreateNewMsgValue: 
	        aResponse = ETrue;
            break;
        default:
            break;
        }
    return retVal;
    }

// ---------------------------------------------------------
// CSmsMtmUiData::PopulateArraysL
//
// ---------------------------------------------------------
void CSmsMtmUiDataCDMA::PopulateArraysL()
    {
    // Read additional functions
    ReadFunctionsFromResourceFileL( R_SMSI_FUNCTION_ARRAY_CDMA );

	// Trap the error
	TRAPD(err,CreateSkinnedBitmapsL( KSmsiNumberOfZoomStates ));
	if(err != KErrNone)
		User::Leave(err);

    }

// ---------------------------------------------------------
// CSmsMtmUiData::CreateSkinnedBitmapsL
//
// ---------------------------------------------------------

void CSmsMtmUiDataCDMA::CreateSkinnedBitmapsL( TInt aNumZoomStates )
    {
	MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    TAknsItemID id;
    CFbsBitmap* bitmap;
    CFbsBitmap* bitmapMask;
    TBool found = ETrue;
    
    CBitmapArray* array;
    
    
    for( TInt i=EMbmMuiuQgn_prop_mce_sms_read; i<=	EMbmMuiuQgn_prop_sms_delivered_urgent_cdma_mask; i++ )
        {
        found = ETrue;
        switch( i )
            {
            // Add to iIconArrays in this order
            case EMbmMuiuQgn_prop_mce_sms_read:
                id.Set( KAknsIIDQgnPropMceSmsRead );
                break;
            case EMbmMuiuQgn_prop_mce_sms_unread:
                id.Set( KAknsIIDQgnPropMceSmsUnread );
                break;
            case EMbmMuiuQgn_prop_mce_smart_read:
                id.Set( KAknsIIDQgnPropMceSmartRead );
                break;
            case EMbmMuiuQgn_prop_mce_smart_unread:
                id.Set( KAknsIIDQgnPropMceSmartUnread );
                break;
			case EMbmMuiuQgn_prop_mce_sms_read_urgent:
                id.Set( KAknsIIDQgnPropMceSmsReadUrgent );
                break;
			case EMbmMuiuQgn_prop_mce_sms_unread_urgent:
                id.Set( KAknsIIDQgnPropMceSmsUnreadUrgent );
                break;
			case EMbmMuiuQgn_prop_sms_waiting_cdma:
				id.Set( KAknsIIDQgnPropSmsWaitingCdma );
				break;
			case EMbmMuiuQgn_prop_sms_failed_cdma:
				id.Set( KAknsIIDQgnPropSmsFailedCdma );
				break;
			case EMbmMuiuQgn_prop_sms_sent_cdma:
				id.Set( KAknsIIDQgnPropSmsSentCdma );
				break;
			case EMbmMuiuQgn_prop_sms_pending_cdma:
				id.Set( KAknsIIDQgnPropSmsPendingCdma );
				break;
			case EMbmMuiuQgn_prop_sms_delivered_cdma:
				id.Set( KAknsIIDQgnPropSmsDeliveredCdma );
				break;
			case EMbmMuiuQgn_prop_sms_waiting_urgent_cdma:
				id.Set( KAknsIIDQgnPropSmsWaitingUrgentCdma );
				break;
			case EMbmMuiuQgn_prop_sms_failed_urgent_cdma:
				id.Set( KAknsIIDQgnPropSmsFailedUrgentCdma );
				break;
			case EMbmMuiuQgn_prop_sms_sent_urgent_cdma:
				id.Set( KAknsIIDQgnPropSmsSentUrgentCdma );
				break;
			case EMbmMuiuQgn_prop_sms_pending_urgent_cdma:
				id.Set( KAknsIIDQgnPropSmsPendingUrgentCdma );
				break;
			case EMbmMuiuQgn_prop_sms_delivered_urgent_cdma:
				id.Set( KAknsIIDQgnPropSmsDeliveredUrgentCdma );
				break;
            default:
                found = EFalse;
                break;
            }
            if( found )
                {
                array = new(ELeave) CArrayPtrFlat<CFbsBitmap>(aNumZoomStates);
                CleanupStack::PushL( array );
				
				// Parse appends the correct resource path
				// /resource/apps or /system/data
				//
				TParse tp;
				tp.Set( KBitmapFile, &KDC_APP_BITMAP_DIR, NULL);  
				
                AknsUtils::CreateIconL( skins, id, bitmap,
                    bitmapMask, tp.FullName(), i, i + 1 );
                    
                array->AppendL(bitmap);
                array->AppendL(bitmapMask);
                iIconArrays->AppendL(array);

                CleanupStack::Pop();//array
                }                
        }
    }

//  End of File
