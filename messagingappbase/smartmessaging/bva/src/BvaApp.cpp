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
*     BVA application class.
*
*/



// INCLUDE FILES
#include    "BvaApp.h"
#include    "BvaDocument.h"
#include    "BvaUid.h"

// ================= MEMBER FUNCTIONS =======================

#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CBvaApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

TUid CBvaApp::AppDllUid() const
    {   
    return TUid::Uid(KUidBVA);
    }

CApaDocument* CBvaApp::CreateDocumentL()
    {
    return CBvaDocument::NewL( *this );
    }

void CBvaApp::GetDefaultDocumentFileName(TFileName& /*aDocumentName*/) const
    {
    // Override default implementation, so aDocumentName doesn't change. 
    }

// End of File
