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
* Description:  MsgSendKeyAcceptingQuery  declaration
*
*/



#ifndef INC_MSGSENDKEYACCEPTINGQUERY_H
#define INC_MSGSENDKEYACCEPTINGQUERY_H

#include <AknQueryDialog.h>

class CMsgSendKeyAcceptingQuery : public CAknQueryDialog
{
public:

    /**
     * Factory method that creates this object.
     * @param aPrompt
     * @param aTone
     * @return
     */
    static CMsgSendKeyAcceptingQuery* NewL(TDesC& aPrompt, const TTone& aTone = ENoTone);

    /**
     * Destructor.
     */
    ~CMsgSendKeyAcceptingQuery();

protected: // from CAknQueryDialog

    /**
     * @param aKeyEvent key event
     * @param aType     event type
     * @return response
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * @param aButtonId
     * @return
     */
    TBool OkToExitL(TInt aButtonId);

    /**
     * @param aButtonId
     * @return
     */
    TInt MappedCommandId(TInt aButtonId);

private:
    /**
     * Constructor.
     * @param aTone
     */
    CMsgSendKeyAcceptingQuery(const TTone aTone);

};

#endif

// End of File
