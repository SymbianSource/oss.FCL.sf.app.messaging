/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: smiltextbuf  declaration
*
*/



#ifndef SMILTEXTBUF_H
#define SMILTEXTBUF_H

#include <e32base.h>

class RSmilTextBuf
	{
public:
	inline TInt MaxLength() const;
	inline TPtr& Text();
	inline const TPtr& Text() const;
	inline void SetTextL(const TDesC &aDes);

	RSmilTextBuf()
		: iPtr(NULL,0), iText(NULL)
		{}
	~RSmilTextBuf()
		{
		delete [] iText;
		}

private:
	TPtr iPtr;
	TText* iText;
	};


inline TPtr& RSmilTextBuf::Text()
	{return(iPtr);}
inline const TPtr& RSmilTextBuf::Text() const
	{return(iPtr);}
inline TInt RSmilTextBuf::MaxLength() const
	{return(iPtr.MaxLength());}
inline void RSmilTextBuf::SetTextL(const TDesC &aDes)
	{
	if (MaxLength()<aDes.Length())
		{
		delete [] iText;
		iText=NULL;
		iPtr.Set(NULL,0,0);
		iText=new TText[aDes.Length()+1];
		iPtr.Set(iText,0,aDes.Length());
		}
	iPtr.Copy(aDes);
	}

#endif
