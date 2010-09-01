/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Mesasge view
*
*/


#include    <MsgMailUIDs.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMailMessageView::CMailMessageView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline CMailMessageView::CMailMessageView()
    {
    }

// Destructor
inline CMailMessageView::~CMailMessageView()
    {
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

// -----------------------------------------------------------------------------
// CMailMessageView::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
inline CMailMessageView* CMailMessageView::NewL( const TUid& aImplementationUid )
    {
    // Get the instantiation
    CMailMessageView* view =
        reinterpret_cast<CMailMessageView*> (
            REComSession::CreateImplementationL(aImplementationUid,
            _FOFF(CMailMessageView, iDtor_ID_Key)));

    return view;
    }

// -----------------------------------------------------------------------------
// CMailMessageView::ListL
// -----------------------------------------------------------------------------
//
inline void CMailMessageView::ListL( RImplInfoPtrArray& aImplInfoArray )
    {
    REComSession::ListImplementationsL( TUid::Uid( KUidMsgMailECOMInterface ),
        aImplInfoArray );
    }

// -----------------------------------------------------------------------------
// CMailMessageView::ViewMessageL
// -----------------------------------------------------------------------------
//
inline void CMailMessageView::ViewMessageL(
	RFileReadStream& /*aReadStream*/,
	MMailAppUiInterface& /*aUICallBack*/)
	{}

// -----------------------------------------------------------------------------
// CMailMessageView::ResourceFile
// -----------------------------------------------------------------------------
//
inline const TDesC& CMailMessageView::ResourceFile()
	{
	return KNullDesC();
	}

// End of File
