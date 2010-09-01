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
* Description: SmilPlayerSynchXmlParser implementation
*
*/



// INCLUDE FILES
#include "SmilPlayerSynchXmlParser.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSmilPlayerSynchXmlParser::CSmilPlayerSynchXmlParser
// Default C++ constructor.
// ----------------------------------------------------------------------------
// 
CSmilPlayerSynchXmlParser::CSmilPlayerSynchXmlParser()
    {
    }

// ----------------------------------------------------------------------------
// CSmilPlayerSynchXmlParser::~CSmilPlayerSynchXmlParser
// Destructor.
// ----------------------------------------------------------------------------
// 
CSmilPlayerSynchXmlParser::~CSmilPlayerSynchXmlParser()
    {
    }


// ----------------------------------------------------------------------------
// CSmilPlayerSynchXmlParser::ParseL
// Performs parsing. 
// ----------------------------------------------------------------------------
// 
CMDXMLDocument* CSmilPlayerSynchXmlParser::ParseL( RFile& aFileHandle )
    {
    CMDXMLParser* xmlParser = CMDXMLParser::NewL( this ); 
    CleanupStack::PushL( xmlParser );

    RFile parseHandle;
    User::LeaveIfError( parseHandle.Duplicate( aFileHandle ) );
    User::LeaveIfError( xmlParser->ParseFile( parseHandle ) ); // XML parser closes the given handle.
    
    iWaitScheduler.Start();
    
    CMDXMLDocument* dom = xmlParser->DetachXMLDoc();
    CleanupStack::PopAndDestroy( xmlParser );
    return dom;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerSynchXmlParser::ParseFileCompleteL
// Called when parsing has been completed. 
// ----------------------------------------------------------------------------
//            
void CSmilPlayerSynchXmlParser::ParseFileCompleteL()
    {
    if ( iWaitScheduler.IsStarted() )
        {
        iWaitScheduler.AsyncStop(); 
        }
    }
    
//  End of File  
 
