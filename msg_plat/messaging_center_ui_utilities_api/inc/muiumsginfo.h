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
*     Encapsulates the data which populates the fields in the message 
*     info dialogs (email, sms, bios, mms), also holds the text for the 
*     labels and fields in the message info dialog and displays a message 
*     info dialog for particular message types
*
*/



#if !defined(__MUIU_MESSAGE_INFO_H__)
#define __MUIU_MESSAGE_INFO_H__


//  INCLUDES
#include <e32base.h>
#include <coneresloader.h>


// DATA TYPES
// struct which encapsulates the data which populates the fields
// in the message info dialogs (email, sms, bios, mms)
struct TMsgInfoMessageInfoData 
    {
    TPtrC iFrom;
    TPtrC iSubject;
    TPtrC iType;
    TPtrC iSize;
    TPtrC iTo;
    TPtrC iCC;
    TPtrC iBCC;
    TPtrC iCallback;
    TPtrC iPriority;
    TPtrC iStatus;
    TTime iDateTime;
    };



// FORWARD DECLARATIONS
class CEikonEnv;


// CLASS DECLARATION

/**
* Class which displays a message info dialog for particular message types
*/
class CMsgInfoMessageInfoDialog : public CBase
    {
    public:
        // different message info dialogs supported
        // Note: Editor = mobile originated (Info dialogs opened from Drafts & Sent Items)
        //       Viewer = mobile terminated
        enum TMessageInfoType
            {
            EEmailEditor,
            EEmailViewer,
            ESmsEditor,
            ESmsViewer,
            EBiosEditor,
            EBiosViewer,
            EMmsEditor,
            EMmsViewer
            };
    public:

        /**
        * Two-phased constructor.
        */  
        IMPORT_C static CMsgInfoMessageInfoDialog* NewL(); 

        /**
        * Destructor.
        */ 
        ~CMsgInfoMessageInfoDialog();

        /**
        * Show the message info dialog.
        */
        IMPORT_C void ExecuteLD( TMsgInfoMessageInfoData& aFieldData, TMessageInfoType aType );
    private:

        /**
        * 
        */
        CMsgInfoMessageInfoDialog();

        /**
        * Symbian OS constructor.
        */
        void ConstructL();

        /**
        * Construct the array of labels and text fields and add it to the listbox model.
        */
        void SetInfoArrayL( TMsgInfoMessageInfoData& aFieldData ) const;


        /**
        * Convert between arabic-indic digits and european digits based on existing language setting.
        * So it'll convert any digit from the string
        * to use either european digits or arabic-indic digits based on current settings.
        * @param aFieldString: Data buffer used in conversion.
        * @param aFieldData: Return converted data in this parameter.
        */
        void DoNumberConversion( HBufC* aFieldString, TPtrC& aFieldData ) const;

        /**
        * Creates formatted date and time strings and return these strings in parameters
        * aDateBuf and aTimeBuf. Function get UTC time in parameter aUTCtime and
        * constructs date and time strings with this UTC time value.
        * @param aUTCtime: UTC time used in time conversion.
        * @param aDateBuf: Return date string after time conversion.
        * @param aTimeBuf: Return time string after time conversion.
        * 
        */
        void GetFormattedTimeAndDateStringsL( TTime& aUTCtime, 
                                              TDes& aDateBuf, 
                                              TDes& aTimeBuf, 
                                              const CEikonEnv* aEikonEnv ) const;


        /**
        * Construct label string and calculate how much space label string and field string and 
        * extra marks need and add this needed space to "aNeededSpace" parameter.
        * @param aLabelResourceId: Resource id for Label
        * @param aFieldText: Field text string.
        * @param aNeededSpace: Space which total buffer need.
        * @return Heap descriptor containing label string.
        */
        HBufC* ConstructLabelStrAndCalculateNeededSpaceLC( TInt aLabelResourceId, 
                                                           const TPtrC& aFieldText,
                                                           TInt& aNeededSpace, 
                                                           const CEikonEnv* aEikonEnv ) const;

        /**
        * Add label string and fiels string to buffer string aQueryDialogBuf.
        * This buffer string is shown in query dialog after all label and filed strings
        * are inserted to buffer string.
        * @param aQueryDialogBuf: Buffer string which is shown in query dialog.
        * @param aLabelText: Label string which is inserted to aQueryDialogBuf buffer string.
        * @param aFieldText: Field string which is inserted to aQueryDialogBuf buffer string.
        * @param aLastItem:  Is true if item is first in list.
        */
        void AddToqueryDialogBuf( HBufC* aQueryDialogBuf,
                                  const TDesC& aLabelText,
                                  const TDesC& aFieldText,
                                  TBool aFirstItem ) const;
                                  
        /**
        * Message info may get phone names and numbers in string which is in form:
        * Timo33 Paajanen44<0505877430>;Simo55 Paajanen66<0505866430> and so on...
        * It must be possible to show this string so that only phone numbers
        * get language spesific number conversion and number groupping.
        * Name parts is displayed as they are in string.
        * So in this example Timo33 Paajanen44 and Simo55 Paajanen66 is shown without
        * lanquage spesific number conversion. Phone numbers 0505877430 and 0505866430
        * are number grouped and displayed after lanquage spesific number conversion.
        * This method handles parameter string aFieldString this vay and return heap descriptor
        * which contain handled string.
        * @param aFieldString: Field text string.
        * @return Heap descriptor containing handled string.
        */                               
        HBufC* GroupPhoneNumberAndDoLangSpecificNumberConversionL( const TDesC& aFieldString );
        
        
        /**
        * Set HBufC string to pointer descriptor
        * @param aFieldString: HBufC string.
        * @param aFieldData: Return pointer descriptor data in this parameter.
        */
        void SetStringToPointerDescriptor( HBufC* aFieldString, TPtrC& aFieldData ) const;

    private:  
        TMessageInfoType    iMessageType;
        RConeResourceLoader iResources;
        TBool               iPriorityFeatureSupported;
    };

#endif  //__MUIU_MESSAGE_INFO_H__

// End of file
