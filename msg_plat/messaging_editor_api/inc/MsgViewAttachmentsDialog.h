/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgViewAttachmentsDialog  declaration
*
*/



#ifndef CMSGVIEWATTACHMENTSDIALOG_H
#define CMSGVIEWATTACHMENTSDIALOG_H

// ========== INCLUDE FILES ================================
#include <eiklbx.h>
#include <eikfrlb.h>        // for CFormattedCellListBoxItemDrawer
#include <aknlists.h>       // for CAknDoubleLargeStyleListBox
#include <AknForm.h>
#include <AknServerApp.h>

#include <MsgAttaListItemArray.h>
#include <MsgAttachmentItemDrawer.h>
#include <MsgAttachmentsListBox.h>

// ========== CONSTANTS ====================================

const TInt KMsgAttaItemTextLength = 256;

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CGulIcon;
class CAknTitlePane;
class CMsgAttachmentModel;
class CMsgAttachmentInfo;
class CDocumentHandler;
class CAknWaitDialog;
class CAknNavigationControlContainer;
class CMsvEntry;
class CAiwGenericParamList;

// ========== CLASS DECLARATION ============================

/**
* View attachments dialog.
*/
class CMsgViewAttachmentsDialog : public CAknDialog,
                                  public MEikListBoxObserver,
                                  public MAknServerAppExitObserver
    {
    public:

        /**
        * Constructor.
        * @param aTitle           title for dialog (shown in title pane)
        * @param aMenuId          resource id for menu
        * @param aAttachmentModel reference to attachment model
        */
        IMPORT_C CMsgViewAttachmentsDialog( TDesC& aTitle,
                                            TInt aMenuId,
                                            CMsgAttachmentModel& aAttachmentModel);

        /**
        * Destructor.
        */
        IMPORT_C ~CMsgViewAttachmentsDialog();

        /**
        * 2nd phase constructor.
        * Deprecated.
        */
        IMPORT_C void ConstructL();

        /**
        * 2nd phase constructor.
        * @param aMessageEntry current message entry
        */
        IMPORT_C void ConstructL( CMsvEntry& aMessageEntry );

    public: // new functions

        /**
        * Returns the index of currently selected listbox item or KErrNotFound if list
        * empty or no selection.
        * @return current index
        */
        IMPORT_C TInt CurrentListItemIndex();

        /**
        * Updates icon of list item indicated by 'aListIndex'.
        * @param aListIndex
        */
        IMPORT_C void UpdateIcon( TInt aListIndex );

        /**
        * Function that TCleanupItem calls. Used for clearing iBusy flag.
        */
        IMPORT_C static void CleanupPointer( TAny* aItem );

        /**
        * Handle listbox item addition. Call this after you have added attachment
        * to the attachment model. If you inserted attachment to some other
        * position than last, use same index as aIndex.
        * @param aIndex item index
        */
        IMPORT_C void ItemAddedL( TInt aIndex = -1 );

        /**
        * Handle listbox item removal. Call this after you have removed attachment
        * from the attachment model.
        * @param aIndex item index
        */
        IMPORT_C void ItemRemovedL( TInt aIndex );

    public:   // from CAknDialog

        /**
        * Processes menu commands.
        * @param aCommandId
        */
        IMPORT_C void ProcessCommandL( TInt aCommandId );

        /**
        * Removes default menu items from CAknFrom and adds own custom menu
        * items whose resource id was given in construstor.
        * @param aMenuId
        * @param aMenuPane
        */
        IMPORT_C void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );

        /**
        * From CCoeControl
        */
        IMPORT_C void HandleResourceChange( TInt aType );
                
    public: // from MAknServerAppExitObserver
    
        /**
        * Handle the exit of a connected server app.
        * This implementation provides S60 default behavior
        * for handling of the EAknCmdExit exit code. Derived classes
        * should base-call this implementation if they override this
        * function.
        *
        * @since S60 5.0
        *
        * @param aReason The reason that the server application exited.
        * This will either be an error code, or the command id that caused
        * the server app to exit.
        */
        IMPORT_C void HandleServerAppExit( TInt aReason );                
        
    protected: // from CAknDialog

        /**
        *
        * @param aButtonId
        * @return
        */
        IMPORT_C TBool OkToExitL( TInt aButtonId );

        /**
        * Create custom control.
        * @param aControlType
        * @return
        */
        IMPORT_C SEikControlInfo CreateCustomControlL( TInt aControlType );

        /**
        * Set the generic parameter list used in opening content CDocumentHandler
        * Ownership is transferred to base class
        */
        IMPORT_C void SetOpenParamList( CAiwGenericParamList* aOpenParamList );

        /**
        * Set the generic parameter list used in opening content CDocumentHandler
        * Ownership is transferred to base class
        */
        IMPORT_C CAiwGenericParamList* OpenParamList();

    protected: // from MEikListBoxObserver

        /**
        * Handles list box events.
        * @param aListBox
        * @param aEventType
        */
        IMPORT_C void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

    protected: // from CEikDialog

        /**
        *
        */
        IMPORT_C void PreLayoutDynInitL();
        
        /**
        *
        */
        IMPORT_C void HandleControlStateChangeL( TInt aControlId );
        
    private:

        /**
        * Common part for ConstructL methods.
        */
        void DoConstructL();

        /**
        * Prepare icons and listbox.
        */
        IMPORT_C virtual void LoadFormDataL();

        /**
        * Inserts given file to attachment model, fetches the bitmap for file
        * and appends new item to the listbox.
        * @param aFileName  file name
        * @param aCommandId menu command id
        * @param aIndex     attachment index
        */
        IMPORT_C virtual void InsertAttachmentL( const TFileName& aFileName,
                                                 TInt aCommandId,
                                                 TInt aIndex = -1 );

        /**
        * Remove attachment from model.
        * @param aIndex attachment index
        */
        IMPORT_C virtual void RemoveAttachmentL( TInt aIndex );

        /**
        * Function that TCleanupItem calls.
        */
        static void CleanupWaitDialog( TAny* aAny );
        
        /**
        * Performs text file saving
        */
        TInt DoSaveTextFileL( RFile& aFile, TInt aCharSet );
        
        /**
        * Performs UI updating.
        */
        void DoUpdateUiL();
        
        /**
        * Update CBA button visibility.
        */
        void UpdateButtonVisiblity( TInt aCommandId, TBool aValue );
        
    protected: // data

        CMsgAttachmentModel&         iAttachmentModel;
        CMsgAttaListItemArray*       iListItems;
        CArrayPtr<CGulIcon>*         iIconArray;
        CMsgAttachmentsListBox*      iListBox;
        CDocumentHandler*           iDocHandler;
        TInt                        iBusy;
        CMsvEntry*                  iMessageEntry;

    private: // data

        HBufC*                       iOldTitle;
        const TDesC&                 iNewTitle;
        CAknTitlePane*               iTitlePane;
        TInt                         iMenuId;
        CAknWaitDialog*              iWaitDialog;
        CAknNavigationControlContainer* iNaviPane;
        RFile                           iREMOVED; // Kept only for maintaining BC
        // If opening through CDocumentHandler needs parameters,
        // derived class must insert parameters here.  
        // The iOpenParamList is initially empty
        CAiwGenericParamList*       iOpenParamList;
    };

#endif // CMSGVIEWATTACHMENTSDIALOG_H

// End of File
