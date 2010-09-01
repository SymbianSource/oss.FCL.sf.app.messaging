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
*     Image conversion
*
*/



// INCLUDE FILES
#include    "OperatorLogoOTAConv.h"
#include    <imageconversion.h>
#include    <StringLoader.h>               // for StringLoader (iCoeEnv)

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COperatorLogoOTAConv::COperatorLogoOTAConv
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
COperatorLogoOTAConv::COperatorLogoOTAConv() : 
    CActive( EPriorityNormal )
    {
    // Empty
    }

// -----------------------------------------------------------------------------
// COperatorLogoOTAConv::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void COperatorLogoOTAConv::ConstructL()
    {    
    CActiveScheduler::Add( this );    
    }

// -----------------------------------------------------------------------------
// COperatorLogoOTAConv::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
COperatorLogoOTAConv* COperatorLogoOTAConv::NewL()
    {
    COperatorLogoOTAConv* self = NewLC();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// COperatorLogoOTAConv::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
COperatorLogoOTAConv* COperatorLogoOTAConv::NewLC()
    {
    COperatorLogoOTAConv* self = new( ELeave ) COperatorLogoOTAConv();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// Destructor
COperatorLogoOTAConv::~COperatorLogoOTAConv()
    {
    }

// -----------------------------------------------------------------------------
// COperatorLogoOTAConv::ConvertImageL
// -----------------------------------------------------------------------------
//
void COperatorLogoOTAConv::ConvertImageL( const TDesC& aFile, CFbsBitmap& aBitmap )
    {

    CImageDecoder* decoder =  
        CImageDecoder::FileNewL( CCoeEnv::Static()->FsSession(), aFile, 
            CImageDecoder::EOptionNone, KImageTypeOTAUid, KNullUid, KNullUid );
    
    CleanupStack::PushL( decoder );

    TFrameInfo info( decoder->FrameInfo() );

    // Create the bitmap
    aBitmap.Create( info.iOverallSizeInPixels, info.iFrameDisplayMode );

    decoder->Convert( &iStatus, aBitmap );
    SetActive();
    StartWait();
    CleanupStack::PopAndDestroy(); // decoder
   
    }

// -----------------------------------------------------------------------------
// COperatorLogoOTAConv::StartWait
// -----------------------------------------------------------------------------
//
void COperatorLogoOTAConv::StartWait()
    {
    if( !iWait.IsStarted() )
        {
        iWait.Start();
        }
    }

// -----------------------------------------------------------------------------
// COperatorLogoOTAConv::StopWait
// -----------------------------------------------------------------------------
//
void COperatorLogoOTAConv::StopWait()
    {
    if( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// -----------------------------------------------------------------------------
// COperatorLogoOTAConv::RunL
// -----------------------------------------------------------------------------
//
void COperatorLogoOTAConv::RunL()
    {
    StopWait();
    }

// -----------------------------------------------------------------------------
// COperatorLogoOTAConv::DoCancel()
// -----------------------------------------------------------------------------
//
void COperatorLogoOTAConv::DoCancel()
    {
    }

//  End of File
