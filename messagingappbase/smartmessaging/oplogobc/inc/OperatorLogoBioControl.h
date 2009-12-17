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
*     Bio control for Operator Logos.
*
*/



#ifndef COPERATORLOGOCONTROL_H
#define COPERATORLOGOCONTROL_H

// INCLUDES
#include <msgbiocontrol.h>              // for CMsgBioControl
#include <RPhCltServer.h>               // RPhCltServer
#include <bldvariant.hrh>

// MACROS

// FORWARD DECLARATIONS
class COpLogoAOCallBack;
class MIHLScaler;

#ifdef RD_PHONE_CLIENT_EXT
class CPhCltImageHandler;
class CPhCltImageParams;
#else
class CPhCltExtImageHandler;
class CPhCltExtImageParams;
#endif

// CLASS DECLARATION

/**
 * Bio control for Operator Logos.
 */
class COperatorLogoBioControl:
    public CMsgBioControl
    {
    public:  // Constructor and destructor

        /**
         * Two-phased constructor
         * @param aObserver Reference to the Bio control observer.
         * @param aSession Reference to Message Server session.
         * @param aId Id of the message.
         * @param aEditorOrViewerMode Enum control's mode
         * @param aFile A pointer to a file name of file based bio
         *        control.         
         * @return The newly created object.
         */
        IMPORT_C static CMsgBioControl* NewL(
            MMsgBioControlObserver& aObserver,
            CMsvSession* aSession,
            TMsvId aId,
            TMsgBioMode aEditorOrViewerMode,
            const RFile* aFile);
         
        /**
        * Destructor.
        */
        ~COperatorLogoBioControl();


    public: // Functions from virtual MMsgBioControl

        /**
        * Calculates and sets size for a Bio control
        * according to aSize.
        * The height of the Bio control may be less or more than requested by
        * aSize, but the width must be exactly the same. If width of the Bio
        * control is not the same as given by aSize, the width must be reset
        * back to requested one.
        * @param aSize A reference to the suggested size and new size.
        */
        void SetAndGetSizeL( TSize& aSize );

        /**
        * This is called by the container to allow the Bio
        * control to add a menu item.
        * @param aMenuPane Reference to the application's menu.
        */
        void SetMenuCommandSetL( CEikMenuPane& aMenuPane );

        /**
        * Returns a rectangle slice of the bio controls
        * viewing area. It is used by the CMsgEditorView class for scrolling
        * the screen.
        * @return TRect to show viewing area
        */
        TRect CurrentLineRect() const;

        /**
        * Returns true if Focus change is possible.
        * @param aDirection The direction to be checked.
        * @return if it is possible, returns ETrue, and vice versa
        */
        TBool IsFocusChangePossible( TMsgFocusDirection aDirection ) const;

        /**
        * The container application obtains a header text
        * from the bio control.
        * @return The header text.
        */
        HBufC* HeaderTextL()  const;

        /**
        * The command handler.
        * The Bio Control should only handle its own commands that it has set
        * using the function SetMenuCommandSetL().
        * @param aCommand ID of command to be handled.
        * @return If the command is handled, it returns ETrue, and vice versa
        */
        TBool HandleBioCommandL(TInt aCommand);

        /**
        * The application can get the option menu permissions using this
        * function.
        * @return The option menu permission flags. If the flag is off it
        * means that the option menu command is not recommended with this
        * Bio Control.
        */
        TUint32 OptionMenuPermissionsL() const;

        /**
        * Get the virtual height of the control's content.
        * @return virtual height in pixels.
        */
        TInt VirtualHeight();

        /**
        * Get the position of invisible cursor.
        * @return cursor position in pixels.
        */
        TInt VirtualVisibleTop();

    public: // Functions from CCoeControl

        /**
        * A CCoeControl virtual for handling key events.
        * @param aKeyEvent The key event.
        * @param aType TEventCode
        * @return EKeyWasConsumed or EKeyWasNotConsumed
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType );

        /**
         * Returns a bio control's a context sensitive help.
		 * @param aHelpContext Help context.
         */
        void GetHelpContext(TCoeHelpContext& aHelpContext) const;        
		
    public: // TCallBack

       /**
        * TCallBack function for image scaling.
        * @param aPtr this class instance
        * @return always zero
        */        
        static TInt ScalingReady( TAny* aPtr );				

    protected: // Functions from CCoeControl

        /**
        * Gives the number of sub controls.
        * @return count of controls be included in this component
        */
        TInt CountComponentControls() const;

        /**
        * Returns a pointer to a certain sub control.
        * @param aIndex Index for control
        * @return Pointer to component in question.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * This is called by the CONE framework, and gives this control a
        * chance to manage the layout of its sub controls.
        */
        void SizeChanged();

        /**
        * This is called when focus is lost or gained, and is used
        * for setting the focus of the list box.
        * @param aDrawNow
        */
        void FocusChanged( TDrawNow aDrawNow );

        /**
        * Sets the container window for this control. The container
        * control uses this function to set the same window for this control.
        * @param aContainer container
        */
        void SetContainerWindowL( const CCoeControl& aContainer );
        
        /**
         * Handles bitmap drawing.
         */    
        void Draw(const TRect& aRect) const;

    private: // New functions

       /**
        * Helper method which creates or rescales iScaledBitmap unless
        * image hasn't been loaded or conversion is currently ongoing.
        * @param aSize size for which to scale
        * @return ETrue if scaling was possible
        */
        TBool ScaleIfPossibleL( TSize aSize );

       /**
        * Callback method called when image scaling is ready.
        */
        void DoScalingReady();

       /**
        * Panic wrapper.
        * @param aCode panic code
        */
        void Panic(TInt aCode);        
        
    private: // Constructors

        /**
        * Constructor is prohibited.
        * @param aObserver Reference to the Bio control observer.
        * @param aSession Reference to Message Server session.
        * @param aId Id of the message in Message Server.
        * @param aEditorOrViewerMode control's mode
        */
        COperatorLogoBioControl( MMsgBioControlObserver& aObserver,
                              CMsvSession* aSession,
                              TMsvId aId,
                              TMsgBioMode aEditorOrViewerMode );


        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    private: // New functions

        /**
        * Reads operator logo file and detach embedded OTA-bitmap from it.
        * Bitmap will be saved to the temp file.
        * @param aReadStream Stream where to get headers off.
        */
        void DetachHeadersFromOpLogoL( RFileReadStream& aReadStream );
        
        /**
        * Decode MCC and MNC information and set corresponding members.
        * @param aFmcc first part of mobile country code
        * @param aSmcc second part of mobile country code
        * @param aMnc mobile network code
        */
        void SetMccAndMnc( TUint8 aFmcc, TUint8 aSmcc, TUint8 aMnc );

        /**
        * Write bitmap to temporary file from the stream
        * @param aReadStream stream where to write
        */
        void WriteBitmapToTempFileL( RFileReadStream& aReadStream );

        /**
        * Try to read operator logo to stream from the file,
        * @param aReadStream Read logo in it.
        */
        void OpenOperatorLogoL( RFileReadStream& aReadStream );

        /**
        * Creates bitmap
        */
        void CreateBitmapL() ;

        /**
        * Try to save logo.
        */
        void TrySaveBitmapL();

#ifdef RD_PHONE_CLIENT_EXT        
        /**
        * Checks if oplogo already exists in phone server.
        * @param aImageHandler reference
        */
        TBool LogoExistsL( 	CPhCltImageHandler& aImageHandler,
        					CPhCltImageParams* aLogoParams );
#else        					
        /**
        * Checks if oplogo already exists in phone server.
        * @param aImageHandler reference
        */
        TBool LogoExistsL( 	CPhCltExtImageHandler& aImageHandler,
        					CPhCltExtImageParams* aLogoParams );
#endif

        /**
        * Detaches and returns the size of temporary bitmap
        * @return the size of bitmap
        */
        TSize DetachSizeFromTempBitmapL();

        /**
        * Calculates cropper rectangle
        * @aparam aSize, the original size of bitmap
        * @return calculated rect
        */
        TRect CalculateCropRect( TSize aSize );

        /**
        * Opens and reads the file to descriptor for CheckMsgFormatL.
        * @return TBool is message valid.
        */
        TBool CheckMsgValidityL();

        /**
        * Checks the format of message data and returns the result as TBool.
        * @return TBool is the message valid or not.
        */
        TBool CheckMsgFormatL( RFile aFile, TPtr8 aPtr );

        /**
        * Gets MCC and MNC from ETel and compares those against
        * message's MCC and MNC return are those MCCs' and MNCs' same.
        */
        TBool IsClearingMessageL();
               
        /**
        * Resets bitmaps, scaler.
        */        
        void Reset();        

#ifndef RD_PHONE_CLIENT_EXT
        /**
        * Load phone server client library so that oplogo can be saved. 
        */        
        void LoadLibraryLC();
#endif

    private: // Hidden away

        /**
        * C++ default constructor, hidden away from outsiders.
        */
        COperatorLogoBioControl();

        /**
        * Copy-constructor is prohibited.
        */
        COperatorLogoBioControl( const COperatorLogoBioControl& aSource );

        /**
        * Assignment operator is prohibited.
        */
        const COperatorLogoBioControl& operator=(
            const COperatorLogoBioControl& aSource );

    private: //Data

        // Connection to phone server for saving oplogo
        RPhCltServer iPhoneServer;

        /// Own: Active object which calls callback method when
        /// scaling has finished    
        COpLogoAOCallBack* iScalingAO;      
        
        /// Own: Bitmap, unscaled original
        CFbsBitmap* iBitmap;   
        
        /// Own: Bitmap, scaled for current layout size
        CFbsBitmap* iScaledBitmap;
        
        /// Own: Bitmap scaler
        MIHLScaler* iScaler;    

        ///Temporary file name and path
        TFileName iTempPathAndName;

        /// does the msg header contain linefeed.
        TBool iLineFeedExists;

        /// Mobile country code
        TInt iMcc;

        /// Mobile Network Code
        TInt iMnc;

#ifndef RD_PHONE_CLIENT_EXT        
        /// The library where the PhoneClient Extension is located.
        RLibrary    iControlDllLibrary;       
#endif
        
        // file server session handle
        RFs iFs; 
        
        // temp file has been successfully created
        TBool iTempFileExists;
    };

#endif // COPERATORLOGOCONTROL_H

//  End of File
