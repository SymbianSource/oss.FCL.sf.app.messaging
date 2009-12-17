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
*     Mce delivery report view container control class implementation.
*
*/



#ifndef     __Mce_App_CMceDeliveryReportControlContainer_H__
#define     __Mce_App_CMceDeliveryReportControlContainer_H__

//  INCLUDES
#include <AknIconArray.h>
#include <coecntrl.h>               // CCoeControl

#include <aknlists.h>               // ListBox

#include "MceDeliveryReportView.h"
#include "MMceLogEngObserver.h"

#include <bldvariant.hrh>
#include <msvapi.h>


//DATA TYPES
enum TMceDeliveryReportsArrayIndexes
    {
    EMceDeliverySmsInfo = 0,
    EMceDeliveryMmsInfo
    };


// FORWARD DECLARATIONS

class CAknTitlePane;
class CEikStatusPane;
class CAknContextPane;
class CMceSessionHolder;

// CLASS DECLARATION

/**
 * Mce delivery report view container control class implementation.
 */

class CMceDeliveryReportControlContainer :  public CCoeControl,
                                            public MCoeControlObserver,
                                            public MMceLogEngObserver
    {
    public:  // interface
        /**
         * Standard creation function.
         *
         * @param aMceDeliveryReportView pointer to the owner of this object.
         * @return pointer to CMceDeliveryReportControlContainer instance.
         */
        static CMceDeliveryReportControlContainer* NewL(
            CMceDeliveryReportView* aMceDeliveryReportView,
            CMceSessionHolder& aSessionHolder,
            const TRect& aRect );

        /**
         * Destructor.
         */
        ~CMceDeliveryReportControlContainer();

    private:
        /**
         * Constructor.
         *
         * @param aMceDeliveryReportView pointer to the owner of this object.
         */
        CMceDeliveryReportControlContainer( CMceDeliveryReportView* aMceDeliveryReportView,
        									CMceSessionHolder& aSessionHolder );

        /**
         * Second phase constructor.
         */
        void ConstructL( const TRect& aRect );

    public:
        /**
         * Returns ListBox control.
         *
         * @return Pointer to listbox control.
         */
        CAknDoubleGraphicStyleListBox* ListBox() const;

        /**
         * Gets from the Shared Data the Current Call Meter value
         *
         * @return error code
         */
        TInt LogDisabledL();
        /**
         * Show 3.0 second long confirmation note to the user.
         */
        void PopUpNoteL();
        /**
         *  Make the empty listbox text from the given resource text
         *
         *  @param  aListBox      The listbox pointer on where to make the text
         *  @param  aResourceText The resource text
         */
        void MakeEmptyTextListBoxL( CEikFormattedCellListBoxTypedef* aListBox,
            TInt aResourceText );        

        /**
        * From CAknAppUi
        */
        void HandleResourceChange( TInt aType );


    protected:  // from CCoeControl
        /**
         * Handles this application view's command keys. Forwards other keys to
         * child control(s).
         *
         * @return EKeyWasConsumed if the keyboard event was consumed or
         *         EKeyWasNotConsumed otherwise.
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
         * Returns the count of child controls.
         *
         * @return number of the subcontrols.
         */
        TInt CountComponentControls() const;

        /**
         * Returns the child controls at aIndex.
         *
         * @param aIndex index of the control
         *
         * @return Sub control from the aIndex.
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
         *  Called when control's size changed.
         */
        void SizeChanged();

         /**
         *  Draw the control's contents
         *
         *  @param aRect    Drawable area
         */
        void Draw( const TRect& aRect ) const;
        
        
        /**
        * From CCoeControl
        */
        void FocusChanged(TDrawNow aDrawNow);
    
        

    protected:  // MCoeControlObserver
        /**
         * Handles event from observed control.
         *
         * @param aControl   The control that sent the event.
         * @param aEventType The event type.
         */
        void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

    private:  // operations 

        /**
         *  Append icon to icon array.
         *
         *  @param aId                    Icon id
         *  @param aIcons                 Icon array
         *  @param aIconFileWithPath      Icon file name with path
         *  @param aIconGraphicsIndex     Picture index.
         *  @param aIconGraphicsMaskIndex Mask index.
         */
        void AddIconL( TAknsItemID aId, CAknIconArray* aIcons,
                       const TDesC& aIconFileWithPath,
                       TInt aIconGraphicsIndex,
                       TInt aIconGraphicsMaskIndex );

    private:  // Commands



        /**
         * Change bitmaps when skin changes.
         */
        void SkinBitmapChangesL( );

    public:   // From MMceLogEngObserver
        /**
         * Application's log engine calls this method when the listbox state has changed
         */
        void MceLogEngineListBoxStateHasChangedL();

        /**
        * From CCoeControl       
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;

    private:  // data
        // Own: ListBox control
        CAknDoubleGraphicStyleListBox* iListBox;
        /// Ref: Pointer to the detail view.
        CMceDeliveryReportView* iMceDeliveryReportView;

        // security data caging
        TFileName           iFilenameMuiu;
        TFileName           iFilenameLog;
        CMceSessionHolder&	iSessionHolder;
    };

#endif  // __Mce_App_CMceDeliveryReportControlContainer_H__

// End of File
