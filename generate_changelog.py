#!/usr/bin/env python

"""Generates a friendly list of changes per language since the last release."""

import sys
import os
import subprocess

class Language(object):
  def __init__(self, name, pathspec):
    self.name = name
    self.pathspec = pathspec

languages = [
  Language("C++", [
      ":(glob)src/google/protobuf/*",
      "src/google/protobuf/compiler/cpp",
      "src/google/protobuf/io",
      "src/google/protobuf/util",
      "src/google/protobuf/stubs",
  ]),
  Language("Java", [
      "java",
      "javanano",
      "src/google/protobuf/compiler/cpp",
  ]),
  Language("Python", [
      "javanano",
      "src/google/protobuf/compiler/python",
  ]),
  Language("JavaScript", [
      "js",
      "src/google/protobuf/compiler/js",
  ]),
  Language("PHP", [
      "php",
      "src/google/protobuf/compiler/php",
  ]),
  Language("Ruby", [
      "ruby",
      "src/google/protobuf/compiler/ruby",
  ]),
  Language("Csharp", [
      "csharp",
      "src/google/protobuf/compiler/csharp",
  ]),
  Language("Objective C", [
      "objectivec",
      "src/google/protobuf/compiler/objectivec",
  ]),
]

if len(sys.argv) < 2:
  print("Usage: generate_changelog.py <previous release>")
  sys.exit(1)

previous = sys.argv[1]
if previous.startswith("-"):
  print("Invalid previous release: %s" % previous)
  sys.exit(1)

with open(os.devnull, "w") as devnull:
  if subprocess.call(
      ["git", "rev-parse", "--verify", previous + "^{commit}"],
      stdout=devnull, stderr=devnull) != 0:
    print("Invalid previous release: %s" % previous)
    sys.exit(1)

for language in languages:
  print(language.name)
  sys.stdout.flush()
  command = [
      "git", "log", "--pretty=oneline", "--abbrev-commit",
      previous + "...HEAD", "--"
  ] + language.pathspec
  process = subprocess.Popen(
      command, stdout=subprocess.PIPE, universal_newlines=True)
  for line in process.stdout:
    print(" - " + line.rstrip())
  if process.wait() != 0:
    sys.exit(process.returncode)
  print("")

print("To view a commit on GitHub: " +
      "https://github.com/google/protobuf/commit/<commit id>")
