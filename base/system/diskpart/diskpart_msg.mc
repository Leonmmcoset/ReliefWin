
MessageIdTypedef=DWORD

SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )

FacilityNames=(System=0x0:FACILITY_SYSTEM
              )

LanguageNames=(English=0x409:MSG00409
               Chinese=0x804:MSG00804)


MessageId=10000
SymbolicName=MSG_COMMAND_ACTIVE
Severity=Informational
Facility=System
Language=English
    On disks with master boot record (MBR) disk formatting, marks
    the partition with focus as active.

Syntax:  ACTIVE

    Writes a value to the disk which is read by the basic input/output
    system (BIOS) at boot. This value specifies that the partition is
    a valid system partition.

    A partition must be selected for this operation to succeed.

    Caution:

        DiskPart verifies only that the partition is capable of containing the
        operating system startup files. DiskPart does not check the contents of
        the partition. If you mistakenly mark a partition as active and it does
        not contain the operating system startup files, your computer might not
        start.

Example:

    ACTIVE
.
Language=Chinese
    On disks with master boot record (MBR) disk formatting, marks
    the partition with focus as active.

Syntax:  ACTIVE

    Writes a value to the disk which is read by the basic input/output
    system (BIOS) at boot. This value specifies that the partition is
    a valid system partition.

    A partition must be selected for this operation to succeed.

    Caution:

        DiskPart verifies only that the partition is capable of containing the
        operating system startup files. DiskPart does not check the contents of
        the partition. If you mistakenly mark a partition as active and it does
        not contain the operating system startup files, your computer might not
        start.

Example:

    ACTIVE
.


MessageId=10001
SymbolicName=MSG_COMMAND_ADD
Severity=Informational
Facility=System
Language=English
<Add ADD command help text here>
.
Language=Chinese
<Add ADD command help text here>
.


MessageId=10002
SymbolicName=MSG_COMMAND_ASSIGN
Severity=Informational
Facility=System
Language=English
    Assigns a drive letter or mounted folder pathname to the volume with focus.

Syntax:  ASSIGN [LETTER=<D> | MOUNT=<PATH>] [NOERR]

    LETTER=<D>  The drive letter to assign to the volume.

    MOUNT=<PATH>
                The mounted folder pathname to assign to the volume.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    If no drive letter or mounted folder is specified, the next available drive
    letter is assigned. If the drive letter or mounted folder is already in use,
    an error is generated.

    By using the ASSIGN command, you can change the drive letter associated
    with a removable drive.

    You cannot assign drive letters to boot volumes or
    volumes that contain the paging file. In addition, you cannot assign
    a drive letter to an Original Equipment Manufacturer (OEM) partition
    unless booted to Windows PE, or any GUID Partition Table (GPT) partition
    other than a basic data partition, an ESP partition or a recovery
    partition.

    A volume must be selected for this operation to succeed.

Example:

    ASSIGN LETTER=D
.
Language=Chinese
    Assigns a drive letter or mounted folder pathname to the volume with focus.

Syntax:  ASSIGN [LETTER=<D> | MOUNT=<PATH>] [NOERR]

    LETTER=<D>  The drive letter to assign to the volume.

    MOUNT=<PATH>
                The mounted folder pathname to assign to the volume.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    If no drive letter or mounted folder is specified, the next available drive
    letter is assigned. If the drive letter or mounted folder is already in use,
    an error is generated.

    By using the ASSIGN command, you can change the drive letter associated
    with a removable drive.

    You cannot assign drive letters to boot volumes or
    volumes that contain the paging file. In addition, you cannot assign
    a drive letter to an Original Equipment Manufacturer (OEM) partition
    unless booted to Windows PE, or any GUID Partition Table (GPT) partition
    other than a basic data partition, an ESP partition or a recovery
    partition.

    A volume must be selected for this operation to succeed.

Example:

    ASSIGN LETTER=D
.


MessageId=10003
SymbolicName=MSG_COMMAND_ATTACH
Severity=Informational
Facility=System
Language=English
<Add ATTACH command help text here>
.
Language=Chinese
<Add ATTACH command help text here>
.


MessageId=10004
SymbolicName=MSG_COMMAND_ATTRIBUTES
Severity=Informational
Facility=System
Language=English
<Add ATTRIBUTES command help text here>
.
Language=Chinese
<Add ATTRIBUTES command help text here>
.


MessageId=10005
SymbolicName=MSG_COMMAND_AUTOMOUNT
Severity=Informational
Facility=System
Language=English
    Enables or disables the automount feature.

Syntax:  AUTOMOUNT [ENABLE] [DISABLE] [SCRUB] [NOERR]

    ENABLE      Enables ReliefWin to automatically assign drive letters
                to volumes that are added to the system.

    DISABLE     Prevents ReliefWin from automatically assigning drive letters
                to volumes that are added to the system.

    SCRUB       Removes mounted folder pathnames, drive letters, mounted
                folder directories, and registry settings, for volumes that
                are no longer in the system. This prevents volumes that were
                previously in the system from being automatically assigned
                their former drive letters and mounted folder pathnames when
                they are reintroduced to the system.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    When the AutoMount feature is enabled, ReliefWin automatically brings the
    volume online and assigns a drive letter and a volume GUID pathname to
    the volume, when the volume is added to the system. In storage area network
    (SAN) configurations, disabling AutoMount prevents ReliefWin from automatically
    onlining the volume and assigning drive letters or volume GUID pathnames
    to any new volumes that are visible to the system.

Example:

    AUTOMOUNT
    AUTOMOUNT ENABLE
    AUTOMOUNT DISABLE
.
Language=Chinese
    Enables or disables the automount feature.

Syntax:  AUTOMOUNT [ENABLE] [DISABLE] [SCRUB] [NOERR]

    ENABLE      Enables ReliefWin to automatically assign drive letters
                to volumes that are added to the system.

    DISABLE     Prevents ReliefWin from automatically assigning drive letters
                to volumes that are added to the system.

    SCRUB       Removes mounted folder pathnames, drive letters, mounted
                folder directories, and registry settings, for volumes that
                are no longer in the system. This prevents volumes that were
                previously in the system from being automatically assigned
                their former drive letters and mounted folder pathnames when
                they are reintroduced to the system.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    When the AutoMount feature is enabled, ReliefWin automatically brings the
    volume online and assigns a drive letter and a volume GUID pathname to
    the volume, when the volume is added to the system. In storage area network
    (SAN) configurations, disabling AutoMount prevents ReliefWin from automatically
    onlining the volume and assigning drive letters or volume GUID pathnames
    to any new volumes that are visible to the system.

Example:

    AUTOMOUNT
    AUTOMOUNT ENABLE
    AUTOMOUNT DISABLE
.


MessageId=10006
SymbolicName=MSG_COMMAND_BREAK
Severity=Informational
Facility=System
Language=English
<Add BREAK command help text here>
.
Language=Chinese
<Add BREAK command help text here>
.


MessageId=10007
SymbolicName=MSG_COMMAND_CLEAN
Severity=Informational
Facility=System
Language=English
    Removes any and all partition or volume formatting from the disk with
    focus.

Syntax:  CLEAN [ALL]

    ALL         Specifies that each and every byte\sector on the disk is set to
                zero, which completely deletes all data contained on the disk.

    On master boot record (MBR) disks, only the MBR partitioning information
    and hidden sector information are overwritten. On GUID partition table
    (GPT) disks, the GPT partitioning information, including the Protective
    MBR, is overwritten. If the ALL parameter is not used, the first 1MB
    and the last 1MB of the disk are zeroed. This erases any disk formatting 
    that had been previously applied to the disk. The disk's state after 
    cleaning the disk is 'UNINITIALIZED'.

Example:

    CLEAN
.
Language=Chinese
    Removes any and all partition or volume formatting from the disk with
    focus.

Syntax:  CLEAN [ALL]

    ALL         Specifies that each and every byte\sector on the disk is set to
                zero, which completely deletes all data contained on the disk.

    On master boot record (MBR) disks, only the MBR partitioning information
    and hidden sector information are overwritten. On GUID partition table
    (GPT) disks, the GPT partitioning information, including the Protective
    MBR, is overwritten. If the ALL parameter is not used, the first 1MB
    and the last 1MB of the disk are zeroed. This erases any disk formatting 
    that had been previously applied to the disk. The disk's state after 
    cleaning the disk is 'UNINITIALIZED'.

Example:

    CLEAN
.


MessageId=10008
SymbolicName=MSG_COMMAND_COMPACT
Severity=Informational
Facility=System
Language=English
<Add COMPACT command help text here>
.
Language=Chinese
<Add COMPACT command help text here>
.


MessageId=10010
SymbolicName=MSG_COMMAND_CONVERT_GPT
Severity=Informational
Facility=System
Language=English
    Converts a disk with an MBR (Master Boot Record) partition table
    into a disk with a GPT (GUID partititon table) partititon table.

Syntax:  CONVERT GPT [NOERR]

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    An MBR disk must be selected for this operation to succeed.

    The disk must have a minimum size of 128 MB.

    Important:
        Only empty disks can be converted to GPT. Backup all data and delete all
        partitions or volumes before you convert the disk.

Example:
    CONVERT GPT
.
Language=Chinese
    Converts a disk with an MBR (Master Boot Record) partition table
    into a disk with a GPT (GUID partititon table) partititon table.

Syntax:  CONVERT GPT [NOERR]

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    An MBR disk must be selected for this operation to succeed.

    The disk must have a minimum size of 128 MB.

    Important:
        Only empty disks can be converted to GPT. Backup all data and delete all
        partitions or volumes before you convert the disk.

Example:
    CONVERT GPT
.


MessageId=10011
SymbolicName=MSG_COMMAND_CONVERT_MBR
Severity=Informational
Facility=System
Language=English
    Converts a disk with a GPT (GUID partititon table) partition table
    into a disk with an MBR (Master Boot Record) partititon table.

Syntax:  CONVERT MBR [NOERR]

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    A GPT disk must be selected for this operation to succeed.

    Important:
        Only empty disks can be converted to MBR. Backup all data and delete all
        partitions or volumes before you convert the disk.

Example:
    CONVERT MBR
.
Language=Chinese
    Converts a disk with a GPT (GUID partititon table) partition table
    into a disk with an MBR (Master Boot Record) partititon table.

Syntax:  CONVERT MBR [NOERR]

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    A GPT disk must be selected for this operation to succeed.

    Important:
        Only empty disks can be converted to MBR. Backup all data and delete all
        partitions or volumes before you convert the disk.

Example:
    CONVERT MBR
.


MessageId=10012
SymbolicName=MSG_COMMAND_CREATE_PARTITION_EFI
Severity=Informational
Facility=System
Language=English
    Creates an Extensible Firmware Interface (EFI) system partition on a GUID
    partition table (GPT) disk.

Syntax:  CREATE PARTITION EFI [SIZE=<N>] [OFFSET=<N>] [NOERR]

    SIZE=<N>    The size of the partition in megabytes (MB). If no size is
                given, the partition continues until there is no more free
                space in the current region.

    OFFSET=<N>  The offset, in kilobytes (KB), at which the partition is
                created. If no offset is given, the partition is placed in the
                first disk extent that is large enough to hold it.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    After the partition has been created, the focus is given to the new
    partition.

    A basic GPT disk must be selected for this operation to succeed.

Example:

    CREATE PARTITION EFI SIZE=1000
.
Language=Chinese
    Creates an Extensible Firmware Interface (EFI) system partition on a GUID
    partition table (GPT) disk.

Syntax:  CREATE PARTITION EFI [SIZE=<N>] [OFFSET=<N>] [NOERR]

    SIZE=<N>    The size of the partition in megabytes (MB). If no size is
                given, the partition continues until there is no more free
                space in the current region.

    OFFSET=<N>  The offset, in kilobytes (KB), at which the partition is
                created. If no offset is given, the partition is placed in the
                first disk extent that is large enough to hold it.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    After the partition has been created, the focus is given to the new
    partition.

    A basic GPT disk must be selected for this operation to succeed.

Example:

    CREATE PARTITION EFI SIZE=1000
.


MessageId=10013
SymbolicName=MSG_COMMAND_CREATE_PARTITION_EXTENDED
Severity=Informational
Facility=System
Language=English
    Creates an extended partition on the disk with focus.
    Applies to master boot record (MBR) disks only.

Syntax:  CREATE PARTITION EXTENDED [SIZE=<N>] [OFFSET=<N>] [ALIGN=<N>] [NOERR]

    SIZE=<N>    The size of the partition in megabytes (MB). If no size is
                given, the partition continues until there is no more free
                space in the extended partition.

    OFFSET=<N>  The offset, in kilobytes (KB), at which the partition is
                created. If no offset is given, the partition will start
                at the beginning of the first free space on the disk that
                is large enough to hold the new partition.

    ALIGN=<N>   Typically used with hardware RAID Logical Unit Number (LUN)
                arrays to improve performance. The partition offset will be
                a multiple of <N>. If the OFFSET parameter is specified, it
                will be rounded to the closest multiple of <N>.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    After the partition has been created, the focus automatically shifts to the
    new partition. Only one extended partition can be created per disk. This
    command fails if you attempt to create an extended partition within another
    extended partition. You must create an extended partition before you can
    create logical partitions.

    A basic MBR disk must be selected for this operation to succeed.

Example:

    CREATE PARTITION EXTENDED SIZE=1000
.
Language=Chinese
    Creates an extended partition on the disk with focus.
    Applies to master boot record (MBR) disks only.

Syntax:  CREATE PARTITION EXTENDED [SIZE=<N>] [OFFSET=<N>] [ALIGN=<N>] [NOERR]

    SIZE=<N>    The size of the partition in megabytes (MB). If no size is
                given, the partition continues until there is no more free
                space in the extended partition.

    OFFSET=<N>  The offset, in kilobytes (KB), at which the partition is
                created. If no offset is given, the partition will start
                at the beginning of the first free space on the disk that
                is large enough to hold the new partition.

    ALIGN=<N>   Typically used with hardware RAID Logical Unit Number (LUN)
                arrays to improve performance. The partition offset will be
                a multiple of <N>. If the OFFSET parameter is specified, it
                will be rounded to the closest multiple of <N>.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    After the partition has been created, the focus automatically shifts to the
    new partition. Only one extended partition can be created per disk. This
    command fails if you attempt to create an extended partition within another
    extended partition. You must create an extended partition before you can
    create logical partitions.

    A basic MBR disk must be selected for this operation to succeed.

Example:

    CREATE PARTITION EXTENDED SIZE=1000
.


MessageId=10014
SymbolicName=MSG_COMMAND_CREATE_PARTITION_LOGICAL
Severity=Informational
Facility=System
Language=English
    Creates a logical partition in an extended partition.
    Applies to master boot record (MBR) disks only.

Syntax:  CREATE PARTITION LOGICAL [SIZE=<N>] [OFFSET=<N>] [ALIGN=<N>] [NOERR]

    SIZE=<N>    The size of the partition in megabytes (MB). The partition is
                at least as big in bytes as the number specified by N. If you
                specify a size for the logical partition, it must be smaller
                than the extended partition. If no size is given, the partition
                continues until there is no more free space in the extended
                partition.

    OFFSET=<N>  The offset, in kilobytes (KB), at which the partition is created.
                If no offset is given, the partition is placed in the first disk
                extent that is large enough to hold it.

    ALIGN=<N>   Typically used with hardware RAID Logical Unit Number (LUN)
                arrays to improve performance. The partition offset will be
                a multiple of <N>. If the OFFSET parameter is specified, it
                will be rounded to the closest multiple of <N>.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    After the partition has been created, the focus automatically shifts to the
    new logical partition.

    A basic MBR disk must be selected for this operation to succeed.

Example:

    CREATE PARTITION LOGICAL SIZE=1000
.
Language=Chinese
    Creates a logical partition in an extended partition.
    Applies to master boot record (MBR) disks only.

Syntax:  CREATE PARTITION LOGICAL [SIZE=<N>] [OFFSET=<N>] [ALIGN=<N>] [NOERR]

    SIZE=<N>    The size of the partition in megabytes (MB). The partition is
                at least as big in bytes as the number specified by N. If you
                specify a size for the logical partition, it must be smaller
                than the extended partition. If no size is given, the partition
                continues until there is no more free space in the extended
                partition.

    OFFSET=<N>  The offset, in kilobytes (KB), at which the partition is created.
                If no offset is given, the partition is placed in the first disk
                extent that is large enough to hold it.

    ALIGN=<N>   Typically used with hardware RAID Logical Unit Number (LUN)
                arrays to improve performance. The partition offset will be
                a multiple of <N>. If the OFFSET parameter is specified, it
                will be rounded to the closest multiple of <N>.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    After the partition has been created, the focus automatically shifts to the
    new logical partition.

    A basic MBR disk must be selected for this operation to succeed.

Example:

    CREATE PARTITION LOGICAL SIZE=1000
.


MessageId=10015
SymbolicName=MSG_COMMAND_CREATE_PARTITION_MSR
Severity=Informational
Facility=System
Language=English
    Creates a Microsoft Reserved (MSR) partition on a GUID partition table
    (GPT) disk.

Syntax:  CREATE PARTITION MSR [SIZE=<N>] [OFFSET=<N>] [NOERR]

    SIZE=<N>    The size of the partition in megabytes (MB). The partition is
                at least as big in bytes as the number specified by N. If no
                size is given, the partition continues until there is no more
                free space in the current region.

    OFFSET=<N>  The offset, in kilobytes (KB), at which the partition is
                created. If no offset is given, the partition is placed in the
                first disk extent that is large enough to hold it.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    A basic GPT disk must be selected for this operation to succeed.

    Caution:

        Be very careful when using this command. Because GPT disks require a
        specific partition layout, creating Microsoft Reserved partitions could
        cause the disk to become unreadable. On GPT disks that are used to boot
        ReliefWin, the EFI System partition is the first partition on the disk,
        followed by the Microsoft Reserved partition. GPT disks used only for
        data storage do not have an EFI System partition, in which case the
        Microsoft Reserved partition is the first partition.

    ReliefWin does not online Microsoft Reserved partitions. You cannot store data
    on them and you cannot delete them.

Example:

    CREATE PARTITION MSR SIZE=1000
.
Language=Chinese
    Creates a Microsoft Reserved (MSR) partition on a GUID partition table
    (GPT) disk.

Syntax:  CREATE PARTITION MSR [SIZE=<N>] [OFFSET=<N>] [NOERR]

    SIZE=<N>    The size of the partition in megabytes (MB). The partition is
                at least as big in bytes as the number specified by N. If no
                size is given, the partition continues until there is no more
                free space in the current region.

    OFFSET=<N>  The offset, in kilobytes (KB), at which the partition is
                created. If no offset is given, the partition is placed in the
                first disk extent that is large enough to hold it.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    A basic GPT disk must be selected for this operation to succeed.

    Caution:

        Be very careful when using this command. Because GPT disks require a
        specific partition layout, creating Microsoft Reserved partitions could
        cause the disk to become unreadable. On GPT disks that are used to boot
        ReliefWin, the EFI System partition is the first partition on the disk,
        followed by the Microsoft Reserved partition. GPT disks used only for
        data storage do not have an EFI System partition, in which case the
        Microsoft Reserved partition is the first partition.

    ReliefWin does not online Microsoft Reserved partitions. You cannot store data
    on them and you cannot delete them.

Example:

    CREATE PARTITION MSR SIZE=1000
.


MessageId=10016
SymbolicName=MSG_COMMAND_CREATE_PARTITION_PRIMARY
Severity=Informational
Facility=System
Language=English
    Creates a primary partition on the basic disk with focus.

Syntax:  CREATE PARTITION PRIMARY [SIZE=<N>] [OFFSET=<N>] [ID={<BYTE> | <GUID>}]
            [ALIGN=<N>] [NOERR]

    SIZE=<N>    The size of the partition in megabytes (MB). If no size is
                given, the partition continues until there is no more
                unallocated space in the current region.

    OFFSET=<N>  The offset, in kilobytes (KB), at which the partition is created.
                If no offset is given, the partition is placed in the first disk
                extent that is large enough to hold it.

    ID={<BYTE> | <GUID>}

                Specifies the partition type.

                Intended for Original Equipment Manufacturer (OEM) use only.

                For master boot record (MBR) disks, you can specify a partition
                type byte, in hexadecimal form, for the partition. If this
                parameter is not specified for an MBR disk, the command creates
                a partition of type 0x06 (specifies no file system is installed).

                    LDM data partition:
                        0x42

                    Recovery partition:
                        0x27

                    Recognized OEM Ids:
                        0x12
                        0x84
                        0xDE
                        0xFE
                        0xA0

                For GUID partition table (GPT) disks you can specify a
                partition type GUID for the partition you want to create.
                Recognized GUIDs include:

                    EFI System partition:
                        c12a7328-f81f-11d2-ba4b-00a0c93ec93b

                    Microsoft Reserved partition:
                        e3c9e316-0b5c-4db8-817d-f92df00215ae

                    Basic data partition:
                        ebd0a0a2-b9e5-4433-87c0-68b6b72699c7

                    LDM Metadata partition on a dynamic disk:
                        5808c8aa-7e8f-42e0-85d2-e1e90434cfb3

                    LDM Data partition on a dynamic disk:
                        af9b60a0-1431-4f62-bc68-3311714a69ad

                    Recovery partition:
                        de94bba4-06d1-4d40-a16a-bfd50179d6ac

                If this parameter is not specified for a GPT disk, the command
                creates a basic data partition.

                Any partition type byte or GUID can be specified with this
                parameter. DiskPart does not check the partition type for
                validity except to ensure that it is a byte in hexadecimal form
                or a GUID.

                Caution:

                    Creating partitions with this parameter might cause your
                    computer to fail or be unable to start up. Unless you are
                    an OEM or an IT professional experienced with GPT disks, do
                    not create partitions on GPT disks using this parameter.
                    Instead, always use the CREATE PARTITION EFI command to
                    create EFI System partitions, the CREATE PARTITION MSR
                    command to create Microsoft Reserved partitions, and the
                    CREATE PARTITION PRIMARY command without this parameter to
                    create primary partitions on GPT disks.

    ALIGN=<N>   Typically used with hardware RAID Logical Unit Number (LUN)
                arrays to improve performance. The partition offset will be
                a multiple of <N>. If the OFFSET parameter is specified, it
                will be rounded to the closest multiple of <N>.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    After you create the partition, the focus automatically shifts to the new
    partition. The partition does not receive a drive letter. You must use the
    assign command to assign a drive letter to the partition.

    A basic disk must be selected for this operation to succeed.

    If a partition type is not specified, the disk is uninitialized and disk
    size is greater than 2TB, it will be initialized to GPT.

Example:

    CREATE PARTITION PRIMARY SIZE=1000
    CREATE PARTITION PRIMARY SIZE=128 ID=c12a7328-f81f-11d2-ba4b-00a0c93ec93b
    CREATE PARTITION PRIMARY SIZE=10000 ID=12
    CREATE PARTITION PRIMARY SIZE=10000 ID=DE
.
Language=Chinese
    Creates a primary partition on the basic disk with focus.

Syntax:  CREATE PARTITION PRIMARY [SIZE=<N>] [OFFSET=<N>] [ID={<BYTE> | <GUID>}]
            [ALIGN=<N>] [NOERR]

    SIZE=<N>    The size of the partition in megabytes (MB). If no size is
                given, the partition continues until there is no more
                unallocated space in the current region.

    OFFSET=<N>  The offset, in kilobytes (KB), at which the partition is created.
                If no offset is given, the partition is placed in the first disk
                extent that is large enough to hold it.

    ID={<BYTE> | <GUID>}

                Specifies the partition type.

                Intended for Original Equipment Manufacturer (OEM) use only.

                For master boot record (MBR) disks, you can specify a partition
                type byte, in hexadecimal form, for the partition. If this
                parameter is not specified for an MBR disk, the command creates
                a partition of type 0x06 (specifies no file system is installed).

                    LDM data partition:
                        0x42

                    Recovery partition:
                        0x27

                    Recognized OEM Ids:
                        0x12
                        0x84
                        0xDE
                        0xFE
                        0xA0

                For GUID partition table (GPT) disks you can specify a
                partition type GUID for the partition you want to create.
                Recognized GUIDs include:

                    EFI System partition:
                        c12a7328-f81f-11d2-ba4b-00a0c93ec93b

                    Microsoft Reserved partition:
                        e3c9e316-0b5c-4db8-817d-f92df00215ae

                    Basic data partition:
                        ebd0a0a2-b9e5-4433-87c0-68b6b72699c7

                    LDM Metadata partition on a dynamic disk:
                        5808c8aa-7e8f-42e0-85d2-e1e90434cfb3

                    LDM Data partition on a dynamic disk:
                        af9b60a0-1431-4f62-bc68-3311714a69ad

                    Recovery partition:
                        de94bba4-06d1-4d40-a16a-bfd50179d6ac

                If this parameter is not specified for a GPT disk, the command
                creates a basic data partition.

                Any partition type byte or GUID can be specified with this
                parameter. DiskPart does not check the partition type for
                validity except to ensure that it is a byte in hexadecimal form
                or a GUID.

                Caution:

                    Creating partitions with this parameter might cause your
                    computer to fail or be unable to start up. Unless you are
                    an OEM or an IT professional experienced with GPT disks, do
                    not create partitions on GPT disks using this parameter.
                    Instead, always use the CREATE PARTITION EFI command to
                    create EFI System partitions, the CREATE PARTITION MSR
                    command to create Microsoft Reserved partitions, and the
                    CREATE PARTITION PRIMARY command without this parameter to
                    create primary partitions on GPT disks.

    ALIGN=<N>   Typically used with hardware RAID Logical Unit Number (LUN)
                arrays to improve performance. The partition offset will be
                a multiple of <N>. If the OFFSET parameter is specified, it
                will be rounded to the closest multiple of <N>.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    After you create the partition, the focus automatically shifts to the new
    partition. The partition does not receive a drive letter. You must use the
    assign command to assign a drive letter to the partition.

    A basic disk must be selected for this operation to succeed.

    If a partition type is not specified, the disk is uninitialized and disk
    size is greater than 2TB, it will be initialized to GPT.

Example:

    CREATE PARTITION PRIMARY SIZE=1000
    CREATE PARTITION PRIMARY SIZE=128 ID=c12a7328-f81f-11d2-ba4b-00a0c93ec93b
    CREATE PARTITION PRIMARY SIZE=10000 ID=12
    CREATE PARTITION PRIMARY SIZE=10000 ID=DE
.


MessageId=10017
SymbolicName=MSG_COMMAND_DELETE_DISK
Severity=Informational
Facility=System
Language=English
<Add DELETE DISK command help text here>
.
Language=Chinese
<Add DELETE DISK command help text here>
.


MessageId=10018
SymbolicName=MSG_COMMAND_DELETE_PARTITION
Severity=Informational
Facility=System
Language=English
Deletes the partition with focus.

Syntax:  DELETE PARTITION [NOERR] [OVERRIDE]

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    OVERRIDE    Enables DiskPart to delete any partition regardless of type.
                Typically, DiskPart only permits you to delete known data
                partitions.

    You cannot delete the system partition, boot partition, or any partition
    that contains the active paging file or crash dump (memory dump) filed.

    A partition must be selected for this operation to succeed.

    Partitions cannot be deleted from dynamic disks or created on dynamic
    disks.

Example:

    DELETE PARTITION
.
Language=Chinese
Deletes the partition with focus.

Syntax:  DELETE PARTITION [NOERR] [OVERRIDE]

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    OVERRIDE    Enables DiskPart to delete any partition regardless of type.
                Typically, DiskPart only permits you to delete known data
                partitions.

    You cannot delete the system partition, boot partition, or any partition
    that contains the active paging file or crash dump (memory dump) filed.

    A partition must be selected for this operation to succeed.

    Partitions cannot be deleted from dynamic disks or created on dynamic
    disks.

Example:

    DELETE PARTITION
.


MessageId=10019
SymbolicName=MSG_COMMAND_DELETE_VOLUME
Severity=Informational
Facility=System
Language=English
<Add DELETE VOLUME command help text here>
.
Language=Chinese
<Add DELETE VOLUME command help text here>
.


MessageId=10020
SymbolicName=MSG_COMMAND_DETAIL_DISK
Severity=Informational
Facility=System
Language=English
    Displays the properties of the selected disk and the list of volumes on
    the disk.

Syntax:  DETAIL DISK

    A disk must be selected for this operation to succeed.

Example:

    DETAIL DISK
.
Language=Chinese
    Displays the properties of the selected disk and the list of volumes on
    the disk.

Syntax:  DETAIL DISK

    A disk must be selected for this operation to succeed.

Example:

    DETAIL DISK
.

MessageId=10021
SymbolicName=MSG_COMMAND_DETAIL_PARTITION
Severity=Informational
Facility=System
Language=English
    Displays the properties for the selected partition.

Syntax:  DETAIL PARTITION

    A partition must be selected for this operation to succeed.

Example:

    DETAIL PARTITION
.
Language=Chinese
    Displays the properties for the selected partition.

Syntax:  DETAIL PARTITION

    A partition must be selected for this operation to succeed.

Example:

    DETAIL PARTITION
.

MessageId=10022
SymbolicName=MSG_COMMAND_DETAIL_VOLUME
Severity=Informational
Facility=System
Language=English
    Displays the properties for the selected volume and the list of disks on
    which the volume resides.

Syntax:  DETAIL VOLUME

    A volume must be selected for this operation to succeed.

Example:

    DETAIL VOLUME
.
Language=Chinese
    Displays the properties for the selected volume and the list of disks on
    which the volume resides.

Syntax:  DETAIL VOLUME

    A volume must be selected for this operation to succeed.

Example:

    DETAIL VOLUME
.


MessageId=10023
SymbolicName=MSG_COMMAND_DETACH
Severity=Informational
Facility=System
Language=English
<Add DETACH command help text here>
.
Language=Chinese
<Add DETACH command help text here>
.


MessageId=10024
SymbolicName=MSG_COMMAND_EXIT
Severity=Informational
Facility=System
Language=English
    Exits the DiskPart command interpreter.

Syntax:  EXIT

Example:

    EXIT
.
Language=Chinese
    Exits the DiskPart command interpreter.

Syntax:  EXIT

Example:

    EXIT
.


MessageId=10025
SymbolicName=MSG_COMMAND_EXPAND
Severity=Informational
Facility=System
Language=English
<Add EXPAND command help text here>
.
Language=Chinese
<Add EXPAND command help text here>
.


MessageId=10026
SymbolicName=MSG_COMMAND_EXTEND
Severity=Informational
Facility=System
Language=English
<Add EXTEND command help text here>
.
Language=Chinese
<Add EXTEND command help text here>
.


MessageId=10027
SymbolicName=MSG_COMMAND_FILESYSTEMS
Severity=Informational
Facility=System
Language=English
    Displays information about the current file system for the selected
    volume, and the supported file systems for formatting the volume.

Syntax:  FILESYSTEMS

    A volume must be selected for this operation to succeed.

Example:

    FILESYSTEMS
.
Language=Chinese
    Displays information about the current file system for the selected
    volume, and the supported file systems for formatting the volume.

Syntax:  FILESYSTEMS

    A volume must be selected for this operation to succeed.

Example:

    FILESYSTEMS
.


MessageId=10028
SymbolicName=MSG_COMMAND_FORMAT
Severity=Informational
Facility=System
Language=English
    Formats the specified volume for use with ReliefWin.

Syntax:  FORMAT [[FS=<FS>] [REVISION=<X.XX>] | RECOMMENDED] [LABEL=<"label">]
                [UNIT=<N>] [QUICK] [COMPRESS] [OVERRIDE] [DUPLICATE] [NOWAIT]
                [NOERR]

    FS=<FS>     Specifies the type of file system. If no file system is given,
                the default file system displayed by the FILESYSTEMS command is
                used.

    REVISION=<X.XX>

                Specifies the file system revision (if applicable).

    RECOMMENDED If specified, use the recommended file system and revision
                instead of the default if a recommendation exists. The
                recommended file system (if one exists) is displayed by the
                FILESYSTEMS command.

    LABEL=<"label">

                Specifies the volume label.

    UNIT=<N>    Overrides the default allocation unit size. Default settings
                are strongly recommended for general use. The default
                allocation unit size for a particular file system is displayed
                by the FILESYSTEMS command.

                NTFS compression is not supported for allocation unit sizes
                above 4096.

    QUICK       Performs a quick format.

    COMPRESS    NTFS only: Files created on the new volume will be compressed
                by default.

    OVERRIDE    Forces the file system to dismount first if necessary. All
                opened handles to the volume would no longer be valid.

    DUPLICATE   UDF Only: This flag applies to UDF format, version 2.5 or
                higher.
                This flag instructs the format operation to duplicate the file
                system meta-data to a second set of sectors on the disk. The
                duplicate meta-data is used by applications, for example repair
                or recovery applications. If the primary meta-data sectors are
                found to be corrupted, the file system meta-data will be read
                from the duplicate sectors.

    NOWAIT      Forces the command to return immediately while the format
                process is still in progress. If NOWAIT is not specified,
                DiskPart will display format progress in percentage.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    A volume must be selected for this operation to succeed.

Examples:

    FORMAT FS=NTFS LABEL="New Volume" QUICK COMPRESS
    FORMAT RECOMMENDED OVERRIDE
.
Language=Chinese
    Formats the specified volume for use with ReliefWin.

Syntax:  FORMAT [[FS=<FS>] [REVISION=<X.XX>] | RECOMMENDED] [LABEL=<"label">]
                [UNIT=<N>] [QUICK] [COMPRESS] [OVERRIDE] [DUPLICATE] [NOWAIT]
                [NOERR]

    FS=<FS>     Specifies the type of file system. If no file system is given,
                the default file system displayed by the FILESYSTEMS command is
                used.

    REVISION=<X.XX>

                Specifies the file system revision (if applicable).

    RECOMMENDED If specified, use the recommended file system and revision
                instead of the default if a recommendation exists. The
                recommended file system (if one exists) is displayed by the
                FILESYSTEMS command.

    LABEL=<"label">

                Specifies the volume label.

    UNIT=<N>    Overrides the default allocation unit size. Default settings
                are strongly recommended for general use. The default
                allocation unit size for a particular file system is displayed
                by the FILESYSTEMS command.

                NTFS compression is not supported for allocation unit sizes
                above 4096.

    QUICK       Performs a quick format.

    COMPRESS    NTFS only: Files created on the new volume will be compressed
                by default.

    OVERRIDE    Forces the file system to dismount first if necessary. All
                opened handles to the volume would no longer be valid.

    DUPLICATE   UDF Only: This flag applies to UDF format, version 2.5 or
                higher.
                This flag instructs the format operation to duplicate the file
                system meta-data to a second set of sectors on the disk. The
                duplicate meta-data is used by applications, for example repair
                or recovery applications. If the primary meta-data sectors are
                found to be corrupted, the file system meta-data will be read
                from the duplicate sectors.

    NOWAIT      Forces the command to return immediately while the format
                process is still in progress. If NOWAIT is not specified,
                DiskPart will display format progress in percentage.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    A volume must be selected for this operation to succeed.

Examples:

    FORMAT FS=NTFS LABEL="New Volume" QUICK COMPRESS
    FORMAT RECOMMENDED OVERRIDE
.


MessageId=10029
SymbolicName=MSG_COMMAND_GPT
Severity=Informational
Facility=System
Language=English
    On basic GUID partition table (GPT) disks, assigns the GPT attribute(s) to
    the partition with focus.

Syntax:  GPT ATTRIBUTES=<N>

    ATTRIBUTES=<N>

                The hexadecimal value indicating the attributes to apply to the
                partition with focus. The GPT attribute field is a 64-bit field
                that contains two subfields. The higher field is interpreted
                only in the context of a particular partition type GUID, while
                the lower field is common to all partition types.

                The following attribute can be set on any partition:

                0x0000000000000001 marks the partition as required. This
                indicates to all disk management utilities that the partition
                should not be deleted. The EFI System partition contains only
                those binaries necessary to start the operating system. This
                makes it easy for original equipment manufacturer (OEM) or
                operating system-specific binaries to be placed in other
                partitions.

                For basic data partitions, the following attributes are defined:

                0x8000000000000000
                GPT_BASIC_DATA_ATTRIBUTE_NO_DRIVE_LETTER
                Prevents the partition from having a drive
                letter automatically assigned. By default, each partition is
                assigned a new drive letter and a volume GUID pathname. Both
                the drive letter and the volume GUID pathname can be used to
                open the volume using Win32 APIs. Setting this attribute ensures
                that when a disk is moved to a new computer, a new drive letter
                will not be automatically generated. Instead, the user can
                manually assign drive letters.

                0x4000000000000000
                GPT_BASIC_DATA_ATTRIBUTE_HIDDEN
                Defines the partition's volume as hidden.
                Setting this attribute specifies that the volume will not be
                assigned a drive letter or a volume GUID pathname. Both
                the drive letter and the volume GUID pathname can be used to
                open the volume using Win32 APIs. A hidden partition's
                volume is not reported by Win32 APIs such as FindFirstVolume
                and FindNextVolume.

                0x2000000000000000
                GPT_BASIC_DATA_ATTRIBUTE_SHADOW_COPY
                Defines the partition as being a Volume Snapshot Service
                shadow copy volume. Fle system filters use this flag to
                prevent the filter from attaching to the volume.

                0x1000000000000000
                GPT_BASIC_DATA_ATTRIBUTE_READ_ONLY
                Prevents the volume from being written to.

                Other attributes may be added at any time.

    GPT partition attributes give additional information about the use of the
    partition.

    A basic GPT partition must be selected for this operation to succeed.

    Caution:

        Changing the GPT attributes might cause your basic data volumes to
        fail to be assigned drive letters, or prevent the file system from
        mounting. Unless you are an original equipment manufacturer (OEM)
        or an IT professional experienced with GPT disks, do not change GPT
        attributes.

Example:

    GPT ATTRIBUTES=0x8000000000000000
    GPT ATTRIBUTES=0xF000000000000001
.
Language=Chinese
    On basic GUID partition table (GPT) disks, assigns the GPT attribute(s) to
    the partition with focus.

Syntax:  GPT ATTRIBUTES=<N>

    ATTRIBUTES=<N>

                The hexadecimal value indicating the attributes to apply to the
                partition with focus. The GPT attribute field is a 64-bit field
                that contains two subfields. The higher field is interpreted
                only in the context of a particular partition type GUID, while
                the lower field is common to all partition types.

                The following attribute can be set on any partition:

                0x0000000000000001 marks the partition as required. This
                indicates to all disk management utilities that the partition
                should not be deleted. The EFI System partition contains only
                those binaries necessary to start the operating system. This
                makes it easy for original equipment manufacturer (OEM) or
                operating system-specific binaries to be placed in other
                partitions.

                For basic data partitions, the following attributes are defined:

                0x8000000000000000
                GPT_BASIC_DATA_ATTRIBUTE_NO_DRIVE_LETTER
                Prevents the partition from having a drive
                letter automatically assigned. By default, each partition is
                assigned a new drive letter and a volume GUID pathname. Both
                the drive letter and the volume GUID pathname can be used to
                open the volume using Win32 APIs. Setting this attribute ensures
                that when a disk is moved to a new computer, a new drive letter
                will not be automatically generated. Instead, the user can
                manually assign drive letters.

                0x4000000000000000
                GPT_BASIC_DATA_ATTRIBUTE_HIDDEN
                Defines the partition's volume as hidden.
                Setting this attribute specifies that the volume will not be
                assigned a drive letter or a volume GUID pathname. Both
                the drive letter and the volume GUID pathname can be used to
                open the volume using Win32 APIs. A hidden partition's
                volume is not reported by Win32 APIs such as FindFirstVolume
                and FindNextVolume.

                0x2000000000000000
                GPT_BASIC_DATA_ATTRIBUTE_SHADOW_COPY
                Defines the partition as being a Volume Snapshot Service
                shadow copy volume. Fle system filters use this flag to
                prevent the filter from attaching to the volume.

                0x1000000000000000
                GPT_BASIC_DATA_ATTRIBUTE_READ_ONLY
                Prevents the volume from being written to.

                Other attributes may be added at any time.

    GPT partition attributes give additional information about the use of the
    partition.

    A basic GPT partition must be selected for this operation to succeed.

    Caution:

        Changing the GPT attributes might cause your basic data volumes to
        fail to be assigned drive letters, or prevent the file system from
        mounting. Unless you are an original equipment manufacturer (OEM)
        or an IT professional experienced with GPT disks, do not change GPT
        attributes.

Example:

    GPT ATTRIBUTES=0x8000000000000000
    GPT ATTRIBUTES=0xF000000000000001
.


MessageId=10030
SymbolicName=MSG_COMMAND_HELP
Severity=Informational
Facility=System
Language=English
    Displays a list of the available commands or detailed help information for a
    specified command.

Syntax:  HELP [<COMMAND>]

    <COMMAND>   The command for which to display detail help.

    If no command is specified, HELP will display all possible commands.

Example:

    HELP
    HELP CREATE PARTITION PRIMARY
.
Language=Chinese
    Displays a list of the available commands or detailed help information for a
    specified command.

Syntax:  HELP [<COMMAND>]

    <COMMAND>   The command for which to display detail help.

    If no command is specified, HELP will display all possible commands.

Example:

    HELP
    HELP CREATE PARTITION PRIMARY
.


MessageId=10031
SymbolicName=MSG_COMMAND_IMPORT
Severity=Informational
Facility=System
Language=English
<Add IMPORT command help text here>
.
Language=Chinese
<Add IMPORT command help text here>
.


MessageId=10032
SymbolicName=MSG_COMMAND_INACTIVE
Severity=Informational
Facility=System
Language=English
    On disks with master boot record (MBR) disk formatting, marks
    the partition with focus as inactive.

Syntax:  INACTIVE

    The computer may start from the next option specified in the BIOS such as a
    CD-ROM drive or a Pre-Boot eXecution Environment (PXE)-based boot
    environment (such as Remote Installation Services (RIS)) when you restart
    the computer.

    A partition must be selected for this operation to succeed.

    Caution:

        Your computer might not start without an active partition. Do not mark
        a system or boot partition as inactive unless you are an experienced
        user with a thorough understanding of ReliefWin storage management.

Example:

    INACTIVE
.
Language=Chinese
    On disks with master boot record (MBR) disk formatting, marks
    the partition with focus as inactive.

Syntax:  INACTIVE

    The computer may start from the next option specified in the BIOS such as a
    CD-ROM drive or a Pre-Boot eXecution Environment (PXE)-based boot
    environment (such as Remote Installation Services (RIS)) when you restart
    the computer.

    A partition must be selected for this operation to succeed.

    Caution:

        Your computer might not start without an active partition. Do not mark
        a system or boot partition as inactive unless you are an experienced
        user with a thorough understanding of ReliefWin storage management.

Example:

    INACTIVE
.


MessageId=10033
SymbolicName=MSG_COMMAND_LIST_DISK
Severity=Informational
Facility=System
Language=English
    Displays a list of disks.

Syntax:  LIST DISK

    Displays a list of disks and information about them, such as their
    size, amount of available free space, whether the disk is a basic
    or dynamic disk, and whether the disk uses the master boot record
    (MBR) or GUID partition table (GPT) partition style. The disk marked
    with an asterisk (*) has focus.

    Note that the FREE column does not display the total amount of free
    space on the disk, but rather the amount of usable free space left
    on the disk. For example, if you have a 10GB disk with 4 primary
    partitions covering 5GB, there is no usable free space left (no
    more partitions may be created). Another example would be you have
    a 10GB disk with 3 primary partitions and an extended partition
    covering 8GB. The exended partition is of size 3GB with one logical
    drive of size 2GB. The disk will show only 1GB as free - the
    1GB of free space in the extended partition.

Example:

    LIST DISK
.
Language=Chinese
    Displays a list of disks.

Syntax:  LIST DISK

    Displays a list of disks and information about them, such as their
    size, amount of available free space, whether the disk is a basic
    or dynamic disk, and whether the disk uses the master boot record
    (MBR) or GUID partition table (GPT) partition style. The disk marked
    with an asterisk (*) has focus.

    Note that the FREE column does not display the total amount of free
    space on the disk, but rather the amount of usable free space left
    on the disk. For example, if you have a 10GB disk with 4 primary
    partitions covering 5GB, there is no usable free space left (no
    more partitions may be created). Another example would be you have
    a 10GB disk with 3 primary partitions and an extended partition
    covering 8GB. The exended partition is of size 3GB with one logical
    drive of size 2GB. The disk will show only 1GB as free - the
    1GB of free space in the extended partition.

Example:

    LIST DISK
.

MessageId=10034
SymbolicName=MSG_COMMAND_LIST_PARTITION
Severity=Informational
Facility=System
Language=English
    Displays a list of partitions in the partition table for the selected disk.

Syntax:  LIST PARTITION

    On dynamic disks, the partitions do not neccessarily correspond to the
    dynamic volumes on the disk. Partitions may not be created or deleted
    on dynamic disks.

    A disk must be selected for this operation to succeed.

Example:

    LIST PARTITION
.
Language=Chinese
    Displays a list of partitions in the partition table for the selected disk.

Syntax:  LIST PARTITION

    On dynamic disks, the partitions do not neccessarily correspond to the
    dynamic volumes on the disk. Partitions may not be created or deleted
    on dynamic disks.

    A disk must be selected for this operation to succeed.

Example:

    LIST PARTITION
.


MessageId=10035
SymbolicName=MSG_COMMAND_LIST_VOLUME
Severity=Informational
Facility=System
Language=English
    Displays a list of basic and dynamic volumes which are installed on the local
    machine.

Syntax:  LIST VOLUME

Example:

    LIST VOLUME
.
Language=Chinese
    Displays a list of basic and dynamic volumes which are installed on the local
    machine.

Syntax:  LIST VOLUME

Example:

    LIST VOLUME
.


MessageId=10036
SymbolicName=MSG_COMMAND_LIST_VDISK
Severity=Informational
Facility=System
Language=English
<Add LIST VDISK command help text here>
.
Language=Chinese
<Add LIST VDISK command help text here>
.


MessageId=10037
SymbolicName=MSG_COMMAND_MERGE
Severity=Informational
Facility=System
Language=English
<Add MERGE command help text here>
.
Language=Chinese
<Add MERGE command help text here>
.


MessageId=10038
SymbolicName=MSG_COMMAND_OFFLINE
Severity=Informational
Facility=System
Language=English
<Add OFFLINE command help text here>
.
Language=Chinese
<Add OFFLINE command help text here>
.


MessageId=10039
SymbolicName=MSG_COMMAND_ONLINE
Severity=Informational
Facility=System
Language=English
<Add ONLINE command help text here>
.
Language=Chinese
<Add ONLINE command help text here>
.


MessageId=10040
SymbolicName=MSG_COMMAND_RECOVER
Severity=Informational
Facility=System
Language=English
<Add RECOVER command help text here>
.
Language=Chinese
<Add RECOVER command help text here>
.


MessageId=10041
SymbolicName=MSG_COMMAND_REM
Severity=Informational
Facility=System
Language=English
    Provides a way to add comments to a script.

Syntax:  REM

Example:

    In this example script, REM is used to provide a comment about what the
    script does.

    REM These commands set up 3 drives.
    CREATE PARTITION PRIMARY SIZE=2048
    ASSIGN d:
    CREATE PARTITION EXTEND
    CREATE PARTITION LOGICAL SIZE=2048
    ASSIGN e:
    CREATE PARTITION LOGICAL
    ASSIGN f:
.
Language=Chinese
    Provides a way to add comments to a script.

Syntax:  REM

Example:

    In this example script, REM is used to provide a comment about what the
    script does.

    REM These commands set up 3 drives.
    CREATE PARTITION PRIMARY SIZE=2048
    ASSIGN d:
    CREATE PARTITION EXTEND
    CREATE PARTITION LOGICAL SIZE=2048
    ASSIGN e:
    CREATE PARTITION LOGICAL
    ASSIGN f:
.


MessageId=10042
SymbolicName=MSG_COMMAND_REMOVE
Severity=Informational
Facility=System
Language=English
    Removes a drive letter or mounted folder pathname from the volume
    with focus.

Syntax:  REMOVE [LETTER=<D> | MOUNT=<PATH> | ALL] [DISMOUNT] [NOERR]

    LETTER=<D>  The drive letter to be removed.

    MOUNT=<PATH>
                The mounted folder path to be removed.

    ALL         Removes all current drive letters and mounted folder paths.

    DISMOUNT    This parameter may be used if 1) all drive letters and
                mounted folder paths have been removed from the volume, or
                2) the ALL parameter is specified.  This parameter
                specifies that the file system is to be dismounted and
                the volume taken offline. If other processes are using
                the volume, DiskPart closes any open handles before
                dismounting the file system and taking the volume offline.
                You can bring the volume online by assigning it a driver
                letter, by creating a mounted folder path to the volume, or
                by using the ONLINE command. DISMOUNT will fail if used
                on a volume that has any remaining drive letters or mounted
                folder paths. For scripting, using REMOVE ALL DISMOUNT is
                recommended.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    If no drive letter or mounted folder path is specified, DiskPart removes
    the first drive letter or mounted folder path it encounters. If the ALL
    parameter is used, all current drive letters and mounted folder paths
    are removed. If the DISMOUNT parameter is used, DiskPart closes all open
    handles to the volume and then dismounts the volume and takes it offline.

    The REMOVE command can be used to change the drive letter associated with a
    removable drive. You cannot remove the drive letters on boot or
    paging volumes.

    A volume must be selected for this operation to succeed.

Example:

    REMOVE LETTER=K
    REMOVE MOUNT=G:\\MountH
    REMOVE ALL DISMOUNT
.
Language=Chinese
    Removes a drive letter or mounted folder pathname from the volume
    with focus.

Syntax:  REMOVE [LETTER=<D> | MOUNT=<PATH> | ALL] [DISMOUNT] [NOERR]

    LETTER=<D>  The drive letter to be removed.

    MOUNT=<PATH>
                The mounted folder path to be removed.

    ALL         Removes all current drive letters and mounted folder paths.

    DISMOUNT    This parameter may be used if 1) all drive letters and
                mounted folder paths have been removed from the volume, or
                2) the ALL parameter is specified.  This parameter
                specifies that the file system is to be dismounted and
                the volume taken offline. If other processes are using
                the volume, DiskPart closes any open handles before
                dismounting the file system and taking the volume offline.
                You can bring the volume online by assigning it a driver
                letter, by creating a mounted folder path to the volume, or
                by using the ONLINE command. DISMOUNT will fail if used
                on a volume that has any remaining drive letters or mounted
                folder paths. For scripting, using REMOVE ALL DISMOUNT is
                recommended.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    If no drive letter or mounted folder path is specified, DiskPart removes
    the first drive letter or mounted folder path it encounters. If the ALL
    parameter is used, all current drive letters and mounted folder paths
    are removed. If the DISMOUNT parameter is used, DiskPart closes all open
    handles to the volume and then dismounts the volume and takes it offline.

    The REMOVE command can be used to change the drive letter associated with a
    removable drive. You cannot remove the drive letters on boot or
    paging volumes.

    A volume must be selected for this operation to succeed.

Example:

    REMOVE LETTER=K
    REMOVE MOUNT=G:\\MountH
    REMOVE ALL DISMOUNT
.


MessageId=10043
SymbolicName=MSG_COMMAND_REPAIR
Severity=Informational
Facility=System
Language=English
<Add REPAIR command help text here>
.
Language=Chinese
<Add REPAIR command help text here>
.


MessageId=10044
SymbolicName=MSG_COMMAND_RESCAN
Severity=Informational
Facility=System
Language=English
    Locates new disks that may have been added to the computer.

Syntax:  RESCAN

Example:

    RESCAN
.
Language=Chinese
    Locates new disks that may have been added to the computer.

Syntax:  RESCAN

Example:

    RESCAN
.


MessageId=10045
SymbolicName=MSG_COMMAND_RETAIN
Severity=Informational
Facility=System
Language=English
<Add RETAIN command help text here>
.
Language=Chinese
<Add RETAIN command help text here>
.


MessageId=10046
SymbolicName=MSG_COMMAND_SAN
Severity=Informational
Facility=System
Language=English
<Add SAN command help text here>
.
Language=Chinese
<Add SAN command help text here>
.


MessageId=10047
SymbolicName=MSG_COMMAND_SELECT_DISK
Severity=Informational
Facility=System
Language=English
    Selects the specified disk and shifts the focus to it.

Syntax:  SELECT DISK=<N>
         SELECT DISK=SYSTEM
         SELECT DISK=NEXT
         SELECT DISK=<Path>

    DISK=<N>
                The DiskPart disk index number of the disk to receive
                focus.

    DISK=<Path>
                The location path of the disk to receive focus.

    DISK=SYSTEM
                On BIOS machines, BIOS disk 0 will receive focus.
                On EFI machines, the disk containing the ESP partition
                used for the current boot will receive focus. On EFI
                machines, if there is no ESP, or there is more than
                one ESP present, or the machine is booted from Windows PE,
                the command will fail.

    DISK=NEXT
                Once a disk is selected, this command is used to iterate
                over all disks in the disk list. The next disk in the list
                will receive focus. If the next disk is the start of the
                enumeration, the command will fail and no disk will have
                focus.

    If no options are specified, the select command lists the disk that
    currently has the focus. You can view the DiskPart index numbers
    for all disks on the computer by using the LIST DISK command.

Example:

    SELECT DISK=1
    SELECT DISK=SYSTEM
    SELECT DISK=NEXT
    SELECT DISK=PCIROOT(0)#PCI(0100)#ATA(C00T00L01)
.
Language=Chinese
    Selects the specified disk and shifts the focus to it.

Syntax:  SELECT DISK=<N>
         SELECT DISK=SYSTEM
         SELECT DISK=NEXT
         SELECT DISK=<Path>

    DISK=<N>
                The DiskPart disk index number of the disk to receive
                focus.

    DISK=<Path>
                The location path of the disk to receive focus.

    DISK=SYSTEM
                On BIOS machines, BIOS disk 0 will receive focus.
                On EFI machines, the disk containing the ESP partition
                used for the current boot will receive focus. On EFI
                machines, if there is no ESP, or there is more than
                one ESP present, or the machine is booted from Windows PE,
                the command will fail.

    DISK=NEXT
                Once a disk is selected, this command is used to iterate
                over all disks in the disk list. The next disk in the list
                will receive focus. If the next disk is the start of the
                enumeration, the command will fail and no disk will have
                focus.

    If no options are specified, the select command lists the disk that
    currently has the focus. You can view the DiskPart index numbers
    for all disks on the computer by using the LIST DISK command.

Example:

    SELECT DISK=1
    SELECT DISK=SYSTEM
    SELECT DISK=NEXT
    SELECT DISK=PCIROOT(0)#PCI(0100)#ATA(C00T00L01)
.


MessageId=10048
SymbolicName=MSG_COMMAND_SELECT_PARTITION
Severity=Informational
Facility=System
Language=English
    Selects the specified partition and shifts the focus to it.

Syntax:  SELECT PARTITION=<N>

    PARTITION=<N>

                The number of the partition to receive the focus.

    If no partition is specified, the select command lists the current
    partition with focus. You can specify the partition by its number. You can
    view the numbers of all partitions on the current disk by using the list
    partition command.

    You must first select a disk using the DiskPart select disk command before
    you can select a partition.

Example:

    SELECT PARTITION=1
.
Language=Chinese
    Selects the specified partition and shifts the focus to it.

Syntax:  SELECT PARTITION=<N>

    PARTITION=<N>

                The number of the partition to receive the focus.

    If no partition is specified, the select command lists the current
    partition with focus. You can specify the partition by its number. You can
    view the numbers of all partitions on the current disk by using the list
    partition command.

    You must first select a disk using the DiskPart select disk command before
    you can select a partition.

Example:

    SELECT PARTITION=1
.


MessageId=10049
SymbolicName=MSG_COMMAND_SELECT_VOLUME
Severity=Informational
Facility=System
Language=English
    Selects the specified volume and shifts the focus to it.

Syntax:  SELECT VOLUME={<N> | <D>}

    VOLUME=<N>  The number of the volume to receive the focus.

    VOLUME=<D>  The drive letter or mounted folder path of the volume
                to receive the focus.

    If no volume is specified, the select command lists the current volume with
    focus. You can specify the volume by number, drive letter, or mounted folder
    path. On a basic disk, selecting a volume also gives the corresponding
    partition focus. You can view the numbers of all volumes on the computer by
    using the list volume command.

Example:

    SELECT VOLUME=1
    SELECT VOLUME=C
    SELECT VOLUME=C:\\MountH
.
Language=Chinese
    Selects the specified volume and shifts the focus to it.

Syntax:  SELECT VOLUME={<N> | <D>}

    VOLUME=<N>  The number of the volume to receive the focus.

    VOLUME=<D>  The drive letter or mounted folder path of the volume
                to receive the focus.

    If no volume is specified, the select command lists the current volume with
    focus. You can specify the volume by number, drive letter, or mounted folder
    path. On a basic disk, selecting a volume also gives the corresponding
    partition focus. You can view the numbers of all volumes on the computer by
    using the list volume command.

Example:

    SELECT VOLUME=1
    SELECT VOLUME=C
    SELECT VOLUME=C:\\MountH
.


MessageId=10050
SymbolicName=MSG_COMMAND_SELECT_VDISK
Severity=Informational
Facility=System
Language=English
<Add SELECT VDISK command help text here>
.
Language=Chinese
<Add SELECT VDISK command help text here>
.


MessageId=10051
SymbolicName=MSG_COMMAND_SETID
Severity=Informational
Facility=System
Language=English
    Changes the partition type field for the partition with focus.

Syntax:  SET ID={<BYTE> | <GUID>} [OVERRIDE] [NOERR]

    ID={<BYTE> | <GUID>}

                Specifies the new partition type.
                For master boot record (MBR) disks, you can specify a partition
                type byte, in hexadecimal form, for the partition. Any
                partition type byte can be specified with this parameter except
                for type 0x42 (LDM partition). Note that the leading '0x' is 
                omitted when specifying the hexadecimal partition type.

                For GUID partition table (GPT) disks you can specify a
                partition type GUID for the partition. Recognized GUIDs
                include:

                    EFI System partition:
                        c12a7328-f81f-11d2-ba4b-00a0c93ec93b

                    Basic data partition:
                        ebd0a0a2-b9e5-4433-87c0-68b6b72699c7

                Any partition type GUID can be specified with this parameter
                except for the following:

                    Microsoft Reserved partition:
                        e3c9e316-0b5c-4db8-817d-f92df00215a

                    LDM Metadata partition on a dynamic disk:
                        5808c8aa-7e8f-42e0-85d2-e1e90434cfb3

                    LDM Data partition on a dynamic disk:
                        af9b60a0-1431-4f62-bc68-3311714a69ad

                    Cluster Metadata partition:
                        db97dba9-0840-4bae-97f0-ffb9a327c7e1


                Other than the limitations mentioned, DiskPart otherwise does
                not check the partition type for validity except to ensure that
                it is a byte in hexadecimal form or a GUID.

    OVERRIDE    Enables DiskPart to force the file system on the volume to
                dismount before changing the partition type. When changing
                the partition type, DiskPart will attempt to lock and dismount
                the file system on the volume. If this parameter is not specified,
                and the call to lock the file system fails, (because some other
                application has an open handle to the volume), the entire
                operation will fail. When this parameter is specified, the
                dismount is forced even if the call to lock the file system
                fails. When a file system is dismounted, all opened handles to
                the volume will become invalid.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    Intended for Original Equipment Manufacturer (OEM) use only.

    A partition must be selected for this operation to succeed.

    Caution:

        Changing partition type fields with this parameter might cause your
        computer to fail or be unable to start up. Unless you are an OEM or an
        IT professional experienced with GPT disks, do not change partition
        type fields on GPT disks using this parameter. Instead, always use the
        CREATE PARTITION EFI command to create EFI System partitions, the
        CREATE PARTITION MSR command to create Microsoft Reserved partitions,
        and the CREATE PARTITION PRIMARY command without the ID parameter to
        create primary partitions on GPT disks.

    This command does not work on dynamic disks nor on Microsoft Reserved
    partitions.

Example:
    SET ID=07 OVERRIDE
    SET ID=ebd0a0a2-b9e5-4433-87c0-68b6b72699c7
.
Language=Chinese
    Changes the partition type field for the partition with focus.

Syntax:  SET ID={<BYTE> | <GUID>} [OVERRIDE] [NOERR]

    ID={<BYTE> | <GUID>}

                Specifies the new partition type.
                For master boot record (MBR) disks, you can specify a partition
                type byte, in hexadecimal form, for the partition. Any
                partition type byte can be specified with this parameter except
                for type 0x42 (LDM partition). Note that the leading '0x' is 
                omitted when specifying the hexadecimal partition type.

                For GUID partition table (GPT) disks you can specify a
                partition type GUID for the partition. Recognized GUIDs
                include:

                    EFI System partition:
                        c12a7328-f81f-11d2-ba4b-00a0c93ec93b

                    Basic data partition:
                        ebd0a0a2-b9e5-4433-87c0-68b6b72699c7

                Any partition type GUID can be specified with this parameter
                except for the following:

                    Microsoft Reserved partition:
                        e3c9e316-0b5c-4db8-817d-f92df00215a

                    LDM Metadata partition on a dynamic disk:
                        5808c8aa-7e8f-42e0-85d2-e1e90434cfb3

                    LDM Data partition on a dynamic disk:
                        af9b60a0-1431-4f62-bc68-3311714a69ad

                    Cluster Metadata partition:
                        db97dba9-0840-4bae-97f0-ffb9a327c7e1


                Other than the limitations mentioned, DiskPart otherwise does
                not check the partition type for validity except to ensure that
                it is a byte in hexadecimal form or a GUID.

    OVERRIDE    Enables DiskPart to force the file system on the volume to
                dismount before changing the partition type. When changing
                the partition type, DiskPart will attempt to lock and dismount
                the file system on the volume. If this parameter is not specified,
                and the call to lock the file system fails, (because some other
                application has an open handle to the volume), the entire
                operation will fail. When this parameter is specified, the
                dismount is forced even if the call to lock the file system
                fails. When a file system is dismounted, all opened handles to
                the volume will become invalid.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    Intended for Original Equipment Manufacturer (OEM) use only.

    A partition must be selected for this operation to succeed.

    Caution:

        Changing partition type fields with this parameter might cause your
        computer to fail or be unable to start up. Unless you are an OEM or an
        IT professional experienced with GPT disks, do not change partition
        type fields on GPT disks using this parameter. Instead, always use the
        CREATE PARTITION EFI command to create EFI System partitions, the
        CREATE PARTITION MSR command to create Microsoft Reserved partitions,
        and the CREATE PARTITION PRIMARY command without the ID parameter to
        create primary partitions on GPT disks.

    This command does not work on dynamic disks nor on Microsoft Reserved
    partitions.

Example:
    SET ID=07 OVERRIDE
    SET ID=ebd0a0a2-b9e5-4433-87c0-68b6b72699c7
.


MessageId=10052
SymbolicName=MSG_COMMAND_SHRINK
Severity=Informational
Facility=System
Language=English
<Add SHRINK command help text here>
.
Language=Chinese
<Add SHRINK command help text here>
.


MessageId=10053
SymbolicName=MSG_COMMAND_UNIQUEID_DISK
Severity=Informational
Facility=System
Language=English
    Displays or sets the GUID partition table (GPT) identifier or master boot
    record (MBR) signature for the disk with focus.

Syntax:  UNIQUEID DISK [ID={<DWORD> | <GUID>}]  [NOERR]

    ID={<DWORD> | <GUID>}

                For MBR disks, you can specify a four-byte (DWORD) value in
                hexadecimal form for the signature.

                For GPT disks, specify a GUID for the identifier.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    A disk must be selected for this operation to succeed.  This command works
    on basic and dynamic disks.

Example:

    UNIQUEID DISK
    UNIQUEID DISK ID=5f1b2c36
    UNIQUEID DISK ID=baf784e7-6bbd-4cfb-aaac-e86c96e166ee
.
Language=Chinese
    Displays or sets the GUID partition table (GPT) identifier or master boot
    record (MBR) signature for the disk with focus.

Syntax:  UNIQUEID DISK [ID={<DWORD> | <GUID>}]  [NOERR]

    ID={<DWORD> | <GUID>}

                For MBR disks, you can specify a four-byte (DWORD) value in
                hexadecimal form for the signature.

                For GPT disks, specify a GUID for the identifier.

    NOERR       For scripting only. When an error is encountered, DiskPart
                continues to process commands as if the error did not occur.
                Without the NOERR parameter, an error causes DiskPart to exit
                with an error code.

    A disk must be selected for this operation to succeed.  This command works
    on basic and dynamic disks.

Example:

    UNIQUEID DISK
    UNIQUEID DISK ID=5f1b2c36
    UNIQUEID DISK ID=baf784e7-6bbd-4cfb-aaac-e86c96e166ee
.
