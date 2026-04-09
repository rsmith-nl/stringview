// file: test.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-04-09 00:08:50 +0200
// Last modified: 2026-04-07T23:05:13+0200

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#define STRINGVIEW_IMPLEMENTATION
#include "single_header/stringview.h"

#define BOLD_WHITE "\033[1;37m"
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define BOLD_GREEN "\033[1;32m"
#define PURPLE "\033[0;35m"
#define BOLD_RED "\033[1;31m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BOLD_YELLOW "\033[1;33m"
#define RESET "\033[0m"  // No Color


// For condition testing, e.g. a == b.
#define test(exp) \
  do \
    if (exp) { \
      printf("\033[0;32mPASSED:\033[0m " #exp "\n"); \
    } else { \
      printf("\033[1;31mFAILED:\033[0m " #exp "\n"); \
      failcount++; \
    } \
  while (0)

#define xstr(a) str(a)
#define str(a) #a

#undef UNUSED
#define UNUSED(x)(void)(x)

int main(int argc, char *argv[])
{
  int32_t failcount = 0;
  UNUSED(argc);
  UNUSED(argv);
  puts(PURPLE"Starting the test"RESET);
  Sv8 a = SV8("test"), b = SV8("tast"), c = SV8(" test"), d = SV8("test ");
  Sv8 e = SV8("exothermic"), f = SV8("exo"), g = SV8("ermic"), h = SV8("othe");
  Sv8 p = SV8("partition");
  puts("Sv8 a = SV8(\"test\"), b = SV8(\"tast\"), c = SV8(\" test\"), d = SV8(\"test \");");
  Sv8 n = {0};
  puts("Sv8 n = {0};");
  test(a.len == 4);
  test(c.len == 5);
  puts(CYAN"Running tests for sv8equals"RESET);
  test(sv8equals(a, a));
  test(!sv8equals(a, n));
  test(!sv8equals(a, b));
  test(!sv8equals(a, c));
  test(sv8equals(a, sv8lstrip(c)));
  test(sv8equals(a, sv8rstrip(d)));
  puts(CYAN"Running tests for sv8startswith/endswith"RESET);
  test(sv8startswith(e, f));
  test(sv8endswith(e, g));
  puts(CYAN"Running tests for sv8contains"RESET);
  test(sv8contains(e, h));
  test(sv8contains(e, f));
  test(sv8contains(e, g));
  puts(CYAN"Running tests for sv8find"RESET);
  test(sv8find(a, SV8("st")) == 2);
  test(sv8find(a, n) == -1);
  test(sv8find(a, SV8("fo")) == -1);
  puts(CYAN"Running tests for sv8?index"RESET);
  test(sv8lindex(e, 'q') == -1);
  test(sv8lindex(e, 'o') == 2);
  test(sv8rindex(e, 'q') == -1);
  test(sv8rindex(e, 'r') == 6);
  puts(CYAN"Running tests for sv8count"RESET);
  test(sv8count(a, 't') == 2);
  test(sv8count(a, 'e') == 1);
  test(sv8count(a, 'q') == 0);
  puts(CYAN"Running tests for sv8cut"RESET);
  Sv8 orig = SV8("first\nlast");
  puts("Sv8 orig = SV8(\"first\\nlast\");");
  Sv8Cut ct = sv8cut(orig, '\n');
  puts("Sv8Cut ct = sv8cut(orig, '\\n');");
  test(ct.ok == true);
  if (ct.ok) {
    test(sv8equals(ct.head, SV8("first")));
    test(sv8equals(ct.tail, SV8("last")));
  }
  puts("ct = sv8cuts(p, SV8(\"tit\"));");
  ct = sv8cuts(p, SV8("tit"));
  test(ct.ok == true);
  puts(CYAN"Running tests for sv8split"RESET);
  orig = SV8("100  0 Hyer's carbon fiber");
  ct = sv8lsplit(orig);
  puts("orig = SV8(\"100  0 Hyer's carbon fiber\");");
  puts("ct = sv8lsplit(orig);");
  test(ct.ok == true);
  if (ct.ok) {
    test(sv8equals(ct.head, SV8("100")));
    test(sv8equals(ct.tail, SV8("0 Hyer's carbon fiber")));
  }
  test(sv8equals(sv8lskip(a, 2), SV8("st")));
  test(sv8equals(sv8rskip(a, 2), SV8("te")));
  puts(CYAN"Running tests for sv8toi"RESET);
  Sv8Int rv = {0};
  rv = sv8toi(SV8("00100"));
  puts("rv = sv8toi(SV8(\"00100\"));");
  test(rv.ok == true);
  test(rv.result == 100);
  test(rv.tail.len == 0);
  rv = sv8toi(SV8("-23"));
  puts("rv = sv8toi(SV8(\"-23\"));");
  test(rv.ok == true);
  test(rv.result == -23);
  test(rv.tail.len == 0);
  rv = sv8toi(SV8("+742"));
  puts("rv = sv8toi(SV8(\"+742\"));");
  test(rv.ok == true);
  test(rv.result == 742);
  test(rv.tail.len == 0);
  rv = sv8toi(SV8("00foo"));
  puts("rv = sv8toi(SV8(\"00foo\"));");
  test(rv.ok == true);
  test(rv.result == 0);
  test(sv8equals(rv.tail, SV8("foo")));
  rv = sv8toi(SV8("0"));
  puts("rv = sv8toi(SV8(\"0\"));");
  test(rv.ok == true);
  test(rv.result == 0);
  rv = sv8toi(SV8("-7bar"));
  puts("rv = sv8toi(SV8(\"-7bar\"));");
  test(rv.ok == true);
  test(rv.result == -7);
  test(sv8equals(rv.tail, SV8("bar")));
  puts(CYAN"Test 32-bit number (107374182)"RESET);
  rv = sv8toi(SV8("107374182"));
  test(rv.ok == true);
  test(rv.is32bits == true);
  test(rv.result == 107374182);
  puts(CYAN"Test 64-bit number (6442450941)"RESET);
  rv = sv8toi(SV8("6442450941"));
  test(rv.ok == true);
  test(rv.is32bits == false);
  test(rv.result == 6442450941);
  puts(CYAN"Test overflow (27670116110564327421)"RESET);
  rv = sv8toi(SV8("27670116110564327421"));
  test(rv.ok == false);
  test(rv.overflow == true);
  puts(CYAN"Running tests for sv8tod"RESET);
  Sv8Double rv2 = {0};
  rv2 = sv8tod(SV8("-13.623e5 Pa"));
  puts("rv2 = sv8tod(SV8(\"-13.623e5 Pa\"));");
  test(rv2.ok && fabs(rv2.result - -1.3623e+06) < 0.001);
  test(sv8equals(rv2.tail, SV8(" Pa")));
  rv2 = sv8tod(SV8("0003.24e+002"));
  puts("rv2 = sv8tod(SV8(\"0003.24e+002\"));");
  test(rv2.ok && fabs(rv2.result - 3.24e2) < 0.001);
  rv2 = sv8tod(SV8("238000 0.23"));
  puts("rv2 = sv8tod(SV8(\"238000 0.23\"));");
  test(rv2.ok && fabs(rv2.result - 238000) < 0.001);
  rv2 = sv8tod(SV8("0 "));
  puts("rv2 = sv8tod(SV8(\"0 \"));");
  test(rv2.ok && fabs(rv2.result - 0) < 0.001);
  rv2 = sv8tod(SV8("0"));
  puts("rv2 = sv8tod(SV8(\"0\"));");
  test(rv2.ok && fabs(rv2.result - 0) < 0.001);
  rv2 = sv8tod(SV8("3.1e-3"));
  puts("rv2 = sv8tod(SV8(\"3.1e-3\"));");
  test(rv2.ok && fabs(rv2.result - 3.1e-3) < 0.001);
  rv2 = sv8tod(SV8("0.07496842391515"));
  puts("rv2 = sv8tod(SV8(\"0.07496842391515\"));");
  test(rv2.ok && fabs(rv2.result - 7.496e-2) < 0.001);
  puts(CYAN"Running tests for sv8hash64"RESET);
  // Hashes calculated with Python:
  //>>> def hash64(s):
  //...     h = 0x100
  //...     for c in s:
  //...         h ^= (c & 255)
  //...         h *= 1111111111111111111
  //...         h = h % 2**64
  //...     return h
  test(sv8hash64(SV8("this is a test")) == 0x102ab6cd62d10f01);
  test(sv8hash64(SV8("foo")) == 0x481ef248036ba4b4);
  puts(PURPLE"***Result ***"RESET);
  if (failcount == 0) {
    puts(GREEN"+++ All tests PASSED! +++"RESET);
  } else {
    printf("\033[1;31m--- %d failing tests.---\033[0m\n", failcount);
  }
  return failcount;
}
