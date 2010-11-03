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
* Description:  
*     Bio control factory.
*
*/



#ifndef MSGBIOCONTROLFACTORY_H
#define MSGBIOCONTROLFACTORY_H

// INCLUDES 
#include <e32base.h> // for CBase
#include <msvstd.h>  // for TMsvId
#include <mmsgbiocontrol.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// FORWARD DECLARATIONS

class CMsgBioControl;
class MMsgBioControlObserver;
class CBIODatabase;
class RFs;
class CMsvSession;

// CLASS DECLARATION

/**
 * Bio control factory. Gets needed information from bif files.
 * Remember to keep the bio factory alive, and destroy it after destroying
 * the Bio Control. Do not call the bio control creation methods more than
 * once for a single instance of the factory.
 */
class CMsgBioControlFactory : public CBase
    {
    public:

        /**
         * Two-phased constructor.
         */
        IMPORT_C static CMsgBioControlFactory* NewL();

        /**
         * Destructor
         */
        IMPORT_C ~CMsgBioControlFactory();

        /**
         * Creates a Bio control. This version reads the message from the
         * server. Do not call the bio control creation methods more than once
         * for a single instance of the factory.
         * @param aBioControlObserver Reference to the Bio control observer.
         * @param aBioMsgType The type of Bio message.
         * @param aEditorOrViewerMode Specifies whether the new Bio control
         *        will be in Editor or Viewer mode.
         * @param aId Id of the message.
         * @param aSession Handle to Message Server session. The ownership is
         * not transferred.
         * @return The newly created Bio control.
         * @exception EMEUErrArgument if aId or aSession is null or
         * aEditorOrViewerMode has invalid value.
         */
        IMPORT_C CMsgBioControl* CreateControlL(
            MMsgBioControlObserver& aBioControlObserver,
            const TUid& aBioMsgType,
            TMsgBioMode aEditorOrViewerMode,
            TMsvId aId,
            CMsvSession* aSession); ///no transfer of ownership

         /**
         * Creates a Bio Control. This version creates a file based Bio Control.
         * Do not call the bio control creation methods more than once for a
         * single instance of the factory.
         * @param aBioControlObserver Reference to the Bio control observer.
         * @param aBioMsgType The type of Bio message.
         * @param aEditorOrViewerMode Specifies whether the new Bio control
         *        will be in Editor or Viewer mode.
         * @param aFile The file handle
         * @return The newly created Bio Control.
         * @exception EMEUErrArgument if aEditorOrViewerMode has undefined
         * value
         */      
        IMPORT_C CMsgBioControl* CreateControlL(
            MMsgBioControlObserver& aBioControlObserver,
            const TUid& aBioMsgType,
            TMsgBioMode aEditorOrViewerMode,
            const RFile& aFile);           

    private:
        /**
         * Default constructor hidden away.
         */
        CMsgBioControlFactory();

        /**
         * Second phase constructor
         */
        void ConstructL();

        /**
         * This does the actual creating of the control.
         * @param aBioControlObserver Reference to the Bio control observer.
         * @param aBioMsgType The type of Bio message.
         * @param aEditorOrViewerMode Specifies whether the new Bio control
         *        will be in Editor or Viewer mode.
         * @param aId Id of the message.
         * @param aSession Handle to Message Server session.
         * @param aFile input file handle.
         * @return The newly created Bio control.
         */
        CMsgBioControl* DoCreateControlL(
            MMsgBioControlObserver& aBioControlObserver,
            const TUid& aBioMsgType,
            TMsgBioMode aEditorOrViewerMode,
            TMsvId aId,
            CMsvSession* aSession, //no transfer of ownership
            const RFile* aFile );

    private: // prohibited

        /**
         * Copy constructor hidden away.
         */
        CMsgBioControlFactory(const CMsgBioControlFactory& aSource);

        /**
         * Assignment operator hidden away.
         */
        const CMsgBioControlFactory& operator=(
            const CMsgBioControlFactory& aSource);

    private:

        /**
         * The library where the Bio control is located.
         */ 
        RLibrary    iControlDllLibrary;

        /**
         * File Server Session
         * It is opened during construction of factory, and closed during
         * destruction of factory.
         */
        RFs         iFs;


        /**
         * A filename for file based bio controls.
         */        
        TFileName   iFileName;

        };

#endif // MSGBIOCONTROLFACTORY_H

// End of file
