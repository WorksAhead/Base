//
// Copyright (c) ZeroC, Inc. All rights reserved.
//
//
// Ice version 3.7.2
//
// <auto-generated>
//
// Generated from file `RpcUploader.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

#ifndef __RpcUploader_h__
#define __RpcUploader_h__

#include <IceUtil/PushDisableWarnings.h>
#include <Ice/ProxyF.h>
#include <Ice/ObjectF.h>
#include <Ice/ValueF.h>
#include <Ice/Exception.h>
#include <Ice/LocalObject.h>
#include <Ice/StreamHelpers.h>
#include <Ice/Comparable.h>
#include <Ice/Proxy.h>
#include <Ice/Object.h>
#include <Ice/GCObject.h>
#include <Ice/Value.h>
#include <Ice/Incoming.h>
#include <Ice/FactoryTableInit.h>
#include <IceUtil/ScopedArray.h>
#include <Ice/Optional.h>
#include <RpcErrorCode.h>
#include <RpcTypedefs.h>
#include <RpcManagedObject.h>
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

class Uploader;
class UploaderPrx;

}

namespace Rpc
{

class Uploader : public virtual ManagedObject
{
public:

    using ProxyType = UploaderPrx;

    /**
     * Determines whether this object supports an interface with the given Slice type ID.
     * @param id The fully-scoped Slice type ID.
     * @param current The Current object for the invocation.
     * @return True if this object supports the interface, false, otherwise.
     */
    virtual bool ice_isA(::std::string id, const ::Ice::Current& current) const override;

    /**
     * Obtains a list of the Slice type IDs representing the interfaces supported by this object.
     * @param current The Current object for the invocation.
     * @return A list of fully-scoped type IDs.
     */
    virtual ::std::vector<::std::string> ice_ids(const ::Ice::Current& current) const override;

    /**
     * Obtains a Slice type ID representing the most-derived interface supported by this object.
     * @param current The Current object for the invocation.
     * @return A fully-scoped type ID.
     */
    virtual ::std::string ice_id(const ::Ice::Current& current) const override;

    /**
     * Obtains the Slice type ID corresponding to this class.
     * @return A fully-scoped type ID.
     */
    static const ::std::string& ice_staticId();

    virtual ErrorCode write(long long int offset, ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*> bytes, const ::Ice::Current& current) = 0;
    /// \cond INTERNAL
    bool _iceD_write(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

    virtual ErrorCode finish(int crc32, const ::Ice::Current& current) = 0;
    /// \cond INTERNAL
    bool _iceD_finish(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

    virtual void cancel(const ::Ice::Current& current) = 0;
    /// \cond INTERNAL
    bool _iceD_cancel(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

    /// \cond INTERNAL
    virtual bool _iceDispatch(::IceInternal::Incoming&, const ::Ice::Current&) override;
    /// \endcond
};

}

namespace Rpc
{

class UploaderPrx : public virtual ::Ice::Proxy<UploaderPrx, ManagedObjectPrx>
{
public:

    ErrorCode write(long long int offset, const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& bytes, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _makePromiseOutgoing<ErrorCode>(true, this, &UploaderPrx::_iceI_write, offset, bytes, context).get();
    }

    template<template<typename> class P = ::std::promise>
    auto writeAsync(long long int offset, const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& bytes, const ::Ice::Context& context = ::Ice::noExplicitContext)
        -> decltype(::std::declval<P<ErrorCode>>().get_future())
    {
        return _makePromiseOutgoing<ErrorCode, P>(false, this, &UploaderPrx::_iceI_write, offset, bytes, context);
    }

    ::std::function<void()>
    writeAsync(long long int offset, const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& bytes,
               ::std::function<void(ErrorCode)> response,
               ::std::function<void(::std::exception_ptr)> ex = nullptr,
               ::std::function<void(bool)> sent = nullptr,
               const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _makeLamdaOutgoing<ErrorCode>(response, ex, sent, this, &Rpc::UploaderPrx::_iceI_write, offset, bytes, context);
    }

    /// \cond INTERNAL
    void _iceI_write(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<ErrorCode>>&, long long int, const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>&, const ::Ice::Context&);
    /// \endcond

    ErrorCode finish(int crc32, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _makePromiseOutgoing<ErrorCode>(true, this, &UploaderPrx::_iceI_finish, crc32, context).get();
    }

    template<template<typename> class P = ::std::promise>
    auto finishAsync(int crc32, const ::Ice::Context& context = ::Ice::noExplicitContext)
        -> decltype(::std::declval<P<ErrorCode>>().get_future())
    {
        return _makePromiseOutgoing<ErrorCode, P>(false, this, &UploaderPrx::_iceI_finish, crc32, context);
    }

    ::std::function<void()>
    finishAsync(int crc32,
                ::std::function<void(ErrorCode)> response,
                ::std::function<void(::std::exception_ptr)> ex = nullptr,
                ::std::function<void(bool)> sent = nullptr,
                const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _makeLamdaOutgoing<ErrorCode>(response, ex, sent, this, &Rpc::UploaderPrx::_iceI_finish, crc32, context);
    }

    /// \cond INTERNAL
    void _iceI_finish(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<ErrorCode>>&, int, const ::Ice::Context&);
    /// \endcond

    void cancel(const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        _makePromiseOutgoing<void>(true, this, &UploaderPrx::_iceI_cancel, context).get();
    }

    template<template<typename> class P = ::std::promise>
    auto cancelAsync(const ::Ice::Context& context = ::Ice::noExplicitContext)
        -> decltype(::std::declval<P<void>>().get_future())
    {
        return _makePromiseOutgoing<void, P>(false, this, &UploaderPrx::_iceI_cancel, context);
    }

    ::std::function<void()>
    cancelAsync(::std::function<void()> response,
                ::std::function<void(::std::exception_ptr)> ex = nullptr,
                ::std::function<void(bool)> sent = nullptr,
                const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _makeLamdaOutgoing<void>(response, ex, sent, this, &Rpc::UploaderPrx::_iceI_cancel, context);
    }

    /// \cond INTERNAL
    void _iceI_cancel(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<void>>&, const ::Ice::Context&);
    /// \endcond

    /**
     * Obtains the Slice type ID of this interface.
     * @return The fully-scoped type ID.
     */
    static const ::std::string& ice_staticId();

protected:

    /// \cond INTERNAL
    UploaderPrx() = default;
    friend ::std::shared_ptr<UploaderPrx> IceInternal::createProxy<UploaderPrx>();

    virtual ::std::shared_ptr<::Ice::ObjectPrx> _newInstance() const override;
    /// \endcond
};

}

/// \cond STREAM
namespace Ice
{

}
/// \endcond

/// \cond INTERNAL
namespace Rpc
{

using UploaderPtr = ::std::shared_ptr<Uploader>;
using UploaderPrxPtr = ::std::shared_ptr<UploaderPrx>;

}
/// \endcond

#else // C++98 mapping

namespace IceProxy
{

namespace Rpc
{

class Uploader;
/// \cond INTERNAL
void _readProxy(::Ice::InputStream*, ::IceInternal::ProxyHandle< Uploader>&);
::IceProxy::Ice::Object* upCast(Uploader*);
/// \endcond

}

}

namespace Rpc
{

class Uploader;
/// \cond INTERNAL
::Ice::Object* upCast(Uploader*);
/// \endcond
typedef ::IceInternal::Handle< Uploader> UploaderPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::Rpc::Uploader> UploaderPrx;
typedef UploaderPrx UploaderPrxPtr;
/// \cond INTERNAL
void _icePatchObjectPtr(UploaderPtr&, const ::Ice::ObjectPtr&);
/// \endcond

}

namespace Rpc
{

/**
 * Base class for asynchronous callback wrapper classes used for calls to
 * IceProxy::Rpc::Uploader::begin_write.
 * Create a wrapper instance by calling ::Rpc::newCallback_Uploader_write.
 */
class Callback_Uploader_write_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_Uploader_write_Base> Callback_Uploader_writePtr;

/**
 * Base class for asynchronous callback wrapper classes used for calls to
 * IceProxy::Rpc::Uploader::begin_finish.
 * Create a wrapper instance by calling ::Rpc::newCallback_Uploader_finish.
 */
class Callback_Uploader_finish_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_Uploader_finish_Base> Callback_Uploader_finishPtr;

/**
 * Base class for asynchronous callback wrapper classes used for calls to
 * IceProxy::Rpc::Uploader::begin_cancel.
 * Create a wrapper instance by calling ::Rpc::newCallback_Uploader_cancel.
 */
class Callback_Uploader_cancel_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_Uploader_cancel_Base> Callback_Uploader_cancelPtr;

}

namespace IceProxy
{

namespace Rpc
{

class Uploader : public virtual ::Ice::Proxy<Uploader, ::IceProxy::Rpc::ManagedObject>
{
public:

    ::Rpc::ErrorCode write(::Ice::Long offset, const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& bytes, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return end_write(_iceI_begin_write(offset, bytes, context, ::IceInternal::dummyCallback, 0, true));
    }

    ::Ice::AsyncResultPtr begin_write(::Ice::Long offset, const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& bytes, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _iceI_begin_write(offset, bytes, context, ::IceInternal::dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_write(::Ice::Long offset, const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& bytes, const ::Ice::CallbackPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_write(offset, bytes, ::Ice::noExplicitContext, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_write(::Ice::Long offset, const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& bytes, const ::Ice::Context& context, const ::Ice::CallbackPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_write(offset, bytes, context, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_write(::Ice::Long offset, const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& bytes, const ::Rpc::Callback_Uploader_writePtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_write(offset, bytes, ::Ice::noExplicitContext, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_write(::Ice::Long offset, const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& bytes, const ::Ice::Context& context, const ::Rpc::Callback_Uploader_writePtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_write(offset, bytes, context, cb, cookie);
    }

    ::Rpc::ErrorCode end_write(const ::Ice::AsyncResultPtr& result);

private:

    ::Ice::AsyncResultPtr _iceI_begin_write(::Ice::Long, const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>&, const ::Ice::Context&, const ::IceInternal::CallbackBasePtr&, const ::Ice::LocalObjectPtr& cookie = 0, bool sync = false);

public:

    ::Rpc::ErrorCode finish(::Ice::Int crc32, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return end_finish(_iceI_begin_finish(crc32, context, ::IceInternal::dummyCallback, 0, true));
    }

    ::Ice::AsyncResultPtr begin_finish(::Ice::Int crc32, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _iceI_begin_finish(crc32, context, ::IceInternal::dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_finish(::Ice::Int crc32, const ::Ice::CallbackPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_finish(crc32, ::Ice::noExplicitContext, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_finish(::Ice::Int crc32, const ::Ice::Context& context, const ::Ice::CallbackPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_finish(crc32, context, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_finish(::Ice::Int crc32, const ::Rpc::Callback_Uploader_finishPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_finish(crc32, ::Ice::noExplicitContext, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_finish(::Ice::Int crc32, const ::Ice::Context& context, const ::Rpc::Callback_Uploader_finishPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_finish(crc32, context, cb, cookie);
    }

    ::Rpc::ErrorCode end_finish(const ::Ice::AsyncResultPtr& result);

private:

    ::Ice::AsyncResultPtr _iceI_begin_finish(::Ice::Int, const ::Ice::Context&, const ::IceInternal::CallbackBasePtr&, const ::Ice::LocalObjectPtr& cookie = 0, bool sync = false);

public:

    void cancel(const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        end_cancel(_iceI_begin_cancel(context, ::IceInternal::dummyCallback, 0, true));
    }

    ::Ice::AsyncResultPtr begin_cancel(const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _iceI_begin_cancel(context, ::IceInternal::dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_cancel(const ::Ice::CallbackPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_cancel(::Ice::noExplicitContext, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_cancel(const ::Ice::Context& context, const ::Ice::CallbackPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_cancel(context, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_cancel(const ::Rpc::Callback_Uploader_cancelPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_cancel(::Ice::noExplicitContext, cb, cookie);
    }

    ::Ice::AsyncResultPtr begin_cancel(const ::Ice::Context& context, const ::Rpc::Callback_Uploader_cancelPtr& cb, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_cancel(context, cb, cookie);
    }

    void end_cancel(const ::Ice::AsyncResultPtr& result);

private:

    ::Ice::AsyncResultPtr _iceI_begin_cancel(const ::Ice::Context&, const ::IceInternal::CallbackBasePtr&, const ::Ice::LocalObjectPtr& cookie = 0, bool sync = false);

public:

    /**
     * Obtains the Slice type ID corresponding to this interface.
     * @return A fully-scoped type ID.
     */
    static const ::std::string& ice_staticId();

protected:
    /// \cond INTERNAL

    virtual ::IceProxy::Ice::Object* _newInstance() const;
    /// \endcond
};

}

}

namespace Rpc
{

class Uploader : virtual public ManagedObject
{
public:

    typedef UploaderPrx ProxyType;
    typedef UploaderPtr PointerType;

    virtual ~Uploader();

    /**
     * Determines whether this object supports an interface with the given Slice type ID.
     * @param id The fully-scoped Slice type ID.
     * @param current The Current object for the invocation.
     * @return True if this object supports the interface, false, otherwise.
     */
    virtual bool ice_isA(const ::std::string& id, const ::Ice::Current& current = ::Ice::emptyCurrent) const;

    /**
     * Obtains a list of the Slice type IDs representing the interfaces supported by this object.
     * @param current The Current object for the invocation.
     * @return A list of fully-scoped type IDs.
     */
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& current = ::Ice::emptyCurrent) const;

    /**
     * Obtains a Slice type ID representing the most-derived interface supported by this object.
     * @param current The Current object for the invocation.
     * @return A fully-scoped type ID.
     */
    virtual const ::std::string& ice_id(const ::Ice::Current& current = ::Ice::emptyCurrent) const;

    /**
     * Obtains the Slice type ID corresponding to this class.
     * @return A fully-scoped type ID.
     */
    static const ::std::string& ice_staticId();

    virtual ErrorCode write(::Ice::Long offset, const ::std::pair<const ::Ice::Byte*, const ::Ice::Byte*>& bytes, const ::Ice::Current& current = ::Ice::emptyCurrent) = 0;
    /// \cond INTERNAL
    bool _iceD_write(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

    virtual ErrorCode finish(::Ice::Int crc32, const ::Ice::Current& current = ::Ice::emptyCurrent) = 0;
    /// \cond INTERNAL
    bool _iceD_finish(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

    virtual void cancel(const ::Ice::Current& current = ::Ice::emptyCurrent) = 0;
    /// \cond INTERNAL
    bool _iceD_cancel(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

    /// \cond INTERNAL
    virtual bool _iceDispatch(::IceInternal::Incoming&, const ::Ice::Current&);
    /// \endcond

protected:

    /// \cond STREAM
    virtual void _iceWriteImpl(::Ice::OutputStream*) const;
    virtual void _iceReadImpl(::Ice::InputStream*);
    /// \endcond
};

/// \cond INTERNAL
inline bool operator==(const Uploader& lhs, const Uploader& rhs)
{
    return static_cast<const ::Ice::Object&>(lhs) == static_cast<const ::Ice::Object&>(rhs);
}

inline bool operator<(const Uploader& lhs, const Uploader& rhs)
{
    return static_cast<const ::Ice::Object&>(lhs) < static_cast<const ::Ice::Object&>(rhs);
}
/// \endcond

}

/// \cond STREAM
namespace Ice
{

}
/// \endcond

namespace Rpc
{

/**
 * Type-safe asynchronous callback wrapper class used for calls to
 * IceProxy::Rpc::Uploader::begin_write.
 * Create a wrapper instance by calling ::Rpc::newCallback_Uploader_write.
 */
template<class T>
class CallbackNC_Uploader_write : public Callback_Uploader_write_Base, public ::IceInternal::TwowayCallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);
    typedef void (T::*Response)(ErrorCode);

    CallbackNC_Uploader_write(const TPtr& obj, Response cb, Exception excb, Sent sentcb)
        : ::IceInternal::TwowayCallbackNC<T>(obj, cb != 0, excb, sentcb), _response(cb)
    {
    }

    /// \cond INTERNAL
    virtual void completed(const ::Ice::AsyncResultPtr& result) const
    {
        UploaderPrx proxy = UploaderPrx::uncheckedCast(result->getProxy());
        ErrorCode ret;
        try
        {
            ret = proxy->end_write(result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::CallbackNC<T>::exception(result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::CallbackNC<T>::_callback.get()->*_response)(ret);
        }
    }
    /// \endcond

private:

    Response _response;
};

/**
 * Creates a callback wrapper instance that delegates to your object.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_write.
 */
template<class T> Callback_Uploader_writePtr
newCallback_Uploader_write(const IceUtil::Handle<T>& instance, void (T::*cb)(ErrorCode), void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Uploader_write<T>(instance, cb, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_write.
 */
template<class T> Callback_Uploader_writePtr
newCallback_Uploader_write(T* instance, void (T::*cb)(ErrorCode), void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Uploader_write<T>(instance, cb, excb, sentcb);
}

/**
 * Type-safe asynchronous callback wrapper class with cookie support used for calls to
 * IceProxy::Rpc::Uploader::begin_write.
 * Create a wrapper instance by calling ::Rpc::newCallback_Uploader_write.
 */
template<class T, typename CT>
class Callback_Uploader_write : public Callback_Uploader_write_Base, public ::IceInternal::TwowayCallback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception& , const CT&);
    typedef void (T::*Sent)(bool , const CT&);
    typedef void (T::*Response)(ErrorCode, const CT&);

    Callback_Uploader_write(const TPtr& obj, Response cb, Exception excb, Sent sentcb)
        : ::IceInternal::TwowayCallback<T, CT>(obj, cb != 0, excb, sentcb), _response(cb)
    {
    }

    /// \cond INTERNAL
    virtual void completed(const ::Ice::AsyncResultPtr& result) const
    {
        UploaderPrx proxy = UploaderPrx::uncheckedCast(result->getProxy());
        ErrorCode ret;
        try
        {
            ret = proxy->end_write(result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::Callback<T, CT>::exception(result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::Callback<T, CT>::_callback.get()->*_response)(ret, CT::dynamicCast(result->getCookie()));
        }
    }
    /// \endcond

private:

    Response _response;
};

/**
 * Creates a callback wrapper instance that delegates to your object.
 * Use this overload when your callback methods receive a cookie value.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_write.
 */
template<class T, typename CT> Callback_Uploader_writePtr
newCallback_Uploader_write(const IceUtil::Handle<T>& instance, void (T::*cb)(ErrorCode, const CT&), void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Uploader_write<T, CT>(instance, cb, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * Use this overload when your callback methods receive a cookie value.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_write.
 */
template<class T, typename CT> Callback_Uploader_writePtr
newCallback_Uploader_write(T* instance, void (T::*cb)(ErrorCode, const CT&), void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Uploader_write<T, CT>(instance, cb, excb, sentcb);
}

/**
 * Type-safe asynchronous callback wrapper class used for calls to
 * IceProxy::Rpc::Uploader::begin_finish.
 * Create a wrapper instance by calling ::Rpc::newCallback_Uploader_finish.
 */
template<class T>
class CallbackNC_Uploader_finish : public Callback_Uploader_finish_Base, public ::IceInternal::TwowayCallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);
    typedef void (T::*Response)(ErrorCode);

    CallbackNC_Uploader_finish(const TPtr& obj, Response cb, Exception excb, Sent sentcb)
        : ::IceInternal::TwowayCallbackNC<T>(obj, cb != 0, excb, sentcb), _response(cb)
    {
    }

    /// \cond INTERNAL
    virtual void completed(const ::Ice::AsyncResultPtr& result) const
    {
        UploaderPrx proxy = UploaderPrx::uncheckedCast(result->getProxy());
        ErrorCode ret;
        try
        {
            ret = proxy->end_finish(result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::CallbackNC<T>::exception(result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::CallbackNC<T>::_callback.get()->*_response)(ret);
        }
    }
    /// \endcond

private:

    Response _response;
};

/**
 * Creates a callback wrapper instance that delegates to your object.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_finish.
 */
template<class T> Callback_Uploader_finishPtr
newCallback_Uploader_finish(const IceUtil::Handle<T>& instance, void (T::*cb)(ErrorCode), void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Uploader_finish<T>(instance, cb, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_finish.
 */
template<class T> Callback_Uploader_finishPtr
newCallback_Uploader_finish(T* instance, void (T::*cb)(ErrorCode), void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Uploader_finish<T>(instance, cb, excb, sentcb);
}

/**
 * Type-safe asynchronous callback wrapper class with cookie support used for calls to
 * IceProxy::Rpc::Uploader::begin_finish.
 * Create a wrapper instance by calling ::Rpc::newCallback_Uploader_finish.
 */
template<class T, typename CT>
class Callback_Uploader_finish : public Callback_Uploader_finish_Base, public ::IceInternal::TwowayCallback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception& , const CT&);
    typedef void (T::*Sent)(bool , const CT&);
    typedef void (T::*Response)(ErrorCode, const CT&);

    Callback_Uploader_finish(const TPtr& obj, Response cb, Exception excb, Sent sentcb)
        : ::IceInternal::TwowayCallback<T, CT>(obj, cb != 0, excb, sentcb), _response(cb)
    {
    }

    /// \cond INTERNAL
    virtual void completed(const ::Ice::AsyncResultPtr& result) const
    {
        UploaderPrx proxy = UploaderPrx::uncheckedCast(result->getProxy());
        ErrorCode ret;
        try
        {
            ret = proxy->end_finish(result);
        }
        catch(const ::Ice::Exception& ex)
        {
            ::IceInternal::Callback<T, CT>::exception(result, ex);
            return;
        }
        if(_response)
        {
            (::IceInternal::Callback<T, CT>::_callback.get()->*_response)(ret, CT::dynamicCast(result->getCookie()));
        }
    }
    /// \endcond

private:

    Response _response;
};

/**
 * Creates a callback wrapper instance that delegates to your object.
 * Use this overload when your callback methods receive a cookie value.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_finish.
 */
template<class T, typename CT> Callback_Uploader_finishPtr
newCallback_Uploader_finish(const IceUtil::Handle<T>& instance, void (T::*cb)(ErrorCode, const CT&), void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Uploader_finish<T, CT>(instance, cb, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * Use this overload when your callback methods receive a cookie value.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_finish.
 */
template<class T, typename CT> Callback_Uploader_finishPtr
newCallback_Uploader_finish(T* instance, void (T::*cb)(ErrorCode, const CT&), void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Uploader_finish<T, CT>(instance, cb, excb, sentcb);
}

/**
 * Type-safe asynchronous callback wrapper class used for calls to
 * IceProxy::Rpc::Uploader::begin_cancel.
 * Create a wrapper instance by calling ::Rpc::newCallback_Uploader_cancel.
 */
template<class T>
class CallbackNC_Uploader_cancel : public Callback_Uploader_cancel_Base, public ::IceInternal::OnewayCallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);
    typedef void (T::*Response)();

    CallbackNC_Uploader_cancel(const TPtr& obj, Response cb, Exception excb, Sent sentcb)
        : ::IceInternal::OnewayCallbackNC<T>(obj, cb, excb, sentcb)
    {
    }
};

/**
 * Creates a callback wrapper instance that delegates to your object.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_cancel.
 */
template<class T> Callback_Uploader_cancelPtr
newCallback_Uploader_cancel(const IceUtil::Handle<T>& instance, void (T::*cb)(), void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Uploader_cancel<T>(instance, cb, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * @param instance The callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_cancel.
 */
template<class T> Callback_Uploader_cancelPtr
newCallback_Uploader_cancel(const IceUtil::Handle<T>& instance, void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Uploader_cancel<T>(instance, 0, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_cancel.
 */
template<class T> Callback_Uploader_cancelPtr
newCallback_Uploader_cancel(T* instance, void (T::*cb)(), void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Uploader_cancel<T>(instance, cb, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * @param instance The callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_cancel.
 */
template<class T> Callback_Uploader_cancelPtr
newCallback_Uploader_cancel(T* instance, void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_Uploader_cancel<T>(instance, 0, excb, sentcb);
}

/**
 * Type-safe asynchronous callback wrapper class with cookie support used for calls to
 * IceProxy::Rpc::Uploader::begin_cancel.
 * Create a wrapper instance by calling ::Rpc::newCallback_Uploader_cancel.
 */
template<class T, typename CT>
class Callback_Uploader_cancel : public Callback_Uploader_cancel_Base, public ::IceInternal::OnewayCallback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception& , const CT&);
    typedef void (T::*Sent)(bool , const CT&);
    typedef void (T::*Response)(const CT&);

    Callback_Uploader_cancel(const TPtr& obj, Response cb, Exception excb, Sent sentcb)
        : ::IceInternal::OnewayCallback<T, CT>(obj, cb, excb, sentcb)
    {
    }
};

/**
 * Creates a callback wrapper instance that delegates to your object.
 * Use this overload when your callback methods receive a cookie value.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_cancel.
 */
template<class T, typename CT> Callback_Uploader_cancelPtr
newCallback_Uploader_cancel(const IceUtil::Handle<T>& instance, void (T::*cb)(const CT&), void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Uploader_cancel<T, CT>(instance, cb, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * Use this overload when your callback methods receive a cookie value.
 * @param instance The callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_cancel.
 */
template<class T, typename CT> Callback_Uploader_cancelPtr
newCallback_Uploader_cancel(const IceUtil::Handle<T>& instance, void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Uploader_cancel<T, CT>(instance, 0, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * Use this overload when your callback methods receive a cookie value.
 * @param instance The callback object.
 * @param cb The success method of the callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_cancel.
 */
template<class T, typename CT> Callback_Uploader_cancelPtr
newCallback_Uploader_cancel(T* instance, void (T::*cb)(const CT&), void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Uploader_cancel<T, CT>(instance, cb, excb, sentcb);
}

/**
 * Creates a callback wrapper instance that delegates to your object.
 * Use this overload when your callback methods receive a cookie value.
 * @param instance The callback object.
 * @param excb The exception method of the callback object.
 * @param sentcb The sent method of the callback object.
 * @return An object that can be passed to an asynchronous invocation of IceProxy::Rpc::Uploader::begin_cancel.
 */
template<class T, typename CT> Callback_Uploader_cancelPtr
newCallback_Uploader_cancel(T* instance, void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_Uploader_cancel<T, CT>(instance, 0, excb, sentcb);
}

}

#endif

#include <IceUtil/PopDisableWarnings.h>
#endif
