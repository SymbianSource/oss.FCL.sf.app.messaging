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
* Description:  Mail central repository handler inline methods.
*
*/

#include <messagingvariant.hrh> // MUIU flags

// -----------------------------------------------------------------------------
// MailAdditionalHeaders()
// -----------------------------------------------------------------------------
//
inline TBool CMailCRHandler::MailAdditionalHeaders() const
    {
    return ( iMailUiLocalVariationFlags & EMailAdditionalHeaders );
    }

// -----------------------------------------------------------------------------
// MailAttachmentField()
// -----------------------------------------------------------------------------
//
inline TBool CMailCRHandler::MailAttachmentField() const
    {
    return  ( iMailUiLocalVariationFlags & EMailAttachmentFieldNew );
    }

// -----------------------------------------------------------------------------
// MailEncodingSetting()
// -----------------------------------------------------------------------------
//
inline TBool CMailCRHandler::MailEncodingSetting() const
    {
    return ( iMailUiLocalVariationFlags & EMailEncodingSetting );
    }

// -----------------------------------------------------------------------------
// MailCopyMoveSupport()
// -----------------------------------------------------------------------------
//
inline TBool CMailCRHandler::MailCopyMoveSupport() const
    {
    return ( iMuiuVariationFlags & KEmailFeatureIdCopyMoveSupported );
    }

// -----------------------------------------------------------------------------
// MailInsertToolBar()
// -----------------------------------------------------------------------------
//
inline TBool CMailCRHandler::MailInsertToolBar() const
    {
    return ( iMailUiLocalVariationFlags & EMailInsertToolBar );
    }
    


