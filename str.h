// file: str.h
// vim:fileencoding=utf-8:ft=cpp:tabstop=2
//
// Copyright © 2025 R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: MIT
// Created: 2025-04-07 22:53:56 +0200
// Last modified: 2025-04-07T23:15:39+0200

#pragma once
#include <stddef.h>  // ptrdiff_t
#include <stdbool.h>  // bool

#define S(s)    (Str){s, sizeof(s)-1}

typedef struct {
    char *data;
    ptrdiff_t len;
} Str;

#ifdef __cplusplus
extern "C" {
#endif

bool str_equals(Str a, Str b);
Str str_lstrip(Str s);
Str str_rstrip(Str s);
Str str_strip(Str s);

#ifdef __cplusplus
}
#endif
