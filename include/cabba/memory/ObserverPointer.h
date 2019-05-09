#pragma once

namespace cabba
{
template<class T>
class ObserverPointer
{
public:

    template <class U>
    friend class OwningPointer;

    ObserverPointer()   = default;
    ~ObserverPointer() { reset(); }

    ObserverPointer(const ObserverPointer& p)
    {
        if (p._count == nullptr)
            return;
            
        _count = p._count;
        (*_count)++;
        _ptr = p._ptr;
        _lived = p._lived;
    }

    bool valid()const
    {
        // Means the ObserverPointer is not pointing to anything
        if (_count == nullptr)
            return false;

        return *_lived;
    }

    ObserverPointer& operator=(const ObserverPointer& p)
    {
        if (p._count == nullptr)
            return *this;

        _count = p._count;
        (*_count)++;
        _ptr = p._ptr;
        _lived = p._lived;
    }

    T* operator->()
    {
        if (_count == nullptr)
            return nullptr;

        if (*_lived)
            return _ptr;
        return nullptr;
    }

    void reset()
    {
        if (_count == nullptr)
            return;

        (*_count)--;

        if (*_count == 0)
        {
            delete _lived;
            delete _count;
        }
    }

    int*    _count  = nullptr;
    bool*   _lived  = nullptr;
    T*      _ptr    = nullptr;

protected:

    ObserverPointer(T* ptr, bool* lived)
        :_count(new int(1)), _ptr(ptr), _lived(lived) {}
};
}