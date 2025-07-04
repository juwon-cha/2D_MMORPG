// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_HEARTBEAT_H_
#define FLATBUFFERS_GENERATED_HEARTBEAT_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 24 &&
              FLATBUFFERS_VERSION_MINOR == 12 &&
              FLATBUFFERS_VERSION_REVISION == 23,
             "Non-compatible flatbuffers version included");

struct C_HEARTBEAT;
struct C_HEARTBEATBuilder;

struct SC_HEARTBEAT;
struct SC_HEARTBEATBuilder;

enum HeartbeatType : uint16_t {
  HeartbeatType_C_HEARTBEAT = 0,
  HeartbeatType_SC_HEARTBEAT = 1,
  HeartbeatType_MIN = HeartbeatType_C_HEARTBEAT,
  HeartbeatType_MAX = HeartbeatType_SC_HEARTBEAT
};

inline const HeartbeatType (&EnumValuesHeartbeatType())[2] {
  static const HeartbeatType values[] = {
    HeartbeatType_C_HEARTBEAT,
    HeartbeatType_SC_HEARTBEAT
  };
  return values;
}

inline const char * const *EnumNamesHeartbeatType() {
  static const char * const names[3] = {
    "C_HEARTBEAT",
    "SC_HEARTBEAT",
    nullptr
  };
  return names;
}

inline const char *EnumNameHeartbeatType(HeartbeatType e) {
  if (::flatbuffers::IsOutRange(e, HeartbeatType_C_HEARTBEAT, HeartbeatType_SC_HEARTBEAT)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesHeartbeatType()[index];
}

struct C_HEARTBEAT FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef C_HEARTBEATBuilder Builder;
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           verifier.EndTable();
  }
};

struct C_HEARTBEATBuilder {
  typedef C_HEARTBEAT Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  explicit C_HEARTBEATBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<C_HEARTBEAT> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<C_HEARTBEAT>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<C_HEARTBEAT> CreateC_HEARTBEAT(
    ::flatbuffers::FlatBufferBuilder &_fbb) {
  C_HEARTBEATBuilder builder_(_fbb);
  return builder_.Finish();
}

struct SC_HEARTBEAT FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef SC_HEARTBEATBuilder Builder;
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           verifier.EndTable();
  }
};

struct SC_HEARTBEATBuilder {
  typedef SC_HEARTBEAT Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  explicit SC_HEARTBEATBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<SC_HEARTBEAT> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<SC_HEARTBEAT>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<SC_HEARTBEAT> CreateSC_HEARTBEAT(
    ::flatbuffers::FlatBufferBuilder &_fbb) {
  SC_HEARTBEATBuilder builder_(_fbb);
  return builder_.Finish();
}

#endif  // FLATBUFFERS_GENERATED_HEARTBEAT_H_
