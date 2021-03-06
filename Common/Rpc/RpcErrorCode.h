//
// Copyright (c) ZeroC, Inc. All rights reserved.
//
//
// Ice version 3.7.2
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

enum class ErrorCode : unsigned char
{
    ec_success = 0,
    ec_username_does_not_exist = 1,
    ec_username_or_password_incorrect = 2,
    ec_engine_version_is_locked = 3,
    ec_engine_version_already_exists = 4,
    ec_engine_version_does_not_exist = 5,
    ec_engine_version_is_removed = 6,
    ec_page_does_not_exist = 7,
    ec_category_does_not_exist = 8,
    ec_file_io_error = 9,
    ec_file_data_error = 10,
    ec_out_of_range = 11,
    ec_access_denied = 12,
    ec_invalid_operation = 13,
    ec_server_busy = 14,
    ec_incomplete_form = 15,
    ec_incomplete_content = 16,
    ec_parent_does_not_exist = 17,
    ec_content_does_not_exist = 18,
    ec_operation_failed = 19,
    ec_incomplete_engine_version = 20,
    ec_extra_does_not_exist = 21,
    ec_incomplete_extra = 22,
    ec_incomplete_client_version = 23,
    ec_client_version_is_locked = 24,
    ec_client_version_already_exists = 25,
    ec_client_version_does_not_exist = 26,
    ec_client_version_is_removed = 27,
    ec_client_version_too_low = 28,
    ec_comment_does_not_exist = 29
};

}

/// \cond STREAM
namespace Ice
{

template<>
struct StreamableTraits< ::Rpc::ErrorCode>
{
    static const StreamHelperCategory helper = StreamHelperCategoryEnum;
    static const int minValue = 0;
    static const int maxValue = 29;
    static const int minWireSize = 1;
    static const bool fixedLength = false;
};

}
/// \endcond

#else // C++98 mapping

namespace Rpc
{

enum ErrorCode
{
    ec_success = 0,
    ec_username_does_not_exist = 1,
    ec_username_or_password_incorrect = 2,
    ec_engine_version_is_locked = 3,
    ec_engine_version_already_exists = 4,
    ec_engine_version_does_not_exist = 5,
    ec_engine_version_is_removed = 6,
    ec_page_does_not_exist = 7,
    ec_category_does_not_exist = 8,
    ec_file_io_error = 9,
    ec_file_data_error = 10,
    ec_out_of_range = 11,
    ec_access_denied = 12,
    ec_invalid_operation = 13,
    ec_server_busy = 14,
    ec_incomplete_form = 15,
    ec_incomplete_content = 16,
    ec_parent_does_not_exist = 17,
    ec_content_does_not_exist = 18,
    ec_operation_failed = 19,
    ec_incomplete_engine_version = 20,
    ec_extra_does_not_exist = 21,
    ec_incomplete_extra = 22,
    ec_incomplete_client_version = 23,
    ec_client_version_is_locked = 24,
    ec_client_version_already_exists = 25,
    ec_client_version_does_not_exist = 26,
    ec_client_version_is_removed = 27,
    ec_client_version_too_low = 28,
    ec_comment_does_not_exist = 29
};

}

/// \cond STREAM
namespace Ice
{

template<>
struct StreamableTraits< ::Rpc::ErrorCode>
{
    static const StreamHelperCategory helper = StreamHelperCategoryEnum;
    static const int minValue = 0;
    static const int maxValue = 29;
    static const int minWireSize = 1;
    static const bool fixedLength = false;
};

}
/// \endcond

#endif

#include <IceUtil/PopDisableWarnings.h>
#endif
