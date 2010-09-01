/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*           Document class
*
*/



#ifndef     NOTVIEWERDOCUMENT_H
#define     NOTVIEWERDOCUMENT_H

// INCLUDES
#include <MsgEditorDocument.h>          // CMsgEditorDocument
#include <mmsnotificationclient.h>
#include "NotMtmUi.h"
#include "NotMtmUiData.h"

// CLASS DECLARATION

/**
*  CNotViewerDocument class
*  Needed by the EIKON framework
*/
class CNotViewerDocument : public CMsgEditorDocument
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        * @param aApp A CEikApplication reference
        */
        CNotViewerDocument( CEikApplication& aApp );

        /**
        * Two-phased constructor.
        * @param aApp A CEikApplication reference
        */
        static CNotViewerDocument* NewL( CEikApplication& aApp );  
        
        /**
        * Destructor.
        */
        virtual ~CNotViewerDocument();

    public: // New stuff

        /*
        * Mutator
        * Set the header modified flag.
        * @param aModified
        */
        inline void SetHeaderModified( TBool aModified );

        /*
        * Accessor
        * Get the header modified flag.
        *
        * @return iHeaderModified flag
        */
        inline TBool HeaderModified() const;

    public: // From CMsgEditorDocument

        /**
         *
         * @return
         */
        TMsvId DefaultMsgFolder() const;

        /**
         *
         * @return
         */
        TMsvId DefaultMsgService() const;

        /**
         *
         * @param aService
         * @param aFolder
         * @return
         */
        TMsvId CreateNewL(TMsvId aService, TMsvId aFolder);

        /**
         * Called when new entry is set.
         */
        void EntryChangedL();

        /**
         * Returns the current Mtm.
         * Overrides the function in Base Class
         * @return a reference to Notification Client MTM
         */
        CMmsNotificationClientMtm& Mtm() const;

        /**
         * Returns the current MtmUi.
         * Overrides the function in Base Class
         * @return a reference to Notification MTM UI
         */
        CNotMtmUi& MtmUi() const;

        /**
         * Returns the current MtmUi Data.
         * Overrides the function in Base Class
         * @return a reference to Notification MTM UI Data
         */
        CNotMtmUiData& MtmUiDataL() const;

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Creates a new CNotViewerAppUi
        * @return a pointer to CNotViewerAppUi
        */
        CEikAppUi* CreateAppUiL();
        
        /**
        * To prevent default-constructor...
        */
        CNotViewerDocument();

    private:

        TBool       iHeaderModified;
    };

#include "NotViewerDocument.inl"

#endif      //  NOTVIEWERDOCUMENT_H
            
// End of File
