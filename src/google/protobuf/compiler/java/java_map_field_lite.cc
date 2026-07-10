// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// https://developers.google.com/protocol-buffers/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <google/protobuf/compiler/java/java_map_field_lite.h>

#include <google/protobuf/compiler/java/java_context.h>
#include <google/protobuf/compiler/java/java_doc_comment.h>
#include <google/protobuf/compiler/java/java_helpers.h>
#include <google/protobuf/compiler/java/java_name_resolver.h>
#include <google/protobuf/io/printer.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace java {

ImmutableMapFieldLiteGenerator::
ImmutableMapFieldLiteGenerator(const FieldDescriptor* descriptor,
                                       int /* messageBitIndex */,
                                       int /* builderBitIndex */,
                                       Context* context)
  : descriptor_(descriptor), name_resolver_(context->GetNameResolver())  {
  SetMapFieldVariables(descriptor, context->GetFieldGeneratorInfo(descriptor),
                       context, &variables_);
}

ImmutableMapFieldLiteGenerator::
~ImmutableMapFieldLiteGenerator() {}

int ImmutableMapFieldLiteGenerator::GetNumBitsForMessage() const {
  return 0;
}

int ImmutableMapFieldLiteGenerator::GetNumBitsForBuilder() const {
  return 0;
}

void ImmutableMapFieldLiteGenerator::
GenerateInterfaceMembers(io::Printer* printer) const {
  GenerateMapFieldInterfaceMembers(descriptor_, variables_, printer);
}

void ImmutableMapFieldLiteGenerator::
GenerateMembers(io::Printer* printer) const {
  printer->Print(
      variables_,
      "private static final class $capitalized_name$DefaultEntryHolder {\n"
      "  static final com.google.protobuf.MapEntryLite<\n"
      "      $type_parameters$> defaultEntry =\n"
      "          com.google.protobuf.MapEntryLite\n"
      "          .<$type_parameters$>newDefaultInstance(\n"
      "              $key_wire_type$,\n"
      "              $key_default_value$,\n"
      "              $value_wire_type$,\n"
      "              $value_default_value$);\n"
      "}\n");
  printer->Print(
      variables_,
      "private com.google.protobuf.MapFieldLite<\n"
      "    $type_parameters$> $name$_ =\n"
      "        com.google.protobuf.MapFieldLite.emptyMapField();\n"
      "private com.google.protobuf.MapFieldLite<$type_parameters$>\n"
      "internalGet$capitalized_name$() {\n"
      "  return $name$_;\n"
      "}\n"
      "private com.google.protobuf.MapFieldLite<$type_parameters$>\n"
      "internalGetMutable$capitalized_name$() {\n"
      "  if (!$name$_.isMutable()) {\n"
      "    $name$_ = $name$_.mutableCopy();\n"
      "  }\n"
      "  return $name$_;\n"
      "}\n");
  printer->Print(
      variables_,
      "$deprecation$\n"
      "public int get$capitalized_name$Count() {\n"
      "  return internalGet$capitalized_name$().size();\n"
      "}\n");
  WriteFieldDocComment(printer, descriptor_);
  printer->Print(
      variables_,
      "$deprecation$\n"
      "public boolean contains$capitalized_name$(\n"
      "    $key_type$ key) {\n"
      "  $key_null_check$\n"
      "  return internalGet$capitalized_name$().containsKey(key);\n"
      "}\n");
  if (GetJavaType(MapValueField(descriptor_)) == JAVATYPE_ENUM) {
    printer->Print(
        variables_,
        "private static final\n"
        "com.google.protobuf.Internal.MapAdapter.Converter<\n"
        "    java.lang.Integer, $value_enum_type$> $name$ValueConverter =\n"
        "        com.google.protobuf.Internal.MapAdapter.newEnumConverter(\n"
        "            $value_enum_type$.internalGetValueMap(),\n"
        "            $unrecognized_value$);\n");
    printer->Print(
        variables_,
        "/**\n"
        " * Use {@link #get$capitalized_name$Map()} instead.\n"
        " */\n"
        "@java.lang.Deprecated\n"
        "public java.util.Map<$boxed_key_type$, $value_enum_type$>\n"
        "get$capitalized_name$() {\n"
        "  return get$capitalized_name$Map();\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$\n"
        "public java.util.Map<$boxed_key_type$, $value_enum_type$>\n"
        "get$capitalized_name$Map() {\n"
        "  return java.util.Collections.unmodifiableMap(\n"
        "      new com.google.protobuf.Internal.MapAdapter<\n"
        "        $boxed_key_type$, $value_enum_type$, java.lang.Integer>(\n"
        "            internalGet$capitalized_name$(),\n"
        "            $name$ValueConverter));\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$\n"
        "public $value_enum_type$ get$capitalized_name$OrDefault(\n"
        "    $key_type$ key,\n"
        "    $value_enum_type$ defaultValue) {\n"
        "  $key_null_check$\n"
        "  java.util.Map<$boxed_key_type$, $boxed_value_type$> map =\n"
        "      internalGet$capitalized_name$();\n"
        "  return map.containsKey(key)\n"
        "         ? $name$ValueConverter.doForward(map.get(key))\n"
        "         : defaultValue;\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$\n"
        "public $value_enum_type$ get$capitalized_name$OrThrow(\n"
        "    $key_type$ key) {\n"
        "  $key_null_check$\n"
        "  java.util.Map<$boxed_key_type$, $boxed_value_type$> map =\n"
        "      internalGet$capitalized_name$();\n"
        "  if (!map.containsKey(key)) {\n"
        "    throw new java.lang.IllegalArgumentException();\n"
        "  }\n"
        "  return $name$ValueConverter.doForward(map.get(key));\n"
        "}\n");
    if (SupportUnknownEnumValue(descriptor_->file())) {
      printer->Print(
          variables_,
          "/**\n"
          " * Use {@link #get$capitalized_name$ValueMap()} instead.\n"
          " */\n"
          "@java.lang.Deprecated\n"
          "public java.util.Map<$boxed_key_type$, $boxed_value_type$>\n"
          "get$capitalized_name$Value() {\n"
          "  return get$capitalized_name$ValueMap();\n"
          "}\n");
      WriteFieldDocComment(printer, descriptor_);
      printer->Print(
          variables_,
          "$deprecation$\n"
          "public java.util.Map<$boxed_key_type$, $boxed_value_type$>\n"
          "get$capitalized_name$ValueMap() {\n"
          "  return java.util.Collections.unmodifiableMap(\n"
          "      internalGet$capitalized_name$());\n"
          "}\n");
      WriteFieldDocComment(printer, descriptor_);
      printer->Print(
          variables_,
          "$deprecation$\n"
          "public $value_type$ get$capitalized_name$ValueOrDefault(\n"
          "    $key_type$ key,\n"
          "    $value_type$ defaultValue) {\n"
          "  $key_null_check$\n"
          "  java.util.Map<$boxed_key_type$, $boxed_value_type$> map =\n"
          "      internalGet$capitalized_name$();\n"
          "  return map.containsKey(key) ? map.get(key) : defaultValue;\n"
          "}\n");
      WriteFieldDocComment(printer, descriptor_);
      printer->Print(
          variables_,
          "$deprecation$\n"
          "public $value_type$ get$capitalized_name$ValueOrThrow(\n"
          "    $key_type$ key) {\n"
          "  $key_null_check$\n"
          "  java.util.Map<$boxed_key_type$, $boxed_value_type$> map =\n"
          "      internalGet$capitalized_name$();\n"
          "  if (!map.containsKey(key)) {\n"
          "    throw new java.lang.IllegalArgumentException();\n"
          "  }\n"
          "  return map.get(key);\n"
          "}\n");
    }
  } else {
    printer->Print(
        variables_,
        "/**\n"
        " * Use {@link #get$capitalized_name$Map()} instead.\n"
        " */\n"
        "@java.lang.Deprecated\n"
        "public java.util.Map<$type_parameters$> get$capitalized_name$() {\n"
        "  return get$capitalized_name$Map();\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$\n"
        "public java.util.Map<$type_parameters$> get$capitalized_name$Map() {\n"
        "  return java.util.Collections.unmodifiableMap(\n"
        "      internalGet$capitalized_name$());\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$\n"
        "public $value_type$ get$capitalized_name$OrDefault(\n"
        "    $key_type$ key,\n"
        "    $value_type$ defaultValue) {\n"
        "  $key_null_check$\n"
        "  java.util.Map<$type_parameters$> map =\n"
        "      internalGet$capitalized_name$();\n"
        "  return map.containsKey(key) ? map.get(key) : defaultValue;\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$\n"
        "public $value_type$ get$capitalized_name$OrThrow(\n"
        "    $key_type$ key) {\n"
        "  $key_null_check$\n"
        "  java.util.Map<$type_parameters$> map =\n"
        "      internalGet$capitalized_name$();\n"
        "  if (!map.containsKey(key)) {\n"
        "    throw new java.lang.IllegalArgumentException();\n"
        "  }\n"
        "  return map.get(key);\n"
        "}\n");
  }

  // Generate private setters for the builder to proxy into.
  if (GetJavaType(MapValueField(descriptor_)) == JAVATYPE_ENUM) {
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "private java.util.Map<$boxed_key_type$, $value_enum_type$>\n"
        "getMutable$capitalized_name$Map() {\n"
        "  return new com.google.protobuf.Internal.MapAdapter<\n"
        "      $boxed_key_type$, $value_enum_type$, java.lang.Integer>(\n"
        "          internalGetMutable$capitalized_name$(),\n"
        "          $name$ValueConverter);\n"
        "}\n");
    if (SupportUnknownEnumValue(descriptor_->file())) {
      WriteFieldDocComment(printer, descriptor_);
      printer->Print(
          variables_,
          "private java.util.Map<$boxed_key_type$, $boxed_value_type$>\n"
          "getMutable$capitalized_name$ValueMap() {\n"
          "  return internalGetMutable$capitalized_name$();\n"
          "}\n");
    }
  } else {
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "private java.util.Map<$type_parameters$>\n"
        "getMutable$capitalized_name$Map() {\n"
        "  return internalGetMutable$capitalized_name$();\n"
        "}\n");
  }
}


void ImmutableMapFieldLiteGenerator::
GenerateBuilderMembers(io::Printer* printer) const {
  printer->Print(
      variables_,
      "$deprecation$\n"
      "public int get$capitalized_name$Count() {\n"
      "  return instance.get$capitalized_name$Map().size();\n"
      "}\n");
  WriteFieldDocComment(printer, descriptor_);
  printer->Print(
      variables_,
      "$deprecation$\n"
      "public boolean contains$capitalized_name$(\n"
      "    $key_type$ key) {\n"
      "  $key_null_check$\n"
      "  return instance.get$capitalized_name$Map().containsKey(key);\n"
      "}\n");
  printer->Print(
      variables_,
      "$deprecation$\n"
      "public Builder clear$capitalized_name$() {\n"
      "  copyOnWrite();\n"
      "  instance.getMutable$capitalized_name$Map().clear();\n"
      "  return this;\n"
      "}\n");
  WriteFieldDocComment(printer, descriptor_);
  printer->Print(
      variables_,
      "$deprecation$\n"
      "public Builder remove$capitalized_name$(\n"
      "    $key_type$ key) {\n"
      "  $key_null_check$\n"
      "  copyOnWrite();\n"
      "  instance.getMutable$capitalized_name$Map().remove(key);\n"
      "  return this;\n"
      "}\n");
  if (GetJavaType(MapValueField(descriptor_)) == JAVATYPE_ENUM) {
    printer->Print(
        variables_,
        "/**\n"
        " * Use {@link #get$capitalized_name$Map()} instead.\n"
        " */\n"
        "@java.lang.Deprecated\n"
        "public java.util.Map<$boxed_key_type$, $value_enum_type$>\n"
        "get$capitalized_name$() {\n"
        "  return get$capitalized_name$Map();\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$\n"
        "public java.util.Map<$boxed_key_type$, $value_enum_type$>\n"
        "get$capitalized_name$Map() {\n"
        "  return java.util.Collections.unmodifiableMap(\n"
        "      instance.get$capitalized_name$Map());\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$\n"
        "public $value_enum_type$ get$capitalized_name$OrDefault(\n"
        "    $key_type$ key,\n"
        "    $value_enum_type$ defaultValue) {\n"
        "  $key_null_check$\n"
        "  java.util.Map<$boxed_key_type$, $value_enum_type$> map =\n"
        "      instance.get$capitalized_name$Map();\n"
        "  return map.containsKey(key)\n"
        "         ? map.get(key)\n"
        "         : defaultValue;\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$\n"
        "public $value_enum_type$ get$capitalized_name$OrThrow(\n"
        "    $key_type$ key) {\n"
        "  $key_null_check$\n"
        "  java.util.Map<$boxed_key_type$, $value_enum_type$> map =\n"
        "      instance.get$capitalized_name$Map();\n"
        "  if (!map.containsKey(key)) {\n"
        "    throw new java.lang.IllegalArgumentException();\n"
        "  }\n"
        "  return map.get(key);\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$public Builder put$capitalized_name$(\n"
        "    $key_type$ key,\n"
        "    $value_enum_type$ value) {\n"
        "  $key_null_check$\n"
        "  $value_null_check$\n"
        "  copyOnWrite();\n"
        "  instance.getMutable$capitalized_name$Map().put(key, value);\n"
        "  return this;\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$public Builder putAll$capitalized_name$(\n"
        "    java.util.Map<$boxed_key_type$, $value_enum_type$> values) {\n"
        "  copyOnWrite();\n"
        "  instance.getMutable$capitalized_name$Map().putAll(values);\n"
        "  return this;\n"
        "}\n");
    if (SupportUnknownEnumValue(descriptor_->file())) {
      printer->Print(
          variables_,
          "/**\n"
          " * Use {@link #get$capitalized_name$ValueMap()} instead.\n"
          " */\n"
          "@java.lang.Deprecated\n"
          "public java.util.Map<$boxed_key_type$, $boxed_value_type$>\n"
          "get$capitalized_name$Value() {\n"
          "  return get$capitalized_name$ValueMap();\n"
          "}\n");
      WriteFieldDocComment(printer, descriptor_);
      printer->Print(
          variables_,
          "$deprecation$\n"
          "public java.util.Map<$boxed_key_type$, $boxed_value_type$>\n"
          "get$capitalized_name$ValueMap() {\n"
          "  return java.util.Collections.unmodifiableMap(\n"
          "      instance.get$capitalized_name$ValueMap());\n"
          "}\n");
      WriteFieldDocComment(printer, descriptor_);
      printer->Print(
          variables_,
          "$deprecation$\n"
          "public $value_type$ get$capitalized_name$ValueOrDefault(\n"
          "    $key_type$ key,\n"
          "    $value_type$ defaultValue) {\n"
          "  $key_null_check$\n"
          "  java.util.Map<$boxed_key_type$, $boxed_value_type$> map =\n"
          "      instance.get$capitalized_name$ValueMap();\n"
          "  return map.containsKey(key) ? map.get(key) : defaultValue;\n"
          "}\n");
      WriteFieldDocComment(printer, descriptor_);
      printer->Print(
          variables_,
          "$deprecation$\n"
          "public $value_type$ get$capitalized_name$ValueOrThrow(\n"
          "    $key_type$ key) {\n"
          "  $key_null_check$\n"
          "  java.util.Map<$boxed_key_type$, $boxed_value_type$> map =\n"
          "      instance.get$capitalized_name$ValueMap();\n"
          "  if (!map.containsKey(key)) {\n"
          "    throw new java.lang.IllegalArgumentException();\n"
          "  }\n"
          "  return map.get(key);\n"
          "}\n");
      WriteFieldDocComment(printer, descriptor_);
      printer->Print(
          variables_,
          "$deprecation$public Builder put$capitalized_name$Value(\n"
          "    $key_type$ key,\n"
          "    $value_type$ value) {\n"
          "  $key_null_check$\n"
          "  copyOnWrite();\n"
          "  instance.getMutable$capitalized_name$ValueMap().put(key, value);\n"
          "  return this;\n"
          "}\n");
      WriteFieldDocComment(printer, descriptor_);
      printer->Print(
          variables_,
          "$deprecation$public Builder putAll$capitalized_name$Value(\n"
          "    java.util.Map<$boxed_key_type$, $boxed_value_type$> values) {\n"
          "  copyOnWrite();\n"
          "  instance.getMutable$capitalized_name$ValueMap().putAll(values);\n"
          "  return this;\n"
          "}\n");
    }
  } else {
    printer->Print(
        variables_,
        "/**\n"
        " * Use {@link #get$capitalized_name$Map()} instead.\n"
        " */\n"
        "@java.lang.Deprecated\n"
        "public java.util.Map<$type_parameters$> get$capitalized_name$() {\n"
        "  return get$capitalized_name$Map();\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$"
        "public java.util.Map<$type_parameters$> get$capitalized_name$Map() {\n"
        "  return java.util.Collections.unmodifiableMap(\n"
        "      instance.get$capitalized_name$Map());\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$\n"
        "public $value_type$ get$capitalized_name$OrDefault(\n"
        "    $key_type$ key,\n"
        "    $value_type$ defaultValue) {\n"
        "  $key_null_check$\n"
        "  java.util.Map<$type_parameters$> map =\n"
        "      instance.get$capitalized_name$Map();\n"
        "  return map.containsKey(key) ? map.get(key) : defaultValue;\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$\n"
        "public $value_type$ get$capitalized_name$OrThrow(\n"
        "    $key_type$ key) {\n"
        "  $key_null_check$\n"
        "  java.util.Map<$type_parameters$> map =\n"
        "      instance.get$capitalized_name$Map();\n"
        "  if (!map.containsKey(key)) {\n"
        "    throw new java.lang.IllegalArgumentException();\n"
        "  }\n"
        "  return map.get(key);\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$"
        "public Builder put$capitalized_name$(\n"
        "    $key_type$ key,\n"
        "    $value_type$ value) {\n"
        "  $key_null_check$\n"
        "  $value_null_check$\n"
        "  copyOnWrite();\n"
        "  instance.getMutable$capitalized_name$Map().put(key, value);\n"
        "  return this;\n"
        "}\n");
    WriteFieldDocComment(printer, descriptor_);
    printer->Print(
        variables_,
        "$deprecation$"
        "public Builder putAll$capitalized_name$(\n"
        "    java.util.Map<$type_parameters$> values) {\n"
        "  copyOnWrite();\n"
        "  instance.getMutable$capitalized_name$Map().putAll(values);\n"
        "  return this;\n"
        "}\n");
  }
}

void ImmutableMapFieldLiteGenerator::
GenerateFieldBuilderInitializationCode(io::Printer* printer)  const {
  // Nothing to initialize.
}

void ImmutableMapFieldLiteGenerator::
GenerateInitializationCode(io::Printer* printer) const {
  // Nothing to initialize.
}

void ImmutableMapFieldLiteGenerator::
GenerateVisitCode(io::Printer* printer) const {
  printer->Print(
      variables_,
      "$name$_ = visitor.visitMap(\n"
      "    $name$_, other.internalGet$capitalized_name$());\n");
}

void ImmutableMapFieldLiteGenerator::
GenerateDynamicMethodMakeImmutableCode(io::Printer* printer) const {
  printer->Print(variables_,
    "$name$_.makeImmutable();\n");
}

void ImmutableMapFieldLiteGenerator::
GenerateParsingCode(io::Printer* printer) const {
  printer->Print(
      variables_,
      "if (!$name$_.isMutable()) {\n"
      "  $name$_ = $name$_.mutableCopy();\n"
      "}\n");
  if (!SupportUnknownEnumValue(descriptor_->file()) &&
      GetJavaType(MapValueField(descriptor_)) == JAVATYPE_ENUM) {
    printer->Print(
        variables_,
        "com.google.protobuf.ByteString bytes = input.readBytes();\n"
        "java.util.Map.Entry<$type_parameters$> $name$__ =\n"
        "    $default_entry$.parseEntry(bytes, extensionRegistry);\n");
    printer->Print(
        variables_,
        "if ($value_enum_type$.forNumber($name$__.getValue()) == null) {\n"
        "  super.mergeLengthDelimitedField($number$, bytes);\n"
        "} else {\n"
        "  $name$_.put($name$__);\n"
        "}\n");
  } else {
    printer->Print(
        variables_,
        "$default_entry$.parseInto($name$_, input, extensionRegistry);");
  }
}

void ImmutableMapFieldLiteGenerator::
GenerateParsingDoneCode(io::Printer* printer) const {
  // Nothing to do here.
}

void ImmutableMapFieldLiteGenerator::
GenerateSerializationCode(io::Printer* printer) const {
  printer->Print(
      variables_,
      "for (java.util.Map.Entry<$type_parameters$> entry\n"
      "     : internalGet$capitalized_name$().entrySet()) {\n"
      "  $default_entry$.serializeTo(\n"
      "      output, $number$, entry.getKey(), entry.getValue());\n"
      "}\n");
}

void ImmutableMapFieldLiteGenerator::
GenerateSerializedSizeCode(io::Printer* printer) const {
  printer->Print(
      variables_,
      "for (java.util.Map.Entry<$type_parameters$> entry\n"
      "     : internalGet$capitalized_name$().entrySet()) {\n"
      "  size += $default_entry$.computeMessageSize(\n"
      "    $number$, entry.getKey(), entry.getValue());\n"
      "}\n");
}

void ImmutableMapFieldLiteGenerator::
GenerateEqualsCode(io::Printer* printer) const {
  printer->Print(
      variables_,
      "result = result && internalGet$capitalized_name$().equals(\n"
      "    other.internalGet$capitalized_name$());\n");
}

void ImmutableMapFieldLiteGenerator::
GenerateHashCode(io::Printer* printer) const {
  printer->Print(
      variables_,
      "if (!internalGet$capitalized_name$().isEmpty()) {\n"
      "  hash = (37 * hash) + $constant_name$;\n"
      "  hash = (53 * hash) + internalGet$capitalized_name$().hashCode();\n"
      "}\n");
}

string ImmutableMapFieldLiteGenerator::GetBoxedType() const {
  return name_resolver_->GetImmutableClassName(descriptor_->message_type());
}

}  // namespace java
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
