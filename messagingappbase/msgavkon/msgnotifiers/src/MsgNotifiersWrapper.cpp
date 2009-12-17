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
* Description:   Methods for MsgNotifiers Ecom Plugins entry point
*                 This is a proxy to use MsgNotifiers.
*
*/



// INCLUDE FILES

#include <implementationproxy.h>
#include <eiknotapi.h>
#include <AknNotifierWrapper.h>

#include "MsgNotifiersConst.h"

_LIT( KMsgNotifiersDll, "MsgNotifiers.dll" ); 


// ----------------------------------------------------------------------------
// DoCreateNotifierArrayL
// ----------------------------------------------------------------------------

void DoCreateNotifierArrayL( 
    CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers )
    {
    CAknCommonNotifierWrapper* master = 
            CAknCommonNotifierWrapper::NewL( KSendAsNotifierPluginUid,
                                             KSendAsNotifierPluginUid,
                                             MEikSrvNotifierBase2::ENotifierPriorityHigh,
                                             KMsgNotifiersDll,
                                             1 ); // no synchronous reply used.
    CleanupStack::PushL( master );    
    aNotifiers->AppendL( master ); 
    CleanupStack::Pop( master );
    
    CAknNotifierWrapperLight* light = new ( ELeave ) CAknNotifierWrapperLight(
                                             *master,
                                             KSendUiNotifierPluginUid,
                                             KSendUiNotifierPluginUid,
                                             MEikSrvNotifierBase2::ENotifierPriorityHigh );
    
    CleanupStack::PushL( light );    
    aNotifiers->AppendL( light ); 
    CleanupStack::Pop( light );
    }

// ----------------------------------------------------------------------------
// NotifierArray()
// ----------------------------------------------------------------------------
CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers = NULL;

    TRAPD( err, notifiers = new (ELeave) CArrayPtrFlat<MEikSrvNotifierBase2>( 2 ) );

    if ( err == KErrNone )
        {
        if ( notifiers )
            {
            TRAPD( err, DoCreateNotifierArrayL( notifiers ));
            if( err )
                {
                TInt count = notifiers->Count();
                while(count--)
                    {
                    (*notifiers)[count]->Release();
                    }
                delete notifiers;
                notifiers = NULL;
                }
            }
        }
    return notifiers;
    }


// ----------------------------------------------------------------------------
// ECOM 
// ----------------------------------------------------------------------------

const TImplementationProxy ImplementationTable[] =
    {
#ifdef __EABI__
    {{0x1028289B},(TFuncPtr)NotifierArray},
#else
    {{0x1028289B},NotifierArray},
#endif
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }
    
//  End of File
