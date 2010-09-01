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
* Description:   Class offers helper methods for BIO controls
*
*/



#ifndef MSGBIOUTILS_H
#define MSGBIOUTILS_H

#include <e32std.h>

/**
* Class offers static helper methods for all the biocontrols.
*/
class MsgBioUtils
    {
    public:

        /**
         * Pops a CAknTextQueryDialog. It will leave with KLeaveWithoutAlert
         * if the user cancels the query.
         * Your resource must be loaded first.
         * @param aResource The query resource ID.
         * @param aText Reference to a descriptor which will get the input.
         * @exception Cancelling the query results in a leave.
         */
        IMPORT_C static void TextQueryL(TInt aQueryResource, TDes& aText);

        /**
         * An error note is popped.
         * The standard resource must have been loaded using
         * LoadStandardBioResourceL().
         * @param aText The error text.
         */
        IMPORT_C static void ErrorNoteL(const TDesC& aText);

        /**
         * An error note is popped.
         * The standard resource must have been loaded using
         * LoadStandardBioResourceL(). Your resource must also be loaded, for
         * eg. with LoadResourceL().
         * @param aStringResource The string resource id.
         */
        IMPORT_C static void ErrorNoteL(TInt aStringResource);

        /**
         * A confirmation note is popped.
         * @param aText The confirmation text.
         */
        IMPORT_C static void ConfirmationNoteL(const TDesC& aText);

        /**
         * A confirmation note is popped.
         * Your resource must be loaded, for eg. with LoadResourceL().
         * CCoeEnv must exist.
         * @param aStringResource The string resource id.
         */
        IMPORT_C static void ConfirmationNoteL(TInt aStringResource);

        /**
         * An information note is popped.
         * The standard resource must have been loaded using
         * LoadStandardBioResourceL().
         * @param aText The information text.
         */
        IMPORT_C static void InformationNoteL(const TDesC& aText);

        /**
         * An information note is popped.
         * The standard resource must have been loaded using
         * LoadStandardBioResourceL(). Your resource must also be loaded, for
         * eg. with LoadResourceL().
         * @param aStringResource The string resource id.
         */
        IMPORT_C static void InformationNoteL(TInt aStringResource);

        /**
         * This helper function increments the number at the end of the
         * string according to the standard way (Folders).
         * If the descriptors max length is not enough, text is clipped away
         * from the end before adding the number.
         * The max length is given as the second parameter
         * @param aText Reference to the descriptor which is to be modified.
         * @param aMaxLength Maximum length that this descriptor can grow to
         * @exception Panics if the descriptor length is zero.
         */
        IMPORT_C static void IncrementNameL(TDes& aText, TInt aMaxLength);

        /**
        * Checks the given ip address validity.
        * @param aIP String to be checked.
        * @return ETrue if the string is a valid IP address, EFalse otherwise.
        */
        IMPORT_C static TBool CheckIP( const TDesC& aIP );

        /**
        * Checks the given URL validity.
        * @param aUrl String to be checked.
        * @return ETrue if the string is a valid URL, EFalse otherwise.
        */
        IMPORT_C static TBool CheckURL( const TDesC& aUrl );
        
        /**
         * Pops a confirmation query. The result is given by the return value.
         * The msgeditorutils.rsc resource must be loaded, for eg. using
         * LoadStandardBioResourceL().
         * @param aText The text that is to be used in the query.
         * @return A user confirmation results in ETrue, and vice versa.
         */
        IMPORT_C static TBool ConfirmationQueryOkCancelL(const TDesC& aText);

        /**
         * Pops a confirmation query. The result is given by the return value.
         * The msgeditorutils.rsc resource must be loaded, for eg. using
         * LoadStandardBioResourceL().
         * @param aStringResource The string resource id.
         * @return A user confirmation results in ETrue, and vice versa.
         */
        IMPORT_C static TBool ConfirmationQueryOkCancelL(TInt aStringResource);

    private:

        /**
        * Get the tokenized value from the given string.
        * @param aIP string where the value is searched.
        * @param aDelimiter character, which separates the values.
        * @param aStartIndex defines the position from where the value is checked.
        * @param aValue the found value is converted to the numeric form 
        * and set to this value.
        * @return ETrue if tokenized value was found and conversion to the numeric value
        * was successfull.
        */
        static TBool GetTokenizedValue( const TDesC& aIP, 
            const TChar aDelimiter,
            TInt& aStartIndex, 
            TInt& aValue );

    private:
        friend class T_MsgEditorUtilsGeneralTests;
    };

#endif // MSGBIOCONTROL_H

// End of file
