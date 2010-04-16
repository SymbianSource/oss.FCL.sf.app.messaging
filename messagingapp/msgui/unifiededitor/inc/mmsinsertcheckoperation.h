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

#ifndef MMS_INSERT_CHECK_OPERATION_H
#define MMS_INSERT_CHECK_OPERATION_H

#include <msgmediainfo.h>
#include <qstring.h>

class CMmsConformance;
class CMsgMediaResolver;
class CDRMHelper;

enum InsertCheckOperationErrors
{
    EInsertSuccess = 0, EInsertQueryAbort, EInsertNotSupported
};

class MmsInsertCheckOperation: public MMediaInfoObserver
{
public:

    /**
     * Constructor
     */
    MmsInsertCheckOperation();

    /**
     * Destructor
     */
    ~MmsInsertCheckOperation();

    /**
     * media insert checks. 
     * @param file, filepath
     */
    int checkModeForInsert(const QString& file);

    /**
     * From MMediaInfoObserver
     */
    void MediaInfoParsed();

private:

    /*
     * Launch query dialog
     */
    bool launchEditorQuery();

private:

    /*
     * Mms conformance.
     * Own
     */
    CMmsConformance* iMmsConformance;
    
    /*
     * Media resolver.
     * Own
     */
    CMsgMediaResolver* iMediaResolver;
    
    /*
     * DRM helper.
     * Own
     */
    CDRMHelper* iDRMHelper;

    /*
     * Mms creation mode
     */
    TInt iCreationMode;

};
#endif //MMS_INSERT_CHECK_OPERATION_H
