/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     CTestVideoConversion implementation*
*/


#include <VideoConversionApi.h>
#include "VideoConversionApiTest.h"
#include "TestVideoConversion.h"

// -----------------------------------------------------------------------------
// CTestVideoConversion::CTestVideoConversion
// Constructor
// -----------------------------------------------------------------------------
//
CTestVideoConversion::CTestVideoConversion( CStifLogger* aLog ):iConverter( NULL ),
                                                                iLog( aLog )                    
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CTestVideoConversion::~CTestVideoConversion()
// Destructor 
// -----------------------------------------------------------------------------
//
CTestVideoConversion::~CTestVideoConversion()
    {
    if( iConverter )
        {
        delete iConverter;
        iConverter = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CTestVideoConversion::NewLC
// -----------------------------------------------------------------------------
//
CTestVideoConversion* CTestVideoConversion::NewLC( CStifLogger* aLog )
    {
    CTestVideoConversion* self = new (ELeave)CTestVideoConversion( aLog );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CTestVideoConversion::NewL
// -----------------------------------------------------------------------------
//
CTestVideoConversion* CTestVideoConversion::NewL( CStifLogger* aLog )
    {
    CTestVideoConversion* self=CTestVideoConversion::NewLC( aLog );
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CTestVideoConversion::ConstructL()
// Checks for available implementations and creates converter
// -----------------------------------------------------------------------------
//
void CTestVideoConversion::ConstructL()
    {
    iLog->Log(_L("Entry_CTestVideoConversion::ConstructL"));
    RImplInfoPtrArray implArray;
    TInt error = KErrNone;
    //Available implementations are listed to array
    iLog->Log(_L("Listing available implementations"));
    CVideoConversionPlugin::ListImplementationsL( implArray );
    if ( implArray.Count() > 0 )
        {
        //if any implementation (video converter) is present it's object is created
        //NewL is called here
        iLog->Log(_L("Found >=1 implementaions of CVideoConversion Plugin & calling NewL"));
        iConverter = CVideoConversionPlugin::NewL( implArray[0]->ImplementationUid() );    
        iLog->Log(_L("Plugin succesfully created "));
        }
    else
        {
        iLog->Log(_L("No implementations of CVideoConversion Plugin found"));
        error = KErrNotFound;
        }
    implArray.ResetAndDestroy();
    implArray.Close();
    if( error == KErrNotFound )
        {
        User::Leave( error );
        }
    else
        {
        iLog->Log(_L("Exit_CTestVideoConversion::ConstructL"));
        }
    }

// -----------------------------------------------------------------------------
// CTestVideoConversion::VideoConversionDone
// This function is implemented to get callback from MVideoConversionCallback
// observer
// -----------------------------------------------------------------------------
//
void CTestVideoConversion::VideoConversionDone( TInt /*aStatus*/ )
    {
    
    }

//End of file
