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
 * Description:  This provides the messaging mw interface for MMS
 *
 */

#ifndef MMSSETTINS_PRIVATE_H
#define MMSSETTINS_PRIVATE_H

#include <smutset.h>
#include <mmsclient.h>

#include "msgsettingengine.h"

/**
 * MMS setting class implements the MMSC settings
 * The settings will be stored in the central repository.
 */

class MmsSettingsPrivate
{
public:

    /**
     * 2 phase constructor
     */
    static MmsSettingsPrivate* NewL();

    /*
     * Destructor
     */
    ~MmsSettingsPrivate();

    void setMMSRetrieval(MsgSettingEngine::MmsRetrieval aRetrieval);

    void setAnonymousMessages(TBool aAnonymous);

    void setReceiveMMSAdverts(TBool aReceiveAdvert);

    void advanceMmsSettings(MsgSettingEngine::MmsRetrieval& aRetrieval,
                            TBool& aAnonymousStatus, TBool& aMmsAdvertsStatus);

    void getAllAccessPoints(RPointerArray<HBufC>& aAccessPoints,
                            TInt& aDefaultIndex);

    void setMMSAccesspoint(TInt& aDefaultIndex);

private:

    /*
     * 1st phase Constructor 
     */
    MmsSettingsPrivate();

    /**
     * 2nd phase constructor
     */
    void ConstructL();

    /*
     * Creates the repository
     */
    void createRepositoryL();

private:
    //nothing private data
};

#endif // MMSSETTINS_PRIVATE_H

