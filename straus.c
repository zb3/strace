/*
 * Strauss awareness implementation.
 *
 * Copyright (c) 2018-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "straus.h"

static const char *straus[] = {
	"",
	"     ____",
	"    /    \\",
	"   |-. .-.|",
	"   (_@)(_@)",
	"   .---_  \\",
	"  /..   \\_/",
	"  |__.-^ /",
	"      }  |",
	"     |   [",
	"     [  ]",
	"    ]   |",
	"    |   [",
	"    [  ]",
	"   /   |        __",
	"  \\|   |/     _/ /_",
	" \\ |   |//___/__/__/_",
	"\\\\  \\ /  //    -____/_",
	"//   \"   \\\\      \\___.-",
	" //     \\\\  __.----._/_",
	"/ //|||\\\\ .-         __>",
	"[        /         __.-",
	"[        [           }",
	"\\        \\          /",
	" \"-._____ \\.____.--\"",
	"    |  | |  |",
	"    |  | |  |",
	"    |  | |  |",
	"    |  | |  |",
	"    {  } {  }",
	"    |  | |  |",
	"    |  | |  |",
	"    |  | |  |",
	"    /  { |  |",
	" .-\"   / [   -._",
	"/___/ /   \\ \\___\"-.",
	"    -\"     \"-",
	"",
	"",
	"",
	"There's nothing more here, really.",
	"",
	"",
	"",
	"...so far.",
	};

const size_t straus_lines = ARRAY_SIZE(straus);

enum {
	STRAUS_BODY_LINES = 37,
	MAX_TIP_LINES = 13,
};

static const char *tips_tricks_tweaks[][MAX_TIP_LINES] = {
	{ "strace has an extensive manual page",
	  "that covers all the possible options",
	  "and contains several useful invocation",
	  "examples." },
	{ "You can use -o|COMMAND to redirect strace's",
	  "output to COMMAND.  This may be useful",
	  "in cases when there is redirection in place",
	  "for the traced program." },
	{ "It's possible to display timestamps",
	  " produced by -r, -t, and -T options",
	  "with nanosecond precision using their",
	  "long variants: --relative-timestamps=ns,",
	  "--absolute-timestamps=ns, and",
	  "--syscall-times=ns, respectively.", "",
	  "Why microseconds are the default?",
	  "To preserve the historic output format",
	  "which was limited by struct timeval",
	  "precision." },
	{ "A particular quote from a particular novel",
	  "by Arthur C. Clarke is printed if an attempt",
	  "is made to attach to a particular process." },
	{ "It's possible to tune the environment",
	  "of the traced process using the -E/--env",
	  "option." },
#if ENABLE_STACKTRACE
	{ "You can print a stack trace for each traced",
	  "call by specifying -k/--stack-traces option.",
#if USE_DEMANGLE
	  "It can even demangle symbol names.",
#endif
	  },
#else
	{ "We would like to tell you that you could",
	  "specify -k/--stack-traces option to print",
	  "stack traces for each traced system call,",
	  "but unfortunately you can't, this strace",
	  "binary is built without stack tracing",
	  "support." },
#endif
	{ "Have you ever hit by accidental overwrite",
	  "of the output file specified in the -o",
	  "option?  Specify -A/--output-append-mode",
	  "as well, and this problem will never annoy",
	  "you again!" },
	{ "strace is about as old as the Linux kernel.",
	  "It has been originally written for SunOS",
	  "by Paul Kranenburg in 1991." },
	{ "strace is able to decode netlink messages.",
	  "It does so automatically for IO performed",
	  "on netlink sockets.  Try it yourself:", "",
	  "    strace -e%network ip a" },
	{ "Filtered syscalls, errors, and signals can",
	  "be specified either by name or by number,",
	  "for example:", "",
	  "    strace --trace=0,1,2 --signal=2,15 true" },
	{ "It is possible to specify -r and -t options",
	  "simultaneously since strace 4.22." },
	{ "Strace can print only successful syscall",
	  "invocations when supplied with",
	  "-z/--successful-only option.  There's also",
	  "a possibility to filter calls with other",
	  "statuses, please refer to -e status option",
	  "documentation." },
	{ "If you trace a process that uses KVM",
	  "subsystem, --kvm=vcpu option may be of use:",
	  "it prints KVM VCPU exit reason.  It requires",
	  "Linux 4.16+, however." },
	{ "It is possible to get strace out of your way",
	  "(in terms of parent/child relationships and",
	  "signal communication) with -D/--daemonize",
	  "option.  Another option that may be of use",
	  "in this case is -I/--interruptible, it",
	  "restricts the set of signals that interrupt",
	  "strace." },
	{ "If strace is too talky to your taste, you",
	  "can silence it with -qqq option." },
	{ "strace prints file paths along with file",
	  "descriptor numbers when it is invoked with",
	  "-y/--decode-fds option.",
	  "When -yy (or --decode-fds=all) is provided,",
	  "it also prints protocol-specific information",
	  "for sockets and device numbers for character",
	  "and block device files." },
	{ "You can control what columns are shown",
	  "in the call summary table produced by -c/-C",
	  "options with -U/--summary-columns option.",
	  "It is a way to print minimum/maximum call",
	  "duration, for example." },
	{ "If you feel that syscall duration shown",
	  "in the call summary table (-c/-C option)",
	  "is not right, you can try to use -w option",
	  "(that collects wall clock time instead),",
	  "maybe that is what you are looking for." },
	{ "strace understands -z option since 2002,",
	  "but it wasn't documented because its",
	  "implementation was broken.  Only 17 years",
	  "later, in strace 5.2, it was properly",
	  "implemented and documented." },
	{ "If you feel that strace is too slow, you may",
	  "want to try --seccomp-bpf option, maybe you",
	  "will feel better." },
	{ "-v is a shorthand for -e abbrev=none and not",
	  " for -e verbose=all. It is idiosyncratic,",
	  "but it is the historic behaviour." },
	{ "strace uses netlink for printing",
	  "protocol-specific information about socket",
	  "descriptors (-yy option)." },
	{ "strace is able to tamper with tracees'",
	  "execution by injecting an arbitrary return",
	  "or error value instead of syscall execution,",
	  "for example:", "",
	  "    strace --inject=unlink:retval=0", "",
	  "would prevent execution of unlink calls, but",
	  "would make the traced process think that",
	  "they have succeeded." },
	{ "strace's tampering capabilities include",
	  "injection of arbitrary return/error values,",
	  "injection of a signal, injection of a delay",
	  "before or after syscall execution." },
	{ "If you want to see numerical values of named",
	  "constants, there is an option for that:",
	  "-X/--const-print-style.  When -Xraw",
	  "(or --const-print-style=raw) is provided,",
	  "strace prints just the numerical value",
	  "of an argument; with -Xverbose, it prints",
	  "values in both numerical and symbolic form." },
	{ "getpid syscall is present on all",
	  "architectures except on Alpha, where getxpid",
	  "syscall (that returns a pair of PID and PPID",
	  "in a pair of registers) is used instead.",
	  "Other two examples of syscalls that utilise",
	  "two registers for their return walues are",
	  "getxuid and getxgid the return a pair",
	  "of real and effective UIDs/GIDs." },
	{ "There are three syscalls that implement",
	  "generic \"open file\" task: open, openat,",
	  "and openat2. On some (newly supported)",
	  "architectures, open syscall is not present.",
	  "How to write a robust filtering expression",
	  "in this case?  With conditional syntax,",
	  "for example:", "",
	  "    strace --trace=?open,?openat,?openat2", "",
	  "You may wont to escape question marks, since",
	  "your shell may interpret them as a path glob",
	  "expression." },
	{ "It is possible to use regular expressions",
	  "for syscall names in the -e trace",
	  "expression, for example:", "",
	  "    strace -e trace=/^sched_.*", "",
	  "will trace all scheduling-related syscalls." },
	{ "IA-64 (Itanium) uses syscall numbers",
	  "beginning from 1024, because numbers",
	  "beginning from 0 were used for i386 compat",
	  "layer.  Other example of a sparse syscall",
	  "table is MIPS, with parts of it beginning",
	  "at index 0 (SVR4 ABI), 1000 (SysV ABI), 2000",
	  "(BSD 4.3 ABI), 3000 (POSIX ABI), 4000 (Linux",
	  "O32 ABI), 5000 (Linux N64 ABI), and 6000",
	  "(Linux N32 ABI)." },
	{ "Der Strauss, the strace's project mascot,",
	  "was conceived in 2017.  It is a brainchild",
	  "of Vitaly Chaykovsky." },
	{ "Medicinal effects of strace can be achieved",
	  "by invoking it with the following options:", "",
	  "    strace -DDDqqq -enone --signal=none" },
	{ "Your Internet connection is working fine.",
	  "No one replies to your e-mails because",
	  "there is no one left." },
	{ "It is possible to print absolute timestamps",
	  "with nanosecond precision using",
	  "--absolute-timestamps=ns option." },
	{ "You smell something not unlike rotting",
	  "meat every time you run me.  I cannot",
	  "promise that the smell will ever go away,",
	  "or that will ever find its source.", "",
	  "But I can promise that you will learn",
	  "to love it." },
	{ "Historically, supplying -o option to strace",
	  "led to silencing of messages about tracee",
	  "attach/detach and personality changes.",
	  "It can be now overridden with --quiet=none",
	  "option." },
	{ "If you open your computer, inside you",
	  "will find a cloth doll, soft and worn.",
	  "You will suddenly remember owning this doll",
	  "as a child, loving and cherishing it.", "",
	  "Do not question the truth of this memory." },
	{ "You can avoid tracing of \"other programs\"",
	  "that are executed by the traced program",
	  "with -b execve option." },
	{ "Touch your monitor.  It's warm, like flesh.",
	  "But it's not flesh.", "Not yet." },
	{ "-F option used to be a separate option",
	  "for following vfork calls." },
	{ "It is possible to provide multiple PIDs",
	  "to a single -p option with whitespace",
	  "or comma as accepted delimiter, in order",
	  "to support usage like", "",
	  "    strace -p \"`pidof PROG`\"",
	  "or",
	  "    strace -p \"`pgrep PROG`\"", "",
	  "pidof uses space as a delimiter, pgrep uses",
	  "newline." },
};

static const char tip_top[] =
	"  ______________________________________________    ";
static const char top_bottom[] =
	" \\______________________________________________/   ";
static const char *tip_left[] = { " / ", " | "};
static const char *tip_right[] = {
	" \\   ",
	" |   ",
	" \\   ",
	"  \\  ",
	"  _\\ ",
	" /   ",
	" |   ", };

bool show_tips = true;


void
print_straus(size_t verbosity)
{
	if (verbosity < STRAUS_START_VERBOSITY)
		return;

	verbosity = MIN(verbosity - STRAUS_START_VERBOSITY, straus_lines);

	for (size_t i = 0; i < verbosity; i++)
		puts(straus[i]);
}

void
print_totd(void)
{
	static bool printed = false;
	const int w = (int) (sizeof(tip_top) - 1 - strlen(tip_left[0])
				- strlen(tip_right[0]));
	struct timeval tv;
	size_t id;
	size_t i;

	if (printed || !show_tips)
		return;

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec ^ tv.tv_usec);
	id = rand() % ARRAY_SIZE(tips_tricks_tweaks);

	fprintf(stderr, "%s%s\n", tip_top, straus[1]);
	fprintf(stderr, "%s%-*s%s%s\n",
		tip_left[0], w, "", tip_right[0], straus[2]);
	for (i = 0; (i < MAX_TIP_LINES) && (tips_tricks_tweaks[id][i] ||
					    (i < (ARRAY_SIZE(tip_right) - 1)));
	     i++) {
		fprintf(stderr, "%s%-*.*s%s%s\n",
			tip_left[MIN(i + 1, ARRAY_SIZE(tip_left) - 1)],
			w, w, tips_tricks_tweaks[id][i] ?: "",
			tip_right[MIN(i + 1, ARRAY_SIZE(tip_right) - 1)],
			straus[MIN(3 + i, STRAUS_BODY_LINES)]);
	}
	fprintf(stderr, "%s%s\n",
		top_bottom, straus[MIN(3 + i, STRAUS_BODY_LINES)]);
	fprintf(stderr, "%*s%*s%*s%s\n",
		(int) strlen(tip_left[0]), "",
		w, "",
		(int) strlen(tip_right[0]), "",
		straus[MIN(4 + i, STRAUS_BODY_LINES)]);

	printed = true;
}
