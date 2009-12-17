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
* Description: G_SmsStrict implementation
*
*/



// G_SmsStrict.cpp
//
// Copyright (c) 2003 Symbian Ltd.  All rights reserved.
//

#include <e32std.h>
#include <convdata.h>
#include <convgeneratedcpp.h>

#define ARRAY_LENGTH(aArray) (sizeof(aArray)/sizeof((aArray)[0]))

#pragma warning (disable: 4049) // compiler limit : terminating line number emission

_LIT8(KLit8ReplacementForUnconvertibleUnicodeCharacters, "\x3f");

GLDEF_C const TDesC8& ReplacementForUnconvertibleUnicodeCharacters_internal()
	{
	return KLit8ReplacementForUnconvertibleUnicodeCharacters;
	}

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SIndexedTable16::SEntry indexedTable16_foreignToUnicode_1[]=
	{
		{
		0x0040
		},
		{
		0x00a3
		},
		{
		0x0024
		},
		{
		0x00a5
		},
		{
		0x00e8
		},
		{
		0x00e9
		},
		{
		0x00f9
		},
		{
		0x00ec
		},
		{
		0x00f2
		},
		{
		0x00e7
		},
		{
		0x000a
		},
		{
		0x00d8
		},
		{
		0x00f8
		},
		{
		0x000d
		},
		{
		0x00c5
		},
		{
		0x00e5
		},
		{
		0x0394
		},
		{
		0x005f
		},
		{
		0x03a6
		},
		{
		0x0393
		},
		{
		0x039b
		},
		{
		0x03a9
		},
		{
		0x03a0
		},
		{
		0x03a8
		},
		{
		0x03a3
		},
		{
		0x0398
		},
		{
		0x039e
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SIndexedTable16::SEntry indexedTable16_foreignToUnicode_2[]=
	{
		{
		0x00c4
		},
		{
		0x00d6
		},
		{
		0x00d1
		},
		{
		0x00dc
		},
		{
		0x00a7
		},
		{
		0x00bf
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_foreignToUnicode_1[]=
	{
		{
		0x1c,
		0x00c6
		},
		{
		0x1d,
		0x00e6
		},
		{
		0x1e,
		0x00df
		},
		{
		0x1f,
		0x00c9
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_foreignToUnicode_2[]=
	{
		{
		0x7b,
		0x00e4
		},
		{
		0x7c,
		0x00f6
		},
		{
		0x7d,
		0x00f1
		},
		{
		0x7e,
		0x00fc
		},
		{
		0x7f,
		0x00e0
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_foreignToUnicode_3[]=
	{
		{
		0x1b28,
		0x007b
		},
		{
		0x1b29,
		0x007d
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_foreignToUnicode_4[]=
	{
		{
		0x1b3c,
		0x005b
		},
		{
		0x1b3d,
		0x007e
		},
		{
		0x1b3e,
		0x005d
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SIndexedTable16::SEntry indexedTable16_unicodeToForeign_1[]=
	{
		{
		0x7b
		},
		{
		0x0f
		},
		{
		0x1d
		},
		{
		0x09
		},
		{
		0x04
		},
		{
		0x05
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_unicodeToForeign_1[]=
	{
		{
		0x005b,
		0x1b3c
		},
		{
		0x005c,
		0x1b2f
		},
		{
		0x005d,
		0x1b3e
		},
		{
		0x005e,
		0x1b14
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_unicodeToForeign_2[]=
	{
		{
		0x007b,
		0x1b28
		},
		{
		0x007c,
		0x1b40
		},
		{
		0x007d,
		0x1b29
		},
		{
		0x007e,
		0x1b3d
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_unicodeToForeign_3[]=
	{
		{
		0x00a3,
		0x01
		},
		{
		0x00a4,
		0x24
		},
		{
		0x00a5,
		0x03
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_unicodeToForeign_4[]=
	{
		{
		0x00c4,
		0x5b
		},
		{
		0x00c5,
		0x0e
		},
		{
		0x00c6,
		0x1c
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_unicodeToForeign_5[]=
	{
		{
		0x00df,
		0x1e
		},
		{
		0x00e0,
		0x7f
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_unicodeToForeign_6[]=
	{
		{
		0x00f1,
		0x7d
		},
		{
		0x00f2,
		0x08
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_unicodeToForeign_7[]=
	{
		{
		0x00f8,
		0x0c
		},
		{
		0x00f9,
		0x06
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_unicodeToForeign_8[]=
	{
		{
		0x0393,
		0x13
		},
		{
		0x0394,
		0x10
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_unicodeToForeign_9[]=
	{
		{
		0x03a8,
		0x17
		},
		{
		0x03a9,
		0x15
		}
	};

LOCAL_D const SCnvConversionData::SVariableByteData::SRange foreignVariableByteDataRanges[]=
	{
		{
		0x00,
		0x1a,
		0,
		0
		},
		{
		0x1b,
		0x1b,
		1,
		0
		},
		{
		0x1c,
		0x7f,
		0,
		0
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange foreignToUnicodeDataRanges[]=
	{
		{
		0x00,
		0x1a,
		SCnvConversionData::SOneDirectionData::SRange::EIndexedTable16,
		0,
		0,
			{
			UData_SIndexedTable16(indexedTable16_foreignToUnicode_1)
			}
		},
		{
		0x1c,
		0x1f,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		0,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_foreignToUnicode_1)
			}
		},
		{
		0x20,
		0x23,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		0,
		0,
			{
			0
			}
		},
		{
		0x24,
		0x24,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		0,
		0,
			{
			STATIC_CAST(TUint, 128)
			}
		},
		{
		0x25,
		0x3f,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		0,
		0,
			{
			0
			}
		},
		{
		0x40,
		0x40,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		0,
		0,
			{
			STATIC_CAST(TUint, 97)
			}
		},
		{
		0x41,
		0x5a,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		0,
		0,
			{
			0
			}
		},
		{
		0x5b,
		0x60,
		SCnvConversionData::SOneDirectionData::SRange::EIndexedTable16,
		0,
		0,
			{
			UData_SIndexedTable16(indexedTable16_foreignToUnicode_2)
			}
		},
		{
		0x61,
		0x7a,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		0,
		0,
			{
			0
			}
		},
		{
		0x7b,
		0x7f,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		0,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_foreignToUnicode_2)
			}
		},
		{
		0x1b0a,
		0x1b0a,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		0,
		0,
			{
			STATIC_CAST(TUint, -6910)
			}
		},
		{
		0x1b14,
		0x1b14,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		0,
		0,
			{
			STATIC_CAST(TUint, -6838)
			}
		},
		{
		0x1b28,
		0x1b29,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		0,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_foreignToUnicode_3)
			}
		},
		{
		0x1b2f,
		0x1b2f,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		0,
		0,
			{
			STATIC_CAST(TUint, -6867)
			}
		},
		{
		0x1b3c,
		0x1b3e,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		0,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_foreignToUnicode_4)
			}
		},
		{
		0x1b40,
		0x1b40,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		0,
		0,
			{
			STATIC_CAST(TUint, -6852)
			}
		},
		{
		0x1b65,
		0x1b65,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		0,
		0,
			{
			STATIC_CAST(TUint, 1351)
			}
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange unicodeToForeignDataRanges[]=
	{
		{
		0x000a,
		0x000a,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		1,
		0,
			{
			0
			}
		},
		{
		0x000c,
		0x000c,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		2,
		0,
			{
			STATIC_CAST(TUint, 6910)
			}
		},
		{
		0x000d,
		0x000d,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		1,
		0,
			{
			0
			}
		},
		{
		0x0020,
		0x0023,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		1,
		0,
			{
			0
			}
		},
		{
		0x0024,
		0x0024,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -34)
			}
		},
		{
		0x0025,
		0x003f,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		1,
		0,
			{
			0
			}
		},
		{
		0x0040,
		0x0040,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -64)
			}
		},
		{
		0x0041,
		0x005a,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		1,
		0,
			{
			0
			}
		},
		{
		0x005b,
		0x005e,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		2,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_unicodeToForeign_1)
			}
		},
		{
		0x005f,
		0x005f,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -78)
			}
		},
		{
		0x0061,
		0x007a,
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		1,
		0,
			{
			0
			}
		},
		{
		0x007b,
		0x007e,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		2,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_unicodeToForeign_2)
			}
		},
		{
		0x00a1,
		0x00a1,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -97)
			}
		},
		{
		0x00a3,
		0x00a5,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		1,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_unicodeToForeign_3)
			}
		},
		{
		0x00a7,
		0x00a7,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -72)
			}
		},
		{
		0x00bf,
		0x00bf,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -95)
			}
		},
		{
		0x00c4,
		0x00c6,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		1,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_unicodeToForeign_4)
			}
		},
		{
		0x00c9,
		0x00c9,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -170)
			}
		},
		{
		0x00d1,
		0x00d1,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -116)
			}
		},
		{
		0x00d6,
		0x00d6,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -122)
			}
		},
		{
		0x00d8,
		0x00d8,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -205)
			}
		},
		{
		0x00dc,
		0x00dc,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -126)
			}
		},
		{
		0x00df,
		0x00e0,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		1,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_unicodeToForeign_5)
			}
		},
		{
		0x00e4,
		0x00e9,
		SCnvConversionData::SOneDirectionData::SRange::EIndexedTable16,
		1,
		0,
			{
			UData_SIndexedTable16(indexedTable16_unicodeToForeign_1)
			}
		},
		{
		0x00ec,
		0x00ec,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -229)
			}
		},
		{
		0x00f1,
		0x00f2,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		1,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_unicodeToForeign_6)
			}
		},
		{
		0x00f6,
		0x00f6,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -122)
			}
		},
		{
		0x00f8,
		0x00f9,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		1,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_unicodeToForeign_7)
			}
		},
		{
		0x00fc,
		0x00fc,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -126)
			}
		},
		{
		0x0393,
		0x0394,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		1,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_unicodeToForeign_8)
			}
		},
		{
		0x0398,
		0x0398,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -895)
			}
		},
		{
		0x039b,
		0x039b,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -903)
			}
		},
		{
		0x039e,
		0x039e,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -900)
			}
		},
		{
		0x03a0,
		0x03a0,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -906)
			}
		},
		{
		0x03a3,
		0x03a3,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -907)
			}
		},
		{
		0x03a6,
		0x03a6,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		1,
		0,
			{
			STATIC_CAST(TUint, -916)
			}
		},
		{
		0x03a8,
		0x03a9,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		1,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_unicodeToForeign_9)
			}
		},
		{
		0x20ac,
		0x20ac,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		2,
		0,
			{
			STATIC_CAST(TUint, -1351)
			}
		}
	};

GLDEF_D const SCnvConversionData conversionData=
	{
	SCnvConversionData::EFixedBigEndian,
		{
		ARRAY_LENGTH(foreignVariableByteDataRanges),
		foreignVariableByteDataRanges
		},
		{
		ARRAY_LENGTH(foreignToUnicodeDataRanges),
		foreignToUnicodeDataRanges
		},
		{
		ARRAY_LENGTH(unicodeToForeignDataRanges),
		unicodeToForeignDataRanges
		},
	NULL,
	NULL
	};

