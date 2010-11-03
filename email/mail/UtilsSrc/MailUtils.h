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
* Description:  Static helper methods.
*
*/


#ifndef MAILUTILS_H
#define MAILUTILS_H

#include <e32std.h>

class RFile;
/**
* Class offers static helper methods.
* @since 2.o
*/
class MailUtils
    {
    public:
        enum TQueryState
            {
            EConnectionOffline = KBit0,
            EQueryAttachmentFetch = KBit1,
            EQueryForward = KBit2
            };
        /**
         * Pops a CAknTextQueryDialog. It will leave with KLeaveWithoutAlert
         * if the user cancels the query.
         * @param aQueryResource The query resource ID.
         * @param aText Reference to a descriptor which will get the input.
         * @exception Cancelling the query results in a leave.
         */
        IMPORT_C static void TextQueryL(TInt aQueryResource, TDes& aText);

        /**
         * A confirmation note is popped.
         * @param aText The confirmation text.
         */
        IMPORT_C static void ConfirmationNoteL(const TDesC& aText);

        /**
         * A confirmation note is popped.
         * @param aStringResource The string resource id.
         */
        IMPORT_C static void ConfirmationNoteL(TInt aStringResource);

        /**
         * An information note is popped.
         * @param aText The information text.
         */
        IMPORT_C static void InformationNoteL(const TDesC& aText);

        /**
         * An information note is popped.
         * @param aStringResource The string resource id.
         */
        IMPORT_C static void InformationNoteL(TInt aStringResource);

        /**
        * Show global error note if message store space is below
        * critical level.
        * @return ETrue if space below CL
        * 
        * !! Deprecated -> to be removed.  Leaves with KErrNotSupported !!
        */
        IMPORT_C static TBool ShowNoteIfMsgStoreSpaceLowL(const TInt aSize);
       
        /**
        * Check if off-line profile is active.
        * @return ETrue if off-line profile is active.
        */
        IMPORT_C static TBool OfflineProfileActiveL();

        /**
        * see CCommonContentPolicy::IsClosedType()
        */
        IMPORT_C static TBool IsClosedMimeL(const TDesC& aMimeType);
		IMPORT_C static TBool IsClosedMimeL(const TDesC8& aMimeType);
		IMPORT_C static TBool IsClosedFileL( RFile& aFileHandle );
		
        /**
         * An Confirmation query.
         * @param aText The information text.
         * @param aNoteResource Confirmation note resource
         */
        IMPORT_C static TBool ConfirmationQueryL(const TDesC& aText, 
            TInt aNoteResource);
        
        /**
         * An Confirmation query.
         * @param aStringResource The string resource id.
         * @param aNoteResource Confirmation note resource
         */
        IMPORT_C static TBool ConfirmationQueryL(TInt aStringResource, 
            TInt aNoteResource);
            
        /**
        * An Confirmation query is popped with Yes/No options.
        * @since Series 60 3.0
        * @param aSize size of the attachment.
        * @param aStatus Status flag. see TQueryState
        * @return ETrue if it is ok to fetch attachment.
        */          
        IMPORT_C static TBool LargeAttachmentQueryL(TInt aSize, 
            TInt aStatus);
        
        /**
        * *DEPRECATED*
        * OfflineProfileActiveWithNoteL.
        * @since Series 60 3.0
        * Leaves KErrNotSupported
        */    
		IMPORT_C static TBool OfflineProfileActiveWithNoteL();
		
        /**
        * An Confirmation query is popped with Yes/No options.
        * @since Series 60 3.0        
        * @param aText The information text.
        */
        IMPORT_C static TBool ConfirmationQueryL( const TDesC& aText );	
        
        /**
        * An Confirmation query is popped with Yes/No options.
        * @since Series 60 3.0        
        * @param aStringResource The information text resource id.
        */        
        IMPORT_C static TBool ConfirmationQueryL( TInt aStringResource );
        
        /**
        * Resolve file size.
        * @param aFilename name of the file
        * @return file size
        */
        IMPORT_C static TInt FileSize( const TDesC& aFilename );
    };

#endif // MAILUTILS_H

// End of file
