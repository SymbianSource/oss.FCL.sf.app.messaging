/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       Implementation of NcnNotification ECom API.
*
*/



#ifndef __RNCNNOTIFICATION_H__
#define __RNCNNOTIFICATION_H__

//  INCLUDES
#include <MNcnInternalNotification.h>


// CLASS DECLARATION

/**
 * This class implements MNcnInternalNotification API as well as
 * MNcnNotification API which is on the top of inheritance hierarchy.
 * This is a RSessionBase class but this is not revealed to the users
 * of the ECom component.
 */
class RNcnNotification
: public RSessionBase,
  public MNcnInternalNotification
    {
    public: // Constructor and destructor
    
       /**
        * Two-phased constructor. Not normally used for R-classes but
        * since this is an ECom component heap construction is feasible.
        * @return newly instantiated object
        */
        static RNcnNotification* NewL();
        
       /**
        * C++ constructor.
        */
        RNcnNotification();

       /**
        * Destructor.
        * Session is closed in destructor since this class is used
        * via M-class interface.
        */
        ~RNcnNotification();
        
    public: // New functions
        
       /**
        * Initializes C/S session.
        */
        void InitializeSessionL();

    public: // From MNcnNotification
        
        TInt NewMessages( const TMsvId& aMailBox,
                            MNcnNotification::TIndicationType aIndicationType,
                            const MDesCArray& aInfo );

        TInt MarkUnread( const TMsvId& aMailBox );
        
        
        
    public: // From MNcnInternalNotification

        TInt NewMessages( const TNcnNotifMessageType aType );

        TInt NewMessages( const TNcnNotifMessageType aType,
                          const TMsvId& aMailBox,
                          const MDesCArray& aInfo );

        TInt MarkUnread( const TNcnUnreadRequestType aRequest );

        TInt MarkUnread( const TNcnUnreadRequestType aRequest,
                         const TMsvId& aMailbox );        
                         
        void MarkUnread( const TNcnUnreadRequestType aRequest,
                         TRequestStatus& aRequestStatus );

        void MarkUnread( const TNcnUnreadRequestType aRequest,
                         const TMsvId& aMailbox,
                         TRequestStatus& aRequestStatus );
                         
        void CancelMarkUnread();
    };

#endif // __RNCNNOTIFICATION_H__

// End of File
