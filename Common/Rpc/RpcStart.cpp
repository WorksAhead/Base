// **********************************************************************
//
// Copyright (c) 2003-2016 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************
//
// Ice version 3.6.3
//
// <auto-generated>
//
// Generated from file `RpcStart.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

#include <RpcStart.h>
#include <IceUtil/PushDisableWarnings.h>
#include <Ice/LocalException.h>
#include <Ice/ObjectFactory.h>
#include <Ice/Outgoing.h>
#include <Ice/OutgoingAsync.h>
#include <Ice/BasicStream.h>
#include <IceUtil/Iterator.h>
#include <IceUtil/PopDisableWarnings.h>

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 306
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 > 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 3
#       error Ice patch level mismatch!
#   endif
#endif

namespace
{

const ::std::string __Rpc__Start__getServerVersion_name = "getServerVersion";

const ::std::string __Rpc__Start__getClientVersion_name = "getClientVersion";

const ::std::string __Rpc__Start__downloadClient_name = "downloadClient";

const ::std::string __Rpc__Start__signup_name = "signup";

const ::std::string __Rpc__Start__login_name = "login";

const ::std::string __Rpc__Start__resetPassword_name = "resetPassword";

}
::IceProxy::Ice::Object* ::IceProxy::Rpc::upCast(::IceProxy::Rpc::Start* p) { return p; }

void
::IceProxy::Rpc::__read(::IceInternal::BasicStream* __is, ::IceInternal::ProxyHandle< ::IceProxy::Rpc::Start>& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::Rpc::Start;
        v->__copyFrom(proxy);
    }
}

::std::string
IceProxy::Rpc::Start::getServerVersion(const ::Ice::Context* __ctx)
{
    __checkTwowayOnly(__Rpc__Start__getServerVersion_name);
    ::IceInternal::Outgoing __og(this, __Rpc__Start__getServerVersion_name, ::Ice::Normal, __ctx);
    __og.writeEmptyParams();
    if(!__og.invoke())
    {
        try
        {
            __og.throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
            throw __uue;
        }
    }
    ::std::string __ret;
    ::IceInternal::BasicStream* __is = __og.startReadParams();
    __is->read(__ret);
    __og.endReadParams();
    return __ret;
}

::Ice::AsyncResultPtr
IceProxy::Rpc::Start::begin_getServerVersion(const ::Ice::Context* __ctx, const ::IceInternal::CallbackBasePtr& __del, const ::Ice::LocalObjectPtr& __cookie)
{
    __checkAsyncTwowayOnly(__Rpc__Start__getServerVersion_name);
    ::IceInternal::OutgoingAsyncPtr __result = new ::IceInternal::OutgoingAsync(this, __Rpc__Start__getServerVersion_name, __del, __cookie);
    try
    {
        __result->prepare(__Rpc__Start__getServerVersion_name, ::Ice::Normal, __ctx);
        __result->writeEmptyParams();
        __result->invoke();
    }
    catch(const ::Ice::Exception& __ex)
    {
        __result->abort(__ex);
    }
    return __result;
}

#ifdef ICE_CPP11

::Ice::AsyncResultPtr
IceProxy::Rpc::Start::__begin_getServerVersion(const ::Ice::Context* __ctx, const ::IceInternal::Function<void (const ::std::string&)>& __response, const ::IceInternal::Function<void (const ::Ice::Exception&)>& __exception, const ::IceInternal::Function<void (bool)>& __sent)
{
    class Cpp11CB : public ::IceInternal::Cpp11FnCallbackNC
    {
    public:

        Cpp11CB(const ::std::function<void (const ::std::string&)>& responseFunc, const ::std::function<void (const ::Ice::Exception&)>& exceptionFunc, const ::std::function<void (bool)>& sentFunc) :
            ::IceInternal::Cpp11FnCallbackNC(exceptionFunc, sentFunc),
            _response(responseFunc)
        {
            CallbackBase::checkCallback(true, responseFunc || exceptionFunc != nullptr);
        }

        virtual void completed(const ::Ice::AsyncResultPtr& __result) const
        {
            ::Rpc::StartPrx __proxy = ::Rpc::StartPrx::uncheckedCast(__result->getProxy());
            ::std::string __ret;
            try
            {
                __ret = __proxy->end_getServerVersion(__result);
            }
            catch(const ::Ice::Exception& ex)
            {
                Cpp11FnCallbackNC::exception(__result, ex);
                return;
            }
            if(_response != nullptr)
            {
                _response(__ret);
            }
        }
    
    private:
        
        ::std::function<void (const ::std::string&)> _response;
    };
    return begin_getServerVersion(__ctx, new Cpp11CB(__response, __exception, __sent));
}
#endif

::std::string
IceProxy::Rpc::Start::end_getServerVersion(const ::Ice::AsyncResultPtr& __result)
{
    ::Ice::AsyncResult::__check(__result, this, __Rpc__Start__getServerVersion_name);
    ::std::string __ret;
    if(!__result->__wait())
    {
        try
        {
            __result->__throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
        }
    }
    ::IceInternal::BasicStream* __is = __result->__startReadParams();
    __is->read(__ret);
    __result->__endReadParams();
    return __ret;
}

::std::string
IceProxy::Rpc::Start::getClientVersion(const ::Ice::Context* __ctx)
{
    __checkTwowayOnly(__Rpc__Start__getClientVersion_name);
    ::IceInternal::Outgoing __og(this, __Rpc__Start__getClientVersion_name, ::Ice::Normal, __ctx);
    __og.writeEmptyParams();
    if(!__og.invoke())
    {
        try
        {
            __og.throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
            throw __uue;
        }
    }
    ::std::string __ret;
    ::IceInternal::BasicStream* __is = __og.startReadParams();
    __is->read(__ret);
    __og.endReadParams();
    return __ret;
}

::Ice::AsyncResultPtr
IceProxy::Rpc::Start::begin_getClientVersion(const ::Ice::Context* __ctx, const ::IceInternal::CallbackBasePtr& __del, const ::Ice::LocalObjectPtr& __cookie)
{
    __checkAsyncTwowayOnly(__Rpc__Start__getClientVersion_name);
    ::IceInternal::OutgoingAsyncPtr __result = new ::IceInternal::OutgoingAsync(this, __Rpc__Start__getClientVersion_name, __del, __cookie);
    try
    {
        __result->prepare(__Rpc__Start__getClientVersion_name, ::Ice::Normal, __ctx);
        __result->writeEmptyParams();
        __result->invoke();
    }
    catch(const ::Ice::Exception& __ex)
    {
        __result->abort(__ex);
    }
    return __result;
}

#ifdef ICE_CPP11

::Ice::AsyncResultPtr
IceProxy::Rpc::Start::__begin_getClientVersion(const ::Ice::Context* __ctx, const ::IceInternal::Function<void (const ::std::string&)>& __response, const ::IceInternal::Function<void (const ::Ice::Exception&)>& __exception, const ::IceInternal::Function<void (bool)>& __sent)
{
    class Cpp11CB : public ::IceInternal::Cpp11FnCallbackNC
    {
    public:

        Cpp11CB(const ::std::function<void (const ::std::string&)>& responseFunc, const ::std::function<void (const ::Ice::Exception&)>& exceptionFunc, const ::std::function<void (bool)>& sentFunc) :
            ::IceInternal::Cpp11FnCallbackNC(exceptionFunc, sentFunc),
            _response(responseFunc)
        {
            CallbackBase::checkCallback(true, responseFunc || exceptionFunc != nullptr);
        }

        virtual void completed(const ::Ice::AsyncResultPtr& __result) const
        {
            ::Rpc::StartPrx __proxy = ::Rpc::StartPrx::uncheckedCast(__result->getProxy());
            ::std::string __ret;
            try
            {
                __ret = __proxy->end_getClientVersion(__result);
            }
            catch(const ::Ice::Exception& ex)
            {
                Cpp11FnCallbackNC::exception(__result, ex);
                return;
            }
            if(_response != nullptr)
            {
                _response(__ret);
            }
        }
    
    private:
        
        ::std::function<void (const ::std::string&)> _response;
    };
    return begin_getClientVersion(__ctx, new Cpp11CB(__response, __exception, __sent));
}
#endif

::std::string
IceProxy::Rpc::Start::end_getClientVersion(const ::Ice::AsyncResultPtr& __result)
{
    ::Ice::AsyncResult::__check(__result, this, __Rpc__Start__getClientVersion_name);
    ::std::string __ret;
    if(!__result->__wait())
    {
        try
        {
            __result->__throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
        }
    }
    ::IceInternal::BasicStream* __is = __result->__startReadParams();
    __is->read(__ret);
    __result->__endReadParams();
    return __ret;
}

::Rpc::ErrorCode
IceProxy::Rpc::Start::downloadClient(::Rpc::DownloaderPrx& __p_downloader, const ::Ice::Context* __ctx)
{
    __checkTwowayOnly(__Rpc__Start__downloadClient_name);
    ::IceInternal::Outgoing __og(this, __Rpc__Start__downloadClient_name, ::Ice::Normal, __ctx);
    __og.writeEmptyParams();
    if(!__og.invoke())
    {
        try
        {
            __og.throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
            throw __uue;
        }
    }
    ::Rpc::ErrorCode __ret;
    ::IceInternal::BasicStream* __is = __og.startReadParams();
    __is->read(__p_downloader);
    __is->read(__ret);
    __og.endReadParams();
    return __ret;
}

::Ice::AsyncResultPtr
IceProxy::Rpc::Start::begin_downloadClient(const ::Ice::Context* __ctx, const ::IceInternal::CallbackBasePtr& __del, const ::Ice::LocalObjectPtr& __cookie)
{
    __checkAsyncTwowayOnly(__Rpc__Start__downloadClient_name);
    ::IceInternal::OutgoingAsyncPtr __result = new ::IceInternal::OutgoingAsync(this, __Rpc__Start__downloadClient_name, __del, __cookie);
    try
    {
        __result->prepare(__Rpc__Start__downloadClient_name, ::Ice::Normal, __ctx);
        __result->writeEmptyParams();
        __result->invoke();
    }
    catch(const ::Ice::Exception& __ex)
    {
        __result->abort(__ex);
    }
    return __result;
}

#ifdef ICE_CPP11

::Ice::AsyncResultPtr
IceProxy::Rpc::Start::__begin_downloadClient(const ::Ice::Context* __ctx, const ::IceInternal::Function<void (::Rpc::ErrorCode, const ::Rpc::DownloaderPrx&)>& __response, const ::IceInternal::Function<void (const ::Ice::Exception&)>& __exception, const ::IceInternal::Function<void (bool)>& __sent)
{
    class Cpp11CB : public ::IceInternal::Cpp11FnCallbackNC
    {
    public:

        Cpp11CB(const ::std::function<void (::Rpc::ErrorCode, const ::Rpc::DownloaderPrx&)>& responseFunc, const ::std::function<void (const ::Ice::Exception&)>& exceptionFunc, const ::std::function<void (bool)>& sentFunc) :
            ::IceInternal::Cpp11FnCallbackNC(exceptionFunc, sentFunc),
            _response(responseFunc)
        {
            CallbackBase::checkCallback(true, responseFunc || exceptionFunc != nullptr);
        }

        virtual void completed(const ::Ice::AsyncResultPtr& __result) const
        {
            ::Rpc::StartPrx __proxy = ::Rpc::StartPrx::uncheckedCast(__result->getProxy());
            ::Rpc::DownloaderPrx __p_downloader;
            ::Rpc::ErrorCode __ret;
            try
            {
                __ret = __proxy->end_downloadClient(__p_downloader, __result);
            }
            catch(const ::Ice::Exception& ex)
            {
                Cpp11FnCallbackNC::exception(__result, ex);
                return;
            }
            if(_response != nullptr)
            {
                _response(__ret, __p_downloader);
            }
        }
    
    private:
        
        ::std::function<void (::Rpc::ErrorCode, const ::Rpc::DownloaderPrx&)> _response;
    };
    return begin_downloadClient(__ctx, new Cpp11CB(__response, __exception, __sent));
}
#endif

::Rpc::ErrorCode
IceProxy::Rpc::Start::end_downloadClient(::Rpc::DownloaderPrx& __p_downloader, const ::Ice::AsyncResultPtr& __result)
{
    ::Ice::AsyncResult::__check(__result, this, __Rpc__Start__downloadClient_name);
    ::Rpc::ErrorCode __ret;
    if(!__result->__wait())
    {
        try
        {
            __result->__throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
        }
    }
    ::IceInternal::BasicStream* __is = __result->__startReadParams();
    __is->read(__p_downloader);
    __is->read(__ret);
    __result->__endReadParams();
    return __ret;
}

::Rpc::ErrorCode
IceProxy::Rpc::Start::signup(const ::std::string& __p_username, const ::std::string& __p_password, const ::Ice::Context* __ctx)
{
    __checkTwowayOnly(__Rpc__Start__signup_name);
    ::IceInternal::Outgoing __og(this, __Rpc__Start__signup_name, ::Ice::Normal, __ctx);
    try
    {
        ::IceInternal::BasicStream* __os = __og.startWriteParams(::Ice::DefaultFormat);
        __os->write(__p_username);
        __os->write(__p_password);
        __og.endWriteParams();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    if(!__og.invoke())
    {
        try
        {
            __og.throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
            throw __uue;
        }
    }
    ::Rpc::ErrorCode __ret;
    ::IceInternal::BasicStream* __is = __og.startReadParams();
    __is->read(__ret);
    __og.endReadParams();
    return __ret;
}

::Ice::AsyncResultPtr
IceProxy::Rpc::Start::begin_signup(const ::std::string& __p_username, const ::std::string& __p_password, const ::Ice::Context* __ctx, const ::IceInternal::CallbackBasePtr& __del, const ::Ice::LocalObjectPtr& __cookie)
{
    __checkAsyncTwowayOnly(__Rpc__Start__signup_name);
    ::IceInternal::OutgoingAsyncPtr __result = new ::IceInternal::OutgoingAsync(this, __Rpc__Start__signup_name, __del, __cookie);
    try
    {
        __result->prepare(__Rpc__Start__signup_name, ::Ice::Normal, __ctx);
        ::IceInternal::BasicStream* __os = __result->startWriteParams(::Ice::DefaultFormat);
        __os->write(__p_username);
        __os->write(__p_password);
        __result->endWriteParams();
        __result->invoke();
    }
    catch(const ::Ice::Exception& __ex)
    {
        __result->abort(__ex);
    }
    return __result;
}

#ifdef ICE_CPP11

::Ice::AsyncResultPtr
IceProxy::Rpc::Start::__begin_signup(const ::std::string& __p_username, const ::std::string& __p_password, const ::Ice::Context* __ctx, const ::IceInternal::Function<void (::Rpc::ErrorCode)>& __response, const ::IceInternal::Function<void (const ::Ice::Exception&)>& __exception, const ::IceInternal::Function<void (bool)>& __sent)
{
    class Cpp11CB : public ::IceInternal::Cpp11FnCallbackNC
    {
    public:

        Cpp11CB(const ::std::function<void (::Rpc::ErrorCode)>& responseFunc, const ::std::function<void (const ::Ice::Exception&)>& exceptionFunc, const ::std::function<void (bool)>& sentFunc) :
            ::IceInternal::Cpp11FnCallbackNC(exceptionFunc, sentFunc),
            _response(responseFunc)
        {
            CallbackBase::checkCallback(true, responseFunc || exceptionFunc != nullptr);
        }

        virtual void completed(const ::Ice::AsyncResultPtr& __result) const
        {
            ::Rpc::StartPrx __proxy = ::Rpc::StartPrx::uncheckedCast(__result->getProxy());
            ::Rpc::ErrorCode __ret;
            try
            {
                __ret = __proxy->end_signup(__result);
            }
            catch(const ::Ice::Exception& ex)
            {
                Cpp11FnCallbackNC::exception(__result, ex);
                return;
            }
            if(_response != nullptr)
            {
                _response(__ret);
            }
        }
    
    private:
        
        ::std::function<void (::Rpc::ErrorCode)> _response;
    };
    return begin_signup(__p_username, __p_password, __ctx, new Cpp11CB(__response, __exception, __sent));
}
#endif

::Rpc::ErrorCode
IceProxy::Rpc::Start::end_signup(const ::Ice::AsyncResultPtr& __result)
{
    ::Ice::AsyncResult::__check(__result, this, __Rpc__Start__signup_name);
    ::Rpc::ErrorCode __ret;
    if(!__result->__wait())
    {
        try
        {
            __result->__throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
        }
    }
    ::IceInternal::BasicStream* __is = __result->__startReadParams();
    __is->read(__ret);
    __result->__endReadParams();
    return __ret;
}

::Rpc::ErrorCode
IceProxy::Rpc::Start::login(const ::std::string& __p_username, const ::std::string& __p_password, ::Rpc::SessionPrx& __p_session, const ::Ice::Context* __ctx)
{
    __checkTwowayOnly(__Rpc__Start__login_name);
    ::IceInternal::Outgoing __og(this, __Rpc__Start__login_name, ::Ice::Normal, __ctx);
    try
    {
        ::IceInternal::BasicStream* __os = __og.startWriteParams(::Ice::DefaultFormat);
        __os->write(__p_username);
        __os->write(__p_password);
        __og.endWriteParams();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    if(!__og.invoke())
    {
        try
        {
            __og.throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
            throw __uue;
        }
    }
    ::Rpc::ErrorCode __ret;
    ::IceInternal::BasicStream* __is = __og.startReadParams();
    __is->read(__p_session);
    __is->read(__ret);
    __og.endReadParams();
    return __ret;
}

::Ice::AsyncResultPtr
IceProxy::Rpc::Start::begin_login(const ::std::string& __p_username, const ::std::string& __p_password, const ::Ice::Context* __ctx, const ::IceInternal::CallbackBasePtr& __del, const ::Ice::LocalObjectPtr& __cookie)
{
    __checkAsyncTwowayOnly(__Rpc__Start__login_name);
    ::IceInternal::OutgoingAsyncPtr __result = new ::IceInternal::OutgoingAsync(this, __Rpc__Start__login_name, __del, __cookie);
    try
    {
        __result->prepare(__Rpc__Start__login_name, ::Ice::Normal, __ctx);
        ::IceInternal::BasicStream* __os = __result->startWriteParams(::Ice::DefaultFormat);
        __os->write(__p_username);
        __os->write(__p_password);
        __result->endWriteParams();
        __result->invoke();
    }
    catch(const ::Ice::Exception& __ex)
    {
        __result->abort(__ex);
    }
    return __result;
}

#ifdef ICE_CPP11

::Ice::AsyncResultPtr
IceProxy::Rpc::Start::__begin_login(const ::std::string& __p_username, const ::std::string& __p_password, const ::Ice::Context* __ctx, const ::IceInternal::Function<void (::Rpc::ErrorCode, const ::Rpc::SessionPrx&)>& __response, const ::IceInternal::Function<void (const ::Ice::Exception&)>& __exception, const ::IceInternal::Function<void (bool)>& __sent)
{
    class Cpp11CB : public ::IceInternal::Cpp11FnCallbackNC
    {
    public:

        Cpp11CB(const ::std::function<void (::Rpc::ErrorCode, const ::Rpc::SessionPrx&)>& responseFunc, const ::std::function<void (const ::Ice::Exception&)>& exceptionFunc, const ::std::function<void (bool)>& sentFunc) :
            ::IceInternal::Cpp11FnCallbackNC(exceptionFunc, sentFunc),
            _response(responseFunc)
        {
            CallbackBase::checkCallback(true, responseFunc || exceptionFunc != nullptr);
        }

        virtual void completed(const ::Ice::AsyncResultPtr& __result) const
        {
            ::Rpc::StartPrx __proxy = ::Rpc::StartPrx::uncheckedCast(__result->getProxy());
            ::Rpc::SessionPrx __p_session;
            ::Rpc::ErrorCode __ret;
            try
            {
                __ret = __proxy->end_login(__p_session, __result);
            }
            catch(const ::Ice::Exception& ex)
            {
                Cpp11FnCallbackNC::exception(__result, ex);
                return;
            }
            if(_response != nullptr)
            {
                _response(__ret, __p_session);
            }
        }
    
    private:
        
        ::std::function<void (::Rpc::ErrorCode, const ::Rpc::SessionPrx&)> _response;
    };
    return begin_login(__p_username, __p_password, __ctx, new Cpp11CB(__response, __exception, __sent));
}
#endif

::Rpc::ErrorCode
IceProxy::Rpc::Start::end_login(::Rpc::SessionPrx& __p_session, const ::Ice::AsyncResultPtr& __result)
{
    ::Ice::AsyncResult::__check(__result, this, __Rpc__Start__login_name);
    ::Rpc::ErrorCode __ret;
    if(!__result->__wait())
    {
        try
        {
            __result->__throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
        }
    }
    ::IceInternal::BasicStream* __is = __result->__startReadParams();
    __is->read(__p_session);
    __is->read(__ret);
    __result->__endReadParams();
    return __ret;
}

::Rpc::ErrorCode
IceProxy::Rpc::Start::resetPassword(const ::std::string& __p_username, const ::std::string& __p_oldPassword, const ::std::string& __p_newPassword, const ::Ice::Context* __ctx)
{
    __checkTwowayOnly(__Rpc__Start__resetPassword_name);
    ::IceInternal::Outgoing __og(this, __Rpc__Start__resetPassword_name, ::Ice::Normal, __ctx);
    try
    {
        ::IceInternal::BasicStream* __os = __og.startWriteParams(::Ice::DefaultFormat);
        __os->write(__p_username);
        __os->write(__p_oldPassword);
        __os->write(__p_newPassword);
        __og.endWriteParams();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    if(!__og.invoke())
    {
        try
        {
            __og.throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
            throw __uue;
        }
    }
    ::Rpc::ErrorCode __ret;
    ::IceInternal::BasicStream* __is = __og.startReadParams();
    __is->read(__ret);
    __og.endReadParams();
    return __ret;
}

::Ice::AsyncResultPtr
IceProxy::Rpc::Start::begin_resetPassword(const ::std::string& __p_username, const ::std::string& __p_oldPassword, const ::std::string& __p_newPassword, const ::Ice::Context* __ctx, const ::IceInternal::CallbackBasePtr& __del, const ::Ice::LocalObjectPtr& __cookie)
{
    __checkAsyncTwowayOnly(__Rpc__Start__resetPassword_name);
    ::IceInternal::OutgoingAsyncPtr __result = new ::IceInternal::OutgoingAsync(this, __Rpc__Start__resetPassword_name, __del, __cookie);
    try
    {
        __result->prepare(__Rpc__Start__resetPassword_name, ::Ice::Normal, __ctx);
        ::IceInternal::BasicStream* __os = __result->startWriteParams(::Ice::DefaultFormat);
        __os->write(__p_username);
        __os->write(__p_oldPassword);
        __os->write(__p_newPassword);
        __result->endWriteParams();
        __result->invoke();
    }
    catch(const ::Ice::Exception& __ex)
    {
        __result->abort(__ex);
    }
    return __result;
}

#ifdef ICE_CPP11

::Ice::AsyncResultPtr
IceProxy::Rpc::Start::__begin_resetPassword(const ::std::string& __p_username, const ::std::string& __p_oldPassword, const ::std::string& __p_newPassword, const ::Ice::Context* __ctx, const ::IceInternal::Function<void (::Rpc::ErrorCode)>& __response, const ::IceInternal::Function<void (const ::Ice::Exception&)>& __exception, const ::IceInternal::Function<void (bool)>& __sent)
{
    class Cpp11CB : public ::IceInternal::Cpp11FnCallbackNC
    {
    public:

        Cpp11CB(const ::std::function<void (::Rpc::ErrorCode)>& responseFunc, const ::std::function<void (const ::Ice::Exception&)>& exceptionFunc, const ::std::function<void (bool)>& sentFunc) :
            ::IceInternal::Cpp11FnCallbackNC(exceptionFunc, sentFunc),
            _response(responseFunc)
        {
            CallbackBase::checkCallback(true, responseFunc || exceptionFunc != nullptr);
        }

        virtual void completed(const ::Ice::AsyncResultPtr& __result) const
        {
            ::Rpc::StartPrx __proxy = ::Rpc::StartPrx::uncheckedCast(__result->getProxy());
            ::Rpc::ErrorCode __ret;
            try
            {
                __ret = __proxy->end_resetPassword(__result);
            }
            catch(const ::Ice::Exception& ex)
            {
                Cpp11FnCallbackNC::exception(__result, ex);
                return;
            }
            if(_response != nullptr)
            {
                _response(__ret);
            }
        }
    
    private:
        
        ::std::function<void (::Rpc::ErrorCode)> _response;
    };
    return begin_resetPassword(__p_username, __p_oldPassword, __p_newPassword, __ctx, new Cpp11CB(__response, __exception, __sent));
}
#endif

::Rpc::ErrorCode
IceProxy::Rpc::Start::end_resetPassword(const ::Ice::AsyncResultPtr& __result)
{
    ::Ice::AsyncResult::__check(__result, this, __Rpc__Start__resetPassword_name);
    ::Rpc::ErrorCode __ret;
    if(!__result->__wait())
    {
        try
        {
            __result->__throwUserException();
        }
        catch(const ::Ice::UserException& __ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
        }
    }
    ::IceInternal::BasicStream* __is = __result->__startReadParams();
    __is->read(__ret);
    __result->__endReadParams();
    return __ret;
}

const ::std::string&
IceProxy::Rpc::Start::ice_staticId()
{
    return ::Rpc::Start::ice_staticId();
}

::IceProxy::Ice::Object*
IceProxy::Rpc::Start::__newInstance() const
{
    return new Start;
}

::Ice::Object* Rpc::upCast(::Rpc::Start* p) { return p; }

namespace
{
const ::std::string __Rpc__Start_ids[2] =
{
    "::Ice::Object",
    "::Rpc::Start"
};

}

bool
Rpc::Start::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__Rpc__Start_ids, __Rpc__Start_ids + 2, _s);
}

::std::vector< ::std::string>
Rpc::Start::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__Rpc__Start_ids[0], &__Rpc__Start_ids[2]);
}

const ::std::string&
Rpc::Start::ice_id(const ::Ice::Current&) const
{
    return __Rpc__Start_ids[1];
}

const ::std::string&
Rpc::Start::ice_staticId()
{
#ifdef ICE_HAS_THREAD_SAFE_LOCAL_STATIC
    static const ::std::string typeId = "::Rpc::Start";
    return typeId;
#else
    return __Rpc__Start_ids[1];
#endif
}

::Ice::DispatchStatus
Rpc::Start::___getServerVersion(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.readEmptyParams();
    ::std::string __ret = getServerVersion(__current);
    ::IceInternal::BasicStream* __os = __inS.__startWriteParams(::Ice::DefaultFormat);
    __os->write(__ret);
    __inS.__endWriteParams(true);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
Rpc::Start::___getClientVersion(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.readEmptyParams();
    ::std::string __ret = getClientVersion(__current);
    ::IceInternal::BasicStream* __os = __inS.__startWriteParams(::Ice::DefaultFormat);
    __os->write(__ret);
    __inS.__endWriteParams(true);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
Rpc::Start::___downloadClient(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.readEmptyParams();
    ::Rpc::DownloaderPrx __p_downloader;
    ::Rpc::ErrorCode __ret = downloadClient(__p_downloader, __current);
    ::IceInternal::BasicStream* __os = __inS.__startWriteParams(::Ice::DefaultFormat);
    __os->write(__p_downloader);
    __os->write(__ret);
    __inS.__endWriteParams(true);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
Rpc::Start::___signup(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.startReadParams();
    ::std::string __p_username;
    ::std::string __p_password;
    __is->read(__p_username);
    __is->read(__p_password);
    __inS.endReadParams();
    ::Rpc::ErrorCode __ret = signup(__p_username, __p_password, __current);
    ::IceInternal::BasicStream* __os = __inS.__startWriteParams(::Ice::DefaultFormat);
    __os->write(__ret);
    __inS.__endWriteParams(true);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
Rpc::Start::___login(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.startReadParams();
    ::std::string __p_username;
    ::std::string __p_password;
    __is->read(__p_username);
    __is->read(__p_password);
    __inS.endReadParams();
    ::Rpc::SessionPrx __p_session;
    ::Rpc::ErrorCode __ret = login(__p_username, __p_password, __p_session, __current);
    ::IceInternal::BasicStream* __os = __inS.__startWriteParams(::Ice::DefaultFormat);
    __os->write(__p_session);
    __os->write(__ret);
    __inS.__endWriteParams(true);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
Rpc::Start::___resetPassword(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.startReadParams();
    ::std::string __p_username;
    ::std::string __p_oldPassword;
    ::std::string __p_newPassword;
    __is->read(__p_username);
    __is->read(__p_oldPassword);
    __is->read(__p_newPassword);
    __inS.endReadParams();
    ::Rpc::ErrorCode __ret = resetPassword(__p_username, __p_oldPassword, __p_newPassword, __current);
    ::IceInternal::BasicStream* __os = __inS.__startWriteParams(::Ice::DefaultFormat);
    __os->write(__ret);
    __inS.__endWriteParams(true);
    return ::Ice::DispatchOK;
}

namespace
{
const ::std::string __Rpc__Start_all[] =
{
    "downloadClient",
    "getClientVersion",
    "getServerVersion",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "login",
    "resetPassword",
    "signup"
};

}

::Ice::DispatchStatus
Rpc::Start::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< const ::std::string*, const ::std::string*> r = ::std::equal_range(__Rpc__Start_all, __Rpc__Start_all + 10, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - __Rpc__Start_all)
    {
        case 0:
        {
            return ___downloadClient(in, current);
        }
        case 1:
        {
            return ___getClientVersion(in, current);
        }
        case 2:
        {
            return ___getServerVersion(in, current);
        }
        case 3:
        {
            return ___ice_id(in, current);
        }
        case 4:
        {
            return ___ice_ids(in, current);
        }
        case 5:
        {
            return ___ice_isA(in, current);
        }
        case 6:
        {
            return ___ice_ping(in, current);
        }
        case 7:
        {
            return ___login(in, current);
        }
        case 8:
        {
            return ___resetPassword(in, current);
        }
        case 9:
        {
            return ___signup(in, current);
        }
    }

    assert(false);
    throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
}

void
Rpc::Start::__writeImpl(::IceInternal::BasicStream* __os) const
{
    __os->startWriteSlice(ice_staticId(), -1, true);
    __os->endWriteSlice();
}

void
Rpc::Start::__readImpl(::IceInternal::BasicStream* __is)
{
    __is->startReadSlice();
    __is->endReadSlice();
}

void 
Rpc::__patch(StartPtr& handle, const ::Ice::ObjectPtr& v)
{
    handle = ::Rpc::StartPtr::dynamicCast(v);
    if(v && !handle)
    {
        IceInternal::Ex::throwUOE(::Rpc::Start::ice_staticId(), v);
    }
}
