# ReliefWin first-run experience

New interactive installations schedule `oobe.exe` before the desktop starts.
Unattended installations keep their supplied account and settings. The
`/components` mode remains available for an existing component-only schedule.
Existing installations can open Settings → System → About → Preview first-run
setup, or run `%SystemRoot%\system32\oobe\oobe.exe /preview`. Preview never
calls the configuration backend or launches network/encryption management.

The flow is language, keyboard/time zone, device name, network, local account,
encryption, optional downloads, review, apply and completion. Network setup and
encryption backup reuse the installed Settings and Disk Encryption applications.
Internet access is optional; no additional component is selected by default.
An encrypted installation still needs its original recovery key: OOBE cannot
reconstruct it from a password. Completion signs out, or restarts if the device
name changed, so the new account can sign in without saving a plaintext
automatic-sign-in password.

Account creation records the account SID for retries. A foreign or replaced
account with the same name cannot be reset. The initial built-in Administrator
is disabled only at successful completion; other existing accounts are retained.
The Run entry stays present until completion succeeds. Interrupted configuration
replays region/time-zone/name operations and resumes the recorded account.

## Sources

* Microsoft Win32 documentation: [USER_INFO_1](https://learn.microsoft.com/en-us/windows/win32/api/lmaccess/ns-lmaccess-user_info_1),
  [NetUserAdd](https://learn.microsoft.com/en-us/windows/win32/api/lmaccess/nf-lmaccess-netuseradd),
  [NetUserSetInfo](https://learn.microsoft.com/en-us/windows/win32/api/lmaccess/nf-lmaccess-netusersetinfo),
  [NetLocalGroupAddMembers](https://learn.microsoft.com/en-us/windows/win32/api/lmaccess/nf-lmaccess-netlocalgroupaddmembers).
* Microsoft Win32 documentation: [SetComputerNameExW](https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-setcomputernameexw),
  [SetDynamicTimeZoneInformation](https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-setdynamictimezoneinformation),
  [ExitWindowsEx](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-exitwindowsex),
  [LookupAccountNameW](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-lookupaccountnamew),
  [LookupAccountSidW](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-lookupaccountsidw),
  [NetLocalGroupGetInfo](https://learn.microsoft.com/en-us/windows/win32/api/lmaccess/nf-lmaccess-netlocalgroupgetinfo),
  [Registry value types](https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-value-types),
  [GetAdaptersAddresses](https://learn.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersaddresses).
* Microsoft Win32 control contracts:
  [CB_SETCURSEL](https://learn.microsoft.com/en-us/windows/win32/controls/cb-setcursel),
  [RedrawWindow](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-redrawwindow),
  [WM_CTLCOLORLISTBOX](https://learn.microsoft.com/en-us/windows/win32/controls/wm-ctlcolorlistbox).
* ReliefWin boot-language file contract: `bootloader/gui/bootlangio.c` (exactly
  five bytes, `zh-CN`, `en-US` or `ja-JP`). This is a ReliefWin-private format.
* ReliefWin encryption status contract: `sdk/include/reliefwin/drivers/volumecrypt.h`
  and `drivers/crypto/reliefcrypt/io.c`. This is a ReliefWin-private IOCTL.
* Icons: existing, unmodified Fluent UI System Icons from `base/shell/shellframe`;
  product logo: existing `base/setup/reliefwin/res/reliefwin.ico`.

## Focused checks

`python3 base/setup/reliefwin/tests/transactions.py` compiles the actual account
and completion functions with ASan/UBSan and deterministic API failure injection.
It checks group assignment, absent plaintext automatic-sign-in password, SID
ownership, retries, rollback, completion ordering, restart selection and component-only behavior.
These mocked calls test OOBE policy, not the Windows API implementations.

The existing `scripts/test_setup_language.py` and
`scripts/test_syssetup_target_path.py` cover the retained locale and target-hive
helpers. Build the `oobe`, `reliefwin`, `syssetup`, `userinit` and `systemsettings`
and `lsasrv`, `advapi32`, `comctl32_v6` targets with `CCACHE_DISABLE=1` on amd64,
i386 and ARM64.

`oobe_test /aliases` checks that the names returned for the Administrators and
Users SIDs exist in SAM and resolve back to those SIDs, without requiring an
English installation. On the unmodified Chinese image the two SAM-name checks
failed. LSA now resolves built-in aliases through SAM instead of its fixed-name
table; the fixed SID objects remain available to LSA's other consumers.
It also checks the returned SID length: `LookupAccountNameW` now returns the
actual SID size after a successful lookup, as its documented output requires.
`oobe_test <username>` additionally checks the logged-in account, both group
memberships, completion markers, absent plaintext sign-in password and inherited
Chinese UI language and keyboard configuration.

`oobe_test /controls` compares a themed dropdown immediately after changing its
selection with its fully repainted appearance, for both `CB_SETCURSEL` and the
Down key. The old direct text path left the previous selection visible under
the new text. The themed path now repaints the complete control background.
The test uses the same common-controls activation manifest as OOBE.

Runtime evidence is collected on the existing amd64 QEMU image with a FAT boot
partition and encrypted NTFS system partition. No Windows 11 reference image or
physical machine is available in this workspace. Neither host tests nor amd64
QEMU establish behavior on untested hardware or other architectures.

The account transaction harness passes 120 checks. On the Chinese amd64 image,
the SID-size regression changed from 2 failures to 0 in 10 checks. The dropdown
regression changed from 2 failures to 0 in 7 checks after loading the rebuilt
common-controls DLL. The actual flow renamed the device to `RELIEF-DESKTOP`,
created `ReliefTester` offline, restarted the encrypted system and reached that
account's desktop. The post-setup test passes all 25 checks, including both group
memberships, inherited Chinese UI/keyboard settings and removal of the Run entry.
All affected targets build on amd64, i386 and ARM64.
The sidebar progress group is centered using its scaled width. Screenshot
measurement on the 1280×900 amd64 QEMU preview reproduced the old 36/77-pixel
side margins and confirms equal 60/60-pixel margins with eight equally spaced
dots after the fix; the updated OOBE builds on all three architectures.
The installed cppcheck 2.7 cannot parse the toolchain's libc++ preprocessing and
some SDK declarations; its incomplete run is not treated as a clean analysis.
