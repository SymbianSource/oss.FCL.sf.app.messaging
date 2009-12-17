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
*     A helper class for filling the viewer component with data from a
*     calendar item.
*
*/



#ifndef __CVCALBCDATAFILLER_H_
#define __CVCALBCDATAFILLER_H_

// INCLUDES

#include <e32base.h>
#include <calrrule.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CCalEntry;
class CRichBio;
class CCoeEnv;

// CLASS DECLARATION

/**
 * A helper class for filling the viewer component with data from a
 * calendar item.
 */
NONSHARABLE_CLASS( CVCalBcDataFiller ) : public CBase
    {
    public: // destruction

        /// destructor
        ~CVCalBcDataFiller();

    public: // new functions

        /**
         * Fills the viewer with data from the calendar item.
         * The parameter would have been const, but the Agenda API has some
         * functions which are not const for some reason.
         * @param aViewer Reference to the viewer.
         * @param aEntry Reference to the agenda entry
         * @param aCoeEnv Reference to CCoeEnv
         */
        static void FillViewerWithDataL(
            CRichBio& aViewer,
            CCalEntry& aEntry,
            CCoeEnv& aCoeEnv);

    private:

        /**
         * Fills the viewer with data from the calendar item.
         * The parameter would have been const, but the Agenda API has some
         * functions which are not const for some reason.
         * @param aViewer Reference to the viewer.
         */
        void FillViewerWithDataL(CRichBio& aViewer);

        /**
         * Adds anniversary specific info to the viewer.
         * @param aViewer Reference to the viewer.
         * @param aStartTime The start time from the entry.
         */
        void AddAnniversarySpecificInfoL(
            CRichBio& aViewer,
            const TTime& aStartTime);

        /**
         * Adds meeting specific info to the viewer.
         * @param aViewer Reference to the viewer.
         * @param aStartTime The start time from the entry.
         * @param aEndTime The start time from the entry.
         */
        void AddMeetingSpecificInfoL(
            CRichBio& aViewer,
            const TTime& aStartTime,
            const TTime& aEndTime);

        /**
         * Adds daynote specific info to the viewer.
         * @param aViewer Reference to the viewer.
         * @param aStartTime The start time from the entry.
         * @param aEndTime The start time from the entry.
         */
        void AddDaynoteSpecificInfoL(
            CRichBio& aViewer,
            const TTime& aStartTime,
            const TTime& aEndTime);

        /**
         * Adds vTodo specific info to the viewer.
         * @since Series S60 3.0
         * @param aViewer Reference to the viewer.
         * @return void
         */
        void AddVTodoSpecificInfoL(
        	CRichBio& aViewer);

        /**
         * Adds alarm informatin to the CRichBio viewer.
         * @param aViewer Reference to the viewer component.
         */
        void AddAlarmInfoL(
            CRichBio& aViewer);

        /**
         * Adds alarm repeat information to the CRichBio viewer
         * if text resource for the repeat value exists.
         * @param aViewer Reference to the viewer component.
         */
        void AddRepeatInfoL(CRichBio& aViewer);

        /**
         * Gives the text resource ID for a certain repeat index type.
         * @param aType Repeat index type.
         * @return Repeat value text resource.
         */
        TInt RepeatIndexValueTextRes(TCalRRule aRepeatRule ) const;

        /**
         * Adds confidentiality info.
         * @param aViewer Reference to the viewer component.
         */
        void AddConfidentialityInfoL(CRichBio& aViewer);

        /**
         * Adds calendar entry description info.
         * @param aViewer Reference to the viewer component.
         */
		void AddDescriptionInfoL( CRichBio& aViewer );

        /**
         * Returns the rich text content from the entry as a HBufC.
         * @param aEntry Reference to the entry.
         * @return The text content.
         */
        HBufC* TextContentsFromEntryL(CCalEntry& aEntry);

        /**
         * Helper for adding to viewer.
         * @param aViewer Reference to the viewer control
         * @param aLabelRes Resource id for label
         * @param aValue Value text
         */
        void AddItemL(CRichBio& aViewer,
            TInt aLabelRes, const TDesC& aValue);

        /**
         * Helper for adding to viewer.
         * @param aViewer Reference to the viewer control
         * @param aLabelRes Resource id for label
         * @param aValueRes Resource id for value text
         */
        void AddItemL(CRichBio& aViewer,
            TInt aLabelRes, TInt aValueRes);

        /**
         * Adds a label and date to the viewer.
         * @param aViewer Reference to the CRichBio viewer.
         * @param aLabelRes Label text resource ID.
         * @param aTime TTime.
         */
        void AddItemAsDateL(CRichBio& aViewer,
            TInt aLabelRes,
            const TTime& aTime);

        /**
         * Adds a label and time to the viewer.
         * @param aViewer Reference to the CRichBio viewer.
         * @param aLabelRes Label text resource ID.
         * @param aTime TTime.
         */
        void AddItemAsTimeL(CRichBio& aViewer,
            TInt aLabelRes,
            const TTime& aTime);

       	/**
        * Adds a label and type to the viewer for vTodo entry
        * @since Series S60 3.0
        * @param aViewer Reference to the CRichBio viewer.
        * @return void
        */
		void AddVTodoTypeInfoL(CRichBio& aViewer);

       	/**
        * Adds a label and subject to the viewer for vTodo entry
        * @since Series S60 3.0
        * @param aViewer Reference to the CRichBio viewer.
        * @return void
        */
		void AddVTodoSubjectInfoL(CRichBio& aViewer);

       	/**
        * Adds a label and duedate to the viewer for vTodo entry
        * @since Series S60 3.0
        * @param aViewer Reference to the CRichBio viewer.
        * @return void
        */
		void AddVTodoDueDateInfoL(CRichBio& aViewer);

       	/**
        * Adds a label and alarminfo to the viewer for vTodo entry
        * @since Series S60 3.0
        * @param aViewer Reference to the CRichBio viewer.
        * @return void
        */
		void AddVTodoAlarmInfoL(CRichBio& aViewer);

       	/**
        * Adds a label and priority to the viewer for vTodo entry
        * @since Series S60 3.0
        * @param aViewer Reference to the CRichBio viewer.
        * @return void
        */
		void AddVTodoPriorityInfoL(CRichBio& aViewer);

    private: // construction

        /**
         * Two phase constructor.
         * @param aEntry Reference to the agenda entry
         * @param aCoeEnv Reference to CCoeEnv
         */
        static CVCalBcDataFiller* NewLC(CCalEntry& aEntry, CCoeEnv& aCoeEnv);

        /**
         * Constructor
         * @param aEntry Reference to the agenda entry.
         */
        CVCalBcDataFiller(CCalEntry& aEntry, CCoeEnv& aCoeEnv);

        /// second phase constructor
        void ConstructL();

    private: //prohibited

        /// default constructor prohibited
        CVCalBcDataFiller();

        /// Copy contructor prohibited.
        CVCalBcDataFiller(const CVCalBcDataFiller& aSource);

        /// Assignment operator prohibited.
        const CVCalBcDataFiller& operator=(const CVCalBcDataFiller& aSource);

    private: // data

        CCoeEnv& iCoeEnv;
        CCalEntry& iEntry;
        HBufC* iDateFormat;
        HBufC* iTimeFormat;
        HBufC* iSummaryDescr;
    };

#endif //__CVCALBCDATAFILLER_H_

//end of file
