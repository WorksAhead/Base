//
// Copyright (c) ZeroC, Inc. All rights reserved.
//
//
// Ice version 3.7.2
//
// <auto-generated>
//
// Generated from file `RpcTypedefs.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

#ifndef __RpcTypedefs_h__
#define __RpcTypedefs_h__

#include <IceUtil/PushDisableWarnings.h>
#include <Ice/ProxyF.h>
#include <Ice/ObjectF.h>
#include <Ice/ValueF.h>
#include <Ice/Exception.h>
#include <Ice/LocalObject.h>
#include <Ice/StreamHelpers.h>
#include <Ice/Comparable.h>
#include <IceUtil/ScopedArray.h>
#include <Ice/Optional.h>
#include <IceUtil/UndefSysMacros.h>

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 307
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 > 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 2
#       error Ice patch level mismatch!
#   endif
#endif

#ifdef ICE_CPP11_MAPPING // C++11 mapping

namespace Rpc
{

using ByteSeq = ::std::vector<::Ice::Byte>;

using StringSeq = ::std::vector<::std::string>;

}

#else // C++98 mapping

namespace Rpc
{

typedef ::std::vector< ::Ice::Byte> ByteSeq;

typedef ::std::vector< ::std::string> StringSeq;

}

#endif

#include <IceUtil/PopDisableWarnings.h>
#endif
