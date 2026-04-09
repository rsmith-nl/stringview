// file: stringview.h
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-04-07 22:53:56 +0200
// Last modified: 2026-03-28T10:03:18+0100

// Inspired by: https://nullprogram.com/blog/2023/10/08/

#pragma once
#include <stddef.h>     // ptrdiff_t
#include <stdbool.h>    // bool
#include <stdint.h>     // int32_t

// The following works with string constants.
// Note that it does not work with members of an array of char pointers!
#define SV8(s)    (Sv8){s, sizeof(s)-1}

// Note that these structures do not allocate memory.
// So they are *views* of a existing string in memory somewhere.

// String view
typedef struct {
  char *data;
  ptrdiff_t len;
} Sv8;

typedef struct {
  Sv8 head;
  Sv8 tail;
  bool ok;
} Sv8Cut;

typedef struct {
  double result;
  Sv8 tail;
  bool ok;
} Sv8Double;

typedef struct {
  int64_t result;
  Sv8 tail;
  bool overflow, is32bits;
  bool ok;
} Sv8Int;

#ifdef __cplusplus
extern "C" {
#endif

// Test two strings for equality of length and content.
extern bool sv8equals(Sv8 a, Sv8 b);

// Return true if s starts with t.
extern bool sv8startswith(Sv8 s, Sv8 t);

// Return true if s ends with e.
extern bool sv8endswith(Sv8 s, Sv8 e);

// Return true if s contained within c.
extern bool sv8contains(Sv8 s, Sv8 c);

// Remove whitespace from the left of a string.
extern Sv8 sv8lstrip(Sv8 s);

// Remove whitespace from the right of a string.
extern Sv8 sv8rstrip(Sv8 s);

// Remove whitespace from both ends of the string.
extern Sv8 sv8strip(Sv8 s);

// Strip num characters from the left of the string.
extern Sv8 sv8lskip(Sv8 s, int32_t num);

// Strip num characters from the right of the string.
extern Sv8 sv8rskip(Sv8 s, int32_t num);

// Return the offset of the string f in the string s.
// If not found, return -1.
extern ptrdiff_t sv8find(Sv8 s, Sv8 f);

// Return the amount of occurrances of the character f in string s.
extern ptrdiff_t sv8count(Sv8 s, char f);

// Return the index of the first instance of c starting from the left, or -1.
extern ptrdiff_t sv8lindex(Sv8 s, char c);

// Return the index of the first instance of c starting from the left, or -1.
extern ptrdiff_t sv8rindex(Sv8 s, char c);

// Cut the string s at the character c.
extern Sv8Cut sv8cut(Sv8 s, char c);

// Cut the string s at the stringview v.
extern Sv8Cut sv8cuts(Sv8 s, Sv8 v);

// Return the Sv8 that stretched from beg to end.
extern Sv8 sv8span(char *beg, char *end);

// Cut the string at the first whitespace from the left.
extern Sv8Cut sv8lsplit(Sv8 s);

// Tries to read an int32_t from the beginning of a stringview.
// Leading whitespace is skipped.
// If an integer was found, Sv8Int.ok equals true, otherwise false.
// A view of the remaining string is returned in Sv8Int.tail.
extern Sv8Int sv8toi(Sv8 s);

// Tries to read a double from the beginning of a stringview.
// Leading whitespace is skipped.
// If an floating point number was found, Sv8Double.ok equals true, otherwise false.
// A view of the remaining string is returned in Sv8Double.tail.
// Does not handle NaN and Inf!
Sv8Double sv8tod(Sv8 s);

// Hash a string view with FNV-1a hash function.
extern uint64_t sv8hash64(Sv8 s);

// Return up to 255 bytes of the Sv8 as a C string.
// This returns a pointer to a static buffer.
// As an alternative for printing, you could use ``printf("%.*s\n", s.len, s.data)``.
extern char *sv8cstring(Sv8 s);

#ifdef __cplusplus
}
#endif

#ifdef STRINGVIEW_IMPLEMENTATION

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static bool _isspace(char c)
{
  if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f') {
    return true;
  }
  return false;
}

bool sv8equals(Sv8 a, Sv8 b)
{
  return (a.len == b.len) && (!a.len || !memcmp(a.data, b.data, a.len));
}

bool sv8startswith(Sv8 s, Sv8 t)
{
  if (s.len == 0 || t.len == 0 || t.len > s.len) {
    return false;
  }
  int32_t j = 0;
  while (t.data[j] == s.data[j]) {
    if (j == t.len - 1) {
      return true;
    }
    j++;
  }
  return false;
}

bool sv8endswith(Sv8 s, Sv8 e)
{
  if (s.len == 0 || e.len == 0 || e.len > s.len) {
    return false;
  }
  char *ptr = s.data + (s.len - e.len);
  int32_t j = 0;
  while (ptr[j] == e.data[j]) {
    if (j == e.len - 1) {
      return true;
    }
    j++;
  }
  return false;
}

bool sv8contains(Sv8 s, Sv8 c)
{
  if (s.len == 0 || c.len == 0 || c.len > s.len) {
    return false;
  }
  for (int32_t j = 0; j <= (s.len - c.len); j++) {
    if (s.data[j] == c.data[0]) {
      // Check from end first
      int32_t matchcount = 0;
      for (int32_t k = c.len - 1; k >= 0; k--) {
        if (s.data[j + k] == c.data[k]) {
          matchcount++;
        }
      }
      if (matchcount == c.len) {
        return true;
      }
    }
  }
  return false;
}

Sv8 sv8lstrip(Sv8 s)
{
  while (s.len && _isspace(*s.data)) {
    s.data++, s.len--;
  }
  return s;
}

Sv8 sv8rstrip(Sv8 s)
{
  while (s.len && (_isspace(s.data[s.len - 1]) || s.data[s.len - 1] == 0)) {
    s.len--;
  }
  return s;
}

Sv8 sv8strip(Sv8 s)
{
  while (s.len && _isspace(*s.data)) {
    s.data++, s.len--;
  }
  while (s.len && (_isspace(s.data[s.len - 1]) || s.data[s.len - 1] == 0)) {
    s.len--;
  }
  return s;
}

Sv8 sv8lskip(Sv8 s, int32_t num)
{
  if (num >= s.len) {
    return s;
  }
  s.data += num;
  s.len -= num;
  return s;
}

Sv8 sv8rskip(Sv8 s, int32_t num)
{
  if (num >= s.len) {
    return s;
  }
  s.len -= num;
  return s;
}

ptrdiff_t sv8find(Sv8 s, Sv8 f)
{
  if (s.len == 0 || f.len == 0 || f.len > s.len) {
    return -1;
  }
  ptrdiff_t maxoff = s.len - f.len;
  for (ptrdiff_t off = 0; off <= maxoff ; off++) {
    if (memcmp(s.data + off, f.data, f.len) == 0) {
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

ptrdiff_t sv8lindex(Sv8 s, char c)
{
  for (int32_t j = 0; j < s.len; j++) {
    if (s.data[j] == c) {
      return j;
    };
  }
  return -1;
}

ptrdiff_t sv8rindex(Sv8 s, char c)
{
  for (int32_t j = s.len - 1; j >= 0; j--) {
    if (s.data[j] == c) {
      return j;
    };
  }
  return -1;
}

Sv8 sv8span(char *beg, char *end)
{
  Sv8 r = {0};
  r.data = beg;
  r.len  = beg ? end - beg : 0;
  return r;
}

Sv8Cut sv8cut(Sv8 s, char c)
{
  Sv8Cut r = {0};
  if (s.len == 0) {
    return r;
  }
  char *beg = s.data;
  char *end = s.data + s.len;
  char *cut = beg;
  for (; cut < end && *cut != c; cut++) {}
  r.ok   = cut < end;
  r.head = sv8span(beg, cut);
  r.tail = sv8span(cut + r.ok, end);
  return r;
}

Sv8Cut sv8cuts(Sv8 s, Sv8 v)
{
  Sv8Cut rv = {0};
  ptrdiff_t ix = sv8find(s, v);
  if (ix == -1) {
    return rv;
  }
  rv.head = s;
  rv.head.len = ix;
  rv.tail.data = s.data + ix + v.len;
  rv.tail.len = s.len - ix - v.len;
  rv.ok = true;
  return rv;
}

Sv8Cut sv8lsplit(Sv8 s)
{
  Sv8Cut r = {0};
  if (s.len == 0) {
    return r;
  }
  char *beg = s.data;
  char *end = s.data + s.len;
  char *cut = beg;
  char *startws = 0, *endws = 0;
  while (cut < end) {
    if (_isspace(*cut)) { // whitespace
      if (startws == 0) {
        startws = cut;
      }
    } else { // not whitespace
      if (startws != 0) {
        endws = cut;
        break;
      }
    }
    cut++;
  }
  r.ok   = cut < end;
  r.head = sv8span(beg, startws);
  r.tail = sv8span(endws, end);
  return r;
}

Sv8Int sv8toi(Sv8 s)
{
  // This scanner is implemented as a state machine.
  // See sv8toi.pdf to see its diagram.
  Sv8Int rv = {0};
  s = sv8lstrip(s);
  char *beg = s.data;
  char *end = s.data + s.len;
  int32_t state = 0;
  int64_t number = 0;
  bool negative = false;
  bool stop = false;
  while (beg < end && !stop) {
    char c = *beg++;
    switch (state) {
      case 0:   // Start state.
        if (c == '+') {
          state = 1;
        } else if (c == '-') {
          negative = true;
          state = 1;
        } else if (c == '0') {
          state = 2;
        } else if (c >= '1' && c <= '9') {
          state = 3;
          number = c - '0';
        } else {
          goto fail2;
        }
        break;
      case 1:   // After a leading + or -.
        if (c == '0') {
          state = 2;
        } else if (c >= '1' && c <= '9') {
          state = 3;
          number = c - '0';
        } else {
          goto fail2;
        }
        break;
      case 2:   // Skip leading zeros.
        if (c == '0') {
          state = 2;
        } else if (c >= '1' && c <= '9') {
          state = 3;
          number = c - '0';
        } else {
          // return 0.
          stop = true;
        }
        break;
      case 3:   // Process digits.
        if (c >= '0' && c <= '9') {
          state = 3;
          if (INT64_MAX / number < 10) { // will overflow.
            goto fail3;
          }
          number = number * 10 + c - '0';
        } else {
          stop = true; // non-numeric character
        }
        break;
    } // switch
  } // while
  if (stop == true) {
    beg--;
  }
  rv.result = number;
  if (negative) {
    rv.result *= -1;
  }
  rv.tail = sv8span(beg, end);
  rv.ok = true;
  if (rv.result > INT32_MIN && rv.result < INT32_MAX) {
    rv.is32bits = true;
  }
  return rv;
fail3:
  rv.overflow = true;
fail2:
  rv.ok = false;
  rv.result = 0;
  rv.tail = s;
  return rv;
}

Sv8Double sv8tod(Sv8 s)
{
  // This scanner is implemented as a state machine.
  // See sv8tod.pdf to see its diagram.
  // Diagram concept from:
  //   https://www.codeproject.com/Articles/5291098/\
  //   Fun-With-State-Machines-Incrementally-Parsing-Numb
  Sv8Double rv = {0};
  int32_t state = 0;
  int64_t whole = 0, fractional = 0, fpower = 1, exponent = 0;
  bool stop = false;
  bool neg_num = false, neg_exp = false;
  s = sv8lstrip(s);
  char *beg = s.data;
  char *end = s.data + s.len;
  while (beg < end && !stop) {
    char c = *beg++;
    switch (state) {
      case 0:   // start state.
        if (c == '0') {
          state = 2;
        } else if (c == '-') {
          state = 1;
          neg_num = true;
        } else if (c == '+') {
          state = 1;
        } else if (c == '0') {
          state = 2;
        } else if (c >= '1' && c <= '9') {
          state = 8;
          whole = c - '0';
        } else {
          goto fail1; // invalid start token
        }
        break;
      case 1:   // After a leading + or -.
        if (c == '0') {
          state = 2;
        } else if (c >= '1' && c <= '9') {
          state = 8;
          whole = c - '0';
        } else {
          goto fail1; // non-number after starting '-'.
        }
        break;
      case 2:   // Skip leading zeroes.
        if (c == '.') {
          state = 3;
        } else if (c == 'e' || c == 'E') {
          state = 5;
        } else if (c == '0') {
          state = 2;
        } else if (c >= '1' && c <= '9') {
          state = 8;
          whole = c - '0';
        } else {
          // return 0.
          rv.result = neg_num ? -0.0 : 0.0;
          rv.ok = true;
          rv.tail = sv8span(beg - 1, end);
          return rv;
        }
        break;
      case 3:   // Decimal digits
        if (c >= '0' && c <= '9') {
          state = 3;
          fpower *= 10;
          fractional = 10 * fractional + c - '0';
        } else if (c == 'e' || c == 'E') {
          state = 5;
        } else {
          stop = true;
        }
        break;
      case 5:   // Found ‘e’ or ‘E’.
        if (c == '0') {
          state = 9;
        } else if (c >= '1' && c <= '9') {
          state = 7;
          exponent = 10 * exponent + c - '0';
        } else if (c == '+') {
          state = 6;
        } else if (c == '-') {
          state = 6;
          neg_exp = true;
        } else {
          goto fail1; // e or E without number.
        }
        break;
      case 6:   // Found + or - in exponent.
        if (c == '0') {
          state = 9;
        } else if (c >= '1' && c <= '9') {
          state = 7;
          exponent = 10 * exponent + c - '0';
        } else {
          goto fail1; // not a number after [eE][+-]
        }
        break;
      case 7:   // Parse exponent digits.
        if (c >= '0' && c <= '9') {
          state = 7;
          exponent = 10 * exponent + c - '0';
        } else {
          stop = true;
        }
        break;
      case 8:   // Process digits before the decimal point.
        if (c == '.') {
          state = 3;
        } else if (c >= '0' && c <= '9') {
          state = 8;
          whole = 10 * whole + c - '0';
        } else if (c == 'e' || c == 'E') {
          state = 5;
        } else {
          stop = true; // in this case it is a whole number.
        }
        break;
      case 9:   // Skip 0 digits in exponent.
        if (c == '0') {
          state = 9;
        } else if (c >= '1' && c <= '9') {
          state = 7;
          exponent = 10 * exponent + c - '0';
        } else {
          goto fail1; // not a number after leading 0 in exponent.
        }
        break;
    } // switch
  } // while
  if (exponent >= 308) {
    goto fail1; // exponent too large
  } else if (neg_exp == true) {
    exponent *= -1;
  }
  if (stop == true) {
    beg--;
  }
  rv.ok = true;
  rv.result = (double)whole + ((double)fractional) / fpower;
  if (neg_num) {
    rv.result *= -1.0;
  }
  rv.result *= pow(10, exponent);
  rv.tail = sv8span(beg, end);
  return rv;
fail1:
  rv.ok = false;
  rv.result = 0.0;
  rv.tail = s;
  return rv;
}

// FNV-1a hash. With thanks to Chris Wellons
// Source: https://nullprogram.com/blog/2025/01/19/
// Also: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
uint64_t sv8hash64(Sv8 s)
{
  uint64_t h = 0x100;
  for (ptrdiff_t i = 0; i < s.len; i++) {
    h ^= s.data[i] & 255;
    h *= 1111111111111111111;
  }
  return h;
}

#define CSBSZ 256
char *sv8cstring(Sv8 s)
{
  static char buf[CSBSZ];
  memset(buf, 0, CSBSZ);
  memcpy(buf, s.data, s.len < CSBSZ ? s.len : (CSBSZ - 1));
  return buf;
}

#endif // STRINGVIEW_IMPLEMENTATION
