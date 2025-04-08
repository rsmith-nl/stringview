// file: str.h
// vim:fileencoding=utf-8:ft=cpp:tabstop=2
//
// Copyright © 2025 R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: MIT
// Created: 2025-04-07 22:53:56 +0200
// Last modified: 2025-04-08T17:07:51+0200

#pragma once
#include <stddef.h>  // ptrdiff_t
#include <stdbool.h>  // bool

#define S8(s)    (s8){s, sizeof(s)-1}

// Note that these strings do not allocate memory.
// Memory should be provided by e.g. an arena.

typedef struct {
  char *data;
  ptrdiff_t len;
} s8;

#ifdef __cplusplus
extern "C" {
#endif

// Test two strings for equality of length and content.
bool s8equals(s8 a, s8 b);

// Remove whitespace from the left of a string.
s8 s8lstrip(s8 s);

// Remove whitespace from the right of a string.
s8 s8rstrip(s8 s);

// Remove whitespace from both ends of the string.
s8 s8strip(s8 s);

// Return the offset of the string f in the string s.
// If not found, return -1.
ptrdiff_t s8find(s8 s, s8 f);

#ifdef __cplusplus
}
#endif
