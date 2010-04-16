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
 * Description:
 *
 */

// INCLUDE FILES

// USER INCLUDES
#include "unieditorsmsplugin.h"
#include "unieditorsmsplugin_p.h"

// DEBUG
#include "debugtraces.h"

// CONSTANTS

//---------------------------------------------------------------
// UniEditorSmsPlugin::UniEditorSmsPlugin()
// @see header
//---------------------------------------------------------------
UniEditorSmsPlugin::UniEditorSmsPlugin(QObject* parent) :
    QObject(parent),
    d_ptr(NULL)
{
    TRAPD(error, d_ptr = UniEditorSmsPluginPrivate::NewL());
    QDEBUG_WRITE_FORMAT("UniEditorSmsPlugin::UniEditorSmsPlugin error = ",error);
}

//---------------------------------------------------------------
// UniEditorSmsPlugin::~UniEditorSmsPlugin()
// @see header
//---------------------------------------------------------------
UniEditorSmsPlugin::~UniEditorSmsPlugin()
{
    delete d_ptr;
}

//---------------------------------------------------------------
// UniEditorSmsPlugin::messageType()
// @see header
//---------------------------------------------------------------
ConvergedMessage::MessageType UniEditorSmsPlugin::messageType()
{
    return ConvergedMessage::Sms;
}

//---------------------------------------------------------------
// UniEditorSmsPlugin::convertFrom
// @see header
//---------------------------------------------------------------
ConvergedMessage* UniEditorSmsPlugin::convertFrom( TMsvId aId )
{
    ConvergedMessage* msg = NULL;
    TRAPD(error, msg = d_ptr->ConvertFromL( aId ));
    QDEBUG_WRITE_FORMAT("UniEditorSmsPlugin::convertFrom error = ",error);
    return msg;
}

//---------------------------------------------------------------
// UniEditorSmsPlugin::deleteDraftsEntry
// @see header
//---------------------------------------------------------------
void UniEditorSmsPlugin::deleteDraftsEntry( TMsvId aId )
{
    TRAPD(error, d_ptr->DeleteDraftsEntryL( aId ));
    QDEBUG_WRITE_FORMAT("UniEditorSmsPlugin::DeleteDraftsEntry error = ",error);
    return;
}

//---------------------------------------------------------------
// UniEditorSmsPlugin::convertTo
// @see header
//---------------------------------------------------------------
TMsvId UniEditorSmsPlugin::convertTo( ConvergedMessage *aMessage )
{
    TMsvId id = -1;
    TRAPD(error, id = d_ptr->ConvertToL( aMessage ));
    QDEBUG_WRITE_FORMAT("UniEditorSmsPlugin::ConvertTo error = ",error);
    return id;
}

//---------------------------------------------------------------
// UniEditorSmsPlugin::send
// @see header
//---------------------------------------------------------------
bool UniEditorSmsPlugin::send(TMsvId aId)
{
    bool ret = true;
    TRAPD(error, d_ptr->SendL( aId ));
    if(error != KErrNone)
    {
        ret = false;
    }
    QDEBUG_WRITE_FORMAT("UniEditorSmsPlugin::Send error = ",error);
    return ret;
}

//---------------------------------------------------------------
// UniEditorSmsPlugin::isServiceValidL
// @see header
//---------------------------------------------------------------
TBool UniEditorSmsPlugin::isServiceValid()
{
    TBool ret = EFalse;
    TRAPD(error, ret = d_ptr->IsServiceValidL());
    QDEBUG_WRITE_FORMAT("UniEditorSmsPlugin::isServiceValid error = ", error);
    return ret;
}

//---------------------------------------------------------------
// UniEditorSmsPlugin::validateServiceL
// @see header
//---------------------------------------------------------------
TBool UniEditorSmsPlugin::validateService( TBool aEmailOverSms/* = EFalse */)
{
    TBool ret = EFalse;
    TRAPD(error, ret = d_ptr->ValidateServiceL( aEmailOverSms ));
    QDEBUG_WRITE_FORMAT("UniEditorSmsPlugin::ValidateService error = ",error);
    return ret;
}

Q_EXPORT_PLUGIN2(unieditorsmsplugin, UniEditorSmsPlugin)
