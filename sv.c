// file: sv.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
//
// Copyright © 2025 R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: MIT
// Created: 2025-04-07 22:53:56 +0200
// Last modified: 2025-04-09T10:13:15+0200

#include <assert.h>
#include <string.h>

#include "sv.h"

bool sv8equals(Sv8 a, Sv8 b)
{
  return (a.len == b.len) && (!a.len || !memcmp(a.data, b.data, a.len));
}

Sv8 sv8lstrip(Sv8 s)
{
  while (s.len && *s.data<=' ') {
    s.data++, s.len--;
  }
  return s;
}

Sv8 sv8rstrip(Sv8 s)
{
  while (s.len && s.data[s.len-1]<=' ') {
    s.len--;
  }
  return s;
}

Sv8 sv8strip(Sv8 s)
{
  while (s.len && *s.data<=' ') {
    s.data++, s.len--;
  }
  while (s.len && s.data[s.len-1]<=' ') {
    s.len--;
  }
  return s;
}

ptrdiff_t sv8find(Sv8 s, Sv8 f)
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

ptrdiff_t sv8count(Sv8 s, char f)
{
  ptrdiff_t rv = 0;
  for (ptrdiff_t j = 0; j < s.len; j++) {
    if (s.data[j] == f) {
      rv++;
    }
  }
  return rv;
}

Sv8 sv8span(char *beg, char *end)
{
    Sv8 r = {0};
    r.data = beg;
    r.len  = beg ? end-beg : 0;
    return r;
}

Cut sv8cut(Sv8 s, char c)
{
  Cut r = {0};
  if (s.len == 0) {
    return r;
  }
  char *beg = s.data;
  char *end = s.data + s.len;
  char *cut = beg;
  for (; cut<end && *cut!=c; cut++) {}
  r.ok   = cut < end;
  r.head = sv8span(beg, cut);
  r.tail = sv8span(cut+r.ok, end);
  return r;
}

