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
*     Declares document for BVA application.
*
*/



// INCLUDE FILES
#include "BvaDocument.h"
#include "BvaAppUi.h"
#include "bvalog.h"

CBvaDocument::~CBvaDocument()
    {
    }

CBvaDocument* CBvaDocument::NewL(CEikApplication& aApp)
    {
    CBvaDocument* self = new (ELeave) CBvaDocument( aApp );
    return self;
    }

void CBvaDocument::OpenFileL(CFileStore*& /*aFileStore*/, RFile& aFile)
	{
	LOG("CBvaDocument::OpenFileL begin");
	TInt size = 0;
	TInt err = aFile.Size( size );

	if ( err == KErrNone && size != 0)
        {
		((CBvaAppUi*)iAppUi)->LoadBioControlL( aFile );
        }

    aFile.Close();
    LOG("CBvaDocument::OpenFileL end");
	}

CEikAppUi* CBvaDocument::CreateAppUiL()
    {
    return new (ELeave) CBvaAppUi;
    }

// End of File
