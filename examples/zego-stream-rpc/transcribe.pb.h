// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: transcribe.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_transcribe_2eproto_2epb_2eh
#define GOOGLE_PROTOBUF_INCLUDED_transcribe_2eproto_2epb_2eh

#include <limits>
#include <string>
#include <type_traits>

#include "google/protobuf/port_def.inc"
#if PROTOBUF_VERSION < 4023000
#error "This file was generated by a newer version of protoc which is"
#error "incompatible with your Protocol Buffer headers. Please update"
#error "your headers."
#endif  // PROTOBUF_VERSION

#if 4023001 < PROTOBUF_MIN_PROTOC_VERSION
#error "This file was generated by an older version of protoc which is"
#error "incompatible with your Protocol Buffer headers. Please"
#error "regenerate this file with a newer version of protoc."
#endif  // PROTOBUF_MIN_PROTOC_VERSION
#include "google/protobuf/port_undef.inc"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/arena.h"
#include "google/protobuf/arenastring.h"
#include "google/protobuf/generated_message_util.h"
#include "google/protobuf/metadata_lite.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/message.h"
#include "google/protobuf/repeated_field.h"  // IWYU pragma: export
#include "google/protobuf/extension_set.h"  // IWYU pragma: export
#include "google/protobuf/unknown_field_set.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"

#define PROTOBUF_INTERNAL_EXPORT_transcribe_2eproto

PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_transcribe_2eproto {
  static const ::uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable
    descriptor_table_transcribe_2eproto;
namespace transcribe {
class AudioData;
struct AudioDataDefaultTypeInternal;
extern AudioDataDefaultTypeInternal _AudioData_default_instance_;
class Text;
struct TextDefaultTypeInternal;
extern TextDefaultTypeInternal _Text_default_instance_;
}  // namespace transcribe
PROTOBUF_NAMESPACE_OPEN
template <>
::transcribe::AudioData* Arena::CreateMaybeMessage<::transcribe::AudioData>(Arena*);
template <>
::transcribe::Text* Arena::CreateMaybeMessage<::transcribe::Text>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

namespace transcribe {

// ===================================================================


// -------------------------------------------------------------------

class AudioData final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:transcribe.AudioData) */ {
 public:
  inline AudioData() : AudioData(nullptr) {}
  ~AudioData() override;
  template<typename = void>
  explicit PROTOBUF_CONSTEXPR AudioData(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  AudioData(const AudioData& from);
  AudioData(AudioData&& from) noexcept
    : AudioData() {
    *this = ::std::move(from);
  }

  inline AudioData& operator=(const AudioData& from) {
    CopyFrom(from);
    return *this;
  }
  inline AudioData& operator=(AudioData&& from) noexcept {
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

  inline const ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance);
  }
  inline ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
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
  static const AudioData& default_instance() {
    return *internal_default_instance();
  }
  static inline const AudioData* internal_default_instance() {
    return reinterpret_cast<const AudioData*>(
               &_AudioData_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(AudioData& a, AudioData& b) {
    a.Swap(&b);
  }
  inline void Swap(AudioData* other) {
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
  void UnsafeArenaSwap(AudioData* other) {
    if (other == this) return;
    ABSL_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  AudioData* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<AudioData>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const AudioData& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const AudioData& from) {
    AudioData::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  ::size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::uint8_t* _InternalSerialize(
      ::uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(AudioData* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::absl::string_view FullMessageName() {
    return "transcribe.AudioData";
  }
  protected:
  explicit AudioData(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kDataFieldNumber = 1,
  };
  // repeated float data = 1;
  int data_size() const;
  private:
  int _internal_data_size() const;

  public:
  void clear_data() ;
  float data(int index) const;
  void set_data(int index, float value);
  void add_data(float value);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField<float>& data() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedField<float>* mutable_data();

  private:
  float _internal_data(int index) const;
  void _internal_add_data(float value);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedField<float>& _internal_data() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedField<float>* _internal_mutable_data();

  public:
  // @@protoc_insertion_point(class_scope:transcribe.AudioData)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedField<float> data_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_transcribe_2eproto;
};// -------------------------------------------------------------------

class Text final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:transcribe.Text) */ {
 public:
  inline Text() : Text(nullptr) {}
  ~Text() override;
  template<typename = void>
  explicit PROTOBUF_CONSTEXPR Text(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Text(const Text& from);
  Text(Text&& from) noexcept
    : Text() {
    *this = ::std::move(from);
  }

  inline Text& operator=(const Text& from) {
    CopyFrom(from);
    return *this;
  }
  inline Text& operator=(Text&& from) noexcept {
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

  inline const ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance);
  }
  inline ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
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
  static const Text& default_instance() {
    return *internal_default_instance();
  }
  static inline const Text* internal_default_instance() {
    return reinterpret_cast<const Text*>(
               &_Text_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(Text& a, Text& b) {
    a.Swap(&b);
  }
  inline void Swap(Text* other) {
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
  void UnsafeArenaSwap(Text* other) {
    if (other == this) return;
    ABSL_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Text* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Text>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Text& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const Text& from) {
    Text::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  ::size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::uint8_t* _InternalSerialize(
      ::uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Text* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::absl::string_view FullMessageName() {
    return "transcribe.Text";
  }
  protected:
  explicit Text(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kContentFieldNumber = 1,
  };
  // string content = 1;
  void clear_content() ;
  const std::string& content() const;




  template <typename Arg_ = const std::string&, typename... Args_>
  void set_content(Arg_&& arg, Args_... args);
  std::string* mutable_content();
  PROTOBUF_NODISCARD std::string* release_content();
  void set_allocated_content(std::string* ptr);

  private:
  const std::string& _internal_content() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_content(
      const std::string& value);
  std::string* _internal_mutable_content();

  public:
  // @@protoc_insertion_point(class_scope:transcribe.Text)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr content_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_transcribe_2eproto;
};

// ===================================================================




// ===================================================================


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// -------------------------------------------------------------------

// AudioData

// repeated float data = 1;
inline int AudioData::_internal_data_size() const {
  return _impl_.data_.size();
}
inline int AudioData::data_size() const {
  return _internal_data_size();
}
inline void AudioData::clear_data() {
  _internal_mutable_data()->Clear();
}
inline float AudioData::data(int index) const {
  // @@protoc_insertion_point(field_get:transcribe.AudioData.data)
  return _internal_data(index);
}
inline void AudioData::set_data(int index, float value) {
  _internal_mutable_data()->Set(index, value);
  // @@protoc_insertion_point(field_set:transcribe.AudioData.data)
}
inline void AudioData::add_data(float value) {
  _internal_add_data(value);
  // @@protoc_insertion_point(field_add:transcribe.AudioData.data)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField<float>& AudioData::data() const {
  // @@protoc_insertion_point(field_list:transcribe.AudioData.data)
  return _internal_data();
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField<float>* AudioData::mutable_data() {
  // @@protoc_insertion_point(field_mutable_list:transcribe.AudioData.data)
  return _internal_mutable_data();
}

inline float AudioData::_internal_data(int index) const {
  return _internal_data().Get(index);
}
inline void AudioData::_internal_add_data(float value) {
  _internal_mutable_data()->Add(value);
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField<float>& AudioData::_internal_data() const {
  return _impl_.data_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField<float>* AudioData::_internal_mutable_data() {
  return &_impl_.data_;
}

// -------------------------------------------------------------------

// Text

// string content = 1;
inline void Text::clear_content() {
  _impl_.content_.ClearToEmpty();
}
inline const std::string& Text::content() const {
  // @@protoc_insertion_point(field_get:transcribe.Text.content)
  return _internal_content();
}
template <typename Arg_, typename... Args_>
inline PROTOBUF_ALWAYS_INLINE void Text::set_content(Arg_&& arg,
                                                     Args_... args) {
  ;
  _impl_.content_.Set(static_cast<Arg_&&>(arg), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:transcribe.Text.content)
}
inline std::string* Text::mutable_content() {
  std::string* _s = _internal_mutable_content();
  // @@protoc_insertion_point(field_mutable:transcribe.Text.content)
  return _s;
}
inline const std::string& Text::_internal_content() const {
  return _impl_.content_.Get();
}
inline void Text::_internal_set_content(const std::string& value) {
  ;


  _impl_.content_.Set(value, GetArenaForAllocation());
}
inline std::string* Text::_internal_mutable_content() {
  ;
  return _impl_.content_.Mutable( GetArenaForAllocation());
}
inline std::string* Text::release_content() {
  // @@protoc_insertion_point(field_release:transcribe.Text.content)
  return _impl_.content_.Release();
}
inline void Text::set_allocated_content(std::string* value) {
  _impl_.content_.SetAllocated(value, GetArenaForAllocation());
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
        if (_impl_.content_.IsDefault()) {
          _impl_.content_.Set("", GetArenaForAllocation());
        }
  #endif  // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:transcribe.Text.content)
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)
}  // namespace transcribe


// @@protoc_insertion_point(global_scope)

#include "google/protobuf/port_undef.inc"

#endif  // GOOGLE_PROTOBUF_INCLUDED_transcribe_2eproto_2epb_2eh
