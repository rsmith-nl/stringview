String views in C
#################

:date: 2025-08-31
:tags: C programming, string handling, public domain
:author: Roland Smith

.. Last modified: 2025-08-31T10:20:19+0200
.. vim:spelllang=en

Introduction
============

This code was inspired by `this blog entry`_ by Chris Wellons.

.. _this blog entry: https://nullprogram.com/blog/2023/10/08/

My interpretation of the basic insight behind it is to separate string
handling from memory storage.
That is why I'm calling them *string views*.
Inspired by Python, I have added *strip* and *lsplit* functionality.

Some functions return a special-purpose structure. Not only does this store
the result of say of a conversion, but also a ``bool`` to indicate if the
operation succeeded and usually a new stringview with the remainder of the
string.

*This makes string handling in C much more enjoyable, and almost Python-like.*


Usage
=====

The included Makefile can build a static and dynamic library on UNIX-like systems.
But its main purpose is to compile the test suite.
In general I would advise just to add ``stringview.h`` and ``stringview.c`` to
your project.
