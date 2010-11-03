/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       Message Editor Base UI control.
*
*/



// ---------------------------------------------------------
// CMsgMediaControl::SetState
// ---------------------------------------------------------
//
inline void CMsgMediaControl::SetState( TMsgAsyncControlState  aState,
                                        TBool aDoNotCallObserver )
    {
    TMsgAsyncControlState oldState = iState;
    iState = aState;
    
    if ( !aDoNotCallObserver )
        {
        for ( TInt current = 0; current < iObservers->Count(); current++ )
            {
            iObservers->At( current )->MsgAsyncControlStateChanged( *this, iState, oldState );
            }
        }   
    }

// ---------------------------------------------------------
// CMsgMediaControl::SetUniqueContentIdL
// ---------------------------------------------------------
//
inline void CMsgMediaControl::SetUniqueContentIdL( const TDesC& aUniqueId )
    {
    iUniqueContentId = aUniqueId;
    }

// ---------------------------------------------------------
// CMsgMediaControl::UniqueContentId
// ---------------------------------------------------------
//        
inline const TDesC& CMsgMediaControl::UniqueContentId() const
    {
    return iUniqueContentId;
    }

//  End of File