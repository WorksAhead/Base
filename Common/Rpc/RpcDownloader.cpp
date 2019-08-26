//
// Copyright (c) ZeroC, Inc. All rights reserved.
//
//
// Ice version 3.7.2
//
// <auto-generated>
//
// Generated from file `RpcDownloader.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

#include <RpcDownloader.h>
#include <IceUtil/PushDisableWarnings.h>
#include <Ice/LocalException.h>
#include <Ice/ValueFactory.h>
#include <Ice/OutgoingAsync.h>
#include <Ice/InputStream.h>
#include <Ice/OutputStream.h>
#include <IceUtil/PopDisableWarnings.h>

#if defined(_MSC_VER)
#   pragma warning(disable:4458) // declaration of ... hides class member
#elif defined(__clang__)
#   pragma clang diagnostic ignored "-Wshadow"
#elif defined(__GNUC__)
#   pragma GCC diagnostic ignored "-Wshadow"
#endif

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

namespace
{

const ::std::string iceC_Rpc_Downloader_ids[3] =
{
    "::Ice::Object",
    "::Rpc::Downloader",
    "::Rpc::ManagedObject"
};
const ::std::string iceC_Rpc_Downloader_ops[] =
{
    "cancel",
    "destroy",
    "finish",
    "getSize",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "read"
};
const ::std::string iceC_Rpc_Downloader_getSize_name = "getSize";
const ::std::string iceC_Rpc_Downloader_read_name = "read";
const ::std::string iceC_Rpc_Downloader_finish_name = "finish";
const ::std::string iceC_Rpc_Downloader_cancel_name = "cancel";

}

bool
Rpc::Downloader::ice_isA(::std::string s, const ::Ice::Current&) const
{
    return ::std::binary_search(iceC_Rpc_Downloader_ids, iceC_Rpc_Downloader_ids + 3, s);
}

::std::vector<::std::string>
Rpc::Downloader::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector<::std::string>(&iceC_Rpc_Downloader_ids[0], &iceC_Rpc_Downloader_ids[3]);
}

::std::string
Rpc::Downloader::ice_id(const ::Ice::Current&) const
{
    return ice_staticId();
}

const ::std::string&
Rpc::Downloader::ice_staticId()
{
    static const ::std::string typeId = "::Rpc::Downloader";
    return typeId;
}

/// \cond INTERNAL
bool
Rpc::Downloader::_iceD_getSize(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::OperationMode::Normal, current.mode);
    inS.readEmptyParams();
    long long int iceP_size;
    ErrorCode ret = this->getSize(iceP_size, current);
    auto ostr = inS.startWriteParams();
    ostr->writeAll(iceP_size, ret);
    inS.endWriteParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
Rpc::Downloader::_iceD_read(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::OperationMode::Normal, current.mode);
    auto istr = inS.startReadParams();
    long long int iceP_offset;
    int iceP_size;
    istr->readAll(iceP_offset, iceP_size);
    inS.endReadParams();
    ByteSeq iceP_bytes;
    ErrorCode ret = this->read(iceP_offset, iceP_size, iceP_bytes, current);
    auto ostr = inS.startWriteParams();
    ostr->writeAll(iceP_bytes, ret);
    inS.endWriteParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
Rpc::Downloader::_iceD_finish(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::OperationMode::Normal, current.mode);
    inS.readEmptyParams();
    this->finish(current);
    inS.writeEmptyParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
Rpc::Downloader::_iceD_cancel(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::OperationMode::Normal, current.mode);
    inS.readEmptyParams();
    this->cancel(current);
    inS.writeEmptyParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
Rpc::Downloader::_iceDispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair<const ::std::string*, const ::std::string*> r = ::std::equal_range(iceC_Rpc_Downloader_ops, iceC_Rpc_Downloader_ops + 9, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - iceC_Rpc_Downloader_ops)
    {
        case 0:
        {
            return _iceD_cancel(in, current);
        }
        case 1:
        {
            return _iceD_destroy(in, current);
        }
        case 2:
        {
            return _iceD_finish(in, current);
        }
        case 3:
        {
            return _iceD_getSize(in, current);
        }
        case 4:
        {
            return _iceD_ice_id(in, current);
        }
        case 5:
        {
            return _iceD_ice_ids(in, current);
        }
        case 6:
        {
            return _iceD_ice_isA(in, current);
        }
        case 7:
        {
            return _iceD_ice_ping(in, current);
        }
        case 8:
        {
            return _iceD_read(in, current);
        }
        default:
        {
            assert(false);
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
        }
    }
}
/// \endcond

/// \cond INTERNAL
void
Rpc::DownloaderPrx::_iceI_getSize(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<Downloader::GetSizeResult>>& outAsync, const ::Ice::Context& context)
{
    _checkTwowayOnly(iceC_Rpc_Downloader_getSize_name);
    outAsync->invoke(iceC_Rpc_Downloader_getSize_name, ::Ice::OperationMode::Normal, ::Ice::FormatType::DefaultFormat, context,
        nullptr,
        nullptr,
        [](::Ice::InputStream* istr)
        {
            Downloader::GetSizeResult v;
            istr->readAll(v.size, v.returnValue);
            return v;
        });
}
/// \endcond

/// \cond INTERNAL
void
Rpc::DownloaderPrx::_iceI_read(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<Downloader::ReadResult>>& outAsync, long long int iceP_offset, int iceP_size, const ::Ice::Context& context)
{
    _checkTwowayOnly(iceC_Rpc_Downloader_read_name);
    outAsync->invoke(iceC_Rpc_Downloader_read_name, ::Ice::OperationMode::Normal, ::Ice::FormatType::DefaultFormat, context,
        [&](::Ice::OutputStream* ostr)
        {
            ostr->writeAll(iceP_offset, iceP_size);
        },
        nullptr,
        [](::Ice::InputStream* istr)
        {
            Downloader::ReadResult v;
            istr->readAll(v.bytes, v.returnValue);
            return v;
        });
}
/// \endcond

/// \cond INTERNAL
void
Rpc::DownloaderPrx::_iceI_finish(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<void>>& outAsync, const ::Ice::Context& context)
{
    outAsync->invoke(iceC_Rpc_Downloader_finish_name, ::Ice::OperationMode::Normal, ::Ice::FormatType::DefaultFormat, context,
        nullptr,
        nullptr);
}
/// \endcond

/// \cond INTERNAL
void
Rpc::DownloaderPrx::_iceI_cancel(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<void>>& outAsync, const ::Ice::Context& context)
{
    outAsync->invoke(iceC_Rpc_Downloader_cancel_name, ::Ice::OperationMode::Normal, ::Ice::FormatType::DefaultFormat, context,
        nullptr,
        nullptr);
}
/// \endcond

/// \cond INTERNAL
::std::shared_ptr<::Ice::ObjectPrx>
Rpc::DownloaderPrx::_newInstance() const
{
    return ::IceInternal::createProxy<DownloaderPrx>();
}
/// \endcond

const ::std::string&
Rpc::DownloaderPrx::ice_staticId()
{
    return Downloader::ice_staticId();
}

#else // C++98 mapping

namespace
{

const ::std::string iceC_Rpc_Downloader_getSize_name = "getSize";

const ::std::string iceC_Rpc_Downloader_read_name = "read";

const ::std::string iceC_Rpc_Downloader_finish_name = "finish";

const ::std::string iceC_Rpc_Downloader_cancel_name = "cancel";

}

/// \cond INTERNAL
::IceProxy::Ice::Object* ::IceProxy::Rpc::upCast(Downloader* p) { return p; }

void
::IceProxy::Rpc::_readProxy(::Ice::InputStream* istr, ::IceInternal::ProxyHandle< Downloader>& v)
{
    ::Ice::ObjectPrx proxy;
    istr->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new Downloader;
        v->_copyFrom(proxy);
    }
}
/// \endcond

::Ice::AsyncResultPtr
IceProxy::Rpc::Downloader::_iceI_begin_getSize(const ::Ice::Context& context, const ::IceInternal::CallbackBasePtr& del, const ::Ice::LocalObjectPtr& cookie, bool sync)
{
    _checkTwowayOnly(iceC_Rpc_Downloader_getSize_name, sync);
    ::IceInternal::OutgoingAsyncPtr result = new ::IceInternal::CallbackOutgoing(this, iceC_Rpc_Downloader_getSize_name, del, cookie, sync);
    try
    {
        result->prepare(iceC_Rpc_Downloader_getSize_name, ::Ice::Normal, context);
        result->writeEmptyParams();
        result->invoke(iceC_Rpc_Downloader_getSize_name);
    }
    catch(const ::Ice::Exception& ex)
    {
        result->abort(ex);
    }
    return result;
}

::Rpc::ErrorCode
IceProxy::Rpc::Downloader::end_getSize(::Ice::Long& iceP_size, const ::Ice::AsyncResultPtr& result)
{
    ::Ice::AsyncResult::_check(result, this, iceC_Rpc_Downloader_getSize_name);
    ::Rpc::ErrorCode ret;
    if(!result->_waitForResponse())
    {
        try
        {
            result->_throwUserException();
        }
        catch(const ::Ice::UserException& ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, ex.ice_id());
        }
    }
    ::Ice::InputStream* istr = result->_startReadParams();
    istr->read(iceP_size);
    istr->read(ret);
    result->_endReadParams();
    return ret;
}

void IceProxy::Rpc::Downloader::_iceI_end_getSize(::Ice::Long& iceP_size, ::Rpc::ErrorCode& ret, const ::Ice::AsyncResultPtr& result)
{
    ::Ice::AsyncResult::_check(result, this, iceC_Rpc_Downloader_getSize_name);
    if(!result->_waitForResponse())
    {
        try
        {
            result->_throwUserException();
        }
        catch(const ::Ice::UserException& ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, ex.ice_id());
        }
    }
    ::Ice::InputStream* istr = result->_startReadParams();
    istr->read(iceP_size);
    istr->read(ret);
    result->_endReadParams();
}

::Ice::AsyncResultPtr
IceProxy::Rpc::Downloader::_iceI_begin_read(::Ice::Long iceP_offset, ::Ice::Int iceP_size, const ::Ice::Context& context, const ::IceInternal::CallbackBasePtr& del, const ::Ice::LocalObjectPtr& cookie, bool sync)
{
    _checkTwowayOnly(iceC_Rpc_Downloader_read_name, sync);
    ::IceInternal::OutgoingAsyncPtr result = new ::IceInternal::CallbackOutgoing(this, iceC_Rpc_Downloader_read_name, del, cookie, sync);
    try
    {
        result->prepare(iceC_Rpc_Downloader_read_name, ::Ice::Normal, context);
        ::Ice::OutputStream* ostr = result->startWriteParams(::Ice::DefaultFormat);
        ostr->write(iceP_offset);
        ostr->write(iceP_size);
        result->endWriteParams();
        result->invoke(iceC_Rpc_Downloader_read_name);
    }
    catch(const ::Ice::Exception& ex)
    {
        result->abort(ex);
    }
    return result;
}

::Rpc::ErrorCode
IceProxy::Rpc::Downloader::end_read(::Rpc::ByteSeq& iceP_bytes, const ::Ice::AsyncResultPtr& result)
{
    ::Ice::AsyncResult::_check(result, this, iceC_Rpc_Downloader_read_name);
    ::Rpc::ErrorCode ret;
    if(!result->_waitForResponse())
    {
        try
        {
            result->_throwUserException();
        }
        catch(const ::Ice::UserException& ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, ex.ice_id());
        }
    }
    ::Ice::InputStream* istr = result->_startReadParams();
    istr->read(iceP_bytes);
    istr->read(ret);
    result->_endReadParams();
    return ret;
}

void IceProxy::Rpc::Downloader::_iceI_end_read(::Rpc::ByteSeq& iceP_bytes, ::Rpc::ErrorCode& ret, const ::Ice::AsyncResultPtr& result)
{
    ::Ice::AsyncResult::_check(result, this, iceC_Rpc_Downloader_read_name);
    if(!result->_waitForResponse())
    {
        try
        {
            result->_throwUserException();
        }
        catch(const ::Ice::UserException& ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, ex.ice_id());
        }
    }
    ::Ice::InputStream* istr = result->_startReadParams();
    istr->read(iceP_bytes);
    istr->read(ret);
    result->_endReadParams();
}

::Ice::AsyncResultPtr
IceProxy::Rpc::Downloader::_iceI_begin_finish(const ::Ice::Context& context, const ::IceInternal::CallbackBasePtr& del, const ::Ice::LocalObjectPtr& cookie, bool sync)
{
    ::IceInternal::OutgoingAsyncPtr result = new ::IceInternal::CallbackOutgoing(this, iceC_Rpc_Downloader_finish_name, del, cookie, sync);
    try
    {
        result->prepare(iceC_Rpc_Downloader_finish_name, ::Ice::Normal, context);
        result->writeEmptyParams();
        result->invoke(iceC_Rpc_Downloader_finish_name);
    }
    catch(const ::Ice::Exception& ex)
    {
        result->abort(ex);
    }
    return result;
}

void
IceProxy::Rpc::Downloader::end_finish(const ::Ice::AsyncResultPtr& result)
{
    _end(result, iceC_Rpc_Downloader_finish_name);
}

::Ice::AsyncResultPtr
IceProxy::Rpc::Downloader::_iceI_begin_cancel(const ::Ice::Context& context, const ::IceInternal::CallbackBasePtr& del, const ::Ice::LocalObjectPtr& cookie, bool sync)
{
    ::IceInternal::OutgoingAsyncPtr result = new ::IceInternal::CallbackOutgoing(this, iceC_Rpc_Downloader_cancel_name, del, cookie, sync);
    try
    {
        result->prepare(iceC_Rpc_Downloader_cancel_name, ::Ice::Normal, context);
        result->writeEmptyParams();
        result->invoke(iceC_Rpc_Downloader_cancel_name);
    }
    catch(const ::Ice::Exception& ex)
    {
        result->abort(ex);
    }
    return result;
}

void
IceProxy::Rpc::Downloader::end_cancel(const ::Ice::AsyncResultPtr& result)
{
    _end(result, iceC_Rpc_Downloader_cancel_name);
}

/// \cond INTERNAL
::IceProxy::Ice::Object*
IceProxy::Rpc::Downloader::_newInstance() const
{
    return new Downloader;
}
/// \endcond

const ::std::string&
IceProxy::Rpc::Downloader::ice_staticId()
{
    return ::Rpc::Downloader::ice_staticId();
}

Rpc::Downloader::~Downloader()
{
}

/// \cond INTERNAL
::Ice::Object* Rpc::upCast(Downloader* p) { return p; }

/// \endcond

namespace
{
const ::std::string iceC_Rpc_Downloader_ids[3] =
{
    "::Ice::Object",
    "::Rpc::Downloader",
    "::Rpc::ManagedObject"
};

}

bool
Rpc::Downloader::ice_isA(const ::std::string& s, const ::Ice::Current&) const
{
    return ::std::binary_search(iceC_Rpc_Downloader_ids, iceC_Rpc_Downloader_ids + 3, s);
}

::std::vector< ::std::string>
Rpc::Downloader::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&iceC_Rpc_Downloader_ids[0], &iceC_Rpc_Downloader_ids[3]);
}

const ::std::string&
Rpc::Downloader::ice_id(const ::Ice::Current&) const
{
    return ice_staticId();
}

const ::std::string&
Rpc::Downloader::ice_staticId()
{
#ifdef ICE_HAS_THREAD_SAFE_LOCAL_STATIC
    static const ::std::string typeId = "::Rpc::Downloader";
    return typeId;
#else
    return iceC_Rpc_Downloader_ids[1];
#endif
}

/// \cond INTERNAL
bool
Rpc::Downloader::_iceD_getSize(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::Normal, current.mode);
    inS.readEmptyParams();
    ::Ice::Long iceP_size;
    ErrorCode ret = this->getSize(iceP_size, current);
    ::Ice::OutputStream* ostr = inS.startWriteParams();
    ostr->write(iceP_size);
    ostr->write(ret);
    inS.endWriteParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
Rpc::Downloader::_iceD_read(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::Normal, current.mode);
    ::Ice::InputStream* istr = inS.startReadParams();
    ::Ice::Long iceP_offset;
    ::Ice::Int iceP_size;
    istr->read(iceP_offset);
    istr->read(iceP_size);
    inS.endReadParams();
    ByteSeq iceP_bytes;
    ErrorCode ret = this->read(iceP_offset, iceP_size, iceP_bytes, current);
    ::Ice::OutputStream* ostr = inS.startWriteParams();
    ostr->write(iceP_bytes);
    ostr->write(ret);
    inS.endWriteParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
Rpc::Downloader::_iceD_finish(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::Normal, current.mode);
    inS.readEmptyParams();
    this->finish(current);
    inS.writeEmptyParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
Rpc::Downloader::_iceD_cancel(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::Normal, current.mode);
    inS.readEmptyParams();
    this->cancel(current);
    inS.writeEmptyParams();
    return true;
}
/// \endcond

namespace
{
const ::std::string iceC_Rpc_Downloader_all[] =
{
    "cancel",
    "destroy",
    "finish",
    "getSize",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "read"
};

}

/// \cond INTERNAL
bool
Rpc::Downloader::_iceDispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair<const ::std::string*, const ::std::string*> r = ::std::equal_range(iceC_Rpc_Downloader_all, iceC_Rpc_Downloader_all + 9, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - iceC_Rpc_Downloader_all)
    {
        case 0:
        {
            return _iceD_cancel(in, current);
        }
        case 1:
        {
            return _iceD_destroy(in, current);
        }
        case 2:
        {
            return _iceD_finish(in, current);
        }
        case 3:
        {
            return _iceD_getSize(in, current);
        }
        case 4:
        {
            return _iceD_ice_id(in, current);
        }
        case 5:
        {
            return _iceD_ice_ids(in, current);
        }
        case 6:
        {
            return _iceD_ice_isA(in, current);
        }
        case 7:
        {
            return _iceD_ice_ping(in, current);
        }
        case 8:
        {
            return _iceD_read(in, current);
        }
        default:
        {
            assert(false);
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
        }
    }
}
/// \endcond

/// \cond STREAM
void
Rpc::Downloader::_iceWriteImpl(::Ice::OutputStream* ostr) const
{
    ostr->startSlice(ice_staticId(), -1, true);
    ::Ice::StreamWriter< Downloader, ::Ice::OutputStream>::write(ostr, *this);
    ostr->endSlice();
}

void
Rpc::Downloader::_iceReadImpl(::Ice::InputStream* istr)
{
    istr->startSlice();
    ::Ice::StreamReader< Downloader, ::Ice::InputStream>::read(istr, *this);
    istr->endSlice();
}
/// \endcond

/// \cond INTERNAL
void
Rpc::_icePatchObjectPtr(DownloaderPtr& handle, const ::Ice::ObjectPtr& v)
{
    handle = DownloaderPtr::dynamicCast(v);
    if(v && !handle)
    {
        IceInternal::Ex::throwUOE(Downloader::ice_staticId(), v);
    }
}
/// \endcond

namespace Ice
{
}

#endif
