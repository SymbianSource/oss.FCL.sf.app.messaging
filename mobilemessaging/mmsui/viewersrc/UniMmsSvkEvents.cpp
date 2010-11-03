/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*  Class which implements handling Side Volume Key events (SVK).
*  This class implements methods of MRemConCoreApiTargetObserver
*
*/



// INCLUDE FILES
#include <remconcoreapitarget.h>
#include <remconcoreapitargetobserver.h>
#include <remconinterfaceselector.h>
#include <aknconsts.h>                      // KAknStandardKeyboardRepeatRate

#include "UniMmsSvkEvents.h"


// CONSTANTS
const TInt KVolumeFirstChangePeriod = KAknStandardKeyboardRepeatRate;
const TInt KVolumeChangePeriod = KAknStandardKeyboardRepeatRate;
const TInt KVolumeChangeUp = 1;      
const TInt KVolumeChangeDown = -1;      

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CMmsSvkEvents
// ----------------------------------------------------------------------------
//
CMmsSvkEvents* CMmsSvkEvents::NewL( MMmsSvkEventsObserver& aObserver )
    {
    CMmsSvkEvents* self = 
        new ( ELeave ) CMmsSvkEvents( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------------------------------
// CMmsSvkEvents
// ----------------------------------------------------------------------------
//
CMmsSvkEvents::CMmsSvkEvents( MMmsSvkEventsObserver& aObserver ) :
    iObserver( aObserver )
    {    
    }

    
// ----------------------------------------------------------------------------
// CMmsSvkEvents::~CMmsSvkEvents
// ----------------------------------------------------------------------------
//
CMmsSvkEvents::~CMmsSvkEvents()
    {    
    if ( iVolumeTimer )
        {
        iVolumeTimer->Cancel();
        delete iVolumeTimer;
        }
    
    delete iInterfaceSelector;
    iCoreTarget = NULL; // For LINT. Owned by iInterfaceSelector
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CMmsSvkEvents::ConstructL( )
    {
    iInterfaceSelector = CRemConInterfaceSelector::NewL();

    // owned by iInterfaceSelector
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this ); 

    iInterfaceSelector->OpenTargetL();

    // Remote control server command repeat timer.
    iVolumeTimer = CPeriodic::NewL( EPriorityNormal );
    }

// ----------------------------------------------------------------------------
// CMmsSvkEvents::MrccatoCommand
// Handles side volume key events.
// ----------------------------------------------------------------------------
//
void CMmsSvkEvents::MrccatoCommand( TRemConCoreApiOperationId   aOperationId, 
                                    TRemConCoreApiButtonAction  aButtonAct )
    {
    TRequestStatus stat;
    switch ( aOperationId )
        {
        case ERemConCoreApiVolumeUp: //  Volume Up
            {
            switch ( aButtonAct )
                {
                case ERemConCoreApiButtonPress:
                    {
                    // Volume up hold down for 0,6 seconds
                    DoChangeVolume( );
                    iVolumeTimer->Cancel();
                    iVolumeTimer->Start(    KVolumeFirstChangePeriod,
                                            KVolumeChangePeriod, 
                                            TCallBack( ChangeVolume, this ) );
                    break;
                    }
                case ERemConCoreApiButtonRelease:
                    {
                    iVolumeTimer->Cancel();
                    break;
                    }
                case ERemConCoreApiButtonClick:
                    {
                    // Volume up clicked
                    iChange = KVolumeChangeUp;
                    DoChangeVolume( );
                    break;
                    }
                default:
                    {
                    // Never hits this
                    break;
                    }
                }
            iCoreTarget->VolumeUpResponse(stat, KErrNone);
            break;
            }
        case ERemConCoreApiVolumeDown: //  Volume Down
            {
            switch ( aButtonAct )
                {
                case ERemConCoreApiButtonPress:
                    {
                    // Volume down hold for 0,6 seconds
                    DoChangeVolume( );
                    iVolumeTimer->Cancel();
                    iVolumeTimer->Start(    KVolumeFirstChangePeriod,
                                            KVolumeChangePeriod, 
                                            TCallBack( ChangeVolume, this ) );
                    break;
                    }
                case ERemConCoreApiButtonRelease:
                    {
                    iVolumeTimer->Cancel();
                    break;
                    }
                case ERemConCoreApiButtonClick:
                    {
                    // Volume down clicked
                    iChange = KVolumeChangeDown;
                    DoChangeVolume( );
                    break;
                    }
                default:
                    {
                    // Never hits this
                    break;
                    }
                }
            iCoreTarget->VolumeDownResponse(stat, KErrNone);
            break;
            }
        default :
            {
            iCoreTarget->SendResponse( stat, aOperationId, KErrNone );
            break;
            }
        }
        
    User::WaitForRequest( stat );
    }

// ----------------------------------------------------------------------------
// ChangeVolumeL
// 
// ----------------------------------------------------------------------------
//
void CMmsSvkEvents::DoChangeVolume( )
    {
    TRAP_IGNORE( iObserver.MmsSvkChangeVolumeL( iChange ) );
    }

// ----------------------------------------------------------------------------
// ChangeVolume
// 
// ----------------------------------------------------------------------------
//
TInt CMmsSvkEvents::ChangeVolume( TAny* aObject )
    {
    // cast, and call non-static function
    static_cast<CMmsSvkEvents*>( aObject )->DoChangeVolume( );
    return KErrNone;
    }

// End of File
