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
* Description:  MsgBody  declaration
*
*/



#ifndef INC_MSGBODY_H
#define INC_MSGBODY_H

// ========== INCLUDE FILES ================================

#include "MsgFormComponent.h"              // for CMsgFormComponent

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CGulIcon;

// ========== CLASS DECLARATION ============================

/**
 * Defines body control container for message editors/viewers.
 *
 */
class CMsgBody : public CMsgFormComponent
{

public:

    /**
     * Factory method that creates this control.
     * @param aParent
     * @param aMargins
     * @return
     */
    static CMsgBody* NewL(const CCoeControl& aParent, const TMargins& aMargins);

    /**
     * Destructor.
     */
    ~CMsgBody();

public:  // from CMsgFormComponent

    /**
     * Calculates and sets the size for the body and returns new size as reference
     * aSize. If aInit == ETrue, sets also size for the controls by calling their
     * SetAndGetSizeL functions.
     *
     * @param aSize
     * @param aInit
     */
    void SetAndGetSizeL(TSize& aSize, TBool aInit);
    
    /**
     * Set body index.
     *
     * @param aIndex
     */
    void SetBodyIndex(TInt aIndex);

public:  // from CCoeControl

    /**
     * Returns a number of controls.
     * @return
     */
    TInt CountComponentControls() const;

    /**
     * Returns a control of index aIndex.
     * @param aIndex
     * @return
     */
    CCoeControl* ComponentControl(TInt aIndex) const;

protected:  // from CCoeControl

    /**
     * Sets new position for all the controls.
     */
    void SizeChanged();

    /**
     * Draw from CCoeControl.
     * @param aRect
     */
    void Draw(const TRect& aRect) const;
    
    /**
     * HandleResourceChange from CCoeControl.
     * @param aType
     */
    void HandleResourceChange(TInt aType);

private:

    /**
     * Constructor.
     * @param aMargins
     */
    CMsgBody(const TMargins& aMargins);

    /**
     * 2nd phase constructor.
     * @param aParent
     */
    void ConstructL(const CCoeControl& aParent);

private:

    /**
     * Constructor (not available).
     */
    CMsgBody();
    
    /**
     * SkinChanged
     */
    void SkinChanged();
    
private:
    TInt        iIndex;
    CGulIcon*   iBoundary;

};

#endif

// End of File
