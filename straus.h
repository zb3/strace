/*
 * Strauss awareness interface declarations.
 *
 * Copyright (c) 2018-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef STRACE_STRAUS_H
#define STRACE_STRAUS_H

enum { STRAUS_START_VERBOSITY = 5 };

extern const size_t straus_lines;

extern void print_straus(size_t verbosity);

#endif /* STRACE_STRAUS_H */
