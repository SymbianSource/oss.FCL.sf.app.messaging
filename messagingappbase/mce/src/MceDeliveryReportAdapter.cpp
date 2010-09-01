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
*     This class formats data from engine to listbox
*
*/



// INCLUDE FILES
#include <avkon.rsg>
#include <MceLogEng.rsg>
#include <calslbs.h>  // TextUtils
#include <StringLoader.h>

#include <featmgr.h>
#include <NumberGrouping.h>

#include "MceDeliveryReportAdapter.h"
#include "MceLogEngine.h"
#include "MceUtils.h"

// security data caging
#include <data_caging_path_literals.hrh>
#include <bldvariant.hrh>

// CONSTANTS
const TInt KTinyBufferLength = 5;
const TInt KSmallBufferLength = 30;
const TInt KMediumBufferLength = 128;
const TInt KLargeBufferLength = 256;
const TInt KInitConcItem = 0;
const TInt KFirstConcItemPos = 0;
const TInt KSecondConcItemPos = 2;
const TInt KConcItemLength = 1;
const TInt KPhoneNumberMaxLen = 48;
const TInt KListBoxCellId = 1;

// LOCAL CONSTANTS AND MACROS

_LIT( KMceDirAndFile,"mcelogeng.rSC" );

_LIT(KSmsEventType, "0\t");
_LIT(KMmsEventType, "1\t");
_LIT(KTab, "\t");

#ifdef RD_MSG_NAVIPANE_IMPROVEMENT
_LIT( KSeparator, "  " );
#else
_LIT( KSeparator, " - " );
#endif // RD_MSG_NAVIPANE_IMPROVEMENT
// ================= MEMBER FUNCTIONS =======================


// Two-phased constructor.
CMceDeliveryReportAdapter* CMceDeliveryReportAdapter::NewL
                            ( CMceLogEngine* aModel,
                              CEikFormattedCellListBox *aListBox )
    {
    CMceDeliveryReportAdapter* self = new( ELeave ) CMceDeliveryReportAdapter( aModel, aListBox );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// C++ default constructor can NOT contain any code that
// might leave.
//
CMceDeliveryReportAdapter::CMceDeliveryReportAdapter( CMceLogEngine* aModel, 
                                                      CEikFormattedCellListBox *aListBox  )
        : iModel( aModel ),        
        iResourceLoader( *CCoeEnv::Static() ),
        iListBox( aListBox )
    {
    TTime universal;
    TTime home;

    universal.UniversalTime();
    home.HomeTime();
    TInt err = home.MinutesFrom( universal, iInterval );

    if( err )
        {
        iInterval = 0;
        }
    }

// Symbian OS default constructor can leave.
void CMceDeliveryReportAdapter::ConstructL()
    {
    // security data caging
    TParse fp;
    fp.Set( KMceDirAndFile, &KDC_APP_RESOURCE_DIR , NULL );
    TFileName filename = fp.FullName();

    iResourceLoader.OpenL( filename );
    CEikonEnv* env = CEikonEnv::Static();

    iBuffer = HBufC::NewL( KLargeBufferLength );
    iTimeText = HBufC::NewL( KMediumBufferLength );
    iPendingText = env->AllocReadResourceL( R_MCE_LOG_ENG_PENDING_TEXT );
    iRead = env->AllocReadResourceL( R_MCE_LOG_ENG_READ_TEXT );  
    iFailedText = env->AllocReadResourceL( R_MCE_LOG_ENG_FAILED_TEXT );
    iDeliveredText = env->AllocReadResourceL( R_MCE_LOG_ENG_DELIVERED_TEXT );
    iTimeFormat = env->AllocReadResourceL( R_QTN_TIME_USUAL_WITH_ZERO );
    
#ifdef RD_MSG_NAVIPANE_IMPROVEMENT    
    iTimeDateFormat = env->AllocReadResourceL( R_QTN_DATE_WITHOUT_YEAR_WITH_ZERO );
#else
    iTimeDateFormat = env->AllocReadResourceL( R_QTN_DATE_SHORT_WITH_ZERO );
#endif // RD_MSG_NAVIPANE_IMPROVEMENT

    iModel->GetStandardStrings( iFailed, iPending );

    iConcItems = new( ELeave ) CArrayFixFlat<TInt>( 1 );
    iConcItems->AppendL( KInitConcItem );
    iConcItems->AppendL( KInitConcItem );

    iConcText = StringLoader::LoadL( R_MCE_LOG_ENG_CONCATENATED_TEXT,
                                            *iConcItems );

    // CenRep key KCRUidNumberGrouping / KNumberGrouping is used to check if
    // number grouping is supported
    if( AknLayoutUtils::LayoutMirrored() || MceUtils::NumberGroupingEnabled() )
        {
        iNumberGrouping = CPNGNumberGrouping::NewL( KPhoneNumberMaxLen );
        }
    }

    
// Destructor
CMceDeliveryReportAdapter::~CMceDeliveryReportAdapter()
    {
    delete iBuffer;
    delete iPendingText;
    delete iFailedText;
    delete iDeliveredText;
    delete iTimeFormat;
    delete iTimeDateFormat;
    delete iTimeText;
    delete iConcItems;
    delete iConcText;   
    delete iRead;   
    iResourceLoader.Close();

    delete iNumberGrouping;
    }


// from MDesCArray
TInt  CMceDeliveryReportAdapter::MdcaCount () const 
    {
    return iModel->Count();
    }
    
// from MDesCArray  
TPtrC16  CMceDeliveryReportAdapter::MdcaPoint( TInt aIndex ) const
    {
    TPtr des = iBuffer->Des();
    /// Double Graphics Style " <SMS/MMS 0/1> \t<Remote Party>\t<time>" - "<Status>" 
        
    const CLogEvent* event = iModel->Event( aIndex );
    
    /// Event type ICON index
    if( event->EventType() == KLogShortMessageEventTypeUid )
        {
        des.Format( KSmsEventType );
        }
    else
        {
        des.Format( KMmsEventType );
        }

    // If there is a "name" then we show it.
    if( event->RemoteParty().Length() &&
       ( event->RemoteParty() != event->Number() ) ) 
        {
        des.Append( event->RemoteParty() );
        }
    else
        {
        if( iNumberGrouping )
            {
            if( event->Number().Length() <= iNumberGrouping->MaxDisplayLength() )
                {
                iNumberGrouping->Set( event->Number() );
                TBuf<KMediumBufferLength> buf( iNumberGrouping->FormattedNumber() );
                AknTextUtils::ClipToFit
                                (   buf
                                ,   AknTextUtils::EClipFromBeginning
                                ,   iListBox
                                ,   aIndex
                                ,   KListBoxCellId
                                );
                // Arabic & Hebrew conversion if needed.
                AknTextUtils::LanguageSpecificNumberConversion( buf ); 
                des.Append( buf );
                }
            else
                {
                TBuf<KMediumBufferLength> buf( event->Number() );
                // Arabic & Hebrew conversion if needed.
                AknTextUtils::LanguageSpecificNumberConversion( buf ); 
                des.Append( buf );
                }
            }
        else
            {
            TBuf<KMediumBufferLength> buf(event->Number());
            AknTextUtils::ClipToFit
                                (   buf
                                ,   AknTextUtils::EClipFromBeginning
                                ,   iListBox
                                ,   aIndex
                                ,   KListBoxCellId
                                );
            // Arabic & Hebrew conversion if needed.
            AknTextUtils::LanguageSpecificNumberConversion( buf ); 
            des.Append( buf );
            }
        }

    des.Append(KTab);
    TTime currentTime;
    currentTime.HomeTime();

    TTime time( event->Time() + iInterval );
    TPtr timePtr = iTimeText->Des();

    TTimeIntervalHours timeDiff;
    currentTime.HoursFrom( time, timeDiff );

    HBufC* tmpTimeFormat;
    if( currentTime.DayNoInYear() != time.DayNoInYear() )
        {
        tmpTimeFormat = iTimeDateFormat;
        }
    else 
        {
        tmpTimeFormat = iTimeFormat;
        }
    TRAPD( err, time.FormatL( timePtr, *tmpTimeFormat ) );
    if( err == KErrNone )
        {
        AknTextUtils::LanguageSpecificNumberConversion( timePtr ); 
        des.Append( timePtr );
        }
    des.Append( KSeparator );

    if( event->Status() == iPending )
        {
        if( iModel->PurgeData( event, iConcItems ) > 1 )
            {            
            TBuf<KSmallBufferLength> numBuf;
            TBuf<KTinyBufferLength> item;

            numBuf.Copy( *iConcText );
            item.Num( iConcItems->At( 1 ) );
            numBuf.Replace( KSecondConcItemPos, KConcItemLength, item );
            item.Num( iConcItems->At( 0 ) );
            numBuf.Replace( KFirstConcItemPos, KConcItemLength, item );
            des.Append( numBuf );
            }
        des.Append( *iPendingText );
        }
    else if( event->Status() == iFailed )
        {
        des.Append( *iFailedText );
        }       
    else if( event->Status() == KLogsMsgReadText )  
        {
        des.Append( *iRead );
        }       
    else
        {
        des.Append( *iDeliveredText );
        }
       
    return iBuffer->Des();
    }

//  End of File 
