#ifndef DEF_H
#define DEF_H

#include <functional>

/**********************************************************************************************************************
@brief: concatenating multiple args into one
***********************************************************************************************************************/
#define CONCATE(...) __VA_ARGS__

/**********************************************************************************************************************
@brief:
***********************************************************************************************************************/
#define ADD_FIELD(type, name, init) ADD_CLASS_FIELD_WITH_GET_SET(type, name, init)
#define ADD_CLASS_FIELD_WITH_GET_SET(type, name, init) \
    public: \
        type& get##name() { return m_##name; } \
        type const & get##name() const{ return m_##name; } \
        void set##name(type name) { m_##name = name; } \
    private: \
        type m_##name = init;

/**********************************************************************************************************************
@brief:
***********************************************************************************************************************/
#define ADD_FIELD_ALL(type, name, init) ADD_CLASS_FIELD_WITH_GET_SET_NOTIFY(type, name, init)
#define ADD_CLASS_FIELD_WITH_GET_SET_NOTIFY(type, name, init) \
    public: \
        type& get##name() { return m_##name; } \
        type const & get##name() const{ return m_##name; } \
        void set##name(type name) { if(m_##name != name) { m_##name = name; emit sig##name##Changed(m_##name);} } \
    Q_SIGNALS: \
        void sig##name##Changed(type); \
    private: \
        type m_##name = init;

/**********************************************************************************************************************
@brief: PROTOTYPE PATTERN
***********************************************************************************************************************/
#define CLONABLE(classname)\
    public:\
        virtual classname* clone() const { return new classname(*this); }

/**********************************************************************************************************************
@brief: CLIP c BETWEEN a AND b
***********************************************************************************************************************/
#define VALUE_CLIP(min,max,value) ( ((value)>(max))?(max):((value)<(min))?(min):(value) )

/**********************************************************************************************************************
@brief: SCOPE GUARD
***********************************************************************************************************************/
template <typename F>
struct ScopeExit
{
    ScopeExit(F f) : f(f) {}
    ~ScopeExit() { f(); }
    F f;
};

template <typename F>
ScopeExit<F> MakeScopeExit(F f) 
{
    return ScopeExit<F>(f);
}

#define SCOPE_EXIT(code) \
    auto scope_exit_##__LINE__ = MakeScopeExit([=](){code;})

/**********************************************************************************************************************
@brief: make callback functor
***********************************************************************************************************************/
#define MAKE_CALLBACK(memberFunc) \
    std::bind( &memberFunc, this, std::placeholders::_1 )

#define MAKE_CALLBACK_OBJ(object, memberFunc) \
    std::bind( &memberFunc, &(object), std::placeholders::_1 )

/**********************************************************************************************************************
@brief:
***********************************************************************************************************************/
#define AppModelSingleton Singleton<AppModel>::getInstance()

#endif
