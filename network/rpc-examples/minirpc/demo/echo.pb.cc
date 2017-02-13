// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: echo.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "echo.pb.h"

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

namespace echo {
class VoidDefaultTypeInternal : public ::google::protobuf::internal::ExplicitlyConstructed<Void> {
} _Void_default_instance_;
class EchoRequestDefaultTypeInternal : public ::google::protobuf::internal::ExplicitlyConstructed<EchoRequest> {
} _EchoRequest_default_instance_;

namespace protobuf_echo_2eproto {


namespace {

::google::protobuf::Metadata file_level_metadata[2];
const ::google::protobuf::ServiceDescriptor* file_level_service_descriptors[1];

}  // namespace

const ::google::protobuf::uint32 TableStruct::offsets[] = {
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Void, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _has_bits_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(EchoRequest, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(EchoRequest, request_),
};

static const ::google::protobuf::internal::MigrationSchema schemas[] = {
  { 0, -1, sizeof(Void)},
  { 4, -1, sizeof(EchoRequest)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&_Void_default_instance_),
  reinterpret_cast<const ::google::protobuf::Message*>(&_EchoRequest_default_instance_),
};

namespace {

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "echo.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, NULL, file_level_service_descriptors);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 2);
}

}  // namespace

void TableStruct::Shutdown() {
  _Void_default_instance_.Shutdown();
  delete file_level_metadata[0].reflection;
  _EchoRequest_default_instance_.Shutdown();
  delete file_level_metadata[1].reflection;
}

void TableStruct::InitDefaultsImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::internal::InitProtobufDefaults();
  _Void_default_instance_.DefaultConstruct();
  _EchoRequest_default_instance_.DefaultConstruct();
}

void InitDefaults() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &TableStruct::InitDefaultsImpl);
}
void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] = {
      "\n\necho.proto\022\004echo\"\006\n\004Void\"\036\n\013EchoReques"
      "t\022\017\n\007request\030\001 \001(\t29\n\013EchoService\022*\n\tech"
      "o_call\022\021.echo.EchoRequest\032\n.echo.VoidB\003\200"
      "\001\001b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 130);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "echo.proto", &protobuf_RegisterTypes);
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

}  // namespace protobuf_echo_2eproto


// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

Void::Void()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    protobuf_echo_2eproto::InitDefaults();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:echo.Void)
}
Void::Void(const Void& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:echo.Void)
}

void Void::SharedCtor() {
  _cached_size_ = 0;
}

Void::~Void() {
  // @@protoc_insertion_point(destructor:echo.Void)
  SharedDtor();
}

void Void::SharedDtor() {
}

void Void::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Void::descriptor() {
  protobuf_echo_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_echo_2eproto::file_level_metadata[0].descriptor;
}

const Void& Void::default_instance() {
  protobuf_echo_2eproto::InitDefaults();
  return *internal_default_instance();
}

Void* Void::New(::google::protobuf::Arena* arena) const {
  Void* n = new Void;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void Void::Clear() {
// @@protoc_insertion_point(message_clear_start:echo.Void)
}

bool Void::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:echo.Void)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
  handle_unusual:
    if (tag == 0 ||
        ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
        ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
      goto success;
    }
    DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
  }
success:
  // @@protoc_insertion_point(parse_success:echo.Void)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:echo.Void)
  return false;
#undef DO_
}

void Void::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:echo.Void)
  // @@protoc_insertion_point(serialize_end:echo.Void)
}

::google::protobuf::uint8* Void::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic;  // Unused
  // @@protoc_insertion_point(serialize_to_array_start:echo.Void)
  // @@protoc_insertion_point(serialize_to_array_end:echo.Void)
  return target;
}

size_t Void::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:echo.Void)
  size_t total_size = 0;

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Void::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:echo.Void)
  GOOGLE_DCHECK_NE(&from, this);
  const Void* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const Void>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:echo.Void)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:echo.Void)
    MergeFrom(*source);
  }
}

void Void::MergeFrom(const Void& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:echo.Void)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
}

void Void::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:echo.Void)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Void::CopyFrom(const Void& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:echo.Void)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Void::IsInitialized() const {
  return true;
}

void Void::Swap(Void* other) {
  if (other == this) return;
  InternalSwap(other);
}
void Void::InternalSwap(Void* other) {
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata Void::GetMetadata() const {
  protobuf_echo_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_echo_2eproto::file_level_metadata[0];
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// Void

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int EchoRequest::kRequestFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

EchoRequest::EchoRequest()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    protobuf_echo_2eproto::InitDefaults();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:echo.EchoRequest)
}
EchoRequest::EchoRequest(const EchoRequest& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  request_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.request().size() > 0) {
    request_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.request_);
  }
  // @@protoc_insertion_point(copy_constructor:echo.EchoRequest)
}

void EchoRequest::SharedCtor() {
  request_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  _cached_size_ = 0;
}

EchoRequest::~EchoRequest() {
  // @@protoc_insertion_point(destructor:echo.EchoRequest)
  SharedDtor();
}

void EchoRequest::SharedDtor() {
  request_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void EchoRequest::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* EchoRequest::descriptor() {
  protobuf_echo_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_echo_2eproto::file_level_metadata[1].descriptor;
}

const EchoRequest& EchoRequest::default_instance() {
  protobuf_echo_2eproto::InitDefaults();
  return *internal_default_instance();
}

EchoRequest* EchoRequest::New(::google::protobuf::Arena* arena) const {
  EchoRequest* n = new EchoRequest;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void EchoRequest::Clear() {
// @@protoc_insertion_point(message_clear_start:echo.EchoRequest)
  request_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

bool EchoRequest::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:echo.EchoRequest)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // string request = 1;
      case 1: {
        if (tag == 10u) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_request()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->request().data(), this->request().length(),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "echo.EchoRequest.request"));
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
  // @@protoc_insertion_point(parse_success:echo.EchoRequest)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:echo.EchoRequest)
  return false;
#undef DO_
}

void EchoRequest::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:echo.EchoRequest)
  // string request = 1;
  if (this->request().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->request().data(), this->request().length(),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "echo.EchoRequest.request");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->request(), output);
  }

  // @@protoc_insertion_point(serialize_end:echo.EchoRequest)
}

::google::protobuf::uint8* EchoRequest::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic;  // Unused
  // @@protoc_insertion_point(serialize_to_array_start:echo.EchoRequest)
  // string request = 1;
  if (this->request().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->request().data(), this->request().length(),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "echo.EchoRequest.request");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->request(), target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:echo.EchoRequest)
  return target;
}

size_t EchoRequest::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:echo.EchoRequest)
  size_t total_size = 0;

  // string request = 1;
  if (this->request().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->request());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void EchoRequest::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:echo.EchoRequest)
  GOOGLE_DCHECK_NE(&from, this);
  const EchoRequest* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const EchoRequest>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:echo.EchoRequest)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:echo.EchoRequest)
    MergeFrom(*source);
  }
}

void EchoRequest::MergeFrom(const EchoRequest& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:echo.EchoRequest)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  if (from.request().size() > 0) {

    request_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.request_);
  }
}

void EchoRequest::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:echo.EchoRequest)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void EchoRequest::CopyFrom(const EchoRequest& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:echo.EchoRequest)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool EchoRequest::IsInitialized() const {
  return true;
}

void EchoRequest::Swap(EchoRequest* other) {
  if (other == this) return;
  InternalSwap(other);
}
void EchoRequest::InternalSwap(EchoRequest* other) {
  request_.Swap(&other->request_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata EchoRequest::GetMetadata() const {
  protobuf_echo_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_echo_2eproto::file_level_metadata[1];
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// EchoRequest

// string request = 1;
void EchoRequest::clear_request() {
  request_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
const ::std::string& EchoRequest::request() const {
  // @@protoc_insertion_point(field_get:echo.EchoRequest.request)
  return request_.GetNoArena();
}
void EchoRequest::set_request(const ::std::string& value) {
  
  request_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:echo.EchoRequest.request)
}
#if LANG_CXX11
void EchoRequest::set_request(::std::string&& value) {
  
  request_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:echo.EchoRequest.request)
}
#endif
void EchoRequest::set_request(const char* value) {
  
  request_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:echo.EchoRequest.request)
}
void EchoRequest::set_request(const char* value, size_t size) {
  
  request_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:echo.EchoRequest.request)
}
::std::string* EchoRequest::mutable_request() {
  
  // @@protoc_insertion_point(field_mutable:echo.EchoRequest.request)
  return request_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
::std::string* EchoRequest::release_request() {
  // @@protoc_insertion_point(field_release:echo.EchoRequest.request)
  
  return request_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
void EchoRequest::set_allocated_request(::std::string* request) {
  if (request != NULL) {
    
  } else {
    
  }
  request_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), request);
  // @@protoc_insertion_point(field_set_allocated:echo.EchoRequest.request)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// ===================================================================

EchoService::~EchoService() {}

const ::google::protobuf::ServiceDescriptor* EchoService::descriptor() {
  protobuf_echo_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_echo_2eproto::file_level_service_descriptors[0];
}

const ::google::protobuf::ServiceDescriptor* EchoService::GetDescriptor() {
  return descriptor();
}

void EchoService::echo_call(::google::protobuf::RpcController* controller,
                         const ::echo::EchoRequest*,
                         ::echo::Void*,
                         ::google::protobuf::Closure* done) {
  controller->SetFailed("Method echo_call() not implemented.");
  done->Run();
}

void EchoService::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                             ::google::protobuf::RpcController* controller,
                             const ::google::protobuf::Message* request,
                             ::google::protobuf::Message* response,
                             ::google::protobuf::Closure* done) {
  GOOGLE_DCHECK_EQ(method->service(), protobuf_echo_2eproto::file_level_service_descriptors[0]);
  switch(method->index()) {
    case 0:
      echo_call(controller,
             ::google::protobuf::down_cast<const ::echo::EchoRequest*>(request),
             ::google::protobuf::down_cast< ::echo::Void*>(response),
             done);
      break;
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      break;
  }
}

const ::google::protobuf::Message& EchoService::GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::echo::EchoRequest::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *::google::protobuf::MessageFactory::generated_factory()
          ->GetPrototype(method->input_type());
  }
}

const ::google::protobuf::Message& EchoService::GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::echo::Void::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *::google::protobuf::MessageFactory::generated_factory()
          ->GetPrototype(method->output_type());
  }
}

EchoService_Stub::EchoService_Stub(::google::protobuf::RpcChannel* channel)
  : channel_(channel), owns_channel_(false) {}
EchoService_Stub::EchoService_Stub(
    ::google::protobuf::RpcChannel* channel,
    ::google::protobuf::Service::ChannelOwnership ownership)
  : channel_(channel),
    owns_channel_(ownership == ::google::protobuf::Service::STUB_OWNS_CHANNEL) {}
EchoService_Stub::~EchoService_Stub() {
  if (owns_channel_) delete channel_;
}

void EchoService_Stub::echo_call(::google::protobuf::RpcController* controller,
                              const ::echo::EchoRequest* request,
                              ::echo::Void* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(0),
                       controller, request, response, done);
}

// @@protoc_insertion_point(namespace_scope)

}  // namespace echo

// @@protoc_insertion_point(global_scope)
