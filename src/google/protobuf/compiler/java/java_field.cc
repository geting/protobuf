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

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#include <google/protobuf/compiler/java/java_field.h>

#include <memory>
#ifndef _SHARED_PTR_H
#include <google/protobuf/stubs/shared_ptr.h>
#endif

#include <google/protobuf/stubs/logging.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/compiler/java/java_context.h>
#include <google/protobuf/compiler/java/java_doc_comment.h>
#include <google/protobuf/compiler/java/java_enum_field.h>
#include <google/protobuf/compiler/java/java_enum_field_lite.h>
#include <google/protobuf/compiler/java/java_helpers.h>
#include <google/protobuf/compiler/java/java_lazy_message_field.h>
#include <google/protobuf/compiler/java/java_lazy_message_field_lite.h>
#include <google/protobuf/compiler/java/java_map_field.h>
#include <google/protobuf/compiler/java/java_map_field_lite.h>
#include <google/protobuf/compiler/java/java_message_field.h>
#include <google/protobuf/compiler/java/java_message_field_lite.h>
#include <google/protobuf/compiler/java/java_name_resolver.h>
#include <google/protobuf/compiler/java/java_primitive_field.h>
#include <google/protobuf/compiler/java/java_primitive_field_lite.h>
#include <google/protobuf/compiler/java/java_string_field.h>
#include <google/protobuf/compiler/java/java_string_field_lite.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/strutil.h>
#include <google/protobuf/stubs/substitute.h>


namespace google {
namespace protobuf {
namespace compiler {
namespace java {

namespace {

string MapFieldTypeName(const FieldDescriptor* field,
                        ClassNameResolver* name_resolver,
                        bool boxed) {
  if (GetJavaType(field) == JAVATYPE_MESSAGE) {
    return name_resolver->GetImmutableClassName(field->message_type());
  } else if (GetJavaType(field) == JAVATYPE_ENUM) {
    return name_resolver->GetImmutableClassName(field->enum_type());
  } else {
    return boxed ? BoxedPrimitiveTypeName(GetJavaType(field))
                 : PrimitiveTypeName(GetJavaType(field));
  }
}

string MapFieldWireType(const FieldDescriptor* field) {
  return "com.google.protobuf.WireFormat.FieldType." +
      string(FieldTypeName(field->type()));
}

ImmutableFieldGenerator* MakeImmutableGenerator(
    const FieldDescriptor* field, int messageBitIndex, int builderBitIndex,
    Context* context) {
  if (field->is_repeated()) {
    switch (GetJavaType(field)) {
      case JAVATYPE_MESSAGE:
        if (IsMapEntry(field->message_type())) {
          return new ImmutableMapFieldGenerator(
              field, messageBitIndex, builderBitIndex, context);
        } else {
          if (IsLazy(field, context->EnforceLite())) {
            return new RepeatedImmutableLazyMessageFieldGenerator(
                field, messageBitIndex, builderBitIndex, context);
          } else {
            return new RepeatedImmutableMessageFieldGenerator(
                field, messageBitIndex, builderBitIndex, context);
          }
        }
      case JAVATYPE_ENUM:
        return new RepeatedImmutableEnumFieldGenerator(
            field, messageBitIndex, builderBitIndex, context);
      case JAVATYPE_STRING:
        return new RepeatedImmutableStringFieldGenerator(
            field, messageBitIndex, builderBitIndex, context);
      default:
        return new RepeatedImmutablePrimitiveFieldGenerator(
            field, messageBitIndex, builderBitIndex, context);
    }
  } else {
    if (field->containing_oneof()) {
      switch (GetJavaType(field)) {
        case JAVATYPE_MESSAGE:
          if (IsLazy(field, context->EnforceLite())) {
            return new ImmutableLazyMessageOneofFieldGenerator(
                field, messageBitIndex, builderBitIndex, context);
          } else {
            return new ImmutableMessageOneofFieldGenerator(
                field, messageBitIndex, builderBitIndex, context);
          }
        case JAVATYPE_ENUM:
          return new ImmutableEnumOneofFieldGenerator(
              field, messageBitIndex, builderBitIndex, context);
        case JAVATYPE_STRING:
          return new ImmutableStringOneofFieldGenerator(
              field, messageBitIndex, builderBitIndex, context);
        default:
          return new ImmutablePrimitiveOneofFieldGenerator(
              field, messageBitIndex, builderBitIndex, context);
      }
    } else {
      switch (GetJavaType(field)) {
        case JAVATYPE_MESSAGE:
          if (IsLazy(field, context->EnforceLite())) {
            return new ImmutableLazyMessageFieldGenerator(
                field, messageBitIndex, builderBitIndex, context);
          } else {
            return new ImmutableMessageFieldGenerator(
                field, messageBitIndex, builderBitIndex, context);
          }
        case JAVATYPE_ENUM:
          return new ImmutableEnumFieldGenerator(
              field, messageBitIndex, builderBitIndex, context);
        case JAVATYPE_STRING:
          return new ImmutableStringFieldGenerator(
              field, messageBitIndex, builderBitIndex, context);
        default:
          return new ImmutablePrimitiveFieldGenerator(
              field, messageBitIndex, builderBitIndex, context);
      }
    }
  }
}

ImmutableFieldLiteGenerator* MakeImmutableLiteGenerator(
    const FieldDescriptor* field, int messageBitIndex, int builderBitIndex,
    Context* context) {
  if (field->is_repeated()) {
    switch (GetJavaType(field)) {
      case JAVATYPE_MESSAGE:
        if (IsMapEntry(field->message_type())) {
          return new ImmutableMapFieldLiteGenerator(
              field, messageBitIndex, builderBitIndex, context);
        } else {
          if (IsLazy(field, context->EnforceLite())) {
            return new RepeatedImmutableLazyMessageFieldLiteGenerator(
                field, messageBitIndex, builderBitIndex, context);
          } else {
            return new RepeatedImmutableMessageFieldLiteGenerator(
                field, messageBitIndex, builderBitIndex, context);
          }
        }
      case JAVATYPE_ENUM:
        return new RepeatedImmutableEnumFieldLiteGenerator(
            field, messageBitIndex, builderBitIndex, context);
      case JAVATYPE_STRING:
        return new RepeatedImmutableStringFieldLiteGenerator(
            field, messageBitIndex, builderBitIndex, context);
      default:
        return new RepeatedImmutablePrimitiveFieldLiteGenerator(
            field, messageBitIndex, builderBitIndex, context);
    }
  } else {
    if (field->containing_oneof()) {
      switch (GetJavaType(field)) {
        case JAVATYPE_MESSAGE:
          if (IsLazy(field, context->EnforceLite())) {
            return new ImmutableLazyMessageOneofFieldLiteGenerator(
                field, messageBitIndex, builderBitIndex, context);
          } else {
            return new ImmutableMessageOneofFieldLiteGenerator(
                field, messageBitIndex, builderBitIndex, context);
          }
        case JAVATYPE_ENUM:
          return new ImmutableEnumOneofFieldLiteGenerator(
              field, messageBitIndex, builderBitIndex, context);
        case JAVATYPE_STRING:
          return new ImmutableStringOneofFieldLiteGenerator(
              field, messageBitIndex, builderBitIndex, context);
        default:
          return new ImmutablePrimitiveOneofFieldLiteGenerator(
              field, messageBitIndex, builderBitIndex, context);
      }
    } else {
      switch (GetJavaType(field)) {
        case JAVATYPE_MESSAGE:
          if (IsLazy(field, context->EnforceLite())) {
            return new ImmutableLazyMessageFieldLiteGenerator(
                field, messageBitIndex, builderBitIndex, context);
          } else {
            return new ImmutableMessageFieldLiteGenerator(
                field, messageBitIndex, builderBitIndex, context);
          }
        case JAVATYPE_ENUM:
          return new ImmutableEnumFieldLiteGenerator(
              field, messageBitIndex, builderBitIndex, context);
        case JAVATYPE_STRING:
          return new ImmutableStringFieldLiteGenerator(
              field, messageBitIndex, builderBitIndex, context);
        default:
          return new ImmutablePrimitiveFieldLiteGenerator(
              field, messageBitIndex, builderBitIndex, context);
      }
    }
  }
}


static inline void ReportUnexpectedPackedFieldsCall(io::Printer* printer) {
  // Reaching here indicates a bug. Cases are:
  //   - This FieldGenerator should support packing,
  //     but this method should be overridden.
  //   - This FieldGenerator doesn't support packing, and this method
  //     should never have been called.
  GOOGLE_LOG(FATAL) << "GenerateParsingCodeFromPacked() "
             << "called on field generator that does not support packing.";
}

}  // namespace

ImmutableFieldGenerator::~ImmutableFieldGenerator() {}

void ImmutableFieldGenerator::
GenerateParsingCodeFromPacked(io::Printer* printer) const {
  ReportUnexpectedPackedFieldsCall(printer);
}

ImmutableFieldLiteGenerator::~ImmutableFieldLiteGenerator() {}

void ImmutableFieldLiteGenerator::
GenerateParsingCodeFromPacked(io::Printer* printer) const {
  ReportUnexpectedPackedFieldsCall(printer);
}

// ===================================================================

template <>
FieldGeneratorMap<ImmutableFieldGenerator>::FieldGeneratorMap(
    const Descriptor* descriptor, Context* context)
    : descriptor_(descriptor),
      field_generators_(new google::protobuf::scoped_ptr<
          ImmutableFieldGenerator>[descriptor->field_count()]) {

  // Construct all the FieldGenerators and assign them bit indices for their
  // bit fields.
  int messageBitIndex = 0;
  int builderBitIndex = 0;
  for (int i = 0; i < descriptor->field_count(); i++) {
    ImmutableFieldGenerator* generator = MakeImmutableGenerator(
        descriptor->field(i), messageBitIndex, builderBitIndex, context);
    field_generators_[i].reset(generator);
    messageBitIndex += generator->GetNumBitsForMessage();
    builderBitIndex += generator->GetNumBitsForBuilder();
  }
}

template<>
FieldGeneratorMap<ImmutableFieldGenerator>::~FieldGeneratorMap() {}

template <>
FieldGeneratorMap<ImmutableFieldLiteGenerator>::FieldGeneratorMap(
    const Descriptor* descriptor, Context* context)
    : descriptor_(descriptor),
      field_generators_(new google::protobuf::scoped_ptr<
          ImmutableFieldLiteGenerator>[descriptor->field_count()]) {
  // Construct all the FieldGenerators and assign them bit indices for their
  // bit fields.
  int messageBitIndex = 0;
  int builderBitIndex = 0;
  for (int i = 0; i < descriptor->field_count(); i++) {
    ImmutableFieldLiteGenerator* generator = MakeImmutableLiteGenerator(
        descriptor->field(i), messageBitIndex, builderBitIndex, context);
    field_generators_[i].reset(generator);
    messageBitIndex += generator->GetNumBitsForMessage();
    builderBitIndex += generator->GetNumBitsForBuilder();
  }
}

template<>
FieldGeneratorMap<ImmutableFieldLiteGenerator>::~FieldGeneratorMap() {}


void SetCommonFieldVariables(const FieldDescriptor* descriptor,
                             const FieldGeneratorInfo* info,
                             std::map<string, string>* variables) {
  (*variables)["field_name"] = descriptor->name();
  (*variables)["name"] = info->name;
  (*variables)["capitalized_name"] = info->capitalized_name;
  (*variables)["disambiguated_reason"] = info->disambiguated_reason;
  (*variables)["constant_name"] = FieldConstantName(descriptor);
  (*variables)["number"] = SimpleItoa(descriptor->number());
}

const FieldDescriptor* MapKeyField(const FieldDescriptor* descriptor) {
  GOOGLE_CHECK_EQ(FieldDescriptor::TYPE_MESSAGE, descriptor->type());
  const Descriptor* message = descriptor->message_type();
  GOOGLE_CHECK(message->options().map_entry());
  return message->FindFieldByName("key");
}

const FieldDescriptor* MapValueField(const FieldDescriptor* descriptor) {
  GOOGLE_CHECK_EQ(FieldDescriptor::TYPE_MESSAGE, descriptor->type());
  const Descriptor* message = descriptor->message_type();
  GOOGLE_CHECK(message->options().map_entry());
  return message->FindFieldByName("value");
}

void SetMapFieldVariables(const FieldDescriptor* descriptor,
                          const FieldGeneratorInfo* info,
                          Context* context,
                          std::map<string, string>* variables) {
  SetCommonFieldVariables(descriptor, info, variables);

  ClassNameResolver* name_resolver = context->GetNameResolver();
  (*variables)["type"] =
      name_resolver->GetImmutableClassName(descriptor->message_type());
  const FieldDescriptor* key = MapKeyField(descriptor);
  const FieldDescriptor* value = MapValueField(descriptor);
  const JavaType key_java_type = GetJavaType(key);
  const JavaType value_java_type = GetJavaType(value);

  (*variables)["key_type"] = MapFieldTypeName(key, name_resolver, false);
  (*variables)["boxed_key_type"] =
      MapFieldTypeName(key, name_resolver, true);
  (*variables)["key_wire_type"] = MapFieldWireType(key);
  (*variables)["key_default_value"] =
      DefaultValue(key, true, name_resolver);
  (*variables)["key_null_check"] = IsReferenceType(key_java_type) ?
      "if (key == null) { throw new java.lang.NullPointerException(); }" : "";
  (*variables)["value_null_check"] = IsReferenceType(value_java_type) ?
      "if (value == null) { throw new java.lang.NullPointerException(); }" : "";

  if (value_java_type == JAVATYPE_ENUM) {
    (*variables)["value_type"] = "int";
    (*variables)["boxed_value_type"] = "java.lang.Integer";
    (*variables)["value_wire_type"] = MapFieldWireType(value);
    (*variables)["value_default_value"] =
        DefaultValue(value, true, name_resolver) + ".getNumber()";
    (*variables)["value_enum_type"] =
        MapFieldTypeName(value, name_resolver, false);

    if (SupportUnknownEnumValue(descriptor->file())) {
      (*variables)["unrecognized_value"] =
          (*variables)["value_enum_type"] + ".UNRECOGNIZED";
    } else {
      (*variables)["unrecognized_value"] =
          DefaultValue(value, true, name_resolver);
    }
  } else {
    (*variables)["value_type"] =
        MapFieldTypeName(value, name_resolver, false);
    (*variables)["boxed_value_type"] =
        MapFieldTypeName(value, name_resolver, true);
    (*variables)["value_wire_type"] = MapFieldWireType(value);
    (*variables)["value_default_value"] =
        DefaultValue(value, true, name_resolver);
  }
  (*variables)["type_parameters"] =
      (*variables)["boxed_key_type"] + ", " + (*variables)["boxed_value_type"];
  (*variables)["deprecation"] = descriptor->options().deprecated()
      ? "@java.lang.Deprecated " : "";
  (*variables)["default_entry"] = (*variables)["capitalized_name"] +
      "DefaultEntryHolder.defaultEntry";
}

void GenerateMapFieldInterfaceMembers(
    const FieldDescriptor* descriptor,
    const std::map<string, string>& variables,
    io::Printer* printer) {
  WriteFieldDocComment(printer, descriptor);
  printer->Print(
      variables,
      "$deprecation$int get$capitalized_name$Count();\n");
  WriteFieldDocComment(printer, descriptor);
  printer->Print(
      variables,
      "$deprecation$boolean contains$capitalized_name$(\n"
      "    $key_type$ key);\n");
  if (GetJavaType(MapValueField(descriptor)) == JAVATYPE_ENUM) {
    printer->Print(
        variables,
        "/**\n"
        " * Use {@link #get$capitalized_name$Map()} instead.\n"
        " */\n"
        "@java.lang.Deprecated\n"
        "java.util.Map<$boxed_key_type$, $value_enum_type$>\n"
        "get$capitalized_name$();\n");
    WriteFieldDocComment(printer, descriptor);
    printer->Print(
        variables,
        "$deprecation$java.util.Map<$boxed_key_type$, $value_enum_type$>\n"
        "get$capitalized_name$Map();\n");
    WriteFieldDocComment(printer, descriptor);
    printer->Print(
        variables,
        "$deprecation$$value_enum_type$ get$capitalized_name$OrDefault(\n"
        "    $key_type$ key,\n"
        "    $value_enum_type$ defaultValue);\n");
    WriteFieldDocComment(printer, descriptor);
    printer->Print(
        variables,
        "$deprecation$$value_enum_type$ get$capitalized_name$OrThrow(\n"
        "    $key_type$ key);\n");
    if (SupportUnknownEnumValue(descriptor->file())) {
      printer->Print(
          variables,
          "/**\n"
          " * Use {@link #get$capitalized_name$ValueMap()} instead.\n"
          " */\n"
          "@java.lang.Deprecated\n"
          "java.util.Map<$type_parameters$>\n"
          "get$capitalized_name$Value();\n");
      WriteFieldDocComment(printer, descriptor);
      printer->Print(
          variables,
          "$deprecation$java.util.Map<$type_parameters$>\n"
          "get$capitalized_name$ValueMap();\n");
      WriteFieldDocComment(printer, descriptor);
      printer->Print(
          variables,
          "$deprecation$\n"
          "$value_type$ get$capitalized_name$ValueOrDefault(\n"
          "    $key_type$ key,\n"
          "    $value_type$ defaultValue);\n");
      WriteFieldDocComment(printer, descriptor);
      printer->Print(
          variables,
          "$deprecation$\n"
          "$value_type$ get$capitalized_name$ValueOrThrow(\n"
          "    $key_type$ key);\n");
    }
  } else {
    printer->Print(
        variables,
        "/**\n"
        " * Use {@link #get$capitalized_name$Map()} instead.\n"
        " */\n"
        "@java.lang.Deprecated\n"
        "java.util.Map<$type_parameters$>\n"
        "get$capitalized_name$();\n");
    WriteFieldDocComment(printer, descriptor);
    printer->Print(
        variables,
        "$deprecation$java.util.Map<$type_parameters$>\n"
        "get$capitalized_name$Map();\n");
    WriteFieldDocComment(printer, descriptor);
    printer->Print(
        variables,
        "$deprecation$\n"
        "$value_type$ get$capitalized_name$OrDefault(\n"
        "    $key_type$ key,\n"
        "    $value_type$ defaultValue);\n");
    WriteFieldDocComment(printer, descriptor);
    printer->Print(
        variables,
        "$deprecation$\n"
        "$value_type$ get$capitalized_name$OrThrow(\n"
        "    $key_type$ key);\n");
  }
}

void SetCommonOneofVariables(const FieldDescriptor* descriptor,
                             const OneofGeneratorInfo* info,
                             std::map<string, string>* variables) {
  (*variables)["oneof_name"] = info->name;
  (*variables)["oneof_capitalized_name"] = info->capitalized_name;
  (*variables)["oneof_index"] =
      SimpleItoa(descriptor->containing_oneof()->index());
  (*variables)["set_oneof_case_message"] = info->name +
      "Case_ = " + SimpleItoa(descriptor->number());
  (*variables)["clear_oneof_case_message"] = info->name +
      "Case_ = 0";
  (*variables)["has_oneof_case_message"] = info->name +
      "Case_ == " + SimpleItoa(descriptor->number());
}

void PrintExtraFieldInfo(const std::map<string, string>& variables,
                         io::Printer* printer) {
  const std::map<string, string>::const_iterator it =
      variables.find("disambiguated_reason");
  if (it != variables.end() && !it->second.empty()) {
    printer->Print(
        variables,
        "// An alternative name is used for field \"$field_name$\" because:\n"
        "//     $disambiguated_reason$\n");
  }
}

}  // namespace java
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
