#pragma once

#include <mutex>
#include <atomic>
#include <memory>

using namespace std;
using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;


using SendBufferRef = shared_ptr<class SendBuffer>;
template <class _Ty>
class BaseRef;

using Int8Ref = shared_ptr<BaseRef<int8>>;
using Int16Ref = shared_ptr<BaseRef<int16>>;
using Int32Ref = shared_ptr<BaseRef<int32>>;
using Int64Ref = shared_ptr<BaseRef<int64>>;
using UInt8Ref = shared_ptr<BaseRef<uint8>>;
using UInt16Ref = shared_ptr<BaseRef<uint16>>;
using UInt32Ref = shared_ptr<BaseRef<uint32>>;
using UInt64Ref = shared_ptr<BaseRef<uint64>>;
using ByteRef = shared_ptr<BaseRef<BYTE>>;