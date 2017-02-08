# set-debuggable

Limited-purpose command-line tool that sets the `debuggable` flag in Android manifest. It fails if the flag is not present (it can't add the flag to the manifest).

Usage:

Copy `AndroidManifest.xml` alongside the executable and run `set-debuggable`.

Sample output:

    skipping node at offset 00000008 of type 00000001, size 0000078C
    RESOURCE_MAP: (offset 00000794, type 00000180, size 00000060)
    [0000] 0101021C
    [0001] 0101021B
    [0002] 010102B7
    [0003] 01010284
    [0004] 01010285
    [0005] 01010286
    [0006] 010102BF
    [0007] 0101026C
    [0008] 01010000
    [0009] 01010002
    [000A] 01010001
    [000B] 0101000F: DEBUGGABLE_ATTR
    [000C] 010103F4
    [000D] 01010003
    [000E] 0101001E
    [000F] 0101001D
    [0010] 0101001F
    [0011] 01010024
    [0012] 0101020C
    [0013] 01010270
    [0014] 01010281
    [0015] 0101028E
    skipping node at offset 000007F4 of type 00000100, size 00000018
    START_ELEMENT with 6 attribute(s) found
    skipping attribute with name ref 00000001, value 00000001
    skipping attribute with name ref 00000000, value 0000001E
    skipping attribute with name ref 00000002, value 00000002
    skipping attribute with name ref 00000019, value 0000001D
    skipping attribute with name ref 0000001A, value 00000017
    skipping attribute with name ref 0000001B, value 00000020
    START_ELEMENT with 5 attribute(s) found
    skipping attribute with name ref 00000007, value FFFFFFFF
    skipping attribute with name ref 00000003, value FFFFFFFF
    skipping attribute with name ref 00000004, value FFFFFFFF
    skipping attribute with name ref 00000005, value FFFFFFFF
    skipping attribute with name ref 00000006, value FFFFFFFF
    skipping node at offset 00000930 of type 00000103, size 00000018
    START_ELEMENT with 5 attribute(s) found
    skipping attribute with name ref 00000008, value 7F040000
    skipping attribute with name ref 0000000A, value 7F030000
    skipping attribute with name ref 00000009, value 7F020000
    'debuggable' attribute: name ref 0000000B, value 00000000
    the debuggable attribute value successfully rewritten to FFFFFFFF.