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
*       MsgEdito audio media control.
*
*/



// ---------------------------------------------------------
// CMsgAudioControl::Volume
// ---------------------------------------------------------
//
inline TInt CMsgAudioControl::Volume()
    {
    TInt volume( 0 );
    if ( iAudioPlayer )
        {
        iAudioPlayer->GetVolume( volume );
        }
    return volume;
    }

// ---------------------------------------------------------
// CMsgAudioControl::MaxVolume
// ---------------------------------------------------------
//
inline TInt CMsgAudioControl::MaxVolume()
    {
    TInt volume( 0 );
    if ( iAudioPlayer && iState>=EMsgAsyncControlStateReady )
        {
        volume = iAudioPlayer->MaxVolume( );
        }
    return volume;
    }

// ---------------------------------------------------------
// CMsgAudioControl::SetVolume
// ---------------------------------------------------------
//
inline void CMsgAudioControl::SetVolume( TInt aVolume )
    {
    if ( iAudioPlayer )
        {
        iAudioPlayer->SetVolume( aVolume );
        }
    }

//  End of File