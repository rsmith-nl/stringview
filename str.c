// file: str.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
//
// Copyright © 2025 R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: MIT
// Created: 2025-04-07 22:53:56 +0200
// Last modified: 2025-04-08T16:02:59+0200

#include <assert.h>
#include <string.h>

#include "str.h"

bool s8equals(s8 a, s8 b)
{
  return (a.len == b.len) && (!a.len || !memcmp(a.data, b.data, a.len));
}

s8 s8lstrip(s8 s)
{
  while (s.len && *s.data<=' ') {
    s.data++, s.len--;
  }
  return s;
}

s8 s8rstrip(s8 s)
{
  while (s.len && s.data[s.len-1]<=' ') {
    s.len--;
  }
  return s;
}

s8 s8strip(s8 s)
{
  while (s.len && *s.data<=' ') {
    s.data++, s.len--;
  }
  while (s.len && s.data[s.len-1]<=' ') {
    s.len--;
  }
  return s;
}

ptrdiff_t s8find(s8 s, s8 f)
{
  if (s.len == 0 || f.len == 0 || f.len > s.len) {
    return -1;
  }
  ptrdiff_t maxoff = s.len - f.len;
  for (ptrdiff_t off = 0; off <= maxoff ; off++) {
    if (memcmp(s.data+off, f.data, f.len) == 0) {
      return off;
    }
  }
  return -1;
}
