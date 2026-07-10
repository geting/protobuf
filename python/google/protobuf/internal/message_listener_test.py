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

"""Tests for google.protobuf.internal.message_listener."""

try:
  import unittest2 as unittest  #PY26
except ImportError:
  import unittest

from google.protobuf.internal import message_listener


class MessageListenerTest(unittest.TestCase):

  def testModifiedIsAbstract(self):
    # The base interface documents Modified() but does not implement it.
    listener = message_listener.MessageListener()
    self.assertRaises(NotImplementedError, listener.Modified)


class NullMessageListenerTest(unittest.TestCase):

  def testModifiedIsNoOp(self):
    listener = message_listener.NullMessageListener()
    # Modified() should simply return None without raising.
    self.assertIsNone(listener.Modified())

  def testModifiedCanBeCalledRepeatedly(self):
    listener = message_listener.NullMessageListener()
    for _ in range(3):
      self.assertIsNone(listener.Modified())


if __name__ == '__main__':
  unittest.main()
