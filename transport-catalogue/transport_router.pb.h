// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: transport_router.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_transport_5frouter_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_transport_5frouter_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_transport_5frouter_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_transport_5frouter_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_transport_5frouter_2eproto;
namespace transport_catalog_serialize {
class RouterSettings;
struct RouterSettingsDefaultTypeInternal;
extern RouterSettingsDefaultTypeInternal _RouterSettings_default_instance_;
}  // namespace transport_catalog_serialize
PROTOBUF_NAMESPACE_OPEN
template<> ::transport_catalog_serialize::RouterSettings* Arena::CreateMaybeMessage<::transport_catalog_serialize::RouterSettings>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace transport_catalog_serialize {

// ===================================================================

class RouterSettings final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:transport_catalog_serialize.RouterSettings) */ {
 public:
  inline RouterSettings() : RouterSettings(nullptr) {}
  ~RouterSettings() override;
  explicit PROTOBUF_CONSTEXPR RouterSettings(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  RouterSettings(const RouterSettings& from);
  RouterSettings(RouterSettings&& from) noexcept
    : RouterSettings() {
    *this = ::std::move(from);
  }

  inline RouterSettings& operator=(const RouterSettings& from) {
    CopyFrom(from);
    return *this;
  }
  inline RouterSettings& operator=(RouterSettings&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const RouterSettings& default_instance() {
    return *internal_default_instance();
  }
  static inline const RouterSettings* internal_default_instance() {
    return reinterpret_cast<const RouterSettings*>(
               &_RouterSettings_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(RouterSettings& a, RouterSettings& b) {
    a.Swap(&b);
  }
  inline void Swap(RouterSettings* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(RouterSettings* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  RouterSettings* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<RouterSettings>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const RouterSettings& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const RouterSettings& from) {
    RouterSettings::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(RouterSettings* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "transport_catalog_serialize.RouterSettings";
  }
  protected:
  explicit RouterSettings(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kBusWaitTimeFieldNumber = 1,
    kBusVelocityFieldNumber = 2,
  };
  // double bus_wait_time = 1;
  void clear_bus_wait_time();
  double bus_wait_time() const;
  void set_bus_wait_time(double value);
  private:
  double _internal_bus_wait_time() const;
  void _internal_set_bus_wait_time(double value);
  public:

  // double bus_velocity = 2;
  void clear_bus_velocity();
  double bus_velocity() const;
  void set_bus_velocity(double value);
  private:
  double _internal_bus_velocity() const;
  void _internal_set_bus_velocity(double value);
  public:

  // @@protoc_insertion_point(class_scope:transport_catalog_serialize.RouterSettings)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    double bus_wait_time_;
    double bus_velocity_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_transport_5frouter_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// RouterSettings

// double bus_wait_time = 1;
inline void RouterSettings::clear_bus_wait_time() {
  _impl_.bus_wait_time_ = 0;
}
inline double RouterSettings::_internal_bus_wait_time() const {
  return _impl_.bus_wait_time_;
}
inline double RouterSettings::bus_wait_time() const {
  // @@protoc_insertion_point(field_get:transport_catalog_serialize.RouterSettings.bus_wait_time)
  return _internal_bus_wait_time();
}
inline void RouterSettings::_internal_set_bus_wait_time(double value) {
  
  _impl_.bus_wait_time_ = value;
}
inline void RouterSettings::set_bus_wait_time(double value) {
  _internal_set_bus_wait_time(value);
  // @@protoc_insertion_point(field_set:transport_catalog_serialize.RouterSettings.bus_wait_time)
}

// double bus_velocity = 2;
inline void RouterSettings::clear_bus_velocity() {
  _impl_.bus_velocity_ = 0;
}
inline double RouterSettings::_internal_bus_velocity() const {
  return _impl_.bus_velocity_;
}
inline double RouterSettings::bus_velocity() const {
  // @@protoc_insertion_point(field_get:transport_catalog_serialize.RouterSettings.bus_velocity)
  return _internal_bus_velocity();
}
inline void RouterSettings::_internal_set_bus_velocity(double value) {
  
  _impl_.bus_velocity_ = value;
}
inline void RouterSettings::set_bus_velocity(double value) {
  _internal_set_bus_velocity(value);
  // @@protoc_insertion_point(field_set:transport_catalog_serialize.RouterSettings.bus_velocity)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace transport_catalog_serialize

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_transport_5frouter_2eproto
