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
* Description:  Factory for view plug-ins.
*
*/


//  INCLUDE FILES
#include "MailLog.h"
#include <e32std.h>
#include <ecom/ecom.h>
#include "CMailViewFactory.h"
#include <CMailMessageView.h>

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// MailViewFactory::CreateAdaptersL
// -----------------------------------------------------------------------------
//
EXPORT_C CArrayPtr<CMailMessageView>* MailViewFactory::CreateViewPluginsL()
    {
    LOG("MailViewFactory::CreateViewPluginsL()");
    // Create an array for adapter information
    RImplInfoPtrArray implArray;
    CleanupStack::PushL( TCleanupItem( CleanupImplArray, &implArray ) );

    // Get the list of adapters
    CMailMessageView::ListL( implArray );

    // Sort them in priority order
    implArray.Sort(
        TLinearOrder<CImplementationInformation>( MailViewFactory::Compare ) );

    // Create an array for adapters
	LOG1( "CreateViewPluginsL: implArray.Count():%d", implArray.Count() );
    CArrayPtr<CMailMessageView>* views =
        new(ELeave) CArrayPtrFlat<CMailMessageView>( Max( 1, implArray.Count() ) );
    CleanupStack::PushL( TCleanupItem( CleanupAdapterArray, views ) );
    // Create the adapters
    for( TInt i( 0 ); i < implArray.Count(); i++ )
        {
        CImplementationInformation& info = *implArray[i];
        TUid implementation( info.ImplementationUid() );
        
       	CMailMessageView* view = NULL;
       	//Create the plug-in view
		view = CMailMessageView::NewL( implementation );
		
        if( view )
            {
            CleanupStack::PushL( view );
            views->AppendL( view );
            LOG("CreateViewPluginsL: views->AppendL");
            CleanupStack::Pop( view );
            }
        }

    CleanupStack::Pop( views );
    CleanupStack::PopAndDestroy(); // implArray

    return views;
    }

// -----------------------------------------------------------------------------
// MailViewFactory::CleanupImplArray
// -----------------------------------------------------------------------------
//
void MailViewFactory::CleanupImplArray( TAny* aAny )
    {
    RImplInfoPtrArray* implArray =
        reinterpret_cast<RImplInfoPtrArray*>( aAny );
    implArray->ResetAndDestroy();
    implArray->Close();
    }

// -----------------------------------------------------------------------------
// MailViewFactory::CleanupAdapterArray
// -----------------------------------------------------------------------------
//
void MailViewFactory::CleanupAdapterArray( TAny* aAny )
    {
    CArrayPtr<CMailMessageView>* views =
        reinterpret_cast<CArrayPtr<CMailMessageView>*>( aAny );
    views->ResetAndDestroy();
    delete views;
    }

// -----------------------------------------------------------------------------
// MailViewFactory::Compare
// -----------------------------------------------------------------------------
//
TInt MailViewFactory::Compare( const CImplementationInformation& aImpl1,
                               const CImplementationInformation& aImpl2 )
    {
    // Compare the numerical values of opaque_data
    TLex8 lex( aImpl1.OpaqueData() );
    TInt impl1( KMaxTInt );
    if( lex.Val( impl1 ) != KErrNone )
        {
        impl1 = KMaxTInt;
        }

    lex.Assign( aImpl2.OpaqueData() );
    TInt impl2( KMaxTInt );
    if( lex.Val( impl2 ) != KErrNone )
        {
        impl2 = KMaxTInt;
        }

    return impl1 - impl2;
    }

//  End of File
