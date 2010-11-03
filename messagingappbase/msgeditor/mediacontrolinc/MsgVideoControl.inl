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
*       MsgEditor video media control - a Message Editor Base control.
*
*/



// ---------------------------------------------------------
// CMsgVideoControl::Volume
// ---------------------------------------------------------
//
inline TInt CMsgVideoControl::Volume() const
    {
#ifdef SIMULATE_VIDEO
    return 0xf5;
#else
    return iVolume;
#endif
    }

// ---------------------------------------------------------
// CMsgVideoControl::MaxVolume
// ---------------------------------------------------------
//
inline TInt CMsgVideoControl::MaxVolume() const
    {
#ifdef SIMULATE_VIDEO
    return 0xff;
#else
    return iMaxVolume;
#endif
    }

// ---------------------------------------------------------
// CMsgVideoControl::ContinueOnFocus
// ---------------------------------------------------------
//
inline void CMsgVideoControl::ContinueOnFocus( TBool aContinue )
    {
    iContinueOnFocus = aContinue;
    }

// ---------------------------------------------------------
// CMsgVideoControl::HasVideoStream
// ---------------------------------------------------------
//
inline TBool CMsgVideoControl::HasVideoStream() const
    {
    return iHasVideo;
    }

//  End of File
