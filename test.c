// file: test.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-04-09 00:08:50 +0200
// Last modified: 2025-09-08T23:20:27+0200

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include "stringview.h"

// For condition testing, e.g. a == b.
#define test(exp) \
  do \
    if (exp) { \
      printf("\033[0;32mPASSED:\033[0m " #exp "\n"); \
    } else { \
      printf("\033[1;31mFAILED:\033[0m " #exp "\n"); \
    } \
  while (0)

#define xstr(a) str(a)
#define str(a) #a

#undef UNUSED
#define UNUSED(x)(void)(x)

int main(int argc, char *argv[])
{
  UNUSED(argc);
  UNUSED(argv);
  Sv8 a = SV8("test"), b = SV8("tast"), c = SV8(" test"), d = SV8("test ");
  puts("Sv8 a = SV8(\"test\"), b = SV8(\"tast\"), c = SV8(\" test\"), d = SV8(\"test \");");
  Sv8 n = {0};
  puts("Sv8 n = {0};");
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
  puts("Sv8 orig = SV8(\"first\\nlast\");");
  Sv8Cut ct = sv8cut(orig, '\n');
  puts("Sv8Cut ct = sv8cut(orig, '\\n');");
  test(ct.ok == true);
  if (ct.ok) {
    test(sv8equals(ct.head, SV8("first")));
    test(sv8equals(ct.tail, SV8("last")));
  }
  orig = SV8("100  0 Hyer's carbon fiber");
  ct = sv8lsplit(orig);
  puts("orig = SV8(\"100  0 Hyer's carbon fiber\");");
  puts("ct = sv8lsplit(orig);");
  test(ct.ok == true);
  if (ct.ok) {
    test(sv8equals(ct.head, SV8("100")));
    test(sv8equals(ct.tail, SV8("0 Hyer's carbon fiber")));
  }
  Sv8Int rv = {0};
  rv = sv8toi(SV8("00100"));
  puts("rv = sv8toi(SV8(\"00100\"));");
  test(rv.ok);
  test(rv.result == 100);
  test(rv.tail.len == 0);
  rv = sv8toi(SV8("-23"));
  puts("rv = sv8toi(SV8(\"-23\"));");
  test(rv.ok);
  test(rv.result == -23);
  test(rv.tail.len == 0);
  rv = sv8toi(SV8("+742"));
  puts("rv = sv8toi(SV8(\"+742\"));");
  test(rv.ok);
  test(rv.result == 742);
  test(rv.tail.len == 0);
  rv = sv8toi(SV8("00foo"));
  puts("rv = sv8toi(SV8(\"00foo\"));");
  test(rv.ok);
  test(rv.result == 0);
  test(sv8equals(rv.tail, SV8("foo")));
  rv = sv8toi(SV8("0"));
  puts("rv = sv8toi(SV8(\"0\"));");
  test(rv.ok);
  test(rv.result == 0);
  rv = sv8toi(SV8("-7bar"));
  puts("rv = sv8toi(SV8(\"-7bar\"));");
  test(rv.ok);
  test(rv.result == -7);
  test(sv8equals(rv.tail, SV8("bar")));
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
  // Hashes calculated with Python:
  //>>> def hash64(s):
  //...     h = 0x100
  //...     for c in s:
  //...         h ^= (c & 255)
  //...         h *= 1111111111111111111
  //...         h = h % 2**64
  //...     return h
  test(sv8hash64(SV8("this is a test"))==0x102ab6cd62d10f01);
  test(sv8hash64(SV8("foo"))==0x481ef248036ba4b4);
  return 0;
}
