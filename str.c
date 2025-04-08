// file: str.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
//
// Copyright © 2025 R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: MIT
// Created: 2025-04-07 22:53:56 +0200
// Last modified: 2025-04-08T15:50:03+0200

#include <assert.h>
#include <string.h>

#include "str.h"

bool str_equals(Str a, Str b) {
    return (a.len == b.len) && (!a.len || !memcmp(a.data, b.data, a.len));
}

Str str_lstrip(Str s) {
    while (s.len && *s.data<=' ') {s.data++, s.len--;}
    return s;
}

Str str_rstrip(Str s) {
    while (s.len && s.data[s.len-1]<=' ') {s.len--;}
    return s;
}

Str str_strip(Str s) {
    while (s.len && *s.data<=' ') {s.data++, s.len--;}
    while (s.len && s.data[s.len-1]<=' ') {s.len--;}
    return s;
}

ptrdiff_t str_find(Str s, Str f) {
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
