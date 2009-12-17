# Copyright (C) 2002 Nokia Corporation.
# This material, including documentation and any related 
# computer programs, is protected by copyright controlled by 
# Nokia Corporation. All rights are reserved. Copying, 
# including reproducing, storing,  adapting or translating, any 
# or all of this material requires the prior written consent of 
# Nokia Corporation. This material also contains confidential 
# information which may not be disclosed to others without the 
# prior written consent of Nokia Corporation.
#

Endianness                                                          FixedBigEndian
ReplacementForUnconvertibleUnicodeCharacters                        0x3f # This is code which replaces unknown chars

StartForeignVariableByteData
#   FirstInitialByteValueInRange    LastInitialByteValueInRange     NumberOfSubsequentBytes
    0x00                            0x1a                            0
    0x1b                            0x1b                            1
    0x1c                            0x7f                            0
EndForeignVariableByteData

# following table is made based on the output of: analyse.pl GSM0338.TXT foreign.out foreign
#   IncludePriority SearchPriority  FirstInputCharacterCodeInRange  LastInputCharacterCodeInRange   Algorithm
StartForeignToUnicodeData
    1               1               0x00                            0x1a                            IndexedTable16 {}
    1               1               0x1c                            0x1f                            KeyedTable1616 {}
    1               1               0x20                            0x23                            Direct {}
    1               1               0x24                            0x24                            Offset {}
    1               1               0x25                            0x3f                            Direct {}
    1               1               0x40                            0x40                            Offset {}
    1               1               0x41                            0x5a                            Direct {}
    1               1               0x5b                            0x60                            IndexedTable16 {}
    1               1               0x61                            0x7a                            Direct {}
    1               1               0x7b                            0x7f                            KeyedTable1616 {}
    1               1               0x1b0a                          0x1b0a                          Offset {}
    1               1               0x1b14                          0x1b14                          Offset {}
    1               1               0x1b28                          0x1b29                          KeyedTable1616 {}
    1               1               0x1b2f                          0x1b2f                          Offset {}
    1               1               0x1b3c                          0x1b3e                          KeyedTable1616 {}
    1               1               0x1b40                          0x1b40                          Offset {}
    1               1               0x1b65                          0x1b65                          Offset {}
EndForeignToUnicodeData

# following table is made based on the output of: analyse.pl GSM0338.TXT unicode.out Unicode
#   IncludePriority SearchPriority  FirstInputCharacterCodeInRange  LastInputCharacterCodeInRange   Algorithm SizeOfOutputCharacterCodeInBytes
StartUnicodeToForeignData
    1               1               0x000a                          0x000a                          Direct 1 {}
# ext; form feed
    1               1               0x000c                          0x000c                          Offset 2 {}
    1               1               0x000d                          0x000d                          Direct 1 {}
    1               1               0x0020                          0x0023                          Direct 1 {}
    1               1               0x0024                          0x0024                          Offset 1 {}
    1               1               0x0025                          0x003f                          Direct 1 {}
    1               1               0x0040                          0x0040                          Offset 1 {}
    1               1               0x0041                          0x005a                          Direct 1 {}
# ext; left square bracket, reverse solidus, right square bracket, circumflex accent
    1               1               0x005b                          0x005e                          KeyedTable1616 2 {}
    1               1               0x005f                          0x005f                          Offset 1 {}
    1               1               0x0061                          0x007a                          Direct 1 {}
# ext; left curly bracket, vertical line, right curly pracket, tilde
    1               1               0x007b                          0x007e                          KeyedTable1616 2 {}
    1               1               0x00a1                          0x00a1                          Offset 1 {}
    1               1               0x00a3                          0x00a5                          KeyedTable1616 1 {}
    1               1               0x00a7                          0x00a7                          Offset 1 {}
    1               1               0x00bf                          0x00bf                          Offset 1 {}
    1               1               0x00c4                          0x00c6                          KeyedTable1616 1 {}
    1               1               0x00c9                          0x00c9                          Offset 1 {}
    1               1               0x00d1                          0x00d1                          Offset 1 {}
    1               1               0x00d6                          0x00d6                          Offset 1 {}
    1               1               0x00d8                          0x00d8                          Offset 1 {}
    1               1               0x00dc                          0x00dc                          Offset 1 {}
    1               1               0x00df                          0x00e0                          KeyedTable1616 1 {}
    1               1               0x00e4                          0x00e9                          IndexedTable16 1 {}
    1               1               0x00ec                          0x00ec                          Offset 1 {}
    1               1               0x00f1                          0x00f2                          KeyedTable1616 1 {}
    1               1               0x00f6                          0x00f6                          Offset 1 {}
    1               1               0x00f8                          0x00f9                          KeyedTable1616 1 {}
    1               1               0x00fc                          0x00fc                          Offset 1 {}
    1               1               0x0393                          0x0394                          KeyedTable1616 1 {}
    1               1               0x0398                          0x0398                          Offset 1 {}
    1               1               0x039b                          0x039b                          Offset 1 {}
    1               1               0x039e                          0x039e                          Offset 1 {}
    1               1               0x03a0                          0x03a0                          Offset 1 {}
    1               1               0x03a3                          0x03a3                          Offset 1 {}
    1               1               0x03a6                          0x03a6                          Offset 1 {}
    1               1               0x03a8                          0x03a9                          KeyedTable1616 1 {}
# ext; euro currency sign
    1               1               0x20ac                          0x20ac                          Offset 2 {}
EndUnicodeToForeignData
