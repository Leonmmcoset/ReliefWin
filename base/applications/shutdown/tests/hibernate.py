# PROJECT:     ReliefWin API tests
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Pin shutdown output and exit status for each hibernation outcome
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

import argparse
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('--source', type=Path)
args = parser.parse_args()
source = args.source or Path(__file__).resolve().parents[1] / 'shutdown.c'
text = source.read_text()
start = text.index('    if (opts.hibernate)')
end = text.index('    /* Both shutdown', start)
fixture = r'''
#include <stdio.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
#define SE_SHUTDOWN_NAME 1
enum {StdOut, StdErr, IDS_ERROR_HIBERNATE, IDS_ERROR_HIBERNATE_ENABLED};
static int Allowed, Suspended, Calls, Message, Stream, Checks, Failures;
static struct {int hibernate; void *remote_system;} opts = {1, NULL};
static int IsPwrHibernateAllowed(void) {return Allowed;}
static void EnablePrivilege(int Name, int Enable) {(void)Name; (void)Enable;}
static int SetSuspendState(int H, int F, int W) {(void)H; (void)F; (void)W; ++Calls; return Suspended;}
static void ConResPuts(int S, int M) {Stream = S; Message = M;}
static int GetLastError(void) {return 1;}
static void DisplayError(int E) {(void)E;}
static int Run(void)
{
'''
tests = r'''
return EXIT_FAILURE;
}
#define CHECK(C) do {++Checks; if (!(C)) {++Failures; printf("FAIL %u: %s\n", __LINE__, #C);}} while (0)
int main(void)
{
    Message = -1; Allowed = Suspended = 1;
    CHECK(Run() == EXIT_SUCCESS && Calls == 1 && Message == -1);
    Message = -1; Calls = 0; Suspended = 0;
    CHECK(Run() == EXIT_FAILURE && Calls == 1 && Message == IDS_ERROR_HIBERNATE && Stream == StdErr);
    Message = -1; Calls = 0; Allowed = 0;
    CHECK(Run() == EXIT_FAILURE && !Calls && Message == IDS_ERROR_HIBERNATE_ENABLED && Stream == StdErr);
    Message = -1; Calls = 0; Allowed = 1; opts.remote_system = &Allowed;
    CHECK(Run() == EXIT_FAILURE && !Calls && Message == -1);
    printf("%u hibernation command checks, %u failures\n", Checks, Failures);
    return Failures != 0;
}
'''
program = Path('/tmp/reliefwin-hibernate-command.c')
binary = Path('/tmp/reliefwin-hibernate-command')
program.write_text(fixture + text[start:end] + tests)
subprocess.run(['clang', '-std=c11', '-Wall', '-Wextra', '-Werror', str(program), '-o', str(binary)], check=True)
raise SystemExit(subprocess.run([str(binary)]).returncode)
