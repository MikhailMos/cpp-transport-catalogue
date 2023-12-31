// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: transport_router.proto

#include "transport_router.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

namespace transport_catalog_serialize {
PROTOBUF_CONSTEXPR RouterSettings::RouterSettings(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.bus_wait_time_)*/0
  , /*decltype(_impl_.bus_velocity_)*/0
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct RouterSettingsDefaultTypeInternal {
  PROTOBUF_CONSTEXPR RouterSettingsDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~RouterSettingsDefaultTypeInternal() {}
  union {
    RouterSettings _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 RouterSettingsDefaultTypeInternal _RouterSettings_default_instance_;
}  // namespace transport_catalog_serialize
static ::_pb::Metadata file_level_metadata_transport_5frouter_2eproto[1];
static constexpr ::_pb::EnumDescriptor const** file_level_enum_descriptors_transport_5frouter_2eproto = nullptr;
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_transport_5frouter_2eproto = nullptr;

const uint32_t TableStruct_transport_5frouter_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::transport_catalog_serialize::RouterSettings, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::transport_catalog_serialize::RouterSettings, _impl_.bus_wait_time_),
  PROTOBUF_FIELD_OFFSET(::transport_catalog_serialize::RouterSettings, _impl_.bus_velocity_),
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::transport_catalog_serialize::RouterSettings)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::transport_catalog_serialize::_RouterSettings_default_instance_._instance,
};

const char descriptor_table_protodef_transport_5frouter_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\026transport_router.proto\022\033transport_cata"
  "log_serialize\"=\n\016RouterSettings\022\025\n\rbus_w"
  "ait_time\030\001 \001(\001\022\024\n\014bus_velocity\030\002 \001(\001b\006pr"
  "oto3"
  ;
static ::_pbi::once_flag descriptor_table_transport_5frouter_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_transport_5frouter_2eproto = {
    false, false, 124, descriptor_table_protodef_transport_5frouter_2eproto,
    "transport_router.proto",
    &descriptor_table_transport_5frouter_2eproto_once, nullptr, 0, 1,
    schemas, file_default_instances, TableStruct_transport_5frouter_2eproto::offsets,
    file_level_metadata_transport_5frouter_2eproto, file_level_enum_descriptors_transport_5frouter_2eproto,
    file_level_service_descriptors_transport_5frouter_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_transport_5frouter_2eproto_getter() {
  return &descriptor_table_transport_5frouter_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_transport_5frouter_2eproto(&descriptor_table_transport_5frouter_2eproto);
namespace transport_catalog_serialize {

// ===================================================================

class RouterSettings::_Internal {
 public:
};

RouterSettings::RouterSettings(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:transport_catalog_serialize.RouterSettings)
}
RouterSettings::RouterSettings(const RouterSettings& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  RouterSettings* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.bus_wait_time_){}
    , decltype(_impl_.bus_velocity_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&_impl_.bus_wait_time_, &from._impl_.bus_wait_time_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.bus_velocity_) -
    reinterpret_cast<char*>(&_impl_.bus_wait_time_)) + sizeof(_impl_.bus_velocity_));
  // @@protoc_insertion_point(copy_constructor:transport_catalog_serialize.RouterSettings)
}

inline void RouterSettings::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.bus_wait_time_){0}
    , decltype(_impl_.bus_velocity_){0}
    , /*decltype(_impl_._cached_size_)*/{}
  };
}

RouterSettings::~RouterSettings() {
  // @@protoc_insertion_point(destructor:transport_catalog_serialize.RouterSettings)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void RouterSettings::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
}

void RouterSettings::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void RouterSettings::Clear() {
// @@protoc_insertion_point(message_clear_start:transport_catalog_serialize.RouterSettings)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&_impl_.bus_wait_time_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.bus_velocity_) -
      reinterpret_cast<char*>(&_impl_.bus_wait_time_)) + sizeof(_impl_.bus_velocity_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* RouterSettings::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // double bus_wait_time = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 9)) {
          _impl_.bus_wait_time_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<double>(ptr);
          ptr += sizeof(double);
        } else
          goto handle_unusual;
        continue;
      // double bus_velocity = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 17)) {
          _impl_.bus_velocity_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<double>(ptr);
          ptr += sizeof(double);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* RouterSettings::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:transport_catalog_serialize.RouterSettings)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // double bus_wait_time = 1;
  static_assert(sizeof(uint64_t) == sizeof(double), "Code assumes uint64_t and double are the same size.");
  double tmp_bus_wait_time = this->_internal_bus_wait_time();
  uint64_t raw_bus_wait_time;
  memcpy(&raw_bus_wait_time, &tmp_bus_wait_time, sizeof(tmp_bus_wait_time));
  if (raw_bus_wait_time != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteDoubleToArray(1, this->_internal_bus_wait_time(), target);
  }

  // double bus_velocity = 2;
  static_assert(sizeof(uint64_t) == sizeof(double), "Code assumes uint64_t and double are the same size.");
  double tmp_bus_velocity = this->_internal_bus_velocity();
  uint64_t raw_bus_velocity;
  memcpy(&raw_bus_velocity, &tmp_bus_velocity, sizeof(tmp_bus_velocity));
  if (raw_bus_velocity != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteDoubleToArray(2, this->_internal_bus_velocity(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:transport_catalog_serialize.RouterSettings)
  return target;
}

size_t RouterSettings::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:transport_catalog_serialize.RouterSettings)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // double bus_wait_time = 1;
  static_assert(sizeof(uint64_t) == sizeof(double), "Code assumes uint64_t and double are the same size.");
  double tmp_bus_wait_time = this->_internal_bus_wait_time();
  uint64_t raw_bus_wait_time;
  memcpy(&raw_bus_wait_time, &tmp_bus_wait_time, sizeof(tmp_bus_wait_time));
  if (raw_bus_wait_time != 0) {
    total_size += 1 + 8;
  }

  // double bus_velocity = 2;
  static_assert(sizeof(uint64_t) == sizeof(double), "Code assumes uint64_t and double are the same size.");
  double tmp_bus_velocity = this->_internal_bus_velocity();
  uint64_t raw_bus_velocity;
  memcpy(&raw_bus_velocity, &tmp_bus_velocity, sizeof(tmp_bus_velocity));
  if (raw_bus_velocity != 0) {
    total_size += 1 + 8;
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData RouterSettings::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    RouterSettings::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*RouterSettings::GetClassData() const { return &_class_data_; }


void RouterSettings::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<RouterSettings*>(&to_msg);
  auto& from = static_cast<const RouterSettings&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:transport_catalog_serialize.RouterSettings)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  static_assert(sizeof(uint64_t) == sizeof(double), "Code assumes uint64_t and double are the same size.");
  double tmp_bus_wait_time = from._internal_bus_wait_time();
  uint64_t raw_bus_wait_time;
  memcpy(&raw_bus_wait_time, &tmp_bus_wait_time, sizeof(tmp_bus_wait_time));
  if (raw_bus_wait_time != 0) {
    _this->_internal_set_bus_wait_time(from._internal_bus_wait_time());
  }
  static_assert(sizeof(uint64_t) == sizeof(double), "Code assumes uint64_t and double are the same size.");
  double tmp_bus_velocity = from._internal_bus_velocity();
  uint64_t raw_bus_velocity;
  memcpy(&raw_bus_velocity, &tmp_bus_velocity, sizeof(tmp_bus_velocity));
  if (raw_bus_velocity != 0) {
    _this->_internal_set_bus_velocity(from._internal_bus_velocity());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void RouterSettings::CopyFrom(const RouterSettings& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:transport_catalog_serialize.RouterSettings)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RouterSettings::IsInitialized() const {
  return true;
}

void RouterSettings::InternalSwap(RouterSettings* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(RouterSettings, _impl_.bus_velocity_)
      + sizeof(RouterSettings::_impl_.bus_velocity_)
      - PROTOBUF_FIELD_OFFSET(RouterSettings, _impl_.bus_wait_time_)>(
          reinterpret_cast<char*>(&_impl_.bus_wait_time_),
          reinterpret_cast<char*>(&other->_impl_.bus_wait_time_));
}

::PROTOBUF_NAMESPACE_ID::Metadata RouterSettings::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_transport_5frouter_2eproto_getter, &descriptor_table_transport_5frouter_2eproto_once,
      file_level_metadata_transport_5frouter_2eproto[0]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace transport_catalog_serialize
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::transport_catalog_serialize::RouterSettings*
Arena::CreateMaybeMessage< ::transport_catalog_serialize::RouterSettings >(Arena* arena) {
  return Arena::CreateMessageInternal< ::transport_catalog_serialize::RouterSettings >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
