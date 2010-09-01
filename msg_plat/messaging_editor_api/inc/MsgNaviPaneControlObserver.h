/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgNaviPaneControlObserver  declaration
*
*/



#ifndef MMSGNAVIPANECONTROLOBSERVER_H
#define MMSGNAVIPANECONTROLOBSERVER_H

// ========== INCLUDE FILES ================================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================
    
/**
* Navigation pane control observer
*
* @since 5.0
*
* @library CMsgEditorAppUi
*/
NONSHARABLE_CLASS( MMsgNaviPaneControlObserver )
    {
    public: // Enumerations
    
        enum TMsgNaviControlEventId
            {
            EMsgNaviLeftArrowPressed,
            EMsgNaviRightArrowPressed
            };
            
    public: // New functions
    
        /**
        * Call back function for reporting navigation control events to the observer
        *
        * @param aEvent - Event
        */
        virtual void HandleNavigationControlEventL( TMsgNaviControlEventId aEvent ) = 0;
    };
    
#endif // MMSGNAVIPANECONTROLOBSERVER_H

// End of File
