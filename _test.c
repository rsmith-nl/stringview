// file: _test.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
//
// Copyright © 2025 R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: MIT
// Created: 2025-04-09 00:08:50 +0200
// Last modified: 2025-04-09T00:36:11+0200

#include <stdio.h>
#include "str.h"

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
  s8 a = S8("test"), b = S8("tast"), c = S8(" test");
  s8 n = {0};
  test(a.len == 4);
  test(c.len == 5);
  test(s8equals(a, a));
  test(!s8equals(a, n));
  test(!s8equals(a, b));
  test(!s8equals(a, c));
  test(s8equals(a, s8lstrip(c)));
  test(s8find(a, S8("st")) != -1);
  test(s8find(a, n) == -1);
  test(s8find(a, S8("fo")) == -1);
  test(s8count(a, 't') == 2);
  test(s8count(a, 'e') == 1);
  test(s8count(a, 'q') == 0);
  return 0;
}
