#! /usr/bin/env python
#
# Protocol Buffers - Google's data interchange format
# Copyright 2008 Google Inc.  All rights reserved.
# https://developers.google.com/protocol-buffers/
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""Tests for google.protobuf.internal.type_checkers."""

try:
  import unittest2 as unittest  #PY26
except ImportError:
  import unittest

from google.protobuf import descriptor
from google.protobuf.internal import type_checkers

_FieldDescriptor = descriptor.FieldDescriptor


class IntValueCheckerTest(unittest.TestCase):

  def testInt32AcceptsInBoundsValues(self):
    checker = type_checkers.Int32ValueChecker()
    self.assertEqual(0, checker.CheckValue(0))
    self.assertEqual(-2147483648, checker.CheckValue(-2147483648))
    self.assertEqual(2147483647, checker.CheckValue(2147483647))

  def testInt32RejectsOutOfBoundsValues(self):
    checker = type_checkers.Int32ValueChecker()
    self.assertRaises(ValueError, checker.CheckValue, 2147483648)
    self.assertRaises(ValueError, checker.CheckValue, -2147483649)

  def testUint32Bounds(self):
    checker = type_checkers.Uint32ValueChecker()
    self.assertEqual(0, checker.CheckValue(0))
    self.assertEqual((1 << 32) - 1, checker.CheckValue((1 << 32) - 1))
    self.assertRaises(ValueError, checker.CheckValue, -1)
    self.assertRaises(ValueError, checker.CheckValue, 1 << 32)

  def testInt64Bounds(self):
    checker = type_checkers.Int64ValueChecker()
    self.assertEqual(-(1 << 63), checker.CheckValue(-(1 << 63)))
    self.assertEqual((1 << 63) - 1, checker.CheckValue((1 << 63) - 1))
    self.assertRaises(ValueError, checker.CheckValue, 1 << 63)
    self.assertRaises(ValueError, checker.CheckValue, -(1 << 63) - 1)

  def testUint64Bounds(self):
    checker = type_checkers.Uint64ValueChecker()
    self.assertEqual(0, checker.CheckValue(0))
    self.assertEqual((1 << 64) - 1, checker.CheckValue((1 << 64) - 1))
    self.assertRaises(ValueError, checker.CheckValue, -1)
    self.assertRaises(ValueError, checker.CheckValue, 1 << 64)

  def testBoolIsAcceptedAsInteger(self):
    checker = type_checkers.Int32ValueChecker()
    # bool is a subclass of int, so it is a valid integral value.
    self.assertEqual(1, checker.CheckValue(True))
    self.assertEqual(0, checker.CheckValue(False))

  def testNonIntegralTypeRaisesTypeError(self):
    checker = type_checkers.Int32ValueChecker()
    self.assertRaises(TypeError, checker.CheckValue, 1.5)
    self.assertRaises(TypeError, checker.CheckValue, '1')

  def testDefaultValueIsZero(self):
    self.assertEqual(0, type_checkers.Int32ValueChecker().DefaultValue())
    self.assertEqual(0, type_checkers.Uint64ValueChecker().DefaultValue())


class UnicodeValueCheckerTest(unittest.TestCase):

  def setUp(self):
    self._checker = type_checkers.UnicodeValueChecker()

  def testAcceptsUnicode(self):
    self.assertEqual(u'abc', self._checker.CheckValue(u'abc'))

  def testDecodesValidUtf8Bytes(self):
    self.assertEqual(u'abc', self._checker.CheckValue(b'abc'))
    # Multi-byte UTF-8 sequence should be decoded to the unicode string.
    self.assertEqual(u'\u00e9', self._checker.CheckValue(b'\xc3\xa9'))

  def testRejectsInvalidUtf8Bytes(self):
    self.assertRaises(ValueError, self._checker.CheckValue, b'\xff\xfe')

  def testRejectsNonStringTypes(self):
    self.assertRaises(TypeError, self._checker.CheckValue, 5)
    self.assertRaises(TypeError, self._checker.CheckValue, None)

  def testDefaultValueIsEmptyUnicode(self):
    self.assertEqual(u'', self._checker.DefaultValue())


class _FakeEnumValueDescriptor(object):

  def __init__(self, number):
    self.number = number


class _FakeEnumDescriptor(object):

  def __init__(self, numbers):
    self.values = [_FakeEnumValueDescriptor(n) for n in numbers]
    self.values_by_number = dict((n, self.values[i])
                                 for i, n in enumerate(numbers))


class EnumValueCheckerTest(unittest.TestCase):

  def setUp(self):
    self._enum_type = _FakeEnumDescriptor([0, 1, 2])
    self._checker = type_checkers.EnumValueChecker(self._enum_type)

  def testAcceptsKnownValues(self):
    self.assertEqual(0, self._checker.CheckValue(0))
    self.assertEqual(2, self._checker.CheckValue(2))

  def testRejectsUnknownValues(self):
    self.assertRaises(ValueError, self._checker.CheckValue, 3)

  def testRejectsNonIntegralValues(self):
    self.assertRaises(TypeError, self._checker.CheckValue, 'a')

  def testDefaultValueIsFirstEnumValue(self):
    self.assertEqual(0, self._checker.DefaultValue())


class TypeCheckerTest(unittest.TestCase):

  def testAcceptsListedTypes(self):
    checker = type_checkers.TypeChecker(int, float)
    self.assertEqual(3, checker.CheckValue(3))
    self.assertEqual(2.5, checker.CheckValue(2.5))

  def testRejectsOtherTypes(self):
    checker = type_checkers.TypeChecker(int)
    self.assertRaises(TypeError, checker.CheckValue, 'x')

  def testTypeCheckerWithDefault(self):
    checker = type_checkers.TypeCheckerWithDefault(0.0, float)
    self.assertEqual(1.5, checker.CheckValue(1.5))
    self.assertEqual(0.0, checker.DefaultValue())
    self.assertRaises(TypeError, checker.CheckValue, 'x')


class _FakeContainingType(object):

  def __init__(self, syntax):
    self.syntax = syntax


class _FakeField(object):

  def __init__(self, cpp_type, field_type=None, enum_type=None, syntax='proto2'):
    self.cpp_type = cpp_type
    self.type = field_type
    self.enum_type = enum_type
    self.containing_type = _FakeContainingType(syntax)


class GetTypeCheckerTest(unittest.TestCase):

  def testStringFieldReturnsUnicodeChecker(self):
    field = _FakeField(_FieldDescriptor.CPPTYPE_STRING,
                       _FieldDescriptor.TYPE_STRING)
    self.assertIsInstance(type_checkers.GetTypeChecker(field),
                          type_checkers.UnicodeValueChecker)

  def testInt32FieldReturnsIntChecker(self):
    field = _FakeField(_FieldDescriptor.CPPTYPE_INT32)
    checker = type_checkers.GetTypeChecker(field)
    self.assertIs(checker,
                  type_checkers._VALUE_CHECKERS[_FieldDescriptor.CPPTYPE_INT32])

  def testClosedEnumFieldReturnsEnumChecker(self):
    enum_type = _FakeEnumDescriptor([0, 1])
    field = _FakeField(_FieldDescriptor.CPPTYPE_ENUM, enum_type=enum_type,
                       syntax='proto2')
    checker = type_checkers.GetTypeChecker(field)
    self.assertIsInstance(checker, type_checkers.EnumValueChecker)

  def testOpenEnumFieldReturnsInt32Checker(self):
    enum_type = _FakeEnumDescriptor([0, 1])
    field = _FakeField(_FieldDescriptor.CPPTYPE_ENUM, enum_type=enum_type,
                       syntax='proto3')
    self.assertTrue(type_checkers.SupportsOpenEnums(field))
    checker = type_checkers.GetTypeChecker(field)
    self.assertIs(checker,
                  type_checkers._VALUE_CHECKERS[_FieldDescriptor.CPPTYPE_INT32])


if __name__ == '__main__':
  unittest.main()
