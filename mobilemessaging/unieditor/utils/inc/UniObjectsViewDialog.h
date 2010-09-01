/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Unified Message Editor Objects view dialog.
*
*/


#ifndef C_UNIOBJECTSVIEWDIALOG_H
#define C_UNIOBJECTSVIEWDIALOG_H

// ========== INCLUDE FILES ================================

#include <MsgViewAttachmentsDialog.h>
#include <MsgAttachmentUtils.h>
#include <AknServerApp.h>

#include "unimodelconst.h"      // TUniLayout
#include <uniobject.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CSendUi;
class CMsvEntry;
class CMessageData;
class CUniObject;
class CUniDataModel;
class CUniDataUtils;
class CUniObjectsModel;
class CUniObjectsInfo;

// ========== DATA TYPES ===================================

enum TUniObjectsDialogExitCmd
    {
    EUniObjectsViewExit,
    EUniObjectsViewBack
    };

// ========== CLASS DECLARATION ============================

/**
*  Unified Editor Objects view dialog.
*
*  @lib UniUtils.lib
*
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CUniObjectsViewDialog ) :    public CMsgViewAttachmentsDialog,
                                                public MUniObjectObserver

    {
    public:

        enum TUniObjectsDialogType
            {
            EEditor,
            EViewer,
            EForward
            };

    public:

        /**
         * Factory method that creates objects view.
         *
         * @since S60 3.2
         *
         * @param aDialogType      type of the dialog (editor, viewer or forward)
         * @param aAttachmentModel reference to attachment model
         * @param aDocument        reference to document owning SmilModel
         * @param aExitCmd         Exit command issued for dialog
         * @return Return value of dialogs ExecuteLD   
         */
        IMPORT_C static TInt ObjectsViewL( TUniObjectsDialogType aDialogType,
                                           CUniObjectsModel& aAttachmentModel,
                                           CEikonEnv* aEikonEnv,
                                           TUniObjectsDialogExitCmd& aExitCmd,
                                           CUniDataModel& aDataModel,
                                           TInt aFocusedItemIndex = -1 );

        /**
         * Constructor.
         *
         * @since S60 3.2
         *
         * @param aTitle           title for dialog (shown in title pane)
         * @param aDialogType      type of the dialog (editor, viewer or forward)
         * @param aMenuId          resource id for menu
         * @param aAttachmentModel reference to attachment model
         * @param aDocument        reference to document owning SmilModel
         * @param aResourceOffset  offset of the resource file (got from
         *                         eikenv->AddResourceFileL)
         * @param aExitCmd         Exit command issued for dialog
         */
        CUniObjectsViewDialog( TDesC& aTitle,
                               TUniObjectsDialogType aDialogType,
                               TInt aMenuId,
                               CUniObjectsModel& aAttachmentModel,
                               TInt aResourceOffset,
                               TUniObjectsDialogExitCmd& aExitCmd,
                               CUniDataModel& aDataModel,
                               TInt aFocusedItemIndex );

        /**
         * Destructor.
         *
         * @since S60 3.2
         */
        virtual ~CUniObjectsViewDialog();

    public:   // from CAknDialog

        /**
         * From CAknDialog. See akndialg.h
         *
         * @since S60 3.2
         */
        void ProcessCommandL( TInt aCommandId );

        /**
         * From CAknDialog. See akndialg.h
         *
         * @since S60 3.2
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
         * From CAknDialog. See akndialg.h
         *
         * @since S60 3.2
         */
        void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );
        
        /** 
         * From CEikDialog
         * Sets the layout for the dialog. Called just before the dialog
         * is activated
         */
        void PreLayoutDynInitL();
        
    public: // from MApaServerAppExitObserver
    
        /**
        * Handle the exit of a connected server app.
        * This implementation provides S60 default behavior
        * for handling of the EAknCmdExit exit code. Derived classes
        * should base-call this implementation if they override this
        * function.
        *
        * @since S60 3.2
        *
        * @param aReason The reason that the server application exited.
        * This will either be an error code, or the command id that caused
        * the server app to exit.
        */
        void HandleServerAppExit( TInt aReason );

    private:
        
        /**
         * Loads form data.
         */
        void LoadFormDataL();
        
        /**
         * Remove attachment from model.
         * @param aIndex 
         */
        void RemoveAttachmentL( TInt aIndex );

        /**
         * Change order of attachment.
         * @param aCommandId   
         * @param aNewPosition 
         */
        void ChangeOrderL( TInt aCommandId, TInt aNewPosition );

        /**
         * From CCoeControl, returns help context
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const; 

        /**
         * By default Symbian constructor is private.
         */
        void ConstructL( CMsvEntry& aEntry );

        /**
         * "Calculates" on which slide we are.
         * @return slide number
         */
        TInt SlideNumber( TInt aIndex );

        /**
         * Checks whether there are both image and text present
         * @return ETrue if both image and text are present, EFalse otherwise
         */
        TBool IsImageAndTextPresent();

        /** 
         * Launches help application
         */
        void LaunchHelpL();

        /**
         * Processes menu command open.
         * @param aIndex Index of the item to be opened 
         */
        void ProcessCommandOpenL( TInt aIndex );

        /**
         * Processes menu command save.
         * @param aIndex Index of the item to be saved 
         */
        void ProcessCommandSaveL( TInt aIndex );

        /**
         * Processes menu command remove.
         * @param aIndex Index of the item to be removed 
         */
        void ProcessCommandRemoveL( TInt aIndex );

        /**
         * Processes menu command send.
         * @param aIndex Index of the item to be removed 
         */
        void ProcessCommandSendL( CMessageData& aMsgData, 
                                  TInt aIndex, 
                                  TUid aServiceId,
                                  TUid aBioUid );

        /**
         * Creates a temporary text file in Unicode format
         */
        void CreateTempTextFileL( CUniObject* aObject, TFileName& aTempFileName );

        /**
         * Creates a temporary path
         */
        void CreateTempPathL( const TDesC& aFileName, TFileName& aTempFileName );

        /**
         * Checks whether an attachment file is of bio message type
         */
        TBool BioMessageTypeL( TPtrC8 aMimeType,
                               TUid& aBioUid );
    
        /*
        * PopulateAttachmentModelL
        * Populates given model from smilmodel contents
        *
        * @param aObjectsViewModel
        */
        static void PopulateAttachmentModelL( CUniObjectsModel& aObjectsViewModel,
                                              CUniDataModel& aUniDataModel );
        
        /**
        * Creates an CUniObjectsInfo* from an CUniObject
        * to be used when populating objects view model
        *
        * @param aObjectsViewModel
        * @param aObject - Object to get info from.
        * @param aAttachment - Indicate the object is attachment object
        *                      (i.e. not referenced from SMIL)
        * @param aSlideNumber - Number of slide on which object is located.
        * @return new CUniObjectsinfo object
        */
        static CUniObjectsInfo* CreateObjectsInfoL( CUniObjectsModel& aObjectsViewModel,
                                                    CUniObject& aObject,
                                                    TBool aAttachmentObject,
                                                    TInt aSlideNumber );
        
        /**
        * Resolves DRM type to CUniObject object.
        *
        * @param aObject - Object to get info from.
        *
        * @return DRM type
        */
        static TInt GetObjectsInfoDRMTypeL( CUniObject& aObject );

        /// MUniObjectObserver
        void HandleUniObjectEvent(  CUniObject&  aUniObject,
                                    TUniObjectEvent aUniEvent );

        /**
        * Checks whether objects has drm rights left just now
        * @return TBool
        */                
        TBool IsRightsNow( CUniObject& aObject ) const;

        /**
        * Checks whether object has drm rights when downloaded to the current slide
        * @return TBool
        */                
        TBool IsDrmRightsWhenLoaded( CUniObject& aObject ) const;
        
    private:  // data

        TUniObjectsDialogType  iDialogType;
        
        TInt iResourceOffset;
        
        CUniDataModel& iDataModel;

        CSendUi* iSendUi;

        TInt iSupportedFeatures;

        TUniObjectsDialogExitCmd iExitCmd;
        
        TInt iFocusedItemIndex;
    };

#endif // C_UNIOBJECTSVIEWDIALOG_H

// End of File
