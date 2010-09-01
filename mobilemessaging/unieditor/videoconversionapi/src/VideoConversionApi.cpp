/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides Video Conversion method
*
*/

 

 
// INCLUDE FILES
#include <e32base.h>
#include "VideoConversionApi.h"


// ============================ MEMBER FUNCTIONS ===============================
 
 
  
 // -----------------------------------------------------------------------------
// CVideoConversionPlugin::NewL
// Factory method for a specific implementation
// -----------------------------------------------------------------------------
//
EXPORT_C CVideoConversionPlugin* CVideoConversionPlugin::NewL( const TUid aImplUid  )
    {   
    TAny* defaultPlugin = REComSession::CreateImplementationL(
                aImplUid,
                _FOFF( CVideoConversionPlugin, iDtor_ID_Key ));   
                         
    return  ( REINTERPRET_CAST(CVideoConversionPlugin*,(defaultPlugin)) ); 
    }   

// Destructor
EXPORT_C CVideoConversionPlugin::~CVideoConversionPlugin()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
    
    


//  End of File
