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

"""Tests for the abstract RPC interfaces in google.protobuf.service."""

try:
  import unittest2 as unittest  #PY26
except ImportError:
  import unittest

from google.protobuf import service


class RpcExceptionTest(unittest.TestCase):

  def testIsAnException(self):
    self.assertTrue(issubclass(service.RpcException, Exception))

  def testCanBeRaisedWithMessage(self):
    try:
      raise service.RpcException('boom')
    except service.RpcException as e:
      self.assertEqual('boom', str(e))
    else:
      self.fail('RpcException was not raised')


class ServiceTest(unittest.TestCase):

  def setUp(self):
    self._service = service.Service()

  def testGetDescriptorIsAbstract(self):
    # GetDescriptor is declared without a self parameter and must be overridden.
    self.assertRaises(NotImplementedError, service.Service.GetDescriptor)

  def testCallMethodIsAbstract(self):
    self.assertRaises(NotImplementedError, self._service.CallMethod,
                      None, None, None, None)

  def testGetRequestClassIsAbstract(self):
    self.assertRaises(NotImplementedError, self._service.GetRequestClass, None)

  def testGetResponseClassIsAbstract(self):
    self.assertRaises(NotImplementedError, self._service.GetResponseClass, None)


class RpcControllerTest(unittest.TestCase):

  def setUp(self):
    self._controller = service.RpcController()

  def testClientSideMethodsAreAbstract(self):
    self.assertRaises(NotImplementedError, self._controller.Reset)
    self.assertRaises(NotImplementedError, self._controller.Failed)
    self.assertRaises(NotImplementedError, self._controller.ErrorText)
    self.assertRaises(NotImplementedError, self._controller.StartCancel)

  def testServerSideMethodsAreAbstract(self):
    self.assertRaises(NotImplementedError, self._controller.SetFailed, 'reason')
    self.assertRaises(NotImplementedError, self._controller.IsCanceled)
    self.assertRaises(NotImplementedError, self._controller.NotifyOnCancel,
                      lambda: None)


class RpcChannelTest(unittest.TestCase):

  def testCallMethodIsAbstract(self):
    channel = service.RpcChannel()
    self.assertRaises(NotImplementedError, channel.CallMethod,
                      None, None, None, None, None)


if __name__ == '__main__':
  unittest.main()
