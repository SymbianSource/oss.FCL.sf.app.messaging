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
*     Inline methods for CMsgEditorModel.
*
*/



//  INLINE METHODS

// ---------------------------------------------------------
// CMsgEditorModel::Session
//
// Return reference to current session.
// ---------------------------------------------------------
//
inline CMsvSession& CMsgEditorModel::Session() const
{
    return *iSession;
}

// ---------------------------------------------------------
// CMsgEditorModel::Mtm
//
// Return reference to mtm.
// ---------------------------------------------------------
//
inline CBaseMtm& CMsgEditorModel::Mtm() const
{
    return *iMtm;
}


// ---------------------------------------------------------
// CMsgEditorModel::Entry
//
// Return current message's entry.
// ---------------------------------------------------------
//
inline const TMsvEntry& CMsgEditorModel::Entry() const
{
    return (iMtm->Entry()).Entry();
}

// ---------------------------------------------------------
// CMsgEditorModel::SetModelObserver
//
// Set the editor model observer.
// ---------------------------------------------------------
//
inline void CMsgEditorModel::SetModelObserver(MMsgEditorModelObserver* aObserver)
{
    iModelObserver = aObserver;
}

// ---------------------------------------------------------
// CMsgEditorModel::MediaAvailable
//
// Returns the current (the most recently known) status of the media.
// ---------------------------------------------------------
//
inline TBool CMsgEditorModel::MediaAvailable() const
{
    return iMediaAvailable;
}

// ---------------------------------------------------------
// CMsgEditorModel::SetMediaAvailable
//
// Allows changing the media availability flag stored in model.
// ---------------------------------------------------------
//
inline void CMsgEditorModel::SetMediaAvailable(TBool aMediaAvailable)
{
    iMediaAvailable = aMediaAvailable;
}

// End of File
