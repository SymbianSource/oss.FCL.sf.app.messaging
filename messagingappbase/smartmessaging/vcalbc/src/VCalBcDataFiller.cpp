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




// INCLUDE FILES
#include "VCalBcDataFiller.h"       // CVCalBcDataFiller
#include "VCalBcPanics.h"           // panic codes

#include <calentry.h>
#include <calalarm.h>
#include <StringLoader.h>           // StringLoader
#include <vcalbc.rsg>               // resouce identifiers
#include <avkon.rsg>                // resouce identifiers
#include <CRichBio.h>               // CRichBio

// CONSTANTS

const TInt KMaxDateTimeTextLength = 100;
const TInt KRepeatIntervalBiWeekly = 2;

//  MEMBER FUNCTIONS

CVCalBcDataFiller::~CVCalBcDataFiller()
    {
    delete iDateFormat;
    delete iTimeFormat;
    delete iSummaryDescr;
    }

void CVCalBcDataFiller::FillViewerWithDataL(
    CRichBio& aViewer,
    CCalEntry& aEntry,
    CCoeEnv& aCoeEnv)
    {
    // An instance is used because otherwise we would have to pass around many
    // arguments.
    CVCalBcDataFiller* filler = CVCalBcDataFiller::NewLC(aEntry, aCoeEnv);
    filler->FillViewerWithDataL(aViewer);
    CleanupStack::PopAndDestroy(filler);
    }

void CVCalBcDataFiller::FillViewerWithDataL(CRichBio& aViewer)
    {
    __ASSERT_DEBUG(&aViewer != NULL, Panic(ENullViewer));

	TTime startTime(Time::NullTTime());
	//we want to trap KErrArgument if time is not set
	TRAP_IGNORE((startTime = iEntry.StartTimeL().TimeLocalL()));
	
	TTime endTime(Time::NullTTime());
	TRAP_IGNORE((endTime = iEntry.EndTimeL().TimeLocalL()));
	
	CCalEntry::TType type( iEntry.EntryTypeL() );
    if (type == CCalEntry::EAnniv)
        {
		AddAnniversarySpecificInfoL(aViewer, startTime);
        }
    else if (type == CCalEntry::EAppt)
        {
		AddMeetingSpecificInfoL(aViewer, startTime, endTime);
        }
    else if (type == CCalEntry::EEvent)
        {
		AddDaynoteSpecificInfoL(aViewer, startTime, endTime);
        }
    else if (type ==CCalEntry::ETodo)
    	{
    	AddVTodoSpecificInfoL(aViewer);
    	}
    else
        {
        // The class CCalenImporter is responsible for checking the Series 60
        // compatibility of the entry. It is used before coming here.
        __ASSERT_ALWAYS(EFalse, Panic(EUnsupportedEntryType));
        return;
        }

    if (type != CCalEntry::ETodo)
    	{
    	AddConfidentialityInfoL(aViewer);
    	}
    
    //the description field is supported by all entry types
	AddDescriptionInfoL( aViewer );
    }

void CVCalBcDataFiller::AddAnniversarySpecificInfoL(
    CRichBio& aViewer,
    const TTime& aStartTime)
    {
    AddItemL(aViewer, R_QTN_SM_CAL_TYPELABEL, R_QTN_SM_CAL_TYPE_ANNIV);
    AddItemL(aViewer, R_QTN_SM_CAL_ANNIVERS_SUBJECT, *iSummaryDescr);
    AddItemAsDateL(aViewer, R_QTN_SM_CAL_ANNIVERS_DATE, aStartTime);
    AddAlarmInfoL(aViewer);
    }

void CVCalBcDataFiller::AddMeetingSpecificInfoL(
    CRichBio& aViewer,
    const TTime& aStartTime,
    const TTime& aEndTime)
    {
    AddItemL(aViewer, R_QTN_SM_CAL_TYPELABEL, R_QTN_SM_CAL_TYPE_MEETING);
    AddItemL(aViewer, R_QTN_SM_CAL_MEETING_SUBJECT, *iSummaryDescr);
    if (iEntry.LocationL().Length() > 0)
        {
        AddItemL(aViewer, R_QTN_SM_CAL_MEETING_LOCATION, iEntry.LocationL());
        }
    AddItemAsDateL(aViewer, R_QTN_SM_CAL_STARTDATE, aStartTime);
    AddItemAsTimeL(aViewer, R_QTN_SM_CAL_MEETING_START_TIME, aStartTime);
    AddItemAsDateL(aViewer, R_QTN_SM_CAL_ENDDATE, aEndTime);
    AddItemAsTimeL(aViewer, R_QTN_SM_CAL_MEETING_END_TIME, aEndTime);
    AddAlarmInfoL(aViewer);
    AddRepeatInfoL(aViewer);
    }

void CVCalBcDataFiller::AddDaynoteSpecificInfoL(
    CRichBio& aViewer,
    const TTime& aStartTime,
    const TTime& aEndTime)
    {
    AddItemL(aViewer, R_QTN_SM_CAL_TYPELABEL, R_QTN_SM_CAL_TYPE_DAYNOTE);
    AddItemL(aViewer, R_QTN_SM_CAL_DAY_NOTE_SUBJECT, *iSummaryDescr);
    
    TInt duration = aEndTime.DaysFrom(aStartTime).Int();
    
    if(duration > 0)
    	duration--;    
    TTimeIntervalDays offset(duration);
    
    TTime endTime(aStartTime);
    endTime += offset;
    AddItemAsDateL(aViewer, R_QTN_SM_CAL_STARTDATE, aStartTime);
    AddItemAsDateL(aViewer, R_QTN_SM_CAL_ENDDATE, endTime);
    AddRepeatInfoL(aViewer);
    }

void CVCalBcDataFiller::AddVTodoSpecificInfoL(CRichBio& aViewer)
	{
	AddVTodoTypeInfoL(aViewer);
	AddVTodoSubjectInfoL(aViewer);
	AddVTodoDueDateInfoL(aViewer);
	AddVTodoAlarmInfoL(aViewer);
	AddVTodoPriorityInfoL(aViewer);
	}

void CVCalBcDataFiller::AddAlarmInfoL(
    CRichBio& aViewer)
    {
    CCalAlarm* alarm = iEntry.AlarmL();
    if ( alarm )
        {
        CleanupStack::PushL( alarm );
   		TTime alarmTime(iEntry.StartTimeL().TimeLocalL());

        alarmTime -= alarm->TimeOffset();
        AddItemAsTimeL(aViewer, R_QTN_SM_CAL_EVENT_ALARM_TIME, alarmTime );
        AddItemAsDateL(aViewer, R_QTN_SM_CAL_EVENT_ALARM_DATE, alarmTime );
        CleanupStack::PopAndDestroy( alarm );
        }
    }


void CVCalBcDataFiller::AddRepeatInfoL(CRichBio& aViewer)
    {
    TCalRRule repeatRule;//( TCalRRule::EInvalid );
    if( !iEntry.GetRRuleL (repeatRule) )
        {
        return;
        }
    TInt valueRes = RepeatIndexValueTextRes( repeatRule );
    if( valueRes )
        {
        AddItemL(aViewer, R_QTN_SM_CAL_MEETING_REPEAT, valueRes);
        TTime endtime = repeatRule.Until().TimeLocalL();
        AddItemAsDateL( aViewer, R_QTN_SM_CAL_MEETING_REPEAT_UNTIL, endtime );
        }
    }


TInt CVCalBcDataFiller::RepeatIndexValueTextRes(TCalRRule aRepeatRule ) const
    {
    TInt res = 0;

    switch( aRepeatRule.Type() )
        {
        case TCalRRule::EInvalid:
            {
            // No repeat value
            break;
            }
        case TCalRRule::EDaily:
            {
            res = R_QTN_SM_CAL_MEETING_RPT_DAILY;
            break;
            }
        case TCalRRule::EWeekly:
            {
            if( aRepeatRule.Interval() == KRepeatIntervalBiWeekly )
                {
                res = R_QTN_SM_CAL_MEETING_RPT_BIWEEKLY;
                }
            else
                {
                res = R_QTN_SM_CAL_MEETING_RPT_WEEKLY;
                }
            break;
            }
        case TCalRRule::EMonthly:
            {
            res = R_QTN_SM_CAL_MEETING_RPT_MONTHLY;
            break;
            }
        case TCalRRule::EYearly:
            {
            res = R_QTN_SM_CAL_MEETING_RPT_YEARLY;
            break;
            }
        default:
            {
            // Unexpected repeat index value
            __ASSERT_DEBUG(res, Panic(EVCalBCPanicUndefinedRepeatIndex));
            res = R_QTN_SM_CAL_RPT_OTHER;
            break;
            }
        }
    return res;
    }


void CVCalBcDataFiller::AddConfidentialityInfoL(CRichBio& aViewer)
    {
    CCalEntry::TReplicationStatus status = iEntry.ReplicationStatusL();
//    TAgnReplicationData::TStatus status = replicationData.Status();
    TInt valueRes = 0;
    if (status == CCalEntry::EOpen)
        {
        valueRes = R_QTN_SM_CAL_SYNC_PUBLIC;
        }
    else if (status == CCalEntry::EPrivate)
        {
        valueRes = R_QTN_SM_CAL_SYNC_PRIVATE;
        }
    else if (status == CCalEntry::ERestricted)
        {
        valueRes = R_QTN_SM_CAL_SYNC_NO_SYNC;
        }
    __ASSERT_DEBUG(valueRes, Panic(EInvalidConfid));
    AddItemL(aViewer, R_QTN_SM_CAL_SYNCHRONISATION, valueRes);
    }


void CVCalBcDataFiller::AddDescriptionInfoL(CRichBio& aViewer)
    {
	const TDesC& descData = iEntry.DescriptionL();
	if ( descData.Length() != 0 )
		{
	    AddItemL(aViewer, R_QTN_SM_CAL_MEETING_DESCRIPTION, descData);
		}
    }


HBufC* CVCalBcDataFiller::TextContentsFromEntryL(CCalEntry& aEntry)
    {
    HBufC* text = aEntry.SummaryL().AllocL();
    /*TPtr textPtr(text->Des());
    // Copy the text from richtext to descriptor:
    aEntry.RichTextL()->Extract(textPtr, 0,
        aEntry.RichTextL()->DocumentLength());
    CleanupStack::Pop(text);*/
    return text;
    }

void CVCalBcDataFiller::AddItemL(CRichBio& aViewer, TInt aLabelRes,
    const TDesC& aValue)
    {
    // Empty fields are not shown.
    if (aValue.Length())
        {
        aViewer.AddItemL(*StringLoader::LoadLC(aLabelRes, &iCoeEnv), aValue);
        CleanupStack::PopAndDestroy(); // (label text)
        }
    }

void CVCalBcDataFiller::AddItemL(CRichBio& aViewer, TInt aLabelRes,
    TInt aValueRes)
    {
    aViewer.AddItemL(
        *StringLoader::LoadLC(aLabelRes, &iCoeEnv),
        *StringLoader::LoadLC(aValueRes, &iCoeEnv));
    CleanupStack::PopAndDestroy(2); // (label and value text)
    }

void CVCalBcDataFiller::AddItemAsDateL(CRichBio& aViewer,
    TInt aLabelRes,
    const TTime& aTime)
    {
    if(aTime != Time::NullTTime())
    	{
    	TBuf<KMaxDateTimeTextLength> tmpString;
    	aTime.FormatL(tmpString, *iDateFormat);
    	AddItemL(aViewer, aLabelRes, tmpString);
    	}
    }

void CVCalBcDataFiller::AddItemAsTimeL(CRichBio& aViewer,
    TInt aLabelRes,
    const TTime& aTime)
    {
    if(aTime != Time::NullTTime())
    	{
    	TBuf<KMaxDateTimeTextLength> tmpString;
    	aTime.FormatL(tmpString, *iTimeFormat);
    	AddItemL(aViewer, aLabelRes, tmpString);
    	}
    }

CVCalBcDataFiller* CVCalBcDataFiller::NewLC(CCalEntry& aEntry, CCoeEnv& aCoeEnv)
    {
    __ASSERT_DEBUG(&aEntry != NULL, Panic(ENullEntryGivenToFiller));
    CVCalBcDataFiller* self = new (ELeave)CVCalBcDataFiller(aEntry, aCoeEnv);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CVCalBcDataFiller::CVCalBcDataFiller(CCalEntry& aEntry, CCoeEnv& aCoeEnv)
    : iCoeEnv(aCoeEnv), iEntry(aEntry)
    {
    }

void CVCalBcDataFiller::ConstructL()
    {
    iDateFormat = StringLoader::LoadL(R_QTN_DATE_USUAL_WITH_ZERO, &iCoeEnv);
    iTimeFormat = StringLoader::LoadL(R_QTN_TIME_USUAL, &iCoeEnv);
    //Get the text field contents. It is needed for all four types of entry.
    iSummaryDescr = TextContentsFromEntryL(iEntry);
    }

void CVCalBcDataFiller::AddVTodoTypeInfoL(CRichBio& aViewer)
	{
	AddItemL(aViewer, R_QTN_SM_CAL_TYPELABEL, R_QTN_SM_CAL_TYPE_TODO);
	}

void CVCalBcDataFiller::AddVTodoSubjectInfoL(CRichBio& aViewer)
	{
    AddItemL(aViewer, R_QTN_SM_TODO_SUBJECT, *iSummaryDescr);
	}

void CVCalBcDataFiller::AddVTodoDueDateInfoL(CRichBio& aViewer)
	{
	TTime dueDate(Time::NullTTime());
	TRAP_IGNORE((dueDate = iEntry.EndTimeL().TimeLocalL()));
	AddItemAsDateL(aViewer, R_QTN_SM_TODO_DUE_DATE, dueDate);
	}

void CVCalBcDataFiller::AddVTodoAlarmInfoL(CRichBio& aViewer)
	{
	CCalAlarm* alarm = iEntry.AlarmL();

	if( alarm )
		{
		CleanupStack::PushL(alarm);
		TTime alarmTime(iEntry.EndTimeL().TimeLocalL());

        alarmTime -= alarm->TimeOffset();
        AddItemAsTimeL(aViewer, R_QTN_SM_CAL_EVENT_ALARM_TIME, alarmTime );
        AddItemAsDateL(aViewer, R_QTN_SM_CAL_EVENT_ALARM_DATE, alarmTime );
        CleanupStack::PopAndDestroy(alarm);
		}


	}

void CVCalBcDataFiller::AddVTodoPriorityInfoL(CRichBio& aViewer)
	{
    TUint todoPriority = iEntry.PriorityL();

    TInt priorityString( 0 );

    if (todoPriority == 1 )
    	{
    	priorityString = R_QTN_SM_TODO_PRIORITY_HIGH;
    	}
    else if (todoPriority == 2)
    	{
    	priorityString = R_QTN_SM_TODO_PRIORITY_NORMAL;
    	}
    else if (todoPriority == 3)
    	{
    	priorityString = R_QTN_SM_TODO_PRIORITY_LOW;
    	}

    if( todoPriority >= 1 && todoPriority <= 3 )
    	AddItemL(aViewer, R_QTN_SM_TODO_PRIORITY, priorityString);

	}

// end of file

