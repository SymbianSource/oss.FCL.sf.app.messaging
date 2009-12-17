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
*		Base class for bio controls
*
*/



// INCLUDE FILES

#include <msvapi.h>                 // MTM server API
#include <AknQueryDialog.h>
#include <eikmenup.h>
#include <bautils.h>                // for BaflUtils
#include <MsgEditorUtils.rsg>       // resouce identifiers
#include <StringLoader.h>           // StringLoader
#include <msgbiocontrolObserver.h>
#include "msgbiocontrol.h"          // for CMsgBioControl
#include "MMsgBioControlExtension.h"    // MMsgBioControlExtension
#include "MsgEditorUtils.pan"       // for MsgEditorUtils panics
#include <data_caging_path_literals.hrh>

// CONSTANTS
_LIT(KMsgEditorUtilsResource, "msgeditorutils.rsc");
enum TStatusFlags
		{
		EFileBased = KBit0
		};

// ================= MEMBER FUNCTIONS =======================

EXPORT_C CMsgBioControl::CMsgBioControl(MMsgBioControlObserver& aObserver,
                                        CMsvSession* aSession, //no ownership
                                        TMsvId aId,
                                        TMsgBioMode aEditorOrViewerMode,
                                        const RFile* aFile):
                            iBioControlObserver(aObserver),
                            iId(aId),
                            iMsvSession(aSession),
                            iMode(aEditorOrViewerMode),
                            iFile(aFile),
                            iBCStatusFlags(0)
    {
    // If aFile presents real file, subsession handle > 0
    // If aFile is dummy file handle ==> subsession handle == 0
    if (aFile)
    	{
	    if( aFile->SubSessionHandle() == 0)
	        {
	        iFile = NULL;
	        }
	    else
	        {
	        iFile = aFile;
	        iBCStatusFlags |= EFileBased;
	        }
    	}
    __ASSERT_ALWAYS(
        ( ( (aEditorOrViewerMode==EMsgBioEditorMode) ||
            (aEditorOrViewerMode==EMsgBioViewerMode) )
          &&
          ( (aSession && aId) ^ (IsFileBased()) )
          ),
          Panic( EMEUErrArgument ) );
    }

EXPORT_C CMsgBioControl::~CMsgBioControl()
    {
    if (iResourceOffsets)
        {
        for (TInt n(0); n < iResourceOffsets->Count(); n++)
            {
            iCoeEnv->DeleteResourceFile(iResourceOffsets->At(n));
            }
        delete iResourceOffsets;
        }
    }

EXPORT_C TBool CMsgBioControl::ConfirmationQueryL(const TDesC& aText)
    {
    //This temporary descriptor is needed because the query wants a non const
    HBufC* text = aText.AllocL();
    CleanupStack::PushL(text);
    CAknQueryDialog* qDlg = new (ELeave) CAknQueryDialog(*text);
    TInt response =
        qDlg->ExecuteLD(R_MSGEDITORUTILS_GENERAL_CONFIRMATION_QUERY);
    CleanupStack::PopAndDestroy(text); // text
    if (response)
        {
        return ETrue;
        }
    return EFalse;
    }

EXPORT_C TBool CMsgBioControl::ConfirmationQueryL(TInt aStringResource)
    {
    HBufC* buf = StringLoader::LoadLC(aStringResource);
    TBool result(ConfirmationQueryL(*buf));
    CleanupStack::PopAndDestroy(buf); //buf
    return result;
    }

EXPORT_C void CMsgBioControl::LoadResourceL(const TDesC& aFile)
    {

    TDriveList driveList;

    TFileName searchPath(NULL);

    // get a list of available drives
    TInt ret = iCoeEnv->FsSession().DriveList(driveList);
    if (ret != KErrNone)
		{
        // cannot continue
        Panic( EMEUResourceFileNotFound );
		}

    // scan all drives
    for ( TInt n=0 ; n < KMaxDrives ; n++ )
        {
        // does the drive exis
        if (driveList[n] != 0)
            {
            TDriveInfo driveInfo;

            ret = iCoeEnv->FsSession().Drive(driveInfo,n);
            if ((ret == KErrNone)
                && (driveInfo.iType != EMediaNotPresent)
                && (driveInfo.iType != EMediaRemote))
                {
                // drive exists..
                TDriveUnit drive(n);

                // append the data.
                searchPath.CopyF(drive.Name());
                searchPath.Append(KDC_RESOURCE_FILES_DIR());
                searchPath.Append(aFile);

                // check does the non-localized file exist
                TBool res = BaflUtils::FileExists( iCoeEnv->FsSession(), searchPath );
                if (res) //found
                    {
                    // Complete searchpath without filename.
                    searchPath.CopyF( drive.Name() );
                    searchPath.Append( KDC_RESOURCE_FILES_DIR() );
                    // no need to scan other drives:
                    break;
                    }
                else
                    {
                    HBufC* originalPath = searchPath.AllocLC();
                    // check is there localized version of resource file
                    BaflUtils::NearestLanguageFile(iCoeEnv->FsSession(), searchPath );

                    // if the original path differs from search path, the
                    // localized resource file is found
                    res = originalPath->Compare(searchPath);

                    CleanupStack::PopAndDestroy(originalPath);
                    if ( res != KErrNone ) // found
                        {
                        // Complete searchpath without filename.
                        searchPath.CopyF(drive.Name());
                        searchPath.Append( KDC_RESOURCE_FILES_DIR() );
                        // no need to scan other drives:
                        break;
                        }
                    }
                }
            }
        }

    // load the resource file from scanned path.
    LoadResourceL(aFile, searchPath);
    }

EXPORT_C void CMsgBioControl::LoadResourceL(const TDesC& aFile, const TDesC& aSearchPath)
    {
    if (!iResourceOffsets)
        {
        iResourceOffsets = new (ELeave) CArrayFixFlat<TInt>(1); // granularity
        }

    // Find the resource file for the nearest language
    TFileName fileName(aSearchPath);
    fileName.Append(aFile);
    BaflUtils::NearestLanguageFile(iCoeEnv->FsSession(), fileName);

    // Check does the resource file exist in given path.
    TBool fileExists( BaflUtils::FileExists(iCoeEnv->FsSession(), fileName) );
    if (fileExists)
        {
        // Add the resource file to the CONE environment, and keep the offset
        iResourceOffsets->SetReserveL(iResourceOffsets->Count() + 1);
        const TInt offset = iCoeEnv->AddResourceFileL(fileName);
        iResourceOffsets->AppendL(offset);
        }
    else
        {
        TParse* fp = new(ELeave) TParse();
	    fp->Set(aFile, &KDC_RESOURCE_FILES_DIR, NULL);

  	    TFileName fileName(fp->FullName());
	    delete fp;

        iResourceOffsets->SetReserveL(iResourceOffsets->Count() + 1);
        const TInt offset = iCoeEnv->AddResourceFileL(fileName);
        User::LeaveIfError( offset);
        iResourceOffsets->AppendL(offset);
		}
    }

EXPORT_C void CMsgBioControl::LoadStandardBioResourceL()
    {
    LoadResourceL(KMsgEditorUtilsResource);
    }

EXPORT_C TBool CMsgBioControl::IsEditor() const
    {
    return (iMode == EMsgBioEditorMode);
    }

EXPORT_C TBool CMsgBioControl::IsFileBased() const
    {
    return (iBCStatusFlags & EFileBased);
	}

EXPORT_C CMsvSession& CMsgBioControl::MsvSession() const
    {
    __ASSERT_ALWAYS(iMsvSession, Panic(EMEUMsvSessionIsNull));
    return *iMsvSession;
    }

EXPORT_C const TFileName& CMsgBioControl::FileName() const
    {
    // deprecated
    __ASSERT_ALWAYS(/*IsFileBased()*/ 0, Panic(EMEUNotFileBase));
    return TFileName(); // should never ever come to this point
    }

EXPORT_C const RFile& CMsgBioControl::FileHandle() const
    {
    __ASSERT_ALWAYS(IsFileBased(), Panic(EMEUNotFileBase));
    return *iFile;
    }

EXPORT_C TUint32 CMsgBioControl::OptionMenuPermissionsL() const
    {
    // This is the default because most of the Bio Controls need these.
    return EMsgBioCallBack
        | EMsgBioDelete
        | EMsgBioMessInfo
        | EMsgBioMove
        | EMsgBioCreateCC
        | EMsgBioSend
        | EMsgBioAddRecipient
        | EMsgBioSave
        | EMsgBioSendingOpt
        | EMsgBioHelp
        | EMsgBioExit;
    }


// default implementation returns height of the control.
EXPORT_C TInt CMsgBioControl::VirtualHeight()
{
    return iSize.iHeight;
}

// default implementation returns 0.
EXPORT_C TInt CMsgBioControl::VirtualVisibleTop()
{
    return 0;
}

// default implementation returns EFalse.
EXPORT_C TBool CMsgBioControl::IsCursorLocation(TMsgCursorLocation /*aLocation*/) const
{
    return EFalse;
}

EXPORT_C void CMsgBioControl::AddMenuItemL(CEikMenuPane& aMenuPane,
    TInt aStringRes, TInt aCommandOffset, TInt aPosition)
    {
    CEikMenuPaneItem::SData menuItem;
    menuItem.iCascadeId = NULL;
    menuItem.iFlags = NULL;
    menuItem.iText.Format(*StringLoader::LoadLC(aStringRes));
    CleanupStack::PopAndDestroy(); // (string)
    menuItem.iCommandId = iBioControlObserver.FirstFreeCommand()
        + aCommandOffset;
    aMenuPane.InsertMenuItemL(menuItem, aPosition);
    }

void CMsgBioControl::SetBioBodyControl( MMsgBioBodyControl* aBioBodyControl )
    {
    iBioBodyControl = aBioBodyControl;
    }

EXPORT_C TBool CMsgBioControl::NotifyEditorViewL(
    TMsgBioControlEventRequest aRequest,
    TInt aDelta)
    {
    if (iBioBodyControl)
        {
        return iBioBodyControl->HandleBaseControlEventRequestL(
            aRequest, aDelta);
        }
    return ETrue;
    }

EXPORT_C void CMsgBioControl::SetExtension(MMsgBioControlExtension* aExt)
    {
    iExt = aExt;
    }
    
#ifdef RD_SCALABLE_UI_V2
EXPORT_C TInt CMsgBioControl::ScrollL( TInt aPixelsToScroll,
                                        TMsgScrollDirection aDirection )
    {
    TInt retval(0);
 
    if(!iExt)
        {
        return retval;
        }

    MMsgBioControlScrollExtension* extension = 
                    static_cast<MMsgBioControlScrollExtension*>
                    ( iExt->BioControlExtension( KMsgBioControlScrollExtension ) );

    if(extension != NULL)
        {
        //call the extension method
        retval = extension->ExtScrollL( aPixelsToScroll, aDirection );
        }

    return retval;
    }

EXPORT_C void CMsgBioControl::NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam )
    {
    if(!iExt)
        {
        return;
        }

    MMsgBioControlScrollExtension* extension =
                    static_cast<MMsgBioControlScrollExtension*>
                    ( iExt->BioControlExtension( KMsgBioControlScrollExtension ) );

    if(extension != NULL)
        {
        extension->ExtNotifyViewEvent( aEvent, aParam );
        }
    }
#else
EXPORT_C TInt CMsgBioControl::ScrollL( TInt /*aPixelsToScroll*/,
                                       TMsgScrollDirection /*aDirection*/ )
    {
    //no op
    return 0;
    }

EXPORT_C void CMsgBioControl::NotifyViewEvent( TMsgViewEvent /*aEvent*/, TInt /*aParam*/ )
    {
    //no op
    }
#endif

//end of file
