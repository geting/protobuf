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

"""Tests for google.protobuf.internal.enum_type_wrapper."""

try:
  import unittest2 as unittest  #PY26
except ImportError:
  import unittest

from google.protobuf.internal import enum_type_wrapper


class _FakeEnumValueDescriptor(object):
  """Minimal stand-in for descriptor.EnumValueDescriptor."""

  def __init__(self, name, number):
    self.name = name
    self.number = number


class _FakeEnumDescriptor(object):
  """Minimal stand-in for descriptor.EnumDescriptor.

  Provides only the attributes that EnumTypeWrapper relies on so the wrapper
  can be exercised without compiling a real .proto definition.
  """

  def __init__(self, name, values):
    # values is an ordered list of (name, number) pairs.
    self.name = name
    self.values = [_FakeEnumValueDescriptor(n, v) for n, v in values]
    self.values_by_name = dict((v.name, v) for v in self.values)
    self.values_by_number = dict((v.number, v) for v in self.values)


class EnumTypeWrapperTest(unittest.TestCase):

  def setUp(self):
    # Deliberately not sorted by number to verify definition order is kept.
    self._enum_type = _FakeEnumDescriptor(
        'Color', [('RED', 0), ('GREEN', 2), ('BLUE', 1)])
    self._wrapper = enum_type_wrapper.EnumTypeWrapper(self._enum_type)

  def testDescriptorIsExposed(self):
    self.assertIs(self._wrapper.DESCRIPTOR, self._enum_type)

  def testName(self):
    self.assertEqual('RED', self._wrapper.Name(0))
    self.assertEqual('BLUE', self._wrapper.Name(1))
    self.assertEqual('GREEN', self._wrapper.Name(2))

  def testNameWithUnknownNumberRaisesValueError(self):
    with self.assertRaises(ValueError) as ctx:
      self._wrapper.Name(99)
    self.assertIn('Color', str(ctx.exception))
    self.assertIn('99', str(ctx.exception))

  def testValue(self):
    self.assertEqual(0, self._wrapper.Value('RED'))
    self.assertEqual(1, self._wrapper.Value('BLUE'))
    self.assertEqual(2, self._wrapper.Value('GREEN'))

  def testValueWithUnknownNameRaisesValueError(self):
    with self.assertRaises(ValueError) as ctx:
      self._wrapper.Value('PURPLE')
    self.assertIn('Color', str(ctx.exception))
    self.assertIn('PURPLE', str(ctx.exception))

  def testNameValueRoundTrip(self):
    for name in ('RED', 'GREEN', 'BLUE'):
      self.assertEqual(name, self._wrapper.Name(self._wrapper.Value(name)))

  def testKeysPreservesDefinitionOrder(self):
    self.assertEqual(['RED', 'GREEN', 'BLUE'], self._wrapper.keys())

  def testValuesPreservesDefinitionOrder(self):
    self.assertEqual([0, 2, 1], self._wrapper.values())

  def testItemsPreservesDefinitionOrder(self):
    self.assertEqual(
        [('RED', 0), ('GREEN', 2), ('BLUE', 1)], self._wrapper.items())

  def testEmptyEnum(self):
    wrapper = enum_type_wrapper.EnumTypeWrapper(
        _FakeEnumDescriptor('Empty', []))
    self.assertEqual([], wrapper.keys())
    self.assertEqual([], wrapper.values())
    self.assertEqual([], wrapper.items())


if __name__ == '__main__':
  unittest.main()
