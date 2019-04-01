/*
 * Strauss awareness implementation.
 *
 * Copyright (c) 2018-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"

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

void
print_straus(size_t verbosity)
{
	if (verbosity < STRAUS_START_VERBOSITY)
		return;

	verbosity = MIN(verbosity - STRAUS_START_VERBOSITY, straus_lines);

	for (size_t i = 0; i < verbosity; i++)
		puts(straus[i]);
}
