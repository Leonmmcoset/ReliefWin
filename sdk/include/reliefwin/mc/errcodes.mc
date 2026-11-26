
;
;// IMPORTANT: When a new language is added, all messages in this file need to be
;// either translated or at least duplicated for the new language.
;// This is a new requirement by MS mc.exe
;// To do this, start with a regex replace:
;// - In VS IDE: "Language=English\r\n(?<String>(?:[^\.].*\r\n)*\.\r\n)" -> "Language=English\r\n${String}Language=MyLanguage\r\n${String}"
;

MessageIdTypedef=ULONG

SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )

FacilityNames=(System=0x0:FACILITY_SYSTEM
               ITF=0x4:FACILITY_ITF
               WIN32=0x7:FACILITY_GENERAL
               CERT=0xB:FACILITY_CERT
              )

LanguageNames=(English=0x409:MSG00409
               Japanese=0x411:MSG00411)

MessageId=0
Severity=Success
Facility=System
SymbolicName=ERROR_SUCCESS
Language=English
The operation completed successfully.
.
Language=Japanese
操作は正常に完了しました。
.

MessageId=1
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_FUNCTION
Language=English
Incorrect function.
.
Language=Japanese
関数が正しくありません。
.

MessageId=2
Severity=Success
Facility=System
SymbolicName=ERROR_FILE_NOT_FOUND
Language=English
The system cannot find the file specified.
.
Language=Japanese
指定されたファイルが見つかりません。
.

MessageId=3
Severity=Success
Facility=System
SymbolicName=ERROR_PATH_NOT_FOUND
Language=English
The system cannot find the path specified.
.
Language=Japanese
指定されたパスが見つかりません。
.

MessageId=4
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_OPEN_FILES
Language=English
The system cannot open the file.
.
Language=Japanese
システムがファイルを開けません。
.

MessageId=5
Severity=Success
Facility=System
SymbolicName=ERROR_ACCESS_DENIED
Language=English
Access is denied.
.
Language=Japanese
アクセスが拒否されました。
.

MessageId=6
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_HANDLE
Language=English
The handle is invalid.
.
Language=Japanese
ハンドルが無効です。
.

MessageId=7
Severity=Success
Facility=System
SymbolicName=ERROR_ARENA_TRASHED
Language=English
The storage control blocks were destroyed.
.
Language=Japanese
ストレージ制御ブロックが破壊されました。
.

MessageId=8
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_ENOUGH_MEMORY
Language=English
Not enough storage is available to process this command.
.
Language=Japanese
このコマンドを処理するのに十分なメモリ リソースがありません。
.

MessageId=9
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_BLOCK
Language=English
The storage control block address is invalid.
.
Language=Japanese
ストレージ コントロール ブロック アドレスが無効です。
.

MessageId=10
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_ENVIRONMENT
Language=English
The environment is incorrect.
.
Language=Japanese
環境が不正です。
.

MessageId=11
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_FORMAT
Language=English
An attempt was made to load a program with an incorrect format.
.
Language=Japanese
不正なフォーマットでプログラムをロードしようとしました。
.

MessageId=12
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_ACCESS
Language=English
The access code is invalid.
.
Language=Japanese
アクセス コードが無効です。
.

MessageId=13
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_DATA
Language=English
The data is invalid.
.
Language=Japanese
データが無効です。
.

MessageId=14
Severity=Success
Facility=System
SymbolicName=ERROR_OUTOFMEMORY
Language=English
Not enough storage is available to complete this operation.
.
Language=Japanese
この操作を完了するのに十分なメモリ リソースがありません。
.

MessageId=15
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_DRIVE
Language=English
The system cannot find the drive specified.
.
Language=Japanese
指定されたドライブが見つかりません。
.

MessageId=16
Severity=Success
Facility=System
SymbolicName=ERROR_CURRENT_DIRECTORY
Language=English
The directory cannot be removed.
.
Language=Japanese
ディレクトリを削除できません。
.

MessageId=17
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_SAME_DEVICE
Language=English
The system cannot move the file to a different disk drive.
.
Language=Japanese
システムはファイルを別のディスク ドライブに移動できません。
.

MessageId=18
Severity=Success
Facility=System
SymbolicName=ERROR_NO_MORE_FILES
Language=English
There are no more files.
.
Language=Japanese
ファイルがありません。
.

MessageId=19
Severity=Success
Facility=System
SymbolicName=ERROR_WRITE_PROTECT
Language=English
The media is write protected.
.
Language=Japanese
メディアが書き込み保護されています。
.

MessageId=20
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_UNIT
Language=English
The system cannot find the device specified.
.
Language=Japanese
指定されたデバイスが見つかりません。
.

MessageId=21
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_READY
Language=English
The device is not ready.
.
Language=Japanese
デバイスの準備ができていません。
.

MessageId=22
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_COMMAND
Language=English
The device does not recognize the command.
.
Language=Japanese
デバイスがコマンドを認識しません。
.

MessageId=23
Severity=Success
Facility=System
SymbolicName=ERROR_CRC
Language=English
Data error (cyclic redundancy check).
.
Language=Japanese
データエラー (巡回冗長検査) です。
.

MessageId=24
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_LENGTH
Language=English
The program issued a command but the command length is incorrect.
.
Language=Japanese
プログラムがコマンドを発行しましたが、コマンド長が正しくありません。
.

MessageId=25
Severity=Success
Facility=System
SymbolicName=ERROR_SEEK
Language=English
The drive cannot locate a specific area or track on the disk.
.
Language=Japanese
ドライブがディスク上の特定の領域またはトラックを検索できません。
.

MessageId=26
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_DOS_DISK
Language=English
The specified disk or diskette cannot be accessed.
.
Language=Japanese
指定されたディスクまたはディスケットにアクセスできません。
.

MessageId=27
Severity=Success
Facility=System
SymbolicName=ERROR_SECTOR_NOT_FOUND
Language=English
The drive cannot find the sector requested.
.
Language=Japanese
ドライブが要求されたセクタを見つけることができません。
.

MessageId=28
Severity=Success
Facility=System
SymbolicName=ERROR_OUT_OF_PAPER
Language=English
The printer is out of paper.
.
Language=Japanese
プリンタの用紙がありません。
.

MessageId=29
Severity=Success
Facility=System
SymbolicName=ERROR_WRITE_FAULT
Language=English
The system cannot write to the specified device.
.
Language=Japanese
システムは指定されたデバイスに書き込めません。
.

MessageId=30
Severity=Success
Facility=System
SymbolicName=ERROR_READ_FAULT
Language=English
The system cannot read from the specified device.
.
Language=Japanese
指定されたデバイスから読み取ることができません。
.

MessageId=31
Severity=Success
Facility=System
SymbolicName=ERROR_GEN_FAILURE
Language=English
A device attached to the system is not functioning.
.
Language=Japanese
システムに接続されたデバイスが機能していません。
.

MessageId=32
Severity=Success
Facility=System
SymbolicName=ERROR_SHARING_VIOLATION
Language=English
The process cannot access the file because it is being used by another process.
.
Language=Japanese
他のプロセスによって使用されているため、プロセスはファイルにアクセスできません。
.

MessageId=33
Severity=Success
Facility=System
SymbolicName=ERROR_LOCK_VIOLATION
Language=English
The process cannot access the file because another process has locked a portion of the file.
.
Language=Japanese
他のプロセスがファイルの一部をロックしたため、プロセスがファイルにアクセスできません。
.

MessageId=34
Severity=Success
Facility=System
SymbolicName=ERROR_WRONG_DISK
Language=English
The wrong diskette is in the drive. Insert %2 (Volume Serial Number: %3) into drive %1.
.
Language=Japanese
誤ったディスケットがドライブに入っています。ドライブ %1 に %2 (ボリューム シリアル番号: %3)  を挿入してください。
.

MessageId=36
Severity=Success
Facility=System
SymbolicName=ERROR_SHARING_BUFFER_EXCEEDED
Language=English
Too many files opened for sharing.
.
Language=Japanese
共有で開いているファイルが多すぎます。
.

MessageId=38
Severity=Success
Facility=System
SymbolicName=ERROR_HANDLE_EOF
Language=English
Reached the end of the file.
.
Language=Japanese
ファイルの最後に到達しました。
.

MessageId=39
Severity=Success
Facility=System
SymbolicName=ERROR_HANDLE_DISK_FULL
Language=English
The disk is full.
.
Language=Japanese
ディスクがいっぱいです。
.

MessageId=50
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_SUPPORTED
Language=English
The request is not supported.
.
Language=Japanese
そのリクエストはサポートされていません。
.

MessageId=51
Severity=Success
Facility=System
SymbolicName=ERROR_REM_NOT_LIST
Language=English
ReliefWin cannot find the network path. Verify that the network path is correct and the destination computer is not busy or turned off. If ReliefWin still cannot find the network path, contact your network administrator.
.
Language=Japanese
ReliefWinでネットワークパスが見つかりません。ネットワークパスが正しく、宛先コンピュータがビジー状態または電源オフになっていないことを確認します。それでもReliefWinがネットワークパスを見つけられない場合は、ネットワーク管理者に連絡してください。
.

MessageId=52
Severity=Success
Facility=System
SymbolicName=ERROR_DUP_NAME
Language=English
You were not connected because a duplicate name exists on the network. Go to System in the Control Panel to change the computer name and try again.
.
Language=Japanese
ネットワーク上に重複した名前が存在するため、接続できませんでした。ドメインに参加している場合は、コントロールパネルの[システム]でコンピュータ名を変更し、再試行してください。ワークグループに参加している場合は、別のワークグループ名を選択してください。
.

MessageId=53
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_NETPATH
Language=English
The network path was not found.
.
Language=Japanese
ネットワークパスが見つかりませんでした。
.

MessageId=54
Severity=Success
Facility=System
SymbolicName=ERROR_NETWORK_BUSY
Language=English
The network is busy.
.
Language=Japanese
ネットワークがビジーです。
.

MessageId=55
Severity=Success
Facility=System
SymbolicName=ERROR_DEV_NOT_EXIST
Language=English
The specified network resource or device is no longer available.
.
Language=Japanese
指定されたネットワークリソースまたはデバイスは使用できなくなりました。
.

MessageId=56
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_CMDS
Language=English
The network BIOS command limit has been reached.
.
Language=Japanese
ネットワーク BIOS コマンドの上限に達しました。
.

MessageId=57
Severity=Success
Facility=System
SymbolicName=ERROR_ADAP_HDW_ERR
Language=English
A network adapter hardware error occurred.
.
Language=Japanese
ネットワーク アダプタのハードウェア エラーが発生しました。
.

MessageId=58
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_NET_RESP
Language=English
The specified server cannot perform the requested operation.
.
Language=Japanese
指定されたサーバーは要求された操作を実行できません。
.

MessageId=59
Severity=Success
Facility=System
SymbolicName=ERROR_UNEXP_NET_ERR
Language=English
An unexpected network error occurred.
.
Language=Japanese
予期しないネットワーク エラーが発生しました。
.

MessageId=60
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_REM_ADAP
Language=English
The remote adapter is not compatible.
.
Language=Japanese
リモート アダプタに互換性がありません。
.

MessageId=61
Severity=Success
Facility=System
SymbolicName=ERROR_PRINTQ_FULL
Language=English
The printer queue is full.
.
Language=Japanese
プリンタキューがいっぱいです。
.

MessageId=62
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SPOOL_SPACE
Language=English
Space to store the file waiting to be printed is not available on the server.
.
Language=Japanese
印刷待機中のファイルを保存する領域がサーバーにありません。
.

MessageId=63
Severity=Success
Facility=System
SymbolicName=ERROR_PRINT_CANCELLED
Language=English
Your file waiting to be printed was deleted.
.
Language=Japanese
印刷待機中のファイルが削除されました。
.

MessageId=64
Severity=Success
Facility=System
SymbolicName=ERROR_NETNAME_DELETED
Language=English
The specified network name is no longer available.
.
Language=Japanese
指定されたネットワーク名は使用できなくなりました。
.

MessageId=65
Severity=Success
Facility=System
SymbolicName=ERROR_NETWORK_ACCESS_DENIED
Language=English
Network access is denied.
.
Language=Japanese
ネットワークアクセスが拒否されました。
.

MessageId=66
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_DEV_TYPE
Language=English
The network resource type is not correct.
.
Language=Japanese
ネットワークリソースのタイプが正しくありません。
.

MessageId=67
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_NET_NAME
Language=English
The network name cannot be found.
.
Language=Japanese
ネットワーク名が見つかりません。
.

MessageId=68
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_NAMES
Language=English
The name limit for the local computer network adapter card was exceeded.
.
Language=Japanese
ローカル コンピュータのネットワーク アダプタ カードの名前の制限を超えました。
.

MessageId=69
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_SESS
Language=English
The network BIOS session limit was exceeded.
.
Language=Japanese
ネットワークBIOSセッションの制限を超えました。
.

MessageId=70
Severity=Success
Facility=System
SymbolicName=ERROR_SHARING_PAUSED
Language=English
The remote server has been paused or is in the process of being started.
.
Language=Japanese
リモートサーバーが一時停止されたか、起動中です。
.

MessageId=71
Severity=Success
Facility=System
SymbolicName=ERROR_REQ_NOT_ACCEP
Language=English
No more connections can be made to this remote computer at this time because there are already as many connections as the computer can accept.
.
Language=Japanese
このコンピュータが受け入れられるだけの接続がすでにあるため、現時点ではこのリモートコンピュータにこれ以上接続できません。
.

MessageId=72
Severity=Success
Facility=System
SymbolicName=ERROR_REDIR_PAUSED
Language=English
The specified printer or disk device has been paused.
.
Language=Japanese
指定されたプリンタまたはディスクデバイスが一時停止しました。
.

MessageId=80
Severity=Success
Facility=System
SymbolicName=ERROR_FILE_EXISTS
Language=English
The file exists.
.
Language=Japanese
ファイルが存在します。
.

MessageId=82
Severity=Success
Facility=System
SymbolicName=ERROR_CANNOT_MAKE
Language=English
The directory or file cannot be created.
.
Language=Japanese
ディレクトリまたはファイルを作成できません。
.

MessageId=83
Severity=Success
Facility=System
SymbolicName=ERROR_FAIL_I24
Language=English
Fail on INT 24.
.
Language=Japanese
INT 24で失敗しました。
.

MessageId=84
Severity=Success
Facility=System
SymbolicName=ERROR_OUT_OF_STRUCTURES
Language=English
Storage to process this request is not available.
.
Language=Japanese
この要求を処理するストレージがありません。
.

MessageId=85
Severity=Success
Facility=System
SymbolicName=ERROR_ALREADY_ASSIGNED
Language=English
The local device name is already in use.
.
Language=Japanese
ローカルデバイス名はすでに使用中です。
.

MessageId=86
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PASSWORD
Language=English
The specified network password is not correct.
.
Language=Japanese
指定されたネットワークパスワードが正しくありません。
.

MessageId=87
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PARAMETER
Language=English
The parameter is incorrect.
.
Language=Japanese
パラメータが不正です。
.

MessageId=88
Severity=Success
Facility=System
SymbolicName=ERROR_NET_WRITE_FAULT
Language=English
A write fault occurred on the network.
.
Language=Japanese
ネットワークで書き込みエラーが発生しました。
.

MessageId=89
Severity=Success
Facility=System
SymbolicName=ERROR_NO_PROC_SLOTS
Language=English
The system cannot start another process at this time.
.
Language=Japanese
システムは現時点で別のプロセスを開始できません。
.

MessageId=100
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_SEMAPHORES
Language=English
Cannot create another system semaphore.
.
Language=Japanese
別のシステム セマフォを作成できません。
.

MessageId=101
Severity=Success
Facility=System
SymbolicName=ERROR_EXCL_SEM_ALREADY_OWNED
Language=English
The exclusive semaphore is owned by another process.
.
Language=Japanese
排他セマフォが他のプロセスによって所有されています。
.

MessageId=102
Severity=Success
Facility=System
SymbolicName=ERROR_SEM_IS_SET
Language=English
The semaphore is set and cannot be closed.
.
Language=Japanese
セマフォが設定されていて閉じることができない
.

MessageId=103
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_SEM_REQUESTS
Language=English
The semaphore cannot be set again.
.
Language=Japanese
セマフォを再び設定できません。
.

MessageId=104
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_AT_INTERRUPT_TIME
Language=English
Cannot request exclusive semaphores at interrupt time.
.
Language=Japanese
割り込み時に排他セマフォを要求できません。
.

MessageId=105
Severity=Success
Facility=System
SymbolicName=ERROR_SEM_OWNER_DIED
Language=English
The previous ownership of this semaphore has ended.
.
Language=Japanese
このセマフォの以前の所有権は終了しました。
.

MessageId=106
Severity=Success
Facility=System
SymbolicName=ERROR_SEM_USER_LIMIT
Language=English
Insert the diskette for drive %1.
.
Language=Japanese
ドライブ %1 用のディスケットを挿入してください。
.

MessageId=107
Severity=Success
Facility=System
SymbolicName=ERROR_DISK_CHANGE
Language=English
The program stopped because an alternate diskette was not inserted.
.
Language=Japanese
代替ディスケットが挿入されていないため、プログラムが停止しました。
.

MessageId=108
Severity=Success
Facility=System
SymbolicName=ERROR_DRIVE_LOCKED
Language=English
The disk is in use or locked by another process.
.
Language=Japanese
ディスクが使用中か、他のプロセスによってロックされています。
.

MessageId=109
Severity=Success
Facility=System
SymbolicName=ERROR_BROKEN_PIPE
Language=English
The pipe has been ended.
.
Language=Japanese
パイプが終了しました。
.

MessageId=110
Severity=Success
Facility=System
SymbolicName=ERROR_OPEN_FAILED
Language=English
The system cannot open the device or file specified.
.
Language=Japanese
システムは指定されたデバイスまたはファイルを開けません。
.

MessageId=111
Severity=Success
Facility=System
SymbolicName=ERROR_BUFFER_OVERFLOW
Language=English
The file name is too long.
.
Language=Japanese
ファイル名が長すぎます。
.

MessageId=112
Severity=Success
Facility=System
SymbolicName=ERROR_DISK_FULL
Language=English
There is not enough space on the disk.
.
Language=Japanese
ディスクに十分な空き容量がありません。
.

MessageId=113
Severity=Success
Facility=System
SymbolicName=ERROR_NO_MORE_SEARCH_HANDLES
Language=English
No more internal file identifiers available.
.
Language=Japanese
使用可能な内部ファイル識別子がありません。
.

MessageId=114
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_TARGET_HANDLE
Language=English
The target internal file identifier is incorrect.
.
Language=Japanese
対象の内部ファイル識別子が不正です。
.

MessageId=117
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_CATEGORY
Language=English
The IOCTL call made by the application program is not correct.
.
Language=Japanese
アプリケーション プログラムが行った IOCTL コールが正しくありません。
.

MessageId=118
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_VERIFY_SWITCH
Language=English
The verify-on-write switch parameter value is not correct.
.
Language=Japanese
ベリファイ オン ライトのスイッチパラメータ値が正しくありません。
.

MessageId=119
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_DRIVER_LEVEL
Language=English
The system does not support the command requested.
.
Language=Japanese
システムは要求されたコマンドをサポートしていません。
.

MessageId=120
Severity=Success
Facility=System
SymbolicName=ERROR_CALL_NOT_IMPLEMENTED
Language=English
This function is not supported on this system.
.
Language=Japanese
この機能はこのシステムではサポートされていません。
.

MessageId=121
Severity=Success
Facility=System
SymbolicName=ERROR_SEM_TIMEOUT
Language=English
The semaphore timeout period has expired.
.
Language=Japanese
セマフォのタイムアウト期間が切れました。
.

MessageId=122
Severity=Success
Facility=System
SymbolicName=ERROR_INSUFFICIENT_BUFFER
Language=English
The data area passed to a system call is too small.
.
Language=Japanese
システムコールに渡されたデータ領域が小さすぎます。
.

MessageId=123
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_NAME
Language=English
The filename, directory name, or volume label syntax is incorrect.
.
Language=Japanese
ファイル名、ディレクトリ名、またはボリューム ラベルの構文が正しくありません。
.

MessageId=124
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_LEVEL
Language=English
The system call level is not correct.
.
Language=Japanese
システム コール レベルが正しくありません。
.

MessageId=125
Severity=Success
Facility=System
SymbolicName=ERROR_NO_VOLUME_LABEL
Language=English
The disk has no volume label.
.
Language=Japanese
ディスクにボリューム ラベルがありません。
.

MessageId=126
Severity=Success
Facility=System
SymbolicName=ERROR_MOD_NOT_FOUND
Language=English
The specified module could not be found.
.
Language=Japanese
指定されたモジュールが見つかりません。
.

MessageId=127
Severity=Success
Facility=System
SymbolicName=ERROR_PROC_NOT_FOUND
Language=English
The specified procedure could not be found.
.
Language=Japanese
指定されたプロシージャが見つかりません。
.

MessageId=128
Severity=Success
Facility=System
SymbolicName=ERROR_WAIT_NO_CHILDREN
Language=English
There are no child processes to wait for.
.
Language=Japanese
待機する子プロセスがありません。
.

MessageId=129
Severity=Success
Facility=System
SymbolicName=ERROR_CHILD_NOT_COMPLETE
Language=English
The %1 application cannot be run in Win32 mode.
.
Language=Japanese
%1 アプリケーションは Win32 モードで実行できません。
.

MessageId=130
Severity=Success
Facility=System
SymbolicName=ERROR_DIRECT_ACCESS_HANDLE
Language=English
Attempt to use a file handle to an open disk partition for an operation other than raw disk I/O.
.
Language=Japanese
開いているディスク パーティションへのファイル ハンドルを生のディスク I/O 以外の操作に使用しようとしました。
.

MessageId=131
Severity=Success
Facility=System
SymbolicName=ERROR_NEGATIVE_SEEK
Language=English
An attempt was made to move the file pointer before the beginning of the file.
.
Language=Japanese
ファイルポインタをファイルの先頭より前に移動しようとしました。
.

MessageId=132
Severity=Success
Facility=System
SymbolicName=ERROR_SEEK_ON_DEVICE
Language=English
The file pointer cannot be set on the specified device or file.
.
Language=Japanese
指定されたデバイスまたはファイルにファイルポインタを設定できません。
.

MessageId=133
Severity=Success
Facility=System
SymbolicName=ERROR_IS_JOIN_TARGET
Language=English
A JOIN or SUBST command cannot be used for a drive that contains previously joined drives.
.
Language=Japanese
JOIN または SUBST コマンドは、以前に結合されたドライブを含むドライブには使用できません。
.

MessageId=134
Severity=Success
Facility=System
SymbolicName=ERROR_IS_JOINED
Language=English
An attempt was made to use a JOIN or SUBST command on a drive that has already been joined.
.
Language=Japanese
すでに結合されているドライブでJOINまたはSUBSTコマンドを使用しようとしました。
.

MessageId=135
Severity=Success
Facility=System
SymbolicName=ERROR_IS_SUBSTED
Language=English
An attempt was made to use a JOIN or SUBST command on a drive that has already been substituted.
.
Language=Japanese
既に置換されているドライブで JOIN コマンドまたは SUBST コマンドを使用しようとしました。
.

MessageId=136
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_JOINED
Language=English
The system tried to delete the JOIN of a drive that is not joined.
.
Language=Japanese
参加していないドライブの JOIN を削除しようとしました。
.

MessageId=137
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_SUBSTED
Language=English
The system tried to delete the substitution of a drive that is not substituted.
.
Language=Japanese
システムは、置換されていないドライブの置換を削除しようとしました。
.

MessageId=138
Severity=Success
Facility=System
SymbolicName=ERROR_JOIN_TO_JOIN
Language=English
The system tried to join a drive to a directory on a joined drive.
.
Language=Japanese
システムは、結合されたドライブ上のディレクトリにドライブを結合しようとしました。
.

MessageId=139
Severity=Success
Facility=System
SymbolicName=ERROR_SUBST_TO_SUBST
Language=English
The system tried to substitute a drive to a directory on a substituted drive.
.
Language=Japanese
システムは、置換されたドライブ上のディレクトリにドライブを置換しようとしました。
.

MessageId=140
Severity=Success
Facility=System
SymbolicName=ERROR_JOIN_TO_SUBST
Language=English
The system tried to join a drive to a directory on a substituted drive.
.
Language=Japanese
システムは、置換されたドライブ上のディレクトリにドライブを結合しようとしました。
.

MessageId=141
Severity=Success
Facility=System
SymbolicName=ERROR_SUBST_TO_JOIN
Language=English
The system tried to SUBST a drive to a directory on a joined drive.
.
Language=Japanese
システムは、結合されたドライブ上のディレクトリにドライブをサブストしようとしました。
.

MessageId=142
Severity=Success
Facility=System
SymbolicName=ERROR_BUSY_DRIVE
Language=English
The system cannot perform a JOIN or SUBST at this time.
.
Language=Japanese
システムは現在 JOIN または SUBST を実行できません。
.

MessageId=143
Severity=Success
Facility=System
SymbolicName=ERROR_SAME_DRIVE
Language=English
The system cannot join or substitute a drive to or for a directory on the same drive.
.
Language=Japanese
システムは、同じドライブ上のディレクトリに、または同じドライブ上のディレクトリを結合または置換できません。
.

MessageId=144
Severity=Success
Facility=System
SymbolicName=ERROR_DIR_NOT_ROOT
Language=English
The directory is not a subdirectory of the root directory.
.
Language=Japanese
ディレクトリはルート ディレクトリのサブディレクトリではありません。
.

MessageId=145
Severity=Success
Facility=System
SymbolicName=ERROR_DIR_NOT_EMPTY
Language=English
The directory is not empty.
.
Language=Japanese
ディレクトリが空ではありません。
.

MessageId=146
Severity=Success
Facility=System
SymbolicName=ERROR_IS_SUBST_PATH
Language=English
The path specified is being used in a substitute.
.
Language=Japanese
指定されたパスが置換で使用されています。
.

MessageId=147
Severity=Success
Facility=System
SymbolicName=ERROR_IS_JOIN_PATH
Language=English
Not enough resources are available to process this command.
.
Language=Japanese
このコマンドを処理するのに十分なリソースがありません。
.

MessageId=148
Severity=Success
Facility=System
SymbolicName=ERROR_PATH_BUSY
Language=English
The path specified cannot be used at this time.
.
Language=Japanese
指定されたパスは現在使用できません。
.

MessageId=149
Severity=Success
Facility=System
SymbolicName=ERROR_IS_SUBST_TARGET
Language=English
An attempt was made to join or substitute a drive for which a directory on the drive is the target of a previous substitute.
.
Language=Japanese
ドライブ上のディレクトリが以前の置換の対象であるドライブに参加または置換しようとしました。
.

MessageId=150
Severity=Success
Facility=System
SymbolicName=ERROR_SYSTEM_TRACE
Language=English
System trace information was not specified in your CONFIG.SYS file, or tracing is disallowed.
.
Language=Japanese
システム トレース情報がCONFIG.SYSファイルで指定されていないか、トレースが許可されていません。
.

MessageId=151
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_EVENT_COUNT
Language=English
The number of specified semaphore events for DosMuxSemWait is not correct.
.
Language=Japanese
DosMuxSemWaitの指定されたセマフォ イベント数が正しくありません。
.

MessageId=152
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_MUXWAITERS
Language=English
DosMuxSemWait did not execute; too many semaphores are already set.
.
Language=Japanese
DosMuxSemWaitが実行されませんでした。すでに設定されているセマフォが多すぎます。
.

MessageId=153
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_LIST_FORMAT
Language=English
The DosMuxSemWait list is not correct.
.
Language=Japanese
DosMuxSemWaitリストが正しくありません。
.

MessageId=154
Severity=Success
Facility=System
SymbolicName=ERROR_LABEL_TOO_LONG
Language=English
The volume label you entered exceeds the label character limit of the target file system.
.
Language=Japanese
入力したボリュームラベルがターゲットファイルシステムのラベル文字数制限を超えています。
.

MessageId=155
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_TCBS
Language=English
Cannot create another thread.
.
Language=Japanese
別のスレッドを作成できません。
.

MessageId=156
Severity=Success
Facility=System
SymbolicName=ERROR_SIGNAL_REFUSED
Language=English
The recipient process has refused the signal.
.
Language=Japanese
受け側のプロセスがシグナルを拒否しました。
.

MessageId=157
Severity=Success
Facility=System
SymbolicName=ERROR_DISCARDED
Language=English
The segment is already discarded and cannot be locked.
.
Language=Japanese
セグメントはすでに破棄されており、ロックできません。
.

MessageId=158
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_LOCKED
Language=English
The segment is already unlocked.
.
Language=Japanese
セグメントはすでにアンロックされています。
.

MessageId=159
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_THREADID_ADDR
Language=English
The address for the thread ID is not correct.
.
Language=Japanese
スレッド ID のアドレスが正しくありません。
.

MessageId=160
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_ARGUMENTS
Language=English
The argument string passed to DosExecPgm is not correct.
.
Language=Japanese
1つ以上の引数が正しくありません。
.

MessageId=161
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_PATHNAME
Language=English
The specified path is invalid.
.
Language=Japanese
指定されたパスが無効です。
.

MessageId=162
Severity=Success
Facility=System
SymbolicName=ERROR_SIGNAL_PENDING
Language=English
A signal is already pending.
.
Language=Japanese
信号が既に保留されています。
.

MessageId=164
Severity=Success
Facility=System
SymbolicName=ERROR_MAX_THRDS_REACHED
Language=English
No more threads can be created in the system.
.
Language=Japanese
システム内にスレッドを作成できません。
.

MessageId=167
Severity=Success
Facility=System
SymbolicName=ERROR_LOCK_FAILED
Language=English
Unable to lock a region of a file.
.
Language=Japanese
ファイルの領域をロックできません。
.

MessageId=170
Severity=Success
Facility=System
SymbolicName=ERROR_BUSY
Language=English
The requested resource is in use.
.
Language=Japanese
要求されたリソースは使用中です。
.

MessageId=173
Severity=Success
Facility=System
SymbolicName=ERROR_CANCEL_VIOLATION
Language=English
A lock request was not outstanding for the supplied cancel region.
.
Language=Japanese
供給されたキャンセル領域に対するロック要求が未処理です。
.

MessageId=174
Severity=Success
Facility=System
SymbolicName=ERROR_ATOMIC_LOCKS_NOT_SUPPORTED
Language=English
The file system does not support atomic changes to the lock type.
.
Language=Japanese
ファイルシステムはロックタイプのアトミックな変更に対応していません。
.

MessageId=180
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SEGMENT_NUMBER
Language=English
The system detected a segment number that was not correct.
.
Language=Japanese
正常でないセグメント番号が検出されました。
.

MessageId=182
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_ORDINAL
Language=English
The operating system cannot run %1.
.
Language=Japanese
オペレーティング システムは %1 を実行できません。
.

MessageId=183
Severity=Success
Facility=System
SymbolicName=ERROR_ALREADY_EXISTS
Language=English
Cannot create a file when that file already exists.
.
Language=Japanese
ファイルが既に存在する場合はファイルを作成できません。
.

MessageId=186
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_FLAG_NUMBER
Language=English
The flag passed is not correct.
.
Language=Japanese
渡されたフラグが正しくありません。
.

MessageId=187
Severity=Success
Facility=System
SymbolicName=ERROR_SEM_NOT_FOUND
Language=English
The specified system semaphore name was not found.
.
Language=Japanese
指定されたシステム セマフォ名が見つかりません。
.

MessageId=188
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_STARTING_CODESEG
Language=English
The operating system cannot run %1.
.
Language=Japanese
オペレーティング システムは %1 を実行できません。
.

MessageId=189
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_STACKSEG
Language=English
The operating system cannot run %1.
.
Language=Japanese
オペレーティング システムは %1 を実行できません。
.

MessageId=190
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_MODULETYPE
Language=English
The operating system cannot run %1.
.
Language=Japanese
オペレーティング システムは %1 を実行できません。
.

MessageId=191
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_EXE_SIGNATURE
Language=English
Cannot run %1 in Win32 mode.
.
Language=Japanese
Win32 モードで %1 を実行できません。
.

MessageId=192
Severity=Success
Facility=System
SymbolicName=ERROR_EXE_MARKED_INVALID
Language=English
The operating system cannot run %1.
.
Language=Japanese
オペレーティング システムは %1 を実行できません。
.

MessageId=193
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_EXE_FORMAT
Language=English
%1 is not a valid Win32 application.
.
Language=Japanese
%1 は有効な Win32 アプリケーションではありません。
.

MessageId=194
Severity=Success
Facility=System
SymbolicName=ERROR_ITERATED_DATA_EXCEEDS_64k
Language=English
The operating system cannot run %1.
.
Language=Japanese
オペレーティング システムは %1 を実行できません。
.

MessageId=195
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_MINALLOCSIZE
Language=English
The operating system cannot run %1.
.
Language=Japanese
オペレーティング システムは %1 を実行できません。
.

MessageId=196
Severity=Success
Facility=System
SymbolicName=ERROR_DYNLINK_FROM_INVALID_RING
Language=English
The operating system cannot run this application program.
.
Language=Japanese
オペレーティング システムはこのアプリケーション プログラムを実行できません。
.

MessageId=197
Severity=Success
Facility=System
SymbolicName=ERROR_IOPL_NOT_ENABLED
Language=English
The operating system is not presently configured to run this application.
.
Language=Japanese
オペレーティング システムは現在、このアプリケーションを実行するように構成されていません。
.

MessageId=198
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SEGDPL
Language=English
The operating system cannot run %1.
.
Language=Japanese
オペレーティング システムは %1 を実行できません。
.

MessageId=199
Severity=Success
Facility=System
SymbolicName=ERROR_AUTODATASEG_EXCEEDS_64k
Language=English
The operating system cannot run this application program.
.
Language=Japanese
オペレーティング システムはこのアプリケーション プログラムを実行できません。
.

MessageId=200
Severity=Success
Facility=System
SymbolicName=ERROR_RING2SEG_MUST_BE_MOVABLE
Language=English
The code segment cannot be greater than or equal to 64K.
.
Language=Japanese
コードセグメントが 64K 以上ではありません。
.

MessageId=201
Severity=Success
Facility=System
SymbolicName=ERROR_RELOC_CHAIN_XEEDS_SEGLIM
Language=English
The operating system cannot run %1.
.
Language=Japanese
オペレーティング システムは %1 を実行できません。
.

MessageId=202
Severity=Success
Facility=System
SymbolicName=ERROR_INFLOOP_IN_RELOC_CHAIN
Language=English
The operating system cannot run %1.
.
Language=Japanese
オペレーティング システムは %1 を実行できません。
.

MessageId=203
Severity=Success
Facility=System
SymbolicName=ERROR_ENVVAR_NOT_FOUND
Language=English
The system could not find the environment option that was entered.
.
Language=Japanese
入力された環境オプションが見つかりません。
.

MessageId=205
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SIGNAL_SENT
Language=English
No process in the command subtree has a signal handler.
.
Language=Japanese
コマンドのサブツリーにシグナル ハンドラを持つプロセスがありません。
.

MessageId=206
Severity=Success
Facility=System
SymbolicName=ERROR_FILENAME_EXCED_RANGE
Language=English
The filename or extension is too long.
.
Language=Japanese
ファイル名または拡張子が長すぎます。
.

MessageId=207
Severity=Success
Facility=System
SymbolicName=ERROR_RING2_STACK_IN_USE
Language=English
The ring 2 stack is in use.
.
Language=Japanese
リング2スタックが使用中です。
.

MessageId=208
Severity=Success
Facility=System
SymbolicName=ERROR_META_EXPANSION_TOO_LONG
Language=English
The global filename characters, * or ?, are entered incorrectly or too many global filename characters are specified.
.
Language=Japanese
グローバル ファイル名で文字 * または ? が間違って入力されたか、グローバル ファイル名文字が多すぎます。
.

MessageId=209
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SIGNAL_NUMBER
Language=English
The signal being posted is not correct.
.
Language=Japanese
書き込まれた信号が正しくありません。
.

MessageId=210
Severity=Success
Facility=System
SymbolicName=ERROR_THREAD_1_INACTIVE
Language=English
The signal handler cannot be set.
.
Language=Japanese
シグナル ハンドラを設定できません。
.

MessageId=212
Severity=Success
Facility=System
SymbolicName=ERROR_LOCKED
Language=English
The segment is locked and cannot be reallocated.
.
Language=Japanese
セグメントがロックされており、再割り当てできません。
.

MessageId=214
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_MODULES
Language=English
Too many dynamic-link modules are attached to this program or dynamic-link module.
.
Language=Japanese
このプログラムまたは動的リンク モジュールにアタッチされている動的リンク モジュールが多すぎます。
.

MessageId=215
Severity=Success
Facility=System
SymbolicName=ERROR_NESTING_NOT_ALLOWED
Language=English
Cannot nest calls to LoadModule.
.
Language=Japanese
LoadModule への呼び出しをネストできません。
.

MessageId=216
Severity=Success
Facility=System
SymbolicName=ERROR_EXE_MACHINE_TYPE_MISMATCH
Language=English
The image file %1 is valid, but is for a machine type other than the current machine.
.
Language=Japanese
このバージョンの %1 は、実行している ReliefWin のバージョンと互換性がありません。コンピュータのシステム情報を確認してから、ソフトウェア発行元に連絡してください。
.

MessageId=217
Severity=Success
Facility=System
SymbolicName=ERROR_EXE_CANNOT_MODIFY_SIGNED_BINARY
Language=English
The image file %1 is signed, unable to modify.
.
Language=Japanese
イメージファイル %1 は署名されています。
.

MessageId=218
Severity=Success
Facility=System
SymbolicName=ERRO_EXE_CANNOT_MODIFY_STRONG_SIGNED_BINARY
Language=English
The image file %1 is strong signed, unable to modify.
.
Language=Japanese
イメージ ファイル %1 は強く署名されています。
.

MessageId=230
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_PIPE
Language=English
The pipe state is invalid.
.
Language=Japanese
パイプの状態が無効です。
.

MessageId=231
Severity=Success
Facility=System
SymbolicName=ERROR_PIPE_BUSY
Language=English
All pipe instances are busy.
.
Language=Japanese
すべてのパイプインスタンスがビジーです。
.

MessageId=232
Severity=Success
Facility=System
SymbolicName=ERROR_NO_DATA
Language=English
The pipe is being closed.
.
Language=Japanese
パイプが閉じられています。
.

MessageId=233
Severity=Success
Facility=System
SymbolicName=ERROR_PIPE_NOT_CONNECTED
Language=English
No process is on the other end of the pipe.
.
Language=Japanese
パイプのもう一方の端にプロセスがありません。
.

MessageId=234
Severity=Success
Facility=System
SymbolicName=ERROR_MORE_DATA
Language=English
More data is available.
.
Language=Japanese
より多くのデータが利用可能です。
.

MessageId=240
Severity=Success
Facility=System
SymbolicName=ERROR_VC_DISCONNECTED
Language=English
The session was canceled.
.
Language=Japanese
セッションがキャンセルされました。
.

MessageId=254
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_EA_NAME
Language=English
The specified extended attribute name was invalid.
.
Language=Japanese
指定された拡張属性名が無効です。
.

MessageId=255
Severity=Success
Facility=System
SymbolicName=ERROR_EA_LIST_INCONSISTENT
Language=English
The extended attributes are inconsistent.
.
Language=Japanese
拡張属性に矛盾があります。
.

MessageId=258
Severity=Success
Facility=System
SymbolicName=WAIT_TIMEOUT
Language=English
The wait operation timed out.
.
Language=Japanese
待機操作はタイムアウトしました。
.

MessageId=259
Severity=Success
Facility=System
SymbolicName=ERROR_NO_MORE_ITEMS
Language=English
No more data is available.
.
Language=Japanese
使用可能なデータはありません。
.

MessageId=266
Severity=Success
Facility=System
SymbolicName=ERROR_CANNOT_COPY
Language=English
The copy functions cannot be used.
.
Language=Japanese
コピー関数は使用できません。
.

MessageId=267
Severity=Success
Facility=System
SymbolicName=ERROR_DIRECTORY
Language=English
The directory name is invalid.
.
Language=Japanese
ディレクトリ名が無効です。
.

MessageId=275
Severity=Success
Facility=System
SymbolicName=ERROR_EAS_DIDNT_FIT
Language=English
The extended attributes did not fit in the buffer.
.
Language=Japanese
拡張属性がバッファに収まりませんでした。
.

MessageId=276
Severity=Success
Facility=System
SymbolicName=ERROR_EA_FILE_CORRUPT
Language=English
The extended attribute file on the mounted file system is corrupt.
.
Language=Japanese
マウントされたファイルシステムの拡張属性ファイルが壊れています。
.

MessageId=277
Severity=Success
Facility=System
SymbolicName=ERROR_EA_TABLE_FULL
Language=English
The extended attribute table file is full.
.
Language=Japanese
拡張属性テーブルファイルがいっぱいです。
.

MessageId=278
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_EA_HANDLE
Language=English
The specified extended attribute handle is invalid.
.
Language=Japanese
指定された拡張属性ハンドルが無効です。
.

MessageId=282
Severity=Success
Facility=System
SymbolicName=ERROR_EAS_NOT_SUPPORTED
Language=English
The mounted file system does not support extended attributes.
.
Language=Japanese
マウントされたファイルシステムは拡張属性をサポートしていません。
.

MessageId=288
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_OWNER
Language=English
Attempt to release mutex not owned by caller.
.
Language=Japanese
呼び出し元が所有していないミューテックスを解放しようとしました。
.

MessageId=298
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_POSTS
Language=English
Too many posts were made to a semaphore.
.
Language=Japanese
セマフォへのポストが多すぎます。
.

MessageId=299
Severity=Success
Facility=System
SymbolicName=ERROR_PARTIAL_COPY
Language=English
Only part of a ReadProcessMemory or WriteProcessMemory request was completed.
.
Language=Japanese
ReadProcessMemoryまたはWriteProcessMemory要求の一部のみが完了しました。
.

MessageId=300
Severity=Success
Facility=System
SymbolicName=ERROR_OPLOCK_NOT_GRANTED
Language=English
The oplock request is denied.
.
Language=Japanese
オプロック要求が拒否されました。
.

MessageId=301
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_OPLOCK_PROTOCOL
Language=English
An invalid oplock acknowledgment was received by the system.
.
Language=Japanese
無効なオプロック承認がシステムによって受信されました。
.

MessageId=302
Severity=Success
Facility=System
SymbolicName=ERROR_DISK_TOO_FRAGMENTED
Language=English
The volume is too fragmented to complete this operation.
.
Language=Japanese
ボリュームが断片化されているため、この操作を完了できません。
.

MessageId=303
Severity=Success
Facility=System
SymbolicName=ERROR_DELETE_PENDING
Language=English
The file cannot be opened because it is in the process of being deleted.
.
Language=Japanese
ファイルは削除中なので開けません。
.

MessageId=317
Severity=Success
Facility=System
SymbolicName=ERROR_MR_MID_NOT_FOUND
Language=English
The system cannot find message text for message number 0x%1 in the message file for %2.
.
Language=Japanese
%2 のメッセージ ファイルにメッセージ番号 0x%1 のメッセージ テキストが見つかりません。
.

MessageId=318
Severity=Success
Facility=System
SymbolicName=ERROR_SCOPE_NOT_FOUND
Language=English
The scope specified was not found.
.
Language=Japanese
指定されたスコープが見つかりません。
.

MessageId=487
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_ADDRESS
Language=English
Attempt to access invalid address.
.
Language=Japanese
無効なアドレスへのアクセスを試みました。
.

MessageId=534
Severity=Success
Facility=System
SymbolicName=ERROR_ARITHMETIC_OVERFLOW
Language=English
Arithmetic result exceeded 32 bits.
.
Language=Japanese
演算結果が32ビットを超えた。
.

MessageId=535
Severity=Success
Facility=System
SymbolicName=ERROR_PIPE_CONNECTED
Language=English
There is a process on other end of the pipe.
.
Language=Japanese
パイプの反対側にプロセスがある。
.

MessageId=536
Severity=Success
Facility=System
SymbolicName=ERROR_PIPE_LISTENING
Language=English
Waiting for a process to open the other end of the pipe.
.
Language=Japanese
パイプのもう一方の端を開くプロセスを待機している。
.

MessageId=537
Severity=Success
Facility=System
SymbolicName=ERROR_ACPI_ERROR
Language=English
An error occurred in the ACPI subsystem.
.
Language=Japanese
ACPIサブシステムでエラーが発生しました。
.


MessageId=538
Severity=Success
Facility=System
SymbolicName=ERROR_ABIOS_ERROR
Language=English
An error occurred in the ABIOS subsystem
.
Language=Japanese
ABIOSサブシステムでエラーが発生しました。
.

MessageId=539
Severity=Success
Facility=System
SymbolicName=ERROR_WX86_WARNING
Language=English
A warning occurred in the WX86 subsystem.
.
Language=Japanese
WX86サブシステムで警告が発生しました。
.

MessageId=540
Severity=Success
Facility=System
SymbolicName=ERROR_WX86_ERROR
Language=English
An error occurred in the WX86 subsystem.
.
Language=Japanese
WX86サブシステムでエラーが発生しました。
.

MessageId=541
Severity=Success
Facility=System
SymbolicName=ERROR_TIMER_NOT_CANCELED
Language=English
An attempt was made to cancel or set a timer that has an associated APC and the subject thread is not the thread that originally set the timer with an associated APC routine.
.
Language=Japanese
関連する APC を持つタイマーをキャンセルまたは設定しようとしましたが、対象のスレッドは、関連する APC ルーチンを持つタイマーを最初に設定したスレッドではありません。
.

MessageId=542
Severity=Success
Facility=System
SymbolicName=ERROR_UNWIND
Language=English
Unwind exception code.
.
Language=Japanese
巻き戻し例外コードです。
.

MessageId=543
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_STACK
Language=English
An invalid or unaligned stack was encountered during an unwind operation.
.
Language=Japanese
巻き戻し操作中に無効なスタックまたはアラインされていないスタックに遭遇しました。
.

MessageId=544
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_UNWIND_TARGET
Language=English
An invalid unwind target was encountered during an unwind operation.
.
Language=Japanese
巻き戻し操作中に無効な巻き戻しターゲットに遭遇しました。
.

MessageId=545
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PORT_ATTRIBUTES
Language=English
Invalid Object Attributes specified to NtCreatePort or invalid Port Attributes specified to NtConnectPort
.
Language=Japanese
NtCreatePort に指定されたオブジェクト属性が無効、または NtConnectPort に指定されたポート属性が無効です。
.

MessageId=546
Severity=Success
Facility=System
SymbolicName=ERROR_PORT_MESSAGE_TOO_LONG
Language=English
Length of message passed to NtRequestPort or NtRequestWaitReplyPort was longer than the maximum message allowed by the port.
.
Language=Japanese
NtRequestPortまたはNtRequestWaitReplyPortに渡されたメッセージの長さが、ポートで許容される最大メッセージより長いです。
.

MessageId=547
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_QUOTA_LOWER
Language=English
An attempt was made to lower a quota limit below the current usage.
.
Language=Japanese
クォータ制限を現在の使用量より低くしようとしました。
.

MessageId=548
Severity=Success
Facility=System
SymbolicName=ERROR_DEVICE_ALREADY_ATTACHED
Language=English
An attempt was made to attach to a device that was already attached to another device.
.
Language=Japanese
すでに別のデバイスにアタッチされているデバイスにアタッチしようとしました。
.

MessageId=549
Severity=Success
Facility=System
SymbolicName=ERROR_INSTRUCTION_MISALIGNMENT
Language=English
An attempt was made to execute an instruction at an unaligned address and the host system does not support unaligned instruction references.
.
Language=Japanese
非整列アドレスで命令を実行しようとしましたが、ホスト システムは非整列命令参照をサポートしていません。
.

MessageId=550
Severity=Success
Facility=System
SymbolicName=ERROR_PROFILING_NOT_STARTED
Language=English
Profiling not started.
.
Language=Japanese
プロファイリングが開始されていません。
.

MessageId=551
Severity=Success
Facility=System
SymbolicName=ERROR_PROFILING_NOT_STOPPED
Language=English
Profiling not stopped.
.
Language=Japanese
プロファイリングが停止していません。
.

MessageId=552
Severity=Success
Facility=System
SymbolicName=ERROR_COULD_NOT_INTERPRET
Language=English
The passed ACL did not contain the minimum required information.
.
Language=Japanese
渡されたACLに必要最低限の情報が含まれていません。
.

MessageId=553
Severity=Success
Facility=System
SymbolicName=ERROR_PROFILING_AT_LIMIT
Language=English
The number of active profiling objects is at the maximum and no more may be started.
.
Language=Japanese
アクティブなプロファイリング オブジェクトの数が最大です。
.

MessageId=554
Severity=Success
Facility=System
SymbolicName=ERROR_CANT_WAIT
Language=English
Used to indicate that an operation cannot continue without blocking for I/O.
.
Language=Japanese
操作がI/Oのためにブロックされずに継続できないことを示すために使用されます。
.

MessageId=555
Severity=Success
Facility=System
SymbolicName=ERROR_CANT_TERMINATE_SELF
Language=English
Indicates that a thread attempted to terminate itself by default (called NtTerminateThread with NULL) and it was the last thread in the current process.
.
Language=Japanese
スレッドがデフォルトで終了しようとし (NULLでNtTerminateThreadを呼び出し) 、それが現在のプロセスの最後のスレッドであったことを示します。
.

MessageId=556
Severity=Success
Facility=System
SymbolicName=ERROR_UNEXPECTED_MM_CREATE_ERR
Language=English
If an MM error is returned which is not defined in the standard FsRtl filter, it is converted to one of the following errors which is guaranteed to be in the filter. In this case information is lost, however, the filter correctly handles the exception.
.
Language=Japanese
標準のFsRtlフィルタで定義されていないMMエラーが返された場合、フィルタで保証されている以下のエラーの1つに変換されます。
この場合、情報は失われますが、フィルタは例外を正しく処理します。
.

MessageId=557
Severity=Success
Facility=System
SymbolicName=ERROR_UNEXPECTED_MM_MAP_ERROR
Language=English
If an MM error is returned which is not defined in the standard FsRtl filter, it is converted to one of the following errors which is guaranteed to be in the filter. In this case information is lost, however, the filter correctly handles the exception.
.
Language=Japanese
標準のFsRtlフィルタで定義されていないMMエラーが返された場合、フィルタで保証されている以下のエラーのいずれかに変換されます。
この場合、情報は失われますが、フィルタは例外を正しく処理します。
.

MessageId=558
Severity=Success
Facility=System
SymbolicName=ERROR_UNEXPECTED_MM_EXTEND_ERR
Language=English
If an MM error is returned which is not defined in the standard FsRtl filter, it is converted to one of the following errors which is guaranteed to be in the filter. In this case information is lost, however, the filter correctly handles the exception.
.
Language=Japanese
標準のFsRtlフィルタで定義されていないMMエラーが返された場合、フィルタで保証されている以下のエラーのいずれかに変換されます。
この場合、情報は失われますが、フィルタは例外を正しく処理します。
.

MessageId=559
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_FUNCTION_TABLE
Language=English
A malformed function table was encountered during an unwind operation.
.
Language=Japanese
巻き戻し操作中に不正なファンクション テーブルが発生しました。
.

MessageId=560
Severity=Success
Facility=System
SymbolicName=ERROR_NO_GUID_TRANSLATION
Language=English
Indicates that an attempt was made to assign protection to a file system file or directory and one of the SIDs in the security descriptor could not be translated into a GUID that could be stored by the file system. This causes the protection attempt to fail, which may cause a file creation attempt to fail.
.
Language=Japanese
ファイルシステムのファイルまたはディレクトリに保護を割り当てようとしたときに、セキュリティ記述子のSIDの1つをファイルシステムに格納できるGUIDに変換できなかったことを示します。
このため、保護の試みが失敗し、ファイル作成の試みが失敗することがあります。
.

MessageId=561
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_LDT_SIZE
Language=English
Indicates that an attempt was made to grow an LDT by setting its size, or that the size was not an even number of selectors.
.
Language=Japanese
LDTのサイズを設定してLDTを大きくしようとした、またはサイズがセレクタの偶数でなかったことを示します。
.

MessageId=563
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_LDT_OFFSET
Language=English
Indicates that the starting value for the LDT information was not an integral multiple of the selector size.
.
Language=Japanese
LDT情報の開始値がセレクタ サイズの整数倍でなかったことを示す。
.

MessageId=564
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_LDT_DESCRIPTOR
Language=English
Indicates that the user supplied an invalid descriptor when trying to set up Ldt descriptors.
.
Language=Japanese
LDTディスクリプタを設定しようとしたときに、ユーザーが無効なディスクリプタを提供したことを示します。
.

MessageId=565
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_THREADS
Language=English
Indicates a process has too many threads to perform the requested action. For example, assignment of a primary token may only be performed when a process has zero or one threads.
.
Language=Japanese
プロセスに要求されたアクションを実行するスレッドが多すぎることを示します。例えば、プライマリ トークンの割り当ては、プロセスのスレッド数が0または1の場合にのみ実行できます。
.

MessageId=566
Severity=Success
Facility=System
SymbolicName=ERROR_THREAD_NOT_IN_PROCESS
Language=English
An attempt was made to operate on a thread within a specific process, but the thread specified is not in the process specified.
.
Language=Japanese
特定のプロセス内のスレッドを操作しようとしましたが、指定されたスレッドは指定されたプロセス内にありません。
.

MessageId=567
Severity=Success
Facility=System
SymbolicName=ERROR_PAGEFILE_QUOTA_EXCEEDED
Language=English
Page file quota was exceeded.
.
Language=Japanese
ページファイルのクォータを超過しました。
.

MessageId=568
Severity=Success
Facility=System
SymbolicName=ERROR_LOGON_SERVER_CONFLICT
Language=English
The Netlogon service cannot start because another Netlogon service running in the domain conflicts with the specified role.
.
Language=Japanese
ドメインで実行されている別のNetlogonサービスが指定されたロールと競合するため、Netlogonサービスを開始できません。
.

MessageId=569
Severity=Success
Facility=System
SymbolicName=ERROR_SYNCHRONIZATION_REQUIRED
Language=English
The SAM database on a Windows Server is significantly out of synchronization with the copy on the Domain Controller. A complete synchronization is required.
.
Language=Japanese
ReliefWin Server上のSAMデータベースがドメイン コントローラー上のコピーと同期していません。完全な同期が必要です。
.

MessageId=570
Severity=Success
Facility=System
SymbolicName=ERROR_NET_OPEN_FAILED
Language=English
The NtCreateFile API failed. This error should never be returned to an application, it is a place holder for the Windows Lan Manager Redirector to use in its internal error mapping routines.
.
Language=Japanese
NtCreateFile APIが失敗しました。このエラーはReliefWin Lan Manager Redirectorが内部のエラーマップルーチンで使用するプレースホルダーです。
.

MessageId=571
Severity=Success
Facility=System
SymbolicName=ERROR_IO_PRIVILEGE_FAILED
Language=English
The I/O permissions for the process could not be changed.
.
Language=Japanese
プロセスのI/O権限を変更できませんでした。
.

MessageId=572
Severity=Success
Facility=System
SymbolicName=ERROR_CONTROL_C_EXIT
Language=English
The application terminated as a result of a CTRL+C.
.
Language=Japanese
CTRL+Cの結果としてアプリケーションが終了しました。
.

MessageId=573
Severity=Success
Facility=System
SymbolicName=ERROR_MISSING_SYSTEMFILE
Language=English
The required system file %hs is bad or missing.
.
Language=Japanese
必要なシステム ファイル %hs が壊れているか見つかりません。
.

MessageId=574
Severity=Success
Facility=System
SymbolicName=ERROR_UNHANDLED_EXCEPTION
Language=English
The exception %s (0x%08lx) occurred in the application at location 0x%08lx.
.
Language=Japanese
例外 %s (0x)  が発生しました。
.

MessageId=575
Severity=Success
Facility=System
SymbolicName=ERROR_APP_INIT_FAILURE
Language=English
The application failed to initialize properly (0x%lx). Click on OK to terminate the application.
.
Language=Japanese
アプリケーションを正しく起動できませんでした (0x%lx) 。OK]をクリックしてアプリケーションを閉じます。
.

MessageId=576
Severity=Success
Facility=System
SymbolicName=ERROR_PAGEFILE_CREATE_FAILED
Language=English
The creation of the paging file %hs failed (%lx). The requested size was %ld.
.
Language=Japanese
ページング ファイル %hs の作成に失敗しました (%lx) 。要求されたサイズは %ld です。
.

MessageId=578
Severity=Success
Facility=System
SymbolicName=ERROR_NO_PAGEFILE
Language=English
No paging file was specified in the system configuration.
.
Language=Japanese
システム構成でページング ファイルが指定されていません。
.

MessageId=579
Severity=Success
Facility=System
SymbolicName=ERROR_ILLEGAL_FLOAT_CONTEXT
Language=English
A real-mode application issued a floating-point instruction and floating-point hardware is not present.
.
Language=Japanese
 リアルモードアプリケーションが浮動小数点命令を発行しましたが、浮動小数点ハードウェアが存在しませんでした。
.

MessageId=580
Severity=Success
Facility=System
SymbolicName=ERROR_NO_EVENT_PAIR
Language=English
An event pair synchronization operation was performed using the thread specific client/server event pair object, but no event pair object was associated with the thread.
.
Language=Japanese
イベント ペア同期操作がスレッド固有のクライアント/サーバー イベント ペア オブジェクトを使用して実行されましたが、イベント ペア オブジェクトがスレッドに関連付けられていませんでした。
.

MessageId=581
Severity=Success
Facility=System
SymbolicName=ERROR_DOMAIN_CTRLR_CONFIG_ERROR
Language=English
A Windows Server has an incorrect configuration.
.
Language=Japanese
ReliefWinサーバーの構成が正しくありません。
.

MessageId=582
Severity=Success
Facility=System
SymbolicName=ERROR_ILLEGAL_CHARACTER
Language=English
An illegal character was encountered. For a multi-byte character set this includes a lead byte without a succeeding trail byte. For the Unicode character set this includes the characters 0xFFFF and 0xFFFE.
.
Language=Japanese
不正な文字が見つかりました。マルチバイト文字セットには、後続のトレールバイトがないリードバイトが含まれます。Unicode文字セットの場合、これには0xFFFFと0xFFFEが含まれます。
.

MessageId=583
Severity=Success
Facility=System
SymbolicName=ERROR_UNDEFINED_CHARACTER
Language=English
The Unicode character is not defined in the Unicode character set installed on the system.
.
Language=Japanese
そのUnicode文字はシステムにインストールされているUnicode文字セットに定義されていません。
.

MessageId=584
Severity=Success
Facility=System
SymbolicName=ERROR_FLOPPY_VOLUME
Language=English
The paging file cannot be created on a floppy diskette.
.
Language=Japanese
ページング ファイルをフロッピー ディスク上に作成できません。
.

MessageId=585
Severity=Success
Facility=System
SymbolicName=ERROR_BIOS_FAILED_TO_CONNECT_INTERRUPT
Language=English
The system bios failed to connect a system interrupt to the device or bus for which the device is connected.
.
Language=Japanese
システムBIOSが、デバイスが接続されているデバイスまたはバスへのシステム割り込みの接続に失敗しました。
.

MessageId=586
Severity=Success
Facility=System
SymbolicName=ERROR_BACKUP_CONTROLLER
Language=English
This operation is only allowed for the Primary Domain Controller of the domain.
.
Language=Japanese
この操作はドメインのプライマリドメイン コントローラーにのみ許可されています。
.

MessageId=587
Severity=Success
Facility=System
SymbolicName=ERROR_MUTANT_LIMIT_EXCEEDED
Language=English
An attempt was made to acquire a mutant such that its maximum count would have been exceeded.
.
Language=Japanese
変異体の最大カウントを超える変異体を取得しようとしました。
.

MessageId=588
Severity=Success
Facility=System
SymbolicName=ERROR_FS_DRIVER_REQUIRED
Language=English
A volume has been accessed for which a file system driver is required that has not yet been loaded.
.
Language=Japanese
まだロードされていないファイル システム ドライバが必要なボリュームにアクセスされました。
.

MessageId=589
Severity=Success
Facility=System
SymbolicName=ERROR_CANNOT_LOAD_REGISTRY_FILE
Language=English
The registry cannot load the hive (file): %hs or its log or alternate. It is corrupt, absent, or not writable.
.
Language=Japanese
レジストリはハイブ (ファイル): %hs またはそのログまたは代替ファイルをロードできません。
.

MessageId=590
Severity=Success
Facility=System
SymbolicName=ERROR_DEBUG_ATTACH_FAILED
Language=English
An unexpected failure occurred while processing a DebugActiveProcess API request. You may choose OK to terminate the process, or Cancel to ignore the error.
.
Language=Japanese
DebugActiveProcess API要求の処理中に予期しない失敗が発生しました。プロセスを終了する場合は [OK] を、エラーを無視する場合は [キャンセル] を選択してください。
.

MessageId=591
Severity=Success
Facility=System
SymbolicName=ERROR_SYSTEM_PROCESS_TERMINATED
Language=English
The %hs system process terminated unexpectedly with a status of 0x%08x (0x%08x 0x%08x). The system has been shut down.
.
Language=Japanese
%hs システム プロセスがステータス 0x で予期せず終了しました。
.

MessageId=592
Severity=Success
Facility=System
SymbolicName=ERROR_DATA_NOT_ACCEPTED
Language=English
The TDI client could not handle the data received during an indication.
.
Language=Japanese
 TDI クライアントが指示中に受信したデータを処理できませんでした。
.

MessageId=593
Severity=Success
Facility=System
SymbolicName=ERROR_VDM_HARD_ERROR
Language=English
NTVDM encountered a hard error.
.
Language=Japanese
NTVDMでハードエラーが発生しました。
.

MessageId=594
Severity=Success
Facility=System
SymbolicName=ERROR_DRIVER_CANCEL_TIMEOUT
Language=English
The driver %hs failed to complete a cancelled I/O request in the allotted time.
.
Language=Japanese
ドライバ %hs が割り当てられた時間内にキャンセルされた I/O 要求を完了できませんでした。
.

MessageId=595
Severity=Success
Facility=System
SymbolicName=ERROR_REPLY_MESSAGE_MISMATCH
Language=English
An attempt was made to reply to an LPC message, but the thread specified by the client ID in the message was not waiting on that message.
.
Language=Japanese
LPC メッセージに返信しようとしましたが、メッセージ内のクライアント ID で指定されたスレッドはそのメッセージを待機していませんでした。
.

MessageId=596
Severity=Success
Facility=System
SymbolicName=ERROR_LOST_WRITEBEHIND_DATA
Language=English
ReliefWin was unable to save all the data for the file %hs. The data has been lost. This error may be caused by a failure of your computer hardware or network connection. Please try to save this file elsewhere.
.
Language=Japanese
ReliefWin はファイル %hs のすべてのデータを保存できませんでした。
このエラーは、コンピュータのハードウェアまたはネットワーク接続の障害によって発生した可能性があります。このファイルを別の場所に保存してください。
.

MessageId=597
Severity=Success
Facility=System
SymbolicName=ERROR_CLIENT_SERVER_PARAMETERS_INVALID
Language=English
The parameter(s) passed to the server in the client/server shared memory window were invalid. Too much data may have been put in the shared memory window.
.
Language=Japanese
クライアント/サーバー共有メモリ ウィンドウでサーバーに渡されたパラメータが無効です。共有メモリ ウィンドウにデータが多すぎる可能性があります。
.

MessageId=598
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_TINY_STREAM
Language=English
The stream is not a tiny stream.
.
Language=Japanese
ストリームがタイニー ストリームではありません。
.

MessageId=599
Severity=Success
Facility=System
SymbolicName=ERROR_STACK_OVERFLOW_READ
Language=English
The request must be handled by the stack overflow code.
.
Language=Japanese
リクエストはスタック オーバーフロー コードで処理する必要があります。
.

MessageId=600
Severity=Success
Facility=System
SymbolicName=ERROR_CONVERT_TO_LARGE
Language=English
Internal OFS status codes indicating how an allocation operation is handled. Either it is retried after the containing onode is moved or the extent stream is converted to a large stream.
.
Language=Japanese
割り当て操作がどのように処理されるかを示す内部 OFS ステータスコード。含むオノードが移動された後に再試行されるか、エクステントストリームがラージストリームに変換されます。
.

MessageId=601
Severity=Success
Facility=System
SymbolicName=ERROR_FOUND_OUT_OF_SCOPE
Language=English
The attempt to find the object found an object matching by ID on the volume but it is out of the scope of the handle used for the operation.
.
Language=Japanese
オブジェクトを検索しようとしたところ、ボリューム上にIDが一致するオブジェクトが見つかりましたが、操作に使用したハンドルの範囲外でした。
.

MessageId=602
Severity=Success
Facility=System
SymbolicName=ERROR_ALLOCATE_BUCKET
Language=English
The bucket array must be grown. Retry transaction after doing so.
.
Language=Japanese
バケット配列を成長させる必要があります。そうしてからトランザクションを再試行してください。
.

MessageId=603
Severity=Success
Facility=System
SymbolicName=ERROR_MARSHALL_OVERFLOW
Language=English
The user/kernel marshalling buffer has overflowed.
.
Language=Japanese
ユーザー/カーネルのマーシャリング バッファがオーバーフローしました。
.

MessageId=604
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_VARIANT
Language=English
The supplied variant structure contains invalid data.
.
Language=Japanese
指定されたバリアント構造体に無効なデータが含まれています。
.

MessageId=605
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_COMPRESSION_BUFFER
Language=English
The specified buffer contains ill-formed data.
.
Language=Japanese
指定されたバッファに不正なデータが含まれています。
.

MessageId=606
Severity=Success
Facility=System
SymbolicName=ERROR_AUDIT_FAILED
Language=English
An attempt to generate a security audit failed.
.
Language=Japanese
セキュリティ監査の生成に失敗しました。
.

MessageId=607
Severity=Success
Facility=System
SymbolicName=ERROR_TIMER_RESOLUTION_NOT_SET
Language=English
The timer resolution was not previously set by the current process.
.
Language=Japanese
タイマー分解能が現在のプロセスによって以前に設定されていません。
.

MessageId=608
Severity=Success
Facility=System
SymbolicName=ERROR_INSUFFICIENT_LOGON_INFO
Language=English
There is insufficient account information to log you on.
.
Language=Japanese
ログオンするためのアカウント情報が不足しています。
.

MessageId=609
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_DLL_ENTRYPOINT
Language=English
The dynamic link library %hs is not written correctly. The stack pointer has been left in an inconsistent state. The entrypoint should be declared as WINAPI or STDCALL. Select YES to fail the DLL load. Select NO to continue execution. Selecting NO may cause the application to operate incorrectly.
.
Language=Japanese
動的リンク ライブラリ %hs が正しく記述されていません。エントリポイントは WINAPI または STDCALL として宣言する必要があります。DLL ロードに失敗するには「はい」を選択してください。実行を続行するには「いいえ」を選択します。いいえを選択すると、アプリケーションが正しく動作しないことがあります。
.

MessageId=610
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_SERVICE_ENTRYPOINT
Language=English
The %hs service is not written correctly. The stack pointer has been left in an inconsistent state. The callback entrypoint should be declared as WINAPI or STDCALL. Selecting OK will cause the service to continue operation. However, the service process may operate incorrectly.
.
Language=Japanese
%hs サービスが正しく書き込まれていません。
コールバック エントリポイントは WINAPI または STDCALL として宣言する必要があります。OKを選択すると、サービスは動作を継続します。ただし、サービス プロセスが正しく動作しない可能性があります。
.

MessageId=611
Severity=Success
Facility=System
SymbolicName=ERROR_IP_ADDRESS_CONFLICT1
Language=English
There is an IP address conflict with another system on the network
.
Language=Japanese
ネットワーク上の他のシステムとIPアドレスが競合しています。
.

MessageId=612
Severity=Success
Facility=System
SymbolicName=ERROR_IP_ADDRESS_CONFLICT2
Language=English
There is an IP address conflict with another system on the network
.
Language=Japanese
ネットワーク上の他のシステムとIPアドレスが競合しています。
.

MessageId=613
Severity=Success
Facility=System
SymbolicName=ERROR_REGISTRY_QUOTA_LIMIT
Language=English
The system has reached the maximum size allowed for the system part of the registry. Additional storage requests will be ignored.
.
Language=Japanese
 システムはレジストリのシステム部分で許容される最大サイズに達しました。追加のストレージ要求は無視されます。
.

MessageId=614
Severity=Success
Facility=System
SymbolicName=ERROR_NO_CALLBACK_ACTIVE
Language=English
A callback return system service cannot be executed when no callback is active.
.
Language=Japanese
コールバックがアクティブでない場合、コールバック リターン システム サービスは実行できません。
.

MessageId=615
Severity=Success
Facility=System
SymbolicName=ERROR_PWD_TOO_SHORT
Language=English
The password provided is too short to meet the policy of your user account. Please choose a longer password.
.
Language=Japanese
指定されたパスワードが短すぎて、ユーザー アカウントのポリシーに適合しません。
長いパスワードを選択してください。
.

MessageId=616
Severity=Success
Facility=System
SymbolicName=ERROR_PWD_TOO_RECENT
Language=English
The policy of your user account does not allow you to change passwords too frequently. This is done to prevent users from changing back to a familiar, but potentially discovered, password. If you feel your password has been compromised then please contact your administrator immediately to have a new one assigned.
.
Language=Japanese
ユーザー アカウントのポリシーでは、パスワードを頻繁に変更することは許可されていません。
これは、ユーザーが使い慣れた、しかし発見される可能性のあるパスワードに変更することを防ぐために行われます。
パスワードが漏洩したと思われる場合は、すぐに管理者に連絡して新しいパスワードを割り当ててください。
.

MessageId=617
Severity=Success
Facility=System
SymbolicName=ERROR_PWD_HISTORY_CONFLICT
Language=English
You have attempted to change your password to one that you have used in the past. The policy of your user account does not allow this. Please select a password that you have not previously used.
.
Language=Japanese
パスワードを過去に使用したものに変更しようとしました。
ユーザー アカウントのポリシーでは許可されていません。
過去に使用したことのないパスワードを選択してください。
.

MessageId=618
Severity=Success
Facility=System
SymbolicName=ERROR_UNSUPPORTED_COMPRESSION
Language=English
The specified compression format is unsupported.
.
Language=Japanese
指定された圧縮形式はサポートされていません。
.

MessageId=619
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_HW_PROFILE
Language=English
The specified hardware profile configuration is invalid.
.
Language=Japanese
指定されたハードウェア プロファイル構成が無効です。
.

MessageId=620
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PLUGPLAY_DEVICE_PATH
Language=English
The specified Plug and Play registry device path is invalid.
.
Language=Japanese
指定された プラグ＆プレイ レジストリ デバイス パスが無効です。
.

MessageId=621
Severity=Success
Facility=System
SymbolicName=ERROR_QUOTA_LIST_INCONSISTENT
Language=English
The specified quota list is internally inconsistent with its descriptor.
.
Language=Japanese
指定されたクォータ リストは、その記述子と内部的に矛盾しています。
.

MessageId=622
Severity=Success
Facility=System
SymbolicName=ERROR_EVALUATION_EXPIRATION
Language=English
The evaluation period for this installation of Windows has expired. This system will shutdown in 1 hour. To restore access to this installation of Windows, please upgrade this installation using a licensed distribution of this product.
.
Language=Japanese
このReliefWinインストールの評価期間が終了しました。このシステムは1時間後にシャットダウンされます。ReliefWinのこのインストールへのアクセスを回復するには、この製品のライセンス配布を使用してこのインストールをアップグレードしてください。
.

MessageId=623
Severity=Success
Facility=System
SymbolicName=ERROR_ILLEGAL_DLL_RELOCATION
Language=English
The system DLL %hs was relocated in memory. The application will not run properly. The relocation occurred because the DLL %hs occupied an address range reserved for ReliefWin system DLLs. The vendor supplying the DLL should be contacted for a new DLL.
.
Language=Japanese
システム DLL %hs がメモリ内に再配置されました。アプリは適切に動作しないかもしれません。再配置は、DLL %hs が ReliefWin システム DLL 用に予約されているアドレス範囲を占有していたために発生しました。DLL を提供しているベンダーに連絡して、新しい DLL を入手してください。
.

MessageId=624
Severity=Success
Facility=System
SymbolicName=ERROR_DLL_INIT_FAILED_LOGOFF
Language=English
The application failed to initialize because the window station is shutting down.
.
Language=Japanese
ウィンドウ ステーションがシャットダウンされたため、アプリケーションの初期化に失敗しました。
.

MessageId=625
Severity=Success
Facility=System
SymbolicName=ERROR_VALIDATE_CONTINUE
Language=English
The validation process needs to continue on to the next step.
.
Language=Japanese
検証プロセスを次のステップに続行する必要があります。
.

MessageId=626
Severity=Success
Facility=System
SymbolicName=ERROR_NO_MORE_MATCHES
Language=English
There are no more matches for the current index enumeration.
.
Language=Japanese
現在のインデックス列挙に一致するものはありません。
.

MessageId=627
Severity=Success
Facility=System
SymbolicName=ERROR_RANGE_LIST_CONFLICT
Language=English
The range could not be added to the range list because of a conflict.
.
Language=Japanese
範囲が競合しているため、範囲を範囲リストに追加できませんでした。
.

MessageId=628
Severity=Success
Facility=System
SymbolicName=ERROR_SERVER_SID_MISMATCH
Language=English
The server process is running under a SID different than that required by client.
.
Language=Japanese
サーバー プロセスがクライアントが要求するSIDと異なるSIDで実行されています。
.

MessageId=629
Severity=Success
Facility=System
SymbolicName=ERROR_CANT_ENABLE_DENY_ONLY
Language=English
A group marked use for deny only cannot be enabled.
.
Language=Japanese
拒否にのみ使用するようにマークされたグループを有効にできません。
.

MessageId=630
Severity=Success
Facility=System
SymbolicName=ERROR_FLOAT_MULTIPLE_FAULTS
Language=English
Multiple floating point faults.
.
Language=Japanese
複数の浮動小数点フォールトです。
.

MessageId=631
Severity=Success
Facility=System
SymbolicName=ERROR_FLOAT_MULTIPLE_TRAPS
Language=English
Multiple floating point traps.
.
Language=Japanese
複数の浮動小数点トラップ複数の浮動小数点トラップです。
.

MessageId=632
Severity=Success
Facility=System
SymbolicName=ERROR_NOINTERFACE
Language=English
The requested interface is not supported.
.
Language=Japanese
要求されたインタフェースはサポートされていません。
.

MessageId=633
Severity=Success
Facility=System
SymbolicName=ERROR_DRIVER_FAILED_SLEEP
Language=English
The driver %hs does not support standby mode. Updating this driver may allow the system to go to standby mode.
.
Language=Japanese
ドライバ %hs はスタンバイ モードをサポートしていません。このドライバを更新すると、システムがスタンバイ モードになることがあります。
.

MessageId=634
Severity=Success
Facility=System
SymbolicName=ERROR_CORRUPT_SYSTEM_FILE
Language=English
The system file %1 has become corrupt and has been replaced.
.
Language=Japanese
システム ファイル %1 が破損したので置換しました。
.

MessageId=635
Severity=Success
Facility=System
SymbolicName=ERROR_COMMITMENT_MINIMUM
Language=English
Your system is low on virtual memory. ReliefWin is increasing the size of your virtual memory paging file. During this process, memory requests for some applications may be denied. For more information, see Help.
.
Language=Japanese
システムの仮想メモリが不足しています。このプロセスの間、一部のアプリケーションのメモリ要求が拒否されることがあります。詳細については、ヘルプを参照してください。
.

MessageId=636
Severity=Success
Facility=System
SymbolicName=ERROR_PNP_RESTART_ENUMERATION
Language=English
A device was removed so enumeration must be restarted.
.
Language=Japanese
デバイスが削除されたため、列挙を再開する必要があります。
.

MessageId=637
Severity=Success
Facility=System
SymbolicName=ERROR_SYSTEM_IMAGE_BAD_SIGNATURE
Language=English
The system image %s is not properly signed. The file has been replaced with the signed file. The system has been shut down.
.
Language=Japanese
システム イメージ %s は正しく署名されていません。
ファイルは署名されたファイルに置き換えられました。
システムはシャットダウンされました。
.

MessageId=638
Severity=Success
Facility=System
SymbolicName=ERROR_PNP_REBOOT_REQUIRED
Language=English
Device will not start without a reboot.
.
Language=Japanese
再起動しないとデバイスが起動しない
.

MessageId=639
Severity=Success
Facility=System
SymbolicName=ERROR_INSUFFICIENT_POWER
Language=English
There is not enough power to complete the requested operation.
.
Language=Japanese
要求された操作を完了するのに十分な電力がありません。
.

MessageId=641
Severity=Success
Facility=System
SymbolicName=ERROR_SYSTEM_SHUTDOWN
Language=English
The system is in the process of shutting down.
.
Language=Japanese
システムがシャットダウン中です。
.

MessageId=642
Severity=Success
Facility=System
SymbolicName=ERROR_PORT_NOT_SET
Language=English
An attempt to remove a processes DebugPort was made, but a port was not already associated with the process.
.
Language=Japanese
プロセスのDebugPortを削除しようとしましたが、ポートがまだプロセスに関連付けられていませんでした。
.

MessageId=643
Severity=Success
Facility=System
SymbolicName=ERROR_DS_VERSION_CHECK_FAILURE
Language=English
This version of ReliefWin is not compatible with the behavior version of directory forest, domain or domain controller.
.
Language=Japanese
このバージョンのReliefWinは、ディレクトリ フォレスト、ドメイン、またはドメイン コントローラの動作バージョンと互換性がありません。
.

MessageId=644
Severity=Success
Facility=System
SymbolicName=ERROR_RANGE_NOT_FOUND
Language=English
The specified range could not be found in the range list.
.
Language=Japanese
指定された範囲が範囲リストに見つかりませんでした。
.

MessageId=646
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_SAFE_MODE_DRIVER
Language=English
The driver was not loaded because the system is booting into safe mode.
.
Language=Japanese
システムがセーフ モードで起動しているため、ドライバがロードされませんでした。
.

MessageId=647
Severity=Success
Facility=System
SymbolicName=ERROR_FAILED_DRIVER_ENTRY
Language=English
The driver was not loaded because it failed its initialization call.
.
Language=Japanese
ドライバの初期化コールに失敗したため、ドライバがロードされませんでした。
.

MessageId=648
Severity=Success
Facility=System
SymbolicName=ERROR_DEVICE_ENUMERATION_ERROR
Language=English
The \"%hs\" encountered an error while applying power or reading the device configuration. This may be caused by a failure of your hardware or by a poor connection.
.
Language=Japanese
「%hs」は、電源の投入またはデバイス構成の読み取り中にエラーが発生しました。<CR><LF>ハードウェアの故障または接続不良が原因である可能性があります。
.

MessageId=649
Severity=Success
Facility=System
SymbolicName=ERROR_MOUNT_POINT_NOT_RESOLVED
Language=English
The create operation failed because the name contained at least one mount point which resolves to a volume to which the specified device object is not attached.
.
Language=Japanese
名前に、指定されたデバッグ対象のボリュームに解決するマウント ポイントが少なくとも 1 つ含まれていたため、作成操作に失敗しました。
.

MessageId=650
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_DEVICE_OBJECT_PARAMETER
Language=English
The device object parameter is either not a valid device object or is not attached to the volume specified by the file name.
.
Language=Japanese
デバイス オブジェクト パラメータが有効なデバイス オブジェクトでないか、ファイル名で指定されたボリュームにアタッチされていません。
.

MessageId=651
Severity=Success
Facility=System
SymbolicName=ERROR_MCA_OCCURED
Language=English
A Machine Check Error has occurred. Please check the system eventlog for additional information.
.
Language=Japanese
マシン チェック エラーが発生しました。追加情報については、システムのイベントログを確認してください。
.

MessageId=652
Severity=Success
Facility=System
SymbolicName=ERROR_DRIVER_DATABASE_ERROR
Language=English
There was error [%2] processing the driver database.
.
Language=Japanese
ドライバ データベースを処理するエラー [%2] が発生しました。
.

MessageId=653
Severity=Success
Facility=System
SymbolicName=ERROR_SYSTEM_HIVE_TOO_LARGE
Language=English
System hive size has exceeded its limit.
.
Language=Japanese
システム ハイブのサイズが制限を超えました。
.

MessageId=654
Severity=Success
Facility=System
SymbolicName=ERROR_DRIVER_FAILED_PRIOR_UNLOAD
Language=English
The driver could not be loaded because a previous version of the driver is still in memory.
.
Language=Japanese
以前のバージョンのドライバがメモリ内に残っているため、ドライバをロードできませんでした。
.

MessageId=655
Severity=Success
Facility=System
SymbolicName=ERROR_VOLSNAP_PREPARE_HIBERNATE
Language=English
Please wait while the Volume Shadow Copy Service prepares volume %hs for hibernation.
.
Language=Japanese
ボリューム シャドウ コピー サービスがボリューム %hs をハイバネーション用に準備するまでお待ちください。
.

MessageId=656
Severity=Success
Facility=System
SymbolicName=ERROR_HIBERNATION_FAILURE
Language=English
The system has failed to hibernate (The error code is %hs). Hibernation will be disabled until the system is restarted.
.
Language=Japanese
システムの休止に失敗しました (エラー コードは %hs) 。システムが再起動されるまで休止は無効になります。
.

MessageId=657
Severity=Success
Facility=System
SymbolicName=ERROR_HUNG_DISPLAY_DRIVER_THREAD
Language=English
The %hs display driver has stopped working normally. Save your work and reboot the system to restore full display functionality. The next time you reboot the machine a dialog will be displayed giving you a chance to report this failure to Microsoft.
.
Language=Japanese
ディスプレイドライバ%hsが正常に動作しなくなりました。作業を保存してシステムを再起動し、ディスプレイ機能を完全に回復してください。次にマシンを再起動するとダイアログが表示され、ReliefWinにこの障害を報告することができます。
.

MessageId=665
Severity=Success
Facility=System
SymbolicName=ERROR_FILE_SYSTEM_LIMITATION
Language=English
The requested operation could not be completed due to a file system limitation.
.
Language=Japanese
ファイル システムの制限により、要求された操作を完了できませんでした。
.

MessageId=668
Severity=Success
Facility=System
SymbolicName=ERROR_ASSERTION_FAILURE
Language=English
An assertion failure has occurred.
.
Language=Japanese
アサーションに失敗しました。
.

MessageId=669
Severity=Success
Facility=System
SymbolicName=ERROR_VERIFIER_STOP
Language=English
Application verifier has found an error in the current process.
.
Language=Japanese
ACPIサブシステムでエラーが発生しました。
.

MessageId=670
Severity=Success
Facility=System
SymbolicName=ERROR_WOW_ASSERTION
Language=English
WOW Assertion Error.
.
Language=Japanese
WOWアサーション エラーです。
.

MessageId=671
Severity=Success
Facility=System
SymbolicName=ERROR_PNP_BAD_MPS_TABLE
Language=English
A device is missing in the system BIOS MPS table. This device will not be used. Please contact your system vendor for system BIOS update.
.
Language=Japanese
システム BIOS MPS テーブルにデバイスがありません。
システムBIOSのアップデートについては、システムベンダにお問い合わせください。
.

MessageId=672
Severity=Success
Facility=System
SymbolicName=ERROR_PNP_TRANSLATION_FAILED
Language=English
A translator failed to translate resources.
.
Language=Japanese
トランスレータがリソースの翻訳に失敗しました。
.

MessageId=673
Severity=Success
Facility=System
SymbolicName=ERROR_PNP_IRQ_TRANSLATION_FAILED
Language=English
A IRQ translator failed to translate resources.
.
Language=Japanese
IRQ トランスレータがリソースの変換に失敗しました。
.

MessageId=674
Severity=Success
Facility=System
SymbolicName=ERROR_PNP_INVALID_ID
Language=English
Driver %2 returned invalid ID for a child device (%3).
.
Language=Japanese
ドライバ %2 は子デバイス (%3)  に対して無効な ID を返しました。
.

MessageId=675
Severity=Success
Facility=System
SymbolicName=ERROR_WAKE_SYSTEM_DEBUGGER
Language=English
The system debugger was awakened by an interrupt.
.
Language=Japanese
割り込みによってシステム デバッガが起動されました。
.

MessageId=676
Severity=Success
Facility=System
SymbolicName=ERROR_HANDLES_CLOSED
Language=English
Handles to objects have been automatically closed as a result of the requested operation.
.
Language=Japanese
要求された操作の結果、オブジェクトへのハンドルが自動的に閉じられました。
.

MessageId=677
Severity=Success
Facility=System
SymbolicName=ERROR_EXTRANEOUS_INFORMATION
Language=English
The specified access control list (ACL) contained more information than was expected.
.
Language=Japanese
指定されたアクセス制御リスト (ACL) に予想以上の情報が含まれていました。
.

MessageId=678
Severity=Success
Facility=System
SymbolicName=ERROR_RXACT_COMMIT_NECESSARY
Language=English
This warning level status indicates that the transaction state already exists for the registry sub-tree, but that a transaction commit was previously aborted. The commit has NOT been completed, but has not been rolled back either (so it may still be committed if desired).
.
Language=Japanese
この警告レベルのステータスは、レジストリ サブツリーに対してトランザクション状態がすでに存在するが、トランザクション コミットが以前に中止されたことを示します。
コミットは完了していませんが、ロールバックもされていません (そのため、必要であればまだコミットできます) 。
.

MessageId=679
Severity=Success
Facility=System
SymbolicName=ERROR_MEDIA_CHECK
Language=English
The media may have changed.
.
Language=Japanese
メディアが変更された可能性があります。
.

MessageId=680
Severity=Success
Facility=System
SymbolicName=ERROR_GUID_SUBSTITUTION_MADE
Language=English
During the translation of a global identifier (GUID) to a Windows security ID (SID), no administratively-defined GUID prefix was found. A substitute prefix was used, which will not compromise system security. However, this may provide a more restrictive access than intended.
.
Language=Japanese
グローバル識別子 (GUID)  から ReliefWin セキュリティ ID (SID)  への変換中に、管理上定義された GUID 接頭辞が見つかりませんでした。システムのセキュリティを損なわない代替接頭辞が使用されました。代替接頭辞が使用されました。
.

MessageId=681
Severity=Success
Facility=System
SymbolicName=ERROR_STOPPED_ON_SYMLINK
Language=English
The create operation stopped after reaching a symbolic link.
.
Language=Japanese
作成操作がシンボリック リンクに達した後に停止しました。
.

MessageId=682
Severity=Success
Facility=System
SymbolicName=ERROR_LONGJUMP
Language=English
A long jump has been executed.
.
Language=Japanese
ロング ジャンプが実行されました。
.

MessageId=683
Severity=Success
Facility=System
SymbolicName=ERROR_PLUGPLAY_QUERY_VETOED
Language=English
The Plug and Play query operation was not successful.
.
Language=Japanese
プラグ アンド プレイのクエリ操作は成功しませんでした。
.

MessageId=684
Severity=Success
Facility=System
SymbolicName=ERROR_UNWIND_CONSOLIDATE
Language=English
A frame consolidation has been executed.
.
Language=Japanese
フレーム連結が実行されました。
.

MessageId=685
Severity=Success
Facility=System
SymbolicName=ERROR_REGISTRY_HIVE_RECOVERED
Language=English
Registry hive (file): %hs was corrupted and it has been recovered. Some data might have been lost.
.
Language=Japanese
レジストリ ハイブ (ファイル): %hs が破損していたため回復しました。一部のデータが失われた可能性があります。
.

MessageId=686
Severity=Success
Facility=System
SymbolicName=ERROR_DLL_MIGHT_BE_INSECURE
Language=English
The application is attempting to run executable code from the module %hs. This may be insecure. An alternative, %hs, is available. Should the application use the secure module %hs?
.
Language=Japanese
アプリケーションは、モジュール %hs から実行可能コードを実行しようとしています。これは安全でない可能性があります。代替のモジュール %hs を使用できます。アプリケーションは安全なモジュール %hs を使用しますか?
.

MessageId=687
Severity=Success
Facility=System
SymbolicName=ERROR_DLL_MIGHT_BE_INCOMPATIBLE
Language=English
The application is loading executable code from the module %hs. This is secure, but may be incompatible with previous releases of the operating system. An alternative, %hs, is available. Should the application use the secure module %hs?
.
Language=Japanese
アプリケーションは、モジュール %hs から実行可能コードをロードしています。これは安全ですが、オペレーティング システムの以前のリリースと互換性がない可能性があります。代替のモジュール %hs を使用できます。アプリケーションは安全なモジュール %hs を使用しますか?
.

MessageId=688
Severity=Success
Facility=System
SymbolicName=ERROR_DBG_EXCEPTION_NOT_HANDLED
Language=English
Debugger did not handle the exception.
.
Language=Japanese
デバッガが例外を処理しませんでした。
.

MessageId=689
Severity=Success
Facility=System
SymbolicName=ERROR_DBG_REPLY_LATER
Language=English
Debugger will reply later.
.
Language=Japanese
デバッガは後で応答します。
.

MessageId=690
Severity=Success
Facility=System
SymbolicName=ERROR_DBG_UNABLE_TO_PROVIDE_HANDLE
Language=English
Debugger can not provide handle.
.
Language=Japanese
デバッガはハンドルを提供できません。
.

MessageId=691
Severity=Success
Facility=System
SymbolicName=ERROR_DBG_TERMINATE_THREAD
Language=English
Debugger terminated thread.
.
Language=Japanese
デバッガがスレッドを終了しました。
.

MessageId=692
Severity=Success
Facility=System
SymbolicName=ERROR_DBG_TERMINATE_PROCESS
Language=English
Debugger terminated process.
.
Language=Japanese
デバッガがプロセスを終了しました。
.

MessageId=693
Severity=Success
Facility=System
SymbolicName=ERROR_DBG_CONTROL_C
Language=English
Debugger got control C.
.
Language=Japanese
デバッガがコントロールCを取得しました。
.

MessageId=694
Severity=Success
Facility=System
SymbolicName=ERROR_DBG_PRINTEXCEPTION_C
Language=English
Debugger printed exception on control C.
.
Language=Japanese
デバッガがコントロール C に例外を出力しました。
.

MessageId=695
Severity=Success
Facility=System
SymbolicName=ERROR_DBG_RIPEXCEPTION
Language=English
Debugger received RIP exception.
.
Language=Japanese
デバッガが RIP 例外を受信しました。
.

MessageId=696
Severity=Success
Facility=System
SymbolicName=ERROR_DBG_CONTROL_BREAK
Language=English
Debugger received control break.
.
Language=Japanese
デバッガが制御ブレークを受信しました。
.

MessageId=697
Severity=Success
Facility=System
SymbolicName=ERROR_DBG_COMMAND_EXCEPTION
Language=English
Debugger command communication exception.
.
Language=Japanese
デバッガコマンド通信例外です。
.

MessageId=698
Severity=Success
Facility=System
SymbolicName=ERROR_OBJECT_NAME_EXISTS
Language=English
An attempt was made to create an object and the object name already existed.
.
Language=Japanese
オブジェクトを作成しようとしましたが、オブジェクト名が既に存在しました。
.

MessageId=699
Severity=Success
Facility=System
SymbolicName=ERROR_THREAD_WAS_SUSPENDED
Language=English
A thread termination occurred while the thread was suspended. The thread was resumed, and termination proceeded.
.
Language=Japanese
スレッドのサスペンド中にスレッドの終了が発生しました。スレッドは再開され、終了が続行されました。
.

MessageId=700
Severity=Success
Facility=System
SymbolicName=ERROR_IMAGE_NOT_AT_BASE
Language=English
An image file could not be mapped at the address specified in the image file. Local fixups must be performed on this image.
.
Language=Japanese
イメージ ファイルで指定されたアドレスにイメージ ファイルをマッピングできませんでした。このイメージに対してローカル フィックスアップを実行する必要があります。
.

MessageId=701
Severity=Success
Facility=System
SymbolicName=ERROR_RXACT_STATE_CREATED
Language=English
This informational level status indicates that a specified registry sub-tree transaction state did not yet exist and had to be created.
.
Language=Japanese
この情報レベルのステータスは、指定されたレジストリのサブツリーのトランザクション状態がまだ存在せず、作成する必要があることを示します。
.

MessageId=702
Severity=Success
Facility=System
SymbolicName=ERROR_SEGMENT_NOTIFICATION
Language=English
A virtual DOS machine (VDM) is loading, unloading, or moving an MS-DOS or Win16 program segment image. An exception is raised so a debugger can load, unload or track symbols and breakpoints within these 16-bit segments.
.
Language=Japanese
仮想DOSマシン (VDM) がMS-DOSまたはWin16プログラムセグメントイメージをロード、アンロード、または移動しています。デバッガがこれらの16ビットセグメント内のシンボルとブレークポイントをロード、アンロード、または追跡できるように例外が発生します。
.

MessageId=703
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_CURRENT_DIRECTORY
Language=English
The process cannot switch to the startup current directory %hs. Select OK to set current directory to %hs, or select CANCEL to exit.
.
Language=Japanese
プロセスが起動時のカレント ディレクトリ %hs に切り替えられません。[OK] を選択してカレント ディレクトリを %hs に設定するか、[キャンセル] を選択して終了します。
.

MessageId=704
Severity=Success
Facility=System
SymbolicName=ERROR_FT_READ_RECOVERY_FROM_BACKUP
Language=English
To satisfy a read request, the NT fault-tolerant file system successfully read the requested data from a redundant copy. This was done because the file system encountered a failure on a member of the fault-tolerant volume, but was unable to reassign the failing area of the device.
.
Language=Japanese
読み取り要求を満たすために、NTフォールト トレラント ファイル システムは要求されたデータを冗長コピーから正常に読み取りました。これは、ファイル システムでフォールト トレラント ボリュームのメンバに障害が発生したが、デバイスの障害領域を再割り当てできなかったためです。
.

MessageId=705
Severity=Success
Facility=System
SymbolicName=ERROR_FT_WRITE_RECOVERY
Language=English
To satisfy a write request, the NT fault-tolerant file system successfully wrote a redundant copy of the information. This was done because the file system encountered a failure on a member of the fault-tolerant volume, but was not able to reassign the failing area of the device.
.
Language=Japanese
書き込み要求を満たすために、NTフォールト トレラント ファイル システムは情報の冗長コピーを正常に書き込みました。これは、ファイル システムでフォールト トレラント ボリュームのメンバに障害が発生したが、デバイスの障害領域を再割り当てできなかったためです。
.

MessageId=706
Severity=Success
Facility=System
SymbolicName=ERROR_IMAGE_MACHINE_TYPE_MISMATCH
Language=English
The image file %hs is valid, but is for a machine type other than the current machine. Select OK to continue, or CANCEL to fail the DLL load.
.
Language=Japanese
イメージ ファイル %hs は有効ですが、現在のマシン以外のマシン タイプ用です。続行するにはOKを、DLLのロードに失敗するにはキャンセルを選択してください。
.

MessageId=707
Severity=Success
Facility=System
SymbolicName=ERROR_RECEIVE_PARTIAL
Language=English
The network transport returned partial data to its client. The remaining data will be sent later.
.
Language=Japanese
ネットワーク トランスポートがクライアントに部分的なデータを返しました。残りのデータは後で送信されます。
.

MessageId=708
Severity=Success
Facility=System
SymbolicName=ERROR_RECEIVE_EXPEDITED
Language=English
The network transport returned data to its client that was marked as expedited by the remote system.
.
Language=Japanese
ネットワークトランスポートは、リモートシステムによって迅速化としてマークされたデータをクライアントに返しました。
.

MessageId=709
Severity=Success
Facility=System
SymbolicName=ERROR_RECEIVE_PARTIAL_EXPEDITED
Language=English
The network transport returned partial data to its client and this data was marked as expedited by the remote system. The remaining data will be sent later.
.
Language=Japanese
ネットワーク トランスポートはクライアントに部分的なデータを返しました。残りのデータは後で送信されます。
.

MessageId=710
Severity=Success
Facility=System
SymbolicName=ERROR_EVENT_DONE
Language=English
The TDI indication has completed successfully.
.
Language=Japanese
TDI指示が正常に完了しました。
.

MessageId=711
Severity=Success
Facility=System
SymbolicName=ERROR_EVENT_PENDING
Language=English
The TDI indication has entered the pending state.
.
Language=Japanese
TDI指示が保留状態に入りました。
.

MessageId=712
Severity=Success
Facility=System
SymbolicName=ERROR_CHECKING_FILE_SYSTEM
Language=English
Checking file system on %wZ.
.
Language=Japanese
%wZ でファイル システムをチェックしています。
.

MessageId=714
Severity=Success
Facility=System
SymbolicName=ERROR_PREDEFINED_HANDLE
Language=English
The specified registry key is referenced by a predefined handle.
.
Language=Japanese
指定されたレジストリ キーは定義済みのハンドルによって参照されます。
.

MessageId=715
Severity=Success
Facility=System
SymbolicName=ERROR_WAS_UNLOCKED
Language=English
The page protection of a locked page was changed to 'No Access' and the page was unlocked from memory and from the process.
.
Language=Japanese
ロックされたページのページ保護が「アクセスなし」に変更され、メモリからもプロセスからもロックが解除されました。
.

MessageId=717
Severity=Success
Facility=System
SymbolicName=ERROR_WAS_LOCKED
Language=English
One of the pages to lock was already locked.
.
Language=Japanese
ロックするページの1つが既にロックされています。
.

MessageId=720
Severity=Success
Facility=System
SymbolicName=ERROR_IMAGE_MACHINE_TYPE_MISMATCH_EXE
Language=English
The image file %hs is valid, but is for a machine type other than the current machine.
.
Language=Japanese
イメージ ファイル %hs は有効ですが、現在のマシン以外のマシン タイプ用です。
.

MessageId=721
Severity=Success
Facility=System
SymbolicName=ERROR_NO_YIELD_PERFORMED
Language=English
A yield execution was performed and no thread was available to run.
.
Language=Japanese
降伏実行が実行され、実行可能なスレッドがありませんでした。
.

MessageId=722
Severity=Success
Facility=System
SymbolicName=ERROR_TIMER_RESUME_IGNORED
Language=English
The resumable flag to a timer API was ignored.
.
Language=Japanese
タイマーAPIへの再開可能フラグが無視されました。
.

MessageId=723
Severity=Success
Facility=System
SymbolicName=ERROR_ARBITRATION_UNHANDLED
Language=English
The arbiter has deferred arbitration of these resources to its parent.
.
Language=Japanese
アービタはこれらのリソースのアービトレーションを親に延期しました。
.

MessageId=724
Severity=Success
Facility=System
SymbolicName=ERROR_CARDBUS_NOT_SUPPORTED
Language=English
The device \"%hs\" has detected a CardBus card in its slot, but the firmware on this system is not configured to allow the CardBus controller to be run in CardBus mode. The operating system will currently accept only 16-bit (R2) pc-cards on this controller.
.
Language=Japanese
「%hs」の設定エラーのため、挿入されたCardBusデバイスが起動できません。
.

MessageId=725
Severity=Success
Facility=System
SymbolicName=ERROR_MP_PROCESSOR_MISMATCH
Language=English
The CPUs in this multiprocessor system are not all the same revision level. To use all processors the operating system restricts itself to the features of the least capable processor in the system. Should problems occur with this system, contact the CPU manufacturer to see if this mix of processors is supported.
.
Language=Japanese
このマルチ プロセッサ システムのCPUは、すべて同じリビジョン レベルではありません。すべてのプロセッサを使用するために、オペレーティング システムはシステム内で最も性能の低いプロセッサの機能に制限しています。このシステムで問題が発生した場合は、CPUメーカーに問い合わせて、このプロセッサの組み合わせがサポートされているかどうかを確認してください。
.

MessageId=726
Severity=Success
Facility=System
SymbolicName=ERROR_HIBERNATED
Language=English
The system was put into hibernation.
.
Language=Japanese
システムは休止状態になりました。
.

MessageId=727
Severity=Success
Facility=System
SymbolicName=ERROR_RESUME_HIBERNATION
Language=English
The system was resumed from hibernation.
.
Language=Japanese
システムは休止状態から再開されました。
.

MessageId=728
Severity=Success
Facility=System
SymbolicName=ERROR_FIRMWARE_UPDATED
Language=English
ReliefWin has detected that the system firmware (BIOS) was updated [previous firmware date = %2, current firmware date %3].
.
Language=Japanese
ReliefWin はシステムのファームウェア (BIOS)  が更新されたことを検出しました [以前のファームウェアの日付 = %2、現在のファームウェアの日付 %3]。
.

MessageId=729
Severity=Success
Facility=System
SymbolicName=ERROR_DRIVERS_LEAKING_LOCKED_PAGES
Language=English
A device driver is leaking locked I/O pages causing system degradation. The system has automatically enabled tracking code in order to try and catch the culprit.
.
Language=Japanese
デバイス ドライバがロックされた I/O ページをリークしてシステムの低下を引き起こしています。システムは自動的に追跡コードを有効にして、犯人を捕まえようとしています。
.

MessageId=730
Severity=Success
Facility=System
SymbolicName=ERROR_WAKE_SYSTEM
Language=English
The system has awoken
.
Language=Japanese
システムが起動しました。
.

MessageId=740
Severity=Success
Facility=System
SymbolicName=ERROR_ELEVATION_REQUIRED
Language=English
The requested operation requires elevation.
.
Language=Japanese
The requested operation requires elevation.
.

MessageId=741
Severity=Success
Facility=System
SymbolicName=ERROR_REPARSE
Language=English
A reparse should be performed by the Object Manager since the name of the file resulted in a symbolic link.
.
Language=Japanese
ファイル名がシンボリック リンクであったため、オブジェクト マネージャで再解析を実行する必要があります。
.

MessageId=742
Severity=Success
Facility=System
SymbolicName=ERROR_OPLOCK_BREAK_IN_PROGRESS
Language=English
An open/create operation completed while an oplock break is underway.
.
Language=Japanese
オプロック解除中にオープン/クリエイト操作が完了しました。
.

MessageId=743
Severity=Success
Facility=System
SymbolicName=ERROR_VOLUME_MOUNTED
Language=English
A new volume has been mounted by a file system.
.
Language=Japanese
新しいボリュームがファイルシステムによってマウントされました。
.

MessageId=744
Severity=Success
Facility=System
SymbolicName=ERROR_RXACT_COMMITTED
Language=English
This success level status indicates that the transaction state already exists for the registry sub-tree, but that a transaction commit was previously aborted. The commit has now been completed.
.
Language=Japanese
この成功レベルのステータスは、レジストリ サブツリーに対してトランザクション状態がすでに存在するが、トランザクション コミットが以前に中止されたことを示します。
.

MessageId=745
Severity=Success
Facility=System
SymbolicName=ERROR_NOTIFY_CLEANUP
Language=English
This indicates that a notify change request has been completed due to closing the handle which made the notify change request.
.
Language=Japanese
これは、通知変更要求を行ったハンドルを閉じることにより、通知変更要求が完了したことを示します。
.

MessageId=746
Severity=Success
Facility=System
SymbolicName=ERROR_PRIMARY_TRANSPORT_CONNECT_FAILED
Language=English
An attempt was made to connect to the remote server %hs on the primary transport, but the connection failed. The computer WAS able to connect on a secondary transport.
.
Language=Japanese
プライマリ トランスポートでリモート サーバ %hs に接続しようとしましたが、接続に失敗しました。
.

MessageId=747
Severity=Success
Facility=System
SymbolicName=ERROR_PAGE_FAULT_TRANSITION
Language=English
Page fault was a transition fault.
.
Language=Japanese
ページフォルトはトランジションフォルトでした。
.

MessageId=748
Severity=Success
Facility=System
SymbolicName=ERROR_PAGE_FAULT_DEMAND_ZERO
Language=English
Page fault was a demand zero fault.
.
Language=Japanese
ページフォルトはデマンドゼロのフォールトでした。
.

MessageId=749
Severity=Success
Facility=System
SymbolicName=ERROR_PAGE_FAULT_COPY_ON_WRITE
Language=English
Page fault was a demand zero fault.
.
Language=Japanese
ページフォルトはデマンドゼロのフォールトでした。
.

MessageId=750
Severity=Success
Facility=System
SymbolicName=ERROR_PAGE_FAULT_GUARD_PAGE
Language=English
Page fault was a demand zero fault.
.
Language=Japanese
ページフォルトはデマンドゼロのフォールトでした。
.

MessageId=751
Severity=Success
Facility=System
SymbolicName=ERROR_PAGE_FAULT_PAGING_FILE
Language=English
Page fault was satisfied by reading from a secondary storage device.
.
Language=Japanese
ページフォルトはセカンダリストレージデバイスからの読み取りによって満たされました。
.

MessageId=752
Severity=Success
Facility=System
SymbolicName=ERROR_CACHE_PAGE_LOCKED
Language=English
Cached page was locked during operation.
.
Language=Japanese
操作中にキャッシュされたページがロックされました。
.

MessageId=753
Severity=Success
Facility=System
SymbolicName=ERROR_CRASH_DUMP
Language=English
Crash dump exists in paging file.
.
Language=Japanese
ページング ファイルにクラッシュ ダンプが存在します。
.

MessageId=754
Severity=Success
Facility=System
SymbolicName=ERROR_BUFFER_ALL_ZEROS
Language=English
Specified buffer contains all zeros.
.
Language=Japanese
指定されたバッファにすべてのゼロが含まれています。
.

MessageId=755
Severity=Success
Facility=System
SymbolicName=ERROR_REPARSE_OBJECT
Language=English
A reparse should be performed by the Object Manager since the name of the file resulted in a symbolic link.
.
Language=Japanese
ファイル名がシンボリック リンクであったため、オブジェクト マネージャで再解析を実行する必要があります。
.

MessageId=756
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_REQUIREMENTS_CHANGED
Language=English
The device has succeeded a query-stop and its resource requirements have changed.
.
Language=Japanese
デバイスがクエリ停止に成功し、リソース要件が変更されました。
.

MessageId=757
Severity=Success
Facility=System
SymbolicName=ERROR_TRANSLATION_COMPLETE
Language=English
The translator has translated these resources into the global space and no further translations should be performed.
.
Language=Japanese
トランスレータはこれらのリソースをグローバル空間に変換しました。
.

MessageId=758
Severity=Success
Facility=System
SymbolicName=ERROR_NOTHING_TO_TERMINATE
Language=English
A process being terminated has no threads to terminate.
.
Language=Japanese
終了するプロセスには終了するスレッドがありません。
.

MessageId=759
Severity=Success
Facility=System
SymbolicName=ERROR_PROCESS_NOT_IN_JOB
Language=English
The specified process is not part of a job.
.
Language=Japanese
指定されたプロセスはジョブの一部ではありません。
.

MessageId=760
Severity=Success
Facility=System
SymbolicName=ERROR_PROCESS_IN_JOB
Language=English
The specified process is part of a job.
.
Language=Japanese
指定されたプロセスはジョブの一部です。
.

MessageId=761
Severity=Success
Facility=System
SymbolicName=ERROR_VOLSNAP_HIBERNATE_READY
Language=English
The system is now ready for hibernation.
.
Language=Japanese
システムは休止状態への準備が整いました。
.

MessageId=762
Severity=Success
Facility=System
SymbolicName=ERROR_FSFILTER_OP_COMPLETED_SUCCESSFULLY
Language=English
A file system or file system filter driver has successfully completed an FsFilter operation.
.
Language=Japanese
ファイル システムまたはファイル システム フィルタ ドライバーが FsFilter 操作を正常に完了しました。
.

MessageId=763
Severity=Success
Facility=System
SymbolicName=ERROR_INTERRUPT_VECTOR_ALREADY_CONNECTED
Language=English
The specified interrupt vector was already connected.
.
Language=Japanese
指定された割り込みベクタはすでに接続されています。
.

MessageId=764
Severity=Success
Facility=System
SymbolicName=ERROR_INTERRUPT_STILL_CONNECTED
Language=English
The specified interrupt vector is still connected.
.
Language=Japanese
指定された割り込みベクタはまだ接続されています。
.

MessageId=765
Severity=Success
Facility=System
SymbolicName=ERROR_WAIT_FOR_OPLOCK
Language=English
An operation is blocked waiting for an oplock.
.
Language=Japanese
操作がブロックされました。
.

MessageId=766
Severity=Success
Facility=System
SymbolicName=ERROR_DBG_EXCEPTION_HANDLED
Language=English
Debugger handled exception.
.
Language=Japanese
デバッガが例外を処理しました。
.

MessageId=767
Severity=Success
Facility=System
SymbolicName=ERROR_DBG_CONTINUE
Language=English
Debugger continued
.
Language=Japanese
デバッガ継続。
.

MessageId=768
Severity=Success
Facility=System
SymbolicName=ERROR_CALLBACK_POP_STACK
Language=English
An exception occurred in a user mode callback and the kernel callback frame should be removed.
.
Language=Japanese
ユーザー モード コールバックで例外が発生したため、カーネル コールバック フレームを削除する必要があります。
.

MessageId=769
Severity=Success
Facility=System
SymbolicName=ERROR_COMPRESSION_DISABLED
Language=English
Compression is disabled for this volume.
.
Language=Japanese
このボリュームでは圧縮が無効になっています。
.

MessageId=770
Severity=Success
Facility=System
SymbolicName=ERROR_CANTFETCHBACKWARDS
Language=English
The data provider cannot fetch backwards through a result set.
.
Language=Japanese
データ プロバイダーは結果セットを後方にフェッチできません。
.

MessageId=771
Severity=Success
Facility=System
SymbolicName=ERROR_CANTSCROLLBACKWARDS
Language=English
The data provider cannot scroll backwards through a result set.
.
Language=Japanese
データ プロバイダーは結果セットを逆方向にスクロールできません。
.

MessageId=772
Severity=Success
Facility=System
SymbolicName=ERROR_ROWSNOTRELEASED
Language=English
The data provider requires that previously fetched data is released before asking for more data.
.
Language=Japanese
データ プロバイダーは、さらにデータを要求する前に、以前にフェッチされたデータを解放する必要があります。
.

MessageId=773
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_ACCESSOR_FLAGS
Language=English
The data provider was not able to interpret the flags set for a column binding in an accessor.
.
Language=Japanese
データ プロバイダーが、アクセサ内の列バインディングに設定されたフラグを解釈できませんでした。
.

MessageId=774
Severity=Success
Facility=System
SymbolicName=ERROR_ERRORS_ENCOUNTERED
Language=English
One or more errors occurred while processing the request.
.
Language=Japanese
要求の処理中に1つ以上のエラーが発生しました。
.

MessageId=775
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_CAPABLE
Language=English
The implementation is not capable of performing the request.
.
Language=Japanese
実装が要求を実行できません。
.

MessageId=776
Severity=Success
Facility=System
SymbolicName=ERROR_REQUEST_OUT_OF_SEQUENCE
Language=English
The client of a component requested an operation which is not valid given the state of the component instance.
.
Language=Japanese
コンポーネントのクライアントが、コンポーネント インスタンスの状態を考慮して有効でない操作を要求しました。
.

MessageId=777
Severity=Success
Facility=System
SymbolicName=ERROR_VERSION_PARSE_ERROR
Language=English
A version number could not be parsed.
.
Language=Japanese
バージョン番号を解析できませんでした。
.

MessageId=778
Severity=Success
Facility=System
SymbolicName=ERROR_BADSTARTPOSITION
Language=English
The iterator's start position is invalid.
.
Language=Japanese
イテレータの開始位置が無効です。
.

MessageId=994
Severity=Success
Facility=System
SymbolicName=ERROR_EA_ACCESS_DENIED
Language=English
Access to the extended attribute was denied.
.
Language=Japanese
拡張属性へのアクセスが拒否されました。
.

MessageId=995
Severity=Success
Facility=System
SymbolicName=ERROR_OPERATION_ABORTED
Language=English
The I/O operation has been aborted because of either a thread exit or an application request.
.
Language=Japanese
スレッド終了またはアプリケーション要求によりI/O操作が中断されました。
.

MessageId=996
Severity=Success
Facility=System
SymbolicName=ERROR_IO_INCOMPLETE
Language=English
Overlapped I/O event is not in a signaled state.
.
Language=Japanese
オーバーラップしたI/Oイベントがシグナルされた状態にありません。
.

MessageId=997
Severity=Success
Facility=System
SymbolicName=ERROR_IO_PENDING
Language=English
Overlapped I/O operation is in progress.
.
Language=Japanese
オーバーラップI/Oオペレーションが進行中です。
.

MessageId=998
Severity=Success
Facility=System
SymbolicName=ERROR_NOACCESS
Language=English
Invalid access to memory location.
.
Language=Japanese
メモリー ロケーションへのアクセスが無効です。
.

MessageId=999
Severity=Success
Facility=System
SymbolicName=ERROR_SWAPERROR
Language=English
Error performing inpage operation.
.
Language=Japanese
ページ内操作の実行エラーです。
.

MessageId=1001
Severity=Success
Facility=System
SymbolicName=ERROR_STACK_OVERFLOW
Language=English
Recursion too deep; the stack overflowed.
.
Language=Japanese
再帰が深すぎます。スタックがオーバーフローしました。
.

MessageId=1002
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_MESSAGE
Language=English
The window cannot act on the sent message.
.
Language=Japanese
ウィンドウは送信されたメッセージを処理できません。
.

MessageId=1003
Severity=Success
Facility=System
SymbolicName=ERROR_CAN_NOT_COMPLETE
Language=English
Cannot complete this function.
.
Language=Japanese
この関数を完了できません。
.

MessageId=1004
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_FLAGS
Language=English
Invalid flags.
.
Language=Japanese
無効なフラグです。
.

MessageId=1005
Severity=Success
Facility=System
SymbolicName=ERROR_UNRECOGNIZED_VOLUME
Language=English
The volume does not contain a recognized file system. Please make sure that all required file system drivers are loaded and that the volume is not corrupted.
.
Language=Japanese
ボリュームに認識されるファイル システムが含まれていません。
必要なファイル システム ドライバがすべてロードされていること、およびボリュームが壊れていないことを確認してください。
.

MessageId=1006
Severity=Success
Facility=System
SymbolicName=ERROR_FILE_INVALID
Language=English
The volume for a file has been externally altered so that the opened file is no longer valid.
.
Language=Japanese
ファイルのボリュームが外部から変更されたため、オープンされたファイルが無効になりました。
.

MessageId=1007
Severity=Success
Facility=System
SymbolicName=ERROR_FULLSCREEN_MODE
Language=English
The requested operation cannot be performed in full-screen mode.
.
Language=Japanese
要求された操作はフルスクリーンモードでは実行できません。
.

MessageId=1008
Severity=Success
Facility=System
SymbolicName=ERROR_NO_TOKEN
Language=English
An attempt was made to reference a token that does not exist.
.
Language=Japanese
存在しないトークンを参照しようとしました。
.

MessageId=1009
Severity=Success
Facility=System
SymbolicName=ERROR_BADDB
Language=English
The configuration registry database is corrupt.
.
Language=Japanese
構成レジストリ データベースが壊れています。
.

MessageId=1010
Severity=Success
Facility=System
SymbolicName=ERROR_BADKEY
Language=English
The configuration registry key is invalid.
.
Language=Japanese
構成レジストリ キーが無効です。
.

MessageId=1011
Severity=Success
Facility=System
SymbolicName=ERROR_CANTOPEN
Language=English
The configuration registry key could not be opened.
.
Language=Japanese
構成レジストリ キーを開けませんでした。
.

MessageId=1012
Severity=Success
Facility=System
SymbolicName=ERROR_CANTREAD
Language=English
The configuration registry key could not be read.
.
Language=Japanese
構成レジストリ キーを読み取れませんでした。
.

MessageId=1013
Severity=Success
Facility=System
SymbolicName=ERROR_CANTWRITE
Language=English
The configuration registry key could not be written.
.
Language=Japanese
構成レジストリ キーを書き込めませんでした。
.

MessageId=1014
Severity=Success
Facility=System
SymbolicName=ERROR_REGISTRY_RECOVERED
Language=English
One of the files in the registry database had to be recovered by use of a log or alternate copy. The recovery was successful.
.
Language=Japanese
レジストリ データベースのファイルの 1 つは、ログまたは代替コピーを使用して回復する必要がありました。回復に成功しました。
.

MessageId=1015
Severity=Success
Facility=System
SymbolicName=ERROR_REGISTRY_CORRUPT
Language=English
The registry is corrupted. The structure of one of the files containing registry data is corrupted, or the system's memory image of the file is corrupted, or the file could not be recovered because the alternate copy or log was absent or corrupted.
.
Language=Japanese
レジストリが壊れています。レジストリ データを含むファイルの構造が壊れているか、ファイルのシステム キャッシュ メモリ イメージが壊れているか、代替コピーまたはログが存在しないか壊れているため、ファイルを回復できませんでした。
.

MessageId=1016
Severity=Success
Facility=System
SymbolicName=ERROR_REGISTRY_IO_FAILED
Language=English
An I/O operation initiated by the registry failed unrecoverably. The registry could not read in, or write out, or flush, one of the files that contain the system's image of the registry.
.
Language=Japanese
レジストリによって開始されたI/O操作が回復不能に失敗しました。レジストリが、レジストリのシステムイメージを含むファイルの1つを読み込めなかったか、書き込めなかったか、フラッシュできなかった。
.

MessageId=1017
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_REGISTRY_FILE
Language=English
The system has attempted to load or restore a file into the registry, but the specified file is not in a registry file format.
.
Language=Japanese
レジストリにファイルをロードまたは復元しようとしましたが、指定されたファイルはレジストリファイル形式ではありません。
.

MessageId=1018
Severity=Success
Facility=System
SymbolicName=ERROR_KEY_DELETED
Language=English
Illegal operation attempted on a registry key that has been marked for deletion.
.
Language=Japanese
削除マークが付けられたレジストリ キーに対して不正な操作を試みました。
.

MessageId=1019
Severity=Success
Facility=System
SymbolicName=ERROR_NO_LOG_SPACE
Language=English
System could not allocate the required space in a registry log.
.
Language=Japanese
レジストリ ログに必要な領域を割り当てられませんでした。
.

MessageId=1020
Severity=Success
Facility=System
SymbolicName=ERROR_KEY_HAS_CHILDREN
Language=English
Cannot create a symbolic link in a registry key that already has subkeys or values.
.
Language=Japanese
すでにサブキーまたは値を持つレジストリ キーにシンボリック リンクを作成できません。
.

MessageId=1021
Severity=Success
Facility=System
SymbolicName=ERROR_CHILD_MUST_BE_VOLATILE
Language=English
Cannot create a stable subkey under a volatile parent key.
.
Language=Japanese
揮発性の親キーの下に安定したサブキーを作成できません。
.

MessageId=1022
Severity=Success
Facility=System
SymbolicName=ERROR_NOTIFY_ENUM_DIR
Language=English
A notify change request is being completed and the information is not being returned in the caller's buffer. The caller now needs to enumerate the files to find the changes.
.
Language=Japanese
通知変更要求が完了しましたが、呼び出し元のバッファに情報が返されていません。呼び出し元は変更点を見つけるためにファイルを列挙する必要があります。
.

MessageId=1051
Severity=Success
Facility=System
SymbolicName=ERROR_DEPENDENT_SERVICES_RUNNING
Language=English
A stop control has been sent to a service that other running services are dependent on.
.
Language=Japanese
他の実行中のサービスが依存しているサービスに停止制御が送信されました。
.

MessageId=1052
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SERVICE_CONTROL
Language=English
The requested control is not valid for this service.
.
Language=Japanese
要求されたコントロールは、このサービスに対して有効ではありません。
.

MessageId=1053
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_REQUEST_TIMEOUT
Language=English
The service did not respond to the start or control request in a timely fashion.
.
Language=Japanese
サービスがタイムリーに開始または制御要求に応答しませんでした。
.

MessageId=1054
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_NO_THREAD
Language=English
A thread could not be created for the service.
.
Language=Japanese
サービスにスレッドを作成できませんでした。
.

MessageId=1055
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_DATABASE_LOCKED
Language=English
The service database is locked.
.
Language=Japanese
サービスデータベースがロックされました。
.

MessageId=1056
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_ALREADY_RUNNING
Language=English
An instance of the service is already running.
.
Language=Japanese
サービスのインスタンスがすでに実行されています。
.

MessageId=1057
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SERVICE_ACCOUNT
Language=English
The account name is invalid or does not exist, or the password is invalid for the account name specified.
.
Language=Japanese
アカウント名が無効か存在しないか、指定されたアカウント名のパスワードが無効です。
.

MessageId=1058
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_DISABLED
Language=English
The service cannot be started, either because it is disabled or because it has no enabled devices associated with it.
.
Language=Japanese
サービスが無効になっているか、有効なデバイスが関連付けられていないため、サービスを開始できません。
.

MessageId=1059
Severity=Success
Facility=System
SymbolicName=ERROR_CIRCULAR_DEPENDENCY
Language=English
Circular service dependency was specified.
.
Language=Japanese
円形のサービス依存関係が指定されました。
.

MessageId=1060
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_DOES_NOT_EXIST
Language=English
The specified service does not exist as an installed service.
.
Language=Japanese
指定されたサービスがインストールされたサービスとして存在しません。
.

MessageId=1061
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_CANNOT_ACCEPT_CTRL
Language=English
The service cannot accept control messages at this time.
.
Language=Japanese
サービスは現在、制御メッセージを受け付けることができません。
.

MessageId=1062
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_NOT_ACTIVE
Language=English
The service has not been started.
.
Language=Japanese
サービスが開始されていません。
.

MessageId=1063
Severity=Success
Facility=System
SymbolicName=ERROR_FAILED_SERVICE_CONTROLLER_CONNECT
Language=English
The service process could not connect to the service controller.
.
Language=Japanese
サービスプロセスがサービスコントローラに接続できませんでした。
.

MessageId=1064
Severity=Success
Facility=System
SymbolicName=ERROR_EXCEPTION_IN_SERVICE
Language=English
An exception occurred in the service when handling the control request.
.
Language=Japanese
制御要求の処理中にサービスで例外が発生しました。
.

MessageId=1065
Severity=Success
Facility=System
SymbolicName=ERROR_DATABASE_DOES_NOT_EXIST
Language=English
The database specified does not exist.
.
Language=Japanese
指定されたデータベースが存在しません。
.

MessageId=1066
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_SPECIFIC_ERROR
Language=English
The service has returned a service-specific error code.
.
Language=Japanese
サービス固有のエラーコードが返されました。
.

MessageId=1067
Severity=Success
Facility=System
SymbolicName=ERROR_PROCESS_ABORTED
Language=English
The process terminated unexpectedly.
.
Language=Japanese
プロセスが予期せず終了しました。
.

MessageId=1068
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_DEPENDENCY_FAIL
Language=English
The dependency service or group failed to start.
.
Language=Japanese
依存サービスまたはグループが開始できませんでした。
.

MessageId=1069
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_LOGON_FAILED
Language=English
The service did not start due to a logon failure.
.
Language=Japanese
ログオンの失敗によりサービスが開始されませんでした。
.

MessageId=1070
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_START_HANG
Language=English
After starting, the service hung in a start-pending state.
.
Language=Japanese
開始後、サービスが開始待ち状態でハングしました。
.

MessageId=1071
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SERVICE_LOCK
Language=English
The specified service database lock is invalid.
.
Language=Japanese
指定されたサービス データベース ロックが無効です。
.

MessageId=1072
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_MARKED_FOR_DELETE
Language=English
The specified service has been marked for deletion.
.
Language=Japanese
指定されたサービスは削除マークが付けられました。
.

MessageId=1073
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_EXISTS
Language=English
The specified service already exists.
.
Language=Japanese
指定されたサービスは既に存在します。
.

MessageId=1074
Severity=Success
Facility=System
SymbolicName=ERROR_ALREADY_RUNNING_LKG
Language=English
The system is currently running with the last-known-good configuration.
.
Language=Japanese
システムは現在、最終既知の構成で動作しています。
.

MessageId=1075
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_DEPENDENCY_DELETED
Language=English
The dependency service does not exist or has been marked for deletion.
.
Language=Japanese
依存サービスが存在しないか、削除マークが付けられています。
.

MessageId=1076
Severity=Success
Facility=System
SymbolicName=ERROR_BOOT_ALREADY_ACCEPTED
Language=English
The current boot has already been accepted for use as the last-known-good control set.
.
Language=Japanese
現在のブートは、Last-Known-Goodコントロールセットとして使用するために既に受け入れられました。
.

MessageId=1077
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_NEVER_STARTED
Language=English
No attempts to start the service have been made since the last boot.
.
Language=Japanese
最後のブート以降、サービスの開始を試みていません。
.

MessageId=1078
Severity=Success
Facility=System
SymbolicName=ERROR_DUPLICATE_SERVICE_NAME
Language=English
The name is already in use as either a service name or a service display name.
.
Language=Japanese
サービス名またはサービス表示名として既に使用されています。
.

MessageId=1079
Severity=Success
Facility=System
SymbolicName=ERROR_DIFFERENT_SERVICE_ACCOUNT
Language=English
The account specified for this service is different from the account specified for other services running in the same process.
.
Language=Japanese
このサービスに指定されたアカウントが、同じプロセスで実行されている他のサービスに指定されたアカウントと異なります。
.

MessageId=1080
Severity=Success
Facility=System
SymbolicName=ERROR_CANNOT_DETECT_DRIVER_FAILURE
Language=English
Failure actions can only be set for Win32 services, not for drivers.
.
Language=Japanese
失敗アクションはWin32サービスに対してのみ設定でき、ドライバに対しては設定できません。
.

MessageId=1081
Severity=Success
Facility=System
SymbolicName=ERROR_CANNOT_DETECT_PROCESS_ABORT
Language=English
This service runs in the same process as the service control manager. Therefore, the service control manager cannot take action if this service's process terminates unexpectedly.
.
Language=Japanese
このサービスはサービスコントロールマネージャと同じプロセスで実行されます。
そのため、このサービスのプロセスが予期せず終了した場合、サービスコントロールマネージャはアクションを実行できません。
.

MessageId=1082
Severity=Success
Facility=System
SymbolicName=ERROR_NO_RECOVERY_PROGRAM
Language=English
No recovery program has been configured for this service.
.
Language=Japanese
このサービスに回復プログラムが設定されていません。
.

MessageId=1083
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_NOT_IN_EXE
Language=English
The executable program that this service is configured to run in does not implement the service.
.
Language=Japanese
このサービスが実行されるように構成されている実行可能プログラムは、サービスを実装していません。
.

MessageId=1084
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_SAFEBOOT_SERVICE
Language=English
This service cannot be started in Safe Mode.
.
Language=Japanese
このサービスはセーフ モードで起動できません。
.

MessageId=1100
Severity=Success
Facility=System
SymbolicName=ERROR_END_OF_MEDIA
Language=English
The physical end of the tape has been reached.
.
Language=Japanese
テープの物理的な終端に達しました。
.

MessageId=1101
Severity=Success
Facility=System
SymbolicName=ERROR_FILEMARK_DETECTED
Language=English
A tape access reached a filemark.
.
Language=Japanese
テープ アクセスがファイルマークに到達しました。
.

MessageId=1102
Severity=Success
Facility=System
SymbolicName=ERROR_BEGINNING_OF_MEDIA
Language=English
The beginning of the tape or a partition was encountered.
.
Language=Japanese
テープの先頭またはパーティションに遭遇しました。
.

MessageId=1103
Severity=Success
Facility=System
SymbolicName=ERROR_SETMARK_DETECTED
Language=English
A tape access reached the end of a set of files.
.
Language=Japanese
テープ アクセスがファイル セットの最後に到達しました。
.

MessageId=1104
Severity=Success
Facility=System
SymbolicName=ERROR_NO_DATA_DETECTED
Language=English
No more data is on the tape.
.
Language=Japanese
テープにデータがありません。
.

MessageId=1105
Severity=Success
Facility=System
SymbolicName=ERROR_PARTITION_FAILURE
Language=English
Tape could not be partitioned.
.
Language=Japanese
テープをパーティション分割できませんでした。
.

MessageId=1106
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_BLOCK_LENGTH
Language=English
When accessing a new tape of a multivolume partition, the current block size is incorrect.
.
Language=Japanese
マルチボリューム パーティションの新しいテープにアクセスすると、現在のブロック サイズが正しくありません。
.

MessageId=1107
Severity=Success
Facility=System
SymbolicName=ERROR_DEVICE_NOT_PARTITIONED
Language=English
Tape partition information could not be found when loading a tape.
.
Language=Japanese
テープの読み込み時にテープ パーティション情報が見つかりませんでした。
.

MessageId=1108
Severity=Success
Facility=System
SymbolicName=ERROR_UNABLE_TO_LOCK_MEDIA
Language=English
Unable to lock the media eject mechanism.
.
Language=Japanese
メディア取り出し機構をロックできません。
.

MessageId=1109
Severity=Success
Facility=System
SymbolicName=ERROR_UNABLE_TO_UNLOAD_MEDIA
Language=English
Unable to unload the media.
.
Language=Japanese
メディアをアンロードできません。
.

MessageId=1110
Severity=Success
Facility=System
SymbolicName=ERROR_MEDIA_CHANGED
Language=English
The media in the drive may have changed.
.
Language=Japanese
ドライブ内のメディアが変更された可能性があります。
.

MessageId=1111
Severity=Success
Facility=System
SymbolicName=ERROR_BUS_RESET
Language=English
The I/O bus was reset.
.
Language=Japanese
I/O バスがリセットされました。
.

MessageId=1112
Severity=Success
Facility=System
SymbolicName=ERROR_NO_MEDIA_IN_DRIVE
Language=English
No media in drive.
.
Language=Japanese
ドライブにメディアがありません。
.

MessageId=1113
Severity=Success
Facility=System
SymbolicName=ERROR_NO_UNICODE_TRANSLATION
Language=English
No mapping for the Unicode character exists in the target multi-byte code page.
.
Language=Japanese
ターゲットのマルチバイト コード ページにUnicode文字のマッピングがありません。
.

MessageId=1114
Severity=Success
Facility=System
SymbolicName=ERROR_DLL_INIT_FAILED
Language=English
A dynamic link library (DLL) initialization routine failed.
.
Language=Japanese
動的リンク ライブラリ (DLL) の初期化ルーチンが失敗しました。
.

MessageId=1115
Severity=Success
Facility=System
SymbolicName=ERROR_SHUTDOWN_IN_PROGRESS
Language=English
A system shutdown is in progress.
.
Language=Japanese
システムのシャットダウンが進行中です。
.

MessageId=1116
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SHUTDOWN_IN_PROGRESS
Language=English
Unable to abort the system shutdown because no shutdown was in progress.
.
Language=Japanese
シャットダウンが進行中でないため、システムのシャットダウンを中止できません。
.

MessageId=1117
Severity=Success
Facility=System
SymbolicName=ERROR_IO_DEVICE
Language=English
The request could not be performed because of an I/O device error.
.
Language=Japanese
要求はI/Oデバイスエラーのため実行できませんでした。
.

MessageId=1118
Severity=Success
Facility=System
SymbolicName=ERROR_SERIAL_NO_DEVICE
Language=English
No serial device was successfully initialized. The serial driver will unload.
.
Language=Japanese
シリアル デバイスが正常に初期化されませんでした。シリアル ドライバはアンロードします。
.

MessageId=1119
Severity=Success
Facility=System
SymbolicName=ERROR_IRQ_BUSY
Language=English
Unable to open a device that was sharing an interrupt request (IRQ) with other devices. At least one other device that uses that IRQ was already opened.
.
Language=Japanese
他のデバイスと割り込み要求 (IRQ)  を共有しているデバイスを開くことができません。そのIRQを使用する少なくとも1つの他のデバイスがすでにオープンされています。
.

MessageId=1120
Severity=Success
Facility=System
SymbolicName=ERROR_MORE_WRITES
Language=English
A serial I/O operation was completed by another write to the serial port. (The IOCTL_SERIAL_XOFF_COUNTER reached zero.)
.
Language=Japanese
シリアルI/O操作がシリアルポートへの別の書き込みによって完了しました。
 (IOCTL_SERIAL_XOFF_COUNTERが0になりました。)
.

MessageId=1121
Severity=Success
Facility=System
SymbolicName=ERROR_COUNTER_TIMEOUT
Language=English
A serial I/O operation completed because the timeout period expired. (The IOCTL_SERIAL_XOFF_COUNTER did not reach zero.)
.
Language=Japanese
タイムアウト期間が切れたため、シリアルI/Oオペレーションが完了しました (IOCTL_SERIAL_XOFF_COUNTERが0になりませんでした)。
.

MessageId=1122
Severity=Success
Facility=System
SymbolicName=ERROR_FLOPPY_ID_MARK_NOT_FOUND
Language=English
No ID address mark was found on the floppy disk.
.
Language=Japanese
フロッピーディスクにIDアドレスマークが見つかりませんでした。
.

MessageId=1123
Severity=Success
Facility=System
SymbolicName=ERROR_FLOPPY_WRONG_CYLINDER
Language=English
Mismatch between the floppy disk sector ID field and the floppy disk controller track address.
.
Language=Japanese
フロッピーディスクのセクタIDフィールドとフロッピーディスクコントローラのトラックアドレスが一致しません。
.

MessageId=1124
Severity=Success
Facility=System
SymbolicName=ERROR_FLOPPY_UNKNOWN_ERROR
Language=English
The floppy disk controller reported an error that is not recognized by the floppy disk driver.
.
Language=Japanese
フロッピーディスクコントローラから、フロッピーディスクドライバが認識できないエラーが報告されました。
.

MessageId=1125
Severity=Success
Facility=System
SymbolicName=ERROR_FLOPPY_BAD_REGISTERS
Language=English
The floppy disk controller returned inconsistent results in its registers.
.
Language=Japanese
フロッピーディスクコントローラがレジスタに矛盾した結果を返しました。
.

MessageId=1126
Severity=Success
Facility=System
SymbolicName=ERROR_DISK_RECALIBRATE_FAILED
Language=English
While accessing the hard disk, a recalibrate operation failed, even after retries.
.
Language=Japanese
ハードディスクにアクセス中に、再試行しても再キャリブレーションに失敗しました。
.

MessageId=1127
Severity=Success
Facility=System
SymbolicName=ERROR_DISK_OPERATION_FAILED
Language=English
While accessing the hard disk, a disk operation failed even after retries.
.
Language=Japanese
ハードディスクにアクセス中、再試行してもディスク操作に失敗しました。
.

MessageId=1128
Severity=Success
Facility=System
SymbolicName=ERROR_DISK_RESET_FAILED
Language=English
While accessing the hard disk, a disk controller reset was needed, but even that failed.
.
Language=Japanese
ハードディスクへのアクセス中にディスクコントローラのリセットが必要でしたが、それにも失敗しました。
.

MessageId=1129
Severity=Success
Facility=System
SymbolicName=ERROR_EOM_OVERFLOW
Language=English
Physical end of tape encountered.
.
Language=Japanese
テープの物理的な終端に遭遇しました。
.

MessageId=1130
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_ENOUGH_SERVER_MEMORY
Language=English
Not enough server storage is available to process this command.
.
Language=Japanese
このコマンドを処理するのに十分なサーバー メモリ リソースがありません。
.

MessageId=1131
Severity=Success
Facility=System
SymbolicName=ERROR_POSSIBLE_DEADLOCK
Language=English
A potential deadlock condition has been detected.
.
Language=Japanese
デッドロックの可能性が検出されました。
.

MessageId=1132
Severity=Success
Facility=System
SymbolicName=ERROR_MAPPED_ALIGNMENT
Language=English
The base address or the file offset specified does not have the proper alignment.
.
Language=Japanese
指定されたベースアドレスまたはファイルオフセットに適切なアライメントがありません。
.

MessageId=1140
Severity=Success
Facility=System
SymbolicName=ERROR_SET_POWER_STATE_VETOED
Language=English
An attempt to change the system power state was vetoed by another application or driver.
.
Language=Japanese
システムの電源状態を変更しようとしましたが、他のアプリケーションまたはドライバによって拒否されました。
.

MessageId=1141
Severity=Success
Facility=System
SymbolicName=ERROR_SET_POWER_STATE_FAILED
Language=English
The system BIOS failed an attempt to change the system power state.
.
Language=Japanese
システム BIOS がシステム電源状態の変更の試みに失敗しました。
.

MessageId=1142
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_LINKS
Language=English
An attempt was made to create more links on a file than the file system supports.
.
Language=Japanese
ファイル システムでサポートされているよりも多くのリンクをファイルに作成しようとしました。
.

MessageId=1150
Severity=Success
Facility=System
SymbolicName=ERROR_OLD_WIN_VERSION
Language=English
The specified program requires a newer version of ReliefWin.
.
Language=Japanese
指定されたプログラムには新しいバージョンの ReliefWin が必要です。
.

MessageId=1151
Severity=Success
Facility=System
SymbolicName=ERROR_APP_WRONG_OS
Language=English
The specified program is not a Windows or MS-DOS program.
.
Language=Japanese
指定されたプログラムは ReliefWin または MS-DOS プログラムではありません。
.

MessageId=1152
Severity=Success
Facility=System
SymbolicName=ERROR_SINGLE_INSTANCE_APP
Language=English
Cannot start more than one instance of the specified program.
.
Language=Japanese
指定されたプログラムの複数のインスタンスを起動できません。
.

MessageId=1153
Severity=Success
Facility=System
SymbolicName=ERROR_RMODE_APP
Language=English
The specified program was written for an earlier version of ReliefWin.
.
Language=Japanese
指定されたプログラムは以前のバージョンの ReliefWin 用に作成されました。
.

MessageId=1154
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_DLL
Language=English
One of the library files needed to run this application is damaged.
.
Language=Japanese
このアプリケーションを実行するために必要なライブラリ ファイルの 1 つが壊れています。
.

MessageId=1155
Severity=Success
Facility=System
SymbolicName=ERROR_NO_ASSOCIATION
Language=English
No application is associated with the specified file for this operation.
.
Language=Japanese
この操作で指定されたファイルに関連付けられたアプリケーションはありません。
.

MessageId=1156
Severity=Success
Facility=System
SymbolicName=ERROR_DDE_FAIL
Language=English
An error occurred in sending the command to the application.
.
Language=Japanese
アプリケーションへのコマンド送信でエラーが発生しました。
.

MessageId=1157
Severity=Success
Facility=System
SymbolicName=ERROR_DLL_NOT_FOUND
Language=English
One of the library files needed to run this application cannot be found.
.
Language=Japanese
このアプリケーションの実行に必要なライブラリ ファイルが見つかりません。
.

MessageId=1158
Severity=Success
Facility=System
SymbolicName=ERROR_NO_MORE_USER_HANDLES
Language=English
The current process has used all of its system allowance of handles for Window Manager objects.
.
Language=Japanese
現在のプロセスがウィンドウ マネージャ オブジェクトのハンドルのシステム許容量をすべて使用しました。
.

MessageId=1159
Severity=Success
Facility=System
SymbolicName=ERROR_MESSAGE_SYNC_ONLY
Language=English
The message can be used only with synchronous operations.
.
Language=Japanese
メッセージは同期操作でのみ使用できます。
.

MessageId=1160
Severity=Success
Facility=System
SymbolicName=ERROR_SOURCE_ELEMENT_EMPTY
Language=English
The indicated source element has no media.
.
Language=Japanese
指定されたソース要素にメディアがありません。
.

MessageId=1161
Severity=Success
Facility=System
SymbolicName=ERROR_DESTINATION_ELEMENT_FULL
Language=English
The indicated destination element already contains media.
.
Language=Japanese
指定された宛先エレメントは既にメディアを含んでいます。
.

MessageId=1162
Severity=Success
Facility=System
SymbolicName=ERROR_ILLEGAL_ELEMENT_ADDRESS
Language=English
The indicated element does not exist.
.
Language=Japanese
指定された要素は存在しません。
.

MessageId=1163
Severity=Success
Facility=System
SymbolicName=ERROR_MAGAZINE_NOT_PRESENT
Language=English
The indicated element is part of a magazine that is not present.
.
Language=Japanese
指定された要素は存在しないマガジンの一部です。
.

MessageId=1164
Severity=Success
Facility=System
SymbolicName=ERROR_DEVICE_REINITIALIZATION_NEEDED
Language=English
The indicated device requires reinitialization due to hardware errors.
.
Language=Japanese
指定されたデバイスは、ハードウェア エラーにより再初期化が必要です。
.

MessageId=1165
Severity=Success
Facility=System
SymbolicName=ERROR_DEVICE_REQUIRES_CLEANING
Language=English
The device has indicated that cleaning is required before further operations are attempted.
.
Language=Japanese
デバイスは、さらなる操作を試みる前にクリーニングが必要であることを示しました。
.

MessageId=1166
Severity=Success
Facility=System
SymbolicName=ERROR_DEVICE_DOOR_OPEN
Language=English
The device has indicated that its door is open.
.
Language=Japanese
デバイスのドアが開いていることが確認されました。
.

MessageId=1167
Severity=Success
Facility=System
SymbolicName=ERROR_DEVICE_NOT_CONNECTED
Language=English
The device is not connected.
.
Language=Japanese
デバイスが接続されていません。
.

MessageId=1168
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_FOUND
Language=English
Element not found.
.
Language=Japanese
エレメントが見つかりません。
.

MessageId=1169
Severity=Success
Facility=System
SymbolicName=ERROR_NO_MATCH
Language=English
There was no match for the specified key in the index.
.
Language=Japanese
インデックスに指定されたキーに一致するものがありませんでした。
.

MessageId=1170
Severity=Success
Facility=System
SymbolicName=ERROR_SET_NOT_FOUND
Language=English
The property set specified does not exist on the object.
.
Language=Japanese
指定されたプロパティ セットがオブジェクトに存在しません。
.

MessageId=1171
Severity=Success
Facility=System
SymbolicName=ERROR_POINT_NOT_FOUND
Language=English
The point passed to GetMouseMovePointsEx is not in the buffer.
.
Language=Japanese
GetMouseMovePointsに渡されたポイントがバッファにありません。
.

MessageId=1172
Severity=Success
Facility=System
SymbolicName=ERROR_NO_TRACKING_SERVICE
Language=English
The tracking (workstation) service is not running.
.
Language=Japanese
トラッキング (ワークステーション) サービスが実行されていません。
.

MessageId=1173
Severity=Success
Facility=System
SymbolicName=ERROR_NO_VOLUME_ID
Language=English
The Volume ID could not be found.
.
Language=Japanese
ボリュームIDが見つかりません。
.

MessageId=1175
Severity=Success
Facility=System
SymbolicName=ERROR_UNABLE_TO_REMOVE_REPLACED
Language=English
Unable to remove the file to be replaced.
.
Language=Japanese
置換するファイルを削除できません。
.

MessageId=1176
Severity=Success
Facility=System
SymbolicName=ERROR_UNABLE_TO_MOVE_REPLACEMENT
Language=English
Unable to move the replacement file to the file to be replaced. The file to be replaced has retained its original name.
.
Language=Japanese
置換ファイルを置換対象ファイルに移動できません。置換するファイルは元の名前を保持しています。
.

MessageId=1177
Severity=Success
Facility=System
SymbolicName=ERROR_UNABLE_TO_MOVE_REPLACEMENT_2
Language=English
Unable to move the replacement file to the file to be replaced. The file to be replaced has been renamed using the backup name.
.
Language=Japanese
置換するファイルを置換するファイルに移動できません。置換対象のファイルはバックアップ名を使用して名前が変更されています。
.

MessageId=1178
Severity=Success
Facility=System
SymbolicName=ERROR_JOURNAL_DELETE_IN_PROGRESS
Language=English
The volume change journal is being deleted.
.
Language=Japanese
ボリューム変更ジャーナルが削除されました。
.

MessageId=1179
Severity=Success
Facility=System
SymbolicName=ERROR_JOURNAL_NOT_ACTIVE
Language=English
The volume change journal is not active.
.
Language=Japanese
ボリューム変更ジャーナルがアクティブではありません。
.

MessageId=1180
Severity=Success
Facility=System
SymbolicName=ERROR_POTENTIAL_FILE_FOUND
Language=English
A file was found, but it may not be the correct file.
.
Language=Japanese
ファイルが見つかりましたが、正しいファイルではないかもしれません。
.

MessageId=1181
Severity=Success
Facility=System
SymbolicName=ERROR_JOURNAL_ENTRY_DELETED
Language=English
The journal entry has been deleted from the journal.
.
Language=Japanese
ジャーナル エントリがジャーナルから削除されました。
.

MessageId=1200
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_DEVICE
Language=English
The specified device name is invalid.
.
Language=Japanese
指定されたデバイス名が無効です。
.

MessageId=1201
Severity=Success
Facility=System
SymbolicName=ERROR_CONNECTION_UNAVAIL
Language=English
The device is not currently connected but it is a remembered connection.
.
Language=Japanese
デバイスは現在接続されていませんが、記憶された接続です。
.

MessageId=1202
Severity=Success
Facility=System
SymbolicName=ERROR_DEVICE_ALREADY_REMEMBERED
Language=English
The local device name has a remembered connection to another network resource.
.
Language=Japanese
ローカルデバイス名は別のネットワークリソースへの接続を記憶しています。
.

MessageId=1203
Severity=Success
Facility=System
SymbolicName=ERROR_NO_NET_OR_BAD_PATH
Language=English
The network path was either typed incorrectly, does not exist, or the network provider is not currently available. Please try retyping the path or contact your network administrator.
.
Language=Japanese
ネットワーク パスの入力が間違っているか、存在しないか、ネットワーク プロバイダが現在利用できません。パスを再入力するか、ネットワーク管理者にお問い合わせください。
.

MessageId=1204
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_PROVIDER
Language=English
The specified network provider name is invalid.
.
Language=Japanese
指定されたネットワーク プロバイダ名が無効です。
.

MessageId=1205
Severity=Success
Facility=System
SymbolicName=ERROR_CANNOT_OPEN_PROFILE
Language=English
Unable to open the network connection profile.
.
Language=Japanese
ネットワーク接続プロファイルを開くことができません。
.

MessageId=1206
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_PROFILE
Language=English
The network connection profile is corrupted.
.
Language=Japanese
ネットワーク接続プロファイルが壊れています。
.

MessageId=1207
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_CONTAINER
Language=English
Cannot enumerate a noncontainer.
.
Language=Japanese
コンテナ以外を列挙できません。
.

MessageId=1208
Severity=Success
Facility=System
SymbolicName=ERROR_EXTENDED_ERROR
Language=English
An extended error has occurred.
.
Language=Japanese
拡張エラーが発生しました。
.

MessageId=1209
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_GROUPNAME
Language=English
The format of the specified group name is invalid.
.
Language=Japanese
指定されたグループ名の形式が無効です。
.

MessageId=1210
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_COMPUTERNAME
Language=English
The format of the specified computer name is invalid.
.
Language=Japanese
指定されたコンピュータ名の書式が無効です。
.

MessageId=1211
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_EVENTNAME
Language=English
The format of the specified event name is invalid.
.
Language=Japanese
指定されたイベント名の書式が無効です。
.

MessageId=1212
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_DOMAINNAME
Language=English
The format of the specified domain name is invalid.
.
Language=Japanese
指定されたドメイン名の書式が無効です。
.

MessageId=1213
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SERVICENAME
Language=English
The format of the specified service name is invalid.
.
Language=Japanese
指定されたサービス名の書式が無効です。
.

MessageId=1214
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_NETNAME
Language=English
The format of the specified network name is invalid.
.
Language=Japanese
指定されたネットワーク名の書式が無効です。
.

MessageId=1215
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SHARENAME
Language=English
The format of the specified share name is invalid.
.
Language=Japanese
指定された共有名の書式が無効です。
.

MessageId=1216
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PASSWORDNAME
Language=English
The format of the specified password is invalid.
.
Language=Japanese
指定されたパスワードの書式が無効です。
.

MessageId=1217
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_MESSAGENAME
Language=English
The format of the specified message name is invalid.
.
Language=Japanese
指定されたメッセージ名の書式が無効です。
.

MessageId=1218
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_MESSAGEDEST
Language=English
The format of the specified message destination is invalid.
.
Language=Japanese
指定されたメッセージ宛先の書式が無効です。
.

MessageId=1219
Severity=Success
Facility=System
SymbolicName=ERROR_SESSION_CREDENTIAL_CONFLICT
Language=English
Multiple connections to a server or shared resource by the same user, using more than one user name, are not allowed. Disconnect all previous connections to the server or shared resource and try again.
.
Language=Japanese
同一ユーザーによる、複数のユーザー名を使用したサーバーまたは共有リソースへの複数接続は許可されていません。サーバーまたは共有リソースへの以前の接続をすべて切断して再試行してください。
.

MessageId=1220
Severity=Success
Facility=System
SymbolicName=ERROR_REMOTE_SESSION_LIMIT_EXCEEDED
Language=English
An attempt was made to establish a session to a network server, but there are already too many sessions established to that server.
.
Language=Japanese
ネットワーク サーバーへのセッションを確立しようとしましたが、そのサーバーへのセッションがすでに多すぎます。
.

MessageId=1221
Severity=Success
Facility=System
SymbolicName=ERROR_DUP_DOMAINNAME
Language=English
The workgroup or domain name is already in use by another computer on the network.
.
Language=Japanese
ワークグループ名またはドメイン名は、ネットワーク上の別のコンピュータによってすでに使用されています。
.

MessageId=1222
Severity=Success
Facility=System
SymbolicName=ERROR_NO_NETWORK
Language=English
The network is not present or not started.
.
Language=Japanese
ネットワークが存在しないか、開始されていません。
.

MessageId=1223
Severity=Success
Facility=System
SymbolicName=ERROR_CANCELLED
Language=English
The operation was canceled by the user.
.
Language=Japanese
ユーザーによって操作がキャンセルされました。
.

MessageId=1224
Severity=Success
Facility=System
SymbolicName=ERROR_USER_MAPPED_FILE
Language=English
The requested operation cannot be performed on a file with a user-mapped section open.
.
Language=Japanese
要求された操作は、ユーザ マップされたセクションが開いているファイルでは実行できません。
.

MessageId=1225
Severity=Success
Facility=System
SymbolicName=ERROR_CONNECTION_REFUSED
Language=English
The remote system refused the network connection.
.
Language=Japanese
リモート コンピューターがネットワーク接続を拒否しました。
.

MessageId=1226
Severity=Success
Facility=System
SymbolicName=ERROR_GRACEFUL_DISCONNECT
Language=English
The network connection was gracefully closed.
.
Language=Japanese
ネットワーク接続が強制終了されました。
.

MessageId=1227
Severity=Success
Facility=System
SymbolicName=ERROR_ADDRESS_ALREADY_ASSOCIATED
Language=English
The network transport endpoint already has an address associated with it.
.
Language=Japanese
ネットワーク トランスポート エンドポイントには、すでにアドレスが関連付けられています。
.

MessageId=1228
Severity=Success
Facility=System
SymbolicName=ERROR_ADDRESS_NOT_ASSOCIATED
Language=English
An address has not yet been associated with the network endpoint.
.
Language=Japanese
ネットワークエンドポイントにアドレスがまだ関連付けられていません。
.

MessageId=1229
Severity=Success
Facility=System
SymbolicName=ERROR_CONNECTION_INVALID
Language=English
An operation was attempted on a nonexistent network connection.
.
Language=Japanese
存在しないネットワーク接続に対して操作を試みました。
.

MessageId=1230
Severity=Success
Facility=System
SymbolicName=ERROR_CONNECTION_ACTIVE
Language=English
An invalid operation was attempted on an active network connection.
.
Language=Japanese
有効なネットワーク接続に対して無効な操作を試行しました。
.

MessageId=1231
Severity=Success
Facility=System
SymbolicName=ERROR_NETWORK_UNREACHABLE
Language=English
The network location cannot be reached. For information about network troubleshooting, see ReactOS Help.
.
Language=Japanese
ネットワークの場所にアクセスできません。ネットワークのトラブルシューティングについては、ReliefWinヘルプを参照してください。
.

MessageId=1232
Severity=Success
Facility=System
SymbolicName=ERROR_HOST_UNREACHABLE
Language=English
The network location cannot be reached. For information about network troubleshooting, see ReactOS Help.
.
Language=Japanese
ネットワークの場所にアクセスできません。ネットワークのトラブルシューティングについては、ReliefWinヘルプを参照してください。
.

MessageId=1233
Severity=Success
Facility=System
SymbolicName=ERROR_PROTOCOL_UNREACHABLE
Language=English
The network location cannot be reached. For information about network troubleshooting, see ReactOS Help.
.
Language=Japanese
ネットワークの場所にアクセスできません。ネットワークのトラブルシューティングについては、ReliefWinヘルプを参照してください。
.

MessageId=1234
Severity=Success
Facility=System
SymbolicName=ERROR_PORT_UNREACHABLE
Language=English
No service is operating at the destination network endpoint on the remote system.
.
Language=Japanese
リモート システムの宛先ネットワーク エンドポイントで動作しているサービスはありません。
.

MessageId=1235
Severity=Success
Facility=System
SymbolicName=ERROR_REQUEST_ABORTED
Language=English
The request was aborted.
.
Language=Japanese
要求は中止されました。
.

MessageId=1236
Severity=Success
Facility=System
SymbolicName=ERROR_CONNECTION_ABORTED
Language=English
The network connection was aborted by the local system.
.
Language=Japanese
ネットワーク接続がローカルシステムによって中止されました。
.

MessageId=1237
Severity=Success
Facility=System
SymbolicName=ERROR_RETRY
Language=English
The operation could not be completed. A retry should be performed.
.
Language=Japanese
操作を完了できませんでした。再試行が必要です。
.

MessageId=1238
Severity=Success
Facility=System
SymbolicName=ERROR_CONNECTION_COUNT_LIMIT
Language=English
A connection to the server could not be made because the limit on the number of concurrent connections for this account has been reached.
.
Language=Japanese
このアカウントの同時接続数の制限に達したため、サーバーに接続できませんでした。
.

MessageId=1239
Severity=Success
Facility=System
SymbolicName=ERROR_LOGIN_TIME_RESTRICTION
Language=English
Attempting to log in during an unauthorized time of day for this account.
.
Language=Japanese
このアカウントで許可されていない時間帯にログインを試みています。
.

MessageId=1240
Severity=Success
Facility=System
SymbolicName=ERROR_LOGIN_WKSTA_RESTRICTION
Language=English
The account is not authorized to log in from this station.
.
Language=Japanese
このアカウントは、このステーションからログインする権限がありません。
.

MessageId=1241
Severity=Success
Facility=System
SymbolicName=ERROR_INCORRECT_ADDRESS
Language=English
The network address could not be used for the operation requested.
.
Language=Japanese
ネットワークアドレスは要求された操作に使用できませんでした。
.

MessageId=1242
Severity=Success
Facility=System
SymbolicName=ERROR_ALREADY_REGISTERED
Language=English
The service is already registered.
.
Language=Japanese
サービスはすでに登録されています。
.

MessageId=1243
Severity=Success
Facility=System
SymbolicName=ERROR_SERVICE_NOT_FOUND
Language=English
The specified service does not exist.
.
Language=Japanese
指定されたサービスは存在しません。
.

MessageId=1244
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_AUTHENTICATED
Language=English
The operation being requested was not performed because the user has not been authenticated.
.
Language=Japanese
ユーザーが認証されていないため、要求された操作は実行されませんでした。
.

MessageId=1245
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_LOGGED_ON
Language=English
The operation being requested was not performed because the user has not logged on to the network. The specified service does not exist.
.
Language=Japanese
ユーザーがネットワークにログオンしていないため、要求された操作は実行されませんでした。指定されたサービスは存在しません。
.

MessageId=1246
Severity=Success
Facility=System
SymbolicName=ERROR_CONTINUE
Language=English
Continue with work in progress.
.
Language=Japanese
進行中の作業を続行してください。
.

MessageId=1247
Severity=Success
Facility=System
SymbolicName=ERROR_ALREADY_INITIALIZED
Language=English
An attempt was made to perform an initialization operation when initialization has already been completed.
.
Language=Japanese
初期化がすでに完了しているときに初期化操作を実行しようとしました。
.

MessageId=1248
Severity=Success
Facility=System
SymbolicName=ERROR_NO_MORE_DEVICES
Language=English
No more local devices.
.
Language=Japanese
ローカル デバイスがありません。
.

MessageId=1249
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SUCH_SITE
Language=English
The specified site does not exist.
.
Language=Japanese
指定されたサイトは存在しません。
.

MessageId=1250
Severity=Success
Facility=System
SymbolicName=ERROR_DOMAIN_CONTROLLER_EXISTS
Language=English
A domain controller with the specified name already exists.
.
Language=Japanese
指定された名前のドメイン コントローラーはすでに存在します。
.

MessageId=1251
Severity=Success
Facility=System
SymbolicName=ERROR_ONLY_IF_CONNECTED
Language=English
This operation is supported only when you are connected to the server.
.
Language=Japanese
この操作はサーバーに接続しているときのみサポートされます。
.

MessageId=1252
Severity=Success
Facility=System
SymbolicName=ERROR_OVERRIDE_NOCHANGES
Language=English
The group policy framework should call the extension even if there are no changes.
.
Language=Japanese
グループ ポリシー フレームワークは、変更がなくても拡張機能を呼び出す必要があります。
.

MessageId=1253
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_USER_PROFILE
Language=English
The specified user does not have a valid profile.
.
Language=Japanese
指定されたユーザーに有効なプロファイルがありません。
.

MessageId=1254
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_SUPPORTED_ON_SBS
Language=English
This operation is not supported on a computer running Windows Server 2003 for Small Business Server.
.
Language=Japanese
この操作は、ReliefWin Server を実行しているコンピュータではサポートされていません。
.

MessageId=1255
Severity=Success
Facility=System
SymbolicName=ERROR_SERVER_SHUTDOWN_IN_PROGRESS
Language=English
The server machine is shutting down.
.
Language=Japanese
サーバー マシンがシャットダウンしています。
.

MessageId=1256
Severity=Success
Facility=System
SymbolicName=ERROR_HOST_DOWN
Language=English
The remote system is not available. For information about network troubleshooting, see ReactOS Help.
.
Language=Japanese
リモート システムが使用できません。ネットワークのトラブルシューティングについては、ReliefWinヘルプを参照してください。
.

MessageId=1257
Severity=Success
Facility=System
SymbolicName=ERROR_NON_ACCOUNT_SID
Language=English
The security identifier provided is not from an account domain.
.
Language=Japanese
指定されたセキュリティ識別子はアカウント ドメインのものではありません。
.

MessageId=1258
Severity=Success
Facility=System
SymbolicName=ERROR_NON_DOMAIN_SID
Language=English
The security identifier provided does not have a domain component.
.
Language=Japanese
指定されたセキュリティ識別子にドメイン コンポーネントがありません。
.

MessageId=1259
Severity=Success
Facility=System
SymbolicName=ERROR_APPHELP_BLOCK
Language=English
AppHelp dialog canceled thus preventing the application from starting.
.
Language=Japanese
AppHelpダイアログがキャンセルされ、アプリケーションが起動できません。
.

MessageId=1260
Severity=Success
Facility=System
SymbolicName=ERROR_ACCESS_DISABLED_BY_POLICY
Language=English
ReliefWin cannot open this program because it has been prevented by a software restriction policy. For more information, open Event Viewer or contact your system administrator.
.
Language=Japanese
このプログラムはグループ ポリシーでブロックされています。詳細については、システム管理者にお問い合わせください。
.

MessageId=1261
Severity=Success
Facility=System
SymbolicName=ERROR_REG_NAT_CONSUMPTION
Language=English
A program attempt to use an invalid register value. Normally caused by an uninitialized register. This error is Itanium specific.
.
Language=Japanese
プログラムが無効なレジスタ値を使用しようとしました。通常、初期化されていないレジスタが原因です。このエラーはItanium固有のものです。
.

MessageId=1262
Severity=Success
Facility=System
SymbolicName=ERROR_CSCSHARE_OFFLINE
Language=English
The share is currently offline or does not exist.
.
Language=Japanese
共有が現在オフラインであるか、存在しません。
.

MessageId=1263
Severity=Success
Facility=System
SymbolicName=ERROR_PKINIT_FAILURE
Language=English
The kerberos protocol encountered an error while validating the KDC certificate during smartcard logon.
.
Language=Japanese
スマートカードログオン中にKDC証明書を検証しているときに、Kerberosプロトコルでエラーが発生しました。システム イベント ログに詳細な情報があります。
.

MessageId=1264
Severity=Success
Facility=System
SymbolicName=ERROR_SMARTCARD_SUBSYSTEM_FAILURE
Language=English
The kerberos protocol encountered an error while attempting to utilize the smartcard subsystem.
.
Language=Japanese
スマートカードサブシステムを使用しようとしたときにKerberosプロトコルでエラーが発生しました。
.

MessageId=1265
Severity=Success
Facility=System
SymbolicName=ERROR_DOWNGRADE_DETECTED
Language=English
The system detected a possible attempt to compromise security. Please ensure that you can contact the server that authenticated you.
.
Language=Japanese
システムは認証要求を処理するドメイン コントローラーに接続できません。後で再試行してください。
.

MessageId=1266
Severity=Success
Facility=System
SymbolicName=SEC_E_SMARTCARD_CERT_REVOKED
Language=English
The smartcard certificate used for authentication has been revoked. Please contact your system administrator. There may be additional information in the event log.
.
Language=Japanese
認証に使用したスマートカード証明書が失効しました。システム管理者に連絡してください。イベントログに追加情報があるかもしれません。
.

MessageId=1267
Severity=Success
Facility=System
SymbolicName=SEC_E_ISSUING_CA_UNTRUSTED
Language=English
An untrusted certificate authority was detected while processing the smartcard certificate used for authentication. Please contact your system administrator.
.
Language=Japanese
認証に使用されたスマートカード証明書の処理中に、信頼されていない認証局が検出されました。システム管理者に連絡してください。
.

MessageId=1268
Severity=Success
Facility=System
SymbolicName=SEC_E_REVOCATION_OFFLINE_C
Language=English
The revocation status of the smartcard certificate used for authentication could not be determined. Please contact your system administrator.
.
Language=Japanese
認証に使用されているスマートカード証明書の失効ステータスを確認できませんでした。システム管理者にお問い合わせください。
.

MessageId=1269
Severity=Success
Facility=System
SymbolicName=SEC_E_PKINIT_CLIENT_FAILUR
Language=English
The smartcard certificate used for authentication was not trusted. Please contact your system administrator.
.
Language=Japanese
認証に使用されたスマートカード証明書が信頼されていません。システム管理者に連絡してください。
.

MessageId=1270
Severity=Success
Facility=System
SymbolicName=SEC_E_SMARTCARD_CERT_EXPIRED
Language=English
The smartcard certificate used for authentication has expired. Please contact your system administrator.
.
Language=Japanese
認証に使用したスマートカード証明書の有効期限が切れています。システム管理者に連絡してください。
.

MessageId=1271
Severity=Success
Facility=System
SymbolicName=ERROR_MACHINE_LOCKED
Language=English
The machine is locked and cannot be shut down without the force option.
.
Language=Japanese
マシンがロックされているため、強制オプションを使用しないとシャットダウンできません。
.

MessageId=1273
Severity=Success
Facility=System
SymbolicName=ERROR_CALLBACK_SUPPLIED_INVALID_DATA
Language=English
An application-defined callback gave invalid data when called.
.
Language=Japanese
アプリケーション定義のコールバックが呼び出されたときに無効なデータを与えました。
.

MessageId=1274
Severity=Success
Facility=System
SymbolicName=ERROR_SYNC_FOREGROUND_REFRESH_REQUIRED
Language=English
The group policy framework should call the extension in the synchronous foreground policy refresh.
.
Language=Japanese
グループ ポリシー フレームワークは、同期フォアグラウンド ポリシー更新で拡張機能を呼び出す必要があります。
.

MessageId=1275
Severity=Success
Facility=System
SymbolicName=ERROR_DRIVER_BLOCKED
Language=English
This driver has been blocked from loading.
.
Language=Japanese
このドライバのロードがブロックされました。
.

MessageId=1276
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_IMPORT_OF_NON_DLL
Language=English
A dynamic link library (DLL) referenced a module that was neither a DLL nor the process's executable image.
.
Language=Japanese
動的リンク ライブラリ (DLL) が、DLLでもプロセスの実行イメージでもないモジュールを参照しました。
.

MessageId=1277
Severity=Success
Facility=System
SymbolicName=ERROR_ACCESS_DISABLED_WEBBLADE
Language=English
ReliefWin cannot open this program since it has been disabled.
.
Language=Japanese
ReliefWinが無効になっているため、このプログラムを開くことができません。
.

MessageId=1278
Severity=Success
Facility=System
SymbolicName=ERROR_ACCESS_DISABLED_WEBBLADE_TAMPER
Language=English
ReliefWin cannot open this program because the license enforcement system has been tampered with or become corrupted.
.
Language=Japanese
ライセンス実施システムが改ざんされたか破損しているため、ReliefWin でこのプログラムを開くことができません。
.

MessageId=1279
Severity=Success
Facility=System
SymbolicName=ERROR_RECOVERY_FAILURE
Language=English
A transaction recovery failed.
.
Language=Japanese
トランザクションのリカバリに失敗しました。
.

MessageId=1280
Severity=Success
Facility=System
SymbolicName=ERROR_ALREADY_FIBER
Language=English
The current thread has already been converted to a fiber.
.
Language=Japanese
現在のスレッドはすでにファイバーに変換されています。
.

MessageId=1281
Severity=Success
Facility=System
SymbolicName=ERROR_ALREADY_THREAD
Language=English
The current thread has already been converted from a fiber.
.
Language=Japanese
現在のスレッドはファイバーから変換済みです。
.

MessageId=1282
Severity=Success
Facility=System
SymbolicName=ERROR_STACK_BUFFER_OVERRUN
Language=English
The system detected an overrun of a stack-based buffer in this application. This overrun could potentially allow a malicious user to gain control of this application.
.
Language=Japanese
このアプリケーションでスタックベースのバッファのオーバーランが検出されました。このオーバーランにより、悪意のあるユーザーがこのアプリケーションを制御できる可能性があります。
.

MessageId=1283
Severity=Success
Facility=System
SymbolicName=ERROR_PARAMETER_QUOTA_EXCEEDED
Language=English
Data present in one of the parameters is more than the function can operate on.
.
Language=Japanese
パラメータの1つに存在するデータは、関数が操作できる範囲を超えています。
.

MessageId=1284
Severity=Success
Facility=System
SymbolicName=ERROR_DEBUGGER_INACTIVE
Language=English
An attempt to do an operation on a debug object failed because the object is in the process of being deleted.
.
Language=Japanese
オブジェクトが削除中であるため、デバッグ オブジェクトに対する操作に失敗しました。
.

MessageId=1285
Severity=Success
Facility=System
SymbolicName=ERROR_DELAY_LOAD_FAILED
Language=English
An attempt to delay-load a .dll or get a function address in a delay-loaded .dll failed.
.
Language=Japanese
.dllの遅延ロードまたは遅延ロードされた.dll内の関数アドレスの取得に失敗しました。
.

MessageId=1286
Severity=Success
Facility=System
SymbolicName=ERROR_VDM_DISALLOWED
Language=English
%1 is a 16-bit application. You do not have permissions to execute 16-bit applications. Check your permissions with your system administrator.
.
Language=Japanese
%1 は 16 ビット アプリケーションです。16 ビット アプリケーションを実行する権限がありません。システム管理者に権限を確認してください。
.

MessageId=1287
Severity=Success
Facility=System
SymbolicName=ERROR_UNIDENTIFIED_ERROR
Language=English
Insufficient information exists to identify the cause of failure.
.
Language=Japanese
障害の原因を特定するための情報が不足しています。
.

MessageId=1288
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_BANDWIDTH_PARAMETERS
Language=English
An invalid budget or period parameter was specified.
.
Language=Japanese
C ランタイム関数に渡されたパラメーターが正しくありません。
.

MessageId=1289
Severity=Success
Facility=System
SymbolicName=ERROR_AFFINITY_NOT_COMPATIBLE
Language=English
An attempt was made to join a thread to a reserve whose affinity did not intersect the reserve affinity or an attempt was made to associate a process with a reserve whose affinity did not intersect the reserve affinity.
.
Language=Japanese
操作がファイルの有効なデータ長を超えて発生しました。
.

MessageId=1290
Severity=Success
Facility=System
SymbolicName=ERROR_THREAD_ALREADY_IN_RESERVE
Language=English
An attempt was made to join a thread to a reserve which was already joined to another reserve.
.
Language=Japanese
同じプロセス内の1つ以上のサービスのSIDタイプの設定が互換性がないため、サービスの開始に失敗しました。サービスSIDタイプが制限されたサービスは、制限されたSIDタイプを持つ他のサービスと同じプロセスでのみ共存できます。このサービスのサービスSIDタイプが構成されたばかりの場合は、このサービスを開始するためにホスト プロセスを再起動する必要があります。
.

MessageId=1291
Severity=Success
Facility=System
SymbolicName=ERROR_THREAD_NOT_IN_RESERVE
Language=English
An attempt was made to disjoin a thread from a reserve, but the thread was not joined to the reserve.
.
Language=Japanese
このサービスをホストしているプロセスは、このサービスを開始するために再起動する必要があります。
.

MessageId=1292
Severity=Success
Facility=System
SymbolicName=ERROR_THREAD_PROCESS_IN_RESERVE
Language=English
An attempt was made to disjoin a thread from a reserve whose process is associated with a reserve.
.
Language=Japanese
操作が実装で定義された制限を超えようとした。
.

MessageId=1293
Severity=Success
Facility=System
SymbolicName=ERROR_PROCESS_ALREADY_IN_RESERVE
Language=English
An attempt was made to associate a process with a reserve that was already associated with a reserve.
.
Language=Japanese
ターゲット プロセス、またはターゲット スレッドの含むプロセスのどちらかが保護されたプロセスです。
.

MessageId=1294
Severity=Success
Facility=System
SymbolicName=ERROR_PROCESS_NOT_IN_RESERVE
Language=English
An attempt was made to disassociate a process from a reserve, but the process did not have an associated reserve.
.
Language=Japanese
サービス通知クライアントがマシン内のサービスの現在の状態から遅れすぎています。
.

MessageId=1295
Severity=Success
Facility=System
SymbolicName=ERROR_PROCESS_THREADS_IN_RESERVE
Language=English
An attempt was made to associate a process with a reserve, but the process contained thread joined to a reserve.
.
Language=Japanese
ストレージクォータを超えているため、要求されたファイル操作に失敗しました。
ディスクスペースを確保するには、ファイルを別の場所に移動するか、不要なファイルを削除してください。詳細については、システム管理者にお問い合わせください。
.

MessageId=1296
Severity=Success
Facility=System
SymbolicName=ERROR_AFFINITY_NOT_SET_IN_RESERVE
Language=English
An attempt was made to set the affinity of a thread or a process, but the thread or process was joined or associated with a reserve.
.
Language=Japanese
ストレージ ポリシーでその種類のファイルがブロックされているため、要求されたファイル操作に失敗しました。詳細については、システム管理者にお問い合わせください。
.

MessageId=1297
Severity=Success
Facility=System
SymbolicName=ERROR_IMPLEMENTATION_LIMIT
Language=English
An operation attempted to exceed an implementation-defined limit.
.
Language=Japanese
サービス ReliefWin 管理コンソール (MMC) スナップイン (services.msc) およびローカルセキュリティ設定MMCスナップイン (secpol.msc) を使用して、サービス構成とアカウント構成を表示できます。
.

MessageId=1298
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CACHE_ONLY
Language=English
The requested object is for internal DS operations only.
.
Language=Japanese
この操作に関与しているスレッドが応答していないようです。
.

MessageId=1300
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_ALL_ASSIGNED
Language=English
Not all privileges referenced are assigned to the caller.
.
Language=Japanese
参照されたすべての特権またはグループが呼び出し元に割り当てられていません。
.

MessageId=1301
Severity=Success
Facility=System
SymbolicName=ERROR_SOME_NOT_MAPPED
Language=English
Some mapping between account names and security IDs was not done.
.
Language=Japanese
アカウント名とセキュリティIDのマッピングが行われていません。
.

MessageId=1302
Severity=Success
Facility=System
SymbolicName=ERROR_NO_QUOTAS_FOR_ACCOUNT
Language=English
No system quota limits are specifically set for this account.
.
Language=Japanese
システムのクォータ制限がこのアカウントに設定されていません。
.

MessageId=1303
Severity=Success
Facility=System
SymbolicName=ERROR_LOCAL_USER_SESSION_KEY
Language=English
No encryption key is available. A well-known encryption key was returned.
.
Language=Japanese
暗号化キーがありません。既知の暗号化キーが返されました。
.

MessageId=1304
Severity=Success
Facility=System
SymbolicName=ERROR_NULL_LM_PASSWORD
Language=English
The password is too complex to be converted to a LAN Manager password. The LAN Manager password returned is a NULL string.
.
Language=Japanese
パスワードが複雑すぎて LAN マネージャ パスワードに変換できません。返されたLAN マネージャ パスワードはNULL文字列です。
.

MessageId=1305
Severity=Success
Facility=System
SymbolicName=ERROR_UNKNOWN_REVISION
Language=English
The revision level is unknown.
.
Language=Japanese
リビジョン レベルが不明です。
.

MessageId=1306
Severity=Success
Facility=System
SymbolicName=ERROR_REVISION_MISMATCH
Language=English
Indicates two revision levels are incompatible.
.
Language=Japanese
2つのリビジョン レベルに互換性がないことを示します。
.

MessageId=1307
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_OWNER
Language=English
This security ID may not be assigned as the owner of this object.
.
Language=Japanese
このセキュリティ ID は、このオブジェクトの所有者として割り当てられていない可能性があります。
.

MessageId=1308
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PRIMARY_GROUP
Language=English
This security ID may not be assigned as the primary group of an object.
.
Language=Japanese
このセキュリティ ID は、オブジェクトのプライマリ グループとして割り当てられていません。
.

MessageId=1309
Severity=Success
Facility=System
SymbolicName=ERROR_NO_IMPERSONATION_TOKEN
Language=English
An attempt has been made to operate on an impersonation token by a thread that is not currently impersonating a client.
.
Language=Japanese
現在クライアントになりすましていないスレッドが、なりすましトークンを操作しようとしました。
.

MessageId=1310
Severity=Success
Facility=System
SymbolicName=ERROR_CANT_DISABLE_MANDATORY
Language=English
The group may not be disabled.
.
Language=Japanese
グループが無効になっていない可能性があります。
.

MessageId=1311
Severity=Success
Facility=System
SymbolicName=ERROR_NO_LOGON_SERVERS
Language=English
There are currently no logon servers available to service the logon request.
.
Language=Japanese
ドメインが利用可能でないため、このクレデンシャルでサインインできません。デバイスが組織のネットワークに接続されていることを確認して、再試行してください。以前に別の資格情報でこのデバイスにサインインした場合は、その資格情報でサインインできます。
.

MessageId=1312
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SUCH_LOGON_SESSION
Language=English
A specified logon session does not exist. It may already have been terminated.
.
Language=Japanese
指定されたログオンセッションが存在しません。すでに終了されている可能性があります。
.

MessageId=1313
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SUCH_PRIVILEGE
Language=English
A specified privilege does not exist.
.
Language=Japanese
指定された権限が存在しません。
.

MessageId=1314
Severity=Success
Facility=System
SymbolicName=ERROR_PRIVILEGE_NOT_HELD
Language=English
A required privilege is not held by the client.
.
Language=Japanese
必要な権限がクライアントにありません。
.

MessageId=1315
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_ACCOUNT_NAME
Language=English
The name provided is not a properly formed account name.
.
Language=Japanese
指定された名前はアカウント名ではありません。
.

MessageId=1316
Severity=Success
Facility=System
SymbolicName=ERROR_USER_EXISTS
Language=English
The specified user already exists.
.
Language=Japanese
指定されたアカウントはすでに存在します。
.

MessageId=1317
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SUCH_USER
Language=English
The specified user does not exist.
.
Language=Japanese
指定されたアカウントは存在しません。
.

MessageId=1318
Severity=Success
Facility=System
SymbolicName=ERROR_GROUP_EXISTS
Language=English
The specified group already exists.
.
Language=Japanese
指定されたグループは既に存在します。
.

MessageId=1319
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SUCH_GROUP
Language=English
The specified group does not exist.
.
Language=Japanese
指定されたグループは存在しません。
.

MessageId=1320
Severity=Success
Facility=System
SymbolicName=ERROR_MEMBER_IN_GROUP
Language=English
Either the specified user account is already a member of the specified group, or the specified group cannot be deleted because it contains a member.
.
Language=Japanese
指定されたユーザ アカウントがすでに指定されたグループのメンバであるか、指定されたグループにメンバが含まれているため削除できません。
.

MessageId=1321
Severity=Success
Facility=System
SymbolicName=ERROR_MEMBER_NOT_IN_GROUP
Language=English
The specified user account is not a member of the specified group account.
.
Language=Japanese
指定されたユーザー アカウントは、指定されたグループ アカウントのメンバーではありません。
.

MessageId=1322
Severity=Success
Facility=System
SymbolicName=ERROR_LAST_ADMIN
Language=English
The last remaining administration account cannot be disabled or deleted.
.
Language=Japanese
管理者アカウントが無効になるか、削除されるか、またはログオンできなくなる可能性があるため、この操作は許可されません。
.

MessageId=1323
Severity=Success
Facility=System
SymbolicName=ERROR_WRONG_PASSWORD
Language=English
Unable to update the password. The value provided as the current password is incorrect.
.
Language=Japanese
パスワードを更新できません。現在のパスワードとして提供された値が正しくありません。
.

MessageId=1324
Severity=Success
Facility=System
SymbolicName=ERROR_ILL_FORMED_PASSWORD
Language=English
Unable to update the password. The value provided for the new password contains values that are not allowed in passwords.
.
Language=Japanese
パスワードを更新できません。新しいパスワードに指定された値には、パスワードで使用できない値が含まれています。
.

MessageId=1325
Severity=Success
Facility=System
SymbolicName=ERROR_PASSWORD_RESTRICTION
Language=English
Unable to update the password. The value provided for the new password does not meet the length, complexity, or history requirement of the domain.
.
Language=Japanese
パスワードを更新できません。新しいパスワードに指定された値は、ドメインの長さ、複雑さ、または履歴の要件を満たしていません。
.

MessageId=1326
Severity=Success
Facility=System
SymbolicName=ERROR_LOGON_FAILURE
Language=English
Logon failure: unknown user name or bad password.
.
Language=Japanese
ログオン失敗: ユーザー名またはパスワードが正しくありません。
.

MessageId=1327
Severity=Success
Facility=System
SymbolicName=ERROR_ACCOUNT_RESTRICTION
Language=English
Logon failure: user account restriction. Possible reasons are blank passwords not allowed, logon hour restrictions, or a policy restriction has been enforced.
.
Language=Japanese
ログオン失敗: アカウント制限により、このユーザーがサインインできません。空白のパスワードが許可されていない、サインイン時間が制限されている、またはポリシーによる制限が適用されているのかもしれません。
.

MessageId=1328
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_LOGON_HOURS
Language=English
Logon failure: account logon time restriction violation.
.
Language=Japanese
ログオン失敗: アカウントに時間制限があるため、今すぐサインインできません。
.

MessageId=1329
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_WORKSTATION
Language=English
Logon failure: user not allowed to log on to this computer.
.
Language=Japanese
ログオン失敗: このユーザはこのコンピュータにサインインできません。
.

MessageId=1330
Severity=Success
Facility=System
SymbolicName=ERROR_PASSWORD_EXPIRED
Language=English
Logon failure: the specified account password has expired.
.
Language=Japanese
ログオン: このアカウントのパスワードの有効期限が切れています。
.

MessageId=1331
Severity=Success
Facility=System
SymbolicName=ERROR_ACCOUNT_DISABLED
Language=English
Logon failure: account currently disabled.
.
Language=Japanese
ログオン失敗: このアカウントは現在無効になっているため、このユーザはサインインできません。
.

MessageId=1332
Severity=Success
Facility=System
SymbolicName=ERROR_NONE_MAPPED
Language=English
No mapping between account names and security IDs was done.
.
Language=Japanese
アカウント名とセキュリティIDの間のマッピングが行われませんでした。
.

MessageId=1333
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_LUIDS_REQUESTED
Language=English
Too many local user identifiers (LUIDs) were requested at one time.
.
Language=Japanese
一度に要求されたローカル ユーザー識別子 (LUID)  の数が多すぎます。
.

MessageId=1334
Severity=Success
Facility=System
SymbolicName=ERROR_LUIDS_EXHAUSTED
Language=English
No more local user identifiers (LUIDs) are available.
.
Language=Japanese
利用可能なローカル ユーザー識別子 (LUID)  がなくなりました。
.

MessageId=1335
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SUB_AUTHORITY
Language=English
The subauthority part of a security ID is invalid for this particular use.
.
Language=Japanese
セキュリティ ID のサブ権限部分は、この特定の用途では無効です。
.

MessageId=1336
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_ACL
Language=English
The access control list (ACL) structure is invalid.
.
Language=Japanese
アクセス制御リスト (ACL)  構造が無効です。
.

MessageId=1337
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SID
Language=English
The security ID structure is invalid.
.
Language=Japanese
セキュリティ ID 構造体が無効です。
.

MessageId=1338
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SECURITY_DESCR
Language=English
The security descriptor structure is invalid.
.
Language=Japanese
セキュリティ記述子構造が無効です。
.

MessageId=1340
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_INHERITANCE_ACL
Language=English
The inherited access control list (ACL) or access control entry (ACE) could not be built.
.
Language=Japanese
継承されたアクセス制御リスト (ACL)  またはアクセス制御エントリ (ACE)  を構築できませんでした。
.

MessageId=1341
Severity=Success
Facility=System
SymbolicName=ERROR_SERVER_DISABLED
Language=English
The server is currently disabled.
.
Language=Japanese
サーバーは現在無効です。
.

MessageId=1342
Severity=Success
Facility=System
SymbolicName=ERROR_SERVER_NOT_DISABLED
Language=English
The server is currently enabled.
.
Language=Japanese
サーバーは現在有効です。
.

MessageId=1343
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_ID_AUTHORITY
Language=English
The value provided was an invalid value for an identifier authority.
.
Language=Japanese
指定された値は、識別子の権限に対して無効な値です。
.

MessageId=1344
Severity=Success
Facility=System
SymbolicName=ERROR_ALLOTTED_SPACE_EXCEEDED
Language=English
No more memory is available for security information updates.
.
Language=Japanese
セキュリティ情報の更新に使用できるメモリがありません。
.

MessageId=1345
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_GROUP_ATTRIBUTES
Language=English
The specified attributes are invalid, or incompatible with the attributes for the group as a whole.
.
Language=Japanese
指定された属性が無効、またはグループ全体の属性と互換性がありません。
.

MessageId=1346
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_IMPERSONATION_LEVEL
Language=English
Either a required impersonation level was not provided, or the provided impersonation level is invalid.
.
Language=Japanese
必要な偽装レベルが指定されていないか、指定された偽装レベルが無効です。
.

MessageId=1347
Severity=Success
Facility=System
SymbolicName=ERROR_CANT_OPEN_ANONYMOUS
Language=English
Cannot open an anonymous level security token.
.
Language=Japanese
匿名レベルのセキュリティ トークンを開けません。
.

MessageId=1348
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_VALIDATION_CLASS
Language=English
The validation information class requested was invalid.
.
Language=Japanese
要求された検証情報クラスが無効です。
.

MessageId=1349
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_TOKEN_TYPE
Language=English
The type of the token is inappropriate for its attempted use.
.
Language=Japanese
トークンの型が使用目的に対して不適切です。
.

MessageId=1350
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SECURITY_ON_OBJECT
Language=English
Unable to perform a security operation on an object that has no associated security.
.
Language=Japanese
セキュリティに関連付けられていないオブジェクトに対してセキュリティ操作を実行できません。
.

MessageId=1351
Severity=Success
Facility=System
SymbolicName=ERROR_CANT_ACCESS_DOMAIN_INFO
Language=English
Configuration information could not be read from the domain controller, either because the machine is unavailable, or access has been denied.
.
Language=Japanese
マシンが使用できないか、アクセスが拒否されたため、ドメイン コントローラから構成情報を読み取れませんでした。
.

MessageId=1352
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SERVER_STATE
Language=English
The security account manager (SAM) or local security authority (LSA) server was in the wrong state to perform the security operation.
.
Language=Japanese
セキュリティ アカウント マネージャ (SAM)  またはローカル セキュリティ権限 (LSA)  サーバーが、セキュリティ操作を実行するのに不適切な状態でした。
.

MessageId=1353
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_DOMAIN_STATE
Language=English
The domain was in the wrong state to perform the security operation.
.
Language=Japanese
ドメインがセキュリティ操作を実行するために間違った状態にありました。
.

MessageId=1354
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_DOMAIN_ROLE
Language=English
This operation is only allowed for the Primary Domain Controller of the domain.
.
Language=Japanese
この操作はドメインのプライマリドメイン コントローラーにのみ許可されています。
.

MessageId=1355
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SUCH_DOMAIN
Language=English
The specified domain either does not exist or could not be contacted.
.
Language=Japanese
指定されたドメインが存在しないか、連絡できませんでした。
.

MessageId=1356
Severity=Success
Facility=System
SymbolicName=ERROR_DOMAIN_EXISTS
Language=English
The specified domain already exists.
.
Language=Japanese
指定されたドメインはすでに存在します。
.

MessageId=1357
Severity=Success
Facility=System
SymbolicName=ERROR_DOMAIN_LIMIT_EXCEEDED
Language=English
An attempt was made to exceed the limit on the number of domains per server.
.
Language=Japanese
サーバーあたりのドメイン数の制限を超えようとしました。
.

MessageId=1358
Severity=Success
Facility=System
SymbolicName=ERROR_INTERNAL_DB_CORRUPTION
Language=English
Unable to complete the requested operation because of either a catastrophic media failure or a data structure corruption on the disk.
.
Language=Japanese
メディアの致命的な障害またはディスク上のデータ構造の破損のため、要求された操作を完了できません。
.

MessageId=1359
Severity=Success
Facility=System
SymbolicName=ERROR_INTERNAL_ERROR
Language=English
An internal error occurred.
.
Language=Japanese
内部エラーが発生しました。
.

MessageId=1360
Severity=Success
Facility=System
SymbolicName=ERROR_GENERIC_NOT_MAPPED
Language=English
Generic access types were contained in an access mask which should already be mapped to nongeneric types.
.
Language=Japanese
汎用アクセス タイプが、汎用タイプ以外のタイプにマップされているはずのアクセス マスクに含まれていました。
.

MessageId=1361
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_DESCRIPTOR_FORMAT
Language=English
A security descriptor is not in the right format (absolute or self-relative).
.
Language=Japanese
セキュリティ記述子の形式が正しくありません (絶対形式または自己相対形式) 。
.

MessageId=1362
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_LOGON_PROCESS
Language=English
The requested action is restricted for use by logon processes only. The calling process has not registered as a logon process.
.
Language=Japanese
要求されたアクションはログオンプロセスによってのみ使用できるように制限されています。呼び出し元のプロセスはログオンプロセスとして登録されていません。
.

MessageId=1363
Severity=Success
Facility=System
SymbolicName=ERROR_LOGON_SESSION_EXISTS
Language=English
Cannot start a new logon session with an ID that is already in use.
.
Language=Japanese
すでに使用されているIDで新しいログオンセッションを開始できません。
.

MessageId=1364
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SUCH_PACKAGE
Language=English
A specified authentication package is unknown.
.
Language=Japanese
指定された認証パッケージが不明です。
.

MessageId=1365
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_LOGON_SESSION_STATE
Language=English
The logon session is not in a state that is consistent with the requested operation.
.
Language=Japanese
ログオン セッションの状態が要求された操作と一致しません。
.

MessageId=1366
Severity=Success
Facility=System
SymbolicName=ERROR_LOGON_SESSION_COLLISION
Language=English
The logon session ID is already in use.
.
Language=Japanese
ログオンセッションIDがすでに使用されています。
.

MessageId=1367
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_LOGON_TYPE
Language=English
A logon request contained an invalid logon type value.
.
Language=Japanese
ログオン要求に無効なログオン タイプ値が含まれていました。
.

MessageId=1368
Severity=Success
Facility=System
SymbolicName=ERROR_CANNOT_IMPERSONATE
Language=English
Unable to impersonate using a named pipe until data has been read from that pipe.
.
Language=Japanese
名前付きパイプからデータが読み込まれるまで、名前付きパイプを使用したなりすましができません。
.

MessageId=1369
Severity=Success
Facility=System
SymbolicName=ERROR_RXACT_INVALID_STATE
Language=English
The transaction state of a registry subtree is incompatible with the requested operation.
.
Language=Japanese
レジストリ サブツリーのトランザクション状態が要求された操作と互換性がありません。
.

MessageId=1370
Severity=Success
Facility=System
SymbolicName=ERROR_RXACT_COMMIT_FAILURE
Language=English
An internal security database corruption has been encountered.
.
Language=Japanese
内部セキュリティ データベースの破損が発生しました。
.

MessageId=1371
Severity=Success
Facility=System
SymbolicName=ERROR_SPECIAL_ACCOUNT
Language=English
Cannot perform this operation on built-in accounts.
.
Language=Japanese
組み込みアカウントでこの操作を実行できません。
.

MessageId=1372
Severity=Success
Facility=System
SymbolicName=ERROR_SPECIAL_GROUP
Language=English
Cannot perform this operation on this built-in special group.
.
Language=Japanese
この組み込みの特別グループに対してこの操作は実行できません。
.

MessageId=1373
Severity=Success
Facility=System
SymbolicName=ERROR_SPECIAL_USER
Language=English
Cannot perform this operation on this built-in special user.
.
Language=Japanese
この組み込みの特別ユーザーに対してこの操作は実行できません。
.

MessageId=1374
Severity=Success
Facility=System
SymbolicName=ERROR_MEMBERS_PRIMARY_GROUP
Language=English
The user cannot be removed from a group because the group is currently the user's primary group.
.
Language=Japanese
グループが現在ユーザのプライマリ グループであるため、ユーザをグループから削除できません。
.

MessageId=1375
Severity=Success
Facility=System
SymbolicName=ERROR_TOKEN_ALREADY_IN_USE
Language=English
The token is already in use as a primary token.
.
Language=Japanese
トークンはすでにプライマリ トークンとして使用されています。
.

MessageId=1376
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SUCH_ALIAS
Language=English
The specified local group does not exist.
.
Language=Japanese
指定されたローカル グループが存在しません。
.

MessageId=1377
Severity=Success
Facility=System
SymbolicName=ERROR_MEMBER_NOT_IN_ALIAS
Language=English
The specified account name is not a member of the local group.
.
Language=Japanese
指定されたアカウント名はグループのメンバーではありません。
.

MessageId=1378
Severity=Success
Facility=System
SymbolicName=ERROR_MEMBER_IN_ALIAS
Language=English
The specified account name is already a member of the local group.
.
Language=Japanese
指定されたアカウント名はすでにグループのメンバーです。
.

MessageId=1379
Severity=Success
Facility=System
SymbolicName=ERROR_ALIAS_EXISTS
Language=English
The specified local group already exists.
.
Language=Japanese
指定されたローカル グループは既に存在します。
.

MessageId=1380
Severity=Success
Facility=System
SymbolicName=ERROR_LOGON_NOT_GRANTED
Language=English
Logon failure: the user has not been granted the requested logon type at this computer.
.
Language=Japanese
ログオン失敗: ユーザーはこのコンピューターで要求されたログオン タイプを許可されていません。
.

MessageId=1381
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_SECRETS
Language=English
The maximum number of secrets that may be stored in a single system has been exceeded.
.
Language=Japanese
1つのシステムに保存できる秘密の最大数を超えました。
.

MessageId=1382
Severity=Success
Facility=System
SymbolicName=ERROR_SECRET_TOO_LONG
Language=English
The length of a secret exceeds the maximum length allowed.
.
Language=Japanese
秘密の長さが許容される最大長を超えました。
.

MessageId=1383
Severity=Success
Facility=System
SymbolicName=ERROR_INTERNAL_DB_ERROR
Language=English
The local security authority database contains an internal inconsistency.
.
Language=Japanese
ローカルのセキュリティ権限データベースに内部不整合があります。
.

MessageId=1384
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_CONTEXT_IDS
Language=English
During a logon attempt, the user's security context accumulated too many security IDs.
.
Language=Japanese
ログオン試行中、ユーザーのセキュリティ コンテキストにセキュリティIDが蓄積されすぎました。
.

MessageId=1385
Severity=Success
Facility=System
SymbolicName=ERROR_LOGON_TYPE_NOT_GRANTED
Language=English
Logon failure: the user has not been granted the requested logon type at this computer.
.
Language=Japanese
ログオン失敗: ユーザーはこのコンピュータで要求されたログオン タイプを許可されていません。
.

MessageId=1386
Severity=Success
Facility=System
SymbolicName=ERROR_NT_CROSS_ENCRYPTION_REQUIRED
Language=English
A cross-encrypted password is necessary to change a user password.
.
Language=Japanese
ユーザーパスワードを変更するには、暗号化されたパスワードが必要です。
.

MessageId=1387
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SUCH_MEMBER
Language=English
A new member could not be added to or removed from the local group because the member does not exist.
.
Language=Japanese
メンバが存在しないため、メンバをローカル グループに追加またはローカル グループから削除できませんでした。
.

MessageId=1388
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_MEMBER
Language=English
A new member could not be added to a local group because the member has the wrong account type.
.
Language=Japanese
メンバのアカウント タイプが正しくないため、新しいメンバをローカル グループに追加できませんでした。
.

MessageId=1389
Severity=Success
Facility=System
SymbolicName=ERROR_TOO_MANY_SIDS
Language=English
Too many security IDs have been specified.
.
Language=Japanese
セキュリティIDが多すぎます。
.

MessageId=1390
Severity=Success
Facility=System
SymbolicName=ERROR_LM_CROSS_ENCRYPTION_REQUIRED
Language=English
A cross-encrypted password is necessary to change this user password.
.
Language=Japanese
このユーザーパスワードを変更するには、暗号化されたパスワードが必要です。
.

MessageId=1391
Severity=Success
Facility=System
SymbolicName=ERROR_NO_INHERITANCE
Language=English
Indicates an ACL contains no inheritable components.
.
Language=Japanese
ACLに継承可能なコンポーネントがないことを示します。
.

MessageId=1392
Severity=Success
Facility=System
SymbolicName=ERROR_FILE_CORRUPT
Language=English
The file or directory is corrupted and unreadable.
.
Language=Japanese
ファイルまたはディレクトリが壊れて読み取り不可能です。
.

MessageId=1393
Severity=Success
Facility=System
SymbolicName=ERROR_DISK_CORRUPT
Language=English
The disk structure is corrupted and unreadable.
.
Language=Japanese
ディスク構造が壊れて読み取りできません。
.

MessageId=1394
Severity=Success
Facility=System
SymbolicName=ERROR_NO_USER_SESSION_KEY
Language=English
There is no user session key for the specified logon session.
.
Language=Japanese
指定されたログオン セッションのユーザー セッション キーがありません。
.

MessageId=1395
Severity=Success
Facility=System
SymbolicName=ERROR_LICENSE_QUOTA_EXCEEDED
Language=English
The service being accessed is licensed for a particular number of connections. No more connections can be made to the service at this time because there are already as many connections as the service can accept.
.
Language=Japanese
アクセスされているサービスは、特定の接続数に対してライセンスされています。サービスが受け入れることができる接続数がすでにあるため、現時点ではこれ以上の接続を行うことはできません。
.

MessageId=1396
Severity=Success
Facility=System
SymbolicName=ERROR_WRONG_TARGET_NAME
Language=English
Logon Failure: The target account name is incorrect.
.
Language=Japanese
ログオン失敗: アクセス先のアカウント名が正しくありません。
.

MessageId=1397
Severity=Success
Facility=System
SymbolicName=ERROR_MUTUAL_AUTH_FAILED
Language=English
Mutual Authentication failed. The server's password is out of date at the domain controller.
.
Language=Japanese
相互認証に失敗しました。ドメイン コントローラーでサーバーのパスワードが無効です。
.

MessageId=1398
Severity=Success
Facility=System
SymbolicName=ERROR_TIME_SKEW
Language=English
There is a time and/or date difference between the client and server.
.
Language=Japanese
クライアントとサーバーの間で時差があります。
.

MessageId=1399
Severity=Success
Facility=System
SymbolicName=ERROR_CURRENT_DOMAIN_NOT_ALLOWED
Language=English
This operation cannot be performed on the current domain.
.
Language=Japanese
この操作は現在のドメインでは実行できません。
.

MessageId=1400
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_WINDOW_HANDLE
Language=English
Invalid window handle.
.
Language=Japanese
無効なウィンドウハンドルです。
.

MessageId=1401
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_MENU_HANDLE
Language=English
Invalid menu handle.
.
Language=Japanese
無効なメニューハンドルです。
.

MessageId=1402
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_CURSOR_HANDLE
Language=English
Invalid cursor handle.
.
Language=Japanese
無効なカーソルハンドルです。
.

MessageId=1403
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_ACCEL_HANDLE
Language=English
Invalid accelerator table handle.
.
Language=Japanese
無効なアクセラレータテーブルハンドルです。
.

MessageId=1404
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_HOOK_HANDLE
Language=English
Invalid hook handle.
.
Language=Japanese
無効なフックハンドルです。
.

MessageId=1405
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_DWP_HANDLE
Language=English
Invalid handle to a multiple-window position structure.
.
Language=Japanese
複数ウィンドウ位置構造体のハンドルが無効です。
.

MessageId=1406
Severity=Success
Facility=System
SymbolicName=ERROR_TLW_WITH_WSCHILD
Language=English
Cannot create a top-level child window.
.
Language=Japanese
トップレベルの子ウィンドウを作成できません。
.

MessageId=1407
Severity=Success
Facility=System
SymbolicName=ERROR_CANNOT_FIND_WND_CLASS
Language=English
Cannot find window class.
.
Language=Japanese
ウィンドウ クラスが見つかりません。
.

MessageId=1408
Severity=Success
Facility=System
SymbolicName=ERROR_WINDOW_OF_OTHER_THREAD
Language=English
Invalid window; it belongs to other thread.
.
Language=Japanese
無効なウィンドウです。
.

MessageId=1409
Severity=Success
Facility=System
SymbolicName=ERROR_HOTKEY_ALREADY_REGISTERED
Language=English
Hot key is already registered.
.
Language=Japanese
ホットキーは既に登録されています。
.

MessageId=1410
Severity=Success
Facility=System
SymbolicName=ERROR_CLASS_ALREADY_EXISTS
Language=English
Class already exists.
.
Language=Japanese
クラスはすでに存在します。
.

MessageId=1411
Severity=Success
Facility=System
SymbolicName=ERROR_CLASS_DOES_NOT_EXIST
Language=English
Class does not exist.
.
Language=Japanese
クラスが存在しません。
.

MessageId=1412
Severity=Success
Facility=System
SymbolicName=ERROR_CLASS_HAS_WINDOWS
Language=English
Class still has open windows.
.
Language=Japanese
クラスはまだウィンドウを開いています。
.

MessageId=1413
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_INDEX
Language=English
Invalid index.
.
Language=Japanese
無効なインデックスです。
.

MessageId=1414
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_ICON_HANDLE
Language=English
Invalid icon handle.
.
Language=Japanese
無効なアイコンハンドルです。
.

MessageId=1415
Severity=Success
Facility=System
SymbolicName=ERROR_PRIVATE_DIALOG_INDEX
Language=English
Using private DIALOG window words.
.
Language=Japanese
プライベート DIALOG ウィンドウの単語を使用しています。
.

MessageId=1416
Severity=Success
Facility=System
SymbolicName=ERROR_LISTBOX_ID_NOT_FOUND
Language=English
The list box identifier was not found.
.
Language=Japanese
リスト ボックスの識別子が見つかりませんでした。
.

MessageId=1417
Severity=Success
Facility=System
SymbolicName=ERROR_NO_WILDCARD_CHARACTERS
Language=English
No wildcards were found.
.
Language=Japanese
ワイルド カードが見つかりませんでした。
.

MessageId=1418
Severity=Success
Facility=System
SymbolicName=ERROR_CLIPBOARD_NOT_OPEN
Language=English
Thread does not have a clipboard open.
.
Language=Japanese
スレッドがクリップボードを開いていません。
.

MessageId=1419
Severity=Success
Facility=System
SymbolicName=ERROR_HOTKEY_NOT_REGISTERED
Language=English
Hot key is not registered.
.
Language=Japanese
ホットキーが登録されていません。
.

MessageId=1420
Severity=Success
Facility=System
SymbolicName=ERROR_WINDOW_NOT_DIALOG
Language=English
The window is not a valid dialog window.
.
Language=Japanese
ウィンドウは有効なダイアログウィンドウではありません。
.

MessageId=1421
Severity=Success
Facility=System
SymbolicName=ERROR_CONTROL_ID_NOT_FOUND
Language=English
Control ID not found.
.
Language=Japanese
コントロールIDが見つかりません。
.

MessageId=1422
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_COMBOBOX_MESSAGE
Language=English
Invalid message for a combo box because it does not have an edit control.
.
Language=Japanese
コンボボックスに編集コントロールがないので無効なメッセージです。
.

MessageId=1423
Severity=Success
Facility=System
SymbolicName=ERROR_WINDOW_NOT_COMBOBOX
Language=English
The window is not a combo box.
.
Language=Japanese
ウィンドウはコンボボックスではありません。
.

MessageId=1424
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_EDIT_HEIGHT
Language=English
Height must be less than 256.
.
Language=Japanese
高さが256未満である必要があります。
.

MessageId=1425
Severity=Success
Facility=System
SymbolicName=ERROR_DC_NOT_FOUND
Language=English
Invalid device context (DC) handle.
.
Language=Japanese
無効なデバイスコンテキスト (DC) ハンドル
.

MessageId=1426
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_HOOK_FILTER
Language=English
Invalid hook procedure type.
.
Language=Japanese
無効なフック プロシージャ タイプです。
.

MessageId=1427
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_FILTER_PROC
Language=English
Invalid hook procedure.
.
Language=Japanese
無効なフック プロシージャです。
.

MessageId=1428
Severity=Success
Facility=System
SymbolicName=ERROR_HOOK_NEEDS_HMOD
Language=English
Cannot set nonlocal hook without a module handle.
.
Language=Japanese
モジュールハンドルを持たない非ローカルフックを設定できません。
.

MessageId=1429
Severity=Success
Facility=System
SymbolicName=ERROR_GLOBAL_ONLY_HOOK
Language=English
This hook procedure can only be set globally.
.
Language=Japanese
このフック プロシージャはグローバルにしか設定できません。
.

MessageId=1430
Severity=Success
Facility=System
SymbolicName=ERROR_JOURNAL_HOOK_SET
Language=English
The journal hook procedure is already installed.
.
Language=Japanese
ジャーナル フック プロシージャは既にインストールされています。
.

MessageId=1431
Severity=Success
Facility=System
SymbolicName=ERROR_HOOK_NOT_INSTALLED
Language=English
The hook procedure is not installed.
.
Language=Japanese
フック プロシージャがインストールされていません。
.

MessageId=1432
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_LB_MESSAGE
Language=English
Invalid message for single-selection list box.
.
Language=Japanese
単一選択リスト ボックスのメッセージが無効です。
.

MessageId=1433
Severity=Success
Facility=System
SymbolicName=ERROR_SETCOUNT_ON_BAD_LB
Language=English
LB_SETCOUNT sent to non-lazy list box.
.
Language=Japanese
非レイジーリストボックスに LB_SETCOUNT を送信しました。
.

MessageId=1434
Severity=Success
Facility=System
SymbolicName=ERROR_LB_WITHOUT_TABSTOPS
Language=English
This list box does not support tab stops.
.
Language=Japanese
このリストボックスはタブストップをサポートしていません。
.

MessageId=1435
Severity=Success
Facility=System
SymbolicName=ERROR_DESTROY_OBJECT_OF_OTHER_THREAD
Language=English
Cannot destroy object created by another thread.
.
Language=Japanese
他のスレッドによって作成されたオブジェクトを破棄できません。
.

MessageId=1436
Severity=Success
Facility=System
SymbolicName=ERROR_CHILD_WINDOW_MENU
Language=English
Child windows cannot have menus.
.
Language=Japanese
子ウィンドウはメニューを持つことができません。
.

MessageId=1437
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SYSTEM_MENU
Language=English
The window does not have a system menu.
.
Language=Japanese
ウィンドウにシステム メニューがありません。
.

MessageId=1438
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_MSGBOX_STYLE
Language=English
Invalid message box style.
.
Language=Japanese
メッセージ ボックスのスタイルが無効です。
.

MessageId=1439
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SPI_VALUE
Language=English
Invalid system-wide (SPI_*) parameter.
.
Language=Japanese
無効なシステム全体 (SPI_*) パラメータ
.

MessageId=1440
Severity=Success
Facility=System
SymbolicName=ERROR_SCREEN_ALREADY_LOCKED
Language=English
Screen already locked.
.
Language=Japanese
画面が既にロックされています。
.

MessageId=1441
Severity=Success
Facility=System
SymbolicName=ERROR_HWNDS_HAVE_DIFF_PARENT
Language=English
All handles to windows in a multiple-window position structure must have the same parent.
.
Language=Japanese
複数ウィンドウの位置構造内のウィンドウへのすべてのハンドルは、同じ親を持つ必要があります。
.

MessageId=1442
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_CHILD_WINDOW
Language=English
The window is not a child window.
.
Language=Japanese
ウィンドウは子ウィンドウではありません。
.

MessageId=1443
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_GW_COMMAND
Language=English
Invalid GW_* command.
.
Language=Japanese
無効なGW_*コマンドです。
.

MessageId=1444
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_THREAD_ID
Language=English
Invalid thread identifier.
.
Language=Japanese
無効なスレッド識別子です。
.

MessageId=1445
Severity=Success
Facility=System
SymbolicName=ERROR_NON_MDICHILD_WINDOW
Language=English
Cannot process a message from a window that is not a multiple document interface (MDI) window.
.
Language=Japanese
MDI (複数文書インターフェイス)  ウィンドウではないウィンドウからのメッセージを処理できません。
.

MessageId=1446
Severity=Success
Facility=System
SymbolicName=ERROR_POPUP_ALREADY_ACTIVE
Language=English
Popup menu already active.
.
Language=Japanese
ポップアップメニューはすでにアクティブです。
.

MessageId=1447
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SCROLLBARS
Language=English
The window does not have scroll bars.
.
Language=Japanese
ウィンドウにスクロール バーがありません。
.

MessageId=1448
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SCROLLBAR_RANGE
Language=English
Scroll bar range cannot be greater than MAXLONG.
.
Language=Japanese
スクロールバーの範囲をMAXLONGより大きくできません。
.

MessageId=1449
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SHOWWIN_COMMAND
Language=English
Cannot show or remove the window in the way specified.
.
Language=Japanese
指定された方法でウィンドウを表示または削除できません。
.

MessageId=1450
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SYSTEM_RESOURCES
Language=English
Insufficient system resources exist to complete the requested service.
.
Language=Japanese
要求されたサービスを完了するためのシステム リソースが不足しています。
.

MessageId=1451
Severity=Success
Facility=System
SymbolicName=ERROR_NONPAGED_SYSTEM_RESOURCES
Language=English
Insufficient system resources exist to complete the requested service.
.
Language=Japanese
要求されたサービスを完了するにはシステム リソースが不足しています。
.

MessageId=1452
Severity=Success
Facility=System
SymbolicName=ERROR_PAGED_SYSTEM_RESOURCES
Language=English
Insufficient system resources exist to complete the requested service.
.
Language=Japanese
要求されたサービスを完了するにはシステム リソースが不足しています。
.

MessageId=1453
Severity=Success
Facility=System
SymbolicName=ERROR_WORKING_SET_QUOTA
Language=English
Insufficient quota to complete the requested service.
.
Language=Japanese
要求されたサービスを完了するためのクォータが不足しています。
.

MessageId=1454
Severity=Success
Facility=System
SymbolicName=ERROR_PAGEFILE_QUOTA
Language=English
Insufficient quota to complete the requested service.
.
Language=Japanese
要求されたサービスを完了するためのクォータが不足しています。
.

MessageId=1455
Severity=Success
Facility=System
SymbolicName=ERROR_COMMITMENT_LIMIT
Language=English
The paging file is too small for this operation to complete.
.
Language=Japanese
ページング ファイルが小さすぎるため、この操作を完了できません。
.

MessageId=1456
Severity=Success
Facility=System
SymbolicName=ERROR_MENU_ITEM_NOT_FOUND
Language=English
A menu item was not found.
.
Language=Japanese
メニュー項目が見つかりませんでした。
.

MessageId=1457
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_KEYBOARD_HANDLE
Language=English
Invalid keyboard layout handle.
.
Language=Japanese
無効なキーボードレイアウトハンドルです。
.

MessageId=1458
Severity=Success
Facility=System
SymbolicName=ERROR_HOOK_TYPE_NOT_ALLOWED
Language=English
Hook type not allowed.
.
Language=Japanese
フックタイプが許可されていません。
.

MessageId=1459
Severity=Success
Facility=System
SymbolicName=ERROR_REQUIRES_INTERACTIVE_WINDOWSTATION
Language=English
This operation requires an interactive window station.
.
Language=Japanese
この操作には対話型ウィンドウ ステーションが必要です。
.

MessageId=1460
Severity=Success
Facility=System
SymbolicName=ERROR_TIMEOUT
Language=English
This operation returned because the timeout period expired.
.
Language=Japanese
タイムアウト時間が過ぎたため、この操作は返されました。
.

MessageId=1461
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_MONITOR_HANDLE
Language=English
Invalid monitor handle.
.
Language=Japanese
無効なモニタハンドルです。
.

MessageId=1500
Severity=Success
Facility=System
SymbolicName=ERROR_EVENTLOG_FILE_CORRUPT
Language=English
The event log file is corrupted.
.
Language=Japanese
イベント ログ ファイルが壊れています。
.

MessageId=1501
Severity=Success
Facility=System
SymbolicName=ERROR_EVENTLOG_CANT_START
Language=English
No event log file could be opened, so the event logging service did not start.
.
Language=Japanese
イベント ログ ファイルを開くことができませんでした。
.

MessageId=1502
Severity=Success
Facility=System
SymbolicName=ERROR_LOG_FILE_FULL
Language=English
The event log file is full.
.
Language=Japanese
イベント ログ ファイルがいっぱいです。
.

MessageId=1503
Severity=Success
Facility=System
SymbolicName=ERROR_EVENTLOG_FILE_CHANGED
Language=English
The event log file has changed between read operations.
.
Language=Japanese
イベント ログ ファイルが読み取り操作の間に変更されました。
.

MessageId=1601
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_SERVICE_FAILURE
Language=English
The ReactOS Installer service could not be accessed. This can occur if you are running ReliefWin in safe mode, or if the ReactOS Installer is not correctly installed. Contact your support personnel for assistance.
.
Language=Japanese
ReactOS Installer サービスにアクセスできませんでした。ReactOS Installerが正しくインストールされていない場合に発生する可能性があります。サポート担当者にお問い合わせください。
.

MessageId=1602
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_USEREXIT
Language=English
User cancelled installation.
.
Language=Japanese
ユーザーがインストールをキャンセルしました。
.

MessageId=1603
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_FAILURE
Language=English
Fatal error during installation.
.
Language=Japanese
インストール中に致命的なエラーが発生しました。
.

MessageId=1604
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_SUSPEND
Language=English
Installation suspended, incomplete.
.
Language=Japanese
インストールが中断されました。
.

MessageId=1605
Severity=Success
Facility=System
SymbolicName=ERROR_UNKNOWN_PRODUCT
Language=English
This action is only valid for products that are currently installed.
.
Language=Japanese
この操作は、現在インストールされている製品に対してのみ有効です。
.

MessageId=1606
Severity=Success
Facility=System
SymbolicName=ERROR_UNKNOWN_FEATURE
Language=English
Feature ID not registered.
.
Language=Japanese
フィーチャーIDが登録されていません。
.

MessageId=1607
Severity=Success
Facility=System
SymbolicName=ERROR_UNKNOWN_COMPONENT
Language=English
Component ID not registered.
.
Language=Japanese
コンポーネントIDが登録されていません。
.

MessageId=1608
Severity=Success
Facility=System
SymbolicName=ERROR_UNKNOWN_PROPERTY
Language=English
Unknown property.
.
Language=Japanese
不明なプロパティ
.

MessageId=1609
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_HANDLE_STATE
Language=English
Handle is in an invalid state.
.
Language=Japanese
ハンドルの状態が無効です。
.

MessageId=1610
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_CONFIGURATION
Language=English
The configuration data for this product is corrupt. Contact your support personnel.
.
Language=Japanese
この製品の設定データが壊れています。サポート担当者に連絡してください。
.

MessageId=1611
Severity=Success
Facility=System
SymbolicName=ERROR_INDEX_ABSENT
Language=English
Component qualifier not present.
.
Language=Japanese
コンポーネント修飾子が存在しません。
.

MessageId=1612
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_SOURCE_ABSENT
Language=English
The installation source for this product is not available. Verify that the source exists and that you can access it.
.
Language=Japanese
この製品のインストール ソースがありません。ソースが存在し、アクセスできることを確認してください。
.

MessageId=1613
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_PACKAGE_VERSION
Language=English
This installation package cannot be installed by the ReactOS Installer service. You must install a ReliefWin service pack that contains a newer version of the ReactOS Installer service.
.
Language=Japanese
このインストール パッケージは ReactOS Installer サービスではインストールできません。新しいバージョンのReactOS Installerサービスが含まれているReliefWinサービスパックをインストールする必要があります。
.

MessageId=1614
Severity=Success
Facility=System
SymbolicName=ERROR_PRODUCT_UNINSTALLED
Language=English
Product is uninstalled.
.
Language=Japanese
製品がアンインストールされました。
.

MessageId=1615
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_QUERY_SYNTAX
Language=English
SQL query syntax invalid or unsupported.
.
Language=Japanese
SQL クエリの構文が無効、またはサポートされていません。
.

MessageId=1616
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_FIELD
Language=English
Record field does not exist.
.
Language=Japanese
レコード フィールドが存在しません。
.

MessageId=1617
Severity=Success
Facility=System
SymbolicName=ERROR_DEVICE_REMOVED
Language=English
The device has been removed.
.
Language=Japanese
デバイスが削除されました。
.

MessageId=1618
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_ALREADY_RUNNING
Language=English
Another installation is already in progress. Complete that installation before proceeding with this install.
.
Language=Japanese
別のインストールがすでに進行中です。このインストールを続行する前に、そのインストールを完了してください。
.

MessageId=1619
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_PACKAGE_OPEN_FAILED
Language=English
This installation package could not be opened. Verify that the package exists and that you can access it, or contact the application vendor to verify that this is a valid Windows Installer package.
.
Language=Japanese
このインストール パッケージを開けませんでした。パッケージが存在し、アクセスできることを確認するか、アプリケーション ベンダーに問い合わせて、このパッケージが有効な ReactOS Installer パッケージであることを確認してください。
.

MessageId=1620
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_PACKAGE_INVALID
Language=English
This installation package could not be opened. Contact the application vendor to verify that this is a valid Windows Installer package.
.
Language=Japanese
このインストール パッケージを開けませんでした。アプリケーション ベンダーに連絡して、これが有効な ReactOS Installer パッケージであることを確認してください。
.

MessageId=1621
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_UI_FAILURE
Language=English
There was an error starting the ReactOS Installer service user interface. Contact your support personnel.
.
Language=Japanese
ReactOS Installerサービスのユーザーインターフェイスの起動にエラーが発生しました。サポート担当者にお問い合わせください。
.

MessageId=1622
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_LOG_FAILURE
Language=English
Error opening installation log file. Verify that the specified log file location exists and that you can write to it.
.
Language=Japanese
インストール ログ ファイルを開くときにエラーが発生しました。指定されたログファイルの場所が存在し、書き込めることを確認してください。
.

MessageId=1623
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_LANGUAGE_UNSUPPORTED
Language=English
The language of this installation package is not supported by your system.
.
Language=Japanese
このインストール パッケージの言語は、システムでサポートされていません。
.

MessageId=1624
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_TRANSFORM_FAILURE
Language=English
Error applying transforms. Verify that the specified transform paths are valid.
.
Language=Japanese
トランスフォームの適用でエラーが発生しました。指定された変換パスが有効であることを確認してください。
.

MessageId=1625
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_PACKAGE_REJECTED
Language=English
This installation is forbidden by system policy. Contact your system administrator.
.
Language=Japanese
このインストールはシステム ポリシーで禁止されています。システム管理者にお問い合わせください。
.

MessageId=1626
Severity=Success
Facility=System
SymbolicName=ERROR_FUNCTION_NOT_CALLED
Language=English
Function could not be executed.
.
Language=Japanese
関数を実行できませんでした。
.

MessageId=1627
Severity=Success
Facility=System
SymbolicName=ERROR_FUNCTION_FAILED
Language=English
Function failed during execution.
.
Language=Japanese
関数の実行に失敗しました。
.

MessageId=1628
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_TABLE
Language=English
Invalid or unknown table specified.
.
Language=Japanese
指定されたテーブルが無効または不明です。
.

MessageId=1629
Severity=Success
Facility=System
SymbolicName=ERROR_DATATYPE_MISMATCH
Language=English
Data supplied is of wrong type.
.
Language=Japanese
指定されたデータの型が間違っています。
.

MessageId=1630
Severity=Success
Facility=System
SymbolicName=ERROR_UNSUPPORTED_TYPE
Language=English
Data of this type is not supported.
.
Language=Japanese
この型のデータはサポートされていません。
.

MessageId=1631
Severity=Success
Facility=System
SymbolicName=ERROR_CREATE_FAILED
Language=English
The ReactOS Installer service failed to start. Contact your support personnel.
.
Language=Japanese
ReactOS Installer サービスの起動に失敗しました。サポート担当者にお問い合わせください。
.

MessageId=1632
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_TEMP_UNWRITABLE
Language=English
The Temp folder is on a drive that is full or inaccessible. Free up space on the drive or verify that you have write permission on the Temp folder.
.
Language=Japanese
Temp フォルダがドライブにあるか、アクセスできません。ドライブの空き容量を確保するか、Tempフォルダの書き込み権限があることを確認してください。
.

MessageId=1633
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_PLATFORM_UNSUPPORTED
Language=English
This installation package is not supported by this processor type. Contact your product vendor.
.
Language=Japanese
このインストールパッケージは、このプロセッサタイプではサポートされていません。製品ベンダーにお問い合わせください。
.

MessageId=1634
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_NOTUSED
Language=English
Component not used on this computer.
.
Language=Japanese
このコンピュータではコンポーネントが使用されていません。
.

MessageId=1635
Severity=Success
Facility=System
SymbolicName=ERROR_PATCH_PACKAGE_OPEN_FAILED
Language=English
This patch package could not be opened. Verify that the patch package exists and that you can access it, or contact the application vendor to verify that this is a valid Windows Installer patch package.
.
Language=Japanese
この更新パッケージを開けませんでした。更新パッケージが存在し、アクセスできることを確認するか、アプリケーション ベンダーに連絡して、この更新パッケージが有効な ReactOS Installer 更新パッケージであることを確認してください。
.

MessageId=1636
Severity=Success
Facility=System
SymbolicName=ERROR_PATCH_PACKAGE_INVALID
Language=English
This patch package could not be opened. Contact the application vendor to verify that this is a valid Windows Installer patch package.
.
Language=Japanese
この更新パッケージを開けませんでした。アプリケーション ベンダーに連絡して、これが有効な ReactOS Installer 更新パッケージであることを確認してください。
.

MessageId=1637
Severity=Success
Facility=System
SymbolicName=ERROR_PATCH_PACKAGE_UNSUPPORTED
Language=English
This patch package cannot be processed by the ReactOS Installer service. You must install a ReliefWin service pack that contains a newer version of the ReactOS Installer service.
.
Language=Japanese
この更新パッケージは ReactOS Installer サービスで処理できません。新しいバージョンの ReactOS Installer サービスが含まれている ReliefWin サービス パックをインストールする必要があります。
.

MessageId=1638
Severity=Success
Facility=System
SymbolicName=ERROR_PRODUCT_VERSION
Language=English
Another version of this product is already installed. Installation of this version cannot continue. To configure or remove the existing version of this product, use Add/Remove Programs on the Control Panel.
.
Language=Japanese
この製品の別のバージョンがすでにインストールされています。このバージョンのインストールを続行できません。この製品の既存のバージョンを設定または削除するには、コントロールパネルの[プログラムの追加と削除]を使用します。
.

MessageId=1639
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_COMMAND_LINE
Language=English
Invalid command line argument. Consult the Windows Installer SDK for detailed command line help.
.
Language=Japanese
コマンド ライン引数が無効です。コマンド ライン ヘルプの詳細については、ReactOS Installer SDK を参照してください。
.

MessageId=1640
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_REMOTE_DISALLOWED
Language=English
Only administrators have permission to add, remove, or configure server software during a Terminal Services remote session. If you want to install or configure software on the server, contact your network administrator.
.
Language=Japanese
管理者だけがターミナルサービスのリモートセッション中にサーバーソフトウェアを追加、削除、または構成する権限を持っています。サーバーにソフトウェアをインストールまたは設定する場合は、ネットワーク管理者に連絡してください。
.

MessageId=1641
Severity=Success
Facility=System
SymbolicName=ERROR_SUCCESS_REBOOT_INITIATED
Language=English
The requested operation completed successfully. The system will be restarted so the changes can take effect.
.
Language=Japanese
要求された操作は正常に完了しました。変更を有効にするためにシステムが再起動されます。
.

MessageId=1642
Severity=Success
Facility=System
SymbolicName=ERROR_PATCH_TARGET_NOT_FOUND
Language=English
The upgrade patch cannot be installed by the ReactOS Installer service because the program to be upgraded may be missing, or the upgrade patch may update a different version of the program. Verify that the program to be upgraded exists on your computer and that you have the correct upgrade patch.
.
Language=Japanese
アップグレード対象のプログラムが存在しないか、アップグレードによって異なるバージョンのプログラムが更新される可能性があるため、ReactOS Installerサービスによってアップグレードをインストールできません。アップグレードするプログラムがコンピュータに存在すること、および正しいアップグレードがあることを確認してください。
.

MessageId=1643
Severity=Success
Facility=System
SymbolicName=ERROR_PATCH_PACKAGE_REJECTED
Language=English
The patch package is not permitted by software restriction policy.
.
Language=Japanese
アップデート パッケージは、ソフトウェア制限ポリシーで許可されていません。
.

MessageId=1644
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_TRANSFORM_REJECTED
Language=English
One or more customizations are not permitted by software restriction policy.
.
Language=Japanese
1つまたは複数のカスタマイズがソフトウェア制限ポリシーで許可されていません。
.

MessageId=1645
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_REMOTE_PROHIBITED
Language=English
The ReactOS Installer does not permit installation from a Remote Desktop Connection.
.
Language=Japanese
ReliefWin インストーラは、リモート デスクトップ接続からのインストールを許可しません。
.

MessageId=1654
Severity=Success
Facility=System
SymbolicName=ERROR_INSTALL_REJECTED
Language=English
The app that you are trying to run is not supported on this version of Windows.
.
Language=Japanese
The app that you are trying to run is not supported on this version of Windows.
.

MessageId=1700
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_STRING_BINDING
Language=English
The string binding is invalid.
.
Language=Japanese
文字列バインディングが無効です。
.

MessageId=1701
Severity=Success
Facility=System
SymbolicName=RPC_S_WRONG_KIND_OF_BINDING
Language=English
The binding handle is not the correct type.
.
Language=Japanese
バインディング ハンドルの型が正しくありません。
.

MessageId=1702
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_BINDING
Language=English
The binding handle is invalid.
.
Language=Japanese
バインディング ハンドルが無効です。
.

MessageId=1703
Severity=Success
Facility=System
SymbolicName=RPC_S_PROTSEQ_NOT_SUPPORTED
Language=English
The RPC protocol sequence is not supported.
.
Language=Japanese
RPCプロトコル シーケンスがサポートされていません。
.

MessageId=1704
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_RPC_PROTSEQ
Language=English
The RPC protocol sequence is invalid.
.
Language=Japanese
RPCプロトコルシーケンスが無効です。
.

MessageId=1705
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_STRING_UUID
Language=English
The string universal unique identifier (UUID) is invalid.
.
Language=Japanese
汎用一意識別子 (UUID)  文字列が無効です。
.

MessageId=1706
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_ENDPOINT_FORMAT
Language=English
The endpoint format is invalid.
.
Language=Japanese
エンドポイントの形式が無効です。
.

MessageId=1707
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_NET_ADDR
Language=English
The network address is invalid.
.
Language=Japanese
ネットワークアドレスが無効です。
.

MessageId=1708
Severity=Success
Facility=System
SymbolicName=RPC_S_NO_ENDPOINT_FOUND
Language=English
No endpoint was found.
.
Language=Japanese
エンドポイントが見つかりません。
.

MessageId=1709
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_TIMEOUT
Language=English
The timeout value is invalid.
.
Language=Japanese
タイムアウト値が無効です。
.

MessageId=1710
Severity=Success
Facility=System
SymbolicName=RPC_S_OBJECT_NOT_FOUND
Language=English
The object universal unique identifier (UUID) was not found.
.
Language=Japanese
オブジェクトの汎用一意識別子 (UUID) が見つかりませんでした。
.

MessageId=1711
Severity=Success
Facility=System
SymbolicName=RPC_S_ALREADY_REGISTERED
Language=English
The object universal unique identifier (UUID) has already been registered.
.
Language=Japanese
オブジェクト汎用一意識別子 (UUID) は既に登録されています。
.

MessageId=1712
Severity=Success
Facility=System
SymbolicName=RPC_S_TYPE_ALREADY_REGISTERED
Language=English
The type universal unique identifier (UUID) has already been registered.
.
Language=Japanese
型の汎用一意識別子 (UUID) はすでに登録されています。
.

MessageId=1713
Severity=Success
Facility=System
SymbolicName=RPC_S_ALREADY_LISTENING
Language=English
The RPC server is already listening.
.
Language=Japanese
RPCサーバーはすでにリッスン中です。
.

MessageId=1714
Severity=Success
Facility=System
SymbolicName=RPC_S_NO_PROTSEQS_REGISTERED
Language=English
No protocol sequences have been registered.
.
Language=Japanese
プロトコルシーケンスが登録されていません。
.

MessageId=1715
Severity=Success
Facility=System
SymbolicName=RPC_S_NOT_LISTENING
Language=English
The RPC server is not listening.
.
Language=Japanese
RPCサーバーはリッスンしていません。
.

MessageId=1716
Severity=Success
Facility=System
SymbolicName=RPC_S_UNKNOWN_MGR_TYPE
Language=English
The manager type is unknown.
.
Language=Japanese
マネージャタイプが不明です。
.

MessageId=1717
Severity=Success
Facility=System
SymbolicName=RPC_S_UNKNOWN_IF
Language=English
The interface is unknown.
.
Language=Japanese
インターフェースが不明です。
.

MessageId=1718
Severity=Success
Facility=System
SymbolicName=RPC_S_NO_BINDINGS
Language=English
There are no bindings.
.
Language=Japanese
バインディングがありません。
.

MessageId=1719
Severity=Success
Facility=System
SymbolicName=RPC_S_NO_PROTSEQS
Language=English
There are no protocol sequences.
.
Language=Japanese
プロトコルシーケンスがありません。
.

MessageId=1720
Severity=Success
Facility=System
SymbolicName=RPC_S_CANT_CREATE_ENDPOINT
Language=English
The endpoint cannot be created.
.
Language=Japanese
エンドポイントを作成できません。
.

MessageId=1721
Severity=Success
Facility=System
SymbolicName=RPC_S_OUT_OF_RESOURCES
Language=English
Not enough resources are available to complete this operation.
.
Language=Japanese
この操作を完了するのに十分なリソースがありません。
.

MessageId=1722
Severity=Success
Facility=System
SymbolicName=RPC_S_SERVER_UNAVAILABLE
Language=English
The RPC server is unavailable.
.
Language=Japanese
RPC サーバーが使用できません。
.

MessageId=1723
Severity=Success
Facility=System
SymbolicName=RPC_S_SERVER_TOO_BUSY
Language=English
The RPC server is too busy to complete this operation.
.
Language=Japanese
RPC サーバーがビジー状態でこの操作を完了できません。
.

MessageId=1724
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_NETWORK_OPTIONS
Language=English
The network options are invalid.
.
Language=Japanese
ネットワーク オプションが無効です。
.

MessageId=1725
Severity=Success
Facility=System
SymbolicName=RPC_S_NO_CALL_ACTIVE
Language=English
There are no remote procedure calls active on this thread.
.
Language=Japanese
このスレッドでアクティブなリモート プロシージャ コールはありません。
.

MessageId=1726
Severity=Success
Facility=System
SymbolicName=RPC_S_CALL_FAILED
Language=English
The remote procedure call failed.
.
Language=Japanese
リモート プロシージャ呼び出しに失敗しました。
.

MessageId=1727
Severity=Success
Facility=System
SymbolicName=RPC_S_CALL_FAILED_DNE
Language=English
The remote procedure call failed and did not execute.
.
Language=Japanese
リモート プロシージャ呼び出しに失敗しました。
.

MessageId=1728
Severity=Success
Facility=System
SymbolicName=RPC_S_PROTOCOL_ERROR
Language=English
A remote procedure call (RPC) protocol error occurred.
.
Language=Japanese
リモート プロシージャ コール (RPC)  プロトコル エラーが発生しました。
.

MessageId=1730
Severity=Success
Facility=System
SymbolicName=RPC_S_UNSUPPORTED_TRANS_SYN
Language=English
The transfer syntax is not supported by the RPC server.
.
Language=Japanese
転送構文は RPC サーバーによってサポートされていません。
.

MessageId=1732
Severity=Success
Facility=System
SymbolicName=RPC_S_UNSUPPORTED_TYPE
Language=English
The universal unique identifier (UUID) type is not supported.
.
Language=Japanese
ユニバーサル一意識別子 (UUID) タイプはサポートされていません。
.

MessageId=1733
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_TAG
Language=English
The tag is invalid.
.
Language=Japanese
タグが無効です。
.

MessageId=1734
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_BOUND
Language=English
The array bounds are invalid.
.
Language=Japanese
配列の境界が無効です。
.

MessageId=1735
Severity=Success
Facility=System
SymbolicName=RPC_S_NO_ENTRY_NAME
Language=English
The binding does not contain an entry name.
.
Language=Japanese
バインディングにエントリ名がありません。
.

MessageId=1736
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_NAME_SYNTAX
Language=English
The name syntax is invalid.
.
Language=Japanese
名前の構文が無効です。
.

MessageId=1737
Severity=Success
Facility=System
SymbolicName=RPC_S_UNSUPPORTED_NAME_SYNTAX
Language=English
The name syntax is not supported.
.
Language=Japanese
名前の構文はサポートされていません。
.

MessageId=1739
Severity=Success
Facility=System
SymbolicName=RPC_S_UUID_NO_ADDRESS
Language=English
No network address is available to use to construct a universal unique identifier (UUID).
.
Language=Japanese
汎用一意識別子 (UUID) を構築するために使用できるネットワーク アドレスがありません。
.

MessageId=1740
Severity=Success
Facility=System
SymbolicName=RPC_S_DUPLICATE_ENDPOINT
Language=English
The endpoint is a duplicate.
.
Language=Japanese
エンドポイントが重複しています。
.

MessageId=1741
Severity=Success
Facility=System
SymbolicName=RPC_S_UNKNOWN_AUTHN_TYPE
Language=English
The authentication type is unknown.
.
Language=Japanese
認証タイプが不明です。
.

MessageId=1742
Severity=Success
Facility=System
SymbolicName=RPC_S_MAX_CALLS_TOO_SMALL
Language=English
The maximum number of calls is too small.
.
Language=Japanese
最大コール数が少なすぎます。
.

MessageId=1743
Severity=Success
Facility=System
SymbolicName=RPC_S_STRING_TOO_LONG
Language=English
The string is too long.
.
Language=Japanese
文字列が長すぎます。
.

MessageId=1744
Severity=Success
Facility=System
SymbolicName=RPC_S_PROTSEQ_NOT_FOUND
Language=English
The RPC protocol sequence was not found.
.
Language=Japanese
RPC プロトコル シーケンスが見つかりません。
.

MessageId=1745
Severity=Success
Facility=System
SymbolicName=RPC_S_PROCNUM_OUT_OF_RANGE
Language=English
The procedure number is out of range.
.
Language=Japanese
プロシージャ番号が範囲外です。
.

MessageId=1746
Severity=Success
Facility=System
SymbolicName=RPC_S_BINDING_HAS_NO_AUTH
Language=English
The binding does not contain any authentication information.
.
Language=Japanese
バインディングに認証情報が含まれていません。
.

MessageId=1747
Severity=Success
Facility=System
SymbolicName=RPC_S_UNKNOWN_AUTHN_SERVICE
Language=English
The authentication service is unknown.
.
Language=Japanese
認証サービスが不明です。
.

MessageId=1748
Severity=Success
Facility=System
SymbolicName=RPC_S_UNKNOWN_AUTHN_LEVEL
Language=English
The authentication level is unknown.
.
Language=Japanese
認証レベルが不明です。
.

MessageId=1749
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_AUTH_IDENTITY
Language=English
The security context is invalid.
.
Language=Japanese
セキュリティコンテキストが無効です。
.

MessageId=1750
Severity=Success
Facility=System
SymbolicName=RPC_S_UNKNOWN_AUTHZ_SERVICE
Language=English
The authorization service is unknown.
.
Language=Japanese
認証サービスが不明です。
.

MessageId=1751
Severity=Success
Facility=System
SymbolicName=EPT_S_INVALID_ENTRY
Language=English
The entry is invalid.
.
Language=Japanese
エントリが無効です。
.

MessageId=1752
Severity=Success
Facility=System
SymbolicName=EPT_S_CANT_PERFORM_OP
Language=English
The server endpoint cannot perform the operation.
.
Language=Japanese
サーバーエンドポイントは操作を実行できません。
.

MessageId=1753
Severity=Success
Facility=System
SymbolicName=EPT_S_NOT_REGISTERED
Language=English
There are no more endpoints available from the endpoint mapper.
.
Language=Japanese
エンドポイントマッパーから利用可能なエンドポイントはありません。
.

MessageId=1754
Severity=Success
Facility=System
SymbolicName=RPC_S_NOTHING_TO_EXPORT
Language=English
No interfaces have been exported.
.
Language=Japanese
インターフェイスがエクスポートされていません。
.

MessageId=1755
Severity=Success
Facility=System
SymbolicName=RPC_S_INCOMPLETE_NAME
Language=English
The entry name is incomplete.
.
Language=Japanese
エントリ名が不完全です。
.

MessageId=1756
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_VERS_OPTION
Language=English
The version option is invalid.
.
Language=Japanese
バージョン オプションが無効です。
.

MessageId=1757
Severity=Success
Facility=System
SymbolicName=RPC_S_NO_MORE_MEMBERS
Language=English
There are no more members.
.
Language=Japanese
メンバがありません。
.

MessageId=1758
Severity=Success
Facility=System
SymbolicName=RPC_S_NOT_ALL_OBJS_UNEXPORTED
Language=English
There is nothing to unexport.
.
Language=Japanese
エクスポートするものがありません。
.

MessageId=1759
Severity=Success
Facility=System
SymbolicName=RPC_S_INTERFACE_NOT_FOUND
Language=English
The interface was not found.
.
Language=Japanese
インターフェイスが見つかりませんでした。
.

MessageId=1760
Severity=Success
Facility=System
SymbolicName=RPC_S_ENTRY_ALREADY_EXISTS
Language=English
The entry already exists.
.
Language=Japanese
エントリはすでに存在します。
.

MessageId=1761
Severity=Success
Facility=System
SymbolicName=RPC_S_ENTRY_NOT_FOUND
Language=English
The entry is not found.
.
Language=Japanese
エントリが見つかりません。
.

MessageId=1762
Severity=Success
Facility=System
SymbolicName=RPC_S_NAME_SERVICE_UNAVAILABLE
Language=English
The name service is unavailable.
.
Language=Japanese
ネーム サービスは使用できません。
.

MessageId=1763
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_NAF_ID
Language=English
The network address family is invalid.
.
Language=Japanese
ネットワーク アドレス ファミリが無効です。
.

MessageId=1764
Severity=Success
Facility=System
SymbolicName=RPC_S_CANNOT_SUPPORT
Language=English
The requested operation is not supported.
.
Language=Japanese
要求された操作はサポートされていません。
.

MessageId=1765
Severity=Success
Facility=System
SymbolicName=RPC_S_NO_CONTEXT_AVAILABLE
Language=English
No security context is available to allow impersonation.
.
Language=Japanese
なりすましを許可するセキュリティ コンテキストがありません。
.

MessageId=1766
Severity=Success
Facility=System
SymbolicName=RPC_S_INTERNAL_ERROR
Language=English
An internal error occurred in a remote procedure call (RPC).
.
Language=Japanese
リモートプロシージャコール (RPC) で内部エラーが発生しました。
.

MessageId=1767
Severity=Success
Facility=System
SymbolicName=RPC_S_ZERO_DIVIDE
Language=English
The RPC server attempted an integer division by zero.
.
Language=Japanese
RPC サーバーがゼロによる整数除算を試みました。
.

MessageId=1768
Severity=Success
Facility=System
SymbolicName=RPC_S_ADDRESS_ERROR
Language=English
An addressing error occurred in the RPC server.
.
Language=Japanese
RPC サーバーでアドレス指定エラーが発生しました。
.

MessageId=1769
Severity=Success
Facility=System
SymbolicName=RPC_S_FP_DIV_ZERO
Language=English
A floating-point operation at the RPC server caused a division by zero.
.
Language=Japanese
RPC サーバーで浮動小数点演算によりゼロによる除算が発生しました。
.

MessageId=1770
Severity=Success
Facility=System
SymbolicName=RPC_S_FP_UNDERFLOW
Language=English
A floating-point underflow occurred at the RPC server.
.
Language=Japanese
RPCサーバーで浮動小数点アンダーフローが発生しました。
.

MessageId=1771
Severity=Success
Facility=System
SymbolicName=RPC_S_FP_OVERFLOW
Language=English
A floating-point overflow occurred at the RPC server.
.
Language=Japanese
RPCサーバーで浮動小数点のオーバーフローが発生しました。
.

MessageId=1772
Severity=Success
Facility=System
SymbolicName=RPC_X_NO_MORE_ENTRIES
Language=English
The list of RPC servers available for the binding of auto handles has been exhausted.
.
Language=Japanese
オートハンドルのバインディングに使用できるRPCサーバーのリストを使い果たしました。
.

MessageId=1773
Severity=Success
Facility=System
SymbolicName=RPC_X_SS_CHAR_TRANS_OPEN_FAIL
Language=English
Unable to open the character translation table file.
.
Language=Japanese
文字変換テーブル ファイルを開くことができません。
.

MessageId=1774
Severity=Success
Facility=System
SymbolicName=RPC_X_SS_CHAR_TRANS_SHORT_FILE
Language=English
The file containing the character translation table has fewer than 512 bytes.
.
Language=Japanese
文字変換テーブルを含むファイルのバイト数が 512 バイト未満です。
.

MessageId=1775
Severity=Success
Facility=System
SymbolicName=RPC_X_SS_IN_NULL_CONTEXT
Language=English
A null context handle was passed from the client to the host during a remote procedure call.
.
Language=Japanese
リモート プロシージャ呼び出し中にクライアントからホストに NULL コンテキスト ハンドルが渡されました。
.

MessageId=1777
Severity=Success
Facility=System
SymbolicName=RPC_X_SS_CONTEXT_DAMAGED
Language=English
The context handle changed during a remote procedure call.
.
Language=Japanese
リモート プロシージャ呼び出し中にコンテキスト ハンドルが変更されました。
.

MessageId=1778
Severity=Success
Facility=System
SymbolicName=RPC_X_SS_HANDLES_MISMATCH
Language=English
The binding handles passed to a remote procedure call do not match.
.
Language=Japanese
リモート プロシージャ呼び出しに渡されたバインディング ハンドルが一致しません。
.

MessageId=1779
Severity=Success
Facility=System
SymbolicName=RPC_X_SS_CANNOT_GET_CALL_HANDLE
Language=English
The stub is unable to get the remote procedure call handle.
.
Language=Japanese
スタブがリモート プロシージャ コール ハンドルを取得できません。
.

MessageId=1780
Severity=Success
Facility=System
SymbolicName=RPC_X_NULL_REF_POINTER
Language=English
A null reference pointer was passed to the stub.
.
Language=Japanese
NULL 参照ポインタがスタブに渡されました。
.

MessageId=1781
Severity=Success
Facility=System
SymbolicName=RPC_X_ENUM_VALUE_OUT_OF_RANGE
Language=English
The enumeration value is out of range.
.
Language=Japanese
列挙値が範囲外です。
.

MessageId=1782
Severity=Success
Facility=System
SymbolicName=RPC_X_BYTE_COUNT_TOO_SMALL
Language=English
The byte count is too small.
.
Language=Japanese
バイト数が少なすぎます。
.

MessageId=1783
Severity=Success
Facility=System
SymbolicName=RPC_X_BAD_STUB_DATA
Language=English
The stub received bad data.
.
Language=Japanese
スタブが不正なデータを受信しました。
.

MessageId=1784
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_USER_BUFFER
Language=English
The supplied user buffer is not valid for the requested operation.
.
Language=Japanese
指定されたユーザー バッファは要求された操作に対して有効ではありません。
.

MessageId=1785
Severity=Success
Facility=System
SymbolicName=ERROR_UNRECOGNIZED_MEDIA
Language=English
The disk media is not recognized. It may not be formatted.
.
Language=Japanese
ディスク メディアが認識されません。フォーマットされていない可能性があります。
.

MessageId=1786
Severity=Success
Facility=System
SymbolicName=ERROR_NO_TRUST_LSA_SECRET
Language=English
The workstation does not have a trust secret.
.
Language=Japanese
ワークステーションにトラストシークレットがありません。
.

MessageId=1787
Severity=Success
Facility=System
SymbolicName=ERROR_NO_TRUST_SAM_ACCOUNT
Language=English
The security database on the server does not have a computer account for this workstation trust relationship.
.
Language=Japanese
サーバーのセキュリティ データベースに、このワークステーションの信頼関係のコンピュータ アカウントがありません。
.

MessageId=1788
Severity=Success
Facility=System
SymbolicName=ERROR_TRUSTED_DOMAIN_FAILURE
Language=English
The trust relationship between the primary domain and the trusted domain failed.
.
Language=Japanese
プライマリ ドメインと信頼されたドメイン間の信頼関係に失敗しました。
.

MessageId=1789
Severity=Success
Facility=System
SymbolicName=ERROR_TRUSTED_RELATIONSHIP_FAILURE
Language=English
The trust relationship between this workstation and the primary domain failed.
.
Language=Japanese
このワークステーションとプライマリドメインの信頼関係に失敗しました。
.

MessageId=1790
Severity=Success
Facility=System
SymbolicName=ERROR_TRUST_FAILURE
Language=English
The network logon failed.
.
Language=Japanese
ネットワーク ログオンに失敗しました。
.

MessageId=1791
Severity=Success
Facility=System
SymbolicName=RPC_S_CALL_IN_PROGRESS
Language=English
A remote procedure call is already in progress for this thread.
.
Language=Japanese
このスレッドでは、リモート プロシージャ呼び出しがすでに進行中です。
.

MessageId=1792
Severity=Success
Facility=System
SymbolicName=ERROR_NETLOGON_NOT_STARTED
Language=English
An attempt was made to logon, but the network logon service was not started.
.
Language=Japanese
ログオンしようとしましたが、ネットワーク ログオン サービスが開始されていません。
.

MessageId=1793
Severity=Success
Facility=System
SymbolicName=ERROR_ACCOUNT_EXPIRED
Language=English
The user's account has expired.
.
Language=Japanese
ユーザーアカウントの有効期限が切れました。
.

MessageId=1794
Severity=Success
Facility=System
SymbolicName=ERROR_REDIRECTOR_HAS_OPEN_HANDLES
Language=English
The redirector is in use and cannot be unloaded.
.
Language=Japanese
リダイレクタが使用中で、アンロードできません。
.

MessageId=1795
Severity=Success
Facility=System
SymbolicName=ERROR_PRINTER_DRIVER_ALREADY_INSTALLED
Language=English
The specified printer driver is already installed.
.
Language=Japanese
指定されたプリンタ ドライバはすでにインストールされています。
.

MessageId=1796
Severity=Success
Facility=System
SymbolicName=ERROR_UNKNOWN_PORT
Language=English
The specified port is unknown.
.
Language=Japanese
指定されたポートが不明です。
.

MessageId=1797
Severity=Success
Facility=System
SymbolicName=ERROR_UNKNOWN_PRINTER_DRIVER
Language=English
The printer driver is unknown.
.
Language=Japanese
プリンタドライバが不明です。
.

MessageId=1798
Severity=Success
Facility=System
SymbolicName=ERROR_UNKNOWN_PRINTPROCESSOR
Language=English
The print processor is unknown.
.
Language=Japanese
プリント プロセッサが不明です。
.

MessageId=1799
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_SEPARATOR_FILE
Language=English
The specified separator file is invalid.
.
Language=Japanese
指定されたセパレータファイルが無効です。
.

MessageId=1800
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PRIORITY
Language=English
The specified priority is invalid.
.
Language=Japanese
指定された優先順位が無効です。
.

MessageId=1801
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PRINTER_NAME
Language=English
The printer name is invalid.
.
Language=Japanese
プリンタ名が無効です。
.

MessageId=1802
Severity=Success
Facility=System
SymbolicName=ERROR_PRINTER_ALREADY_EXISTS
Language=English
The printer already exists.
.
Language=Japanese
プリンタは既に存在します。
.

MessageId=1803
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PRINTER_COMMAND
Language=English
The printer command is invalid.
.
Language=Japanese
プリンタコマンドが無効です。
.

MessageId=1804
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_DATATYPE
Language=English
The specified datatype is invalid.
.
Language=Japanese
指定されたデータ型が無効です。
.

MessageId=1805
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_ENVIRONMENT
Language=English
The environment specified is invalid.
.
Language=Japanese
指定された環境が無効です。
.

MessageId=1806
Severity=Success
Facility=System
SymbolicName=RPC_S_NO_MORE_BINDINGS
Language=English
There are no more bindings.
.
Language=Japanese
バインディングがありません。
.

MessageId=1807
Severity=Success
Facility=System
SymbolicName=ERROR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT
Language=English
The account used is an interdomain trust account. Use your global user account or local user account to access this server.
.
Language=Japanese
使用されたアカウントはドメイン間信頼アカウントです。このサーバーにアクセスするには、グローバルユーザーアカウントまたはローカルユーザーアカウントを使用してください。
.

MessageId=1808
Severity=Success
Facility=System
SymbolicName=ERROR_NOLOGON_WORKSTATION_TRUST_ACCOUNT
Language=English
The account used is a computer account. Use your global user account or local user account to access this server.
.
Language=Japanese
使用されたアカウントはコンピュータアカウントです。このサーバにアクセスするには、グローバルユーザアカウントまたはローカルユーザアカウントを使用してください。
.

MessageId=1809
Severity=Success
Facility=System
SymbolicName=ERROR_NOLOGON_SERVER_TRUST_ACCOUNT
Language=English
The account used is a server trust account. Use your global user account or local user account to access this server.
.
Language=Japanese
使用されたアカウントはサーバーの信頼アカウントです。このサーバーにアクセスするには、グローバルユーザーアカウントまたはローカルユーザーアカウントを使用してください。
.

MessageId=1810
Severity=Success
Facility=System
SymbolicName=ERROR_DOMAIN_TRUST_INCONSISTENT
Language=English
The name or security ID (SID) of the domain specified is inconsistent with the trust information for that domain.
.
Language=Japanese
指定されたドメインの名前またはセキュリティ ID (SID)  が、そのドメインの信頼情報と一致しません。
.

MessageId=1811
Severity=Success
Facility=System
SymbolicName=ERROR_SERVER_HAS_OPEN_HANDLES
Language=English
The server is in use and cannot be unloaded.
.
Language=Japanese
サーバーが使用中で、アンロードできません。
.

MessageId=1812
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_DATA_NOT_FOUND
Language=English
The specified image file did not contain a resource section.
.
Language=Japanese
指定されたイメージ ファイルにリソース セクションが含まれていません。
.

MessageId=1813
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_TYPE_NOT_FOUND
Language=English
The specified resource type cannot be found in the image file.
.
Language=Japanese
指定されたリソース タイプがイメージ ファイルに見つかりません。
.

MessageId=1814
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_NAME_NOT_FOUND
Language=English
The specified resource name cannot be found in the image file.
.
Language=Japanese
指定されたリソース名がイメージファイルで見つかりません。
.

MessageId=1815
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_LANG_NOT_FOUND
Language=English
The specified resource language ID cannot be found in the image file.
.
Language=Japanese
指定されたリソース言語IDがイメージファイルに見つかりません。
.

MessageId=1816
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_ENOUGH_QUOTA
Language=English
Not enough quota is available to process this command.
.
Language=Japanese
このコマンドを処理するのに十分なクォータがありません。
.

MessageId=1817
Severity=Success
Facility=System
SymbolicName=RPC_S_NO_INTERFACES
Language=English
No interfaces have been registered.
.
Language=Japanese
インターフェイスが登録されていません。
.

MessageId=1818
Severity=Success
Facility=System
SymbolicName=RPC_S_CALL_CANCELLED
Language=English
The remote procedure call was cancelled.
.
Language=Japanese
リモート プロシージャ コールがキャンセルされました。
.

MessageId=1819
Severity=Success
Facility=System
SymbolicName=RPC_S_BINDING_INCOMPLETE
Language=English
The binding handle does not contain all required information.
.
Language=Japanese
バインディング ハンドルに必要な情報がすべて含まれていません。
.

MessageId=1820
Severity=Success
Facility=System
SymbolicName=RPC_S_COMM_FAILURE
Language=English
A communications failure occurred during a remote procedure call.
.
Language=Japanese
リモート プロシージャ呼び出し中に通信エラーが発生しました。
.

MessageId=1821
Severity=Success
Facility=System
SymbolicName=RPC_S_UNSUPPORTED_AUTHN_LEVEL
Language=English
The requested authentication level is not supported.
.
Language=Japanese
要求された認証レベルはサポートされていません。
.

MessageId=1822
Severity=Success
Facility=System
SymbolicName=RPC_S_NO_PRINC_NAME
Language=English
No principal name registered.
.
Language=Japanese
プリンシパル名が登録されていません。
.

MessageId=1823
Severity=Success
Facility=System
SymbolicName=RPC_S_NOT_RPC_ERROR
Language=English
The error specified is not a valid Windows RPC error code.
.
Language=Japanese
指定されたエラーは有効な ReliefWin RPC エラーコードではありません。
.

MessageId=1824
Severity=Success
Facility=System
SymbolicName=RPC_S_UUID_LOCAL_ONLY
Language=English
A UUID that is valid only on this computer has been allocated.
.
Language=Japanese
このコンピュータでのみ有効なUUIDが割り当てられました。
.

MessageId=1825
Severity=Success
Facility=System
SymbolicName=RPC_S_SEC_PKG_ERROR
Language=English
A security package specific error occurred.
.
Language=Japanese
セキュリティパッケージ固有のエラーが発生しました。
.

MessageId=1826
Severity=Success
Facility=System
SymbolicName=RPC_S_NOT_CANCELLED
Language=English
Thread is not canceled.
.
Language=Japanese
スレッドがキャンセルされていません。
.

MessageId=1827
Severity=Success
Facility=System
SymbolicName=RPC_X_INVALID_ES_ACTION
Language=English
Invalid operation on the encoding/decoding handle.
.
Language=Japanese
エンコード/デコード ハンドルに対する操作が無効です。
.

MessageId=1828
Severity=Success
Facility=System
SymbolicName=RPC_X_WRONG_ES_VERSION
Language=English
Incompatible version of the serializing package.
.
Language=Japanese
シリアル化パッケージの互換性のないバージョンです。
.

MessageId=1829
Severity=Success
Facility=System
SymbolicName=RPC_X_WRONG_STUB_VERSION
Language=English
Incompatible version of the RPC stub.
.
Language=Japanese
RPC スタブの互換性のないバージョンです。
.

MessageId=1830
Severity=Success
Facility=System
SymbolicName=RPC_X_INVALID_PIPE_OBJECT
Language=English
The RPC pipe object is invalid or corrupted.
.
Language=Japanese
RPCパイプ オブジェクトが無効または壊れています。
.

MessageId=1831
Severity=Success
Facility=System
SymbolicName=RPC_X_WRONG_PIPE_ORDER
Language=English
An invalid operation was attempted on an RPC pipe object.
.
Language=Japanese
RPCパイプオブジェクトに対して無効な操作が試みられました。
.

MessageId=1832
Severity=Success
Facility=System
SymbolicName=RPC_X_WRONG_PIPE_VERSION
Language=English
Unsupported RPC pipe version.
.
Language=Japanese
サポートされていないRPCパイプのバージョンです。
.

MessageId=1898
Severity=Success
Facility=System
SymbolicName=RPC_S_GROUP_MEMBER_NOT_FOUND
Language=English
The group member was not found.
.
Language=Japanese
グループ メンバが見つかりません。
.

MessageId=1899
Severity=Success
Facility=System
SymbolicName=EPT_S_CANT_CREATE
Language=English
The endpoint mapper database entry could not be created.
.
Language=Japanese
エンドポイントマッパーのデータベースエントリを作成できませんでした。
.

MessageId=1900
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_OBJECT
Language=English
The object universal unique identifier (UUID) is the nil UUID.
.
Language=Japanese
オブジェクトの一意識別子 (UUID)  が nil UUID です。
.

MessageId=1901
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_TIME
Language=English
The specified time is invalid.
.
Language=Japanese
指定された時間は無効です。
.

MessageId=1902
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_FORM_NAME
Language=English
The specified form name is invalid.
.
Language=Japanese
指定されたフォーム名が無効です。
.

MessageId=1903
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_FORM_SIZE
Language=English
The specified form size is invalid.
.
Language=Japanese
指定されたフォームのサイズが無効です。
.

MessageId=1904
Severity=Success
Facility=System
SymbolicName=ERROR_ALREADY_WAITING
Language=English
The specified printer handle is already being waited on
.
Language=Japanese
指定されたプリンタ ハンドルは既に待ち受け中です。
.

MessageId=1905
Severity=Success
Facility=System
SymbolicName=ERROR_PRINTER_DELETED
Language=English
The specified printer has been deleted.
.
Language=Japanese
指定されたプリンタは削除されました。
.

MessageId=1906
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PRINTER_STATE
Language=English
The state of the printer is invalid.
.
Language=Japanese
プリンタの状態が無効です。
.

MessageId=1907
Severity=Success
Facility=System
SymbolicName=ERROR_PASSWORD_MUST_CHANGE
Language=English
The user's password must be changed before logging on the first time.
.
Language=Japanese
サインインする前にユーザのパスワードを変更する必要があります。
.

MessageId=1908
Severity=Success
Facility=System
SymbolicName=ERROR_DOMAIN_CONTROLLER_NOT_FOUND
Language=English
Could not find the domain controller for this domain.
.
Language=Japanese
このドメインのドメイン コントローラーが見つかりませんでした。
.

MessageId=1909
Severity=Success
Facility=System
SymbolicName=ERROR_ACCOUNT_LOCKED_OUT
Language=English
The referenced account is currently locked out and may not be used to log on.
.
Language=Japanese
参照されているアカウントは現在ロックアウトされているため、ログオンできません。
.

MessageId=1910
Severity=Success
Facility=System
SymbolicName=OR_INVALID_OXID
Language=English
The object exporter specified was not found.
.
Language=Japanese
指定されたオブジェクト エクスポーターが見つかりませんでした。
.

MessageId=1911
Severity=Success
Facility=System
SymbolicName=OR_INVALID_OID
Language=English
The object specified was not found.
.
Language=Japanese
指定されたオブジェクトが見つかりません。
.

MessageId=1912
Severity=Success
Facility=System
SymbolicName=OR_INVALID_SET
Language=English
The object resolver set specified was not found.
.
Language=Japanese
指定されたオブジェクト リゾルバ セットが見つかりません。
.

MessageId=1913
Severity=Success
Facility=System
SymbolicName=RPC_S_SEND_INCOMPLETE
Language=English
Some data remains to be sent in the request buffer.
.
Language=Japanese
要求バッファに送信するデータが残っています。
.

MessageId=1914
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_ASYNC_HANDLE
Language=English
Invalid asynchronous remote procedure call handle.
.
Language=Japanese
無効な非同期リモート プロシージャ コール ハンドルです。
.

MessageId=1915
Severity=Success
Facility=System
SymbolicName=RPC_S_INVALID_ASYNC_CALL
Language=English
Invalid asynchronous RPC call handle for this operation.
.
Language=Japanese
この操作の無効な非同期RPCコールハンドルです。
.

MessageId=1916
Severity=Success
Facility=System
SymbolicName=RPC_X_PIPE_CLOSED
Language=English
The RPC pipe object has already been closed.
.
Language=Japanese
RPC パイプ オブジェクトは既に閉じられています。
.

MessageId=1917
Severity=Success
Facility=System
SymbolicName=RPC_X_PIPE_DISCIPLINE_ERROR
Language=English
The RPC call completed before all pipes were processed.
.
Language=Japanese
すべてのパイプが処理される前にRPCコールが完了しました。
.

MessageId=1918
Severity=Success
Facility=System
SymbolicName=RPC_X_PIPE_EMPTY
Language=English
No more data is available from the RPC pipe.
.
Language=Japanese
RPCパイプから利用可能なデータはありません。
.

MessageId=1919
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SITENAME
Language=English
No site name is available for this machine.
.
Language=Japanese
このマシンで使用可能なサイト名がありません。
.

MessageId=1920
Severity=Success
Facility=System
SymbolicName=ERROR_CANT_ACCESS_FILE
Language=English
The file cannot be accessed by the system.
.
Language=Japanese
ファイルはシステムからアクセスできません。
.

MessageId=1921
Severity=Success
Facility=System
SymbolicName=ERROR_CANT_RESOLVE_FILENAME
Language=English
The name of the file cannot be resolved by the system.
.
Language=Japanese
ファイル名はシステムで解決できません。
.

MessageId=1922
Severity=Success
Facility=System
SymbolicName=RPC_S_ENTRY_TYPE_MISMATCH
Language=English
The entry is not of the expected type.
.
Language=Japanese
エントリが予期されたタイプではありません。
.

MessageId=1923
Severity=Success
Facility=System
SymbolicName=RPC_S_NOT_ALL_OBJS_EXPORTED
Language=English
Not all object UUIDs could be exported to the specified entry.
.
Language=Japanese
すべてのオブジェクト UUID を指定されたエントリにエクスポートできませんでした。
.

MessageId=1924
Severity=Success
Facility=System
SymbolicName=RPC_S_INTERFACE_NOT_EXPORTED
Language=English
Interface could not be exported to the specified entry.
.
Language=Japanese
指定されたエントリにインターフェイスをエクスポートできませんでした。
.

MessageId=1925
Severity=Success
Facility=System
SymbolicName=RPC_S_PROFILE_NOT_ADDED
Language=English
The specified profile entry could not be added.
.
Language=Japanese
指定されたプロファイル エントリを追加できませんでした。
.

MessageId=1926
Severity=Success
Facility=System
SymbolicName=RPC_S_PRF_ELT_NOT_ADDED
Language=English
The specified profile element could not be added.
.
Language=Japanese
指定されたプロファイル要素を追加できませんでした。
.

MessageId=1927
Severity=Success
Facility=System
SymbolicName=RPC_S_PRF_ELT_NOT_REMOVED
Language=English
The specified profile element could not be removed.
.
Language=Japanese
指定されたプロファイル要素を削除できませんでした。
.

MessageId=1928
Severity=Success
Facility=System
SymbolicName=RPC_S_GRP_ELT_NOT_ADDED
Language=English
The group element could not be added.
.
Language=Japanese
グループ要素を追加できませんでした。
.

MessageId=1929
Severity=Success
Facility=System
SymbolicName=RPC_S_GRP_ELT_NOT_REMOVED
Language=English
The group element could not be removed.
.
Language=Japanese
グループ要素を削除できませんでした。
.

MessageId=1930
Severity=Success
Facility=System
SymbolicName=ERROR_KM_DRIVER_BLOCKED
Language=English
The printer driver is not compatible with a policy enabled on your computer that blocks NT 4.0 drivers.
.
Language=Japanese
プリンタ ドライバは、NT 4.0 ドライバをブロックするコンピュータで有効になっているポリシーと互換性がありません。
.

MessageId=1931
Severity=Success
Facility=System
SymbolicName=ERROR_CONTEXT_EXPIRED
Language=English
The context has expired and can no longer be used.
.
Language=Japanese
コンテキストの有効期限が切れたため、使用できなくなりました。
.

MessageId=1932
Severity=Success
Facility=System
SymbolicName=ERROR_PER_USER_TRUST_QUOTA_EXCEEDED
Language=English
The current user's delegated trust creation quota has been exceeded.
.
Language=Japanese
現在のユーザーの委任トラスト作成クォータを超えています。
.

MessageId=1933
Severity=Success
Facility=System
SymbolicName=ERROR_ALL_USER_TRUST_QUOTA_EXCEEDED
Language=English
The total delegated trust creation quota has been exceeded.
.
Language=Japanese
委任されたトラストの作成クォータの合計を超えました。
.

MessageId=1934
Severity=Success
Facility=System
SymbolicName=ERROR_USER_DELETE_TRUST_QUOTA_EXCEEDED
Language=English
The current user's delegated trust deletion quota has been exceeded.
.
Language=Japanese
現在のユーザの委任されたトラストの削除クオータを超えています。
.

MessageId=2000
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PIXEL_FORMAT
Language=English
The pixel format is invalid.
.
Language=Japanese
ピクセル形式が無効です。
.

MessageId=2001
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_DRIVER
Language=English
The specified driver is invalid.
.
Language=Japanese
指定されたドライバが無効です。
.

MessageId=2002
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_WINDOW_STYLE
Language=English
The window style or class attribute is invalid for this operation.
.
Language=Japanese
ウィンドウのスタイルまたはクラス属性が無効です。
.

MessageId=2003
Severity=Success
Facility=System
SymbolicName=ERROR_METAFILE_NOT_SUPPORTED
Language=English
The requested metafile operation is not supported.
.
Language=Japanese
要求されたメタファイル操作はサポートされていません。
.

MessageId=2004
Severity=Success
Facility=System
SymbolicName=ERROR_TRANSFORM_NOT_SUPPORTED
Language=English
The requested transformation operation is not supported.
.
Language=Japanese
要求された変換操作はサポートされていません。
.

MessageId=2005
Severity=Success
Facility=System
SymbolicName=ERROR_CLIPPING_NOT_SUPPORTED
Language=English
The requested clipping operation is not supported.
.
Language=Japanese
要求されたクリッピング操作はサポートされていません。
.

MessageId=2010
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_CMM
Language=English
The specified color management module is invalid.
.
Language=Japanese
指定されたカラー マネージメント モジュールが無効です。
.

MessageId=2011
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PROFILE
Language=English
The specified color profile is invalid.
.
Language=Japanese
指定されたカラープロファイルが無効です。
.

MessageId=2012
Severity=Success
Facility=System
SymbolicName=ERROR_TAG_NOT_FOUND
Language=English
The specified tag was not found.
.
Language=Japanese
指定されたタグが見つかりません。
.

MessageId=2013
Severity=Success
Facility=System
SymbolicName=ERROR_TAG_NOT_PRESENT
Language=English
A required tag is not present.
.
Language=Japanese
必須タグが存在しません。
.

MessageId=2014
Severity=Success
Facility=System
SymbolicName=ERROR_DUPLICATE_TAG
Language=English
The specified tag is already present.
.
Language=Japanese
指定されたタグは既に存在します。
.

MessageId=2015
Severity=Success
Facility=System
SymbolicName=ERROR_PROFILE_NOT_ASSOCIATED_WITH_DEVICE
Language=English
The specified color profile is not associated with any device.
.
Language=Japanese
指定されたカラープロファイルが指定されたデバイスに関連付けられていません。
.

MessageId=2016
Severity=Success
Facility=System
SymbolicName=ERROR_PROFILE_NOT_FOUND
Language=English
The specified color profile was not found.
.
Language=Japanese
指定されたカラープロファイルが見つかりません。
.

MessageId=2017
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_COLORSPACE
Language=English
The specified color space is invalid.
.
Language=Japanese
指定された色空間が無効です。
.

MessageId=2018
Severity=Success
Facility=System
SymbolicName=ERROR_ICM_NOT_ENABLED
Language=English
Image Color Management is not enabled.
.
Language=Japanese
イメージ カラー マネージメントが有効になっていません。
.

MessageId=2019
Severity=Success
Facility=System
SymbolicName=ERROR_DELETING_ICM_XFORM
Language=English
There was an error while deleting the color transform.
.
Language=Japanese
色変換の削除中にエラーが発生しました。
.

MessageId=2020
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_TRANSFORM
Language=English
The specified color transform is invalid.
.
Language=Japanese
指定された色変換が無効です。
.

MessageId=2021
Severity=Success
Facility=System
SymbolicName=ERROR_COLORSPACE_MISMATCH
Language=English
The specified transform does not match the bitmap's color space.
.
Language=Japanese
指定されたトランスフォームはビットマップの色空間と一致しません。
.

MessageId=2022
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_COLORINDEX
Language=English
The specified named color index is not present in the profile.
.
Language=Japanese
指定されたカラーインデックスがプロファイルに存在しません。
.

MessageId=2108
Severity=Success
Facility=System
SymbolicName=ERROR_CONNECTED_OTHER_PASSWORD
Language=English
The network connection was made successfully, but the user had to be prompted for a password other than the one originally specified.
.
Language=Japanese
ネットワーク接続に成功しましたが、最初に指定されたパスワード以外のパスワードを入力する必要がありました。
.

MessageId=2109
Severity=Success
Facility=System
SymbolicName=ERROR_CONNECTED_OTHER_PASSWORD_DEFAULT
Language=English
The network connection was made successfully using default credentials.
.
Language=Japanese
既定の認証情報を使用してネットワーク接続に成功しました。
.

MessageId=2202
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_USERNAME
Language=English
The specified username is invalid.
.
Language=Japanese
指定されたユーザー名が無効です。
.

MessageId=2250
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_CONNECTED
Language=English
This network connection does not exist.
.
Language=Japanese
このネットワーク接続は存在しません。
.

MessageId=2401
Severity=Success
Facility=System
SymbolicName=ERROR_OPEN_FILES
Language=English
This network connection has files open or requests pending.
.
Language=Japanese
このネットワーク接続には、開いているファイルまたは保留中の要求があります。
.

MessageId=2402
Severity=Success
Facility=System
SymbolicName=ERROR_ACTIVE_CONNECTIONS
Language=English
Active connections still exist.
.
Language=Japanese
アクティブな接続がまだ存在します。
.

MessageId=2404
Severity=Success
Facility=System
SymbolicName=ERROR_DEVICE_IN_USE
Language=English
The device is in use by an active process and cannot be disconnected.
.
Language=Japanese
デバイスはアクティブなプロセスによって使用中であり、切断できません。
.

MessageId=3000
Severity=Success
Facility=System
SymbolicName=ERROR_UNKNOWN_PRINT_MONITOR
Language=English
The specified print monitor is unknown.
.
Language=Japanese
指定されたプリントモニタが不明です。
.

MessageId=3001
Severity=Success
Facility=System
SymbolicName=ERROR_PRINTER_DRIVER_IN_USE
Language=English
The specified printer driver is currently in use.
.
Language=Japanese
指定されたプリンタドライバは現在使用中です。
.

MessageId=3002
Severity=Success
Facility=System
SymbolicName=ERROR_SPOOL_FILE_NOT_FOUND
Language=English
The spool file was not found.
.
Language=Japanese
スプールファイルが見つかりません。
.

MessageId=3003
Severity=Success
Facility=System
SymbolicName=ERROR_SPL_NO_STARTDOC
Language=English
A StartDocPrinter call was not issued.
.
Language=Japanese
StartDocPrinterコールが発行されませんでした。
.

MessageId=3004
Severity=Success
Facility=System
SymbolicName=ERROR_SPL_NO_ADDJOB
Language=English
An AddJob call was not issued.
.
Language=Japanese
AddJob呼び出しが発行されませんでした。
.

MessageId=3005
Severity=Success
Facility=System
SymbolicName=ERROR_PRINT_PROCESSOR_ALREADY_INSTALLED
Language=English
The specified print processor has already been installed.
.
Language=Japanese
指定されたプリント プロセッサはすでにインストールされています。
.

MessageId=3006
Severity=Success
Facility=System
SymbolicName=ERROR_PRINT_MONITOR_ALREADY_INSTALLED
Language=English
The specified print monitor has already been installed.
.
Language=Japanese
指定されたプリントモニターはすでにインストールされています。
.

MessageId=3007
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_PRINT_MONITOR
Language=English
The specified print monitor does not have the required functions.
.
Language=Japanese
指定されたプリントモニターに必要な機能がありません。
.

MessageId=3008
Severity=Success
Facility=System
SymbolicName=ERROR_PRINT_MONITOR_IN_USE
Language=English
The specified print monitor is currently in use.
.
Language=Japanese
指定された印刷モニターは現在使用中です。
.

MessageId=3009
Severity=Success
Facility=System
SymbolicName=ERROR_PRINTER_HAS_JOBS_QUEUED
Language=English
The requested operation is not allowed when there are jobs queued to the printer.
.
Language=Japanese
プリンタにキューイングされているジョブがある場合、要求された操作は許可されません。
.

MessageId=3010
Severity=Success
Facility=System
SymbolicName=ERROR_SUCCESS_REBOOT_REQUIRED
Language=English
The requested operation is successful. Changes will not be effective until the system is rebooted.
.
Language=Japanese
要求された操作は成功しました。要求された操作は成功しました。
.

MessageId=3011
Severity=Success
Facility=System
SymbolicName=ERROR_SUCCESS_RESTART_REQUIRED
Language=English
The requested operation is successful. Changes will not be effective until the service is restarted.
.
Language=Japanese
要求された操作は成功しました。サービスが再起動されるまで変更は有効になりません。
.

MessageId=3012
Severity=Success
Facility=System
SymbolicName=ERROR_PRINTER_NOT_FOUND
Language=English
No printers were found.
.
Language=Japanese
プリンタが見つかりませんでした。
.

MessageId=3013
Severity=Success
Facility=System
SymbolicName=ERROR_PRINTER_DRIVER_WARNED
Language=English
The printer driver is known to be unreliable.
.
Language=Japanese
プリンタドライバが信頼できないことが判明しました。
.

MessageId=3014
Severity=Success
Facility=System
SymbolicName=ERROR_PRINTER_DRIVER_BLOCKED
Language=English
The printer driver is known to harm the system.
.
Language=Japanese
プリンタドライバがシステムに悪影響を与えることが判明しました。
.

MessageId=3100
Severity=Success
Facility=System
SymbolicName=ERROR_XML_UNDEFINED_ENTITY
Language=English
The XML contains an entity reference to an undefined entity.
.
Language=Japanese
ネットワークソフトウェアエラーが発生したため、操作に失敗しました。
.

MessageId=3101
Severity=Success
Facility=System
SymbolicName=ERROR_XML_MALFORMED_ENTITY
Language=English
The XML contains a malformed entity reference.
.
Language=Japanese
オプション %1 で制御されるリソースが不足しました。
.

MessageId=3102
Severity=Success
Facility=System
SymbolicName=ERROR_XML_CHAR_NOT_IN_RANGE
Language=English
The XML contains a character which is not permitted in XML.
.
Language=Japanese
サービスがネットワーク制御ブロック (NCB)  のセグメントで長期ロックを取得できませんでした。
エラーコードはデータです。
.

MessageId=3200
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_EXTERNAL_PROXY
Language=English
The manifest contained a duplicate definition for external proxy stub %1 at (%1:%2,%3)
.
Language=Japanese
マニフェストは、(%1:%2,%3)における外部プロキシ スタブ %1 に重複する定義が含まれています。
.

MessageId=3201
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_ASSEMBLY_REFERENCE
Language=English
The manifest already contains a reference to %4 - a second reference was found at (%1:%2,%3)
.
Language=Japanese
マニフェストはすでに %4 への参照を含んでいます。2 番目の参照は (%1:%2,%3) で見つかりました。
.

MessageId=3202
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_INVALID_ASSEMBLY_REFERENCE
Language=English
The assembly reference at (%1:%2,%3) is invalid.
.
Language=Japanese
(%1:%2,%3) におけるアセンブリ参照は無効です。
.

MessageId=3203
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_INVALID_ASSEMBLY_DEFINITION
Language=English
The assembly definition at (%1:%2,%3) is invalid.
.
Language=Japanese
(%1:%2,%3) におけるアセンブリ定義は無効です。
.

MessageId=3204
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_WINDOW_CLASS
Language=English
The manifest already contained the window class %4, found a second declaration at (%1:%2,%3)
.
Language=Japanese
マニフェストはすでにウィンドウ クラス %4 を含んでいます。2 番目の宣言は (%1:%2,%3) で見つかりました。
.

MessageId=3205
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_PROGID
Language=English
The manifest already declared the progId %4, found a second declaration at (%1:%2,%3)
.
Language=Japanese
マニフェストはすでに progId %4 を宣言しています。2 番目の定義は (%1:%2,%3) で見つかりました。
.

MessageId=3206
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_NOINHERIT
Language=English
Only one noInherit tag may be present in a manifest, found a second tag at (%1:%2,%3)
.
Language=Japanese
マニフェスト内部に noInherit タグが一つだけなければなりません。2 番目のタグが (%1:%2,%3) で見つかりました。
.

MessageId=3207
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_NOINHERITABLE
Language=English
Only one noInheritable tag may be present in a manifest, found a second tag at (%1:%2,%3)
.
Language=Japanese
マニフェストには、noInheritable タグが 1 つだけ存在する可能性があります。2 番目のタグが (%1:%2,%3) で見つかりました。
.

MessageId=3208
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_COM_CLASS
Language=English
The manifest contained a duplicate declaration of COM class %4 at (%1:%2,%3)
.
Language=Japanese
マニフェスト内部の (%1:%2,%3) において COM クラス %4 の重複宣言が含まれていました。
.

MessageId=3209
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_FILE_NAME
Language=English
The manifest already declared the file %4, a second definition was found at (%1:%2,%3)
.
Language=Japanese
マニフェストでファイル %4 が既に宣言されていますが、(%1:%2,%3) に 2 番目の定義が見つかりました。
.

MessageId=3210
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_CLR_SURROGATE
Language=English
CLR surrogate %1 was already defined, second definition at (%1:%2,%3) is invalid.
.
Language=Japanese
CLR サロゲート %1 は既に定義されています。(%1:%2,%3) での 2 番目の定義は無効です。
.

MessageId=3211
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_TYPE_LIBRARY
Language=English
Type library %1 was already defined, second definition at (%1:%2,%3) is invalid.
.
Language=Japanese
型ライブラリ %1 は既に定義されています。(%1:%2,%3) での 2 番目の定義は無効です。
.

MessageId=3212
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_PROXY_STUB
Language=English
Proxy stub definition %1 was already defined, second definition at (%1:%2,%3) is invalid.
.
Language=Japanese
プロキシ・スタブ定義 %1 は既に定義されているので、(%1:%2,%3) での 2 番目の定義は無効です。
.

MessageId=3213
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_CATEGORY_NAME
Language=English
Category friendly name %4 was already used, second definition was found at (%1:%2,%3) is invalid.
.
Language=Japanese
カテゴリ・フレンドリー名 %4 は既に使用されています。(%1:%2,%3) に 2 番目の定義が見つかりましたが、無効です。
.

MessageId=3214
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_DUPLICATE_TOP_LEVEL_IDENTITY_FOUND
Language=English
Only one top-level assemblyIdentity tag may be present in a manifest. A second tag with identity %4 was found at (%1:%2,%3)
.
Language=Japanese
マニフェストには、トップレベルの assemblyIdentity タグを 1 つだけ含めることができます。ID %4 を持つ 2 番目のタグが (%1:%2,%3) に見つかりました。
.

MessageId=3215
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_UNKNOWN_ROOT_ELEMENT
Language=English
The root element for a manifest found at (%1:%2,%3) was not expected or was of the wrong version.
.
Language=Japanese
(%1:%2,%3) で見つかったマニフェストのルート要素が予期されていないか、間違ったバージョンでした。
.

MessageId=3216
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_INVALID_ELEMENT
Language=English
The element found at (%1:%2,%3) was not expected according to the manifest schema.
.
Language=Japanese
(%1:%2,%3) で見つかった要素は、マニフェスト スキーマに従って予期されていませんでした。
.

MessageId=3217
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_MISSING_REQUIRED_ATTRIBUTE
Language=English
The element found at (%1:%2,%3) was missing the required attribute '%4'. See the manifest schema for more information
.
Language=Japanese
(%1:%2,%3) で見つかった要素には必須属性 '%4' がありませんでした。詳細は、マニフェスト スキーマを参照してください。
.

MessageId=3218
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_INVALID_ATTRIBUTE_VALUE
Language=English
The attribute value %4 at (%1:%2,%3) was invalid according to the schema.
.
Language=Japanese
(%1:%2,%3) の属性値 %4 はスキーマに従って無効でした。
.

MessageId=3219
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_COMPILER_UNEXPECTED_PCDATA
Language=English
PCDATA or CDATA found at (%1:%2,%3) in the source document was not expected in the parent element %4.
.
Language=Japanese
ソース ドキュメント内の (%1:%2,%3) で見つかった PCDATA または CDATA は、親要素 %4 では予期されませんでした。
.

MessageId=3220
Severity=Success
Facility=System
SymbolicName=ERROR_PCM_DUPLICATE_STRING_TABLE_ENT
Language=English
The string table entry with culture %4, name %5, and value '%6' at (%1:%2,%3) duplicated a previous entry.
.
Language=Japanese
(%1:%2,%3) のカルチャ %4、名前 %5、値 '%6' を持つ文字列テーブル・エントリが前のエントリと重複しています。
.

MessageId=4000
Severity=Success
Facility=System
SymbolicName=ERROR_WINS_INTERNAL
Language=English
WINS encountered an error while processing the command.
.
Language=Japanese
コマンドの処理中に WINS でエラーが発生しました。
.

MessageId=4001
Severity=Success
Facility=System
SymbolicName=ERROR_CAN_NOT_DEL_LOCAL_WINS
Language=English
The local WINS cannot be deleted.
.
Language=Japanese
ローカル WINS を削除できません。
.

MessageId=4002
Severity=Success
Facility=System
SymbolicName=ERROR_STATIC_INIT
Language=English
The importation from the file failed.
.
Language=Japanese
ファイルからのインポートに失敗しました。
.

MessageId=4003
Severity=Success
Facility=System
SymbolicName=ERROR_INC_BACKUP
Language=English
The backup failed. Was a full backup done before?
.
Language=Japanese
バックアップに失敗しました。前に完全バックアップが行われましたか。
.

MessageId=4004
Severity=Success
Facility=System
SymbolicName=ERROR_FULL_BACKUP
Language=English
The backup failed. Check the directory to which you are backing the database.
.
Language=Japanese
バックアップに失敗しました。データベースのバックアップ先ディレクトリを確認してください。
.

MessageId=4005
Severity=Success
Facility=System
SymbolicName=ERROR_REC_NON_EXISTENT
Language=English
The name does not exist in the WINS database.
.
Language=Japanese
名前が WINS データベースに存在しません。
.

MessageId=4006
Severity=Success
Facility=System
SymbolicName=ERROR_RPL_NOT_ALLOWED
Language=English
Replication with a nonconfigured partner is not allowed.
.
Language=Japanese
構成されていないパートナーとのレプリケーションは許可されていません。
.

MessageId=4100
Severity=Success
Facility=System
SymbolicName=ERROR_DHCP_ADDRESS_CONFLICT
Language=English
The DHCP client has obtained an IP address that is already in use on the network. The local interface will be disabled until the DHCP client can obtain a new address.
.
Language=Japanese
DHCPクライアントが、ネットワーク上ですでに使用されているIPアドレスを取得しました。DHCPクライアントが新しいアドレスを取得できるようになるまで、ローカル インタフェースは無効になります。
.

MessageId=4200
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_GUID_NOT_FOUND
Language=English
The GUID passed was not recognized as valid by a WMI data provider.
.
Language=Japanese
渡された GUID が WMI データプロバイダによって有効であると認識されませんでした。
.

MessageId=4201
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_INSTANCE_NOT_FOUND
Language=English
The instance name passed was not recognized as valid by a WMI data provider.
.
Language=Japanese
渡されたインスタンス名は、WMI データ プロバイダによって有効であると認識されませんでした。
.

MessageId=4202
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_ITEMID_NOT_FOUND
Language=English
The data item ID passed was not recognized as valid by a WMI data provider.
.
Language=Japanese
渡されたデータ項目 ID は、WMI データ プロバイダーによって有効であると認識されませんでした。
.

MessageId=4203
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_TRY_AGAIN
Language=English
The WMI request could not be completed and should be retried.
.
Language=Japanese
WMI 要求を完了できませんでした。
.

MessageId=4204
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_DP_NOT_FOUND
Language=English
The WMI data provider could not be located.
.
Language=Japanese
WMI データ プロバイダーが見つかりません。
.

MessageId=4205
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_UNRESOLVED_INSTANCE_REF
Language=English
The WMI data provider references an instance set that has not been registered.
.
Language=Japanese
WMI データ プロバイダーは、登録されていないインスタンス セットを参照しています。
.

MessageId=4206
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_ALREADY_ENABLED
Language=English
The WMI data block or event notification has already been enabled.
.
Language=Japanese
WMI データ ブロックまたはイベント通知はすでに有効になっています。
.

MessageId=4207
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_GUID_DISCONNECTED
Language=English
The WMI data block is no longer available.
.
Language=Japanese
WMI データ ブロックは使用できなくなりました。
.

MessageId=4208
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_SERVER_UNAVAILABLE
Language=English
The WMI data service is not available.
.
Language=Japanese
WMI データ サービスは使用できません。
.

MessageId=4209
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_DP_FAILED
Language=English
The WMI data provider failed to carry out the request.
.
Language=Japanese
WMI データ プロバイダーが要求を実行できませんでした。
.

MessageId=4210
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_INVALID_MOF
Language=English
The WMI MOF information is not valid.
.
Language=Japanese
WMI MOF 情報が無効です。
.

MessageId=4211
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_INVALID_REGINFO
Language=English
The WMI registration information is not valid.
.
Language=Japanese
WMI 登録情報が無効です。
.

MessageId=4212
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_ALREADY_DISABLED
Language=English
The WMI data block or event notification has already been disabled.
.
Language=Japanese
WMI データ ブロックまたはイベント通知はすでに無効になっています。
.

MessageId=4213
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_READ_ONLY
Language=English
The WMI data item or data block is read-only.
.
Language=Japanese
WMI データ項目またはデータ ブロックは読み取り専用です。
.

MessageId=4214
Severity=Success
Facility=System
SymbolicName=ERROR_WMI_SET_FAILURE
Language=English
The WMI data item or data block could not be changed.
.
Language=Japanese
WMI データ項目またはデータ ブロックを変更できませんでした。
.

MessageId=4300
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_MEDIA
Language=English
The media identifier does not represent a valid medium.
.
Language=Japanese
メディア識別子が有効なメディアを表していません。
.

MessageId=4301
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_LIBRARY
Language=English
The library identifier does not represent a valid library.
.
Language=Japanese
ライブラリ識別子が有効なライブラリを表していません。
.

MessageId=4302
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_MEDIA_POOL
Language=English
The media pool identifier does not represent a valid media pool.
.
Language=Japanese
メディアプール識別子が有効なメディアプールを表していません。
.

MessageId=4303
Severity=Success
Facility=System
SymbolicName=ERROR_DRIVE_MEDIA_MISMATCH
Language=English
The drive and medium are not compatible or exist in different libraries.
.
Language=Japanese
ドライブとメディアに互換性がないか、異なるライブラリに存在します。
.

MessageId=4304
Severity=Success
Facility=System
SymbolicName=ERROR_MEDIA_OFFLINE
Language=English
The medium currently exists in an offline library and must be online to perform this operation.
.
Language=Japanese
メディアは現在オフラインのライブラリに存在するため、この操作を実行するにはオンラインにする必要があります。
.

MessageId=4305
Severity=Success
Facility=System
SymbolicName=ERROR_LIBRARY_OFFLINE
Language=English
The operation cannot be performed on an offline library.
.
Language=Japanese
この操作はオフラインのライブラリでは実行できません。
.

MessageId=4306
Severity=Success
Facility=System
SymbolicName=ERROR_EMPTY
Language=English
The library, drive, or media pool is empty.
.
Language=Japanese
ライブラリ、ドライブ、またはメディアプールが空です。
.

MessageId=4307
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_EMPTY
Language=English
The library, drive, or media pool must be empty to perform this operation.
.
Language=Japanese
この操作を実行するには、ライブラリ、ドライブ、またはメディア プールが空である必要があります。
.

MessageId=4308
Severity=Success
Facility=System
SymbolicName=ERROR_MEDIA_UNAVAILABLE
Language=English
No media is currently available in this media pool or library.
.
Language=Japanese
このメディアプールまたはライブラリで現在使用可能なメディアはありません。
.

MessageId=4309
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_DISABLED
Language=English
A resource required for this operation is disabled.
.
Language=Japanese
この操作に必要なリソースが無効です。
.

MessageId=4310
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_CLEANER
Language=English
The media identifier does not represent a valid cleaner.
.
Language=Japanese
メディア識別子が有効なクリーナーを表していません。
.

MessageId=4311
Severity=Success
Facility=System
SymbolicName=ERROR_UNABLE_TO_CLEAN
Language=English
The drive cannot be cleaned or does not support cleaning.
.
Language=Japanese
ドライブをクリーニングできないか、クリーニングをサポートしていません。
.

MessageId=4312
Severity=Success
Facility=System
SymbolicName=ERROR_OBJECT_NOT_FOUND
Language=English
The object identifier does not represent a valid object.
.
Language=Japanese
オブジェクト識別子が有効なオブジェクトを表していません。
.

MessageId=4313
Severity=Success
Facility=System
SymbolicName=ERROR_DATABASE_FAILURE
Language=English
Unable to read from or write to the database.
.
Language=Japanese
データベースからの読み取りまたはデータベースへの書き込みができません。
.

MessageId=4314
Severity=Success
Facility=System
SymbolicName=ERROR_DATABASE_FULL
Language=English
The database is full.
.
Language=Japanese
データベースがいっぱいです。
.

MessageId=4315
Severity=Success
Facility=System
SymbolicName=ERROR_MEDIA_INCOMPATIBLE
Language=English
The medium is not compatible with the device or media pool.
.
Language=Japanese
メディアがデバイスまたはメディア プールと互換性がありません。
.

MessageId=4316
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_NOT_PRESENT
Language=English
The resource required for this operation does not exist.
.
Language=Japanese
この操作に必要なリソースが存在しません。
.

MessageId=4317
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_OPERATION
Language=English
The operation identifier is not valid.
.
Language=Japanese
操作識別子が無効です。
.

MessageId=4318
Severity=Success
Facility=System
SymbolicName=ERROR_MEDIA_NOT_AVAILABLE
Language=English
The media is not mounted or ready for use.
.
Language=Japanese
メディアがマウントされていないか、使用可能な状態になっていません。
.

MessageId=4319
Severity=Success
Facility=System
SymbolicName=ERROR_DEVICE_NOT_AVAILABLE
Language=English
The device is not ready for use.
.
Language=Japanese
デバイスは使用できません。
.

MessageId=4320
Severity=Success
Facility=System
SymbolicName=ERROR_REQUEST_REFUSED
Language=English
The operator or administrator has refused the request.
.
Language=Japanese
オペレータまたは管理者が要求を拒否しました。
.

MessageId=4321
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_DRIVE_OBJECT
Language=English
The drive identifier does not represent a valid drive.
.
Language=Japanese
ドライブ識別子が有効なドライブを表していません。
.

MessageId=4322
Severity=Success
Facility=System
SymbolicName=ERROR_LIBRARY_FULL
Language=English
Library is full. No slot is available for use.
.
Language=Japanese
ライブラリがいっぱいです。使用可能なスロットがありません。
.

MessageId=4323
Severity=Success
Facility=System
SymbolicName=ERROR_MEDIUM_NOT_ACCESSIBLE
Language=English
The transport cannot access the medium.
.
Language=Japanese
トランスポートがメディアにアクセスできません。
.

MessageId=4324
Severity=Success
Facility=System
SymbolicName=ERROR_UNABLE_TO_LOAD_MEDIUM
Language=English
Unable to load the medium into the drive.
.
Language=Japanese
ドライブに媒体をロードできません。
.

MessageId=4325
Severity=Success
Facility=System
SymbolicName=ERROR_UNABLE_TO_INVENTORY_DRIVE
Language=English
Unable to retrieve status about the drive.
.
Language=Japanese
ドライブのステータスを取得できません。
.

MessageId=4326
Severity=Success
Facility=System
SymbolicName=ERROR_UNABLE_TO_INVENTORY_SLOT
Language=English
Unable to retrieve status about the slot.
.
Language=Japanese
ドライブのステータスを取得できません: スロットのステータスを取得できません。
.

MessageId=4327
Severity=Success
Facility=System
SymbolicName=ERROR_UNABLE_TO_INVENTORY_TRANSPORT
Language=English
Unable to retrieve status about the transport.
.
Language=Japanese
スロットのステータスを取得できません: トランスポートのステータスを取得できません。
.

MessageId=4328
Severity=Success
Facility=System
SymbolicName=ERROR_TRANSPORT_FULL
Language=English
Cannot use the transport because it is already in use.
.
Language=Japanese
トランスポートは既に使用されているため使用できません。
.

MessageId=4329
Severity=Success
Facility=System
SymbolicName=ERROR_CONTROLLING_IEPORT
Language=English
Unable to open or close the inject/eject port.
.
Language=Japanese
インジェクト/イジェクトポートを開閉できません。
.

MessageId=4330
Severity=Success
Facility=System
SymbolicName=ERROR_UNABLE_TO_EJECT_MOUNTED_MEDIA
Language=English
Unable to eject the media because it is in a drive.
.
Language=Japanese
ドライブ内にあるためメディアをイジェクトできない。
.

MessageId=4331
Severity=Success
Facility=System
SymbolicName=ERROR_CLEANER_SLOT_SET
Language=English
A cleaner slot is already reserved.
.
Language=Japanese
クリーナー スロットが既に予約されています。
.

MessageId=4332
Severity=Success
Facility=System
SymbolicName=ERROR_CLEANER_SLOT_NOT_SET
Language=English
A cleaner slot is not reserved.
.
Language=Japanese
クリーナー スロットが予約されていません。
.

MessageId=4333
Severity=Success
Facility=System
SymbolicName=ERROR_CLEANER_CARTRIDGE_SPENT
Language=English
The cleaner cartridge has performed the maximum number of drive cleanings.
.
Language=Japanese
クリーナー カートリッジが最大数のドライブ クリーニングを実行しました。
.

MessageId=4334
Severity=Success
Facility=System
SymbolicName=ERROR_UNEXPECTED_OMID
Language=English
Unexpected on-medium identifier.
.
Language=Japanese
予期しないオンメディア識別子。
.

MessageId=4335
Severity=Success
Facility=System
SymbolicName=ERROR_CANT_DELETE_LAST_ITEM
Language=English
The last remaining item in this group or resource cannot be deleted.
.
Language=Japanese
このグループまたはリソースの最後に残っているアイテムは削除できません。
.

MessageId=4336
Severity=Success
Facility=System
SymbolicName=ERROR_MESSAGE_EXCEEDS_MAX_SIZE
Language=English
The message provided exceeds the maximum size allowed for this parameter.
.
Language=Japanese
指定されたメッセージは、このパラメータで許可される最大サイズを超えます。
.

MessageId=4337
Severity=Success
Facility=System
SymbolicName=ERROR_VOLUME_CONTAINS_SYS_FILES
Language=English
The volume contains system or paging files.
.
Language=Japanese
ボリュームにシステム ファイルまたはページング ファイルが含まれています。
.

MessageId=4338
Severity=Success
Facility=System
SymbolicName=ERROR_INDIGENOUS_TYPE
Language=English
The media type cannot be removed from this library since at least one drive in the library reports it can support this media type.
.
Language=Japanese
ライブラリ内の少なくとも1つのドライブがこのメディア タイプをサポートできると報告しているため、このメディア タイプをこのライブラリから削除できません。
.

MessageId=4339
Severity=Success
Facility=System
SymbolicName=ERROR_NO_SUPPORTING_DRIVES
Language=English
This offline media cannot be mounted on this system since no enabled drives are present which can be used.
.
Language=Japanese
このオフライン メディアは、使用可能なドライブが存在しないため、このシステムにマウントできません。
.

MessageId=4340
Severity=Success
Facility=System
SymbolicName=ERROR_CLEANER_CARTRIDGE_INSTALLED
Language=English
A cleaner cartridge is present in the tape library.
.
Language=Japanese
テープ ライブラリにクリーナー カートリッジがあります。
.

MessageId=4350
Severity=Success
Facility=System
SymbolicName=ERROR_FILE_OFFLINE
Language=English
The remote storage service was not able to recall the file.
.
Language=Japanese
このファイルは現在このコンピュータでは使用できません。
.

MessageId=4351
Severity=Success
Facility=System
SymbolicName=ERROR_REMOTE_STORAGE_NOT_ACTIVE
Language=English
The remote storage service is not operational at this time.
.
Language=Japanese
リモート ストレージ サービスは現在動作していません。
.

MessageId=4352
Severity=Success
Facility=System
SymbolicName=ERROR_REMOTE_STORAGE_MEDIA_ERROR
Language=English
The remote storage service encountered a media error.
.
Language=Japanese
リモート ストレージ サービスでメディア エラーが発生しました。
.

MessageId=4390
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_A_REPARSE_POINT
Language=English
The file or directory is not a reparse point.
.
Language=Japanese
ファイルまたはディレクトリはリパース ポイントではありません。
.

MessageId=4391
Severity=Success
Facility=System
SymbolicName=ERROR_REPARSE_ATTRIBUTE_CONFLICT
Language=English
The reparse point attribute cannot be set because it conflicts with an existing attribute.
.
Language=Japanese
リパース ポイント属性は既存の属性と競合するため設定できません。
.

MessageId=4392
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_REPARSE_DATA
Language=English
The data present in the reparse point buffer is invalid.
.
Language=Japanese
リパース ポイント バッファに存在するデータが無効です。
.

MessageId=4393
Severity=Success
Facility=System
SymbolicName=ERROR_REPARSE_TAG_INVALID
Language=English
The tag present in the reparse point buffer is invalid.
.
Language=Japanese
リパース ポイントバッファに存在するタグは無効です。
.

MessageId=4394
Severity=Success
Facility=System
SymbolicName=ERROR_REPARSE_TAG_MISMATCH
Language=English
There is a mismatch between the tag specified in the request and the tag present in the reparse point.
.
Language=Japanese
要求で指定されたタグとリパース ポイントに存在するタグの間に不一致があります。
.

MessageId=4500
Severity=Success
Facility=System
SymbolicName=ERROR_VOLUME_NOT_SIS_ENABLED
Language=English
Single Instance Storage is not available on this volume.
.
Language=Japanese
シングル インスタンス ストレージはこのボリュームでは使用できません。
.

MessageId=5001
Severity=Success
Facility=System
SymbolicName=ERROR_DEPENDENT_RESOURCE_EXISTS
Language=English
The cluster resource cannot be moved to another group because other resources are dependent on it.
.
Language=Japanese
他のリソースがそのリソースに依存しているため、クラスタ リソースを他のグループに移動することはできません。
.

MessageId=5002
Severity=Success
Facility=System
SymbolicName=ERROR_DEPENDENCY_NOT_FOUND
Language=English
The cluster resource dependency cannot be found.
.
Language=Japanese
クラスタ リソースの依存関係が見つかりません。
.

MessageId=5003
Severity=Success
Facility=System
SymbolicName=ERROR_DEPENDENCY_ALREADY_EXISTS
Language=English
The cluster resource cannot be made dependent on the specified resource because it is already dependent.
.
Language=Japanese
クラスタ リソースはすでに依存関係にあるため、指定されたリソースに依存させることはできません。
.

MessageId=5004
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_NOT_ONLINE
Language=English
The cluster resource is not online.
.
Language=Japanese
クラスタ リソースがオンラインではありません。
.

MessageId=5005
Severity=Success
Facility=System
SymbolicName=ERROR_HOST_NODE_NOT_AVAILABLE
Language=English
A cluster node is not available for this operation.
.
Language=Japanese
クラスタ ノードがこの操作で使用できません。
.

MessageId=5006
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_NOT_AVAILABLE
Language=English
The cluster resource is not available.
.
Language=Japanese
クラスタ リソースが使用できません。
.

MessageId=5007
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_NOT_FOUND
Language=English
The cluster resource could not be found.
.
Language=Japanese
クラスタ リソースが見つかりませんでした。
.

MessageId=5008
Severity=Success
Facility=System
SymbolicName=ERROR_SHUTDOWN_CLUSTER
Language=English
The cluster is being shut down.
.
Language=Japanese
クラスタがシャットダウンされました。
.

MessageId=5009
Severity=Success
Facility=System
SymbolicName=ERROR_CANT_EVICT_ACTIVE_NODE
Language=English
A cluster node cannot be evicted from the cluster unless the node is down.
.
Language=Japanese
ノードがダウンしているか最後のノードでない限り、クラスタ ノードをクラスタから退去させることはできません。
.

MessageId=5010
Severity=Success
Facility=System
SymbolicName=ERROR_OBJECT_ALREADY_EXISTS
Language=English
The object already exists.
.
Language=Japanese
オブジェクトはすでに存在します。
.

MessageId=5011
Severity=Success
Facility=System
SymbolicName=ERROR_OBJECT_IN_LIST
Language=English
The object is already in the list.
.
Language=Japanese
オブジェクトはすでにリストにあります。
.

MessageId=5012
Severity=Success
Facility=System
SymbolicName=ERROR_GROUP_NOT_AVAILABLE
Language=English
The cluster group is not available for any new requests.
.
Language=Japanese
クラスタ グループは新しい要求に対して使用できません。
.

MessageId=5013
Severity=Success
Facility=System
SymbolicName=ERROR_GROUP_NOT_FOUND
Language=English
The cluster group could not be found.
.
Language=Japanese
クラスタ グループが見つかりません。
.

MessageId=5014
Severity=Success
Facility=System
SymbolicName=ERROR_GROUP_NOT_ONLINE
Language=English
The operation could not be completed because the cluster group is not online.
.
Language=Japanese
クラスタ グループがオンラインではないため、操作を完了できませんでした。
.

MessageId=5015
Severity=Success
Facility=System
SymbolicName=ERROR_HOST_NODE_NOT_RESOURCE_OWNER
Language=English
The cluster node is not the owner of the resource.
.
Language=Japanese
指定されたクラスタ ノードはリソースの所有者ではありません。
.

MessageId=5016
Severity=Success
Facility=System
SymbolicName=ERROR_HOST_NODE_NOT_GROUP_OWNER
Language=English
The cluster node is not the owner of the group.
.
Language=Japanese
指定されたクラスタ ノードはグループの所有者ではありません。
.

MessageId=5017
Severity=Success
Facility=System
SymbolicName=ERROR_RESMON_CREATE_FAILED
Language=English
The cluster resource could not be created in the specified resource monitor.
.
Language=Japanese
指定されたリソース モニタにクラスタ リソースを作成できませんでした。
.

MessageId=5018
Severity=Success
Facility=System
SymbolicName=ERROR_RESMON_ONLINE_FAILED
Language=English
The cluster resource could not be brought online by the resource monitor.
.
Language=Japanese
クラスタ リソースをリソース モニタでオンラインにできませんでした。
.

MessageId=5019
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_ONLINE
Language=English
The operation could not be completed because the cluster resource is online.
.
Language=Japanese
クラスタ リソースがオンラインであるため、操作を完了できませんでした。
.

MessageId=5020
Severity=Success
Facility=System
SymbolicName=ERROR_QUORUM_RESOURCE
Language=English
The cluster resource could not be deleted or brought offline because it is the quorum resource.
.
Language=Japanese
クラスタ リソースがクォーラム リソースであるため、削除またはオフラインにできませんでした。
.

MessageId=5021
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_QUORUM_CAPABLE
Language=English
The cluster could not make the specified resource a quorum resource because it is not capable of being a quorum resource.
.
Language=Japanese
クラスタが指定されたリソースをクォーラム リソースにできませんでした。
.

MessageId=5022
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_SHUTTING_DOWN
Language=English
The cluster software is shutting down.
.
Language=Japanese
クラスタ ソフトウェアをシャットダウンしています。
.

MessageId=5023
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_STATE
Language=English
The group or resource is not in the correct state to perform the requested operation.
.
Language=Japanese
グループまたはリソースが要求された操作を実行するのに適切な状態ではありません。
.

MessageId=5024
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_PROPERTIES_STORED
Language=English
The properties were stored but not all changes will take effect until the next time the resource is brought online.
.
Language=Japanese
プロパティは保存されましたが、リソースが次回オンラインになるまですべての変更が反映されません。
.

MessageId=5025
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_QUORUM_CLASS
Language=English
The cluster could not make the specified resource a quorum resource because it does not belong to a shared storage class.
.
Language=Japanese
指定されたリソースが共有ストレージ クラスに属していないため、クラスタをクォーラム リソースにできませんでした。
.

MessageId=5026
Severity=Success
Facility=System
SymbolicName=ERROR_CORE_RESOURCE
Language=English
The cluster resource could not be deleted since it is a core resource.
.
Language=Japanese
クラスタ リソースはコア リソースであるため削除できませんでした。
.

MessageId=5027
Severity=Success
Facility=System
SymbolicName=ERROR_QUORUM_RESOURCE_ONLINE_FAILED
Language=English
The quorum resource failed to come online.
.
Language=Japanese
クォーラム リソースのオンライン化に失敗しました。
.

MessageId=5028
Severity=Success
Facility=System
SymbolicName=ERROR_QUORUMLOG_OPEN_FAILED
Language=English
The quorum log could not be created or mounted successfully.
.
Language=Japanese
クォーラム ログを正常に作成またはマウントできませんでした。
.

MessageId=5029
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTERLOG_CORRUPT
Language=English
The cluster log is corrupt.
.
Language=Japanese
クラスタ ログが壊れています。
.

MessageId=5030
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTERLOG_RECORD_EXCEEDS_MAXSIZE
Language=English
The record could not be written to the cluster log since it exceeds the maximum size.
.
Language=Japanese
レコードが最大サイズを超えるため、クラスタ ログに書き込めませんでした。
.

MessageId=5031
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTERLOG_EXCEEDS_MAXSIZE
Language=English
The cluster log exceeds its maximum size.
.
Language=Japanese
クラスタ ログが最大サイズを超えました。
.

MessageId=5032
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTERLOG_CHKPOINT_NOT_FOUND
Language=English
No checkpoint record was found in the cluster log.
.
Language=Japanese
クラスタ ログにチェックポイント レコードが見つかりませんでした。
.

MessageId=5033
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTERLOG_NOT_ENOUGH_SPACE
Language=English
The minimum required disk space needed for logging is not available.
.
Language=Japanese
ロギングに必要な最小限のディスク領域がありません。
.

MessageId=5034
Severity=Success
Facility=System
SymbolicName=ERROR_QUORUM_OWNER_ALIVE
Language=English
The cluster node failed to take control of the quorum resource because the resource is owned by another active node.
.
Language=Japanese
クラスタ ノードがクォーラム リソースの制御に失敗しました。
.

MessageId=5035
Severity=Success
Facility=System
SymbolicName=ERROR_NETWORK_NOT_AVAILABLE
Language=English
A cluster network is not available for this operation.
.
Language=Japanese
クラスタ ネットワークがこの操作で使用できません。
.

MessageId=5036
Severity=Success
Facility=System
SymbolicName=ERROR_NODE_NOT_AVAILABLE
Language=English
A cluster node is not available for this operation.
.
Language=Japanese
この操作ではクラスタ ノードを使用できません。
.

MessageId=5037
Severity=Success
Facility=System
SymbolicName=ERROR_ALL_NODES_NOT_AVAILABLE
Language=English
All cluster nodes must be running to perform this operation.
.
Language=Japanese
この操作を実行するには、すべてのクラスタ ノードが実行中である必要があります。
.

MessageId=5038
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_FAILED
Language=English
A cluster resource failed.
.
Language=Japanese
クラスタ リソースが失敗しました。
.

MessageId=5039
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_INVALID_NODE
Language=English
The cluster node is not valid.
.
Language=Japanese
クラスタ ノードが無効です。
.

MessageId=5040
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_EXISTS
Language=English
The cluster node already exists.
.
Language=Japanese
クラスタノードはすでに存在します。
.

MessageId=5041
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_JOIN_IN_PROGRESS
Language=English
A node is in the process of joining the cluster.
.
Language=Japanese
ノードがクラスタに参加中です。
.

MessageId=5042
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_NOT_FOUND
Language=English
The cluster node was not found.
.
Language=Japanese
クラスタ ノードが見つかりませんでした。
.

MessageId=5043
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_LOCAL_NODE_NOT_FOUND
Language=English
The cluster local node information was not found.
.
Language=Japanese
クラスタのローカル ノード情報が見つかりませんでした。
.

MessageId=5044
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NETWORK_EXISTS
Language=English
The cluster network already exists.
.
Language=Japanese
クラスタ ネットワークは既に存在します。
.

MessageId=5045
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NETWORK_NOT_FOUND
Language=English
The cluster network was not found.
.
Language=Japanese
クラスタ ネットワークが見つかりませんでした。
.

MessageId=5046
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NETINTERFACE_EXISTS
Language=English
The cluster network interface already exists.
.
Language=Japanese
クラスタ ネットワーク インタフェースは既に存在します。
.

MessageId=5047
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NETINTERFACE_NOT_FOUND
Language=English
The cluster network interface was not found.
.
Language=Japanese
クラスタ ネットワーク インタフェースが見つかりませんでした。
.

MessageId=5048
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_INVALID_REQUEST
Language=English
The cluster request is not valid for this object.
.
Language=Japanese
クラスタ要求はこのオブジェクトに対して有効ではありません。
.

MessageId=5049
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_INVALID_NETWORK_PROVIDER
Language=English
The cluster network provider is not valid.
.
Language=Japanese
クラスタ ネットワーク プロバイダが無効です。
.

MessageId=5050
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_DOWN
Language=English
The cluster node is down.
.
Language=Japanese
クラスタ ノードがダウンしています。
.

MessageId=5051
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_UNREACHABLE
Language=English
The cluster node is not reachable.
.
Language=Japanese
クラスタ ノードにアクセスできません。
.

MessageId=5052
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_NOT_MEMBER
Language=English
The cluster node is not a member of the cluster.
.
Language=Japanese
クラスタノードはクラスタのメンバではありません。
.

MessageId=5053
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_JOIN_NOT_IN_PROGRESS
Language=English
A cluster join operation is not in progress.
.
Language=Japanese
クラスタ参加操作が進行中ではありません。
.

MessageId=5054
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_INVALID_NETWORK
Language=English
The cluster network is not valid.
.
Language=Japanese
クラスタ ネットワークが無効です。
.

MessageId=5056
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_UP
Language=English
The cluster node is up.
.
Language=Japanese
クラスタ ノードがアップしました。
.

MessageId=5057
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_IPADDR_IN_USE
Language=English
The cluster IP address is already in use.
.
Language=Japanese
クラスタIPアドレスはすでに使用されています。
.

MessageId=5058
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_NOT_PAUSED
Language=English
The cluster node is not paused.
.
Language=Japanese
クラスタノードは一時停止していません。
.

MessageId=5059
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NO_SECURITY_CONTEXT
Language=English
No cluster security context is available.
.
Language=Japanese
クラスタ セキュリティ コンテキストがありません。
.

MessageId=5060
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NETWORK_NOT_INTERNAL
Language=English
The cluster network is not configured for internal cluster communication.
.
Language=Japanese
クラスタ ネットワークがクラスタ内部通信用に構成されていません。
.

MessageId=5061
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_ALREADY_UP
Language=English
The cluster node is already up.
.
Language=Japanese
クラスタ ノードがすでに起動しています。
.

MessageId=5062
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_ALREADY_DOWN
Language=English
The cluster node is already down.
.
Language=Japanese
クラスタ ノードはすでにダウンしています。
.

MessageId=5063
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NETWORK_ALREADY_ONLINE
Language=English
The cluster network is already online.
.
Language=Japanese
クラスタ ネットワークはすでにオンラインです。
.

MessageId=5064
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NETWORK_ALREADY_OFFLINE
Language=English
The cluster network is already offline.
.
Language=Japanese
クラスタ ネットワークはすでにオフラインです。
.

MessageId=5065
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_ALREADY_MEMBER
Language=English
The cluster node is already a member of the cluster.
.
Language=Japanese
クラスタノードはすでにクラスタのメンバです。
.

MessageId=5066
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_LAST_INTERNAL_NETWORK
Language=English
The cluster network is the only one configured for internal cluster communication between two or more active cluster nodes. The internal communication capability cannot be removed from the network.
.
Language=Japanese
クラスタ ネットワークは、2つ以上のアクティブなクラスタ ノード間のクラスタ内部通信用に構成されたものだけです。内部通信機能はネットワークから削除できません。
.

MessageId=5067
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NETWORK_HAS_DEPENDENTS
Language=English
One or more cluster resources depend on the network to provide service to clients. The client access capability cannot be removed from the network.
.
Language=Japanese
1つ以上のクラスタ リソースがクライアントにサービスを提供するためにネットワークに依存しています。クライアント アクセス ケイパビリティをネットワークから削除できません。
.

MessageId=5068
Severity=Success
Facility=System
SymbolicName=ERROR_INVALID_OPERATION_ON_QUORUM
Language=English
This operation cannot be performed on the cluster resource as it the quorum resource. You may not bring the quorum resource offline or modify its possible owners list.
.
Language=Japanese
この操作はクォーラム リソースを含むクラスタ グループでは現在実行できません。
.

MessageId=5069
Severity=Success
Facility=System
SymbolicName=ERROR_DEPENDENCY_NOT_ALLOWED
Language=English
The cluster quorum resource is not allowed to have any dependencies.
.
Language=Japanese
クラスタ クォーラム リソースは依存関係を持つことを許可されていません。
.

MessageId=5070
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_PAUSED
Language=English
The cluster node is paused.
.
Language=Japanese
クラスタ ノードが一時停止しています。
.

MessageId=5071
Severity=Success
Facility=System
SymbolicName=ERROR_NODE_CANT_HOST_RESOURCE
Language=English
The cluster resource cannot be brought online. The owner node cannot run this resource.
.
Language=Japanese
クラスタ リソースをオンラインにできません。
所有ノードはこのリソースを実行できません。
.

MessageId=5072
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_NOT_READY
Language=English
The cluster node is not ready to perform the requested operation.
.
Language=Japanese
クラスタ ノードが要求された操作を実行する準備ができていません。
.

MessageId=5073
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_SHUTTING_DOWN
Language=English
The cluster node is shutting down.
.
Language=Japanese
クラスタ ノードをシャットダウンしています。
.

MessageId=5074
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_JOIN_ABORTED
Language=English
The cluster join operation was aborted.
.
Language=Japanese
クラスタ参加操作が中断されました。
.

MessageId=5075
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_INCOMPATIBLE_VERSIONS
Language=English
The cluster join operation failed due to incompatible software versions between the joining node and its sponsor.
.
Language=Japanese
参加ノードとクラスタ内の他のノードのオペレーティング システムのバージョンに互換性がないため、ノードのクラスタ参加に失敗しました。
.

MessageId=5076
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_MAXNUM_OF_RESOURCES_EXCEEDED
Language=English
This resource cannot be created because the cluster has reached the limit on the number of resources it can monitor.
.
Language=Japanese
クラスタが監視できるリソース数の上限に達したため、このリソースを作成できません。
.

MessageId=5077
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_SYSTEM_CONFIG_CHANGED
Language=English
The system configuration changed during the cluster join or form operation. The join or form operation was aborted.
.
Language=Japanese
クラスタ参加またはフォーム操作中にシステム構成が変更されました。参加またはフォーム操作は中止されました。
.

MessageId=5078
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_RESOURCE_TYPE_NOT_FOUND
Language=English
The specified resource type was not found.
.
Language=Japanese
指定されたリソース タイプが見つかりませんでした。
.

MessageId=5079
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_RESTYPE_NOT_SUPPORTED
Language=English
The specified node does not support a resource of this type. This may be due to version inconsistencies or due to the absence of the resource DLL on this node.
.
Language=Japanese
指定されたノードはこのタイプのリソースをサポートしていません。これは、バージョンの不一致、またはこのノードにリソースDLLがないことが原因です。
.

MessageId=5080
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_RESNAME_NOT_FOUND
Language=English
The specified resource name is supported by this resource DLL. This may be due to a bad (or changed) name supplied to the resource DLL.
.
Language=Japanese
指定されたリソース名は、このリソースDLLではサポートされていません。これは、リソースDLLに提供された名前が間違っている (または変更された) ことが原因である可能性があります。
.

MessageId=5081
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NO_RPC_PACKAGES_REGISTERED
Language=English
No authentication package could be registered with the RPC server.
.
Language=Japanese
認証パッケージを RPC サーバーに登録できませんでした。
.

MessageId=5082
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_OWNER_NOT_IN_PREFLIST
Language=English
You cannot bring the group online because the owner of the group is not in the preferred list for the group. To change the owner node for the group, move the group.
.
Language=Japanese
グループの所有者がグループの優先リストにないため、グループをオンラインにできません。グループの所有者ノードを変更するには、グループを移動してください。
.

MessageId=5083
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_DATABASE_SEQMISMATCH
Language=English
The join operation failed because the cluster database sequence number has changed or is incompatible with the locker node. This may happen during a join operation if the cluster database was changing during the join.
.
Language=Japanese
クラスタ データベースのシーケンス番号が変更されたか、ロッカー ノードと互換性がないため、参加操作に失敗しました。これは、参加中にクラスタ データベースが変更された場合に、参加操作中に発生する可能性があります。
.

MessageId=5084
Severity=Success
Facility=System
SymbolicName=ERROR_RESMON_INVALID_STATE
Language=English
The resource monitor will not allow the fail operation to be performed while the resource is in its current state. This may happen if the resource is in a pending state.
.
Language=Japanese
リソースが現在の状態の間、リソース モニタはフェイル操作の実行を許可しません。リソースがペンディング状態の場合に発生する可能性があります。
.

MessageId=5085
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_GUM_NOT_LOCKER
Language=English
A non locker code got a request to reserve the lock for making global updates.
.
Language=Japanese
非ロッカー コードがグローバル更新のためのロック予約要求を受け取りました。
.

MessageId=5086
Severity=Success
Facility=System
SymbolicName=ERROR_QUORUM_DISK_NOT_FOUND
Language=English
The quorum disk could not be located by the cluster service.
.
Language=Japanese
クラスタサービスによってクォーラムディスクを検出できませんでした。
.

MessageId=5087
Severity=Success
Facility=System
SymbolicName=ERROR_DATABASE_BACKUP_CORRUPT
Language=English
The backup up cluster database is possibly corrupt.
.
Language=Japanese
バックアップされたクラスタ データベースが破損している可能性があります。
.

MessageId=5088
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NODE_ALREADY_HAS_DFS_ROOT
Language=English
A DFS root already exists in this cluster node.
.
Language=Japanese
DFSルートがこのクラスタノードにすでに存在します。
.

MessageId=5089
Severity=Success
Facility=System
SymbolicName=ERROR_RESOURCE_PROPERTY_UNCHANGEABLE
Language=English
An attempt to modify a resource property failed because it conflicts with another existing property.
.
Language=Japanese
リソース プロパティを変更しようとしましたが、既存のプロパティと競合したため変更できませんでした。
.

MessageId=5890
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_MEMBERSHIP_INVALID_STATE
Language=English
An operation was attempted that is incompatible with the current membership state of the node.
.
Language=Japanese
ノードの現在のメンバシップ状態と互換性のない操作を試みました。
.

MessageId=5891
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_QUORUMLOG_NOT_FOUND
Language=English
The quorum resource does not contain the quorum log.
.
Language=Japanese
クォーラムリソースにクォーラム ログが含まれていません。
.

MessageId=5892
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_MEMBERSHIP_HALT
Language=English
The membership engine requested shutdown of the cluster service on this node.
.
Language=Japanese
メンバーシップ エンジンがこのノードのクラスタ サービスのシャットダウンを要求しました。
.

MessageId=5893
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_INSTANCE_ID_MISMATCH
Language=English
The join operation failed because the cluster instance ID of the joining node does not match the cluster instance ID of the sponsor node.
.
Language=Japanese
参加ノードのクラスタ インスタンスIDがスポンサー ノードのクラスタ インスタンスIDと一致しないため、参加操作は失敗しました。
.

MessageId=5894
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NETWORK_NOT_FOUND_FOR_IP
Language=English
A matching network for the specified IP address could not be found. Please also specify a subnet mask and a cluster network.
.
Language=Japanese
指定されたIPアドレスに一致するクラスタ ネットワークが見つかりませんでした。
.

MessageId=5895
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_PROPERTY_DATA_TYPE_MISMATCH
Language=English
The actual data type of the property did not match the expected data type of the property.
.
Language=Japanese
プロパティの実際のデータ型がプロパティの予想されるデータ型と一致しませんでした。
.

MessageId=5896
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_EVICT_WITHOUT_CLEANUP
Language=English
The cluster node was evicted from the cluster successfully, but the node was not cleaned up. Extended status information explaining why the node was not cleaned up is available.
.
Language=Japanese
クラスタ ノードは正常にクラスタから削除されましたが、ノードはクリーンアップされませんでした。どのクリーンアップ手順が失敗し、どのように回復するかを確認するには、イベントビューアを使用してFailover Clusteringアプリケーションのイベントログを参照してください。
.

MessageId=5897
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_PARAMETER_MISMATCH
Language=English
Two or more parameter values specified for a resource's properties are in conflict.
.
Language=Japanese
リソースのプロパティに指定された2つ以上のパラメータ値が競合しています。
.

MessageId=5898
Severity=Success
Facility=System
SymbolicName=ERROR_NODE_CANNOT_BE_CLUSTERED
Language=English
This computer cannot be made a member of a cluster.
.
Language=Japanese
このコンピュータをクラスタのメンバにできません。
.

MessageId=5899
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_WRONG_OS_VERSION
Language=English
This computer cannot be made a member of a cluster because it does not have the correct version of Windows installed.
.
Language=Japanese
このコンピュータには正しいバージョンのReliefWinがインストールされていないため、クラスタのメンバにできません。
.

MessageId=5900
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_CANT_CREATE_DUP_CLUSTER_NAME
Language=English
A cluster cannot be created with the specified cluster name because that cluster name is already in use. Specify a different name for the cluster.
.
Language=Japanese
指定したクラスタ名がすでに使用されているため、指定したクラスタ名でクラスタを作成できません。別のクラスタ名を指定してください。
.

MessageId=5901
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSCFG_ALREADY_COMMITTED
Language=English
The cluster configuration action has already been committed.
.
Language=Japanese
クラスタ構成アクションはすでにコミットされています。
.

MessageId=5902
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSCFG_ROLLBACK_FAILED
Language=English
The cluster configuration action could not be rolled back.
.
Language=Japanese
クラスタ構成アクションをロールバックできませんでした。
.

MessageId=5903
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSCFG_SYSTEM_DISK_DRIVE_LETTER_CONFLICT
Language=English
The drive letter assigned to a system disk on one node conflicted with the driver letter assigned to a disk on another node.
.
Language=Japanese
あるノードのシステム ディスクに割り当てられたドライブ文字と、別のノードのディスクに割り当てられたドライブ文字が競合しました。
.

MessageId=5904
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_OLD_VERSION
Language=English
One or more nodes in the cluster are running a version of Windows that does not support this operation.
.
Language=Japanese
クラスタ内の1つまたは複数のノードで、この操作をサポートしないバージョンのReliefWinが実行されています。
.

MessageId=5905
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_MISMATCHED_COMPUTER_ACCT_NAME
Language=English
The name of the corresponding computer account doesn't match the Network Name for this resource.
.
Language=Japanese
対応するコンピュータのアカウント名がこのリソースのネットワーク名に一致しません。
.

MessageId=5906
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_NO_NET_ADAPTERS
Language=English
No network adapters are available.
.
Language=Japanese
使用可能なネットワーク アダプタがありません。
.

MessageId=5907
Severity=Success
Facility=System
SymbolicName=ERROR_CLUSTER_POISONED
Language=English
The cluster node has been poisoned.
.
Language=Japanese
クラスタ ノードがポイズンされました。
.

MessageId=6000
Severity=Success
Facility=System
SymbolicName=ERROR_ENCRYPTION_FAILED
Language=English
The specified file could not be encrypted.
.
Language=Japanese
指定されたファイルを暗号化できませんでした。
.

MessageId=6001
Severity=Success
Facility=System
SymbolicName=ERROR_DECRYPTION_FAILED
Language=English
The specified file could not be decrypted.
.
Language=Japanese
指定されたファイルは復号化できませんでした。
.

MessageId=6002
Severity=Success
Facility=System
SymbolicName=ERROR_FILE_ENCRYPTED
Language=English
The specified file is encrypted and the user does not have the ability to decrypt it.
.
Language=Japanese
指定されたファイルは暗号化されており、ユーザーには復号化する権限がありません。
.

MessageId=6003
Severity=Success
Facility=System
SymbolicName=ERROR_NO_RECOVERY_POLICY
Language=English
There is no valid encryption recovery policy configured for this system.
.
Language=Japanese
このシステムに有効な暗号化回復ポリシーが構成されていません。
.

MessageId=6004
Severity=Success
Facility=System
SymbolicName=ERROR_NO_EFS
Language=English
The required encryption driver is not loaded for this system.
.
Language=Japanese
必要な暗号化ドライバがこのシステムにロードされていません。
.

MessageId=6005
Severity=Success
Facility=System
SymbolicName=ERROR_WRONG_EFS
Language=English
The file was encrypted with a different encryption driver than is currently loaded.
.
Language=Japanese
ファイルは現在ロードされている暗号化ドライバとは異なる暗号化ドライバで暗号化されました。
.

MessageId=6006
Severity=Success
Facility=System
SymbolicName=ERROR_NO_USER_KEYS
Language=English
There are no EFS keys defined for the user.
.
Language=Japanese
ユーザーに定義されたEFSキーがありません。
.

MessageId=6007
Severity=Success
Facility=System
SymbolicName=ERROR_FILE_NOT_ENCRYPTED
Language=English
The specified file is not encrypted.
.
Language=Japanese
指定されたファイルは暗号化されていません。
.

MessageId=6008
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_EXPORT_FORMAT
Language=English
The specified file is not in the defined EFS export format.
.
Language=Japanese
指定されたファイルは定義されたEFSエクスポート形式ではありません。
.

MessageId=6009
Severity=Success
Facility=System
SymbolicName=ERROR_FILE_READ_ONLY
Language=English
The specified file is read-only.
.
Language=Japanese
指定されたファイルは読み取り専用です。
.

MessageId=6010
Severity=Success
Facility=System
SymbolicName=ERROR_DIR_EFS_DISALLOWED
Language=English
The directory has been disabled for encryption.
.
Language=Japanese
ディレクトリは暗号化が無効になっています。
.

MessageId=6011
Severity=Success
Facility=System
SymbolicName=ERROR_EFS_SERVER_NOT_TRUSTED
Language=English
The server is not trusted for remote encryption operation.
.
Language=Japanese
サーバーはリモート暗号化操作に対して信頼されていません。
.

MessageId=6012
Severity=Success
Facility=System
SymbolicName=ERROR_BAD_RECOVERY_POLICY
Language=English
Recovery policy configured for this system contains invalid recovery certificate.
.
Language=Japanese
このシステムに設定されている回復ポリシーに無効な回復証明書が含まれています。
.

MessageId=6013
Severity=Success
Facility=System
SymbolicName=ERROR_EFS_ALG_BLOB_TOO_BIG
Language=English
The encryption algorithm used on the source file needs a bigger key buffer than the one on the destination file.
.
Language=Japanese
ソース ファイルで使用される暗号化アルゴリズムには、宛先ファイルで使用されるものより大きなキー バッファが必要です。
.

MessageId=6014
Severity=Success
Facility=System
SymbolicName=ERROR_VOLUME_NOT_SUPPORT_EFS
Language=English
The disk partition does not support file encryption.
.
Language=Japanese
ディスク パーティションはファイル暗号化をサポートしていません。
.

MessageId=6015
Severity=Success
Facility=System
SymbolicName=ERROR_EFS_DISABLED
Language=English
This machine is disabled for file encryption.
.
Language=Japanese
このマシンではファイル暗号化は無効です。
.

MessageId=6016
Severity=Success
Facility=System
SymbolicName=ERROR_EFS_VERSION_NOT_SUPPORT
Language=English
A newer system is required to decrypt this encrypted file.
.
Language=Japanese
この暗号化されたファイルを復号化するには、新しいシステムが必要です。
.

MessageId=6118
Severity=Success
Facility=System
SymbolicName=ERROR_NO_BROWSER_SERVERS_FOUND
Language=English
The list of servers for this workgroup is not currently available.
.
Language=Japanese
このワークグループのサーバーのリストは現在利用できません。
.

MessageId=6200
Severity=Success
Facility=System
SymbolicName=SCHED_E_SERVICE_NOT_LOCALSYSTEM
Language=English
The Task Scheduler service must be configured to run in the System account to function properly. Individual tasks may be configured to run in other accounts.
.
Language=Japanese
タスクスケジューラサービスを正しく機能させるには、システムアカウントで実行するように構成する必要があります。個々のタスクは他のアカウントで実行するように構成できます。
.

MessageId=7001
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_WINSTATION_NAME_INVALID
Language=English
The specified session name is invalid.
.
Language=Japanese
指定されたセッション名が無効です。
.

MessageId=7002
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_INVALID_PD
Language=English
The specified protocol driver is invalid.
.
Language=Japanese
指定されたプロトコルドライバーが無効です。
.

MessageId=7003
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_PD_NOT_FOUND
Language=English
The specified protocol driver was not found in the system path.
.
Language=Japanese
指定されたプロトコルドライバーがシステムパスに見つかりません。
.

MessageId=7004
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_WD_NOT_FOUND
Language=English
The specified terminal connection driver was not found in the system path.
.
Language=Japanese
指定されたターミナル接続ドライバがシステムパスで見つかりません。
.

MessageId=7005
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_CANNOT_MAKE_EVENTLOG_ENTRY
Language=English
A registry key for event logging could not be created for this session.
.
Language=Japanese
イベントログ用のレジストリキーがこのセッションに作成できませんでした。
.

MessageId=7006
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_SERVICE_NAME_COLLISION
Language=English
A service with the same name already exists on the system.
.
Language=Japanese
同じ名前のサービスがすでにシステムに存在します。
.

MessageId=7007
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_CLOSE_PENDING
Language=English
A close operation is pending on the session.
.
Language=Japanese
セッションでクローズ操作が保留されています。
.

MessageId=7008
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_NO_OUTBUF
Language=English
There are no free output buffers available.
.
Language=Japanese
空き出力バッファがありません。
.

MessageId=7009
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_MODEM_INF_NOT_FOUND
Language=English
The MODEM.INF file was not found.
.
Language=Japanese
MODEM.INFファイルが見つかりませんでした。
.

MessageId=7010
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_INVALID_MODEMNAME
Language=English
The modem name was not found in MODEM.INF.
.
Language=Japanese
モデム名が MODEM.INF に見つかりませんでした。
.

MessageId=7011
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_MODEM_RESPONSE_ERROR
Language=English
The modem did not accept the command sent to it. Verify that the configured modem name matches the attached modem.
.
Language=Japanese
モデムが送信されたコマンドを受け付けませんでした。設定されているモデム名が接続されているモデムと一致しているか確認してください。
.

MessageId=7012
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_MODEM_RESPONSE_TIMEOUT
Language=English
The modem did not respond to the command sent to it. Verify that the modem is properly cabled and powered on.
.
Language=Japanese
モデムが送信されたコマンドに応答しませんでした。モデムが正しくケーブル接続され、電源が入っていることを確認してください。
.

MessageId=7013
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_MODEM_RESPONSE_NO_CARRIER
Language=English
Carrier detect has failed or carrier has been dropped due to disconnect.
.
Language=Japanese
キャリア検出に失敗したか、キャリアが切断されました。
.

MessageId=7014
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_MODEM_RESPONSE_NO_DIALTONE
Language=English
Dial tone not detected within the required time. Verify that the phone cable is properly attached and functional.
.
Language=Japanese
必要な時間内にダイヤルトーンが検出されませんでした。電話ケーブルが正しく接続され、機能していることを確認してください。
.

MessageId=7015
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_MODEM_RESPONSE_BUSY
Language=English
Busy signal detected at remote site on callback.
.
Language=Japanese
コールバック時にリモート サイトでビジー信号が検出されました。
.

MessageId=7016
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_MODEM_RESPONSE_VOICE
Language=English
Voice detected at remote site on callback.
.
Language=Japanese
コールバック時にリモート サイトで音声を検出しました。
.

MessageId=7017
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_TD_ERROR
Language=English
Transport driver error
.
Language=Japanese
トランスポート ドライバ エラーです。
.

MessageId=7022
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_WINSTATION_NOT_FOUND
Language=English
The specified session cannot be found.
.
Language=Japanese
指定されたセッションが見つかりません。
.

MessageId=7023
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_WINSTATION_ALREADY_EXISTS
Language=English
The specified session name is already in use.
.
Language=Japanese
指定されたセッション名はすでに使用されています。
.

MessageId=7024
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_WINSTATION_BUSY
Language=English
The requested operation cannot be completed because the terminal connection is currently busy processing a connect, disconnect, reset, or delete operation.
.
Language=Japanese
Remote Desktop Servicesがビジー状態のため、実行しようとしているタスクを完了できません。数分後に再試行してください。他のユーザーはログオンできるはずです。
.

MessageId=7025
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_BAD_VIDEO_MODE
Language=English
An attempt has been made to connect to a session whose video mode is not supported by the current client.
.
Language=Japanese
ビデオモードが現在のクライアントでサポートされていないセッションに接続しようとしました。
.

MessageId=7035
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_GRAPHICS_INVALID
Language=English
The application attempted to enable DOS graphics mode. DOS graphics mode is not supported.
.
Language=Japanese
アプリケーションが DOS グラフィック モードを有効にしようとしました。DOS グラフィック モードはサポートされていません。
.

MessageId=7037
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_LOGON_DISABLED
Language=English
Your interactive logon privilege has been disabled. Please contact your administrator.
.
Language=Japanese
対話ログオン権限が無効になりました。管理者に連絡してください。
.

MessageId=7038
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_NOT_CONSOLE
Language=English
The requested operation can be performed only on the system console. This is most often the result of a driver or system DLL requiring direct console access.
.
Language=Japanese
要求された操作はシステム コンソールでのみ実行できます。これは多くの場合、コンソールへの直接アクセスを必要とするドライバまたはシステムDLLが原因です。
.

MessageId=7040
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_CLIENT_QUERY_TIMEOUT
Language=English
The client failed to respond to the server connect message.
.
Language=Japanese
クライアントはサーバーの接続メッセージに応答できませんでした。
.

MessageId=7041
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_CONSOLE_DISCONNECT
Language=English
Disconnecting the console session is not supported.
.
Language=Japanese
コンソールセッションの切断はサポートされていません。
.

MessageId=7042
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_CONSOLE_CONNECT
Language=English
Reconnecting a disconnected session to the console is not supported.
.
Language=Japanese
切断されたセッションをコンソールに再接続することはサポートされていません。
.

MessageId=7044
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_SHADOW_DENIED
Language=English
The request to control another session remotely was denied.
.
Language=Japanese
別のセッションをリモートで制御する要求が拒否されました。
.

MessageId=7045
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_WINSTATION_ACCESS_DENIED
Language=English
The requested session access is denied.
.
Language=Japanese
要求されたセッションへのアクセスが拒否されました。
.

MessageId=7049
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_INVALID_WD
Language=English
The specified terminal connection driver is invalid.
.
Language=Japanese
指定されたターミナル接続ドライバが無効です。
.

MessageId=7050
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_SHADOW_INVALID
Language=English
The requested session cannot be controlled remotely. This may be because the session is disconnected or does not currently have a user logged on.
.
Language=Japanese
要求されたセッションをリモート制御できません。
セッションが切断されているか、現在ログオンしているユーザーがいない可能性があります。
.

MessageId=7051
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_SHADOW_DISABLED
Language=English
The requested session is not configured to allow remote control.
.
Language=Japanese
要求されたセッションはリモート コントロールを許可するように構成されていません。
.

MessageId=7052
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_CLIENT_LICENSE_IN_USE
Language=English
Your request to connect to this Terminal Server has been rejected. Your Terminal Server client license number is currently being used by another user. Please call your system administrator to obtain a unique license number.
.
Language=Japanese
このターミナル サーバーへの接続要求は拒否されました。ターミナル サーバーのクライアントライセンス番号は、現在別のユーザーによって使用されています。システム管理者に連絡して、固有のライセンス番号を取得してください。
.

MessageId=7053
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_CLIENT_LICENSE_NOT_SET
Language=English
Your request to connect to this Terminal Server has been rejected. Your Terminal Server client license number has not been entered for this copy of the Terminal Server client. Please contact your system administrator.
.
Language=Japanese
このターミナル サーバーへの接続要求は拒否されました。ターミナル サーバー クライアントのライセンス番号が、ターミナル サーバー クライアントのこのコピーに入力されていません。
システム管理者に連絡してください。
.

MessageId=7054
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_LICENSE_NOT_AVAILABLE
Language=English
The system has reached its licensed logon limit. Please try again later.
.
Language=Japanese
このコンピューターへの接続数には制限があり、現在すべての接続が使用中です。後で接続するか、システム管理者に連絡してください。
.

MessageId=7055
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_LICENSE_CLIENT_INVALID
Language=English
The client you are using is not licensed to use this system. Your logon request is denied.
.
Language=Japanese
使用しているクライアントは、このシステムを使用するライセンスがありません。ログオン要求は拒否されました。
.

MessageId=7056
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_LICENSE_EXPIRED
Language=English
The system license has expired. Your logon request is denied.
.
Language=Japanese
システムのライセンスの有効期限が切れています。ログオン要求は拒否されました。
.

MessageId=7057
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_SHADOW_NOT_RUNNING
Language=English
Remote control could not be terminated because the specified session is not currently being remotely controlled.
.
Language=Japanese
指定されたセッションは現在リモート制御されていないため、リモート制御を終了できませんでした。
.

MessageId=7058
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_SHADOW_ENDED_BY_MODE_CHANGE
Language=English
The remote control of the console was terminated because the display mode was changed. Changing the display mode in a remote control session is not supported.
.
Language=Japanese
コンソールの表示モードが変更されたため、リモート コントロールを終了しました。リモート コントロールセッションでの表示モードの変更はサポートされていません。
.

MessageId=7059
Severity=Success
Facility=System
SymbolicName=ERROR_ACTIVATION_COUNT_EXCEEDED
Language=English
Activation has already been reset the maximum number of times for this installation. Your activation timer will not be cleared.
.
Language=Japanese
アクティベーションが、このインストールの最大回数までリセットされています。アクティベーション タイマーはクリアされません。
.

MessageId=7060
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_WINSTATIONS_DISABLED
Language=English
Remote logins are currently disabled.
.
Language=Japanese
リモートログインは現在無効になっています。
.

MessageId=7061
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_ENCRYPTION_LEVEL_REQUIRED
Language=English
You do not have the proper encryption level to access this Session.
.
Language=Japanese
このセッションにアクセスするための適切な暗号化レベルがありません。
.

MessageId=7062
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_SESSION_IN_USE
Language=English
The user %s\\%s is currently logged on to this computer. Only the current user or an administrator can log on to this computer.
.
Language=Japanese
現在、ユーザ %s\\\\%s がこのコンピュータにログオンしています。現在のユーザまたは管理者のみがこのコンピュータにログオンできます。
.

MessageId=7063
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_NO_FORCE_LOGOFF
Language=English
The user %s\\%s is already logged on to the console of this computer. You do not have permission to log in at this time. To resolve this issue, contact %s\\%s and have them log off.
.
Language=Japanese
ユーザ %s\\\\%s は、このコンピュータのコンソールに既にログオンしています。現時点ではログインする権限がありません。この問題を解決するには、%sに連絡してログオフさせてください。
.

MessageId=7064
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_ACCOUNT_RESTRICTION
Language=English
Unable to log you on because of an account restriction.
.
Language=Japanese
アカウント制限のためログオンできません。
.

MessageId=7065
Severity=Success
Facility=System
SymbolicName=ERROR_RDP_PROTOCOL_ERROR
Language=English
The RDP protocol component %2 detected an error in the protocol stream and has disconnected the client.
.
Language=Japanese
RDP プロトコル コンポーネント %2 がプロトコル ストリームでエラーを検出したので、クライアントを切断しました。
.

MessageId=7066
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_CDM_CONNECT
Language=English
The Client Drive Mapping Service Has Connected on Terminal Connection.
.
Language=Japanese
クライアント ドライブ マッピング サービスがターミナル接続に接続されました。
.

MessageId=7067
Severity=Success
Facility=System
SymbolicName=ERROR_CTX_CDM_DISCONNECT
Language=English
The Client Drive Mapping Service Has Disconnected on Terminal Connection.
.
Language=Japanese
クライアント ドライブ マッピング サービスがターミナル接続で切断されました。
.

MessageId=8001
Severity=Success
Facility=System
SymbolicName=FRS_ERR_INVALID_API_SEQUENCE
Language=English
The file replication service API was called incorrectly.
.
Language=Japanese
ファイル レプリケーション サービス API の呼び出しに誤りがありました。
.

MessageId=8002
Severity=Success
Facility=System
SymbolicName=FRS_ERR_STARTING_SERVICE
Language=English
The file replication service cannot be started.
.
Language=Japanese
ファイル レプリケーション サービスを開始できません。
.

MessageId=8003
Severity=Success
Facility=System
SymbolicName=FRS_ERR_STOPPING_SERVICE
Language=English
The file replication service cannot be stopped.
.
Language=Japanese
ファイル レプリケーション サービスを停止できません。
.

MessageId=8004
Severity=Success
Facility=System
SymbolicName=FRS_ERR_INTERNAL_API
Language=English
The file replication service API terminated the request. The event log may have more information.
.
Language=Japanese
ファイル レプリケーション サービス API が要求を終了しました。イベントログに詳細情報が記載されている場合があります。
.

MessageId=8005
Severity=Success
Facility=System
SymbolicName=FRS_ERR_INTERNAL
Language=English
The file replication service terminated the request. The event log may have more information.
.
Language=Japanese
ファイル レプリケーション サービスが要求を終了しました。イベント ログに詳細情報が記載されている場合があります。
.

MessageId=8006
Severity=Success
Facility=System
SymbolicName=FRS_ERR_SERVICE_COMM
Language=English
The file replication service cannot be contacted. The event log may have more information.
.
Language=Japanese
ファイル レプリケーション サービスにアクセスできません。イベントログに詳細が記録されている可能性があります。
.

MessageId=8007
Severity=Success
Facility=System
SymbolicName=FRS_ERR_INSUFFICIENT_PRIV
Language=English
The file replication service cannot satisfy the request because the user has insufficient privileges. The event log may have more information.
.
Language=Japanese
ユーザーに十分な権限がないため、ファイル レプリケーション サービスは要求を満たすことができません。イベント ログに詳細情報が記載されている場合があります。
.

MessageId=8008
Severity=Success
Facility=System
SymbolicName=FRS_ERR_AUTHENTICATION
Language=English
The file replication service cannot satisfy the request because authenticated RPC is not available. The event log may have more information.
.
Language=Japanese
認証されたRPCが使用できないため、ファイル レプリケーション サービスは要求を満たすことができません。イベント ログに詳細情報が記載されている場合があります。
.

MessageId=8009
Severity=Success
Facility=System
SymbolicName=FRS_ERR_PARENT_INSUFFICIENT_PRIV
Language=English
The file replication service cannot satisfy the request because the user has insufficient privileges on the domain controller. The event log may have more information.
.
Language=Japanese
ユーザーにドメイン コントローラー上で十分な権限がないため、ファイル レプリケーション サービスは要求を満たすことができません。イベントログに詳細情報が記載されている場合があります。
.

MessageId=8010
Severity=Success
Facility=System
SymbolicName=FRS_ERR_PARENT_AUTHENTICATION
Language=English
The file replication service cannot satisfy the request because authenticated RPC is not available on the domain controller. The event log may have more information.
.
Language=Japanese
認証されたRPCがドメイン コントローラーで使用できないため、ファイルレプリケーションサービスは要求を満たすことができません。イベント ログに詳細情報が記載されている場合があります。
.

MessageId=8011
Severity=Success
Facility=System
SymbolicName=FRS_ERR_CHILD_TO_PARENT_COMM
Language=English
The file replication service cannot communicate with the file replication service on the domain controller. The event log may have more information.
.
Language=Japanese
ファイルレプリケーションサービスは、ドメインコントローラのファイルレプリケーションサービスと通信できません。イベントログに詳細情報が記載されている可能性があります。
.

MessageId=8012
Severity=Success
Facility=System
SymbolicName=FRS_ERR_PARENT_TO_CHILD_COMM
Language=English
The file replication service on the domain controller cannot communicate with the file replication service on this computer. The event log may have more information.
.
Language=Japanese
ドメインコントローラのファイルレプリケーションサービスは、このコンピュータのファイルレプリケーションサービスと通信できません。イベントログに詳細が記載されている場合があります。
.

MessageId=8013
Severity=Success
Facility=System
SymbolicName=FRS_ERR_SYSVOL_POPULATE
Language=English
The file replication service cannot populate the system volume because of an internal error. The event log may have more information.
.
Language=Japanese
内部エラーが発生したため、ファイルレプリケーションサービスがシステムボリュームを作成できません。イベントログに詳細情報が記載されている場合があります。
.

MessageId=8014
Severity=Success
Facility=System
SymbolicName=FRS_ERR_SYSVOL_POPULATE_TIMEOUT
Language=English
The file replication service cannot populate the system volume because of an internal timeout. The event log may have more information.
.
Language=Japanese
ファイル レプリケーション サービスがシステム ボリュームに入力できません。
.

MessageId=8015
Severity=Success
Facility=System
SymbolicName=FRS_ERR_SYSVOL_IS_BUSY
Language=English
The file replication service cannot process the request. The system volume is busy with a previous request.
.
Language=Japanese
ファイル レプリケーション サービスは要求を処理できません。システム ボリュームは前の要求でビジー状態です。
.

MessageId=8016
Severity=Success
Facility=System
SymbolicName=FRS_ERR_SYSVOL_DEMOTE
Language=English
The file replication service cannot stop replicating the system volume because of an internal error. The event log may have more information.
.
Language=Japanese
内部エラーが発生したため、ファイル レプリケーション サービスはシステム ボリュームのレプリケーションを停止できません。イベント ログに詳細情報が記載されている場合があります。
.

MessageId=8017
Severity=Success
Facility=System
SymbolicName=FRS_ERR_INVALID_SERVICE_PARAMETER
Language=English
The file replication service detected an invalid parameter.
.
Language=Japanese
ファイル レプリケーション サービスが無効なパラメータを検出しました。
.

MessageId=8200
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NOT_INSTALLED
Language=English
An error occurred while installing the directory service. For more information, see the event log.
.
Language=Japanese
ディレクトリサービスのインストール中にエラーが発生しました。詳細については、イベントログを参照してください。
.

MessageId=8201
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MEMBERSHIP_EVALUATED_LOCALLY
Language=English
The directory service evaluated group memberships locally.
.
Language=Japanese
ディレクトリ サービスがローカルでグループ メンバーシップを評価しました。
.

MessageId=8202
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_ATTRIBUTE_OR_VALUE
Language=English
The specified directory service attribute or value does not exist.
.
Language=Japanese
指定されたディレクトリ サービス属性または値が存在しません。
.

MessageId=8203
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INVALID_ATTRIBUTE_SYNTAX
Language=English
The attribute syntax specified to the directory service is invalid.
.
Language=Japanese
ディレクトリ サービスに指定された属性構文が無効です。
.

MessageId=8204
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ATTRIBUTE_TYPE_UNDEFINED
Language=English
The attribute type specified to the directory service is not defined.
.
Language=Japanese
ディレクトリ サービスに指定された属性タイプが定義されていません。
.

MessageId=8205
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ATTRIBUTE_OR_VALUE_EXISTS
Language=English
The specified directory service attribute or value already exists.
.
Language=Japanese
指定されたディレクトリ サービスの属性または値は既に存在します。
.

MessageId=8206
Severity=Success
Facility=System
SymbolicName=ERROR_DS_BUSY
Language=English
The directory service is busy.
.
Language=Japanese
ディレクトリ サービスがビジーです。
.

MessageId=8207
Severity=Success
Facility=System
SymbolicName=ERROR_DS_UNAVAILABLE
Language=English
The directory service is unavailable.
.
Language=Japanese
ディレクトリサービスは利用できません。
.

MessageId=8208
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_RIDS_ALLOCATED
Language=English
The directory service was unable to allocate a relative identifier.
.
Language=Japanese
ディレクトリ サービスが相対識別子を割り当てられませんでした。
.

MessageId=8209
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_MORE_RIDS
Language=English
The directory service has exhausted the pool of relative identifiers.
.
Language=Japanese
ディレクトリ サービスは相対識別子のプールを使い果たしました。
.

MessageId=8210
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INCORRECT_ROLE_OWNER
Language=English
The requested operation could not be performed because the directory service is not the master for that type of operation.
.
Language=Japanese
ディレクトリ サービスがそのタイプの操作のマスターではないため、要求された操作を実行できませんでした。
.

MessageId=8211
Severity=Success
Facility=System
SymbolicName=ERROR_DS_RIDMGR_INIT_ERROR
Language=English
The directory service was unable to initialize the subsystem that allocates relative identifiers.
.
Language=Japanese
ディレクトリ サービスは相対識別子を割り当てるサブシステムを初期化できませんでした。
.

MessageId=8212
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OBJ_CLASS_VIOLATION
Language=English
The requested operation did not satisfy one or more constraints associated with the class of the object.
.
Language=Japanese
要求された操作は、オブジェクトのクラスに関連付けられた1つ以上の制約を満たしていません。
.

MessageId=8213
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_ON_NON_LEAF
Language=English
The directory service can perform the requested operation only on a leaf object.
.
Language=Japanese
ディレクトリ サービスは要求された操作をリーフ オブジェクトに対してのみ実行できます。
.

MessageId=8214
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_ON_RDN
Language=English
The directory service cannot perform the requested operation on the RDN attribute of an object.
.
Language=Japanese
ディレクトリ サービスはオブジェクトのRDN属性に対して要求された操作を実行できません。
.

MessageId=8215
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_MOD_OBJ_CLASS
Language=English
The directory service detected an attempt to modify the object class of an object.
.
Language=Japanese
ディレクトリ サービスがオブジェクトのオブジェクト クラスを変更しようとする試みを検出しました。
.

MessageId=8216
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CROSS_DOM_MOVE_ERROR
Language=English
The requested cross-domain move operation could not be performed.
.
Language=Japanese
要求されたクロスドメイン移動操作を実行できませんでした。
.

MessageId=8217
Severity=Success
Facility=System
SymbolicName=ERROR_DS_GC_NOT_AVAILABLE
Language=English
Unable to contact the global catalog server.
.
Language=Japanese
グローバルカタログサーバーに接続できません。
.

MessageId=8218
Severity=Success
Facility=System
SymbolicName=ERROR_SHARED_POLICY
Language=English
The policy object is shared and can only be modified at the root.
.
Language=Japanese
ポリシー オブジェクトは共有され、ルートでのみ変更できます。
.

MessageId=8219
Severity=Success
Facility=System
SymbolicName=ERROR_POLICY_OBJECT_NOT_FOUND
Language=English
The policy object does not exist.
.
Language=Japanese
ポリシー オブジェクトが存在しません。
.

MessageId=8220
Severity=Success
Facility=System
SymbolicName=ERROR_POLICY_ONLY_IN_DS
Language=English
The requested policy information is only in the directory service.
.
Language=Japanese
要求されたポリシー情報はディレクトリ サービスにしかありません。
.

MessageId=8221
Severity=Success
Facility=System
SymbolicName=ERROR_PROMOTION_ACTIVE
Language=English
A domain controller promotion is currently active.
.
Language=Japanese
ドメインコントローラのプロモーションが現在アクティブです。
.

MessageId=8222
Severity=Success
Facility=System
SymbolicName=ERROR_NO_PROMOTION_ACTIVE
Language=English
A domain controller promotion is not currently active
.
Language=Japanese
ドメイン コントローラーのプロモーションは現在アクティブではありません。
.

MessageId=8224
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OPERATIONS_ERROR
Language=English
An operations error occurred.
.
Language=Japanese
操作エラーが発生しました。
.

MessageId=8225
Severity=Success
Facility=System
SymbolicName=ERROR_DS_PROTOCOL_ERROR
Language=English
A protocol error occurred.
.
Language=Japanese
プロトコルエラーが発生しました。
.

MessageId=8226
Severity=Success
Facility=System
SymbolicName=ERROR_DS_TIMELIMIT_EXCEEDED
Language=English
The time limit for this request was exceeded.
.
Language=Japanese
この要求の制限時間を超えました。
.

MessageId=8227
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SIZELIMIT_EXCEEDED
Language=English
The size limit for this request was exceeded.
.
Language=Japanese
このリクエストのサイズ制限を超えました。
.

MessageId=8228
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ADMIN_LIMIT_EXCEEDED
Language=English
The administrative limit for this request was exceeded.
.
Language=Japanese
この要求の管理者リミットを超えました。
.

MessageId=8229
Severity=Success
Facility=System
SymbolicName=ERROR_DS_COMPARE_FALSE
Language=English
The compare response was false.
.
Language=Japanese
比較応答が偽です 
 。
.

MessageId=8230
Severity=Success
Facility=System
SymbolicName=ERROR_DS_COMPARE_TRUE
Language=English
The compare response was true.
.
Language=Japanese
比較応答はtrueでした。
.

MessageId=8231
Severity=Success
Facility=System
SymbolicName=ERROR_DS_AUTH_METHOD_NOT_SUPPORTED
Language=English
The requested authentication method is not supported by the server.
.
Language=Japanese
要求された認証方法はサーバーによってサポートされていません。
.

MessageId=8232
Severity=Success
Facility=System
SymbolicName=ERROR_DS_STRONG_AUTH_REQUIRED
Language=English
A more secure authentication method is required for this server.
.
Language=Japanese
このサーバーにはより安全な認証方法が必要です。
.

MessageId=8233
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INAPPROPRIATE_AUTH
Language=English
Inappropriate authentication.
.
Language=Japanese
不適切な認証です。
.

MessageId=8234
Severity=Success
Facility=System
SymbolicName=ERROR_DS_AUTH_UNKNOWN
Language=English
The authentication mechanism is unknown.
.
Language=Japanese
認証メカニズムが不明です。
.

MessageId=8235
Severity=Success
Facility=System
SymbolicName=ERROR_DS_REFERRAL
Language=English
A referral was returned from the server.
.
Language=Japanese
認証メカニズムが不明です: サーバーから照会が返されました。
.

MessageId=8236
Severity=Success
Facility=System
SymbolicName=ERROR_DS_UNAVAILABLE_CRIT_EXTENSION
Language=English
The server does not support the requested critical extension.
.
Language=Japanese
サーバーは要求された重要な拡張をサポートしていません。
.

MessageId=8237
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CONFIDENTIALITY_REQUIRED
Language=English
This request requires a secure connection.
.
Language=Japanese
この要求には安全な接続が必要です。
.

MessageId=8238
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INAPPROPRIATE_MATCHING
Language=English
Inappropriate matching.
.
Language=Japanese
不適切なマッチングです。
.

MessageId=8239
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CONSTRAINT_VIOLATION
Language=English
A constraint violation occurred.
.
Language=Japanese
制約違反が発生しました: 制約違反が発生しました。
.

MessageId=8240
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_SUCH_OBJECT
Language=English
There is no such object on the server.
.
Language=Japanese
サーバー上にそのようなオブジェクトはありません。
.

MessageId=8241
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ALIAS_PROBLEM
Language=English
There is an alias problem.
.
Language=Japanese
エイリアスに問題があります。
.

MessageId=8242
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INVALID_DN_SYNTAX
Language=English
An invalid dn syntax has been specified.
.
Language=Japanese
無効な dn 構文が指定されました。
.

MessageId=8243
Severity=Success
Facility=System
SymbolicName=ERROR_DS_IS_LEAF
Language=English
The object is a leaf object.
.
Language=Japanese
オブジェクトがリーフ オブジェクトです。
.

MessageId=8244
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ALIAS_DEREF_PROBLEM
Language=English
There is an alias dereferencing problem.
.
Language=Japanese
エイリアスの再参照に問題があります。
.

MessageId=8245
Severity=Success
Facility=System
SymbolicName=ERROR_DS_UNWILLING_TO_PERFORM
Language=English
The server is unwilling to process the request.
.
Language=Japanese
サーバーはリクエストを処理する気がありません。
.

MessageId=8246
Severity=Success
Facility=System
SymbolicName=ERROR_DS_LOOP_DETECT
Language=English
A loop has been detected.
.
Language=Japanese
ループが検出されました。
.

MessageId=8247
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAMING_VIOLATION
Language=English
There is a naming violation.
.
Language=Japanese
ネーミング違反があります。
.

MessageId=8248
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OBJECT_RESULTS_TOO_LARGE
Language=English
The result set is too large.
.
Language=Japanese
結果セットが大きすぎます。
.

MessageId=8249
Severity=Success
Facility=System
SymbolicName=ERROR_DS_AFFECTS_MULTIPLE_DSAS
Language=English
The operation affects multiple DSAs
.
Language=Japanese
操作が複数のDSAに影響します。
.

MessageId=8250
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SERVER_DOWN
Language=English
The server is not operational.
.
Language=Japanese
サーバーが稼動していません。
.

MessageId=8251
Severity=Success
Facility=System
SymbolicName=ERROR_DS_LOCAL_ERROR
Language=English
A local error has occurred.
.
Language=Japanese
ローカルエラーが発生しました。
.

MessageId=8252
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ENCODING_ERROR
Language=English
An encoding error has occurred.
.
Language=Japanese
エンコード エラーが発生しました。
.

MessageId=8253
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DECODING_ERROR
Language=English
A decoding error has occurred.
.
Language=Japanese
デコードエラーが発生しました。
.

MessageId=8254
Severity=Success
Facility=System
SymbolicName=ERROR_DS_FILTER_UNKNOWN
Language=English
The search filter cannot be recognized.
.
Language=Japanese
検索フィルタを認識できません。
.

MessageId=8255
Severity=Success
Facility=System
SymbolicName=ERROR_DS_PARAM_ERROR
Language=English
One or more parameters are illegal.
.
Language=Japanese
1つ以上のパラメータが不正です。
.

MessageId=8256
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NOT_SUPPORTED
Language=English
The specified method is not supported.
.
Language=Japanese
指定されたメソッドはサポートされていません。
.

MessageId=8257
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_RESULTS_RETURNED
Language=English
No results were returned.
.
Language=Japanese
結果が返されませんでした。
.

MessageId=8258
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CONTROL_NOT_FOUND
Language=English
The specified control is not supported by the server.
.
Language=Japanese
指定されたコントロールはサーバーによってサポートされていません。
.

MessageId=8259
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CLIENT_LOOP
Language=English
A referral loop was detected by the client.
.
Language=Japanese
クライアントによって参照ループが検出されました。
.

MessageId=8260
Severity=Success
Facility=System
SymbolicName=ERROR_DS_REFERRAL_LIMIT_EXCEEDED
Language=English
The preset referral limit was exceeded.
.
Language=Japanese
プリセットの参照制限を超えました。
.

MessageId=8261
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SORT_CONTROL_MISSING
Language=English
The search requires a SORT control.
.
Language=Japanese
検索にはSORTコントロールが必要です。
.

MessageId=8262
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OFFSET_RANGE_ERROR
Language=English
The search results exceed the offset range specified.
.
Language=Japanese
検索結果が指定されたオフセット範囲を超えています。
.

MessageId=8301
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ROOT_MUST_BE_NC
Language=English
The root object must be the head of a naming context. The root object cannot have an instantiated parent.
.
Language=Japanese
ルート オブジェクトはネーミング コンテキストの先頭でなければなりません。ルート オブジェクトはインスタンス化された親を持つことはできません。
.

MessageId=8302
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ADD_REPLICA_INHIBITED
Language=English
The add replica operation cannot be performed. The naming context must be writeable in order to create the replica.
.
Language=Japanese
レプリカの追加操作は実行できません。レプリカを作成するには、ネーム コンテキストが書き込み可能である必要があります。
.

MessageId=8303
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ATT_NOT_DEF_IN_SCHEMA
Language=English
A reference to an attribute that is not defined in the schema occurred.
.
Language=Japanese
スキーマで定義されていない属性への参照が発生しました。
.

MessageId=8304
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MAX_OBJ_SIZE_EXCEEDED
Language=English
The maximum size of an object has been exceeded.
.
Language=Japanese
オブジェクトの最大サイズを超えました。
.

MessageId=8305
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OBJ_STRING_NAME_EXISTS
Language=English
An attempt was made to add an object to the directory with a name that is already in use.
.
Language=Japanese
既に使用されている名前のオブジェクトをディレクトリに追加しようとしました。
.

MessageId=8306
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_RDN_DEFINED_IN_SCHEMA
Language=English
An attempt was made to add an object of a class that does not have an RDN defined in the schema.
.
Language=Japanese
スキーマにRDNが定義されていないクラスのオブジェクトを追加しようとしました。
.

MessageId=8307
Severity=Success
Facility=System
SymbolicName=ERROR_DS_RDN_DOESNT_MATCH_SCHEMA
Language=English
An attempt was made to add an object using an RDN that is not the RDN defined in the schema.
.
Language=Japanese
スキーマで定義されている RDN ではない RDN を使用してオブジェクトを追加しようとしました。
.

MessageId=8308
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_REQUESTED_ATTS_FOUND
Language=English
None of the requested attributes were found on the objects.
.
Language=Japanese
要求された属性がオブジェクトに見つかりませんでした。
.

MessageId=8309
Severity=Success
Facility=System
SymbolicName=ERROR_DS_USER_BUFFER_TO_SMALL
Language=English
The user buffer is too small.
.
Language=Japanese
ユーザー バッファが小さすぎます。
.

MessageId=8310
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ATT_IS_NOT_ON_OBJ
Language=English
The attribute specified in the operation is not present on the object.
.
Language=Japanese
操作で指定された属性がオブジェクトに存在しません。
.

MessageId=8311
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ILLEGAL_MOD_OPERATION
Language=English
Illegal modify operation. Some aspect of the modification is not permitted.
.
Language=Japanese
不正な変更操作です。改造の様相は許されません。
.

MessageId=8312
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OBJ_TOO_LARGE
Language=English
The specified object is too large.
.
Language=Japanese
指定されたオブジェクトが大きすぎます。
.

MessageId=8313
Severity=Success
Facility=System
SymbolicName=ERROR_DS_BAD_INSTANCE_TYPE
Language=English
The specified instance type is not valid.
.
Language=Japanese
指定されたインスタンス タイプが無効です。
.

MessageId=8314
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MASTERDSA_REQUIRED
Language=English
The operation must be performed at a master DSA.
.
Language=Japanese
操作はマスターDSAで実行する必要があります。
.

MessageId=8315
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OBJECT_CLASS_REQUIRED
Language=English
The object class attribute must be specified.
.
Language=Japanese
オブジェクト クラス属性を指定する必要があります。
.

MessageId=8316
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MISSING_REQUIRED_ATT
Language=English
A required attribute is missing.
.
Language=Japanese
必須属性がありません。
.

MessageId=8317
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ATT_NOT_DEF_FOR_CLASS
Language=English
An attempt was made to modify an object to include an attribute that is not legal for its class
.
Language=Japanese
オブジェクトを変更して、そのクラスで有効でない属性を含めようとしました。
.

MessageId=8318
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ATT_ALREADY_EXISTS
Language=English
The specified attribute is already present on the object.
.
Language=Japanese
指定された属性はオブジェクトに既に存在します。
.

MessageId=8320
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_ADD_ATT_VALUES
Language=English
The specified attribute is not present, or has no values.
.
Language=Japanese
指定された属性が存在しないか、値がありません。
.

MessageId=8321
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SINGLE_VALUE_CONSTRAINT
Language=English
Multiple values were specified for an attribute that can have only one value.
.
Language=Japanese
値が 1 つしかない属性に複数の値が指定されました。
.

MessageId=8322
Severity=Success
Facility=System
SymbolicName=ERROR_DS_RANGE_CONSTRAINT
Language=English
A value for the attribute was not in the acceptable range of values.
.
Language=Japanese
属性の値が許容される値の範囲にありませんでした。
.

MessageId=8323
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ATT_VAL_ALREADY_EXISTS
Language=English
The specified value already exists.
.
Language=Japanese
指定された値は既に存在します。
.

MessageId=8324
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_REM_MISSING_ATT
Language=English
The attribute cannot be removed because it is not present on the object.
.
Language=Japanese
属性がオブジェクトに存在しないため削除できません。
.

MessageId=8325
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_REM_MISSING_ATT_VAL
Language=English
The attribute value cannot be removed because it is not present on the object.
.
Language=Japanese
属性値がオブジェクトに存在しないため、削除できません。
.

MessageId=8326
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ROOT_CANT_BE_SUBREF
Language=English
The specified root object cannot be a subref.
.
Language=Japanese
指定されたルート オブジェクトをサブ参照にすることはできません。
.

MessageId=8327
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_CHAINING
Language=English
Chaining is not permitted.
.
Language=Japanese
チェーンは許可されていません。
.

MessageId=8328
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_CHAINED_EVAL
Language=English
Chained evaluation is not permitted.
.
Language=Japanese
連鎖評価は許可されていません。
.

MessageId=8329
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_PARENT_OBJECT
Language=English
The operation could not be performed because the object's parent is either uninstantiated or deleted.
.
Language=Japanese
オブジェクトの親がインスタンス化されていないか削除されているため、操作を実行できませんでした。
.

MessageId=8330
Severity=Success
Facility=System
SymbolicName=ERROR_DS_PARENT_IS_AN_ALIAS
Language=English
Having a parent that is an alias is not permitted. Aliases are leaf objects.
.
Language=Japanese
エイリアスを親に持つことは許可されていません。エイリアスはリーフ オブジェクトです。
.

MessageId=8331
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_MIX_MASTER_AND_REPS
Language=English
The object and parent must be of the same type, either both masters or both replicas.
.
Language=Japanese
オブジェクトと親は、両方のマスタまたは両方のレプリカで同じ型でなければなりません。
.

MessageId=8332
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CHILDREN_EXIST
Language=English
The operation cannot be performed because child objects exist. This operation can only be performed on a leaf object.
.
Language=Japanese
子オブジェクトが存在するため、操作を実行できません。この操作はリーフ オブジェクトに対してのみ実行できます。
.

MessageId=8333
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OBJ_NOT_FOUND
Language=English
Directory object not found.
.
Language=Japanese
ディレクトリ オブジェクトが見つかりません。
.

MessageId=8334
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ALIASED_OBJ_MISSING
Language=English
The aliased object is missing.
.
Language=Japanese
エイリアスされたオブジェクトがありません。
.

MessageId=8335
Severity=Success
Facility=System
SymbolicName=ERROR_DS_BAD_NAME_SYNTAX
Language=English
The object name has bad syntax.
.
Language=Japanese
オブジェクト名の構文が正しくありません。
.

MessageId=8336
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ALIAS_POINTS_TO_ALIAS
Language=English
It is not permitted for an alias to refer to another alias.
.
Language=Japanese
エイリアスが別のエイリアスを参照することは許可されていません。
.

MessageId=8337
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_DEREF_ALIAS
Language=English
The alias cannot be dereferenced.
.
Language=Japanese
エイリアスを参照解除できません。
.

MessageId=8338
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OUT_OF_SCOPE
Language=English
The operation is out of scope.
.
Language=Japanese
操作が範囲外です。
.

MessageId=8339
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OBJECT_BEING_REMOVED
Language=English
The operation cannot continue because the object is in the process of being removed.
.
Language=Japanese
オブジェクトが削除中なので操作を続行できません。
.

MessageId=8340
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_DELETE_DSA_OBJ
Language=English
The DSA object cannot be deleted.
.
Language=Japanese
DSA オブジェクトを削除できません。
.

MessageId=8341
Severity=Success
Facility=System
SymbolicName=ERROR_DS_GENERIC_ERROR
Language=English
A directory service error has occurred.
.
Language=Japanese
ディレクトリサービスエラーが発生しました。
.

MessageId=8342
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DSA_MUST_BE_INT_MASTER
Language=English
The operation can only be performed on an internal master DSA object.
.
Language=Japanese
この操作は内部マスターDSAオブジェクトに対してのみ実行できます。
.

MessageId=8343
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CLASS_NOT_DSA
Language=English
The object must be of class DSA.
.
Language=Japanese
オブジェクトはDSAクラスでなければなりません。
.

MessageId=8344
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INSUFF_ACCESS_RIGHTS
Language=English
Insufficient access rights to perform the operation.
.
Language=Japanese
操作を実行するためのアクセス権が不足しています。
.

MessageId=8345
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ILLEGAL_SUPERIOR
Language=English
The object cannot be added because the parent is not on the list of possible superiors.
.
Language=Japanese
親が可能な上位のリストにないため、オブジェクトを追加できません。
.

MessageId=8346
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ATTRIBUTE_OWNED_BY_SAM
Language=English
Access to the attribute is not permitted because the attribute is owned by the Security Accounts Manager (SAM).
.
Language=Japanese
属性がセキュリティ アカウント マネージャ (SAM) によって所有されているため、属性へのアクセスが許可されていません。
.

MessageId=8347
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_TOO_MANY_PARTS
Language=English
The name has too many parts.
.
Language=Japanese
名前の部分が多すぎます。
.

MessageId=8348
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_TOO_LONG
Language=English
The name is too long.
.
Language=Japanese
名前が長すぎます。
.

MessageId=8349
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_VALUE_TOO_LONG
Language=English
The name value is too long.
.
Language=Japanese
名前の値が長すぎます。
.

MessageId=8350
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_UNPARSEABLE
Language=English
The directory service encountered an error parsing a name.
.
Language=Japanese
ディレクトリ サービスで名前の解析エラーが発生しました。
.

MessageId=8351
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_TYPE_UNKNOWN
Language=English
The directory service cannot get the attribute type for a name.
.
Language=Japanese
ディレクトリ サービスは名前の属性タイプを取得できません。
.

MessageId=8352
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NOT_AN_OBJECT
Language=English
The name does not identify an object; the name identifies a phantom.
.
Language=Japanese
名前がオブジェクトを識別していません。
.

MessageId=8353
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SEC_DESC_TOO_SHORT
Language=English
The security descriptor is too short.
.
Language=Japanese
セキュリティ記述子が短すぎます。
.

MessageId=8354
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SEC_DESC_INVALID
Language=English
The security descriptor is invalid.
.
Language=Japanese
セキュリティ記述子が無効です。
.

MessageId=8355
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_DELETED_NAME
Language=English
Failed to create name for deleted object.
.
Language=Japanese
削除されたオブジェクトの名前の作成に失敗しました。
.

MessageId=8356
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SUBREF_MUST_HAVE_PARENT
Language=English
The parent of a new subref must exist.
.
Language=Japanese
新しいサブ参照の親が存在する必要があります。
.

MessageId=8357
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NCNAME_MUST_BE_NC
Language=English
The object must be a naming context.
.
Language=Japanese
オブジェクトはネーミング コンテキストでなければなりません。
.

MessageId=8358
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_ADD_SYSTEM_ONLY
Language=English
It is not permitted to add an attribute which is owned by the system.
.
Language=Japanese
システムが所有する属性を追加することは許可されていません。
.

MessageId=8359
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CLASS_MUST_BE_CONCRETE
Language=English
The class of the object must be structural; you cannot instantiate an abstract class.
.
Language=Japanese
オブジェクトのクラスは構造的でなければなりません。
.

MessageId=8360
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INVALID_DMD
Language=English
The schema object could not be found.
.
Language=Japanese
スキーマ オブジェクトが見つかりません。
.

MessageId=8361
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OBJ_GUID_EXISTS
Language=English
A local object with this GUID (dead or alive) already exists.
.
Language=Japanese
スキーマ オブジェクトが見つかりません: このGUID (デッドまたはアライブ) を持つローカル オブジェクトがすでに存在します。
.

MessageId=8362
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NOT_ON_BACKLINK
Language=English
The operation cannot be performed on a back link.
.
Language=Japanese
バック リンクに対して操作を実行できません。
.

MessageId=8363
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_CROSSREF_FOR_NC
Language=English
The cross reference for the specified naming context could not be found.
.
Language=Japanese
指定されたネーミング コンテキストの相互参照が見つかりません。
.

MessageId=8364
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SHUTTING_DOWN
Language=English
The operation could not be performed because the directory service is shutting down.
.
Language=Japanese
ディレクトリ サービスがシャットダウンしているため、操作を実行できませんでした。
.

MessageId=8365
Severity=Success
Facility=System
SymbolicName=ERROR_DS_UNKNOWN_OPERATION
Language=English
The directory service request is invalid.
.
Language=Japanese
ディレクトリ サービス要求が無効です。
.

MessageId=8366
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INVALID_ROLE_OWNER
Language=English
The role owner attribute could not be read.
.
Language=Japanese
ロール所有者属性を読み取れませんでした。
.

MessageId=8367
Severity=Success
Facility=System
SymbolicName=ERROR_DS_COULDNT_CONTACT_FSMO
Language=English
The requested FSMO operation failed. The current FSMO holder could not be reached.
.
Language=Japanese
要求されたFSMO操作は失敗しました。現在のFSMO保持者に連絡できませんでした。
.

MessageId=8368
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CROSS_NC_DN_RENAME
Language=English
Modification of a DN across a naming context is not permitted.
.
Language=Japanese
ネーミング コンテキスト全体でDNの変更は許可されていません。
.

MessageId=8369
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_MOD_SYSTEM_ONLY
Language=English
The attribute cannot be modified because it is owned by the system.
.
Language=Japanese
属性はシステムによって所有されているため変更できません。
.

MessageId=8370
Severity=Success
Facility=System
SymbolicName=ERROR_DS_REPLICATOR_ONLY
Language=English
Only the replicator can perform this function.
.
Language=Japanese
レプリケータのみがこの機能を実行できます。
.

MessageId=8371
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OBJ_CLASS_NOT_DEFINED
Language=English
The specified class is not defined.
.
Language=Japanese
指定されたクラスは定義されていません。
.

MessageId=8372
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OBJ_CLASS_NOT_SUBCLASS
Language=English
The specified class is not a subclass.
.
Language=Japanese
指定されたクラスはサブクラスではありません。
.

MessageId=8373
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_REFERENCE_INVALID
Language=English
The name reference is invalid.
.
Language=Japanese
名前の参照が無効です。
.

MessageId=8374
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CROSS_REF_EXISTS
Language=English
A cross reference already exists.
.
Language=Japanese
相互参照は既に存在します。
.

MessageId=8375
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_DEL_MASTER_CROSSREF
Language=English
It is not permitted to delete a master cross reference.
.
Language=Japanese
マスター相互参照の削除は許可されていません。
.

MessageId=8376
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SUBTREE_NOTIFY_NOT_NC_HEAD
Language=English
Subtree notifications are only supported on NC heads.
.
Language=Japanese
サブツリー通知はNCヘッドでのみサポートされます。
.

MessageId=8377
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NOTIFY_FILTER_TOO_COMPLEX
Language=English
Notification filter is too complex.
.
Language=Japanese
通知フィルタが複雑すぎます。
.

MessageId=8378
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DUP_RDN
Language=English
Schema update failed: duplicate RDN.
.
Language=Japanese
スキーマの更新に失敗しました: RDNが重複しています。
.

MessageId=8379
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DUP_OID
Language=English
Schema update failed: duplicate OID
.
Language=Japanese
スキーマの更新に失敗しました: OID が重複しています。
.

MessageId=8380
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DUP_MAPI_ID
Language=English
Schema update failed: duplicate MAPI identifier.
.
Language=Japanese
スキーマの更新に失敗しました: MAPI 識別子が重複しています。
.

MessageId=8381
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DUP_SCHEMA_ID_GUID
Language=English
Schema update failed: duplicate schema-id GUID.
.
Language=Japanese
スキーマの更新に失敗しました: スキーマ ID GUID が重複しています。
.

MessageId=8382
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DUP_LDAP_DISPLAY_NAME
Language=English
Schema update failed: duplicate LDAP display name.
.
Language=Japanese
スキーマの更新に失敗しました: LDAP 表示名が重複しています。
.

MessageId=8383
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SEMANTIC_ATT_TEST
Language=English
Schema update failed: range-lower less than range upper
.
Language=Japanese
スキーマの更新に失敗しました: range-lower が range upper よりも小さいです。
.

MessageId=8384
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SYNTAX_MISMATCH
Language=English
Schema update failed: syntax mismatch
.
Language=Japanese
スキーマの更新に失敗しました: 構文の不一致です。
.

MessageId=8385
Severity=Success
Facility=System
SymbolicName=ERROR_DS_EXISTS_IN_MUST_HAVE
Language=English
Schema deletion failed: attribute is used in must-contain
.
Language=Japanese
スキーマの削除に失敗しました: 属性が must-contain で使用されています。
.

MessageId=8386
Severity=Success
Facility=System
SymbolicName=ERROR_DS_EXISTS_IN_MAY_HAVE
Language=English
Schema deletion failed: attribute is used in may-contain
.
Language=Japanese
スキーマの削除に失敗しました: 属性が may-contain で使用されています。
.

MessageId=8387
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NONEXISTENT_MAY_HAVE
Language=English
Schema update failed: attribute in may-contain does not exist
.
Language=Japanese
スキーマの更新に失敗しました: may-contain 内に属性が存在しません。
.

MessageId=8388
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NONEXISTENT_MUST_HAVE
Language=English
Schema update failed: attribute in must-contain does not exist
.
Language=Japanese
スキーマ更新に失敗しました: must-contain 内の属性が存在しません。
.

MessageId=8389
Severity=Success
Facility=System
SymbolicName=ERROR_DS_AUX_CLS_TEST_FAIL
Language=English
Schema update failed: class in aux-class list does not exist or is not an auxiliary class
.
Language=Japanese
スキーマの更新に失敗しました: aux-class リスト内のクラスが存在しないか、補助クラスではありません。
.

MessageId=8390
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NONEXISTENT_POSS_SUP
Language=English
Schema update failed: class in poss-superiors does not exist
.
Language=Japanese
スキーマの更新に失敗しました: poss-superiors 内のクラスが存在しません。
.

MessageId=8391
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SUB_CLS_TEST_FAIL
Language=English
Schema update failed: class in subclassof list does not exist or does not satisfy hierarchy rules
.
Language=Japanese
スキーマの更新に失敗しました: subclassof リスト内のクラスが存在しないか、階層ルールを満たしていません。
.

MessageId=8392
Severity=Success
Facility=System
SymbolicName=ERROR_DS_BAD_RDN_ATT_ID_SYNTAX
Language=English
Schema update failed: Rdn-Att-Id has wrong syntax
.
Language=Japanese
スキーマの更新に失敗しました: Rdn-Att-Idの構文が間違っています。
.

MessageId=8393
Severity=Success
Facility=System
SymbolicName=ERROR_DS_EXISTS_IN_AUX_CLS
Language=English
Schema deletion failed: class is used as auxiliary class
.
Language=Japanese
スキーマの削除に失敗しました: クラスが補助クラスとして使用されています。
.

MessageId=8394
Severity=Success
Facility=System
SymbolicName=ERROR_DS_EXISTS_IN_SUB_CLS
Language=English
Schema deletion failed: class is used as sub class
.
Language=Japanese
スキーマの削除に失敗しました: クラスがサブクラスとして使用されています。
.

MessageId=8395
Severity=Success
Facility=System
SymbolicName=ERROR_DS_EXISTS_IN_POSS_SUP
Language=English
Schema deletion failed: class is used as poss superior
.
Language=Japanese
スキーマの削除に失敗しました。
.

MessageId=8396
Severity=Success
Facility=System
SymbolicName=ERROR_DS_RECALCSCHEMA_FAILED
Language=English
Schema update failed in recalculating validation cache.
.
Language=Japanese
検証キャッシュの再計算でスキーマの更新に失敗しました。
.

MessageId=8397
Severity=Success
Facility=System
SymbolicName=ERROR_DS_TREE_DELETE_NOT_FINISHED
Language=English
The tree deletion is not finished.
.
Language=Japanese
ツリーの削除が完了していません。ツリーの削除を続行するには、再度要求する必要があります。
.

MessageId=8398
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_DELETE
Language=English
The requested delete operation could not be performed.
.
Language=Japanese
要求された削除操作を実行できませんでした。
.

MessageId=8399
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ATT_SCHEMA_REQ_ID
Language=English
Cannot read the governs class identifier for the schema record.
.
Language=Japanese
スキーマ レコードのガバメント クラス識別子を読み取ることができません。
.

MessageId=8400
Severity=Success
Facility=System
SymbolicName=ERROR_DS_BAD_ATT_SCHEMA_SYNTAX
Language=English
The attribute schema has bad syntax.
.
Language=Japanese
属性スキーマの構文が正しくありません。
.

MessageId=8401
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_CACHE_ATT
Language=English
The attribute could not be cached.
.
Language=Japanese
属性をキャッシュできませんでした。
.

MessageId=8402
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_CACHE_CLASS
Language=English
The class could not be cached.
.
Language=Japanese
クラスをキャッシュできませんでした。
.

MessageId=8403
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_REMOVE_ATT_CACHE
Language=English
The attribute could not be removed from the cache.
.
Language=Japanese
属性をキャッシュから削除できませんでした。
.

MessageId=8404
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_REMOVE_CLASS_CACHE
Language=English
The class could not be removed from the cache.
.
Language=Japanese
クラスをキャッシュから削除できませんでした。
.

MessageId=8405
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_RETRIEVE_DN
Language=English
The distinguished name attribute could not be read.
.
Language=Japanese
識別名属性を読み取れませんでした。
.

MessageId=8406
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MISSING_SUPREF
Language=English
No superior reference has been configured for the directory service. The directory service is therefore unable to issue referrals to objects outside this forest.
.
Language=Japanese
ディレクトリ サービスに上位参照が構成されていません。したがって、ディレクトリサービスはこのフォレスト外のオブジェクトへの参照を発行できません。
.

MessageId=8407
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_RETRIEVE_INSTANCE
Language=English
The instance type attribute could not be retrieved.
.
Language=Japanese
インスタンス タイプ属性を取得できませんでした。
.

MessageId=8408
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CODE_INCONSISTENCY
Language=English
An internal error has occurred.
.
Language=Japanese
内部エラーが発生しました。
.

MessageId=8409
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DATABASE_ERROR
Language=English
A database error has occurred.
.
Language=Japanese
データベースエラーが発生しました。
.

MessageId=8410
Severity=Success
Facility=System
SymbolicName=ERROR_DS_GOVERNSID_MISSING
Language=English
The attribute GOVERNSID is missing.
.
Language=Japanese
属性GOVERNSIDが見つかりません。
.

MessageId=8411
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MISSING_EXPECTED_ATT
Language=English
An expected attribute is missing.
.
Language=Japanese
指定された属性が見つかりません。
.

MessageId=8412
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NCNAME_MISSING_CR_REF
Language=English
The specified naming context is missing a cross reference.
.
Language=Japanese
指定された名前コンテキストに相互参照がありません。
.

MessageId=8413
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SECURITY_CHECKING_ERROR
Language=English
A security checking error has occurred.
.
Language=Japanese
セキュリティ チェック エラーが発生しました。
.

MessageId=8414
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SCHEMA_NOT_LOADED
Language=English
The schema is not loaded.
.
Language=Japanese
スキーマがロードされていません。
.

MessageId=8415
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SCHEMA_ALLOC_FAILED
Language=English
Schema allocation failed. Please check if the machine is running low on memory.
.
Language=Japanese
スキーマの割り当てに失敗しました。マシンのメモリが不足していないか確認してください。
.

MessageId=8416
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ATT_SCHEMA_REQ_SYNTAX
Language=English
Failed to obtain the required syntax for the attribute schema.
.
Language=Japanese
スキーマの割り当てに失敗しました: 属性スキーマに必要な構文の取得に失敗しました。
.

MessageId=8417
Severity=Success
Facility=System
SymbolicName=ERROR_DS_GCVERIFY_ERROR
Language=English
The global catalog verification failed. The global catalog is not available or does not support the operation. Some part of the directory is currently not available.
.
Language=Japanese
グローバル カタログの検証に失敗しました。グローバル カタログが使用できないか、操作をサポートしていません。ディレクトリの一部は現在使用できません。
.

MessageId=8418
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_SCHEMA_MISMATCH
Language=English
The replication operation failed because of a schema mismatch between the servers involved.
.
Language=Japanese
関係するサーバ間でスキーマが一致しなかったため、レプリケーション操作に失敗しました。
.

MessageId=8419
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_FIND_DSA_OBJ
Language=English
The DSA object could not be found.
.
Language=Japanese
DSA オブジェクトが見つかりませんでした。
.

MessageId=8420
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_FIND_EXPECTED_NC
Language=English
The naming context could not be found.
.
Language=Japanese
ネーミング コンテキストが見つかりません。
.

MessageId=8421
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_FIND_NC_IN_CACHE
Language=English
The naming context could not be found in the cache.
.
Language=Japanese
ネーミング コンテキストがキャッシュに見つかりませんでした。
.

MessageId=8422
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_RETRIEVE_CHILD
Language=English
The child object could not be retrieved.
.
Language=Japanese
子オブジェクトを取得できませんでした。
.

MessageId=8423
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SECURITY_ILLEGAL_MODIFY
Language=English
The modification was not permitted for security reasons.
.
Language=Japanese
セキュリティ上の理由で変更が許可されませんでした。
.

MessageId=8424
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_REPLACE_HIDDEN_REC
Language=English
The operation cannot replace the hidden record.
.
Language=Japanese
操作は非表示レコードを置換できません。
.

MessageId=8425
Severity=Success
Facility=System
SymbolicName=ERROR_DS_BAD_HIERARCHY_FILE
Language=English
The hierarchy file is invalid.
.
Language=Japanese
階層ファイルが無効です。
.

MessageId=8426
Severity=Success
Facility=System
SymbolicName=ERROR_DS_BUILD_HIERARCHY_TABLE_FAILED
Language=English
The attempt to build the hierarchy table failed.
.
Language=Japanese
階層テーブルの構築に失敗しました。
.

MessageId=8427
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CONFIG_PARAM_MISSING
Language=English
The directory configuration parameter is missing from the registry.
.
Language=Japanese
ディレクトリ構成パラメータがレジストリにありません。
.

MessageId=8428
Severity=Success
Facility=System
SymbolicName=ERROR_DS_COUNTING_AB_INDICES_FAILED
Language=English
The attempt to count the address book indices failed.
.
Language=Japanese
アドレス帳インデックスのカウントに失敗しました。
.

MessageId=8429
Severity=Success
Facility=System
SymbolicName=ERROR_DS_HIERARCHY_TABLE_MALLOC_FAILED
Language=English
The allocation of the hierarchy table failed.
.
Language=Japanese
階層テーブルの割り当てに失敗しました。
.

MessageId=8430
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INTERNAL_FAILURE
Language=English
The directory service encountered an internal failure.
.
Language=Japanese
ディレクトリ サービスで内部障害が発生しました。
.

MessageId=8431
Severity=Success
Facility=System
SymbolicName=ERROR_DS_UNKNOWN_ERROR
Language=English
The directory service encountered an unknown failure.
.
Language=Japanese
ディレクトリ サービスで不明な障害が発生しました。
.

MessageId=8432
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ROOT_REQUIRES_CLASS_TOP
Language=English
A root object requires a class of 'top'.
.
Language=Japanese
ルート オブジェクトがtopのクラスを必要とします。
.

MessageId=8433
Severity=Success
Facility=System
SymbolicName=ERROR_DS_REFUSING_FSMO_ROLES
Language=English
This directory server is shutting down, and cannot take ownership of new floating single-master operation roles.
.
Language=Japanese
このディレクトリサーバーはシャットダウン中で、新しいフローティングシングルマスター操作ロールの所有権を取得できません。
.

MessageId=8434
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MISSING_FSMO_SETTINGS
Language=English
The directory service is missing mandatory configuration information, and is unable to determine the ownership of floating single-master operation roles.
.
Language=Japanese
ディレクトリサービスに必須の構成情報がなく、フローティング シングルマスター操作ロールの所有権を決定できません。
.

MessageId=8435
Severity=Success
Facility=System
SymbolicName=ERROR_DS_UNABLE_TO_SURRENDER_ROLES
Language=English
The directory service was unable to transfer ownership of one or more floating single-master operation roles to other servers.
.
Language=Japanese
ディレクトリサービスは、1つ以上のフローティングシングルマスターオペレーションロールの所有権を他のサーバーに転送できませんでした。
.

MessageId=8436
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_GENERIC
Language=English
The replication operation failed.
.
Language=Japanese
レプリケーション操作に失敗しました。
.

MessageId=8437
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_INVALID_PARAMETER
Language=English
An invalid parameter was specified for this replication operation.
.
Language=Japanese
このレプリケーション操作に無効なパラメータが指定されました。
.

MessageId=8438
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_BUSY
Language=English
The directory service is too busy to complete the replication operation at this time.
.
Language=Japanese
ディレクトリ サービスがビジー状態で、レプリケーション操作を完了できません。
.

MessageId=8439
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_BAD_DN
Language=English
The distinguished name specified for this replication operation is invalid.
.
Language=Japanese
このレプリケーション操作に指定された識別名は無効です。
.

MessageId=8440
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_BAD_NC
Language=English
The naming context specified for this replication operation is invalid.
.
Language=Japanese
このレプリケーション操作に指定された名前コンテキストが無効です。
.

MessageId=8441
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_DN_EXISTS
Language=English
The distinguished name specified for this replication operation already exists.
.
Language=Japanese
このレプリケーション操作に指定された識別名はすでに存在します。
.

MessageId=8442
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_INTERNAL_ERROR
Language=English
The replication system encountered an internal error.
.
Language=Japanese
レプリケーション システムで内部エラーが発生しました。
.

MessageId=8443
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_INCONSISTENT_DIT
Language=English
The replication operation encountered a database inconsistency.
.
Language=Japanese
レプリケーション操作でデータベースの不整合が発生しました。
.

MessageId=8444
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_CONNECTION_FAILED
Language=English
The server specified for this replication operation could not be contacted.
.
Language=Japanese
このレプリケーション操作で指定されたサーバーにアクセスできませんでした。
.

MessageId=8445
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_BAD_INSTANCE_TYPE
Language=English
The replication operation encountered an object with an invalid instance type.
.
Language=Japanese
レプリケーション操作で無効なインスタンス型を持つオブジェクトに遭遇しました。
.

MessageId=8446
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_OUT_OF_MEM
Language=English
The replication operation failed to allocate memory.
.
Language=Japanese
レプリケーション操作はメモリの割り当てに失敗しました。
.

MessageId=8447
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_MAIL_PROBLEM
Language=English
The replication operation encountered an error with the mail system.
.
Language=Japanese
レプリケーション操作でメール システムでエラーが発生しました。
.

MessageId=8448
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_REF_ALREADY_EXISTS
Language=English
The replication reference information for the target server already exists.
.
Language=Japanese
ターゲット サーバのレプリケーション参照情報はすでに存在します。
.

MessageId=8449
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_REF_NOT_FOUND
Language=English
The replication reference information for the target server does not exist.
.
Language=Japanese
対象サーバーのレプリケーション参照情報が存在しません。
.

MessageId=8450
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_OBJ_IS_REP_SOURCE
Language=English
The naming context cannot be removed because it is replicated to another server.
.
Language=Japanese
ネーミング コンテキストは別のサーバにレプリケートされているため削除できません。
.

MessageId=8451
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_DB_ERROR
Language=English
The replication operation encountered a database error.
.
Language=Japanese
レプリケーション操作でデータベース エラーが発生しました。
.

MessageId=8452
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_NO_REPLICA
Language=English
The naming context is in the process of being removed or is not replicated from the specified server.
.
Language=Japanese
ネーミング コンテキストが削除処理中です。
.

MessageId=8453
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_ACCESS_DENIED
Language=English
Replication access was denied.
.
Language=Japanese
レプリケーション アクセスが拒否されました。
.

MessageId=8454
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_NOT_SUPPORTED
Language=English
The requested operation is not supported by this version of the directory service.
.
Language=Japanese
要求された操作は、このバージョンのディレクトリ サービスではサポートされていません。
.

MessageId=8455
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_RPC_CANCELLED
Language=English
The replication remote procedure call was cancelled.
.
Language=Japanese
レプリケーション リモート プロシージャ呼び出しがキャンセルされました。
.

MessageId=8456
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_SOURCE_DISABLED
Language=English
The source server is currently rejecting replication requests.
.
Language=Japanese
ソース サーバーは現在レプリケーション要求を拒否しています。
.

MessageId=8457
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_SINK_DISABLED
Language=English
The destination server is currently rejecting replication requests.
.
Language=Japanese
コピー先サーバーは現在レプリケーション要求を拒否しています。
.

MessageId=8458
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_NAME_COLLISION
Language=English
The replication operation failed due to a collision of object names.
.
Language=Japanese
オブジェクト名の衝突によりレプリケーション操作に失敗しました。
.

MessageId=8459
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_SOURCE_REINSTALLED
Language=English
The replication source has been reinstalled.
.
Language=Japanese
レプリケーション ソースが再インストールされました。
.

MessageId=8460
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_MISSING_PARENT
Language=English
The replication operation failed because a required parent object is missing.
.
Language=Japanese
必要な親オブジェクトが見つからないため、レプリケーション操作に失敗しました。
.

MessageId=8461
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_PREEMPTED
Language=English
The replication operation was preempted.
.
Language=Japanese
レプリケーション操作が先取りされました。
.

MessageId=8462
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_ABANDON_SYNC
Language=English
The replication synchronization attempt was abandoned because of a lack of updates.
.
Language=Japanese
更新がないため、レプリケーションの同期化を中止しました。
.

MessageId=8463
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_SHUTDOWN
Language=English
The replication operation was terminated because the system is shutting down.
.
Language=Japanese
システムがシャットダウンしているため、レプリケーション操作を終了しました。
.

MessageId=8464
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_INCOMPATIBLE_PARTIAL_SET
Language=English
Synchronization attempt failed because the destination DC is currently waiting to synchronize new partial attributes from source. This condition is normal if a recent schema change modified the partial attribute set. The destination partial attribute set is not a subset of the source partial attribute set.
.
Language=Japanese
宛先DCがソースからの新しい部分属性の同期を待機中のため、同期に失敗しました。この状態は、最近のスキーマ変更によって部分属性セットが変更された場合は正常です。宛先の部分属性セットはソースの部分属性セットのサブセットではありません。
.

MessageId=8465
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_SOURCE_IS_PARTIAL_REPLICA
Language=English
The replication synchronization attempt failed because a master replica attempted to sync from a partial replica.
.
Language=Japanese
マスター レプリカが部分レプリカから同期しようとしたため、レプリケーションの同期に失敗しました。
.

MessageId=8466
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_EXTN_CONNECTION_FAILED
Language=English
The server specified for this replication operation was contacted, but that server was unable to contact an additional server needed to complete the operation.
.
Language=Japanese
このレプリケーション操作に指定されたサーバーに連絡しましたが、そのサーバーが操作を完了するために必要な追加のサーバーに連絡できませんでした。
.

MessageId=8467
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INSTALL_SCHEMA_MISMATCH
Language=English
The version of the Active Directory schema of the source forest is not compatible with the version of Active Directory on this computer.
.
Language=Japanese
ソース フォレストのディレクトリ サービス スキーマのバージョンは、このコンピュータのディレクトリ サービスのバージョンと互換性がありません。
.

MessageId=8468
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DUP_LINK_ID
Language=English
Schema update failed: An attribute with the same link identifier already exists.
.
Language=Japanese
スキーマの更新に失敗しました。
.

MessageId=8469
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_ERROR_RESOLVING
Language=English
Name translation: Generic processing error.
.
Language=Japanese
名前変換: 汎用処理エラーです。
.

MessageId=8470
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_ERROR_NOT_FOUND
Language=English
Name translation: Could not find the name or insufficient right to see name.
.
Language=Japanese
名前変換: 名前が見つかりません。
.

MessageId=8471
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_ERROR_NOT_UNIQUE
Language=English
Name translation: Input name mapped to more than one output name.
.
Language=Japanese
名前変換: 入力名が複数の出力名にマップされました。
.

MessageId=8472
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_ERROR_NO_MAPPING
Language=English
Name translation: Input name found, but not the associated output format.
.
Language=Japanese
名前変換: 入力名は見つかりましたが、関連する出力形式は見つかりませんでした。
.

MessageId=8473
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_ERROR_DOMAIN_ONLY
Language=English
Name translation: Unable to resolve completely, only the domain was found.
.
Language=Japanese
名前変換: 完全に解決できません。ドメインのみが見つかりました。
.

MessageId=8474
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_ERROR_NO_SYNTACTICAL_MAPPING
Language=English
Name translation: Unable to perform purely syntactical mapping at the client without going out to the wire.
.
Language=Japanese
名前変換：クライアント側で純粋な構文マッピングを行うことができません。
.

MessageId=8475
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CONSTRUCTED_ATT_MOD
Language=English
Modification of a constructed attribute is not allowed.
.
Language=Japanese
構築された属性の変更は許可されていません。
.

MessageId=8476
Severity=Success
Facility=System
SymbolicName=ERROR_DS_WRONG_OM_OBJ_CLASS
Language=English
The OM-Object-Class specified is incorrect for an attribute with the specified syntax.
.
Language=Japanese
指定された構文の属性に対して、指定された OM-Object-Class が不正です。
.

MessageId=8477
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_REPL_PENDING
Language=English
The replication request has been posted; waiting for reply.
.
Language=Japanese
レプリケーション要求が投稿されました。
.

MessageId=8478
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DS_REQUIRED
Language=English
The requested operation requires a directory service, and none was available.
.
Language=Japanese
要求された操作にはディレクトリ サービスが必要です。
.

MessageId=8479
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INVALID_LDAP_DISPLAY_NAME
Language=English
The LDAP display name of the class or attribute contains non-ASCII characters.
.
Language=Japanese
クラスまたは属性のLDAP表示名にASCII以外の文字が含まれています。
.

MessageId=8480
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NON_BASE_SEARCH
Language=English
The requested search operation is only supported for base searches.
.
Language=Japanese
要求された検索操作はベース検索でのみサポートされています。
.

MessageId=8481
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_RETRIEVE_ATTS
Language=English
The search failed to retrieve attributes from the database.
.
Language=Japanese
データベースからの属性の取得に失敗しました。
.

MessageId=8482
Severity=Success
Facility=System
SymbolicName=ERROR_DS_BACKLINK_WITHOUT_LINK
Language=English
The schema update operation tried to add a backward link attribute that has no corresponding forward link.
.
Language=Japanese
スキーマ更新操作は、対応する前方リンクがない後方リンク属性を追加しようとしました。
.

MessageId=8483
Severity=Success
Facility=System
SymbolicName=ERROR_DS_EPOCH_MISMATCH
Language=English
Source and destination of a cross domain move do not agree on the object's epoch number. Either source or destination does not have the latest version of the object.
.
Language=Japanese
オブジェクトのエポック番号でドメイン間移動の送信元と送信先が一致しません。移動元または移動先のどちらかがオブジェクトの最新バージョンを持っていません。
.

MessageId=8484
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SRC_NAME_MISMATCH
Language=English
Source and destination of a cross domain move do not agree on the object's current name. Either source or destination does not have the latest version of the object.
.
Language=Japanese
クロスドメイン移動の移動元と移動先でオブジェクトの現在の名前が一致しません。移動元または移動先のどちらかがオブジェクトの最新バージョンを持っていません。
.

MessageId=8485
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SRC_AND_DST_NC_IDENTICAL
Language=English
Source and destination of a cross domain move operation are identical. Caller should use local move operation instead of cross domain move operation.
.
Language=Japanese
クロスドメイン移動操作の移動元と移動先が同じです。呼び出し元では、クロスドメイン移動操作の代わりにローカル移動操作を使用してください。
.

MessageId=8486
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DST_NC_MISMATCH
Language=English
Source and destination for a cross domain move are not in agreement on the naming contexts in the forest. Either source or destination does not have the latest version of the Partitions container.
.
Language=Japanese
クロスドメイン移動の移動元と移動先がフォレスト内の名前コンテキストで一致していません。移動元または移動先のいずれかがパーティション コンテナの最新バージョンを持っていません。
.

MessageId=8487
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NOT_AUTHORITIVE_FOR_DST_NC
Language=English
Destination of a cross domain move is not authoritative for the destination naming context.
.
Language=Japanese
クロスドメイン移動の移動先が、移動先のネーム コンテキストに対して権限を持っていません。
.

MessageId=8488
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SRC_GUID_MISMATCH
Language=English
Source and destination of a cross domain move do not agree on the identity of the source object. Either source or destination does not have the latest version of the source object.
.
Language=Japanese
クロスドメイン移動の移動元と移動先が、移動先のネーミング コンテキストで一致していません。
.

MessageId=8489
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_MOVE_DELETED_OBJECT
Language=English
Object being moved across domains is already known to be deleted by the destination server. The source server does not have the latest version of the source object.
.
Language=Japanese
ドメインをまたいで移動されるオブジェクトは、移動先のサーバーですでに削除されていることがわかっています。移動元サーバに移動元オブジェクトの最新バージョンがありません。
.

MessageId=8490
Severity=Success
Facility=System
SymbolicName=ERROR_DS_PDC_OPERATION_IN_PROGRESS
Language=English
Another operation, which requires exclusive access to the PDC PSMO, is already in progress.
.
Language=Japanese
PDC FSMOへの排他アクセスを必要とする別の操作がすでに進行中です。
.

MessageId=8491
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CROSS_DOMAIN_CLEANUP_REQD
Language=English
A cross domain move operation failed such that the two versions of the moved object exist - one each in the source and destination domains. The destination object needs to be removed to restore the system to a consistent state.
.
Language=Japanese
移動元ドメインと移動先ドメインにそれぞれ1つずつ、移動されたオブジェクトの2つのバージョンが存在するようなクロスドメインの移動操作に失敗しました。システムを一貫性のある状態に復元するには、移動先のオブジェクトを削除する必要があります。
.

MessageId=8492
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ILLEGAL_XDOM_MOVE_OPERATION
Language=English
This object may not be moved across domain boundaries either because cross domain moves for this class are disallowed, or the object has some special characteristics, e.g.: trust account or restricted RID, which prevent its move.
.
Language=Japanese
このクラスのドメイン間の移動が禁止されているか、オブジェクトに特別な特性 (信頼アカウントや制限付きRIDなど) があるため、このオブジェクトをドメイン境界を越えて移動できません。
.

MessageId=8493
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_WITH_ACCT_GROUP_MEMBERSHPS
Language=English
Can't move objects with memberships across domain boundaries as once moved, this would violate the membership conditions of the account group. Remove the object from any account group memberships and retry.
.
Language=Japanese
一度移動するとアカウント グループのメンバシップ条件に違反するため、ドメイン境界を越えてメンバシップを持つオブジェクトを移動できません。オブジェクトをアカウント グループのメンバシップから削除して再試行してください。
.

MessageId=8494
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NC_MUST_HAVE_NC_PARENT
Language=English
A naming context head must be the immediate child of another naming context head, not of an interior node.
.
Language=Japanese
名前付けコンテキスト ヘッドは、内部ノードではなく別の名前付けコンテキスト ヘッドの直接の子でなければなりません。
.

MessageId=8495
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CR_IMPOSSIBLE_TO_VALIDATE
Language=English
The directory cannot validate the proposed naming context name because it does not hold a replica of the naming context above the proposed naming context. Please ensure that the domain naming master role is held by a server that is configured as a global catalog server, and that the server is up to date with its replication partners. (Applies only to Windows 2000 Domain Naming masters)
.
Language=Japanese
ディレクトリが提案されたネーミング コンテキストの上のネーミング コンテキストのレプリカを保持していないため、提案されたネーミング コンテキスト名を検証できません。グローバルカタログサーバーとして構成されているサーバーによってドメインネーミングマスターの役割が保持され、そのサーバーがレプリケーションパートナーと最新であることを確認してください。 (ReliefWin 2000ドメイン ネーミング マスターにのみ適用されます) 。
.

MessageId=8496
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DST_DOMAIN_NOT_NATIVE
Language=English
Destination domain must be in native mode.
.
Language=Japanese
宛先ドメインがネイティブモードである必要があります。
.

MessageId=8497
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MISSING_INFRASTRUCTURE_CONTAINER
Language=English
The operation cannot be performed because the server does not have an infrastructure container in the domain of interest.
.
Language=Japanese
サーバーが対象のドメインにインフラストラクチャ コンテナを持っていないため、操作を実行できません。
.

MessageId=8498
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_MOVE_ACCOUNT_GROUP
Language=English
Cross-domain move of non-empty account groups is not allowed.
.
Language=Japanese
空でないアカウントグループのクロスドメイン移動は許可されていません。
.

MessageId=8499
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_MOVE_RESOURCE_GROUP
Language=English
Cross-domain move of non-empty resource groups is not allowed.
.
Language=Japanese
空でないリソースグループのクロスドメイン移動は許可されていません。
.

MessageId=8500
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INVALID_SEARCH_FLAG
Language=English
The search flags for the attribute are invalid. The ANR bit is valid only on attributes of Unicode or Teletex strings.
.
Language=Japanese
属性の検索フラグが無効です。ANRビットはUnicodeまたはTeletex文字列の属性でのみ有効です。
.

MessageId=8501
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_TREE_DELETE_ABOVE_NC
Language=English
Tree deletions starting at an object which has an NC head as a descendant are not allowed.
.
Language=Japanese
NCヘッドを子孫に持つオブジェクトからツリーを削除することはできません。
.

MessageId=8502
Severity=Success
Facility=System
SymbolicName=ERROR_DS_COULDNT_LOCK_TREE_FOR_DELETE
Language=English
The directory service failed to lock a tree in preparation for a tree deletion because the tree was in use.
.
Language=Japanese
ツリーが使用中であるため、ディレクトリ サービスがツリーの削除に備えてツリーをロックできませんでした。
.

MessageId=8503
Severity=Success
Facility=System
SymbolicName=ERROR_DS_COULDNT_IDENTIFY_OBJECTS_FOR_TREE_DELETE
Language=English
The directory service failed to identify the list of objects to delete while attempting a tree deletion.
.
Language=Japanese
ツリーの削除を試みているときに、ディレクトリ サービスが削除するオブジェクトのリストを識別できませんでした。
.

MessageId=8504
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SAM_INIT_FAILURE
Language=English
Security Accounts Manager initialization failed because of the following error: %1.
Error Status: 0x%2. Click OK to shut down the system and reboot into Directory Services Restore Mode. Check the event log for detailed information.
.
Language=Japanese
セキュリティ アカウント マネージャの初期化は次のエラーにより失敗しました: %1
エラー ステータス: 0x%2
このシステムをシャットダウンし、ディレクトリ サービス リストア モードに再起動してください。詳細についてはイベント ログを確認してください。
.

MessageId=8505
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SENSITIVE_GROUP_VIOLATION
Language=English
Only an administrator can modify the membership list of an administrative group.
.
Language=Japanese
管理グループのメンバーシップ リストを変更できるのは管理者のみです。
.

MessageId=8506
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_MOD_PRIMARYGROUPID
Language=English
Cannot change the primary group ID of a domain controller account.
.
Language=Japanese
ドメイン コントローラ アカウントのプライマリグループIDを変更できません。
.

MessageId=8507
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ILLEGAL_BASE_SCHEMA_MOD
Language=English
An attempt is made to modify the base schema.
.
Language=Japanese
ベース スキーマを変更しようとしました。
.

MessageId=8508
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NONSAFE_SCHEMA_CHANGE
Language=English
Adding a new mandatory attribute to an existing class, deleting a mandatory attribute from an existing class, or adding an optional attribute to the special class Top that is not a backlink attribute (directly or through inheritance, for example, by adding or deleting an auxiliary class) is not allowed.
.
Language=Japanese
既存のクラスに新しい必須属性を追加すること、既存のクラスから必須属性を削除すること、または特別なクラスTopにバックリンク属性ではないオプション属性を追加すること (直接、または補助クラスを追加または削除するなど継承によって) は許可されていません。
.

MessageId=8509
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SCHEMA_UPDATE_DISALLOWED
Language=English
Schema update is not allowed on this DC because the DC is not the schema FSMO Role Owner.
.
Language=Japanese
DC がスキーマ FSMO ロール所有者ではないため、この DC でのスキーマ更新は許可されません。
.

MessageId=8510
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_CREATE_UNDER_SCHEMA
Language=English
An object of this class cannot be created under the schema container. You can only create attribute-schema and class-schema objects under the schema container.
.
Language=Japanese
このクラスのオブジェクトはスキーマ コンテナの下に作成できません。スキーマ コンテナの下に作成できるのは、属性スキーマ オブジェクトとクラス スキーマ オブジェクトのみです。
.

MessageId=8511
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INSTALL_NO_SRC_SCH_VERSION
Language=English
The replica/child install failed to get the objectVersion attribute on the schema container on the source DC. Either the attribute is missing on the schema container or the credentials supplied do not have permission to read it.
.
Language=Japanese
レプリカ/チャイルド インストールで、ソース DC のスキーマ コンテナの objectVersion 属性の取得に失敗しました。スキーマ コンテナに属性がないか、提供された資格情報に読み取り権限がありません。
.

MessageId=8512
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INSTALL_NO_SCH_VERSION_IN_INIFILE
Language=English
The replica/child install failed to read the objectVersion attribute in the SCHEMA section of the file schema.ini in the system32 directory.
.
Language=Japanese
レプリカ/チャイルドのインストールで、system32 ディレクトリのファイル schema.ini の SCHEMA セクションにある objectVersion 属性の読み取りに失敗しました。
.

MessageId=8513
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INVALID_GROUP_TYPE
Language=English
The specified group type is invalid.
.
Language=Japanese
指定されたグループ タイプが無効です。
.

MessageId=8514
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_NEST_GLOBALGROUP_IN_MIXEDDOMAIN
Language=English
Cannot nest global groups in a mixed domain if the group is security-enabled.
.
Language=Japanese
グループがセキュリティ有効の場合、混合ドメインにグローバル グループをネストできません。
.

MessageId=8515
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_NEST_LOCALGROUP_IN_MIXEDDOMAIN
Language=English
Cannot nest local groups in a mixed domain if the group is security-enabled.
.
Language=Japanese
グループがセキュリティ対応の場合、混合ドメインにローカルグループをネストできません。
.

MessageId=8516
Severity=Success
Facility=System
SymbolicName=ERROR_DS_GLOBAL_CANT_HAVE_LOCAL_MEMBER
Language=English
A global group cannot have a local group as a member.
.
Language=Japanese
グローバルグループにローカルグループをメンバとして含めることはできません。
.

MessageId=8517
Severity=Success
Facility=System
SymbolicName=ERROR_DS_GLOBAL_CANT_HAVE_UNIVERSAL_MEMBER
Language=English
A global group cannot have a universal group as a member.
.
Language=Japanese
グローバル グループにユニバーサル グループをメンバに持つことはできません。
.

MessageId=8518
Severity=Success
Facility=System
SymbolicName=ERROR_DS_UNIVERSAL_CANT_HAVE_LOCAL_MEMBER
Language=English
A universal group cannot have a local group as a member.
.
Language=Japanese
ユニバーサル グループにローカル グループをメンバに持つことはできません。
.

MessageId=8519
Severity=Success
Facility=System
SymbolicName=ERROR_DS_GLOBAL_CANT_HAVE_CROSSDOMAIN_MEMBER
Language=English
A global group cannot have a cross-domain member.
.
Language=Japanese
グローバルグループにクロスドメインのメンバを持つことはできません。
.

MessageId=8520
Severity=Success
Facility=System
SymbolicName=ERROR_DS_LOCAL_CANT_HAVE_CROSSDOMAIN_LOCAL_MEMBER
Language=English
A local group cannot have another cross-domain local group as a member.
.
Language=Japanese
ローカルグループにクロスドメインのローカルグループをメンバとして持つことはできません。
.

MessageId=8521
Severity=Success
Facility=System
SymbolicName=ERROR_DS_HAVE_PRIMARY_MEMBERS
Language=English
A group with primary members cannot change to a security-disabled group.
.
Language=Japanese
プライマリ メンバを持つグループをセキュリティ無効グループに変更できません。
.

MessageId=8522
Severity=Success
Facility=System
SymbolicName=ERROR_DS_STRING_SD_CONVERSION_FAILED
Language=English
The schema cache load failed to convert the string default SD on a class-schema object.
.
Language=Japanese
グループがセキュリティ無効グループに変更されました。
.

MessageId=8523
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAMING_MASTER_GC
Language=English
Only DSAs configured to be Global Catalog servers should be allowed to hold the Domain Naming Master FSMO role. (Applies only to Windows 2000 servers)
.
Language=Japanese
グローバルカタログサーバーとして構成されたDSAのみがドメインネーミングマスターFSMOロールを保持できます。 (ReliefWin 2000サーバーにのみ適用されます) 。
.

MessageId=8524
Severity=Success
Facility=System
SymbolicName=ERROR_DS_LOOKUP_FAILURE
Language=English
The DSA operation is unable to proceed because of a DNS lookup failure.
.
Language=Japanese
DNS検索に失敗したため、DSA操作を続行できません。
.

MessageId=8525
Severity=Success
Facility=System
SymbolicName=ERROR_DS_COULDNT_UPDATE_SPNS
Language=English
While processing a change to the DNS Host Name for an object, the Service Principal Name values could not be kept in sync.
.
Language=Japanese
オブジェクトのDNSホスト名の変更処理中に、サービス プリンシパル名の値を同期できませんでした。
.

MessageId=8526
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_RETRIEVE_SD
Language=English
The Security Descriptor attribute could not be read.
.
Language=Japanese
セキュリティ記述子属性を読み取れませんでした。
.

MessageId=8527
Severity=Success
Facility=System
SymbolicName=ERROR_DS_KEY_NOT_UNIQUE
Language=English
The object requested was not found, but an object with that key was found.
.
Language=Japanese
要求されたオブジェクトは見つかりませんでしたが、そのキーを持つオブジェクトが見つかりました。
.

MessageId=8528
Severity=Success
Facility=System
SymbolicName=ERROR_DS_WRONG_LINKED_ATT_SYNTAX
Language=English
The syntax of the linked attributed being added is incorrect. Forward links can only have syntax 2.5.5.1, 2.5.5.7, and 2.5.5.14, and backlinks can only have syntax 2.5.5.1.
.
Language=Japanese
追加されるリンク属性の構文が正しくありません。フォワード リンクの構文は2.5.5.1、2.5.5.7、および2.5.5.14のみ、バックリンクの構文は2.5.5.1のみです。
.

MessageId=8529
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SAM_NEED_BOOTKEY_PASSWORD
Language=English
Security Account Manager needs to get the boot password.
.
Language=Japanese
セキュリティ アカウント マネージャでブート パスワードを取得する必要があります。
.

MessageId=8530
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SAM_NEED_BOOTKEY_FLOPPY
Language=English
Security Account Manager needs to get the boot key from floppy disk.
.
Language=Japanese
セキュリティ アカウント マネージャは、フロッピー ディスクからブート キーを取得する必要があります。
.

MessageId=8531
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_START
Language=English
Directory Service cannot start.
.
Language=Japanese
ディレクトリサービスを開始できません。
.

MessageId=8532
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INIT_FAILURE
Language=English
Directory Services could not start.
.
Language=Japanese
ディレクトリサービスを開始できませんでした。
.

MessageId=8533
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_PKT_PRIVACY_ON_CONNECTION
Language=English
The connection between client and server requires packet privacy or better.
.
Language=Japanese
クライアントとサーバー間の接続には、パケット プライバシー以上が必要です。
.

MessageId=8534
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SOURCE_DOMAIN_IN_FOREST
Language=English
The source domain may not be in the same forest as destination.
.
Language=Japanese
送信元ドメインが宛先と同じフォレストにない可能性があります。
.

MessageId=8535
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DESTINATION_DOMAIN_NOT_IN_FOREST
Language=English
The destination domain must be in the forest.
.
Language=Japanese
宛先ドメインがフォレスト内にある必要があります。
.

MessageId=8536
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DESTINATION_AUDITING_NOT_ENABLED
Language=English
The operation requires that destination domain auditing be enabled.
.
Language=Japanese
この操作では、宛先ドメインの監査が有効になっている必要があります。
.

MessageId=8537
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_FIND_DC_FOR_SRC_DOMAIN
Language=English
The operation couldn't locate a DC for the source domain.
.
Language=Japanese
この操作はソースドメインのDCを見つけられませんでした。
.

MessageId=8538
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SRC_OBJ_NOT_GROUP_OR_USER
Language=English
The source object must be a group or user.
.
Language=Japanese
ソース オブジェクトはグループまたはユーザーでなければなりません。
.

MessageId=8539
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SRC_SID_EXISTS_IN_FOREST
Language=English
The source object's SID already exists in destination forest.
.
Language=Japanese
送信元オブジェクトのSIDが宛先フォレストに既に存在します。
.

MessageId=8540
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SRC_AND_DST_OBJECT_CLASS_MISMATCH
Language=English
The source and destination object must be of the same type.
.
Language=Japanese
送信元オブジェクトと宛先オブジェクトは同じタイプでなければなりません。
.

MessageId=8541
Severity=Success
Facility=System
SymbolicName=ERROR_SAM_INIT_FAILURE
Language=English
Security Accounts Manager initialization failed because of the following error: %1.
Error Status: 0x%2. Click OK to shut down the system and reboot into Safe Mode. Check the event log for detailed information.
.
Language=Japanese
セキュリティ アカウント マネージャの初期化は次のエラーにより失敗しました: %1
エラー ステータス: 0x%2
OKをクリックしてシステムをシャットダウンし、セーフモードで再起動します。詳細情報については、イベント ログを確認してください。
.

MessageId=8542
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_SCHEMA_INFO_SHIP
Language=English
Schema information could not be included in the replication request.
.
Language=Japanese
スキーマ情報をレプリケーション要求に含めることができませんでした。
.

MessageId=8543
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_SCHEMA_CONFLICT
Language=English
The replication operation could not be completed due to a schema incompatibility.
.
Language=Japanese
スキーマの互換性がないため、レプリケーション操作を完了できませんでした。
.

MessageId=8544
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_EARLIER_SCHEMA_CONLICT
Language=English
The replication operation could not be completed due to a previous schema incompatibility.
.
Language=Japanese
以前のスキーマの互換性がないため、レプリケーション操作を完了できませんでした。
.

MessageId=8545
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_OBJ_NC_MISMATCH
Language=English
The replication update could not be applied because either the source or the destination has not yet received information regarding a recent cross-domain move operation.
.
Language=Japanese
送信元または送信先のいずれかが最近のクロスドメイン移動操作に関する情報をまだ受信していないため、レプリケーション更新を適用できませんでした。
.

MessageId=8546
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NC_STILL_HAS_DSAS
Language=English
The requested domain could not be deleted because there exist domain controllers that still host this domain.
.
Language=Japanese
まだこのドメインをホストしているドメイン コントローラーが存在するため、要求されたドメインを削除できませんでした。
.

MessageId=8547
Severity=Success
Facility=System
SymbolicName=ERROR_DS_GC_REQUIRED
Language=English
The requested operation can be performed only on a global catalog server.
.
Language=Japanese
要求された操作はグローバル カタログ サーバーでのみ実行できます。
.

MessageId=8548
Severity=Success
Facility=System
SymbolicName=ERROR_DS_LOCAL_MEMBER_OF_LOCAL_ONLY
Language=English
A local group can only be a member of other local groups in the same domain.
.
Language=Japanese
ローカルグループは同じドメイン内の他のローカルグループのメンバにしかなれません。
.

MessageId=8549
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_FPO_IN_UNIVERSAL_GROUPS
Language=English
Foreign security principals cannot be members of universal groups.
.
Language=Japanese
外部セキュリティプリンシパルをユニバーサルグループのメンバにすることはできません。
.

MessageId=8550
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_ADD_TO_GC
Language=English
The attribute is not allowed to be replicated to the GC because of security reasons.
.
Language=Japanese
属性は、セキュリティ上の理由から GC 2011 に複製することはできません。
.

MessageId=8551
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_CHECKPOINT_WITH_PDC
Language=English
The checkpoint with the PDC could not be taken because there are too many modifications being processed currently.
.
Language=Japanese
現在処理中の変更が多すぎるため、PDCとのチェックポイントを取ることができませんでした。
.

MessageId=8552
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SOURCE_AUDITING_NOT_ENABLED
Language=English
The operation requires that source domain auditing be enabled.
.
Language=Japanese
この操作では、ソース ドメインの監査が有効になっている必要があります。
.

MessageId=8553
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_CREATE_IN_NONDOMAIN_NC
Language=English
Security principal objects can only be created inside domain naming contexts.
.
Language=Japanese
セキュリティプリンシパルオブジェクトはドメイン命名コンテキスト内でのみ作成できます。
.

MessageId=8554
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INVALID_NAME_FOR_SPN
Language=English
A Service Principal Name (SPN) could not be constructed because the provided hostname is not in the necessary format.
.
Language=Japanese
指定されたホスト名が必要な形式でないため、サービスプリンシパル名 (SPN) を構築できませんでした。
.

MessageId=8555
Severity=Success
Facility=System
SymbolicName=ERROR_DS_FILTER_USES_CONTRUCTED_ATTRS
Language=English
A Filter was passed that uses constructed attributes.
.
Language=Japanese
構築された属性を使用するフィルタが渡されました。
.

MessageId=8556
Severity=Success
Facility=System
SymbolicName=ERROR_DS_UNICODEPWD_NOT_IN_QUOTES
Language=English
The unicodePwd attribute value must be enclosed in double quotes.
.
Language=Japanese
unicodePwd 属性値は二重引用符で囲む必要があります。
.

MessageId=8557
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MACHINE_ACCOUNT_QUOTA_EXCEEDED
Language=English
Your computer could not be joined to the domain. You have exceeded the maximum number of computer accounts you are allowed to create in this domain. Contact your system administrator to have this limit reset or increased.
.
Language=Japanese
コンピュータをドメインに参加できませんでした。このドメインで作成できるコンピュータアカウントの最大数を超えています。システム管理者に連絡して、この上限をリセットするか、上限を増やしてください。
.

MessageId=8558
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MUST_BE_RUN_ON_DST_DC
Language=English
For security reasons, the operation must be run on the destination DC.
.
Language=Japanese
セキュリティ上の理由から、この操作は宛先DCで実行する必要があります。
.

MessageId=8559
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SRC_DC_MUST_BE_SP4_OR_GREATER
Language=English
For security reasons, the source DC must be NT4SP4 or greater.
.
Language=Japanese
セキュリティ上の理由から、ソースDCはNT4SP4以上でなければなりません。
.

MessageId=8560
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_TREE_DELETE_CRITICAL_OBJ
Language=English
Critical Directory Service System objects cannot be deleted during tree delete operations. The tree delete may have been partially performed.
.
Language=Japanese
ツリーの削除操作中に、重要なディレクトリ サービス システム オブジェクトを削除できません。ツリーの削除が部分的に実行された可能性があります。
.

MessageId=8561
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INIT_FAILURE_CONSOLE
Language=English
Directory Services could not start because of the following error: %1.
Error Status: 0x%2. Please click OK to shutdown the system. You can use the recovery console to diagnose the system further.
.
Language=Japanese
次のエラーが発生したため、ディレクトリ サービスを開始できませんでした: %1
エラー ステータス: 0x%2
[OK] をクリックしてシステムをシャットダウンしてください。回復コンソールを使用してシステムをさらに診断できます。
.

MessageId=8562
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SAM_INIT_FAILURE_CONSOLE
Language=English
Security Accounts Manager initialization failed because of the following error: %1.
Error Status: 0x%2. Please click OK to shutdown the system. You can use the recovery console to diagnose the system further.
.
Language=Japanese
セキュリティ アカウント マネージャの初期化が次のエラーにより失敗しました: %1
エラー ステータス: 0x%2
[OK] をクリックしてシステムをシャットダウンしてください。回復コンソールを使用してシステムをさらに診断できます。
.

MessageId=8563
Severity=Success
Facility=System
SymbolicName=ERROR_DS_FOREST_VERSION_TOO_HIGH
Language=English
The version of the operating system installed is incompatible with the current forest functional level. You must upgrade to a new version of the operating system before this server can become a domain controller in this forest.
.
Language=Japanese
オペレーティング システムのバージョンは現在の AD DS フォレスト機能ルと互換性がありません。
.

MessageId=8564
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DOMAIN_VERSION_TOO_HIGH
Language=English
The version of the operating system installed is incompatible with the current domain functional level. You must upgrade to a new version of the operating system before this server can become a domain controller in this domain.
.
Language=Japanese
インストールされているオペレーティングシステムのバージョンは、現在のドメイン機能レベルと互換性がありません。このサーバーがこのドメインのドメイン コントローラーになる前に、新しいバージョンのオペレーティングシステムにアップグレードする必要があります。
.

MessageId=8565
Severity=Success
Facility=System
SymbolicName=ERROR_DS_FOREST_VERSION_TOO_LOW
Language=English
This version of the operating system installed on this server no longer supports the current forest functional level. You must raise the forest functional level before this server can become a domain controller in this forest.
.
Language=Japanese
このサーバーにインストールされているオペレーティングシステムのバージョンは、現在のAD DSフォレスト機能レベルまたはAD LDS構成セット機能レベルをサポートしなくなりました。このサーバーがこのフォレストまたは構成セットでAD DSドメイン コントローラーまたはAD LDSインスタンスになるには、AD DSフォレスト機能レベルまたはAD LDS構成セット機能レベルを上げる必要があります。
.

MessageId=8566
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DOMAIN_VERSION_TOO_LOW
Language=English
This version of the operating system installed on this server no longer supports the current domain functional level. You must raise the domain functional level before this server can become a domain controller in this domain.
.
Language=Japanese
このサーバーにインストールされているオペレーティング システムのバージョンは、現在のドメイン機能レベルをサポートしていません。このサーバーがこのドメインのドメイン コントローラーになる前に、ドメイン機能レベルを上げる必要があります。
.

MessageId=8567
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INCOMPATIBLE_VERSION
Language=English
The version of the operating system installed on this server is incompatible with the functional level of the domain or forest.
.
Language=Japanese
このサーバーにインストールされているオペレーティング システムのバージョンは、ドメインまたはフォレストの機能レベルと互換性がありません。
.

MessageId=8568
Severity=Success
Facility=System
SymbolicName=ERROR_DS_LOW_DSA_VERSION
Language=English
The functional level of the domain (or forest) cannot be raised to the requested value, because there exist one or more domain controllers in the domain (or forest) that are at a lower incompatible functional level.
.
Language=Japanese
ドメイン (またはフォレスト) の機能レベルは、ドメイン (またはフォレスト) 内に互換性のない下位の機能レベルにある1つまたは複数のドメイン コントローラーが存在するため、要求された値まで上げることができません。
.

MessageId=8569
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_BEHAVIOR_VERSION_IN_MIXEDDOMAIN
Language=English
The forest functional level cannot be raised to the requested level since one or more domains are still in mixed domain mode. All domains in the forest must be in native mode before you can raise the forest functional level.
.
Language=Japanese
1つ以上のドメインが混合ドメインモードのままであるため、フォレスト機能レベルを要求された値まで上げることができません。フォレスト機能レベルを上げるには、フォレスト内のすべてのドメインがネイティブ モードである必要があります。
.

MessageId=8570
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NOT_SUPPORTED_SORT_ORDER
Language=English
The sort order requested is not supported.
.
Language=Japanese
要求されたソート順序はサポートされていません。
.

MessageId=8571
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_NOT_UNIQUE
Language=English
The requested name already exists as a unique identifier.
.
Language=Japanese
要求された名前は一意識別子として既に存在します。
.

MessageId=8572
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MACHINE_ACCOUNT_CREATED_PRENT4
Language=English
The machine account was created pre-NT4. The account needs to be recreated.
.
Language=Japanese
マシン アカウントがNT4以前に作成されました。アカウントを再作成する必要があります。
.

MessageId=8573
Severity=Success
Facility=System
SymbolicName=ERROR_DS_OUT_OF_VERSION_STORE
Language=English
The database is out of version store.
.
Language=Japanese
データベースがバージョンストアから外れています。
.

MessageId=8574
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INCOMPATIBLE_CONTROLS_USED
Language=English
Unable to continue operation because multiple conflicting controls were used.
.
Language=Japanese
複数の競合するコントロールが使用されたため、操作を続行できません。
.

MessageId=8575
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_REF_DOMAIN
Language=English
Unable to find a valid security descriptor reference domain for this partition.
.
Language=Japanese
このパーティションの有効なセキュリティ記述子参照ドメインが見つかりません。
.

MessageId=8576
Severity=Success
Facility=System
SymbolicName=ERROR_DS_RESERVED_LINK_ID
Language=English
Schema update failed: The link identifier is reserved.
.
Language=Japanese
スキーマの更新に失敗しました: リンク識別子は予約されています。
.

MessageId=8577
Severity=Success
Facility=System
SymbolicName=ERROR_DS_LINK_ID_NOT_AVAILABLE
Language=English
Schema update failed: There are no link identifiers available.
.
Language=Japanese
スキーマの更新に失敗しました。
.

MessageId=8578
Severity=Success
Facility=System
SymbolicName=ERROR_DS_AG_CANT_HAVE_UNIVERSAL_MEMBER
Language=English
An account group cannot have a universal group as a member.
.
Language=Japanese
アカウント グループにユニバーサル グループをメンバとして含めることはできません。
.

MessageId=8579
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MODIFYDN_DISALLOWED_BY_INSTANCE_TYPE
Language=English
Rename or move operations on naming context heads or read-only objects are not allowed.
.
Language=Japanese
名前付けコンテキスト ヘッドまたは読み取り専用オブジェクトに対する名前の変更または移動操作は許可されていません。
.

MessageId=8580
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_OBJECT_MOVE_IN_SCHEMA_NC
Language=English
Move operations on objects in the schema naming context are not allowed.
.
Language=Japanese
スキーマ名前コンテキスト内のオブジェクトに対する移動操作は許可されていません。
.

MessageId=8581
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MODIFYDN_DISALLOWED_BY_FLAG
Language=English
A system flag has been set on the object and does not allow the object to be moved or renamed.
.
Language=Japanese
システム フラグがオブジェクトに設定されているため、オブジェクトの移動または名前の変更ができません。
.

MessageId=8582
Severity=Success
Facility=System
SymbolicName=ERROR_DS_MODIFYDN_WRONG_GRANDPARENT
Language=English
This object is not allowed to change its grandparent container. Moves are not forbidden on this object, but are restricted to sibling containers.
.
Language=Japanese
このオブジェクトは祖父母コンテナを変更できません。このオブジェクトでは移動は禁止されていませんが、兄弟コンテナに制限されています。
.

MessageId=8583
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NAME_ERROR_TRUST_REFERRAL
Language=English
Unable to resolve completely, a referral to another forest is generated.
.
Language=Japanese
完全に解決できないため、別のフォレストへの参照が生成されます。
.

MessageId=8584
Severity=Success
Facility=System
SymbolicName=ERROR_NOT_SUPPORTED_ON_STANDARD_SERVER
Language=English
The requested action is not supported on standard server.
.
Language=Japanese
要求されたアクションは標準サーバーでサポートされていません。
.

MessageId=8585
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_ACCESS_REMOTE_PART_OF_AD
Language=English
Could not access a partition of the Active Directory located on a remote server. Make sure at least one server is running for the partition in question.
.
Language=Japanese
リモートサーバーにあるディレクトリサービスのパーティションにアクセスできませんでした。問題のパーティションに対して少なくとも1つのサーバーが実行されていることを確認してください。
.

MessageId=8586
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CR_IMPOSSIBLE_TO_VALIDATE_V2
Language=English
The directory cannot validate the proposed naming context (or partition) name because it does not hold a replica nor can it contact a replica of the naming context above the proposed naming context. Please ensure that the parent naming context is properly registered in DNS, and at least one replica of this naming context is reachable by the Domain Naming master.
.
Language=Japanese
ディレクトリがレプリカを保持していないため、提案されたネーミング コンテキスト (またはパーティション) 名を検証できません。親ネーミング コンテキストがDNSに正しく登録されており、このネーミング コンテキストの少なくとも1つのレプリカがドメイン ネーミング マスターによって到達可能であることを確認してください。
.

MessageId=8587
Severity=Success
Facility=System
SymbolicName=ERROR_DS_THREAD_LIMIT_EXCEEDED
Language=English
The thread limit for this request was exceeded.
.
Language=Japanese
この要求のスレッド制限を超えました。
.

MessageId=8588
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NOT_CLOSEST
Language=English
The Global catalog server is not in the closet site.
.
Language=Japanese
グローバル カタログ サーバーが最も近いサイトにありません。
.

MessageId=8589
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_DERIVE_SPN_WITHOUT_SERVER_REF
Language=English
The DS cannot derive a service principal name (SPN) with which to mutually authenticate the target server because the corresponding server object in the local DS database has no serverReference attribute.
.
Language=Japanese
ローカル DS データベース内の対応するサーバー オブジェクトに serverReference 属性がないため、DS はターゲット サーバーを相互に認証するためのサービス プリンシパル名 (SPN)  を導き出すことができません。
.

MessageId=8590
Severity=Success
Facility=System
SymbolicName=ERROR_DS_SINGLE_USER_MODE_FAILED
Language=English
The Directory Service failed to enter single user mode.
.
Language=Japanese
ディレクトリ サービスがシングル ユーザー モードに移行できませんでした。
.

MessageId=8591
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NTDSCRIPT_SYNTAX_ERROR
Language=English
The Directory Service cannot parse the script because of a syntax error.
.
Language=Japanese
構文エラーにより、ディレクトリ サービスはスクリプトを解析できません。
.

MessageId=8592
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NTDSCRIPT_PROCESS_ERROR
Language=English
The Directory Service cannot process the script because of an error.
.
Language=Japanese
ディレクトリ サービスはエラーによりスクリプトを処理できません。
.

MessageId=8593
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DIFFERENT_REPL_EPOCHS
Language=English
The directory service cannot perform the requested operation because the servers involved are of different replication epochs (which is usually related to a domain rename that is in progress).
.
Language=Japanese
関係するサーバーのレプリケーション エポックが異なるため、ディレクトリ サービスは要求された操作を実行できません (これは通常、進行中のドメイン名の変更に関連しています) 。
.

MessageId=8594
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRS_EXTENSIONS_CHANGED
Language=English
The directory service binding must be renegotiated due to a change in the server extensions information.
.
Language=Japanese
サーバー拡張情報の変更により、ディレクトリサービスのバインディングを再交渉する必要があります。
.

MessageId=8595
Severity=Success
Facility=System
SymbolicName=ERROR_DS_REPLICA_SET_CHANGE_NOT_ALLOWED_ON_DISABLED_CR
Language=English
Operation not allowed on a disabled cross ref.
.
Language=Japanese
無効化されたCross Refに対する操作は許可されていません。
.

MessageId=8596
Severity=Success
Facility=System
SymbolicName=ERROR_DS_NO_MSDS_INTID
Language=English
Schema update failed: No values for msDS-IntId are available.
.
Language=Japanese
スキーマの更新に失敗しました。
.

MessageId=8597
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DUP_MSDS_INTID
Language=English
Schema update failed: Duplicate msDS-IntId. Retry the operation.
.
Language=Japanese
スキーマの更新に失敗しました: msDS-INtId が重複しています。操作を再試行してください。
.

MessageId=8598
Severity=Success
Facility=System
SymbolicName=ERROR_DS_EXISTS_IN_RDNATTID
Language=English
Schema deletion failed: attribute is used in rDNAttID.
.
Language=Japanese
スキーマの削除に失敗しました: 属性がrDNAで使用されています。
.

MessageId=8599
Severity=Success
Facility=System
SymbolicName=ERROR_DS_AUTHORIZATION_FAILED
Language=English
The directory service failed to authorize the request.
.
Language=Japanese
ディレクトリ サービスは要求の認証に失敗しました。
.

MessageId=8600
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INVALID_SCRIPT
Language=English
The Directory Service cannot process the script because it is invalid.
.
Language=Japanese
スクリプトが無効なため、ディレクトリ サービスで処理できません。
.

MessageId=8601
Severity=Success
Facility=System
SymbolicName=ERROR_DS_REMOTE_CROSSREF_OP_FAILED
Language=English
The remote create cross reference operation failed on the Domain Naming Master FSMO. The operation's error is in the extended data.
.
Language=Japanese
ドメインネーミングマスターFSMOでリモートの相互参照の作成操作に失敗しました。操作のエラーは拡張データにあります。
.

MessageId=8602
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CROSS_REF_BUSY
Language=English
A cross reference is in use locally with the same name.
.
Language=Japanese
ローカルで同じ名前の相互参照が使用されています。
.

MessageId=8603
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_DERIVE_SPN_FOR_DELETED_DOMAIN
Language=English
The DS cannot derive a service principal name (SPN) with which to mutually authenticate the target server because the server's domain has been deleted from the forest.
.
Language=Japanese
サーバーのドメインがフォレストから削除されたため、DSがターゲットサーバーを相互認証するためのサービスプリンシパル名 (SPN) を導出できません。
.

MessageId=8604
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_DEMOTE_WITH_WRITEABLE_NC
Language=English
Writeable NCs prevent this DC from demoting.
.
Language=Japanese
書き込み可能なNCにより、このDCは降格できません。
.

MessageId=8605
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DUPLICATE_ID_FOUND
Language=English
The requested object has a non-unique identifier and cannot be retrieved.
.
Language=Japanese
要求されたオブジェクトに一意でない識別子があり、取得できません。
.

MessageId=8606
Severity=Success
Facility=System
SymbolicName=ERROR_DS_INSUFFICIENT_ATTR_TO_CREATE_OBJECT
Language=English
Insufficient attributes were given to create an object. This object may not exist because it may have been deleted and already garbage collected.
.
Language=Japanese
オブジェクトを作成するための属性が不足しています。このオブジェクトは削除され、すでにガベージ コレクトされている可能性があるため、存在しない可能性があります。
.

MessageId=8607
Severity=Success
Facility=System
SymbolicName=ERROR_DS_GROUP_CONVERSION_ERROR
Language=English
The group cannot be converted due to attribute restrictions on the requested group type.
.
Language=Japanese
要求されたグループ タイプの属性制限のため、グループを変換できません。
.

MessageId=8608
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_MOVE_APP_BASIC_GROUP
Language=English
Cross-domain move of non-empty basic application groups is not allowed.
.
Language=Japanese
空でない基本アプリケーション グループのクロスドメイン移動は許可されていません。
.

MessageId=8609
Severity=Success
Facility=System
SymbolicName=ERROR_DS_CANT_MOVE_APP_QUERY_GROUP
Language=English
Cross-domain move on non-empty query based application groups is not allowed.
.
Language=Japanese
空でないクエリベースのアプリケーショングループのクロスドメイン移動は許可されていません。
.

MessageId=8610
Severity=Success
Facility=System
SymbolicName=ERROR_DS_ROLE_NOT_VERIFIED
Language=English
The role owner could not be verified because replication of its partition has not occurred recently.
.
Language=Japanese
ディレクトリ パーティションが少なくとも1つのレプリケーション パートナーと正常にレプリケートされていないため、FSMOロールの所有権を検証できませんでした。
.

MessageId=8611
Severity=Success
Facility=System
SymbolicName=ERROR_DS_WKO_CONTAINER_CANNOT_BE_SPECIAL
Language=English
The target container for a redirection of a well-known object container cannot already be a special container.
.
Language=Japanese
既知のオブジェクト コンテナのリダイレクトのターゲット コンテナは、すでに特殊コンテナであることはできません。
.

MessageId=8612
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DOMAIN_RENAME_IN_PROGRESS
Language=English
The Directory Service cannot perform the requested operation because a domain rename operation is in progress.
.
Language=Japanese
ドメイン名の変更操作が進行中のため、ディレクトリ サービスは要求された操作を実行できません。
.

MessageId=8613
Severity=Success
Facility=System
SymbolicName=ERROR_DS_EXISTING_AD_CHILD_NC
Language=English
The Active Directory detected an Active Directory child partition below the requested new partition name. The Active Directory's partition hierarchy must be created in a top-down method.
.
Language=Japanese
ディレクトリ サービスは要求されたパーティション名の下に子パーティションを検出しました。パーティション階層はトップダウン方式で作成する必要があります。
.

MessageId=8614
Severity=Success
Facility=System
SymbolicName=ERROR_DS_REPL_LIFETIME_EXCEEDED
Language=English
The Active Directory cannot replicate with this server because the time since the last replication with this server has exceeded the tombstone lifetime.
.
Language=Japanese
このサーバーとの最後のレプリケーションからの時間が墓碑銘の有効期間を超えているため、ディレクトリ サービスはこのサーバーとレプリケーションできません。
.

MessageId=8615
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DISALLOWED_IN_SYSTEM_CONTAINER
Language=English
The requested operation is not allowed on an object under the system container.
.
Language=Japanese
要求された操作は、システム コンテナ配下のオブジェクトでは許可されていません。
.

MessageId=8616
Severity=Success
Facility=System
SymbolicName=ERROR_DS_LDAP_SEND_QUEUE_FULL
Language=English
The LDAP servers network send queue has filled up because the client is not processing the results of its requests fast enough. No more requests will be processed until the client catches up. If the client does not catch up then it will be disconnected.
.
Language=Japanese
クライアントが要求の結果を十分に速く処理できないため、LDAPサーバーのネットワーク送信キューがいっぱいになりました。クライアントが追いつくまで、これ以上のリクエストは処理されません。クライアントが追いつかない場合、クライアントは切断されます。
.

MessageId=8617
Severity=Success
Facility=System
SymbolicName=ERROR_DS_DRA_OUT_SCHEDULE_WINDOW
Language=English
The scheduled replication did not take place because the system was too busy to execute the request within the schedule window. The replication queue is overloaded. Consider reducing the number of partners or decreasing the scheduled replication frequency.
.
Language=Japanese
システムがビジー状態でスケジュール ウィンドウ内に要求を実行できなかったため、スケジュールされたレプリケーションが実行されませんでした。レプリケーション キューが過負荷です。パートナーの数を減らすか、スケジュールされたレプリケーションの頻度を減らしてください。
.

MessageId=9001
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_FORMAT_ERROR
Language=English
DNS server unable to interpret format.
.
Language=Japanese
DNSサーバーがフォーマットを解釈できません。
.

MessageId=9002
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_SERVER_FAILURE
Language=English
DNS server failure.
.
Language=Japanese
DNSサーバーの失敗です。
.

MessageId=9003
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_NAME_ERROR
Language=English
DNS name does not exist.
.
Language=Japanese
DNS名が存在しません。
.

MessageId=9004
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_NOT_IMPLEMENTED
Language=English
DNS request not supported by name server.
.
Language=Japanese
DNS リクエストがネームサーバーによってサポートされていません。
.

MessageId=9005
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_REFUSED
Language=English
DNS operation refused.
.
Language=Japanese
DNS 操作が拒否されました。
.

MessageId=9006
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_YXDOMAIN
Language=English
DNS name that ought not exist, does exist.
.
Language=Japanese
存在しないはずのDNS名が存在する。
.

MessageId=9007
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_YXRRSET
Language=English
DNS RR set that ought not exist, does exist.
.
Language=Japanese
存在しないはずのDNS RRセットが存在します。
.

MessageId=9008
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_NXRRSET
Language=English
DNS RR set that ought to exist, does not exist.
.
Language=Japanese
存在しないはずのDNS RRセットが存在しない。
.

MessageId=9009
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_NOTAUTH
Language=English
DNS server not authoritative for zone.
.
Language=Japanese
DNSサーバーがゾーンの権威ではありません。
.

MessageId=9010
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_NOTZONE
Language=English
DNS name in update or prereq is not in zone.
.
Language=Japanese
更新または prereq の DNS 名がゾーンにありません。
.

MessageId=9016
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_BADSIG
Language=English
DNS signature failed to verify.
.
Language=Japanese
DNS署名の検証に失敗しました。
.

MessageId=9017
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_BADKEY
Language=English
DNS bad key.
.
Language=Japanese
DNSキーが不正です。
.

MessageId=9018
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE_BADTIME
Language=English
DNS signature validity expired.
.
Language=Japanese
DNS署名の有効期限が切れました。
.

MessageId=9501
Severity=Success
Facility=System
SymbolicName=DNS_INFO_NO_RECORDS
Language=English
No records found for given DNS query.
.
Language=Japanese
指定されたDNSクエリのレコードが見つかりません。
.

MessageId=9502
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_BAD_PACKET
Language=English
Bad DNS packet.
.
Language=Japanese
不正なDNSパケットです。
.

MessageId=9503
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NO_PACKET
Language=English
No DNS packet.
.
Language=Japanese
DNS パケットがありません。
.

MessageId=9504
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RCODE
Language=English
DNS error, check rcode.
.
Language=Japanese
DNSエラー、rcodeをチェックしてください。
.

MessageId=9505
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_UNSECURE_PACKET
Language=English
Unsecured DNS packet.
.
Language=Japanese
安全でないDNSパケットです。
.

MessageId=9551
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_INVALID_TYPE
Language=English
Invalid DNS type.
.
Language=Japanese
無効なDNSタイプです。
.

MessageId=9552
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_INVALID_IP_ADDRESS
Language=English
Invalid IP address.
.
Language=Japanese
無効なIPアドレスです。
.

MessageId=9553
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_INVALID_PROPERTY
Language=English
Invalid property.
.
Language=Japanese
無効なプロパティです。
.

MessageId=9554
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_TRY_AGAIN_LATER
Language=English
Try DNS operation again later.
.
Language=Japanese
DNS操作を後で再試行してください。
.

MessageId=9555
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NOT_UNIQUE
Language=English
Record for given name and type is not unique.
.
Language=Japanese
指定された名前と型のレコードは一意ではありません。
.

MessageId=9556
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NON_RFC_NAME
Language=English
DNS name does not comply with RFC specifications.
.
Language=Japanese
DNS 名が RFC 仕様に準拠していません。
.

MessageId=9557
Severity=Success
Facility=System
SymbolicName=DNS_STATUS_FQDN
Language=English
DNS name is a fully-qualified DNS name.
.
Language=Japanese
DNS名は完全修飾DNS名です。
.

MessageId=9558
Severity=Success
Facility=System
SymbolicName=DNS_STATUS_DOTTED_NAME
Language=English
DNS name is dotted (multi-label).
.
Language=Japanese
DNS名がドット (マルチラベル) です。
.

MessageId=9559
Severity=Success
Facility=System
SymbolicName=DNS_STATUS_SINGLE_PART_NAME
Language=English
DNS name is a single-part name.
.
Language=Japanese
DNS名がシングルパート名 
 です。
.

MessageId=9560
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_INVALID_NAME_CHAR
Language=English
DSN name contains an invalid character.
.
Language=Japanese
DNS名に無効な文字が含まれています。
.

MessageId=9561
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NUMERIC_NAME
Language=English
DNS name is entirely numeric.
.
Language=Japanese
DNS名がすべて数字です。
.

MessageId=9562
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NOT_ALLOWED_ON_ROOT_SERVER
Language=English
The operation requested is not permitted on a DNS root server.
.
Language=Japanese
要求された操作はDNSルート サーバーで許可されていません。
.

MessageId=9563
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NOT_ALLOWED_UNDER_DELEGATION
Language=English
The record could not be created because this part of the DNS namespace has been delegated to another server.
.
Language=Japanese
DNSネームスペースのこの部分が別のサーバーに委譲されているため、レコードを作成できませんでした。
.

MessageId=9564
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_CANNOT_FIND_ROOT_HINTS
Language=English
The DNS server could not find a set of root hints.
.
Language=Japanese
DNSサーバーがルートヒントのセットを見つけられませんでした。
.

MessageId=9565
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_INCONSISTENT_ROOT_HINTS
Language=English
The DNS server found root hints but they were not consistent across all adapters.
.
Language=Japanese
DNSサーバーがルートヒントを見つけましたが、すべてのアダプターで一貫していませんでした。
.

MessageId=9601
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_ZONE_DOES_NOT_EXIST
Language=English
DNS zone does not exist.
.
Language=Japanese
DNSゾーンが存在しません。
.

MessageId=9602
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NO_ZONE_INFO
Language=English
DNS zone information not available.
.
Language=Japanese
DNSゾーン情報が利用できません。
.

MessageId=9603
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_INVALID_ZONE_OPERATION
Language=English
Invalid operation for DNS zone.
.
Language=Japanese
DNSゾーンの操作が無効です。
.

MessageId=9604
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_ZONE_CONFIGURATION_ERROR
Language=English
Invalid DNS zone configuration.
.
Language=Japanese
無効なDNSゾーン構成です。
.

MessageId=9605
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_ZONE_HAS_NO_SOA_RECORD
Language=English
DNS zone has no start of authority (SOA) record.
.
Language=Japanese
DNSゾーンにSOAレコードがありません。
.

MessageId=9606
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_ZONE_HAS_NO_NS_RECORDS
Language=English
DNS zone has no name server (NS) record.
.
Language=Japanese
DNSゾーンにネームサーバー (NS) レコードがありません。
.

MessageId=9607
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_ZONE_LOCKED
Language=English
DNS zone is locked.
.
Language=Japanese
DNSゾーンがロックされています。
.

MessageId=9608
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_ZONE_CREATION_FAILED
Language=English
DNS zone creation failed.
.
Language=Japanese
DNSゾーンの作成に失敗しました。
.

MessageId=9609
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_ZONE_ALREADY_EXISTS
Language=English
DNS zone already exists.
.
Language=Japanese
DNSゾーンはすでに存在します。
.

MessageId=9610
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_AUTOZONE_ALREADY_EXISTS
Language=English
DNS automatic zone already exists.
.
Language=Japanese
DNS自動ゾーンはすでに存在します。
.

MessageId=9611
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_INVALID_ZONE_TYPE
Language=English
Invalid DNS zone type.
.
Language=Japanese
無効なDNSゾーンタイプです。
.

MessageId=9612
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_SECONDARY_REQUIRES_MASTER_IP
Language=English
Secondary DNS zone requires master IP address.
.
Language=Japanese
セカンダリDNSゾーンにはマスターIPアドレスが必要です。
.

MessageId=9613
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_ZONE_NOT_SECONDARY
Language=English
DNS zone not secondary.
.
Language=Japanese
DNSゾーンはセカンダリではありません。
.

MessageId=9614
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NEED_SECONDARY_ADDRESSES
Language=English
Need secondary IP address.
.
Language=Japanese
セカンダリIPアドレスが必要です。
.

MessageId=9615
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_WINS_INIT_FAILED
Language=English
WINS initialization failed.
.
Language=Japanese
WINSの初期化に失敗しました。
.

MessageId=9616
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NEED_WINS_SERVERS
Language=English
Need WINS servers.
.
Language=Japanese
WINSサーバーが必要です。
.

MessageId=9617
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NBSTAT_INIT_FAILED
Language=English
NBTSTAT initialization call failed.
.
Language=Japanese
NBTSTAT初期化コールに失敗しました。
.

MessageId=9618
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_SOA_DELETE_INVALID
Language=English
Invalid delete of start of authority (SOA)
.
Language=Japanese
権限の開始 (SOA) の無効な削除です。
.

MessageId=9619
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_FORWARDER_ALREADY_EXISTS
Language=English
A conditional forwarding zone already exists for that name.
.
Language=Japanese
その名前に対して条件付き転送ゾーンがすでに存在します。
.

MessageId=9620
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_ZONE_REQUIRES_MASTER_IP
Language=English
This zone must be configured with one or more master DNS server IP addresses.
.
Language=Japanese
このゾーンは1つ以上のマスターDNSサーバーIPアドレスで構成されている必要があります。
.

MessageId=9621
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_ZONE_IS_SHUTDOWN
Language=English
The operation cannot be performed because this zone is shutdown.
.
Language=Japanese
このゾーンはシャットダウンされているため、操作は実行できません。
.

MessageId=9651
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_PRIMARY_REQUIRES_DATAFILE
Language=English
Primary DNS zone requires datafile.
.
Language=Japanese
プライマリDNSゾーンにはデータファイルが必要です。
.

MessageId=9652
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_INVALID_DATAFILE_NAME
Language=English
Invalid datafile name for DNS zone.
.
Language=Japanese
DNSゾーンのデータファイル名が無効です。
.

MessageId=9653
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_DATAFILE_OPEN_FAILURE
Language=English
Failed to open datafile for DNS zone.
.
Language=Japanese
DNSゾーンのデータファイルを開けませんでした。
.

MessageId=9654
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_FILE_WRITEBACK_FAILED
Language=English
Failed to write datafile for DNS zone.
.
Language=Japanese
DNSゾーンのデータファイルの書き込みに失敗しました。
.

MessageId=9655
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_DATAFILE_PARSING
Language=English
Failure while reading datafile for DNS zone.
.
Language=Japanese
DNSゾーンのデータファイルの読み込みに失敗しました
.

MessageId=9701
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RECORD_DOES_NOT_EXIST
Language=English
DNS record does not exist.
.
Language=Japanese
DNSレコードが存在しません。
.

MessageId=9702
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RECORD_FORMAT
Language=English
DNS record format error.
.
Language=Japanese
DNSレコードのフォーマットエラーです。
.

MessageId=9703
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NODE_CREATION_FAILED
Language=English
Node creation failure in DNS.
.
Language=Japanese
DNSのノード作成に失敗しました。
.

MessageId=9704
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_UNKNOWN_RECORD_TYPE
Language=English
Unknown DNS record type.
.
Language=Japanese
不明な DNS レコード タイプです。
.

MessageId=9705
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RECORD_TIMED_OUT
Language=English
DNS record timed out.
.
Language=Japanese
DNSレコードがタイムアウトしました。
.

MessageId=9706
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NAME_NOT_IN_ZONE
Language=English
Name not in DNS zone.
.
Language=Japanese
DNSゾーンに名前がありません。
.

MessageId=9707
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_CNAME_LOOP
Language=English
CNAME loop detected.
.
Language=Japanese
CNAMEループが検出されました。
.

MessageId=9708
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NODE_IS_CNAME
Language=English
Node is a CNAME DNS record.
.
Language=Japanese
ノードはCNAME DNSレコードです。
.

MessageId=9709
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_CNAME_COLLISION
Language=English
A CNAME record already exists for given name.
.
Language=Japanese
指定された名前のCNAMEレコードは既に存在します。
.

MessageId=9710
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RECORD_ONLY_AT_ZONE_ROOT
Language=English
Record only at DNS zone root.
.
Language=Japanese
レコードはDNSゾーンルートにのみ存在します。
.

MessageId=9711
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_RECORD_ALREADY_EXISTS
Language=English
DNS record already exists.
.
Language=Japanese
DNSレコードはすでに存在します。
.

MessageId=9712
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_SECONDARY_DATA
Language=English
Secondary DNS zone data error.
.
Language=Japanese
セカンダリDNSゾーンのデータ エラーです。
.

MessageId=9713
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NO_CREATE_CACHE_DATA
Language=English
Could not create DNS cache data.
.
Language=Japanese
DNSキャッシュデータを作成できませんでした。
.

MessageId=9714
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NAME_DOES_NOT_EXIST
Language=English
DNS name does not exist.
.
Language=Japanese
DNS名が存在しません。
.

MessageId=9715
Severity=Success
Facility=System
SymbolicName=DNS_WARNING_PTR_CREATE_FAILED
Language=English
Could not create pointer (PTR) record.
.
Language=Japanese
ポインタ (PTR) レコードを作成できませんでした。
.

MessageId=9716
Severity=Success
Facility=System
SymbolicName=DNS_WARNING_DOMAIN_UNDELETED
Language=English
DNS domain was undeleted.
.
Language=Japanese
DNSドメインが削除されました。
.

MessageId=9717
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_DS_UNAVAILABLE
Language=English
The directory service is unavailable.
.
Language=Japanese
ディレクトリ サービスは使用できません。
.

MessageId=9718
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_DS_ZONE_ALREADY_EXISTS
Language=English
DNS zone already exists in the directory service.
.
Language=Japanese
DNSゾーンはすでにディレクトリサービスに存在します。
.

MessageId=9719
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NO_BOOTFILE_IF_DS_ZONE
Language=English
DNS server not creating or reading the boot file for the directory service integrated DNS zone.
.
Language=Japanese
DNSサーバーがディレクトリサービス統合DNSゾーンのブートファイルを作成または読み込んでいません。
.

MessageId=9751
Severity=Success
Facility=System
SymbolicName=DNS_INFO_AXFR_COMPLETE
Language=English
DNS AXFR (zone transfer) complete.
.
Language=Japanese
DNS AXFR (ゾーン転送) が完了しました。
.

MessageId=9752
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_AXFR
Language=English
DNS zone transfer failed.
.
Language=Japanese
DNS AXFR (ゾーン転送)  が完了しました: DNSゾーン転送に失敗しました。
.

MessageId=9753
Severity=Success
Facility=System
SymbolicName=DNS_INFO_ADDED_LOCAL_WINS
Language=English
Added local WINS server.
.
Language=Japanese
ローカルWINSサーバーを追加しました。
.

MessageId=9801
Severity=Success
Facility=System
SymbolicName=DNS_STATUS_CONTINUE_NEEDED
Language=English
Secure update call needs to continue update request.
.
Language=Japanese
Secure Updateコールは更新要求を続行する必要があります。
.

MessageId=9851
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NO_TCPIP
Language=English
TCP/IP network protocol not installed.
.
Language=Japanese
TCP/IPネットワークプロトコルがインストールされていません。
.

MessageId=9852
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_NO_DNS_SERVERS
Language=English
No DNS servers configured for local system.
.
Language=Japanese
ローカル システムに構成された DNS サーバーがありません。
.

MessageId=9901
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_DP_DOES_NOT_EXIST
Language=English
The specified directory partition does not exist.
.
Language=Japanese
指定されたディレクトリ パーティションが存在しません。
.

MessageId=9902
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_DP_ALREADY_EXISTS
Language=English
The specified directory partition already exists.
.
Language=Japanese
指定されたディレクトリ パーティションはすでに存在します。
.

MessageId=9903
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_DP_NOT_ENLISTED
Language=English
The DNS server is not enlisted in the specified directory partition.
.
Language=Japanese
この DNS サーバーは指定されたディレクトリ パーティションに登録されていません。
.

MessageId=9904
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_DP_ALREADY_ENLISTED
Language=English
The DNS server is already enlisted in the specified directory partition.
.
Language=Japanese
このDNSサーバーは、指定されたディレクトリ パーティションにすでに登録されています。
.

MessageId=9905
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_DP_NOT_AVAILABLE
Language=English
The directory partition is not available at this time. Please wait a few minutes and try again.
.
Language=Japanese
ディレクトリ パーティションは現在使用できません。数分待ってから再試行してください。
.

MessageId=9906
Severity=Success
Facility=System
SymbolicName=DNS_ERROR_DP_FSMO_ERROR
Language=English
The application directory partition operation failed. The domain controller holding the domain naming master role is down or unable to service the request or is not running Windows Server 2003.
.
Language=Japanese
ドメインネーミングマスターFSMOロールに到達できなかったため、操作は失敗しました。ドメイン ネーミング マスター FSMO ロールを保持するドメイン コントローラーがダウンしているか、要求を処理できないか、ReliefWin Server 2003 以降が実行されていません。
.

MessageId=10004
Severity=Success
Facility=System
SymbolicName=WSAEINTR
Language=English
A blocking operation was interrupted by a call to WSACancelBlockingCall.
.
Language=Japanese
WSACancelBlockingCall の呼び出しによってブロック操作が中断されました。
.

MessageId=10009
Severity=Success
Facility=System
SymbolicName=WSAEBADF
Language=English
The file handle supplied is not valid.
.
Language=Japanese
指定されたファイルハンドルが無効です。
.

MessageId=10013
Severity=Success
Facility=System
SymbolicName=WSAEACCES
Language=English
An attempt was made to access a socket in a way forbidden by its access permissions.
.
Language=Japanese
アクセス許可で禁止されている方法でソケットにアクセスしようとしました。
.

MessageId=10014
Severity=Success
Facility=System
SymbolicName=WSAEFAULT
Language=English
The system detected an invalid pointer address in attempting to use a pointer argument in a call.
.
Language=Japanese
呼び出しでポインタ引数を使用しようとして、システムが無効なポインタアドレスを検出しました。
.

MessageId=10022
Severity=Success
Facility=System
SymbolicName=WSAEINVAL
Language=English
An invalid argument was supplied.
.
Language=Japanese
無効な引数が指定されました。
.

MessageId=10024
Severity=Success
Facility=System
SymbolicName=WSAEMFILE
Language=English
Too many open sockets.
.
Language=Japanese
オープンしているソケットが多すぎます。
.

MessageId=10035
Severity=Success
Facility=System
SymbolicName=WSAEWOULDBLOCK
Language=English
A non-blocking socket operation could not be completed immediately.
.
Language=Japanese
ノンブロッキングソケット操作を直ちに完了できませんでした。
.

MessageId=10036
Severity=Success
Facility=System
SymbolicName=WSAEINPROGRESS
Language=English
A blocking operation is currently executing.
.
Language=Japanese
ブロック操作は現在実行中です。
.

MessageId=10037
Severity=Success
Facility=System
SymbolicName=WSAEALREADY
Language=English
An operation was attempted on a non-blocking socket that already had an operation in progress.
.
Language=Japanese
既に実行中の操作があるノンブロッキングソケットで操作を試みました。
.

MessageId=10038
Severity=Success
Facility=System
SymbolicName=WSAENOTSOCK
Language=English
An operation was attempted on something that is not a socket.
.
Language=Japanese
ソケットではない何かに対して操作を試みました。
.

MessageId=10039
Severity=Success
Facility=System
SymbolicName=WSAEDESTADDRREQ
Language=English
A required address was omitted from an operation on a socket.
.
Language=Japanese
ソケットに対する操作で必要なアドレスが省略されました。
.

MessageId=10040
Severity=Success
Facility=System
SymbolicName=WSAEMSGSIZE
Language=English
A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram into was smaller than the datagram itself.
.
Language=Japanese
データグラムソケットで送信されたメッセージが内部メッセージ バッファまたは他のネットワーク制限よりも大きかったか、データグラムを受信するために使用されたバッファがデータグラム自体よりも小さかったです。
.

MessageId=10041
Severity=Success
Facility=System
SymbolicName=WSAEPROTOTYPE
Language=English
A protocol was specified in the socket function call that does not support the semantics of the socket type requested.
.
Language=Japanese
要求されたソケットタイプのセマンティクスをサポートしていないプロトコルがソケット関数呼び出しに指定されました。
.

MessageId=10042
Severity=Success
Facility=System
SymbolicName=WSAENOPROTOOPT
Language=English
An unknown, invalid, or unsupported option or level was specified in a getsockopt or setsockopt call.
.
Language=Japanese
getsockoptまたはsetsockopt呼び出しで未知の、無効な、またはサポートされていないオプションまたはレベルが指定されました。
.

MessageId=10043
Severity=Success
Facility=System
SymbolicName=WSAEPROTONOSUPPORT
Language=English
The requested protocol has not been configured into the system, or no implementation for it exists.
.
Language=Japanese
要求されたプロトコルがシステムに設定されていないか、そのプロトコルの実装が存在しません。
.

MessageId=10044
Severity=Success
Facility=System
SymbolicName=WSAESOCKTNOSUPPORT
Language=English
The support for the specified socket type does not exist in this address family.
.
Language=Japanese
指定されたソケットタイプのサポートがこのアドレスファミリに存在しない。
.

MessageId=10045
Severity=Success
Facility=System
SymbolicName=WSAEOPNOTSUPP
Language=English
The attempted operation is not supported for the type of object referenced.
.
Language=Japanese
試行された操作は参照されたオブジェクトのタイプではサポートされていません。
.

MessageId=10046
Severity=Success
Facility=System
SymbolicName=WSAEPFNOSUPPORT
Language=English
The protocol family has not been configured into the system or no implementation for it exists.
.
Language=Japanese
プロトコルファミリがシステムに設定されていないか、その実装が存在しません。
.

MessageId=10047
Severity=Success
Facility=System
SymbolicName=WSAEAFNOSUPPORT
Language=English
An address incompatible with the requested protocol was used.
.
Language=Japanese
要求されたプロトコルと互換性のないアドレスが使用されました。
.

MessageId=10048
Severity=Success
Facility=System
SymbolicName=WSAEADDRINUSE
Language=English
Only one usage of each socket address (protocol/network address/port) is normally permitted.
.
Language=Japanese
各ソケットアドレス (プロトコル/ネットワークアドレス/ポート) の使用は通常1回のみ許可されます。
.

MessageId=10049
Severity=Success
Facility=System
SymbolicName=WSAEADDRNOTAVAIL
Language=English
The requested address is not valid in its context.
.
Language=Japanese
要求されたアドレスはそのコンテキストでは無効です。
.

MessageId=10050
Severity=Success
Facility=System
SymbolicName=WSAENETDOWN
Language=English
A socket operation encountered a dead network.
.
Language=Japanese
要求されたアドレスが無効です: ソケット操作がデッドネットワークに遭遇しました。
.

MessageId=10051
Severity=Success
Facility=System
SymbolicName=WSAENETUNREACH
Language=English
A socket operation was attempted to an unreachable network.
.
Language=Japanese
到達不能なネットワークに対してソケット操作を試みました。
.

MessageId=10052
Severity=Success
Facility=System
SymbolicName=WSAENETRESET
Language=English
The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress.
.
Language=Japanese
操作の進行中にキープアライブアクティビティが障害を検出したため、接続が切断されました。
.

MessageId=10053
Severity=Success
Facility=System
SymbolicName=WSAECONNABORTED
Language=English
An established connection was aborted by the software in your host machine.
.
Language=Japanese
確立された接続がホスト マシンのソフトウェアによって中断されました。
.

MessageId=10054
Severity=Success
Facility=System
SymbolicName=WSAECONNRESET
Language=English
An existing connection was forcibly closed by the remote host.
.
Language=Japanese
既存の接続が強制的に閉じられました: 既存の接続がリモートホストによって強制的に閉じられました。
.

MessageId=10055
Severity=Success
Facility=System
SymbolicName=WSAENOBUFS
Language=English
An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full.
.
Language=Japanese
既存の接続がリモートホストによって強制的に閉じられました: システムに十分なバッファ領域がないか、キューが満杯であるため、ソケットに対する操作を実行できませんでした。
.

MessageId=10056
Severity=Success
Facility=System
SymbolicName=WSAEISCONN
Language=English
A connect request was made on an already connected socket.
.
Language=Japanese
既に接続されているソケットに対して接続要求を行いました。
.

MessageId=10057
Severity=Success
Facility=System
SymbolicName=WSAENOTCONN
Language=English
A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) no address was supplied.
.
Language=Japanese
ソケットが接続されておらず、 (sendto 呼び出しでデータグラムソケットに送信する場合)  アドレスが指定されなかったため、データの送受信要求を拒否しました。
.

MessageId=10058
Severity=Success
Facility=System
SymbolicName=WSAESHUTDOWN
Language=English
A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call.
.
Language=Japanese
以前のシャットダウン呼び出しでその方向のソケットが既にシャットダウンされていたため、データの送受信要求が許可されませんでした。
.

MessageId=10059
Severity=Success
Facility=System
SymbolicName=WSAETOOMANYREFS
Language=English
Too many references to some kernel object.
.
Language=Japanese
カーネルオブジェクトへの参照が多すぎます。
.

MessageId=10060
Severity=Success
Facility=System
SymbolicName=WSAETIMEDOUT
Language=English
A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond.
.
Language=Japanese
接続された相手が一定時間経っても適切に応答しないか、接続に失敗したため、または接続ホストが応答しなかったため、接続に失敗しました。
.

MessageId=10061
Severity=Success
Facility=System
SymbolicName=WSAECONNREFUSED
Language=English
No connection could be made because the target machine actively refused it.
.
Language=Japanese
ターゲットマシンがアクティブに拒否したため、接続できませんでした。
.

MessageId=10062
Severity=Success
Facility=System
SymbolicName=WSAELOOP
Language=English
Cannot translate name.
.
Language=Japanese
名前を変換できません。
.

MessageId=10063
Severity=Success
Facility=System
SymbolicName=WSAENAMETOOLONG
Language=English
Name component or name was too long.
.
Language=Japanese
名前のコンポーネントまたは名前が長すぎます。
.

MessageId=10064
Severity=Success
Facility=System
SymbolicName=WSAEHOSTDOWN
Language=English
A socket operation failed because the destination host was down.
.
Language=Japanese
宛先ホストがダウンしていたため、ソケット操作に失敗しました。
.

MessageId=10065
Severity=Success
Facility=System
SymbolicName=WSAEHOSTUNREACH
Language=English
A socket operation was attempted to an unreachable host.
.
Language=Japanese
ソケット操作に失敗しました: 到達不能なホストに対してソケット操作を試行しました。
.

MessageId=10066
Severity=Success
Facility=System
SymbolicName=WSAENOTEMPTY
Language=English
Cannot remove a directory that is not empty.
.
Language=Japanese
空ではないディレクトリを削除できません。
.

MessageId=10067
Severity=Success
Facility=System
SymbolicName=WSAEPROCLIM
Language=English
A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously.
.
Language=Japanese
ReliefWin ソケットの実装では、同時に使用できるアプリケーションの数に制限がある場合があります。
.

MessageId=10068
Severity=Success
Facility=System
SymbolicName=WSAEUSERS
Language=English
Ran out of quota.
.
Language=Japanese
クォータを使い果たしました。
.

MessageId=10069
Severity=Success
Facility=System
SymbolicName=WSAEDQUOT
Language=English
Ran out of disk quota.
.
Language=Japanese
ディスククォータを使い果たしました。
.

MessageId=10070
Severity=Success
Facility=System
SymbolicName=WSAESTALE
Language=English
File handle reference is no longer available.
.
Language=Japanese
ファイル ハンドル参照が使用できなくなりました。
.

MessageId=10071
Severity=Success
Facility=System
SymbolicName=WSAEREMOTE
Language=English
Item is not available locally.
.
Language=Japanese
アイテムはローカルでは使用できません。
.

MessageId=10091
Severity=Success
Facility=System
SymbolicName=WSASYSNOTREADY
Language=English
WSAStartup cannot function at this time because the underlying system it uses to provide network services is currently unavailable.
.
Language=Japanese
WSAStartup がネットワーク サービスを提供するために使用する基礎となるシステムが現在使用できないため、WSAStartup は現在機能できません。
.

MessageId=10092
Severity=Success
Facility=System
SymbolicName=WSAVERNOTSUPPORTED
Language=English
The Windows Sockets version requested is not supported.
.
Language=Japanese
要求された ReliefWin ソケットのバージョンはサポートされていません。
.

MessageId=10093
Severity=Success
Facility=System
SymbolicName=WSANOTINITIALISED
Language=English
Either the application has not called WSAStartup, or WSAStartup failed.
.
Language=Japanese
アプリケーションがWSAStartupを呼び出していないか、WSAStartupが失敗しました。
.

MessageId=10101
Severity=Success
Facility=System
SymbolicName=WSAEDISCON
Language=English
Returned by WSARecv or WSARecvFrom to indicate the remote party has initiated a graceful shutdown sequence.
.
Language=Japanese
WSARecvまたはWSARecvFromがリモート パーティがグレースフル シャットダウン シーケンスを開始したことを示すために返します。
.

MessageId=10102
Severity=Success
Facility=System
SymbolicName=WSAENOMORE
Language=English
No more results can be returned by WSALookupServiceNext.
.
Language=Japanese
WSALookupServiceNext によって返される結果はありません。
.

MessageId=10103
Severity=Success
Facility=System
SymbolicName=WSAECANCELLED
Language=English
A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled.
.
Language=Japanese
この呼び出しの処理中に WSALookupServiceEnd への呼び出しが行われました。この呼び出しはキャンセルされました。
.

MessageId=10104
Severity=Success
Facility=System
SymbolicName=WSAEINVALIDPROCTABLE
Language=English
The procedure call table is invalid.
.
Language=Japanese
プロシージャ呼び出しテーブルが無効です。
.

MessageId=10105
Severity=Success
Facility=System
SymbolicName=WSAEINVALIDPROVIDER
Language=English
The requested service provider is invalid.
.
Language=Japanese
要求されたサービス プロバイダが無効です。
.

MessageId=10106
Severity=Success
Facility=System
SymbolicName=WSAEPROVIDERFAILEDINIT
Language=English
The requested service provider could not be loaded or initialized.
.
Language=Japanese
要求されたサービス プロバイダをロードまたは初期化できませんでした。
.

MessageId=10107
Severity=Success
Facility=System
SymbolicName=WSASYSCALLFAILURE
Language=English
A system call that should never fail has failed.
.
Language=Japanese
システムコールに失敗しました。
.

MessageId=10108
Severity=Success
Facility=System
SymbolicName=WSASERVICE_NOT_FOUND
Language=English
No such service is known. The service cannot be found in the specified name space.
.
Language=Japanese
そのようなサービスはありません。指定された名前空間にサービスが見つかりません。
.

MessageId=10109
Severity=Success
Facility=System
SymbolicName=WSATYPE_NOT_FOUND
Language=English
The specified class was not found.
.
Language=Japanese
指定されたクラスが見つかりません。
.

MessageId=10110
Severity=Success
Facility=System
SymbolicName=WSA_E_NO_MORE
Language=English
No more results can be returned by WSALookupServiceNext.
.
Language=Japanese
WSALookupServiceNext によって返される結果はありません。
.

MessageId=10111
Severity=Success
Facility=System
SymbolicName=WSA_E_CANCELLED
Language=English
A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled.
.
Language=Japanese
この呼び出しの処理中に WSALookupServiceEnd への呼び出しが行われました。この呼び出しはキャンセルされました。
.

MessageId=10112
Severity=Success
Facility=System
SymbolicName=WSAEREFUSED
Language=English
A database query failed because it was actively refused.
.
Language=Japanese
アクティブに拒否されたため、データベース照会に失敗しました。
.

MessageId=11001
Severity=Success
Facility=System
SymbolicName=WSAHOST_NOT_FOUND
Language=English
No such host is known.
.
Language=Japanese
そのようなホストは存在しません。
.

MessageId=11002
Severity=Success
Facility=System
SymbolicName=WSATRY_AGAIN
Language=English
This is usually a temporary error during hostname resolution and means that the local server did not receive a response from an authoritative server.
.
Language=Japanese
これは通常、ホスト名解決中の一時的なエラーで、ローカルサーバーが権威サーバーからの応答を受信しなかったことを意味します。
.

MessageId=11003
Severity=Success
Facility=System
SymbolicName=WSANO_RECOVERY
Language=English
A non-recoverable error occurred during a database lookup.
.
Language=Japanese
データベースの検索中に回復不能なエラーが発生しました。
.

MessageId=11004
Severity=Success
Facility=System
SymbolicName=WSANO_DATA
Language=English
The requested name is valid, but no data of the requested type was found.
.
Language=Japanese
要求された名前は有効ですが、要求された型のデータが見つかりませんでした。
.

MessageId=11005
Severity=Success
Facility=System
SymbolicName=WSA_QOS_RECEIVERS
Language=English
At least one reserve has arrived.
.
Language=Japanese
少なくとも1つのリザーブが到着しました。
.

MessageId=11006
Severity=Success
Facility=System
SymbolicName=WSA_QOS_SENDERS
Language=English
At least one path has arrived.
.
Language=Japanese
少なくとも1つのパスが到着しました。
.

MessageId=11007
Severity=Success
Facility=System
SymbolicName=WSA_QOS_NO_SENDERS
Language=English
There are no senders.
.
Language=Japanese
送信者がありません。
.

MessageId=11008
Severity=Success
Facility=System
SymbolicName=WSA_QOS_NO_RECEIVERS
Language=English
There are no receivers.
.
Language=Japanese
受信者がありません。
.

MessageId=11009
Severity=Success
Facility=System
SymbolicName=WSA_QOS_REQUEST_CONFIRMED
Language=English
Reserve has been confirmed.
.
Language=Japanese
リザーブが確認されました。
.

MessageId=11010
Severity=Success
Facility=System
SymbolicName=WSA_QOS_ADMISSION_FAILURE
Language=English
Error due to lack of resources.
.
Language=Japanese
リソース不足によるエラーです。
.

MessageId=11011
Severity=Success
Facility=System
SymbolicName=WSA_QOS_POLICY_FAILURE
Language=English
Rejected for administrative reasons - bad credentials.
.
Language=Japanese
管理上の理由で拒否されました - 不正な資格情報です。
.

MessageId=11012
Severity=Success
Facility=System
SymbolicName=WSA_QOS_BAD_STYLE
Language=English
Unknown or conflicting style.
.
Language=Japanese
不明なスタイルまたは競合するスタイルです。
.

MessageId=11013
Severity=Success
Facility=System
SymbolicName=WSA_QOS_BAD_OBJECT
Language=English
Problem with some part of the filterspec or providerspecific buffer in general.
.
Language=Japanese
フィルタスペックまたはプロバイダ固有のバッファの一部に問題があります。
.

MessageId=11014
Severity=Success
Facility=System
SymbolicName=WSA_QOS_TRAFFIC_CTRL_ERROR
Language=English
Problem with some part of the flowspec.
.
Language=Japanese
flowpecの一部に問題があります。
.

MessageId=11015
Severity=Success
Facility=System
SymbolicName=WSA_QOS_GENERIC_ERROR
Language=English
General QOS error.
.
Language=Japanese
一般的なQOSエラーです。
.

MessageId=11016
Severity=Success
Facility=System
SymbolicName=WSA_QOS_ESERVICETYPE
Language=English
An invalid or unrecognized service type was found in the flowspec.
.
Language=Japanese
フロー仕様で無効または認識できないサービスタイプが見つかりました。
.

MessageId=11017
Severity=Success
Facility=System
SymbolicName=WSA_QOS_EFLOWSPEC
Language=English
An invalid or inconsistent flowspec was found in the QOS structure.
.
Language=Japanese
QOS構造に無効な、または一貫性のないフロー仕様が見つかりました。
.

MessageId=11018
Severity=Success
Facility=System
SymbolicName=WSA_QOS_EPROVSPECBUF
Language=English
Invalid QOS provider-specific buffer.
.
Language=Japanese
無効なQOSプロバイダ固有バッファです。
.

MessageId=11019
Severity=Success
Facility=System
SymbolicName=WSA_QOS_EFILTERSTYLE
Language=English
An invalid QOS filter style was used.
.
Language=Japanese
無効なQOSフィルタ スタイルが使用されました。
.

MessageId=11020
Severity=Success
Facility=System
SymbolicName=WSA_QOS_EFILTERTYPE
Language=English
An invalid QOS filter type was used.
.
Language=Japanese
無効なQOSフィルタ タイプが使用されました。
.

MessageId=11021
Severity=Success
Facility=System
SymbolicName=WSA_QOS_EFILTERCOUNT
Language=English
An incorrect number of QOS FILTERSPECs were specified in the FLOWDESCRIPTOR.
.
Language=Japanese
FLOWDESCRIPTORに不正な数のQOS FILTERSPECが指定されました。
.

MessageId=11022
Severity=Success
Facility=System
SymbolicName=WSA_QOS_EOBJLENGTH
Language=English
An object with an invalid ObjectLength field was specified in the QOS provider-specific buffer.
.
Language=Japanese
無効なObjectLengthフィールドを持つオブジェクトがQOSプロバイダ固有バッファに指定されました。
.

MessageId=11023
Severity=Success
Facility=System
SymbolicName=WSA_QOS_EFLOWCOUNT
Language=English
An incorrect number of flow descriptors was specified in the QOS structure.
.
Language=Japanese
QOS構造体に不正な数のフロー記述子が指定されました。
.

MessageId=11024
Severity=Success
Facility=System
SymbolicName=WSA_QOS_EUNKNOWNPSOBJ
Language=English
An unrecognized object was found in the QOS provider-specific buffer.
.
Language=Japanese
QOSプロバイダ固有バッファで認識できないオブジェクトが見つかりました。
.

MessageId=11025
Severity=Success
Facility=System
SymbolicName=WSA_QOS_EPOLICYOBJ
Language=English
An invalid policy object was found in the QOS provider-specific buffer.
.
Language=Japanese
QOSプロバイダ固有のバッファで無効なポリシーオブジェクトが見つかりました。
.

MessageId=11026
Severity=Success
Facility=System
SymbolicName=WSA_QOS_EFLOWDESC
Language=English
An invalid QOS flow descriptor was found in the flow descriptor list.
.
Language=Japanese
フロー記述子リストで無効なQOSフロー記述子が見つかりました。
.

MessageId=11027
Severity=Success
Facility=System
SymbolicName=WSA_QOS_EPSFLOWSPEC
Language=English
An invalid or inconsistent flowspec was found in the QOS provider-specific buffer.
.
Language=Japanese
QOSプロバイダ固有のバッファに無効な、または一貫性のないフロー仕様が見つかりました。
.

MessageId=11028
Severity=Success
Facility=System
SymbolicName=WSA_QOS_EPSFILTERSPEC
Language=English
An invalid FILTERSPEC was found in the QOS provider-specific buffer.
.
Language=Japanese
QOSプロバイダ固有のバッファで無効なFILTERSPECが見つかりました。
.

MessageId=11029
Severity=Success
Facility=System
SymbolicName=WSA_QOS_ESDMODEOBJ
Language=English
An invalid shape discard mode object was found in the QOS provider-specific buffer.
.
Language=Japanese
無効な形状破棄モードオブジェクトがQOSプロバイダ固有のバッファで見つかりました。
.

MessageId=11030
Severity=Success
Facility=System
SymbolicName=WSA_QOS_ESHAPERATEOBJ
Language=English
An invalid shaping rate object was found in the QOS provider-specific buffer.
.
Language=Japanese
無効なシェーピングレートオブジェクトがQOSプロバイダ固有のバッファで見つかりました。
.

MessageId=11031
Severity=Success
Facility=System
SymbolicName=WSA_QOS_RESERVED_PETYPE
Language=English
A reserved policy element was found in the QOS provider-specific buffer.
.
Language=Japanese
QOSプロバイダ固有のバッファで予約されたポリシー要素が見つかりました。
.

MessageId=13000
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_QM_POLICY_EXISTS
Language=English
The specified quick mode policy already exists.
.
Language=Japanese
指定されたクイック モード ポリシーは既に存在します。
.

MessageId=13001
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_QM_POLICY_NOT_FOUND
Language=English
The specified quick mode policy was not found.
.
Language=Japanese
指定されたクイック モード ポリシーが見つかりません。
.

MessageId=13002
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_QM_POLICY_IN_USE
Language=English
The specified quick mode policy is being used.
.
Language=Japanese
指定されたクイック モード ポリシーが使用されています。
.

MessageId=13003
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_MM_POLICY_EXISTS
Language=English
The specified main mode policy already exists.
.
Language=Japanese
指定されたメイン モード ポリシーは既に存在します。
.

MessageId=13004
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_MM_POLICY_NOT_FOUND
Language=English
The specified main mode policy was not found.
.
Language=Japanese
指定されたメイン モード ポリシーが見つかりません。
.

MessageId=13005
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_MM_POLICY_IN_USE
Language=English
The specified main mode policy is being used.
.
Language=Japanese
指定されたメインモード ポリシーが使用されています。
.

MessageId=13006
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_MM_FILTER_EXISTS
Language=English
The specified main mode filter already exists.
.
Language=Japanese
指定されたメインモード フィルタは既に存在します。
.

MessageId=13007
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_MM_FILTER_NOT_FOUND
Language=English
The specified main mode filter was not found.
.
Language=Japanese
指定されたメイン モード フィルタが見つかりません。
.

MessageId=13008
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_TRANSPORT_FILTER_EXISTS
Language=English
The specified transport mode filter already exists.
.
Language=Japanese
指定されたトランスポートモード フィルタは既に存在します。
.

MessageId=13009
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_TRANSPORT_FILTER_NOT_FOUND
Language=English
The specified transport mode filter does not exist.
.
Language=Japanese
指定されたトランスポートモード フィルタは存在しません。
.

MessageId=13010
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_MM_AUTH_EXISTS
Language=English
The specified main mode authentication list exists.
.
Language=Japanese
指定されたメインモード認証リストが存在します。
.

MessageId=13011
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_MM_AUTH_NOT_FOUND
Language=English
The specified main mode authentication list was not found.
.
Language=Japanese
指定されたメインモード認証リストが見つかりません。
.

MessageId=13012
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_MM_AUTH_IN_USE
Language=English
The specified quick mode policy is being used.
.
Language=Japanese
指定されたメインモード認証リストが使用されています。
.

MessageId=13013
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_DEFAULT_MM_POLICY_NOT_FOUND
Language=English
The specified main mode policy was not found.
.
Language=Japanese
指定されたデフォルトのメインモード ポリシーが見つかりません。
.

MessageId=13014
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_DEFAULT_MM_AUTH_NOT_FOUND
Language=English
The specified quick mode policy was not found.
.
Language=Japanese
指定されたデフォルトのメインモード認証リストが見つかりません。
.

MessageId=13015
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_DEFAULT_QM_POLICY_NOT_FOUND
Language=English
The manifest file contains one or more syntax errors.
.
Language=Japanese
指定されたデフォルトのクイック モード ポリシーが見つかりません。
.

MessageId=13016
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_TUNNEL_FILTER_EXISTS
Language=English
The application attempted to activate a disabled activation context.
.
Language=Japanese
指定されたトンネル モード フィルタが存在します。
.

MessageId=13017
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_TUNNEL_FILTER_NOT_FOUND
Language=English
The requested lookup key was not found in any active activation context.
.
Language=Japanese
指定されたトンネルモード フィルタが見つかりません。
.

MessageId=13018
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_MM_FILTER_PENDING_DELETION
Language=English
The Main Mode filter is pending deletion.
.
Language=Japanese
メインモード フィルタは削除待ちです。
.

MessageId=13019
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_TRANSPORT_FILTER_PENDING_DELETION
Language=English
The transport filter is pending deletion.
.
Language=Japanese
トランスポートフィルタは削除待ちです。
.

MessageId=13020
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_TUNNEL_FILTER_PENDING_DELETION
Language=English
The tunnel filter is pending deletion.
.
Language=Japanese
トンネルフィルタが削除待ちです。
.

MessageId=13021
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_MM_POLICY_PENDING_DELETION
Language=English
The Main Mode policy is pending deletion.
.
Language=Japanese
メインモード ポリシーが削除待ちです。
.

MessageId=13022
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_MM_AUTH_PENDING_DELETION
Language=English
The Main Mode authentication bundle is pending deletion.
.
Language=Japanese
メインモード認証バンドルが削除待ちです。
.

MessageId=13023
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_QM_POLICY_PENDING_DELETION
Language=English
The Quick Mode policy is pending deletion.
.
Language=Japanese
クイック モード ポリシーが削除待ちです。
.

MessageId=13024
Severity=Success
Facility=System
SymbolicName=WARNING_IPSEC_MM_POLICY_PRUNED
Language=English
The Main Mode policy was successfully added, but some of the requested offers are not supported.
.
Language=Japanese
メイン モード ポリシーが正常に追加されましたが、要求されたオファーの一部がサポートされていません。
.

MessageId=13025
Severity=Success
Facility=System
SymbolicName=WARNING_IPSEC_QM_POLICY_PRUNED
Language=English
The Quick Mode policy was successfully added, but some of the requested offers are not supported.
.
Language=Japanese
クイック モード ポリシーが正常に追加されましたが、要求されたオファーの一部がサポートされていません。
.

MessageId=13801
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_AUTH_FAIL
Language=English
IKE authentication credentials are unacceptable.
.
Language=Japanese
IKE認証資格情報が受け入れられません。
.

MessageId=13802
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_ATTRIB_FAIL
Language=English
IKE security attributes are unacceptable.
.
Language=Japanese
IKEセキュリティ属性が受け入れられません。
.

MessageId=13803
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_NEGOTIATION_PENDING
Language=English
IKE Negotiation in progress.
.
Language=Japanese
IKE ネゴシエーションが進行中です。
.

MessageId=13804
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_GENERAL_PROCESSING_ERROR
Language=English
General processing error.
.
Language=Japanese
一般処理エラーです。
.

MessageId=13805
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_TIMED_OUT
Language=English
Negotiation timed out.
.
Language=Japanese
ネゴシエーションがタイムアウトしました
.

MessageId=13806
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_NO_CERT
Language=English
IKE failed to find valid machine certificate.
.
Language=Japanese
IKEが有効なマシン証明書を見つけられませんでした。
.

MessageId=13807
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_SA_DELETED
Language=English
IKE SA deleted by peer before establishment completed.
.
Language=Japanese
確立完了前にピアによってIKE SAが削除されました。
.

MessageId=13808
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_SA_REAPED
Language=English
IKE SA deleted before establishment completed.
.
Language=Japanese
確立完了前にIKE SAが削除されました。
.

MessageId=13809
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_MM_ACQUIRE_DROP
Language=English
Negotiation request sat in Queue too long.
.
Language=Japanese
ネゴシエーション要求がキューに長すぎます。
.

MessageId=13810
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_QM_ACQUIRE_DROP
Language=English
Negotiation request sat in Queue too long.
.
Language=Japanese
ネゴシエーションリクエストがキューに長すぎます。
.

MessageId=13811
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_QUEUE_DROP_MM
Language=English
Negotiation request sat in Queue too long.
.
Language=Japanese
キューに入れられた交渉要求が長すぎます。
.

MessageId=13812
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_QUEUE_DROP_NO_MM
Language=English
Negotiation request sat in Queue too long.
.
Language=Japanese
キューに入れられた交渉要求が長すぎます。
.

MessageId=13813
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_DROP_NO_RESPONSE
Language=English
No response from peer.
.
Language=Japanese
相手からの応答がありません。
.

MessageId=13814
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_MM_DELAY_DROP
Language=English
Negotiation took too long.
.
Language=Japanese
ネゴシエーションに時間がかかりすぎました。
.

MessageId=13815
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_QM_DELAY_DROP
Language=English
Negotiation took too long.
.
Language=Japanese
ネゴシエーションに時間がかかりすぎました。
.

MessageId=13816
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_ERROR
Language=English
Unknown error occurred.
.
Language=Japanese
不明なエラーが発生しました。
.

MessageId=13817
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_CRL_FAILED
Language=English
Certificate Revocation Check failed.
.
Language=Japanese
証明書の失効チェックに失敗しました。
.

MessageId=13818
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_KEY_USAGE
Language=English
Invalid certificate key usage.
.
Language=Japanese
無効な証明書キーの使用です。
.

MessageId=13819
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_CERT_TYPE
Language=English
Invalid certificate type.
.
Language=Japanese
証明書のタイプが無効です。
.

MessageId=13820
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_NO_PRIVATE_KEY
Language=English
No private key associated with machine certificate.
.
Language=Japanese
使用されているマシン証明書に秘密鍵がないため、IKEネゴシエーションに失敗しました。IPsec証明書には秘密鍵が必要です。プライベートキーを持つ証明書への置き換えについては、ネットワークセキュリティ管理者にお問い合わせください。
.

MessageId=13822
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_DH_FAIL
Language=English
Failure in Diffie-Hellman computation.
.
Language=Japanese
Diffie-Hellmanの計算に失敗しました。
.

MessageId=13824
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_HEADER
Language=English
Invalid header.
.
Language=Japanese
無効なヘッダーです。
.

MessageId=13825
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_NO_POLICY
Language=English
No policy configured.
.
Language=Japanese
ポリシーが設定されていません。
.

MessageId=13826
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_SIGNATURE
Language=English
Failed to verify signature.
.
Language=Japanese
署名の検証に失敗しました。
.

MessageId=13827
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_KERBEROS_ERROR
Language=English
Failed to authenticate using Kerberos.
.
Language=Japanese
Kerberosを使用した認証に失敗しました。
.

MessageId=13828
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_NO_PUBLIC_KEY
Language=English
Peer's certificate did not have a public key.
.
Language=Japanese
Peerの証明書に公開鍵がありませんでした。
.

MessageId=13829
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR
Language=English
Error processing error payload.
.
Language=Japanese
エラーペイロードの処理エラーです。
.

MessageId=13830
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_SA
Language=English
Error processing SA payload.
.
Language=Japanese
SAペイロードの処理エラーです。
.

MessageId=13831
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_PROP
Language=English
Error processing Proposal payload.
.
Language=Japanese
Proposalペイロードの処理エラーです。
.

MessageId=13832
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_TRANS
Language=English
Error processing Transform payload.
.
Language=Japanese
Transformペイロードの処理エラーです。
.

MessageId=13833
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_KE
Language=English
Error processing KE payload.
.
Language=Japanese
KE ペイロードの処理エラーです。
.

MessageId=13834
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_ID
Language=English
Error processing ID payload.
.
Language=Japanese
IDペイロードの処理エラーです。
.

MessageId=13835
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_CERT
Language=English
Error processing Cert payload.
.
Language=Japanese
Cert ペイロードの処理エラーです。
.

MessageId=13836
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_CERT_REQ
Language=English
Error processing Certificate Request payload.
.
Language=Japanese
証明書要求ペイロードの処理エラーです。
.

MessageId=13837
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_HASH
Language=English
Error processing Hash payload.
.
Language=Japanese
ハッシュペイロードの処理エラーです。
.

MessageId=13838
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_SIG
Language=English
Error processing Signature payload.
.
Language=Japanese
署名ペイロードの処理エラーです。
.

MessageId=13839
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_NONCE
Language=English
Error processing Nonce payload.
.
Language=Japanese
Nonceペイロードの処理エラーです。
.

MessageId=13840
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_NOTIFY
Language=English
Error processing Notify payload.
.
Language=Japanese
通知ペイロードの処理エラーです。
.

MessageId=13841
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_DELETE
Language=English
Error processing Delete Payload.
.
Language=Japanese
削除ペイロードの処理エラーです。
.

MessageId=13842
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PROCESS_ERR_VENDOR
Language=English
Error processing VendorId payload.
.
Language=Japanese
VendorId ペイロードの処理エラーです。
.

MessageId=13843
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_PAYLOAD
Language=English
Invalid payload received.
.
Language=Japanese
無効なペイロードを受信しました。
.

MessageId=13844
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_LOAD_SOFT_SA
Language=English
Soft SA loaded.
.
Language=Japanese
ソフトSAをロードしました。
.

MessageId=13845
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_SOFT_SA_TORN_DOWN
Language=English
Soft SA torn down.
.
Language=Japanese
ソフト SA を破棄しました。
.

MessageId=13846
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_COOKIE
Language=English
Invalid cookie received.
.
Language=Japanese
無効なクッキーを受信しました。
.

MessageId=13847
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_NO_PEER_CERT
Language=English
Peer failed to send valid machine certificate.
.
Language=Japanese
ピアが有効なマシン証明書を送信できませんでした。
.

MessageId=13848
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_PEER_CRL_FAILED
Language=English
Certification Revocation check of peer's certificate failed.
.
Language=Japanese
ピア証明書の証明書失効チェックに失敗しました。
.

MessageId=13849
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_POLICY_CHANGE
Language=English
New policy invalidated SAs formed with old policy.
.
Language=Japanese
新しいポリシーは古いポリシーで形成されたSAを無効にしました。
.

MessageId=13850
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_NO_MM_POLICY
Language=English
There is no available Main Mode IKE policy.
.
Language=Japanese
利用可能なメインモードIKEポリシーがありません。
.

MessageId=13851
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_NOTCBPRIV
Language=English
Failed to enabled TCB privilege.
.
Language=Japanese
TCB特権の有効化に失敗しました。
.

MessageId=13852
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_SECLOADFAIL
Language=English
Failed to load SECURITY.DLL.
.
Language=Japanese
SECURITY.DLLのロードに失敗しました。
.

MessageId=13853
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_FAILSSPINIT
Language=English
Failed to obtain security function table dispatch address from SSPI.
.
Language=Japanese
SSPI からセキュリティ関数テーブルのディスパッチアドレスを取得できませんでした。
.

MessageId=13854
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_FAILQUERYSSP
Language=English
Failed to query Kerberos package to obtain max token size.
.
Language=Japanese
トークンの最大サイズを取得するための Kerberos パッケージへのクエリに失敗しました。
.

MessageId=13855
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_SRVACQFAIL
Language=English
Failed to obtain Kerberos server credentials for ISAKMP/ERROR_IPSEC_IKE service. Kerberos authentication will not function. The most likely reason for this is lack of domain membership. This is normal if your computer is a member of a workgroup.
.
Language=Japanese
ISAKMP/ERROR_IPSEC_IKEサービスのKerberosサーバー認証情報の取得に失敗しました。Kerberos認証は機能しません。最も考えられる原因は、ドメインメンバーシップがないことです。コンピュータがワークグループのメンバーであれば、これは正常です。
.

MessageId=13856
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_SRVQUERYCRED
Language=English
Failed to determine SSPI principal name for ISAKMP/ERROR_IPSEC_IKE service (QueryCredentialsAttributes).
.
Language=Japanese
ISAKMP/ERROR_IPSEC_IKEサービス (QueryCredentialsAttributes) のSSPIプリンシパル名の決定に失敗しました。
.

MessageId=13857
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_GETSPIFAIL
Language=English
Failed to obtain new SPI for the inbound SA from IPSec driver. The most common cause for this is that the driver does not have the correct filter. Check your policy to verify the filters.
.
Language=Japanese
IPsec ドライバから受信 SA の新しい SPI を取得できませんでした。この最も一般的な原因は、ドライバが正しいフィルタを持っていないことです。フィルタを確認するためにポリシーをチェックしてください。
.

MessageId=13858
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_FILTER
Language=English
Given filter is invalid.
.
Language=Japanese
指定されたフィルタは無効です。
.

MessageId=13859
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_OUT_OF_MEMORY
Language=English
Memory allocation failed.
.
Language=Japanese
メモリの割り当てに失敗しました。
.

MessageId=13860
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_ADD_UPDATE_KEY_FAILED
Language=English
Failed to add Security Association to IPSec Driver. The most common cause for this is if the IKE negotiation took too long to complete. If the problem persists, reduce the load on the faulting machine.
.
Language=Japanese
IPsec Driver へのセキュリティアソシエーションの追加に失敗しました。この問題の最も一般的な原因は、IKEネゴシエーションの完了に時間がかかりすぎた場合です。問題が解決しない場合は、障害が発生しているマシンの負荷を下げてください。
.

MessageId=13861
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_POLICY
Language=English
Invalid policy.
.
Language=Japanese
無効なポリシーです。
.

MessageId=13862
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_UNKNOWN_DOI
Language=English
Invalid DOI.
.
Language=Japanese
無効なDOIです。
.

MessageId=13863
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_SITUATION
Language=English
Invalid situation.
.
Language=Japanese
無効な状況です。
.

MessageId=13864
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_DH_FAILURE
Language=English
Diffie-Hellman failure.
.
Language=Japanese
Diffie-Hellmanの失敗です。
.

MessageId=13865
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_GROUP
Language=English
Invalid Diffie-Hellman group.
.
Language=Japanese
無効なDiffie-Hellmanグループです。
.

MessageId=13866
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_ENCRYPT
Language=English
Error encrypting payload.
.
Language=Japanese
ペイロードの暗号化エラーです。
.

MessageId=13867
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_DECRYPT
Language=English
Error decrypting payload.
.
Language=Japanese
ペイロードの復号化エラーです。
.

MessageId=13868
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_POLICY_MATCH
Language=English
Policy match error.
.
Language=Japanese
ポリシー一致エラーです。
.

MessageId=13869
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_UNSUPPORTED_ID
Language=English
Unsupported ID.
.
Language=Japanese
サポートされていないID。
.

MessageId=13870
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_HASH
Language=English
Hash verification failed.
.
Language=Japanese
ハッシュ検証に失敗しました。
.

MessageId=13871
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_HASH_ALG
Language=English
Invalid hash algorithm.
.
Language=Japanese
無効なハッシュアルゴリズムです。
.

MessageId=13872
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_HASH_SIZE
Language=English
Invalid hash size.
.
Language=Japanese
無効なハッシュサイズです。
.

MessageId=13873
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_ENCRYPT_ALG
Language=English
Invalid encryption algorithm.
.
Language=Japanese
無効な暗号化アルゴリズムです。
.

MessageId=13874
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_AUTH_ALG
Language=English
Invalid authentication algorithm.
.
Language=Japanese
無効な認証アルゴリズムです。
.

MessageId=13875
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_SIG
Language=English
Invalid certificate signature.
.
Language=Japanese
無効な証明書署名です。
.

MessageId=13876
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_LOAD_FAILED
Language=English
Load failed.
.
Language=Japanese
ロードに失敗しました。
.

MessageId=13877
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_RPC_DELETE
Language=English
Deleted via RPC call.
.
Language=Japanese
RPCコールで削除されました。
.

MessageId=13878
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_BENIGN_REINIT
Language=English
Temporary state created to perform reinit. This is not a real failure.
.
Language=Japanese
再初期化を実行するために一時的な状態が作成されました。これは本当の失敗ではありません。
.

MessageId=13879
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_RESPONDER_LIFETIME_NOTIFY
Language=English
The lifetime value received in the Responder Lifetime Notify is below the Windows 2000 configured minimum value. Please fix the policy on the peer machine.
.
Language=Japanese
レスポンダライフタイム通知で受信したライフタイム値が、ReliefWin 2000で設定されている最小値を下回っています。対向マシンのポリシーを修正してください。
.

MessageId=13881
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_INVALID_CERT_KEYLEN
Language=English
Key length in certificate is too small for configured security requirements.
.
Language=Japanese
証明書の鍵の長さが、構成されたセキュリティ要件に対して小さすぎます。
.

MessageId=13882
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_MM_LIMIT
Language=English
Max number of established MM SAs to peer exceeded.
.
Language=Japanese
ピアに対して確立されたMM SAの最大数を超えました。
.

MessageId=13883
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_NEGOTIATION_DISABLED
Language=English
IKE received a policy that disables negotiation.
.
Language=Japanese
IKEはネゴシエーションを無効にするポリシーを受信しました。
.

MessageId=13884
Severity=Success
Facility=System
SymbolicName=ERROR_IPSEC_IKE_NEG_STATUS_END
Language=English
ERROR_IPSEC_IKE_NEG_STATUS_END
.
Language=Japanese
メインモードのクイックモードの上限に達しました。新しいメインモードが開始されます。
.

MessageId=14000
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_SECTION_NOT_FOUND
Language=English
The requested section was not present in the activation context.
.
Language=Japanese
アクティブ化コンテキストに要求されたセクションがありません。
.

MessageId=14001
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_CANT_GEN_ACTCTX
Language=English
This application has failed to start because the application configuration is incorrect. Reinstalling the application may fix this problem.
.
Language=Japanese
サイドバイサイド構成が正しくないため、アプリケーションの起動に失敗しました。詳細については、アプリケーションのイベントログを参照するか、コマンドラインのsxstrace.exeツールを使用してください。
.

MessageId=14002
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_INVALID_ACTCTXDATA_FORMAT
Language=English
The application binding data format is invalid.
.
Language=Japanese
アプリケーション バインディング データ形式が無効です。
.

MessageId=14003
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_ASSEMBLY_NOT_FOUND
Language=English
The referenced assembly is not installed on your system.
.
Language=Japanese
参照されたアセンブリがシステムにインストールされていません。
.

MessageId=14004
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_MANIFEST_FORMAT_ERROR
Language=English
The manifest file does not begin with the required tag and format information.
.
Language=Japanese
マニフェスト ファイルが必要なタグおよびフォーマット情報で始まっていません。
.

MessageId=14005
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_MANIFEST_PARSE_ERROR
Language=English
The manifest file contains one or more syntax errors.
.
Language=Japanese
マニフェスト ファイルに 1 つ以上の構文エラーがあります。
.

MessageId=14006
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_ACTIVATION_CONTEXT_DISABLED
Language=English
The application attempted to activate a disabled activation context.
.
Language=Japanese
アプリケーションが無効なアクティブ化コンテキストをアクティブ化しようとしました。
.

MessageId=14007
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_KEY_NOT_FOUND
Language=English
The requested lookup key was not found in any active activation context.
.
Language=Japanese
要求された検索キーがアクティブなアクティブ化コンテキストに見つかりませんでした。
.

MessageId=14008
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_VERSION_CONFLICT
Language=English
A component version required by the application conflicts with another component version already active.
.
Language=Japanese
アプリケーションで必要なコンポーネントのバージョンが、すでにアクティブな別のコンポーネントのバージョンと競合しています。
.

MessageId=14009
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_WRONG_SECTION_TYPE
Language=English
The type requested activation context section does not match the query API used.
.
Language=Japanese
要求されたアクティベーション コンテキスト セクションのタイプが、使用されたクエリ API と一致しません。
.

MessageId=14010
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_THREAD_QUERIES_DISABLED
Language=English
Lack of system resources has required isolated activation to be disabled for the current thread of execution.
.
Language=Japanese
システム リソースの不足により、現在の実行スレッドで隔離されたアクティブ化を無効にする必要があります。
.

MessageId=14011
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_PROCESS_DEFAULT_ALREADY_SET
Language=English
An attempt to set the process default activation context failed because the process default activation context was already set.
.
Language=Japanese
プロセスの既定のアクティブ化コンテキストがすでに設定されていたため、プロセスの既定のアクティブ化コンテキストを設定しようとしましたが失敗しました。
.

MessageId=14012
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_UNKNOWN_ENCODING_GROUP
Language=English
The encoding group identifier specified is not recognized.
.
Language=Japanese
指定されたエンコード グループ識別子が認識されません。
.

MessageId=14013
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_UNKNOWN_ENCODING
Language=English
The encoding requested is not recognized.
.
Language=Japanese
要求されたエンコーディングが認識されません。
.

MessageId=14014
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_INVALID_XML_NAMESPACE_URI
Language=English
The manifest contains a reference to an invalid URI.
.
Language=Japanese
マニフェストに無効な URI への参照が含まれています。
.

MessageId=14015
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_ROOT_MANIFEST_DEPENDENCY_NOT_INSTALLED
Language=English
The application manifest contains a reference to a dependent assembly which is not installed.
.
Language=Japanese
アプリケーション マニフェストに、インストールされていない依存アセンブリへの参照が含まれています。
.

MessageId=14016
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_LEAF_MANIFEST_DEPENDENCY_NOT_INSTALLED
Language=English
The manifest for an assembly used by the application has a reference to a dependent assembly which is not installed.
.
Language=Japanese
アプリケーションが使用するアセンブリのマニフェストに、インストールされていない依存アセンブリへの参照があります。
.

MessageId=14017
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_INVALID_ASSEMBLY_IDENTITY_ATTRIBUTE
Language=English
The manifest contains an attribute for the assembly identity which is not valid.
.
Language=Japanese
マニフェストに有効でないアセンブリ ID の属性が含まれています。
.

MessageId=14018
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_MANIFEST_MISSING_REQUIRED_DEFAULT_NAMESPACE
Language=English
The manifest is missing the required default namespace specification on the assembly element.
.
Language=Japanese
マニフェストに、アセンブリ要素に必要なデフォルト名前空間指定がありません。
.

MessageId=14019
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_MANIFEST_INVALID_REQUIRED_DEFAULT_NAMESPACE
Language=English
The manifest has a default namespace specified on the assembly element but its value is not \"urn:schemas-microsoft-com:asm.v1\".
.
Language=Japanese
マニフェストにはアセンブリ要素で指定された既定のネームスペースがありますが、その値は \"urn:schemas-microsoft-com:asm.v1\" ではありません。
.

MessageId=14020
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_PRIVATE_MANIFEST_CROSS_PATH_WITH_REPARSE_POINT
Language=English
The private manifest probe has crossed the reparse-point-associated path.
.
Language=Japanese
プローブされたプライベート マニフェストがサポートされていないリパース ポイントを持つパスを横切りました。
.

MessageId=14021
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_DUPLICATE_DLL_NAME
Language=English
Two or more components referenced directly or indirectly by the application manifest have files by the same name.
.
Language=Japanese
アプリケーション マニフェストによって直接または間接的に参照される 2 つ以上のコンポーネントに、同じ名前のファイルがあります。
.

MessageId=14022
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_DUPLICATE_WINDOWCLASS_NAME
Language=English
Two or more components referenced directly or indirectly by the application manifest have window classes with the same name.
.
Language=Japanese
アプリケーション マニフェストによって直接または間接的に参照される2つ以上のコンポーネントに、同じ名前のウィンドウ クラスがあります。
.

MessageId=14023
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_DUPLICATE_CLSID
Language=English
Two or more components referenced directly or indirectly by the application manifest have the same COM server CLSIDs.
.
Language=Japanese
アプリケーション マニフェストによって直接または間接的に参照される2つ以上のコンポーネントに、同じCOMサーバーCLSIDがあります。
.

MessageId=14024
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_DUPLICATE_IID
Language=English
Two or more components referenced directly or indirectly by the application manifest have proxies for the same COM interface IIDs.
.
Language=Japanese
アプリケーション マニフェストによって直接または間接的に参照される2つ以上のコンポーネントが、同じCOMインターフェイスIIDのプロキシを持っています。
.

MessageId=14025
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_DUPLICATE_TLBID
Language=English
Two or more components referenced directly or indirectly by the application manifest have the same COM type library TLBIDs.
.
Language=Japanese
アプリケーション マニフェストによって直接または間接的に参照される2つ以上のコンポーネントが同じCOMタイプライブラリTLBIDsを持っています。
.

MessageId=14026
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_DUPLICATE_PROGID
Language=English
Two or more components referenced directly or indirectly by the application manifest have the same COM ProgIDs.
.
Language=Japanese
アプリケーション マニフェストによって直接または間接的に参照される2つ以上のコンポーネントが同じ COM ProgIDs を持っています。
.

MessageId=14027
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_DUPLICATE_ASSEMBLY_NAME
Language=English
Two or more components referenced directly or indirectly by the application manifest are different versions of the same component which is not permitted.
.
Language=Japanese
アプリケーション マニフェストによって直接または間接的に参照される2つ以上のコンポーネントは、許可されていない同じコンポーネントの異なるバージョンです。
.

MessageId=14028
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_FILE_HASH_MISMATCH
Language=English
A component's file does not match the verification information present in the component manifest.
.
Language=Japanese
コンポーネントのファイルがコンポーネント マニフェストに存在する検証情報と一致しません。
.

MessageId=14029
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_POLICY_PARSE_ERROR
Language=English
The policy manifest contains one or more syntax errors.
.
Language=Japanese
ポリシー マニフェストに1つ以上の構文エラーがあります。
.

MessageId=14030
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_MISSINGQUOTE
Language=English
Manifest Parse Error : A string literal was expected, but no opening quote character was found.
.
Language=Japanese
マニフェスト解析エラー: 文字列リテラルが期待されましたが、開始引用符が見つかりませんでした。
.

MessageId=14031
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_COMMENTSYNTAX
Language=English
Manifest Parse Error : Incorrect syntax was used in a comment.
.
Language=Japanese
マニフェスト解析エラー : コメントに不正な構文が使用されました。
.

MessageId=14032
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_BADSTARTNAMECHAR
Language=English
Manifest Parse Error : A name was started with an invalid character.
.
Language=Japanese
マニフェスト解析エラー: 名前が無効な文字で始まっています。
.

MessageId=14033
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_BADNAMECHAR
Language=English
Manifest Parse Error : A name contained an invalid character.
.
Language=Japanese
マニフェスト解析エラー: 名前に無効な文字が含まれています。
.

MessageId=14034
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_BADCHARINSTRING
Language=English
Manifest Parse Error : A string literal contained an invalid character.
.
Language=Japanese
マニフェスト解析エラー: 文字列リテラルに無効な文字が含まれていました。
.

MessageId=14035
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_XMLDECLSYNTAX
Language=English
Manifest Parse Error : Invalid syntax for an XML declaration.
.
Language=Japanese
マニフェスト解析エラー : xml 宣言の構文が無効です。
.

MessageId=14036
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_BADCHARDATA
Language=English
Manifest Parse Error : An invalid character was found in text content.
.
Language=Japanese
マニフェスト解析エラー: テキスト内容に無効な文字が見つかりました。
.

MessageId=14037
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_MISSINGWHITESPACE
Language=English
Manifest Parse Error : Required white space was missing.
.
Language=Japanese
マニフェスト解析エラー: 必要な空白文字が見つかりませんでした。
.

MessageId=14038
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_EXPECTINGTAGEND
Language=English
Manifest Parse Error : The character '>' was expected.
.
Language=Japanese
マニフェスト解析エラーです: 文字が期待されました。
.

MessageId=14039
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_MISSINGSEMICOLON
Language=English
Manifest Parse Error : A semi colon character was expected.
.
Language=Japanese
マニフェスト解析エラーです: セミコロン文字が予期されました。
.

MessageId=14040
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_UNBALANCEDPAREN
Language=English
Manifest Parse Error : Unbalanced parentheses.
.
Language=Japanese
マニフェスト解析エラー: カッコのバランスが取れていません。
.

MessageId=14041
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_INTERNALERROR
Language=English
Manifest Parse Error : Internal error.
.
Language=Japanese
マニフェスト解析エラー : 内部エラーです。
.

MessageId=14042
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_UNEXPECTED_WHITESPACE
Language=English
Manifest Parse Error : White space is not allowed at this location.
.
Language=Japanese
マニフェスト解析エラー: この場所では空白文字は使用できません。
.

MessageId=14043
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_INCOMPLETE_ENCODING
Language=English
Manifest Parse Error : End of file reached in invalid state for current encoding.
.
Language=Japanese
マニフェスト解析エラー : 現在のエンコーディングでは無効な状態でファイルの最後に到達しました。
.

MessageId=14044
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_MISSING_PAREN
Language=English
Manifest Parse Error : Missing parenthesis.
.
Language=Japanese
マニフェスト解析エラー : カッコがありません。
.

MessageId=14045
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_EXPECTINGCLOSEQUOTE
Language=English
Manifest Parse Error : A single or double closing quote character (\' or \") is missing.
.
Language=Japanese
マニフェスト 解析エラー : カッコがありません。
.

MessageId=14046
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_MULTIPLE_COLONS
Language=English
Manifest Parse Error : Multiple colons are not allowed in a name.
.
Language=Japanese
マニフェスト解析エラー : 名前に複数のコロンは使用できません。
.

MessageId=14047
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_INVALID_DECIMAL
Language=English
Manifest Parse Error : Invalid character for decimal digit.
.
Language=Japanese
マニフェスト解析エラー : 小数桁に無効な文字があります。
.

MessageId=14048
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_INVALID_HEXIDECIMAL
Language=English
Manifest Parse Error : Invalid character for hexadecimal digit.
.
Language=Japanese
マニフェスト解析エラー : 16進数に無効な文字があります。
.

MessageId=14049
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_INVALID_UNICODE
Language=English
Manifest Parse Error : Invalid Unicode character value for this platform.
.
Language=Japanese
マニフェスト解析エラー : このプラットフォームのユニコード文字値が無効です。
.

MessageId=14050
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_WHITESPACEORQUESTIONMARK
Language=English
Manifest Parse Error : Expecting white space or '?'.
.
Language=Japanese
マニフェスト解析エラー: 空白または？を予期しています。
.

MessageId=14051
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_UNEXPECTEDENDTAG
Language=English
Manifest Parse Error : End tag was not expected at this location.
.
Language=Japanese
マニフェスト解析エラー: 終了タグはこの場所では期待されませんでした。
.

MessageId=14052
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_UNCLOSEDTAG
Language=English
Manifest Parse Error : The following tags were not closed: %1.
.
Language=Japanese
マニフェスト 解析エラー: 次のタグが閉じられていません: %1
.

MessageId=14053
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_DUPLICATEATTRIBUTE
Language=English
Manifest Parse Error : Duplicate attribute.
.
Language=Japanese
マニフェスト解析エラー: 属性が重複しています。
.

MessageId=14054
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_MULTIPLEROOTS
Language=English
Manifest Parse Error : Only one top level element is allowed in an XML document.
.
Language=Japanese
マニフェスト解析エラー: XML ドキュメントで許可されるトップレベル要素は 1 つだけです。
.

MessageId=14055
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_INVALIDATROOTLEVEL
Language=English
Manifest Parse Error : Invalid at the top level of the document.
.
Language=Japanese
マニフェスト解析エラー : ドキュメントのトップ レベルが無効です。
.

MessageId=14056
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_BADXMLDECL
Language=English
Manifest Parse Error : Invalid XML declaration.
.
Language=Japanese
マニフェスト解析エラー : 無効な xml 宣言です。
.

MessageId=14057
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_MISSINGROOT
Language=English
Manifest Parse Error : XML document must have a top level element.
.
Language=Japanese
マニフェスト解析エラー: XML ドキュメントにはトップレベル要素が必要です。
.

MessageId=14058
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_UNEXPECTEDEOF
Language=English
Manifest Parse Error : Unexpected end of file.
.
Language=Japanese
マニフェスト解析エラー : 予期しないファイルの終了です。
.

MessageId=14059
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_BADPEREFINSUBSET
Language=English
Manifest Parse Error : Parameter entities cannot be used inside markup declarations in an internal subset.
.
Language=Japanese
マニフェスト解析エラー: パラメータ エンティティは内部サブセットのマークアップ宣言内では使用できません。
.

MessageId=14060
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_UNCLOSEDSTARTTAG
Language=English
Manifest Parse Error : Element was not closed.
.
Language=Japanese
マニフェスト解析エラー: 要素が閉じられていません。
.

MessageId=14061
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_UNCLOSEDENDTAG
Language=English
Manifest Parse Error : End element was missing the character '>'.
.
Language=Japanese
マニフェスト解析エラー: End要素に文字がありません。
.

MessageId=14062
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_UNCLOSEDSTRING
Language=English
Manifest Parse Error : A string literal was not closed.
.
Language=Japanese
マニフェスト解析エラー: 文字列リテラルが閉じられていません。
.

MessageId=14063
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_UNCLOSEDCOMMENT
Language=English
Manifest Parse Error : A comment was not closed.
.
Language=Japanese
マニフェスト解析エラー: コメントが閉じられていません。
.

MessageId=14064
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_UNCLOSEDDECL
Language=English
Manifest Parse Error : A declaration was not closed.
.
Language=Japanese
マニフェスト 解析エラー: 宣言が閉じられていません。
.

MessageId=14065
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_UNCLOSEDCDATA
Language=English
Manifest Parse Error : A CDATA section was not closed.
.
Language=Japanese
マニフェスト解析エラー: CDATA セクションが閉じられていません。
.

MessageId=14066
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_RESERVEDNAMESPACE
Language=English
Manifest Parse Error : The namespace prefix is not allowed to start with the reserved string \"xml\".
.
Language=Japanese
マニフェスト解析エラー: 名前空間接頭辞を予約文字列で開始することはできません。
.

MessageId=14067
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_INVALIDENCODING
Language=English
Manifest Parse Error : System does not support the specified encoding.
.
Language=Japanese
マニフェスト解析エラー : システムが指定されたエンコードをサポートしていません。
.

MessageId=14068
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_INVALIDSWITCH
Language=English
Manifest Parse Error : Switch from current encoding to specified encoding not supported.
.
Language=Japanese
マニフェスト解析エラー : 現在のエンコーディングから指定されたエンコーディングへの切り替えがサポートされていません。
.

MessageId=14069
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_BADXMLCASE
Language=English
Manifest Parse Error : The name 'xml' is reserved and must be lower case.
.
Language=Japanese
マニフェスト解析エラーです: 名前'xml'は予約されているため、小文字にする必要があります。
.

MessageId=14070
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_INVALID_STANDALONE
Language=English
Manifest Parse Error : The standalone attribute must have the value 'yes' or 'no'.
.
Language=Japanese
マニフェスト解析エラー : スタンドアロン属性は、値 'yes'または 'no'が必要です。
.

MessageId=14071
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_UNEXPECTED_STANDALONE
Language=English
Manifest Parse Error : The standalone attribute cannot be used in external entities.
.
Language=Japanese
マニフェスト解析エラー : スタンドアロン属性は外部エンティティでは使用できません。
.

MessageId=14072
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_INVALID_VERSION
Language=English
Manifest Parse Error : Invalid version number.
.
Language=Japanese
マニフェスト解析エラー : 無効なバージョン番号です。
.

MessageId=14073
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_XML_E_MISSINGEQUALS
Language=English
Manifest Parse Error : Missing equals sign between attribute and attribute value.
.
Language=Japanese
マニフェスト解析エラー: 属性と属性値の間に等号がありません。
.

MessageId=14074
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_PROTECTION_RECOVERY_FAILED
Language=English
Assembly Protection Error: Unable to recover the specified assembly.
.
Language=Japanese
アセンブリ保護エラー : 指定されたアセンブリを回復できません。
.

MessageId=14075
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_PROTECTION_PUBLIC_KEY_TOO_SHORT
Language=English
Assembly Protection Error: The public key for an assembly was too short to be allowed.
.
Language=Japanese
アセンブリ保護エラー : アセンブリの公開キーが短すぎて許可されませんでした。
.

MessageId=14076
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_PROTECTION_CATALOG_NOT_VALID
Language=English
Assembly Protection Error: The catalog for an assembly is not valid, or does not match the assembly's manifest.
.
Language=Japanese
アセンブリ保護エラー: アセンブリのカタログが有効でないか、アセンブリのマニフェストと一致しません。
.

MessageId=14077
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_UNTRANSLATABLE_HRESULT
Language=English
An HRESULT could not be translated to a corresponding Win32 error code.
.
Language=Japanese
HRESULT を対応する Win32 エラー コードに変換できませんでした。
.

MessageId=14078
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_PROTECTION_CATALOG_FILE_MISSING
Language=English
Assembly Protection Error: The catalog for an assembly is missing.
.
Language=Japanese
アセンブリ保護エラー: アセンブリのカタログがありません。
.

MessageId=14079
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_MISSING_ASSEMBLY_IDENTITY_ATTRIBUTE
Language=English
The supplied assembly identity is missing one or more attributes which must be present in this context.
.
Language=Japanese
指定されたアセンブリ ID に、このコンテキストに存在する必要がある属性が 1 つ以上ありません。
.

MessageId=14080
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_INVALID_ASSEMBLY_IDENTITY_ATTRIBUTE_NAME
Language=English
The supplied assembly identity has one or more attribute names that contain characters not permitted in XML names.
.
Language=Japanese
指定されたアセンブリ ID には、XML 名で許可されていない文字を含む属性名が 1 つ以上あります。
.

MessageId=14081
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_ASSEMBLY_MISSING
Language=English
The referenced assembly could not be found.
.
Language=Japanese
参照されたアセンブリが見つかりません。
.

MessageId=14082
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_CORRUPT_ACTIVATION_STACK
Language=English
The activation context activation stack for the running thread of execution is corrupt.
.
Language=Japanese
実行スレッドのアクティブ化コンテキストのアクティブ化スタックが壊れています。
.

MessageId=14083
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_CORRUPTION
Language=English
The application isolation metadata for this process or thread has become corrupt.
.
Language=Japanese
このプロセスまたはスレッドのアプリケーション分離メタデータが破損しています。
.

MessageId=14084
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_EARLY_DEACTIVATION
Language=English
The activation context being deactivated is not the most recently activated one.
.
Language=Japanese
アクティブ化解除されるアクティブ化コンテキストは、最近アクティブ化されたものではありません。
.

MessageId=14085
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_INVALID_DEACTIVATION
Language=English
The activation context being deactivated is not active for the current thread of execution.
.
Language=Japanese
アクティブ化解除されたアクティブ化コンテキストは、現在の実行スレッドに対してアクティブではありません。
.

MessageId=14086
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_MULTIPLE_DEACTIVATION
Language=English
The activation context being deactivated has already been deactivated.
.
Language=Japanese
アクティブ化解除されるアクティブ化コンテキストは既にアクティブ化解除されています。
.

MessageId=14087
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_PROCESS_TERMINATION_REQUESTED
Language=English
A component used by the isolation facility has requested to terminate the process.
.
Language=Japanese
分離機能で使用されているコンポーネントがプロセスの終了を要求しました。
.

MessageId=14088
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_RELEASE_ACTIVATION_CONTEXT
Language=English
A kernel mode component is releasing a reference on an activation context.
.
Language=Japanese
カーネルモードのコンポーネントがアクティブ化コンテキストの参照を解放しています。
.

MessageId=14089
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_SYSTEM_DEFAULT_ACTIVATION_CONTEXT_EMPTY
Language=English
The activation context of system default assembly could not be generated.
.
Language=Japanese
システム デフォルト アセンブリのアクティブ化コンテキストを生成できませんでした。
.

MessageId=14090
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_INVALID_IDENTITY_ATTRIBUTE_VALUE
Language=English
The value of an attribute in an identity is not within the legal range.
.
Language=Japanese
ID の属性の値が正当な範囲内にありません。
.

MessageId=14091
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_INVALID_IDENTITY_ATTRIBUTE_NAME
Language=English
The name of an attribute in an identity is not within the legal range.
.
Language=Japanese
ID の属性名が有効な範囲内にありません。
.

MessageId=14092
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_IDENTITY_DUPLICATE_ATTRIBUTE
Language=English
An identity contains two definitions for the same attribute.
.
Language=Japanese
ID に同じ属性の定義が 2 つ含まれています。
.

MessageId=14093
Severity=Success
Facility=System
SymbolicName=ERROR_SXS_IDENTITY_PARSE_ERROR
Language=English
The identity string is malformed. This may be due to a trailing comma, more than two unnamed attributes, missing attribute name or missing attribute value.
.
Language=Japanese
ID 文字列が不正です。これは、末尾のカンマ、2つ以上の名前のない属性、属性名の欠落、または属性値の欠落が原因です。
.

MessageId=15000
Severity=Success
Facility=System
SymbolicName=ERROR_EVT_INVALID_CHANNEL_PATH
Language=English
The specified channel path is invalid. See extended error info for more details.
.
Language=Japanese
指定されたチャンネル・パスが無効です。詳細は拡張エラー情報を参照してください。
.

MessageId=15001
Severity=Success
Facility=System
SymbolicName=ERROR_EVT_INVALID_QUERY
Language=English
The specified query is invalid. See extended error info for more details.
.
Language=Japanese
指定されたクエリが無効です。詳細は拡張エラー情報を参照してください。
.

MessageId=15002
Severity=Success
Facility=System
SymbolicName=ERROR_EVT_PUBLISHER_MANIFEST_NOT_FOUND
Language=English
The publisher did indicate they have a manifest/resource but a manifest/resource could not be found.
.
Language=Japanese
パブリッシャはマニフェスト／リソースがあることを示したが、マニフェスト／リソースは見つかりませんでした。
.

MessageId=15003
Severity=Success
Facility=System
SymbolicName=ERROR_EVT_PUBLISHER_MANIFEST_NOT_SPECIFIED
Language=English
The publisher does not have a manifest and is performing an operation which requires they have a manifest.
.
Language=Japanese
パブリッシャはマニフェストを持っておらず、マニフェストを持っていることを要求する操作を行っています。
.

MessageId=15004
Severity=Success
Facility=System
SymbolicName=ERROR_EVT_NO_REGISTERED_TEMPLATE
Language=English
There is no registered template for specified event id.
.
Language=Japanese
指定されたイベントIDに対応するテンプレートが登録されていません。
.

MessageId=15005
Severity=Success
Facility=System
SymbolicName=ERROR_EVT_EVENT_CHANNEL_MISMATCH
Language=English
The specified event was declared in the manifest to go a different channel than the one this publisher handle is bound to.
.
Language=Japanese
パブリッシャによって発生されたイベント データは、パブリッシャのマニフェスト内のイベント テンプレート定義と互換性がありません。
.

MessageId=15006
Severity=Success
Facility=System
SymbolicName=ERROR_EVT_UNEXPECTED_VALUE_TYPE
Language=English
The type of a specified substitution value does not match the type expected from the template definition.
.
Language=Japanese
指定された置換値の型が、テンプレート定義から期待される型と一致しません。
.

MessageId=15007
Severity=Success
Facility=System
SymbolicName=ERROR_EVT_UNEXPECTED_NUM_VALUES
Language=English
The number of specified substitution values does not match the number expected from the template definition.
.
Language=Japanese
指定された置換値の数が、テンプレート定義から予想される数と一致しません。
.

MessageId=15008
Severity=Success
Facility=System
SymbolicName=ERROR_EVT_CHANNEL_NOT_FOUND
Language=English
The specified channel could not be found. Check channel configuration.
.
Language=Japanese
指定されたチャネルが見つからない。チャンネル構成を確認してください。
.

MessageId=15009
Severity=Success
Facility=System
SymbolicName=ERROR_EVT_MALFORMED_XML_TEXT
Language=English
The specified xml text was not well-formed. See Extended Error for more details.
.
Language=Japanese
指定されたxmlテキストは整形式ではありませんでした。詳細は拡張エラーを参照してください。
.

MessageId=15010
Severity=Success
Facility=System
SymbolicName=ERROR_EVT_CHANNEL_PATH_TOO_GENERAL
Language=English
The specified channel path selects more than one instance of a channel. The operation requires that only one channel be selected. It may be necessary to scope channel path to version / publicKeyToken to select only one instance.
.
Language=Japanese
指定されたチャネルパスが、複数のチャネルのインスタンスを選択する。この操作では、1 つのチャネルのみが選択される必要があります。1つのインスタンスのみを選択するために、チャネルパスを version / publicKeyToken にスコープする必要があるかもしれません。
.

MessageId=0x4001
Severity=Warning
Facility=System
SymbolicName=E_NOTIMPL
Language=English
Not implemented
.
Language=Japanese
Not implemented
.

MessageId=0x4004
Severity=Warning
Facility=System
SymbolicName=E_ABORT
Language=English
Operation aborted
.
Language=Japanese
Operation aborted
.

MessageId=0x4005
Severity=Warning
Facility=System
SymbolicName=E_FAIL
Language=English
Unspecified error
.
Language=Japanese
Unspecified error
.

MessageId=0xFFFF
Severity=Warning
Facility=System
SymbolicName=E_UNEXPECTED
Language=English
Catastrophic failure
.
Language=Japanese
Catastrophic failure
.

MessageId=0x000E
Severity=Warning
Facility=WIN32
SymbolicName=E_OUTOFMEMORY
Language=English
Out of memory
.
Language=Japanese
この操作を完了するのに十分なメモリ リソースがありません。
.

MessageId=0x0057
Severity=Warning
Facility=WIN32
SymbolicName=E_INVALIDARG
Language=English
One or more arguments are invalid
.
Language=Japanese
パラメータが不正です。
.

MessageId=0x0006
Severity=Warning
Facility=WIN32
SymbolicName=E_HANDLE
Language=English
Invalid handle
.
Language=Japanese
ハンドルが無効です。
.

MessageId=0x0005
Severity=Warning
Facility=WIN32
SymbolicName=E_ACCESSDENIED
Language=English
General access denied error
.
Language=Japanese
一般的なアクセス拒否エラーです。
.


MessageId=0x0000
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_OLEVERB
Language=English
Invalid OLEVERB structure
.
Language=Japanese
無効な OLEVERB 構造体です。
.

MessageId=0x0001
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_ADVF
Language=English
Invalid advise flags
.
Language=Japanese
無効なアドバイス フラグです。

.

MessageId=0x0002
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_ENUM_NOMORE
Language=English
Can't enumerate any more, because the associated data is missing
.
Language=Japanese
関連データがないため、これ以上列挙できません。
.

MessageId=0x0003
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_ADVISENOTSUPPORTED
Language=English
This implementation doesn't take advises
.
Language=Japanese
この実装はアドバイスを受けません。
.

MessageId=0x0004
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_NOCONNECTION
Language=English
There is no connection for this connection ID
.
Language=Japanese
この接続IDには接続がありません。

.

MessageId=0x0005
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_NOTRUNNING
Language=English
Need to run the object to perform this operation
.
Language=Japanese
この操作を実行するには、オブジェクトを実行する必要があります。
.

MessageId=0x0006
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_NOCACHE
Language=English
There is no cache to operate on
.
Language=Japanese
操作するキャッシュがありません。
.

MessageId=0x0007
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_BLANK
Language=English
Uninitialized object
.
Language=Japanese
初期化されていないオブジェクトです。
.

MessageId=0x0008
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_CLASSDIFF
Language=English
Linked object's source class has changed
.
Language=Japanese
リンクされたオブジェクトのソース クラスが変更されました。

.

MessageId=0x0009
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_CANT_GETMONIKER
Language=English
Not able to get the moniker of the object
.
Language=Japanese
オブジェクトのモニカを取得できません。
.

MessageId=0x000A
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_CANT_BINDTOSOURCE
Language=English
Not able to bind to the source
.
Language=Japanese
ソースにバインドできません。
.

MessageId=0x000B
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_STATIC
Language=English
Object is static; operation not allowed
.
Language=Japanese
オブジェクトは静的です。操作できません。
.

MessageId=0x000C
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_PROMPTSAVECANCELLED
Language=English
User canceled out of save dialog
.
Language=Japanese
ユーザーが保存ダイアログからキャンセルしました。
.

MessageId=0x000D
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_INVALIDRECT
Language=English
Invalid rectangle
.
Language=Japanese
無効な長方形です。
.

MessageId=0x000E
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_WRONGCOMPOBJ
Language=English
compobj.dll is too old for the ole2.dll initialized
.
Language=Japanese
compobj.dll は ole2.dll が初期化されるには古すぎます。
.

MessageId=0x000F
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_INVALIDHWND
Language=English
Invalid window handle
.
Language=Japanese
無効なウィンドウハンドルです。
.

MessageId=0x0010
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_NOT_INPLACEACTIVE
Language=English
Object is not in any of the inplace active states
.
Language=Japanese
オブジェクトがいずれのインプレース アクティブ状態でもありません。
.

MessageId=0x0011
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_CANTCONVERT
Language=English
Not able to convert object
.
Language=Japanese
オブジェクトを変換できません。
.

MessageId=0x0012
Severity=Warning
Facility=ITF
SymbolicName=OLE_E_NOSTORAGE
Language=English
Not able to perform the operation because object is not given storage yet
.
Language=Japanese
オブジェクトにまだストレージが与えられていないため、操作を実行できません。
.

MessageId=0x0064
Severity=Warning
Facility=ITF
SymbolicName=DV_E_FORMATETC
Language=English
Invalid FORMATETC structure
.
Language=Japanese
無効な FORMATETC 構造体です。
.

MessageId=0x0065
Severity=Warning
Facility=ITF
SymbolicName=DV_E_DVTARGETDEVICE
Language=English
Invalid DVTARGETDEVICE structure
.
Language=Japanese
無効な DVTARGETDEVICE 構造体です。
.

MessageId=0x0066
Severity=Warning
Facility=ITF
SymbolicName=DV_E_STGMEDIUM
Language=English
Invalid STDGMEDIUM structure
.
Language=Japanese
無効な STDGMEDIUM 構造体です。
.

MessageId=0x0067
Severity=Warning
Facility=ITF
SymbolicName=DV_E_STATDATA
Language=English
Invalid STATDATA structure
.
Language=Japanese
無効な STATDATA 構造体です。
.

MessageId=0x0068
Severity=Warning
Facility=ITF
SymbolicName=DV_E_LINDEX
Language=English
Invalid lindex
.
Language=Japanese
無効な lindex です。
.

MessageId=0x0069
Severity=Warning
Facility=ITF
SymbolicName=DV_E_TYMED
Language=English
Invalid tymed
.
Language=Japanese
無効な tymed です。
.

MessageId=0x006A
Severity=Warning
Facility=ITF
SymbolicName=DV_E_CLIPFORMAT
Language=English
Invalid clipboard format
.
Language=Japanese
無効なクリップボード形式です。
.

MessageId=0x006B
Severity=Warning
Facility=ITF
SymbolicName=DV_E_DVASPECT
Language=English
Invalid aspect(s)
.
Language=Japanese
無効なアスペクトです。
.

MessageId=0x006C
Severity=Warning
Facility=ITF
SymbolicName=DV_E_DVTARGETDEVICE_SIZE
Language=English
tdSize parameter of the DVTARGETDEVICE structure is invalid
.
Language=Japanese
DVTARGETDEVICE 構造体の tdSize パラメータが無効です。
.

MessageId=0x006D
Severity=Warning
Facility=ITF
SymbolicName=DV_E_NOIVIEWOBJECT
Language=English
Object doesn't support IViewObject interface
.
Language=Japanese
オブジェクトがIViewObjectインターフェイスをサポートしていません。
.

MessageId=0x0100
Severity=Warning
Facility=ITF
SymbolicName=DRAGDROP_E_NOTREGISTERED
Language=English
Trying to revoke a drop target that has not been registered
.
Language=Japanese
登録されていないドロップ目標を取り消そうとしています。
.

MessageId=0x0101
Severity=Warning
Facility=ITF
SymbolicName=DRAGDROP_E_ALREADYREGISTERED
Language=English
This window has already been registered as a drop target
.
Language=Japanese
このウィンドウはすでにドロップターゲットとして登録されています。
.

MessageId=0x0102
Severity=Warning
Facility=ITF
SymbolicName=DRAGDROP_E_INVALIDHWND
Language=English
Invalid window handle
.
Language=Japanese
無効なウィンドウ ハンドルです。
.

MessageId=0x0110
Severity=Warning
Facility=ITF
SymbolicName=CLASS_E_NOAGGREGATION
Language=English
Class does not support aggregation (or class object is remote)
.
Language=Japanese
クラスが集約をサポートしていません (またはクラス・オブジェクトがリモートです)。
.

MessageId=0x0111
Severity=Warning
Facility=ITF
SymbolicName=CLASS_E_CLASSNOTAVAILABLE
Language=English
ClassFactory cannot supply requested class
.
Language=Japanese
ClassFactory が要求されたクラスを提供できません。
.

MessageId=0x0112
Severity=Warning
Facility=ITF
SymbolicName=CLASS_E_NOTLICENSED
Language=English
Class is not licensed for use
.
Language=Japanese
クラスの使用が許可されていません。
.

MessageId=0x0001
Severity=Warning
Facility=CERT
SymbolicName=TRUST_E_PROVIDER_UNKNOWN
Language=English
Unknown trust provider.
.
Language=Japanese
Unknown trust provider.
.

MessageId=0x0002
Severity=Warning
Facility=CERT
SymbolicName=TRUST_E_ACTION_UNKNOWN
Language=English
The trust verification action specified is not supported by the specified trust provider.
.
Language=Japanese
The trust verification action specified is not supported by the specified trust provider.
.

MessageId=0x0003
Severity=Warning
Facility=CERT
SymbolicName=TRUST_E_SUBJECT_FORM_UNKNOWN
Language=English
The form specified for the subject is not one supported or known by the specified trust provider.
.
Language=Japanese
The form specified for the subject is not one supported or known by the specified trust provider.
.

MessageId=0x0004
Severity=Warning
Facility=CERT
SymbolicName=TRUST_E_SUBJECT_NOT_TRUSTED
Language=English
The subject is not trusted for the specified action.
.
Language=Japanese
The subject is not trusted for the specified action.
.

MessageId=0x0005
Severity=Warning
Facility=CERT
SymbolicName=DIGSIG_E_ENCODE
Language=English
Error due to problem in ASN.1 encoding process.
.
Language=Japanese
Error due to problem in ASN.1 encoding process.
.

MessageId=0x0006
Severity=Warning
Facility=CERT
SymbolicName=DIGSIG_E_DECODE
Language=English
Error due to problem in ASN.1 decoding process.
.
Language=Japanese
Error due to problem in ASN.1 decoding process.
.

MessageId=0x0007
Severity=Warning
Facility=CERT
SymbolicName=DIGSIG_E_EXTENSIBILITY
Language=English
Reading / writing Extensions where Attributes are appropriate, and vice versa.
.
Language=Japanese
Reading / writing Extensions where Attributes are appropriate, and vice versa.
.

MessageId=0x0008
Severity=Warning
Facility=CERT
SymbolicName=DIGSIG_E_CRYPTO
Language=English
Unspecified cryptographic failure.
.
Language=Japanese
Unspecified cryptographic failure.
.

MessageId=0x0009
Severity=Warning
Facility=CERT
SymbolicName=PERSIST_E_SIZEDEFINITE
Language=English
The size of the data could not be determined.
.
Language=Japanese
The size of the data could not be determined.
.

MessageId=0x000A
Severity=Warning
Facility=CERT
SymbolicName=PERSIST_E_SIZEINDEFINITE
Language=English
The size of the indefinite-sized data could not be determined.
.
Language=Japanese
The size of the indefinite-sized data could not be determined.
.

MessageId=0x000B
Severity=Warning
Facility=CERT
SymbolicName=PERSIST_E_NOTSELFSIZING
Language=English
This object does not read and write self-sizing data.
.
Language=Japanese
This object does not read and write self-sizing data.
.

MessageId=0x0100
Severity=Warning
Facility=CERT
SymbolicName=TRUST_E_NOSIGNATURE
Language=English
No signature was present in the subject.
.
Language=Japanese
No signature was present in the subject.
.

MessageId=0x0101
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_EXPIRED
Language=English
A required certificate is not within its validity period when verifying against the current system clock or the timestamp in the signed file.
.
Language=Japanese
A required certificate is not within its validity period when verifying against the current system clock or the timestamp in the signed file.
.

MessageId=0x0102
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_VALIDITYPERIODNESTING
Language=English
The validity periods of the certification chain do not nest correctly.
.
Language=Japanese
The validity periods of the certification chain do not nest correctly.
.

MessageId=0x0103
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_ROLE
Language=English
A certificate that can only be used as an end-entity is being used as a CA or vice versa.
.
Language=Japanese
A certificate that can only be used as an end-entity is being used as a CA or vice versa.
.

MessageId=0x0104
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_PATHLENCONST
Language=English
A path length constraint in the certification chain has been violated.
.
Language=Japanese
A path length constraint in the certification chain has been violated.
.

MessageId=0x0105
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_CRITICAL
Language=English
A certificate contains an unknown extension that is marked 'critical'.
.
Language=Japanese
A certificate contains an unknown extension that is marked 'critical'.
.

MessageId=0x0106
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_PURPOSE
Language=English
A certificate being used for a purpose other than the ones specified by its CA.
.
Language=Japanese
A certificate being used for a purpose other than the ones specified by its CA.
.

MessageId=0x0107
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_ISSUERCHAINING
Language=English
A parent of a given certificate in fact did not issue that child certificate.
.
Language=Japanese
A parent of a given certificate in fact did not issue that child certificate.
.

MessageId=0x0108
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_MALFORMED
Language=English
A certificate is missing or has an empty value for an important field, such as a subject or issuer name.
.
Language=Japanese
A certificate is missing or has an empty value for an important field, such as a subject or issuer name.
.

MessageId=0x0109
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_UNTRUSTEDROOT
Language=English
A certificate chain processed, but terminated in a root certificate which is not trusted by the trust provider.
.
Language=Japanese
A certificate chain processed, but terminated in a root certificate which is not trusted by the trust provider.
.

MessageId=0x010A
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_CHAINING
Language=English
A certificate chain could not be built to a trusted root authority.
.
Language=Japanese
A certificate chain could not be built to a trusted root authority.
.

MessageId=0x010B
Severity=Warning
Facility=CERT
SymbolicName=TRUST_E_FAIL
Language=English
Generic trust failure.
.
Language=Japanese
Generic trust failure.
.

MessageId=0x010C
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_REVOKED
Language=English
A certificate was explicitly revoked by its issuer.
.
Language=Japanese
A certificate was explicitly revoked by its issuer.
.

MessageId=0x010D
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_UNTRUSTEDTESTROOT
Language=English
The certification path terminates with the test root which is not trusted with the current policy settings.
.
Language=Japanese
The certification path terminates with the test root which is not trusted with the current policy settings.
.

MessageId=0x010E
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_REVOCATION_FAILURE
Language=English
The revocation process could not continue - the certificate(s) could not be checked.
.
Language=Japanese
The revocation process could not continue - the certificate(s) could not be checked.
.

MessageId=0x010F
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_CN_NO_MATCH
Language=English
The certificate's CN name does not match the passed value.
.
Language=Japanese
The certificate's CN name does not match the passed value.
.

MessageId=0x0110
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_WRONG_USAGE
Language=English
The certificate is not valid for the requested usage.
.
Language=Japanese
The certificate is not valid for the requested usage.
.

MessageId=0x0111
Severity=Warning
Facility=CERT
SymbolicName=TRUST_E_EXPLICIT_DISTRUST
Language=English
The certificate was explicitly marked as untrusted by the user.
.
Language=Japanese
The certificate was explicitly marked as untrusted by the user.
.

MessageId=0x0112
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_UNTRUSTEDCA
Language=English
A certification chain processed correctly, but one of the CA certificates is not trusted by the policy provider.
.
Language=Japanese
A certification chain processed correctly, but one of the CA certificates is not trusted by the policy provider.
.

MessageId=0x0113
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_INVALID_POLICY
Language=English
The certificate has invalid policy.
.
Language=Japanese
The certificate has invalid policy.
.

MessageId=0x0114
Severity=Warning
Facility=CERT
SymbolicName=CERT_E_INVALID_NAME
Language=English
The certificate has an invalid name. The name is not included in the permitted list or is explicitly excluded.
.
Language=Japanese
The certificate has an invalid name. The name is not included in the permitted list or is explicitly excluded.
.
