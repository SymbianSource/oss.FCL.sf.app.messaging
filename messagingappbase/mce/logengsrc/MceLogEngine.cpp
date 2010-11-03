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
*     This class communicates with the log db. This also handles all the
*     filtering options and registers Mce to LogEng to get notidied about
*     changes in log DB.
*
*/



// INCLUDE FILES
#include <e32base.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "logeng.h"         // for Event Type enumerations
#else
#include "logeng.h"         // for event type enumerations
#include "logsmspdudata.h"         // for event type enumerations
#endif
#include "MceLogEngine.h"
#include "MceLogEngineReader.h"
#include "MceLogPbkMatcher.h"
#include "MMceLogEngObserver.h"

// LOCAL CONSTANTS AND MACROS
_LIT( KPrivateNumber, "Private");

//  LOCAL FUNCTIONS


// ================= MEMBER FUNCTIONS =======================

CMceLogEngine::CMceLogEngine() :
    iForeground( ETrue ), // at foreground
    iDbDirty( EFalse )    // db is "clean"
    {
    }

EXPORT_C CMceLogEngine* CMceLogEngine::NewL()
    {
    CMceLogEngine* self = new(ELeave) CMceLogEngine();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CMceLogEngine::~CMceLogEngine()
    {
    if (iEventArray)
        {
        iEventArray->ResetAndDestroy();
        delete iEventArray;
        }
    delete iEngineReader;
    delete iLogClient;
    delete iPbkMatcher;
    iFsSession.Close();
    }


void CMceLogEngine::ConstructL()
    {
    User::LeaveIfError(iFsSession.Connect());
    iEventArray = new (ELeave) CArrayPtrFlat<CLogEvent>(1);
    iLogClient = CLogClient::NewL(iFsSession);
    }

EXPORT_C const CLogEvent* CMceLogEngine::Event( TInt aIndex )
    {
    return iEventArray->At(aIndex);
    }

EXPORT_C TInt CMceLogEngine::Count() const
    {
    return iEventArray->Count();
    }

void CMceLogEngine::ReadEventsCompletedL( TBool aStatus )
    {
    if ( ! aStatus )
       {
       ResetArray();
       }

    if( iContainerObserver )
        {
        iContainerObserver->MceLogEngineListBoxStateHasChangedL();
        }
    }

void CMceLogEngine::ResetArray()
    {
    if( iEventArray )
        {
        iEventArray->ResetAndDestroy();
        }
    }

void CMceLogEngine::UpdateEventsL()
    {
    if (iEngineReader)
        {
        iEngineReader->Cancel();
        }
    else
        {
        iEngineReader = CMceLogEngineReader::NewL(this);
        }

    iEngineReader->StartReadingL();
    }

void CMceLogEngine::AddEventToListL( CLogEvent* aEvent )
    {
    iEventArray->AppendL( aEvent );
    }


EXPORT_C void CMceLogEngine::SetContainerObserver( MMceLogEngObserver* aContainerObserver )
    {
    iContainerObserver = aContainerObserver;
    }


EXPORT_C void CMceLogEngine::RemoveContainerObserver()
    {
    iContainerObserver = NULL;
    }


EXPORT_C TInt CMceLogEngine::PurgeData( const CLogEvent* aEvent, CArrayFixFlat<TInt>* aData )
    {
    TPckgBuf<TLogSmsPduData> packedData;
    TInt sMsTotal( 0 );
    TInt sMsPending( 0 );

    if( aEvent->EventType() == ( KLogsEngMmsEventTypeUid ))
        {
        return sMsTotal;
        }

    packedData.Copy( aEvent->Data().Ptr(), sizeof( TLogSmsPduData ) );
    if( aEvent->Data().Length() > 0 )
        {
        sMsTotal = packedData().iTotal;
        sMsPending = sMsTotal
                      - packedData().iDelivered
                      - packedData().iFailed;
        aData->At( 0 ) = sMsPending;
        aData->At( 1 ) = sMsTotal;
        }

    return sMsTotal;
    }

EXPORT_C void CMceLogEngine::GetStandardStrings( TDes& aFailed, TDes& aPending )
    {
    iLogClient->GetString( aPending, R_LOG_DEL_PENDING );
    iLogClient->GetString( aFailed, R_LOG_DEL_FAILED );
    }

EXPORT_C TBool CMceLogEngine::IsNumberAvailable( TInt aIndex )
    {
    if( Count() == 0 )
        {
        return EFalse;
        }

    if( Event( aIndex )->Number().Size() == 0 )
        {
        return EFalse;
        }

    if( Event( aIndex )->RemoteParty() == KPrivateNumber )
        {
        return EFalse;
        }

    return ETrue;
    }


EXPORT_C void CMceLogEngine::ForegroundGainedL()
    {
    UpdateEventsL();
    if( !iPbkMatcher )
        {
        iPbkMatcher = CMceLogPbkMatcher::NewL( iFsSession, this );
        }
    iPbkMatcher->StartL();
    }


EXPORT_C void CMceLogEngine::ForegroundLost()
    {
    }

void CMceLogEngine::PbkMatchingDoneL()
    {
    }

EXPORT_C void CMceLogEngine::ClearDrListL()
    {
    ResetArray();

    if( !iEngineReader )
        {
        iEngineReader = CMceLogEngineReader::NewL(this);
        }
    iEngineReader->ClearDrListL();

    if( iContainerObserver )
        {
        iContainerObserver->MceLogEngineListBoxStateHasChangedL();
        }
    }

// End of file
