#pragma once

#include <cabba/memory/ObserverPointer.h>
#include <type_traits>

namespace cabba
{

/*
 * @brief   Owns a pointer. The difference with std::unique_ptr is the
 *          observer mechanism. You can create Observers that points
 *          to this owningPointer. If the OwningPointer is deleted,
 *          the observers knows about this
 */
template<class T>
class OwningPointer
{
public:

// Lifecycle

    OwningPointer() = default;
    OwningPointer(T* t) { _ptr = t; }

    // Can't copy a Owning pointer around
    OwningPointer(const OwningPointer&) = delete;
    OwningPointer(OwningPointer&& p) { move(p); }

    ~OwningPointer() { destroy(); }

// Asignment Operator

    // Still can't copy a owning pointer
    OwningPointer& operator=(const OwningPointer&) = delete;
    OwningPointer& operator=(OwningPointer&& p)noexcept
    {
        move(p);
        return *this;
    }

// Operators

    T* operator->() { return _ptr;  }
    T& operator*()  { return *_ptr; }
    operator bool() { return _ptr != nullptr; }

// Functions
    
    /*!
     * @brief   Returns a pointer to the managed object
     *
     * @return  Returns the pointer to the managed object.
     *          Returns nullptr if nothing is currently being managed
     */
    T* get()    { return _ptr; }

    /*!
     * @brief   Deletes the current managed object 
     */
    void destroy()
    {
        if (_lived != nullptr)
            * _lived = false;

        _lived = nullptr;

        delete _ptr;
        _ptr = nullptr;
    }


    /*!
     * @brief   Replace the current managed object, deleting the previous
     *          one in the process
     *
     *          If the param is set to nullptr, nothing happens. Observers
     *          are broken in the process
     */
    void replace(T* new_ptr)
    {
        // If the new_ptr is equal to nullptr, nothing happens
        if (new_ptr == nullptr)
            return;

        // If some observer exist, we tell them the data they were tracking
        // doesn't exist anymore
        if (_lived != nullptr)
            *_lived = false;

        // We set _lived back to nullptr since that's another object and
        // new observers could be created for this object
        _lived = nullptr;

        // We delete the old ptr and store the new one
        delete _ptr;
        _ptr = new_ptr;
    }

    /*!
     * @brief   Stops managing the lifecycle of the current object
     *
     *          /!\ This actually release the pointer from it's main duty.
     *          In a real life scenario, you'd have to use get() beforehand
     *          to get back the pointer and manage it another way
     *      
     *          This will also nullify the observers that have been created,
     *          since we don't have any visibility to the pointer
     */
    void release()
    {
        _ptr = nullptr;

        if (_lived != nullptr)
            *_lived = false;
    }

    ObserverPointer<T> create_observer()
    {
        // Creates a thing in the heap to track if the 
        // pointer is still valid or not
        _lived = new bool(true);
        return ObserverPointer<T>(_ptr, _lived);
    }

    template<class U>
    ObserverPointer<U> create_derived_observer();

private:

// Functions

    void move(OwningPointer & p)
    {
        if (_ptr != nullptr)
            delete _ptr;

        //Also has to kill the observers

        if (_lived != nullptr)
            *_lived = false;

        _lived = nullptr;


        if (p._lived != nullptr)
            *p._lived = false;

        _ptr        = p._ptr;

        p._ptr      = nullptr;
        p._lived    = nullptr;
    }

// Member variables 

    // I should really try to replace that with T** once
    // the unit test are up 
    bool*     _lived    = nullptr;  
                                
    // Pointer to the data currently managed by the owning pointer
    T*      _ptr        = nullptr;
};


template<class T, typename... Args>
OwningPointer<T> make_owning_pointer(Args&&... args)
{
    return OwningPointer<T>(new T(std::forward<Args>(args)...));
}

template<class T>
template<class U>
ObserverPointer<U> OwningPointer<T>::create_derived_observer()
{
    static_assert(std::is_base_of<T, U>(),
                  "Template parameter is not a base of observed Pointer");
    // Creates a bool on the heap so the observer knows
    // if the pointer is still valid or not
    _lived = new bool(true);
    return ObserverPointer<U>(static_cast<U*>(_ptr), _lived);
}

}