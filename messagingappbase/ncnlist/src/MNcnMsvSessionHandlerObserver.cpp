/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Methods for MNcnMsvSessionHandlerObserver class.
*
*/



// INCLUDE FILES
#include    "MNcnMsvSessionHandlerObserver.h"
#include    "NcnDebug.h"
#include    <msvapi.h>                          // For CMsvSession

// ================= LOCAL CONSTANTS =======================
namespace
    {
    }

// ================= MEMBER FUNCTIONS =======================

// -------------------------------------------------------
//  MNcnMsvSessionHandlerObserver::HandleMsvSessionReadyL
// -------------------------------------------------------
// 
void MNcnMsvSessionHandlerObserver::HandleMsvSessionReadyL( CMsvSession& /*aMsvSession*/ )
    {
    // default implementation
    NCN_RDEBUG( _L( "MNcnMsvSessionHandlerObserver::HandleMsvSessionReadyL - default method called" ) );
    }

// -------------------------------------------------------
//  MNcnMsvSessionHandlerObserver::HandleMsvSessionClosedL
// -------------------------------------------------------
//
void MNcnMsvSessionHandlerObserver::HandleMsvSessionClosedL()
    {
    // default implementation
    NCN_RDEBUG( _L( "MNcnMsvSessionHandlerObserver::HandleMsvSessionClosedL - default method called" ) );
    }

// -------------------------------------------------------
//  MNcnMsvSessionHandlerObserver::HandleMsvEntryCreatedL
// -------------------------------------------------------
//    
void MNcnMsvSessionHandlerObserver::HandleMsvEntryCreatedL( const TMsvId& /*aMsvId*/ )
    {
    // default implementation
    NCN_RDEBUG( _L( "MNcnMsvSessionHandlerObserver::HandleMsvEntryCreatedL - default method called" ) );
    }

// -------------------------------------------------------
//  MNcnMsvSessionHandlerObserver::HandleMsvEntryDeletedL
// -------------------------------------------------------
//    
void MNcnMsvSessionHandlerObserver::HandleMsvEntryDeletedL( const TMsvId& /*aMsvId*/ )
    {
    // default implementation
    NCN_RDEBUG( _L( "MNcnMsvSessionHandlerObserver::HandleMsvEntryDeletedL - default method called" ) );
    }        
        
// -------------------------------------------------------
//  MNcnMsvSessionHandlerObserver::HandleMsvMediaChangedL
// -------------------------------------------------------
//    
void MNcnMsvSessionHandlerObserver::HandleMsvMediaChangedL( const TDriveNumber& /*aDriveNumber*/  )
    {
    // default implementation
    NCN_RDEBUG( _L( "MNcnMsvSessionHandlerObserver::HandleMsvMediaChangedL - default method called" ) );
    } 
    
// -------------------------------------------------------
//  MNcnMsvSessionHandlerObserver::HandleMsvMediaUnavailableL
// -------------------------------------------------------
//    
void MNcnMsvSessionHandlerObserver::HandleMsvMediaUnavailableL( )
    {
    // default implementation
    NCN_RDEBUG( _L( "MNcnMsvSessionHandlerObserver::HandleMsvMediaUnavailableL - default method called" ) );
    } 

// -------------------------------------------------------
//  MNcnMsvSessionHandlerObserver::HandleMsvMediaAvailableL
// -------------------------------------------------------
//    
void MNcnMsvSessionHandlerObserver::HandleMsvMediaAvailableL( const TDriveNumber& /*aDriveNumber*/  )
    {
    // default implementation
    NCN_RDEBUG( _L( "MNcnMsvSessionHandlerObserver::HandleMsvMediaAvailableL - default method called" ) );
    }
    
//  End of File
