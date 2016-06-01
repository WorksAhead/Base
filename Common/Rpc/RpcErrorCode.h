// **********************************************************************
//
// Copyright (c) 2003-2016 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************
//
// Ice version 3.6.2
//
// <auto-generated>
//
// Generated from file `RpcErrorCode.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

#ifndef __RpcErrorCode_h__
#define __RpcErrorCode_h__

#include <IceUtil/PushDisableWarnings.h>
#include <Ice/ProxyF.h>
#include <Ice/ObjectF.h>
#include <Ice/Exception.h>
#include <Ice/LocalObject.h>
#include <Ice/StreamHelpers.h>
#include <IceUtil/ScopedArray.h>
#include <IceUtil/Optional.h>
#include <IceUtil/UndefSysMacros.h>

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 306
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 > 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 2
#       error Ice patch level mismatch!
#   endif
#endif

namespace Rpc
{

enum ErrorCode
{
    ec_success = 0,
    ec_username_already_exists = 1,
    ec_username_or_password_incorrect = 2,
    ec_engine_version_is_locked = 3,
    ec_engine_version_already_exists = 4,
    ec_engine_version_does_not_exist = 5,
    ec_engine_version_is_removed = 6,
    ec_file_io_error = 7,
    ec_file_data_error = 8,
    ec_access_denied = 9
};

}

namespace Ice
{
template<>
struct StreamableTraits< ::Rpc::ErrorCode>
{
    static const StreamHelperCategory helper = StreamHelperCategoryEnum;
    static const int minValue = 0;
    static const int maxValue = 9;
    static const int minWireSize = 1;
    static const bool fixedLength = false;
};

}

#include <IceUtil/PopDisableWarnings.h>
#endif
