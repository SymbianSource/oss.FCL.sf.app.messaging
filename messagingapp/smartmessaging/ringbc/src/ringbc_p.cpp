/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *     A class which takes care of reading the possible new
 *     service centres from SIM and adds them to pda-side Sms Settings.
 *
 */

// SYSTEM INCLUDES
#include <pathinfo.h>
#include <f32file.h>
#include <hbmessagebox.h>

// USER INCLUDES
#include "ringbc_p.h"
#include "ringbctoneconverter.h"
#include "s60qconversions.h"
#include "debugtraces.h"

//CONSTANTS 
_LIT(KRingingToneFileExtension,".rng");


// ----------------------------------------------------------------------------
// RingBcPrivate::RingBcPrivate
// @see ringbc_p.h
// ----------------------------------------------------------------------------
RingBcPrivate::RingBcPrivate()

    {
    QDEBUG_WRITE("RingBcPrivate::RingBcPrivate : Enter")

    TRAP_IGNORE(initL());

    QDEBUG_WRITE("RingBcPrivate::RingBcPrivate : Exit")

    }

// ----------------------------------------------------------------------------
// RingBcPrivate::~RingBcPrivate
// @see ringbc_p.h
// ----------------------------------------------------------------------------
RingBcPrivate::~RingBcPrivate()
    {
    QDEBUG_WRITE("RingBcPrivate::~RingBcPrivate : Enter")
    
    delete mConverter;
    
    QDEBUG_WRITE("RingBcPrivate::~RingBcPrivate : Exit")
    }

// ----------------------------------------------------------------------------
// RingBcPrivate::initL
// @see ringbc_p.h
// ----------------------------------------------------------------------------
void RingBcPrivate::initL()
    {
    QDEBUG_WRITE("RingBcPrivate::initL : Enter")
    
    mConverter = RingBCNSMConverter::NewL();
    
    QDEBUG_WRITE("RingBcPrivate::initL : Exit")
    }

// ----------------------------------------------------------------------------
// RingBcPrivate::saveTone
// @see ringbc_p.h
// ----------------------------------------------------------------------------
void RingBcPrivate::saveTone(const QString &path)
    {
    QDEBUG_WRITE("RingBcPrivate::saveTone : Enter")
    
    int error(KErrNone);
    HBufC* fileName = S60QConversions::qStringToS60Desc(path);
    TRAP(error, error = saveToneL(*fileName));
    if(error)
        {
        QDEBUG_WRITE_FORMAT("RingBcPrivate::saveTone Error code =",error)
        
        if(error == KErrCorrupt)
            {
            HbMessageBox::information("Ringing Tone Corrupted");
            QDEBUG_WRITE("RingBcPrivate::saveTone : Ringing tone corrupted")
            }
        else if(error == KErrNoMemory || error == KErrDiskFull)
            {
            HbMessageBox::information("No memory to save");
            QDEBUG_WRITE("RingBcPrivate::saveTone : Low memory")
            }
        else
            {
            HbMessageBox::information("Error in Saving");
            QDEBUG_WRITE("RingBcPrivate::saveTone : Error in Saving")
            }
        }
    else
        {
        HbMessageBox::information("Saved succesfully");
        QDEBUG_WRITE("RingBcPrivate::saveTone : Ringing tone saved successfully")
        }
        
    delete fileName;
    
    QDEBUG_WRITE("RingBcPrivate::saveTone : Exit")
    }
    
// ----------------------------------------------------------------------------
// RingBcPrivate::saveToneL
// @see ringbc_p.h
// ----------------------------------------------------------------------------
int  RingBcPrivate::saveToneL(const TDesC& aFileName)
    {
    QDEBUG_WRITE("RingBcPrivate::saveToneL : Enter")
    
    RFs fsSession;
    TInt error = fsSession.Connect();
    if( error == KErrNone)
        {
        CleanupClosePushL(fsSession);

        RFile file;
        error = file.Open(fsSession,aFileName, EFileRead);

        if(error == KErrNone)
            {
            CleanupClosePushL(file);
            TInt size;
            User::LeaveIfError(file.Size(size));
            if (size)
                {
                HBufC8* dataBuf = HBufC8::NewLC(size);
                TPtr8 data(dataBuf->Des());
                User::LeaveIfError(file.Read(data, size));             
             
                TBool valid = mConverter->IsRingToneMimeTypeL(data);
                if(valid)
                    {
                    HBufC* title = mConverter->TitleLC(data);
                    TFileName path = PathInfo::PhoneMemoryRootPath();
                    path.Append( PathInfo::SimpleSoundsPath() );
                    path.Append(*title);
                    path.Append(KRingingToneFileExtension);

                    CFileMan* fileMan = CFileMan::NewL(fsSession);
                    CleanupStack::PushL( fileMan );
                    error = fileMan->Copy( file, path );
                    CleanupStack::PopAndDestroy(2); // title,fileMan        
                    }
                else 
                    {
                    error = KErrCorrupt;
                    }
                CleanupStack::PopAndDestroy(); // dataBuf      
                }   
            CleanupStack::PopAndDestroy(); //file
            }
        CleanupStack::PopAndDestroy(); //fsSession
        }
    
    QDEBUG_WRITE("RingBcPrivate::saveToneL : Exit")
    return error;
    }

// ----------------------------------------------------------------------------
// RingBcPrivate::toneTitle
// @see ringbc_p.h
// ----------------------------------------------------------------------------
 QString RingBcPrivate::toneTitle(const QString &path)
     {
     QDEBUG_WRITE("RingBcPrivate::toneTitle : Enter")
     
     QString title; 
     HBufC* fileName = S60QConversions::qStringToS60Desc(path);
     TRAP_IGNORE( title = titleL(*fileName));
     
     QDEBUG_WRITE("RingBcPrivate::toneTitle : Exit")
     return title;         
     }

 // ----------------------------------------------------------------------------
 // RingBcPrivate::titleL
 // @see ringbc_p.h
 // ----------------------------------------------------------------------------
 QString RingBcPrivate::titleL(const TDesC& aFileName)
     {
     QDEBUG_WRITE("RingBcPrivate::titleL : Enter")
     
     QString title; // initialize to null string
     RFs fsSession;
     TInt error = fsSession.Connect();
     if( error == KErrNone)
         {
         CleanupClosePushL(fsSession);

         RFile file;
         error = file.Open(fsSession,aFileName, EFileRead);
        
         if(error == KErrNone)
             {
             CleanupClosePushL(file);
             TInt size;
             User::LeaveIfError(file.Size(size));
             if (size)
                 {
                 HBufC8* dataBuf = HBufC8::NewLC(size);
                 TPtr8 data(dataBuf->Des());
                 User::LeaveIfError(file.Read(data, size));             

                 TBool valid = mConverter->IsRingToneMimeTypeL(data);
                 if(valid)
                     {
                     HBufC* toneTitle = mConverter->TitleLC(data);
                     title = S60QConversions::s60DescToQString(*toneTitle);
                     CleanupStack::PopAndDestroy(); //title
                     }
                 CleanupStack::PopAndDestroy(); //dataBuf
                 }
             CleanupStack::PopAndDestroy(); //file
             }
         CleanupStack::PopAndDestroy(); //fsSession
         }
     QDEBUG_WRITE("RingBcPrivate::titleL : Exit")
     return title;           
     }
 
//  End of File  
