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
* Description: Imap4MtmUi.inl
*
*
*/


inline CImap4ClientMtm* CImap4MtmUi::Imap4BaseMtm() const
    {
    return (CImap4ClientMtm*)&BaseMtm();
    }

inline TBool CImap4MtmUi::ServiceIsIdle(TInt aFlags) const
    {
    return aFlags&EIsIdle;
    }

inline TBool CImap4MtmUi::ServiceIsDisconnected(TInt aFlags) const
    {
    return aFlags&EIsDisconnected;
    }

inline TBool CImap4MtmUi::ServiceIsConnecting(TInt aFlags) const
    {
    return aFlags&EIsConnecting;
    }

inline TBool CImap4MtmUi::ServiceIsDisconnecting(TInt aFlags) const
    {
    return aFlags&EIsDisconnecting;
    }

inline TBool CImap4MtmUi::ServiceIsLoaded(TInt aFlags) const
    {
    return aFlags&EIsLoaded;
    }
