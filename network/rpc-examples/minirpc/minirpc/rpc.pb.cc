// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rpc.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "rpc.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace minirpc {
class RpcMessageDefaultTypeInternal : public ::google::protobuf::internal::ExplicitlyConstructed<RpcMessage> {
} _RpcMessage_default_instance_;

namespace protobuf_rpc_2eproto {


namespace {

::google::protobuf::Metadata file_level_metadata[1];
const ::google::protobuf::EnumDescriptor* file_level_enum_descriptors[2];

}  // namespace

const ::google::protobuf::uint32 TableStruct::offsets[] = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RpcMessage, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RpcMessage, type_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RpcMessage, id_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RpcMessage, service_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RpcMessage, method_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RpcMessage, request_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RpcMessage, response_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RpcMessage, ec_),
};

static const ::google::protobuf::internal::MigrationSchema schemas[] = {
  { 0, -1, sizeof(RpcMessage)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&_RpcMessage_default_instance_),
};

namespace {

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "rpc.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, file_level_enum_descriptors, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

}  // namespace

void TableStruct::Shutdown() {
  _RpcMessage_default_instance_.Shutdown();
  delete file_level_metadata[0].reflection;
}

void TableStruct::InitDefaultsImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::internal::InitProtobufDefaults();
  _RpcMessage_default_instance_.DefaultConstruct();
}

void InitDefaults() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &TableStruct::InitDefaultsImpl);
}
void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] = {
      "\n\trpc.proto\022\007minirpc\"\240\001\n\nRpcMessage\022\"\n\004t"
      "ype\030\001 \001(\0162\024.minirpc.MessageType\022\n\n\002id\030\002 "
      "\001(\006\022\017\n\007service\030\003 \001(\t\022\016\n\006method\030\004 \001(\t\022\017\n\007"
      "request\030\005 \001(\014\022\020\n\010response\030\006 \001(\014\022\036\n\002ec\030\007 "
      "\001(\0162\022.minirpc.ErrorCode*<\n\013MessageType\022\014"
      "\n\010MT_ERROR\020\000\022\016\n\nMT_REQUEST\020\001\022\017\n\013MT_RESPO"
      "NSE\020\002*u\n\tErrorCode\022\022\n\016EC_WRONG_PROTO\020\000\022\021"
      "\n\rEC_NO_SERVICE\020\001\022\020\n\014EC_NO_METHOD\020\002\022\026\n\022E"
      "C_INVALID_REQUEST\020\003\022\027\n\023EC_INVALID_RESPON"
      "SE\020\004B\003\200\001\001b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 377);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "rpc.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::OnShutdown(&TableStruct::Shutdown);
}

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;

}  // namespace protobuf_rpc_2eproto

const ::google::protobuf::EnumDescriptor* MessageType_descriptor() {
  protobuf_rpc_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_rpc_2eproto::file_level_enum_descriptors[0];
}
bool MessageType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* ErrorCode_descriptor() {
  protobuf_rpc_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_rpc_2eproto::file_level_enum_descriptors[1];
}
bool ErrorCode_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      return true;
    default:
      return false;
  }
}


// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int RpcMessage::kTypeFieldNumber;
const int RpcMessage::kIdFieldNumber;
const int RpcMessage::kServiceFieldNumber;
const int RpcMessage::kMethodFieldNumber;
const int RpcMessage::kRequestFieldNumber;
const int RpcMessage::kResponseFieldNumber;
const int RpcMessage::kEcFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

RpcMessage::RpcMessage()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    protobuf_rpc_2eproto::InitDefaults();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:minirpc.RpcMessage)
}
RpcMessage::RpcMessage(const RpcMessage& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  service_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.service().size() > 0) {
    service_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.service_);
  }
  method_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.method().size() > 0) {
    method_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.method_);
  }
  request_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.request().size() > 0) {
    request_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.request_);
  }
  response_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.response().size() > 0) {
    response_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.response_);
  }
  ::memcpy(&id_, &from.id_,
    reinterpret_cast<char*>(&ec_) -
    reinterpret_cast<char*>(&id_) + sizeof(ec_));
  // @@protoc_insertion_point(copy_constructor:minirpc.RpcMessage)
}

void RpcMessage::SharedCtor() {
  service_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  method_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  request_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  response_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&id_, 0, reinterpret_cast<char*>(&ec_) -
    reinterpret_cast<char*>(&id_) + sizeof(ec_));
  _cached_size_ = 0;
}

RpcMessage::~RpcMessage() {
  // @@protoc_insertion_point(destructor:minirpc.RpcMessage)
  SharedDtor();
}

void RpcMessage::SharedDtor() {
  service_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  method_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  request_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  response_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void RpcMessage::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* RpcMessage::descriptor() {
  protobuf_rpc_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_rpc_2eproto::file_level_metadata[0].descriptor;
}

const RpcMessage& RpcMessage::default_instance() {
  protobuf_rpc_2eproto::InitDefaults();
  return *internal_default_instance();
}

RpcMessage* RpcMessage::New(::google::protobuf::Arena* arena) const {
  RpcMessage* n = new RpcMessage;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void RpcMessage::Clear() {
// @@protoc_insertion_point(message_clear_start:minirpc.RpcMessage)
  service_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  method_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  request_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  response_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&id_, 0, reinterpret_cast<char*>(&ec_) -
    reinterpret_cast<char*>(&id_) + sizeof(ec_));
}

bool RpcMessage::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:minirpc.RpcMessage)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // .minirpc.MessageType type = 1;
      case 1: {
        if (tag == 8u) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          set_type(static_cast< ::minirpc::MessageType >(value));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // fixed64 id = 2;
      case 2: {
        if (tag == 17u) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED64>(
                 input, &id_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string service = 3;
      case 3: {
        if (tag == 26u) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_service()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->service().data(), this->service().length(),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "minirpc.RpcMessage.service"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string method = 4;
      case 4: {
        if (tag == 34u) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_method()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->method().data(), this->method().length(),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "minirpc.RpcMessage.method"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // bytes request = 5;
      case 5: {
        if (tag == 42u) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_request()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // bytes response = 6;
      case 6: {
        if (tag == 50u) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_response()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // .minirpc.ErrorCode ec = 7;
      case 7: {
        if (tag == 56u) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          set_ec(static_cast< ::minirpc::ErrorCode >(value));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:minirpc.RpcMessage)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:minirpc.RpcMessage)
  return false;
#undef DO_
}

void RpcMessage::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:minirpc.RpcMessage)
  // .minirpc.MessageType type = 1;
  if (this->type() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->type(), output);
  }

  // fixed64 id = 2;
  if (this->id() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteFixed64(2, this->id(), output);
  }

  // string service = 3;
  if (this->service().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->service().data(), this->service().length(),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "minirpc.RpcMessage.service");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      3, this->service(), output);
  }

  // string method = 4;
  if (this->method().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->method().data(), this->method().length(),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "minirpc.RpcMessage.method");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      4, this->method(), output);
  }

  // bytes request = 5;
  if (this->request().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      5, this->request(), output);
  }

  // bytes response = 6;
  if (this->response().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      6, this->response(), output);
  }

  // .minirpc.ErrorCode ec = 7;
  if (this->ec() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      7, this->ec(), output);
  }

  // @@protoc_insertion_point(serialize_end:minirpc.RpcMessage)
}

::google::protobuf::uint8* RpcMessage::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic;  // Unused
  // @@protoc_insertion_point(serialize_to_array_start:minirpc.RpcMessage)
  // .minirpc.MessageType type = 1;
  if (this->type() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->type(), target);
  }

  // fixed64 id = 2;
  if (this->id() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFixed64ToArray(2, this->id(), target);
  }

  // string service = 3;
  if (this->service().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->service().data(), this->service().length(),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "minirpc.RpcMessage.service");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->service(), target);
  }

  // string method = 4;
  if (this->method().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->method().data(), this->method().length(),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "minirpc.RpcMessage.method");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        4, this->method(), target);
  }

  // bytes request = 5;
  if (this->request().size() > 0) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        5, this->request(), target);
  }

  // bytes response = 6;
  if (this->response().size() > 0) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        6, this->response(), target);
  }

  // .minirpc.ErrorCode ec = 7;
  if (this->ec() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      7, this->ec(), target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:minirpc.RpcMessage)
  return target;
}

size_t RpcMessage::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:minirpc.RpcMessage)
  size_t total_size = 0;

  // string service = 3;
  if (this->service().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->service());
  }

  // string method = 4;
  if (this->method().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->method());
  }

  // bytes request = 5;
  if (this->request().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::BytesSize(
        this->request());
  }

  // bytes response = 6;
  if (this->response().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::BytesSize(
        this->response());
  }

  // fixed64 id = 2;
  if (this->id() != 0) {
    total_size += 1 + 8;
  }

  // .minirpc.MessageType type = 1;
  if (this->type() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::EnumSize(this->type());
  }

  // .minirpc.ErrorCode ec = 7;
  if (this->ec() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::EnumSize(this->ec());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void RpcMessage::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:minirpc.RpcMessage)
  GOOGLE_DCHECK_NE(&from, this);
  const RpcMessage* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const RpcMessage>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:minirpc.RpcMessage)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:minirpc.RpcMessage)
    MergeFrom(*source);
  }
}

void RpcMessage::MergeFrom(const RpcMessage& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:minirpc.RpcMessage)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  if (from.service().size() > 0) {

    service_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.service_);
  }
  if (from.method().size() > 0) {

    method_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.method_);
  }
  if (from.request().size() > 0) {

    request_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.request_);
  }
  if (from.response().size() > 0) {

    response_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.response_);
  }
  if (from.id() != 0) {
    set_id(from.id());
  }
  if (from.type() != 0) {
    set_type(from.type());
  }
  if (from.ec() != 0) {
    set_ec(from.ec());
  }
}

void RpcMessage::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:minirpc.RpcMessage)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RpcMessage::CopyFrom(const RpcMessage& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:minirpc.RpcMessage)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RpcMessage::IsInitialized() const {
  return true;
}

void RpcMessage::Swap(RpcMessage* other) {
  if (other == this) return;
  InternalSwap(other);
}
void RpcMessage::InternalSwap(RpcMessage* other) {
  service_.Swap(&other->service_);
  method_.Swap(&other->method_);
  request_.Swap(&other->request_);
  response_.Swap(&other->response_);
  std::swap(id_, other->id_);
  std::swap(type_, other->type_);
  std::swap(ec_, other->ec_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata RpcMessage::GetMetadata() const {
  protobuf_rpc_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_rpc_2eproto::file_level_metadata[0];
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// RpcMessage

// .minirpc.MessageType type = 1;
void RpcMessage::clear_type() {
  type_ = 0;
}
::minirpc::MessageType RpcMessage::type() const {
  // @@protoc_insertion_point(field_get:minirpc.RpcMessage.type)
  return static_cast< ::minirpc::MessageType >(type_);
}
void RpcMessage::set_type(::minirpc::MessageType value) {
  
  type_ = value;
  // @@protoc_insertion_point(field_set:minirpc.RpcMessage.type)
}

// fixed64 id = 2;
void RpcMessage::clear_id() {
  id_ = GOOGLE_ULONGLONG(0);
}
::google::protobuf::uint64 RpcMessage::id() const {
  // @@protoc_insertion_point(field_get:minirpc.RpcMessage.id)
  return id_;
}
void RpcMessage::set_id(::google::protobuf::uint64 value) {
  
  id_ = value;
  // @@protoc_insertion_point(field_set:minirpc.RpcMessage.id)
}

// string service = 3;
void RpcMessage::clear_service() {
  service_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
const ::std::string& RpcMessage::service() const {
  // @@protoc_insertion_point(field_get:minirpc.RpcMessage.service)
  return service_.GetNoArena();
}
void RpcMessage::set_service(const ::std::string& value) {
  
  service_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:minirpc.RpcMessage.service)
}
#if LANG_CXX11
void RpcMessage::set_service(::std::string&& value) {
  
  service_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:minirpc.RpcMessage.service)
}
#endif
void RpcMessage::set_service(const char* value) {
  
  service_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:minirpc.RpcMessage.service)
}
void RpcMessage::set_service(const char* value, size_t size) {
  
  service_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:minirpc.RpcMessage.service)
}
::std::string* RpcMessage::mutable_service() {
  
  // @@protoc_insertion_point(field_mutable:minirpc.RpcMessage.service)
  return service_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
::std::string* RpcMessage::release_service() {
  // @@protoc_insertion_point(field_release:minirpc.RpcMessage.service)
  
  return service_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
void RpcMessage::set_allocated_service(::std::string* service) {
  if (service != NULL) {
    
  } else {
    
  }
  service_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), service);
  // @@protoc_insertion_point(field_set_allocated:minirpc.RpcMessage.service)
}

// string method = 4;
void RpcMessage::clear_method() {
  method_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
const ::std::string& RpcMessage::method() const {
  // @@protoc_insertion_point(field_get:minirpc.RpcMessage.method)
  return method_.GetNoArena();
}
void RpcMessage::set_method(const ::std::string& value) {
  
  method_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:minirpc.RpcMessage.method)
}
#if LANG_CXX11
void RpcMessage::set_method(::std::string&& value) {
  
  method_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:minirpc.RpcMessage.method)
}
#endif
void RpcMessage::set_method(const char* value) {
  
  method_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:minirpc.RpcMessage.method)
}
void RpcMessage::set_method(const char* value, size_t size) {
  
  method_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:minirpc.RpcMessage.method)
}
::std::string* RpcMessage::mutable_method() {
  
  // @@protoc_insertion_point(field_mutable:minirpc.RpcMessage.method)
  return method_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
::std::string* RpcMessage::release_method() {
  // @@protoc_insertion_point(field_release:minirpc.RpcMessage.method)
  
  return method_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
void RpcMessage::set_allocated_method(::std::string* method) {
  if (method != NULL) {
    
  } else {
    
  }
  method_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), method);
  // @@protoc_insertion_point(field_set_allocated:minirpc.RpcMessage.method)
}

// bytes request = 5;
void RpcMessage::clear_request() {
  request_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
const ::std::string& RpcMessage::request() const {
  // @@protoc_insertion_point(field_get:minirpc.RpcMessage.request)
  return request_.GetNoArena();
}
void RpcMessage::set_request(const ::std::string& value) {
  
  request_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:minirpc.RpcMessage.request)
}
#if LANG_CXX11
void RpcMessage::set_request(::std::string&& value) {
  
  request_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:minirpc.RpcMessage.request)
}
#endif
void RpcMessage::set_request(const char* value) {
  
  request_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:minirpc.RpcMessage.request)
}
void RpcMessage::set_request(const void* value, size_t size) {
  
  request_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:minirpc.RpcMessage.request)
}
::std::string* RpcMessage::mutable_request() {
  
  // @@protoc_insertion_point(field_mutable:minirpc.RpcMessage.request)
  return request_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
::std::string* RpcMessage::release_request() {
  // @@protoc_insertion_point(field_release:minirpc.RpcMessage.request)
  
  return request_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
void RpcMessage::set_allocated_request(::std::string* request) {
  if (request != NULL) {
    
  } else {
    
  }
  request_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), request);
  // @@protoc_insertion_point(field_set_allocated:minirpc.RpcMessage.request)
}

// bytes response = 6;
void RpcMessage::clear_response() {
  response_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
const ::std::string& RpcMessage::response() const {
  // @@protoc_insertion_point(field_get:minirpc.RpcMessage.response)
  return response_.GetNoArena();
}
void RpcMessage::set_response(const ::std::string& value) {
  
  response_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:minirpc.RpcMessage.response)
}
#if LANG_CXX11
void RpcMessage::set_response(::std::string&& value) {
  
  response_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:minirpc.RpcMessage.response)
}
#endif
void RpcMessage::set_response(const char* value) {
  
  response_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:minirpc.RpcMessage.response)
}
void RpcMessage::set_response(const void* value, size_t size) {
  
  response_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:minirpc.RpcMessage.response)
}
::std::string* RpcMessage::mutable_response() {
  
  // @@protoc_insertion_point(field_mutable:minirpc.RpcMessage.response)
  return response_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
::std::string* RpcMessage::release_response() {
  // @@protoc_insertion_point(field_release:minirpc.RpcMessage.response)
  
  return response_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
void RpcMessage::set_allocated_response(::std::string* response) {
  if (response != NULL) {
    
  } else {
    
  }
  response_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), response);
  // @@protoc_insertion_point(field_set_allocated:minirpc.RpcMessage.response)
}

// .minirpc.ErrorCode ec = 7;
void RpcMessage::clear_ec() {
  ec_ = 0;
}
::minirpc::ErrorCode RpcMessage::ec() const {
  // @@protoc_insertion_point(field_get:minirpc.RpcMessage.ec)
  return static_cast< ::minirpc::ErrorCode >(ec_);
}
void RpcMessage::set_ec(::minirpc::ErrorCode value) {
  
  ec_ = value;
  // @@protoc_insertion_point(field_set:minirpc.RpcMessage.ec)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace minirpc

// @@protoc_insertion_point(global_scope)