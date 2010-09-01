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
*  Static class which launches editor application (provided editor
*  conforms to \msgeditor architecture.
*
*/




#ifndef __MUIUMSGEDITORLAUNCHER_H__
#define __MUIUMSGEDITORLAUNCHER_H__


//  INCLUDES
#include <apparc.h>
#include <msvapi.h>
#include <msvids.h>
#include <mtclbase.h>   // TMsvPartList

// CONSTANTS
const TUid KUidMsvEditorParameterValue = {0x100014AA};


// FORWARD DECLARATIONS (These declarations are for class MsgEditorLauncher)
class CEikDocument;
class TEditorParameters;


// CLASS DECLARATION

/**
* TEditorParameters
*/
class TEditorParameters
    {
    public:
        inline TEditorParameters();
        inline TEditorParameters( const TEditorParameters& aParams );
        inline void ExternalizeL( RWriteStream& aStream ) const;
        inline void InternalizeL( RReadStream& aStream );
    public:
        TMsvId  iId;
        TInt    iFlags;
        TMsvPartList iPartList;
        TMsvId iDestinationFolderId;
        TUid	iSpecialAppId;
    };

/**
* TEditorFlags
*/
enum TEditorFlags
    {
    EMsgReadOnly = 0x00010,
    EMsgSwitch = 0x00020,          // Internal
    EMsgEditorChanged = 0x00040,       // Internal
    EMsgCreateNewMessage = 0x00080,
    EMsgLaunchEditorThenWait = 0x00100,
    EMsgLaunchEditorEmbedded = 0x00200,
    EMsgForwardMessage = 0x01000,
    EMsgReplyToMessageSender = 0x02000,
    EMsgReplyToMessageAll = 0x04000,
    EMsgCreateMessageToService = 0x08000,
    EMsgEditNewMessage = 0x10000,
    EMsgPreviewMessage = 0x20000,
    EMsgCreateDuplicate = 0x40000,
    EMsgUnreadMessage = 0x80000,
    EMsgFirstFreeEditorSpecificFlag = 0x100000,
    EMsgLaunchEditorStandAlone = 0x200000
    };

inline TEditorParameters::TEditorParameters()
:   iId( KMsvNullIndexEntryId ),
    iFlags( 0 ),
    iPartList( 0 ),
    iDestinationFolderId( KMsvNullIndexEntryId ),
    iSpecialAppId( TUid::Uid(0) )
    {}

inline TEditorParameters::TEditorParameters( const TEditorParameters& aParams )
:   iId( aParams.iId ),
    iFlags( aParams.iFlags ),
    iPartList( aParams.iPartList ),
    iDestinationFolderId( aParams.iDestinationFolderId ),
    iSpecialAppId( aParams.iSpecialAppId )
    {}

inline void TEditorParameters::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iId );
    aStream.WriteInt32L( iFlags );
    aStream.WriteInt32L( iPartList );
    aStream.WriteInt32L( iDestinationFolderId );
    }
inline void TEditorParameters::InternalizeL( RReadStream& aStream )
    {
    iId = aStream.ReadInt32L();
    iFlags = aStream.ReadInt32L();
    iPartList = aStream.ReadInt32L();
    iDestinationFolderId = aStream.ReadInt32L();
    }


// CLASS DECLARATION

/**
* MsgEditorLauncher
*
* Static class which launches editor application (provided editor
* conforms to \msgeditor architecture).
*
* The returned operation completes either when the launched
* application closes down, or after the launch has been initiated
* depending on the values in the TEditorParameters.
*/

class MsgEditorLauncher
    {
    public:

        /**
        * Launch message editor
        * @param aSession: reference to CMsvSession object
        * @param aMtmType: mtm type
        * @param aStatus: request status
        * @param aParams: editor launch parameters, should have always embedded flag on: aParams.iFlags&EMsgLaunchEditorEmbedded
        * @param aAppName: editor name
        * @param aAddtionamParams: additional parameters
        */
        IMPORT_C static CMsvOperation* LaunchEditorApplicationL(
            CMsvSession& aSession,
            TUid aMtmType,
            TRequestStatus& aStatus,
            const TEditorParameters& aParams,
            const TDesC& aAppName,
            const TDesC8* aAddtionalParams = NULL );

        /**
        * Launch message editor
        * @param aSession: reference to CMsvSession object
        * @param aMtmType: mtm type
        * @param aStatus: request status
        * @param aParams: editor launch parameters, should have always embedded flag on: aParams.iFlags&EMsgLaunchEditorEmbedded
        * @param aAppName: editor name
        * @param aProgressPack: progress buffer
        * @param aAddtionamParams: additional parameters
        */
        IMPORT_C static CMsvOperation* LaunchEditorApplicationL(
            CMsvSession& aSession,
            TUid aMtmType,
            TRequestStatus& aStatus,
            const TEditorParameters& aParams,
            const TDesC& aAppName,
            const TDesC8& aProgressPack,
            const TDesC8* aAddtionalParams = NULL );

    private:
        /**
        * Launch editor application embedded.
        * @param aSession: reference to CMsvSession object
        * @param aMtmType: mtm type
        * @param aStatus: request status
        * @param aParams: editor launch parameters.
        * @param aAppName: editor name
        * @param aProgressPack: progress buffer
        */
        static CMsvOperation* LaunchEditorApplicationEmbeddedL(
            CMsvSession& aSession,
            TUid aMtmType,
            TRequestStatus& aStatus,
            const TEditorParameters& aParams,
            const TDesC& aAppName,
            const TDesC8& aProgressPack );

        /**
        * Launch editor application stand alone.
        * @param aSession: reference to CMsvSession object
        * @param aMtmType: mtm type
        * @param aStatus: request status
        * @param aParams: editor launch parameters.
        * @param aAppName: editor name
        * @param aProgressPack: progress buffer
        * @param aAddtionamParams: additional parameters
        * @param aLaunchThenWait: Not in use yet
        */
        static CMsvOperation* LaunchEditorApplicationStandAloneL(
            CMsvSession& aSession,
            TUid aMtmType,
            TRequestStatus& aStatus,
            const TEditorParameters& aParams,
            const TDesC& aAppName,
            const TDesC8& aProgressPack,
            const TDesC8* aAddtionalParams,
            TBool aLaunchThenWait );

		/**
		* DiscoverL (Internal)
		* Finds the right application id to be started and returns it.
		* First checks if TEditorParameters.iSpecialAppId is set. If it is,
		* returns that. Then connects RApaLsSession and initializes
		* it with GetServerApps command. Then gets all the services that
		* implement the service by using GetServiceImplementationsLC. 
		* Calls TryMatchL for each service and if there's a match,
		* the appId is returned. If no implementation is found, 0 is returned.
		*/
    	static TUid DiscoverL( const TEditorParameters aParams );

		/**
		* TryMatchL (Internal)
		* If TEditorParameters.iFlags has either EMsgReadOnly
		* or EMsgPreviewMessage bit set, this function looks for a service
		* that implements the KMsgServiceView part of the service. Otherwise
		* looks for KMsgServiceEdit part.
		* Reads the parameters ( service type (View/Edit), MtmType) from 
		* the opaque data. If type and mtm match, sets aAppUi as
		* the application id and returns ETrue. If not found, returns EFalse.
		*/
    	static TBool TryMatchL(  const TEditorParameters aParams, const TApaAppServiceInfo aInfo, TUid& aAppUid  );

    };


/**
* MsvUiEditorUtilities
*
* Helper class to resolver application
*/
class MsvUiEditorUtilities
    {
    public:
        IMPORT_C static HBufC* ResolveAppFileNameL( TUid aAppUid );
    };


/**
* MsgEditorServiceResolver
*
* Helper class to resolver right service
*
* For internal use of MsgEditorLauncher
*/
class MsgEditorServiceResolver
	{
	public:
		/**
		* DiscoverL
		* Finds the right application id to be started and returns it.
		* First checks if TEditorParameters.iSpecialAppId is set. If it is,
		* returns that. Then connects RApaLsSession and initializes
		* it with GetServerApps command. Then gets all the services that
		* implement the service by using GetServiceImplementationsLC. 
		* Calls TryMatchL for each service and if there's a match,
		* the appId is returned. If no implementation is found, 0 is returned.
		*/
    	static TUid DiscoverL( const TEditorParameters aParams, TUid aMtmType );

		/**
		* TryMatchL (Internal)
		* If TEditorParameters.iFlags has either EMsgReadOnly
		* or EMsgPreviewMessage bit set, this function looks for a service
		* that implements the KMsgServiceView part of the service. Otherwise
		* looks for KMsgServiceEdit part.
		* Reads the parameters ( service type (View/Edit), MtmType) from 
		* the opaque data. If type and mtm match, sets aAppUi as
		* the application id and returns ETrue. If not found, returns EFalse.
		*/
    	static TBool TryMatchL(  const TEditorParameters aParams, TUid aMtmType, const TApaAppServiceInfo aInfo, TUid& aAppUid  );
	};



#endif // __MSGEDITORLAUNCHER_H__

// End of file
