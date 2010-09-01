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
* Description:  MsgEditorKeyCatcher  declaration
*
*/



#ifndef MSGEDITORKEYCATCHER_H
#define MSGEDITORKEYCATCHER_H

// ========== INCLUDE FILES ================================

#include <coecntrl.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CMsgAddressControlEditor;

// ========== CLASS DECLARATION ============================

/**
 *  CMsgEditorKeyCatcher class
 *
 */
class CMsgEditorKeyCatcher : public CCoeControl
{
public: // Constructors and destructor

    /**
     * EPOC default constructor.
     * @param aRect Frame rectangle for container.
     */
    void ConstructL(CMsgAddressControlEditor* aEditor);

    /**
     * Default c++ constructor.
     */
    CMsgEditorKeyCatcher();

    /**
     * Destructor.
     */
    ~CMsgEditorKeyCatcher();

public: // New functions

public: // Functions from base classes

    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

private: // Constructors and destructor

private: // Functions from base classes

private:

    CMsgAddressControlEditor* iEditor; // not owned
};

#endif

// End of File
