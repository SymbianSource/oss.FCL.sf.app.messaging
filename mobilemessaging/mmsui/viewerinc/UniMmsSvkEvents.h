/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*  Class which implements handling Side Volume Key events (SVK).
*  This class implements methods of MRemConCoreApiTargetObserver
*
*/



#ifndef MMSSVKEVENTS_H
#define MMSSVKEVENTS_H

// INCLUDES
#include <remconcoreapitargetobserver.h>

// FORWARD DECLARATIONS

// CONSTANTS

// CLASS DECLARATION
class CRemConInterfaceSelector;
class CRemConCoreApiTarget;

class MMmsSvkEventsObserver
    {
public:
    /**
    * Called when volume level should be changed.
    * @param aVolumeChange: +1 change volume up
    *                       -1 change volume down
    */
    virtual void MmsSvkChangeVolumeL( TInt aVolumeChange ) = 0;
        
    };
    

class CMmsSvkEvents : public CBase,
						public MRemConCoreApiTargetObserver
    {
    public: // Constructors and destructor
        /**
        * Symbian constructor
        * @param aObserver observer to implement callback functions
        */

        static CMmsSvkEvents* NewL( MMmsSvkEventsObserver& aObserver );

        /**
        * Destructor.
        */
        virtual ~CMmsSvkEvents();


    protected: // New functions
        /**
        * Default constructor.
        */
        CMmsSvkEvents( MMmsSvkEventsObserver& aObserver );

        /**
        * EPOC constructor.
        */
        void ConstructL();
        	
    	// From MRemConCoreApiTargetObserver
    	void MrccatoCommand(TRemConCoreApiOperationId aOperationId, 
    	 					TRemConCoreApiButtonAction aButtonAct);
    	

    private:

        /**
        * Calls respective observer function
        */
        void DoChangeVolume( );
    
        /**
        * ChangeVolume is the callback function called from change volume timer.
        */
        static TInt ChangeVolume(TAny* aThis);

	private:
	    MMmsSvkEventsObserver&      iObserver;
	
		CRemConInterfaceSelector*   iInterfaceSelector;
		CRemConCoreApiTarget*       iCoreTarget;
		CPeriodic*                  iVolumeTimer;
        /// indicates size of volume level change
        TInt                        iChange;
    };


#endif

// End of File
