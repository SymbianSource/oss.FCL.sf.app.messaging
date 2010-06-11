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
 * Description: mms creation mode checks
 *
 */

#ifndef MMS_CONFORMANCE_CHECK_OPERATION_H
#define MMS_CONFORMANCE_CHECK_OPERATION_H

#include <QObject>

#include <MsgMediaInfo.h>
#include <qstring.h>

#ifdef BUILD_MSGUI_UTILS_DLL
#define MSGUI_UTILS_DLL_EXPORT Q_DECL_EXPORT
#else
#define MSGUI_UTILS_DLL_EXPORT Q_DECL_IMPORT
#endif

class CMmsConformance;
class CMsgMediaResolver;
class CDRMHelper;
class HbAction;

enum MmsConformanceCheckErrors
{
    EInsertSuccess = 0, EInsertQueryAbort, EInsertNotSupported
};

/**
 * This class provides interfaces which validate the media for 
 * insertion into editor and also validates if the mms message
 * can be forwarded.
 * 
 */

class MSGUI_UTILS_DLL_EXPORT MmsConformanceCheck : public QObject
{
    Q_OBJECT
    
public:

    /**
     * Constructor
     */
    MmsConformanceCheck();

    /**
     * Destructor
     */
    ~MmsConformanceCheck();

    /**
     * Checks if the file passes conformance checks 
     * @param file, filepath
     * @param showNote, note is shown if enabled
     * @return EInsertSuccess for successfull cases
     */
    int checkModeForInsert(const QString& file,bool showNote = true);

private:

    /*
     * Launch notification dialog
     */
    void showPopup(const QString& text);
    
private slots:
    
	/**
     * This slot is called when insert media dialog is launched.
     * @param action selected action (yes or no).
     */
    void onDialogInsertMedia(HbAction* action);
private:

    /*
     * Mms creation mode
     */
    TInt iCreationMode;
    
    /**
     * Max MMS composition size
     */
    int iMaxMmsSize;    
    
    /**
     * Conformance status
     */
    TUint32 iConfStatus;

};
#endif //MMS_CONFORMANCE_CHECK_OPERATION_H
