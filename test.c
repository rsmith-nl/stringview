// file: _test.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
//
// Copyright © 2025 R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: MIT
// Created: 2025-04-09 00:08:50 +0200
// Last modified: 2025-08-05T22:24:11+0200

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stringview.h"

// For condition testing, e.g. a == b.
#define test(exp) \
  do \
    if (exp) { \
      fprintf(stderr, "\033[0;32mPASSED:\033[0m " #exp "\n"); \
    } else { \
      fprintf(stderr, "\033[1;31mFAILED:\033[0m " #exp "\n"); \
    } \
  while (0)

#undef UNUSED
#define UNUSED(x)(void)(x)

int main(int argc, char *argv[])
{
  UNUSED(argc);
  UNUSED(argv);
  Sv8 a = SV8("test"), b = SV8("tast"), c = SV8(" test"), d = SV8("test ");
  Sv8 n = {0};
  test(a.len == 4);
  test(c.len == 5);
  test(sv8equals(a, a));
  test(!sv8equals(a, n));
  test(!sv8equals(a, b));
  test(!sv8equals(a, c));
  test(sv8equals(a, sv8lstrip(c)));
  test(sv8equals(a, sv8rstrip(d)));
  test(sv8find(a, SV8("st")) != -1);
  test(sv8find(a, n) == -1);
  test(sv8find(a, SV8("fo")) == -1);
  test(sv8count(a, 't') == 2);
  test(sv8count(a, 'e') == 1);
  test(sv8count(a, 'q') == 0);
  Sv8 orig = SV8("first\nlast");
  Sv8Cut ct = sv8cut(orig, '\n');
  test(ct.ok == true);
  if (ct.ok) {
    test(sv8equals(ct.head, SV8("first")));
    test(sv8equals(ct.tail, SV8("last")));
  }
  orig = SV8("100  0 Hyer's carbon fiber");
  ct = sv8lsplit(orig);
  test(ct.ok == true);
  if (ct.ok) {
    test(sv8equals(ct.head, SV8("100")));
    test(sv8equals(ct.tail, SV8("0 Hyer's carbon fiber")));
  }
  Sv8Int rv;
  rv = sv8toi(SV8("00100"));
  test(rv.ok && rv.result == 100 && rv.tail.len == 0);
  rv = sv8toi(SV8("-23"));
  test(rv.ok && rv.result == -23 && rv.tail.len == 0);
  rv = sv8toi(SV8("+742"));
  test(rv.ok && rv.result == 742 && rv.tail.len == 0);
  rv = sv8toi(SV8("00foo"));
  test(rv.ok && rv.result == 0 && sv8equals(rv.tail, SV8("foo")));
  rv = sv8toi(SV8("-7bar"));
  test(rv.ok && rv.result == -7 && sv8equals(rv.tail, SV8("bar")));
  return 0;
}
