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
* Description:  TestAttachDlg implementation
*
*/


#include <eikmenup.h>                       // for CEikMenuPane

#include <avkon.hrh>
#include <avkon.rsg>
#include "MsgViewAttachmentsDialog.h"
#include "MsgAttachmentInfo.h"
#include "MsgAttachmentModel.h"
#include "TestAttachDlg.h"
#include "MsgEditorAppUi.hrh"               // for EMsgViewAttachmentCmdOpen
#include <MsgEditorAppUi.rsg>               // atta dlg base resouce identifiers

enum TTestAttachmentCmds
{
    ETestAttachmentFetch = EMsgViewAttachmentCmdLast,
    ETestAttachmentSendVia,
    ETestAttachmentInsert
};

CTestEditorViewAttachmentDialog::CTestEditorViewAttachmentDialog (
    TDesC&                  aTitle,
    TInt                    aMenuId,
    CMsgAttachmentModel&    aAttachmentModel)
    :
    CMsgViewAttachmentsDialog (aTitle, aMenuId, aAttachmentModel)
{
}

void CTestEditorViewAttachmentDialog::ProcessCommandL(TInt aCommandId)
{
    HideMenu();

    TInt index = CurrentListItemIndex();

    switch (aCommandId)
    {
        // this overrides Open command from parent class.

        // handle our own menu command.
    case ETestAttachmentFetch:
        if (index != -1)
        {
            CMsgAttachmentInfo& attInfo = iAttachmentModel.AttachmentInfoAt(index);

            // fetch attachment...

            //... and then set flag.
            attInfo.SetFetched (ETrue);

            SetOpenParamList( NULL );
            OpenParamList();
            HandleControlStateChangeL( 0 );
            
            // this updates the icon of current list item.
            UpdateIcon(index);
        }
        break;

    default:
        // call parent class to process other commands.
        CMsgViewAttachmentsDialog::ProcessCommandL(aCommandId);
        break;
    }

}

TKeyResponse CTestEditorViewAttachmentDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    TInt index = CurrentListItemIndex();

    if (aType == EEventKey && !MenuShowing())
    {
        switch (aKeyEvent.iCode)
        {
        case EKeyEnter:
        case EKeyOK:
            if (index != -1 && iAttachmentModel.AttachmentInfoAt(index).IsFetched())
            {
                ProcessCommandL(EMsgViewAttachmentCmdOpen);
            }
            return EKeyWasConsumed;
        }

        switch (aKeyEvent.iScanCode)
        {
        case EStdKeyBackspace:
            ProcessCommandL(EMsgViewAttachmentCmdRemove);
            return EKeyWasConsumed;

        case EStdKeyYes:
        case EStdKeyNo:
            User::InfoPrint(_L("Not implemented"));
            return EKeyWasConsumed;
        }
    }

    return CMsgViewAttachmentsDialog::OfferKeyEventL(aKeyEvent, aType);
}

void CTestEditorViewAttachmentDialog::DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane)
{
    CMsgViewAttachmentsDialog::DynInitMenuPaneL(aMenuId, aMenuPane);

    if (aMenuId == R_MEB_ATTACHMENTS_MENUPANE /*R_AVKON_FORM_MENUPANE*/)
    {
        TInt index = CurrentListItemIndex();

        if (index != -1)
        {
            CMsgAttachmentInfo& attInfo = iAttachmentModel.AttachmentInfoAt(index);

            if (!attInfo.IsFetched())
            {
                aMenuPane->DeleteMenuItem(EMsgViewAttachmentCmdOpen);
            }
        }

    }
}
