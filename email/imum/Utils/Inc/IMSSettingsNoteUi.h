/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains functions to show notes in UI
*
*
*/


#ifndef IMSSETTINGSNOTEUI_H
#define IMSSETTINGSNOTEUI_H

// INCLUDES
#include <e32base.h>
#include <e32cmn.h>
#include <AknQueryDialog.h>
#include <ImumUtils.rsg>
#include "MuiuDynamicHeader.h"
#include "MuiuDynamicSettingsDialog.h"
#include <AknInfoPopupNoteController.h>

// CONSTANTS
const TInt KMaxVisibleTime = KMaxTInt / 1000;
enum TIMSNotes
    {
    EIMSConfirmationNote = 0,
    EIMSInformationNote,
    EIMSErrorNote,
    EIMSWarningNote
    };

enum TIMSErrors
    {
    // Common
    EIMSNoError = 0,
    EIMSItemInvalid,
    EIMSItemEmpty,
    // Mailbox
    EIMSMailboxNameExists,
    EIMSMailboxNameEmpty,
    EIMSMailboxNameRejected,
    // Always Online
    EIMSAoAlwaysAsk,
    EIMSAoIAPPassword,
    EIMSAoNoLogin,
    EIMSAoOmaEmnOn,
    EIMSOmaEmnAoOn,
    EIMSAoManyAlwaysOnline,
    EIMSAoFetchNotAccepted,
    EIMSAoIapTypeCSD
    };

// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CImumInternalApiImpl;
class CIMSSettingsBaseUI;
class CAknResourceNoteDialog;
class CMuiuFlags;

// CLASS DECLARATION

/**
*
*  @lib
*  @since S60 3.0
*/
NONSHARABLE_CLASS( CIMSSettingsNoteUi ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Creates object from CIMSSettingsNoteUi and leaves it to cleanup stack
        * @since S60 3.0
        * @return, Constructed object
        */
        static CIMSSettingsNoteUi* NewLC(
            CImumInternalApiImpl& aEmailApi,
            CIMSSettingsBaseUI& aDialog );

        /**
        * Create object from CIMSSettingsNoteUi
        * @since S60 3.0
        * @return, Constructed object
        */
        static CIMSSettingsNoteUi* NewL(
            CImumInternalApiImpl& aEmailApi,
            CIMSSettingsBaseUI& aDialog );

        /**
        * Destructor
        * @since S60 3.0
        */
        virtual ~CIMSSettingsNoteUi();

    public: // New functions

        /**
        * Shows the query for user and waits for user answer.
        *
        * @since S60 3.0
        * @param aTextResource Resource string, that shall be shown in note
        * @param aDialogResource ResourceId of dialog, which shall be used
        *        Locate the resource dialogs from emailutils.ra
        * @param aTone Tone to be played with query
        *        Class CAknQueryDialog defines the TTone type, which contains
        *        following enumerations:
        *           - ENoTone
        *           - EConfirmationTone
        *           - EWarningTone
        *           - EErrorTone
        *        Example usage: CAknQueryDialog::EConfirmationTone
        * @return Command returned from the query
        */
        IMPORT_C static TInt ShowQueryL(
            const TUint aTextResource,
            const TInt  aDialogResource = R_EMAIL_CONFIRMATION_QUERY,
            const TBool& aClearBackground = EFalse,
            const CAknQueryDialog::TTone aTone = CAknQueryDialog::ENoTone );

        IMPORT_C static TInt ShowQueryL(
            const TDesC& aText,
            const TInt  aDialogResource = R_EMAIL_CONFIRMATION_QUERY,
            const TBool& aClearBackground = EFalse,
            const CAknQueryDialog::TTone aTone = CAknQueryDialog::ENoTone );

        /**
         * 
         *
         * @since S60 v3.2
         */    
        template<TInt TLength>    
        static inline TInt ShowQueryL(
            const TInt aTextResource,
            const TDesC& aAdditionalText,            
            const TInt aDialogResource = R_EMAIL_CONFIRMATION_QUERY,
            const TBool& aClearBackground = EFalse,
            const CAknQueryDialog::TTone& aTone = CAknQueryDialog::ENoTone );
            

        /**
        * This function fetches the text from given resource file and shows
        * the string found from it in small note box.
        * More info in file aknnotewrappers.h
        *
        * @since S60 3.0
        * @param aNoteType Type of note to be shown
        *        Avkon offers several type of notes, which differs from each
        *        other. There are following types of notes available:
        *           - EMsvConfirmationNote
        *           - EMsvInformationNote
        *           - EMsvErrorNote
        *           - EMsvWarningNote
        * @param aNoteWaiting Parameter to create the note
        *        When parameter is TRUE, created note will be waitnote
        *        When parameter is FALSE, note is not waitnote
        * @param aResource Resource string, that shall be shown in note
        * @param aFile File, from the string shall be read
        */
        IMPORT_C static void ShowNoteL(
            const TUint      aResource,
            const TIMSNotes& aNoteType = EIMSErrorNote,
            const TBool      aNoteWaiting = EFalse );

        IMPORT_C static void ShowNoteL(
            const TDesC&      aText,
            const TIMSNotes& aNoteType = EIMSErrorNote,
            const TBool      aNoteWaiting = EFalse );

        /**
        * @since S60 3.2
        */
        IMPORT_C static void ShowNoteL(
            const TUint aResource,
            TIMSNotes aNoteType,
            TBool aNoteWaiting,
            TInt aTimeOut
            );

        /**
        *
        * @since S60 3.0
        */
        static HBufC* MakeStringLC( const TUint aTextResource );

        /**
        *
        * @since S60 3.0
        */
        static void MakeString(
            TDes& aDest,
            TInt aResourceId,
            TInt aValue );

        /**
        *
        * @since S60 3.0
        */
        static void MakeStringL(
            TDes& aDest,
            TInt aResourceId,
            const TDesC& aSubs );

        /**
        *
        * @since S60 3.2
        */
        static void MakeString(
            TDes& aDest,
            const TDesC& aTag,
            const TDesC& aSubs );

        /**
        *
        * @since S60 3.0
        */
        static CAknInfoPopupNoteController* ShowPopupLC(
            const TInt aResource,
            const TInt aAfterMs,
            const TInt aPopupVisibleTime = KMaxVisibleTime );

        /**
        *
        * @since S60 3.0
        */
        static CAknInfoPopupNoteController* ShowPopupL(
            const TInt aResource,
            const TInt aAfterMs,
            const TInt aPopupVisibleTimeMs = KMaxVisibleTime );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult ShowDialog(
            const CMuiuSettingBase& aBaseItem,
            const TIMSErrors aError,
            const TDesC& aNewText );

    public: // Functions from base classes

    protected:  // Constructors

        /**
        * Default constructor for classCIMSSettingsNoteUi
        * @since S60 3.0
        * @return, Constructed object
        */
        CIMSSettingsNoteUi(
            CImumInternalApiImpl& aEmailApi,
            CIMSSettingsBaseUI& aDialog );

        /**
        * Symbian 2-phase constructor
        * @since S60 3.0
        */
        void ConstructL();

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult ShowProtocolErrorNoteL(
            const TIMSErrors aError );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult ShowEmailAddressErrorNoteL(
            const TIMSErrors aError );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult ShowServerErrorNoteL(
            const TIMSErrors aError );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult ShowIapErrorNoteL(
            const TIMSErrors aError );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult ShowWizMailboxNameErrorNoteL(
            const TIMSErrors aError );

        /**
        *
        * @since S60 3.0
        */
        TBool ShowMailboxAlreadyExistsErrorNoteL(
            const TDesC& aMailboxName );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult ShowSetMailboxNameErrorNoteL(
            const TIMSErrors aError,
            const TDesC& aMailboxName );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult ShowProtocolReadOnlyErrorNoteL(
            const TIMSErrors aError );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult ShowAlwaysOnlineErrorNoteL(
            const TIMSErrors aError );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult ShowAoCsdIapError();

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult ShowAlwaysOnlineRemovalQueryL(
            const TIMSErrors aError );

        /**
        *
        * @since S60 3.0
        */
        TMuiuPageEventResult ShowFetchLimitErrorNoteL(
            const TIMSErrors aError );
            
            
        /**
        * Selects which dialog to show based on aBaseItem
        * @param aBaseItem determines which note should be shown
        * @param aError tells what error has occurred
        * @return users answer
        * @since S60 3.2
        */
        TMuiuPageEventResult SelectDialogL( 
           const CMuiuSettingBase& aBaseItem,
           const TIMSErrors aError,
           const TDesC& aNewText );

        /**
        * @since S60 3.2
        */
        static CAknResourceNoteDialog* NewNoteL( TIMSNotes aNoteType,
            TBool aNoteWaiting );
        
        /**
         * Removes a screen clearer from the stack if a leave occurs.
         * @param aScreenClearer ScreenClearer pointer.
         */
        static void ScreenClearerCleanupOperation( TAny* aScreenClearer );

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        // Dialog that owns the notification UI
        CIMSSettingsBaseUI&     iDialog;
        // Internal mail API implementation
        CImumInternalApiImpl&       iMailboxApi;
    };

#include "IMSSettingsNoteUi.inl"

#endif // IMSSETTINGSNOTEUI_H