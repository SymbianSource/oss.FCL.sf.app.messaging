/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  TestAttachDlg  declaration
*
*/


#ifndef TESTATTACHDLG_H_
#define TESTATTACHDLG_H_
#include "MsgViewAttachmentsDialog.h"

class CTestEditorViewAttachmentDialog : public CMsgViewAttachmentsDialog
{
public:
    CTestEditorViewAttachmentDialog(TDesC& aTitle, TInt aMenuId, CMsgAttachmentModel& aAttachmentModel);

    void ProcessCommandL(TInt aCommandId);

    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);
};
#endif /*TESTATTACHDLG_H_*/
