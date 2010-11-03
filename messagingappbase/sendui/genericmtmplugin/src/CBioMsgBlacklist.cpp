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
* Description:   Used to check if sending of bio message is supported.
*
*/



// INCLUDE FILES
#include <coemain.h>
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR
#include <barsread.h>
#include <sendnorm.rsg>
#include "CBioMsgBlacklist.h"


// CONSTANTS
const TInt KLoopStartIndex = 0;
const TInt KBioMsgArrayGranularity = 4;


// ============================ MEMBER FUNCTIONS ==============================


// ---------------------------------------------------------
// CBioMsgBlacklist::NewL
//
// ---------------------------------------------------------
//
CBioMsgBlacklist* CBioMsgBlacklist::NewL(CCoeEnv* aCoeEnv )
    {
    CBioMsgBlacklist* self = new (ELeave) CBioMsgBlacklist( aCoeEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CBioMsgBlacklist::~CBioMsgBlacklist
//
// ---------------------------------------------------------
//
CBioMsgBlacklist::~CBioMsgBlacklist( )
    {
    delete iBioMsgBlacklistArray;
    }


// ---------------------------------------------------------
// CBioMsgBlacklist::CBioMsgBlacklist
//
// ---------------------------------------------------------
//
CBioMsgBlacklist::CBioMsgBlacklist( CCoeEnv* aCoeEnv )
    :
    iCoeEnv( aCoeEnv )
    {
    }


// ---------------------------------------------------------
// CBioMsgBlacklist::ConstructL
//
// ---------------------------------------------------------
//
void CBioMsgBlacklist::ConstructL()
    {
    iBioMsgBlacklistArray = new ( ELeave ) CArrayFixFlat<TInt32>( KBioMsgArrayGranularity );

    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, R_BIOMSGBLACKLIST_ARRAY );
    const TInt count = reader.ReadInt16();
    for ( TInt loop = KLoopStartIndex; loop < count; loop++ )
        {
        iBioMsgBlacklistArray->AppendL( reader.ReadInt32() );
        }
    CleanupStack::PopAndDestroy( /*&reader*/ );
    }



// ---------------------------------------------------------
// CBioMsgBlacklist::CanSendBioMsg
//
// ---------------------------------------------------------
//
TBool CBioMsgBlacklist::CanSendBioMsg( TUid aBioMessageUid )
    {
    if ( aBioMessageUid.iUid != 0 )
        {
        const TInt count = iBioMsgBlacklistArray->Count();
        for ( TInt loop = KLoopStartIndex; loop < count; loop++ )
            {
            if ( (*iBioMsgBlacklistArray)[loop] == aBioMessageUid.iUid )
                {
                return EFalse;
                }
            }
        }
    return ETrue;
    }

// End of file
