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
* Description:  PostcardAppUi  declaration
*
*/



#ifndef POSTCARDAPPUI_H
#define POSTCARDAPPUI_H

// ========== INCLUDE FILES ================================

#include <MsgEditorAppUi.h>             // CMsgEditorAppUi
#include <MsgEditor.hrh> //zoom
#include <MsgEditorSettingCacheUpdatePlugin.h>//zoom

#include <MMGFetchVerifier.h>           // MMGFetchVerifier
#include <MsgAttachmentUtils.h>			// fetchtype

#include "PostcardDocument.h"           // TPostcardMsgType
#include "PostcardOperationObserver.h"  // MPostcardOperationObserver
#include "PostcardQueryWaiter.h"        // PostcardQueryWaiter
#ifdef RD_SCALABLE_UI_V2
#include "PostcardPointerObserver.h"    // MPocaPointerEventObserver
#endif


// ========== CONSTANTS ====================================

const TInt KPostcardAmountOfFreeDiskSpaceNeeded = 2000;

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CAknNavigationControlContainer;
class CAknTitlePane;
class CAknInputBlock;
class CAknWaitDialog;
class CAknGlobalMsgQuery;
class CEikMenuBar;
class CEikMenuPane;

class CIdle;
class CPlainText;
class CContactCard;
class CFbsBitmap;
class CGulIcon;

class CMsgImageInfo;

class CPostcardOperation;
class CPostcardController;
class CPostcardCenRep;

// ========== CLASS DECLARATION ============================

/**
 * Postcard AppUi module
 *
 */
class CPostcardAppUi :
    public CMsgEditorAppUi,
    public MMGFetchVerifier,
    public MPostcardOperationObserver,
#ifdef RD_SCALABLE_UI_V2
    public MPocaPointerEventObserver,
#endif // RD_SCALABLE_UI_V2
    public MPostcardQueryWaiterCallback
{

public:

    /**
     * Constructor.
     */
    CPostcardAppUi();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    ~CPostcardAppUi();

    /**
     * Route key events to view.
     * @param aKeyEvent key event
     * @param aType     event type
     * @return          response
     * from CCoeAppUi
     */
    TKeyResponse HandleKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
    * From CEikAppUi 
    */
    void HandleCommandL( TInt aCommand );

    /**
    * From MEikMenuObserver 
    */
    void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );


private:

    /**
     * Inits the main menu options
     * @param aMenuPane
     */
    void DynInitMainMenuL( CEikMenuPane* aMenuPane );

    /**
     * Inits the Insert submenu options
     * @param aMenuPane
     */
    void DynInitInsertSubMenuL( CEikMenuPane* aMenuPane );

    /**
     * Inits the context menu options
     * @param aMenuPane
     */
    void DynInitContextMenuL( CEikMenuPane* aMenuPane );

    /**
     * Checks if option aMenuItem is invisible
     * @param aMenuitem
     * @return ETrue if invisible
     */
    TBool IsMainMenuOptionInvisible( TInt aMenuItem );

    /**
    * Creates a media info object from given file and
    * performs a preliminary check whether it is ok to 
    * insert the given media file.
    *
    * The media info object is created to "iImageInfo"
    * member variable.
    *
    * @param aFileName  Media file to be checked
    * @return ETrue if media info object was created
    *         EFalse otherwise
    */        
    TBool CreateMediaInfoForInsertL( const TDesC& aFileName );

    /**
    * Handles selection key press. Either opens text/greeting dialog or open context menu
    */
    void DoSelectionKeyL();

public:

    /** 
    * Launches help application
    * @param aContext launches the aContext help page from Postcard manual
    */
    void LaunchHelpL( const TDesC& aContext );

    /**
    * Accessor
    * Get reference to file session
    * @return Reference to file session
    */        
    RFs& FsSession() const;

    /**
     * Tries to exit the application.
     */
    void DoExternalExit( );

    /**
    * Callback for CIdle
    */        
	static TInt DelayedExit( TAny* aThis );

    /**
    * From MMGFetchVerifier
    */
    TBool VerifySelectionL( const MDesCArray* aSelectedFiles );

    /**
    * From MMsgEditorLauncher
    */
    void LaunchViewL();

    /**
    * From CMsgEditorAppUi
    */
    void DoMsgSaveExitL();

    /**
    * From MPostcardOperationObserver
    * Checks the result of the event and shows possible error note.
    * Calls right function to finalize launch, save or image insertion.
    */
    void PostcardOperationEvent(
            TPostcardOperationType aOperation,
            TPostcardOperationEvent aEvent );

	/**
	* From MPostcardQueryWaiterCallback
	* If aResult is ok, starts the sending operation (save operation with right flag).
	* Otherwise just deletes the query and returns.
	*/
	void QueryWaiterCallbackL( TInt aResult );

	/**
	* From MPostcardQueryWaiterCallback
	* Called when QueryWaiterCallbackL() leaves. Should perform cleanup.
	* After the callback, the leave is propagated to active scheduler.
	*/
	void QueryWaiterCallbackError();

    /**
     * Returns application menubar.
     * @return pointer to menubar.
     */
    CEikMenuBar* MenuBar() const;

    /**
     * Asks the controller to reread the coordinates from the laf
     */
    void HandleResourceChangeL( TInt aType );

    /**
     * Starts CIdle to call ExternalExit
     */
	void DoDelayedExit( TInt aDelayTime );

    /**
    * Save message and exits
    */
    void ExitAndSaveL();

    /**
    * Shows "closing message" list query. The options
    * in the query are "Save to draft" and "Delete message".
    * Should be called when "Close" softkey is pressed in 
    * non-empty message.
    *
    * @return EMsgCloseCancel, if the query is cancelled
    *         EMsgCloseSave, if "Save to drafts" is selected
    *         EMsgCloseDelete, if "Delete message" is selected
    */
    TInt ShowCloseQueryL();

    /**
     * Takes the ownership of the aIcon and also passes the icon to controller
     */
    void SetImage( CGulIcon* aIcon );

    /**
     * Takes the ownership of the aItem.
     * Takes the right fields from aItem and writes them into a bitmap
     * Passes the bitmap to controller.
     */
    void SetRecipientL( CContactCard& aItem );

    /**
     * Takes the ownership of the aPlainText.
     * Writes the text into a bitmap.
     * Passes the bitmap to controller.
     */
    void SetTextL( CPlainText& aPlainText );

    /**
     * Get maximum text length allowed for edit control
     */
    TInt MaxTextLength( TInt aControlId );

    /**
     * Return reference to cenrep parameters
     */
    const CPostcardCenRep& CenRep();

    /**
    * Gets the local zoom level
    */
    void GetZoomLevelL( TInt& aZoomLevel );

    /**
    * Handles the changing of the local zoom by calling CMsgEditorAppUi::HandleLocalZoomChangeL
    * @param aNewZoom is the new zoom level
    */
    void HandleLocalZoomChangeL( TMsgCommonCommands aNewZoom );


protected:

    /**
    *  Active scheduler start for saving.
    */
    void BeginActiveWait();
    
    /**
    *  Active scheduler stop for saving.
    */
    void EndActiveWait();

#ifdef RD_SCALABLE_UI_V2
private: // from MPocaPointerEventObserver
    void PocaPointerEventL( TPocaPointerEvent aEvent, TPocaPointerEventType aEventType );
#endif // RD_SCALABLE_UI_V2

private:

    /**
    * Called when open operation finishes. Removes wait note.
    * Updates navipane, checks if greeting or address fields are too long.
    * Focuses the right object.
    */
    void CompleteLaunchViewL( );

    /**
    * Checks whether there is enough disk space to save message or image.
    */
    TBool CanSaveMessageL( TInt aAmount = KPostcardAmountOfFreeDiskSpaceNeeded );

    /**
    * Checks message content and start the sending query
    */        
    void DoSendL();

    /**
    * Starts the saving operation.
    */
    void DoSaveL();

    /**
    * Shows the save query if necessary and calls DoSave if needed
    */        
    void DoBackSaveL();

    /**
    * Deletes image if it's focused
    */        
    void DoBackspaceL( );

    /**
    * Calls MTMUI's Forward feature to forward the postcard to someone else
    */        
    void DoForwardMessageL( );

    /**
    * Opens the address dialog.
    * param aKeyEvent - if set, SimulateKeyEvent is called for it in the dialog
    * param aFirstFocused - if set, focuses the given field when dialog started
    */        
    void DoEditAddressL( TKeyEvent aKeyEvent = TKeyEvent(),
                               TInt aFirstFocused = -1 );

    /**
    * Opens the greeting text dialog
    * param aKeyEvent - if set, SimulateKeyEvent is called for it in the dialog
    */        
    void DoEditGreetingL( TKeyEvent aKeyEvent = TKeyEvent( ) );

    /**
    * Updates TMsvEntry and calls DoShowMessageInfoL
    */        
    void DoUpdateEntryAndShowMessageInfoL( );

    /**
    * Calls MsgAttachmentUtils::FetchFileL to fetch file aFetchType
    */        
	TBool FetchFileL( MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType );

    /**
    * Call FetchFileL with right parameter
    * @param aFetchType: EImage or ENewImage
    */        
    void DoInsertImageL(
        MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType );

    /**
    * Calls NextMessageL if next/prev message is available
    */        
	TKeyResponse NavigateBetweenMessagesL( TBool aNextMessage );
	
public: // These are functions that also address and greeting dialogs use

    /**
    * Creates and shows an information note with text string aResourceId
    */
    void ShowInformationNoteL( TInt aResourceID, TBool aWaiting );
    
    /**
    * Creates and shows an information note with text string aText
    */
    void ShowInformationNoteL( const TDesC& aText, TBool aWaiting );
    
    /**
    * Calls global error UI to show an information note with text string aResourceId.
    */
    void ShowGlobalInformationNoteL( TInt aResourceId, TBool aWaiting );

    /**
    * Creates and shows a confirmation query.
    * First loads the string aResourceId from the resource file.
    * Then calls the other ShowConfirmationQuery to show the query.
    */
    TInt ShowConfirmationQueryL( TInt aResourceId, TBool aOnlyOK=EFalse ) const;

    /**
    * Creates and shows a confirmation query.
    * Creates a query dialog with text aText. If aOnlyOK is true, the softkeys
    * are OK-Empty. Otherwise they are Yes-No.
    * Returns the value returned by the dialog.
    */
    TInt ShowConfirmationQueryL( const TDesC& aText, TBool aOnlyOK=EFalse ) const;

    
    /**
    * Puts a wait note on the screen with text aResourceId
    * Stays on the screen until RemoveWaitNote is called
    * param aNoTimeout - if true, note is shown immediately
    * result true if the dialog was successfully shown
    */        
    TBool ShowWaitNoteL( TInt aResourceId, TBool aNoTimeout );

    /**
    * Removes possible current wait note from the screen.
    */
    void RemoveWaitNote();

    /**
    * Reads and returns the mandatory fields' byte from the resource
    */
	TInt MandatoryAddressFields( );
    
    /**
    * Reads string aResourceId from the resource file and leaves in the callstack
    * return the read string
    */
	HBufC* ReadFromResourceLC( TInt& aResourceId );

    /**
    * Calls MTMUI to show the message info in the screen.
    */        
    void DoShowMessageInfoL();
	
    /**
    * Called by Text editor dialog to ask if the recipient field has any data
    * Returns ETrue if iContact has any data.
    */        
	TBool ContactHasData( );

private:
    
    /**
    * Returns true if there's no image, no text nor recipient address
    */        
    TBool IsMessageEmpty( );

    /**
    * Returns a reference to the document class
    */        
	CPostcardDocument& Document();

    /**
    * Creates a set of address strings by using iContact
    * and passes the strings into CreateTransparentIconL.
    * Passes the returned transparent bitmap to Controller class.
    */        
    void RefreshRecipientBitmapL( );

    /**
    * Refreshes the lines how the greeting and recipients focus lines are drawn
    */        
    void RefreshFocusLinesL( );

    /**
    * Uses iGreeting as source text.
    * Writes the text into a transparent bitmap and passes it into controller
    */        
	void CreateGreetingBitmapL( );

    /**
    * Creates a transparent icon with text aText written into it.
    */        
	CGulIcon* CreateTransparentIconL( const TDesC& aText );

    /**
    * Creates a transparent icon with text aStrings line by line written into it.
    */        
	CGulIcon* CreateTransparentIconL( const MDesCArray& aStrings );

    /**
    * Creates an empty bitmap of size aSize.
    * Used by the functions above.
    */        
	CFbsBitmap* CreateEmptyBitmapLC( const TSize& aSize );
	
    /**
    * Returns ETrue if aContact's any field has any data
    */        
	TBool ContactHasData( const CContactCard& aContact );

    /**
    * Reads the max lenghts of each field from resources.
    * Returns EFalse if either greeting text or any of the address fields
    * are too long.
    */        
	TBool CheckLengthsL( );
	
    /**
    * Creates and returns the string that is shown in the send confirmation query
    * after user has chosen to send the postcard.
    */        
	HBufC* CreateAddressStringForSendConfirmLC( );

    /**
    * Set main view middle softkey label according to the postcard side
    * viewed and mode (viewer / edit).
    */        
	void SetMiddleSoftkeyL();
    
    /**
    * Creates and sets application icon to title pane.
    */
    void CreateAndSetTitleIconL();
    
    /**
    * Sets application icon to title pane.
    */
    void SetTitleIconL();
    
    /**
     * Checks if phone is in offline mode or not
     *
     * @return ETrue if phone is in offline mode
     *         Otherwise EFalse.
     */
    TBool IsPhoneOfflineL() const;

private:

	enum TPostcardFlags
		{
		EPostcardGreetingActive = 1,        // Is Greeting dialog open
		EPostcardRecipientActive = 2,      // Is Address dialog open
		EPostcardCamcorder = 4,		    // Is Camcorder supported
		EPostcardEditMessageActive = 8,    // Is Edit message active
		EPostcardHelp = 16,		        // Is Help supported
		EPostcardExitCalledFromDialog = 32,// Is exit called from text or address dialogs
		EPostcardRunAppShutterAtExit = 64,
		EPostcardLaunchSuccessful = 128,
		EPostcardClosing = 256,
        EPostcardFeatureOffline = 512   // Is offline supported
		};

    enum TPostcardFocusArea
        {
        EPostcardFocusAreaGreeting = 0,
        EPostcardFocusAreaAddress,
        EPostcardFocusAreaLast
        };

    enum TPostcardCloseQueryOptions
        {
        EPostcardCloseCancel,
        EPostcardCloseSave,
        EPostcardCloseDelete
        };

	TInt						iPocaFlags;         // postcard flags
    TInt                        iResourceFile;      // Offset of resource file
    TInt                        iBaseResourceFile;  // Offset of editor base res file

    CAknNavigationControlContainer*    	iNaviPane;
    CAknTitlePane*              iTitlePane;
    CGulIcon*                   iAppIcon;
    
    CMsgImageInfo*              iImageInfo;     // media info created by verify selection

    CPostcardOperation*     	iOperation;     // current postcard operation
    CAknInputBlock*             iAbsorber;      // For absorbing keypresses
    CPeriodic*                  iIdle;          // For making external exit
    CPostcardController*        iController;    // Postcard controller class
    
    // Global message query
    CAknGlobalMsgQuery*         iMessageQuery;  // Message query for confirming send operation
	HBufC*						iSendText;      // query text used by MsgQuery

	CAknWaitDialog*				iWaitDialog;    // wait dialog to be shown on screen
    TPostcardMsgType            iMsgType;       // type of postcard that is open
    CGulIcon*                   iImage;         // image that user has inserted
    CPlainText*               	iGreeting;      // greeting text that user has entered
    TInt                        iGreetingLength;// length of greeting text (excluding end separator)
	CContactCard*				iContact;       // recipient address that user has entered

    TInt                        iMskResource;   // current middle softkey resource ID
    CPostcardCenRep*            iCenRep;    // central repository interface
    
    CActiveSchedulerWait        iWait;
};

#endif

// End of File
