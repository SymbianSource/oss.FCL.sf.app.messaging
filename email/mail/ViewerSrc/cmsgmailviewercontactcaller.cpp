/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Mail viewer contact caller.
*
*/

#include "cmsgmailviewercontactcaller.h"
#include "cmsgmailviewercontactmatcher.h"
#include "MailLog.h"
#include "MailUtils.h"
#include "msgmailviewer.hrh"
#include <MsgMailViewer.rsg>
#include <AiwServiceHandler.h>
#include <eikmenup.h>
#include <aiwdialdataext.h>

// Constants 
    
// ---------------------------------------------------------------------------
// CMsgMailViewerContactCaller
// ---------------------------------------------------------------------------
//
CMsgMailViewerContactCaller::CMsgMailViewerContactCaller(
    CAiwServiceHandler& aServiceHandler,
    CMsgMailViewerContactMatcher& aContactMatcher )
    : iServiceHandler( aServiceHandler ),
      iContactMatcher( aContactMatcher )
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
inline void CMsgMailViewerContactCaller::ConstructL()
    {    
    iServiceHandler.AttachMenuL ( R_MSGMAILVIEWER_CALL_MENU,
                                  R_MSGMAILVIEWER_CALL_INTEREST );
                                  
    // Attach also base interest
    iServiceHandler.AttachL( R_MSGMAILVIEWER_CALL_INTEREST );
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CMsgMailViewerContactCaller* CMsgMailViewerContactCaller::NewL(
    CAiwServiceHandler& aServiceHandler,
    CMsgMailViewerContactMatcher& aContactMatcher )
    {
    CMsgMailViewerContactCaller* self =
        new( ELeave ) CMsgMailViewerContactCaller( aServiceHandler,
                                                   aContactMatcher );
    CleanupStack::PushL( self );
    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CMsgMailViewerContactCaller
// ---------------------------------------------------------------------------
//
CMsgMailViewerContactCaller::~CMsgMailViewerContactCaller()
    {
    } 
 
// ------------------------------------------------------------------------------
// CMsgMailViewerContactCaller::DynInitMenuPaneL()
// ------------------------------------------------------------------------------
//    
void CMsgMailViewerContactCaller::DynInitMenuPaneL(
    TInt aResourceId, 
    CEikMenuPane* aMenuPane,
    TBool aShowCallOption )
    {
	if ( iServiceHandler.HandleSubmenuL( *aMenuPane ) )
		{
		return;
		}
	if ( aResourceId == R_MSGMAILVIEWER_CALL_MENU )
		{
		if ( aShowCallOption )
		    {
            iServiceHandler.InitializeMenuPaneL(
                *aMenuPane, 
                R_MSGMAILVIEWER_CALL_MENU,
                EMsgMailViewerCmdAIWRangeStart,
                iServiceHandler.InParamListL() );
		    }
        else
            {
            aMenuPane->SetItemDimmed( KAiwCmdCall, ETrue );
            }
		}      
    }
     
// ------------------------------------------------------------------------------
// CMsgMailViewerContactCaller::CanHandleCommand()
// ------------------------------------------------------------------------------
//
TBool CMsgMailViewerContactCaller::CanHandleCommand( TInt aCommand ) const
    {
    TBool retVal( EFalse );
    
    if(aCommand == EMsgMailViewerCmdCall)
        {
        retVal = ETrue;
        }
    else
        {
        TInt aiwCommand =  iServiceHandler.ServiceCmdByMenuCmd( aCommand );
        LOG1("iServiceHandler.ServiceCmdByMenuCmd:%d", aiwCommand );
        retVal = ( aiwCommand == KAiwCmdCall );
        }

    return retVal;
    }
     
// ------------------------------------------------------------------------------
// CMsgMailViewerContactCaller::CallToContactL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerContactCaller::CallToContactL(
    TInt aCommand,
    const TDesC& aEmailAddress )
    {
    ASSERT( CanHandleCommand( aCommand ) );
    iCommand = aCommand;
    iContactMatcher.FindContactL( aEmailAddress,
                                  CMsgMailViewerContactMatcher::ECallToContact,
                                  TCallBack( MatchDoneL, this ) );
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerContactCaller::MatchDoneL()
// ------------------------------------------------------------------------------
//     
TInt CMsgMailViewerContactCaller::MatchDoneL( TAny* aCaller )	// CSI: 40 # We must return 
																// the integer value although this 
																// is a leaving method.
    {    
    CMsgMailViewerContactCaller* caller =
        static_cast<CMsgMailViewerContactCaller*>( aCaller );
    caller->DoCallToContactL();
    return 0;
    }     
     
// ------------------------------------------------------------------------------
// CMsgMailViewerContactCaller::DoCallToContactL()
// ------------------------------------------------------------------------------
//     
void CMsgMailViewerContactCaller::DoCallToContactL()
    {
    HBufC* number = iContactMatcher.GetDataL();
    if ( number )
        {
        CleanupStack::PushL( number );        
        CAiwGenericParamList& paramList = iServiceHandler.InParamListL();
        
        CAiwDialDataExt* dialData = CAiwDialDataExt::NewLC();

        dialData->SetPhoneNumberL( *number ); 
        HBufC* name = iContactMatcher.GetNameL();        
        if ( name )
            {            
            CleanupStack::PushL( name );
        
            dialData->SetNameL( name->Left( AIWDialDataExt::KMaximumNameLength ) );
            CleanupStack::PopAndDestroy( name );      
            }
            
        dialData->SetWindowGroup( CCoeEnv::Static()->RootWin().Identifier() );
        dialData->FillInParamListL( paramList );
        
    //Internal command which comes when user has pressed send key.
    if ( iCommand == EMsgMailViewerCmdCall )
           {
           iServiceHandler.ExecuteServiceCmdL( KAiwCmdCall,
                                        paramList,
                                        iServiceHandler.OutParamListL() );
                                        
           }
        //Aiw command from menu
    	else
    	   {
    	   iServiceHandler.ExecuteMenuCmdL( iCommand,
    									 paramList,
    									 iServiceHandler.OutParamListL() );    
    	   }
    	    
        CleanupStack::PopAndDestroy(2);				// CSI: 47 # number, dial
        }
    else
        {
        MailUtils::InformationNoteL( R_MSGMAILVIEWER_NO_MATCHES_NOTE );
        }

    }
             
// End of File

