/*
* Copyright (c) 2002 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*           Execute Call command.
*
*/



// INCLUDE FILES
#include <coemain.h>
#include <AiwServiceHandler.h>  //CAiwServiceHandler
#include <AiwCommon.hrh>        //KAiwCmdCall, KAiwClassBase
#include <AiwCommon.h>          //RCriteriaArray, CAiwCriteriaItem
#include <AiwGenericParam.h>    //CAiwGenericParamList, TAiwGenericParam
#include "MuiuCallCmd.h"



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMuiuCallCmd::CMuiuCallCmd
//
// ---------------------------------------------------------
CMuiuCallCmd::CMuiuCallCmd ()
    {
    }

// ---------------------------------------------------------
// CMuiuCallCmd::NewL
//
// ---------------------------------------------------------
CMuiuCallCmd* CMuiuCallCmd::NewL()
    {
    CMuiuCallCmd* self = new( ELeave ) CMuiuCallCmd();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMuiuCallCmd::~CMuiuCallCmd
//
// ---------------------------------------------------------
CMuiuCallCmd::~CMuiuCallCmd()
    {
    }


// ---------------------------------------------------------
// CMuiuCallCmd::ExecuteLD
//
// Start dialing and show calling dialog
// ---------------------------------------------------------
void CMuiuCallCmd::ExecuteLD( const TDesC& aNumber, const TDesC& aName, TBool aCallTypeVoIP )
    {
    CleanupStack::PushL( this );
    // Create AIW service handler
    CAiwServiceHandler* serviceHandler = CAiwServiceHandler::NewLC();
    // Create AIW interest
    RCriteriaArray interest;
    CleanupClosePushL( interest );
    CAiwCriteriaItem* criteria = CAiwCriteriaItem::NewLC( 
                                 KAiwCmdCall, KAiwCmdCall,
                                 _L8( "*" ) );
    TUid base;
    base.iUid = KAiwClassBase;
    criteria->SetServiceClass( base );
    User::LeaveIfError( interest.Append( criteria ) );
    // Attach to AIW interest
    serviceHandler->AttachL( interest );
    iNameBuffer = aName.Left( iNameBuffer.MaxLength() );
    iTelNumber = aNumber.Left( iTelNumber.MaxLength() );
    CAiwDialDataExt* dialDataExt = CAiwDialDataExt::NewLC();
    // Set test parameters  
    dialDataExt->SetNameL( iNameBuffer );
    dialDataExt->SetPhoneNumberL( iTelNumber );
    if ( aCallTypeVoIP )
        {
        dialDataExt->SetCallType( CAiwDialData::EAIWVoiP );        
        }
    else
        {
        dialDataExt->SetCallType( CAiwDialData::EAIWVoice ); 
        }
    dialDataExt->SetWindowGroup( CCoeEnv::Static()->RootWin().Identifier() );
    CAiwGenericParamList& paramList = serviceHandler->InParamListL();
    dialDataExt->FillInParamListL( paramList );
    // Execute AIW command with notification callback
    serviceHandler->ExecuteServiceCmdL( KAiwCmdCall, 
                                         paramList,
                                         serviceHandler->OutParamListL(),
                                         0, 
                                         NULL );
    serviceHandler->DetachL( interest );
    CleanupStack::PopAndDestroy( 4 ); // criteria, interest, serviceHandler, dialDataExt
    CleanupStack::PopAndDestroy(); // this (Destroy itself as promised)
    }


// ---------------------------------------------------------
// CMuiuCallCmd::ConstructL
//
//	Symbian OS default constructor can leave.
// ---------------------------------------------------------
void CMuiuCallCmd::ConstructL()
    {
    }


//  End of File  
