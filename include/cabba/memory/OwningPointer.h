#pragma once

#include <cabba/memory/ObserverPointer.h>
#include <type_traits>
#include <utility>
#include <assert.h>


namespace cabba
{
/*
 * @brief   Owns a pointer. The difference with std::unique_ptr is the
 *          observer mechanism. 
 *          
 *          You can create Observers that points to this owningPointer. 
 *          Observers always knows what's inside the OwningPointer. Even when
 *          the OwningPointer is deleted before the observers, observers will
 *          point to a nullptr and thus knows that there's no more data.
 *          
 *          Remarks : Should I add another variable on the heap
 *          to really notify the observers that the OwningPointer has been 
 *          destroyed?
 */
template<class T>
class OwningPointer
{
public:

// Lifecycle

    OwningPointer() = default;
    OwningPointer(T* t) { _ptr = t; }

    // You can't copy an OwningPointer, only move it
    OwningPointer(const OwningPointer&) = delete;
    OwningPointer(OwningPointer&& p)    { move(p); }
    ~OwningPointer()                    
    { 
        replace(nullptr);

        // Notifies the observers that the current object is deleted
        if (_alive)
            *_alive= false;   
    }

// Asignment Operator

    // You can't copy a OwningPointer, only move it
    OwningPointer& operator=(const OwningPointer&) = delete;
    OwningPointer& operator=(OwningPointer&& p)noexcept
    {
        move(p);
        return *this;
    }

// Operators

    operator bool() { return _ptr != nullptr; }
    T* operator->() { return _ptr;  }
    T& operator*()  
    { 
        assert(_ptr != nullptr&&"OwningPointer stores a nullptr");
        return *_ptr; 
    }

// Functions
    
    /*!
     * @brief   Returns a pointer to the managed object
     *
     * @return  Returns a pointer to the managed object or nullptr if 
     *          nothing is owned
     */
    T* get()    { return _ptr; }

    /*!
     * @brief   Replace the current managed object, deleting the previous
     *          one in the process
     * @param   ptr     Pointer to the object that will be managed
     */
    void replace(T* ptr)
    {
        // We delete the old ptr and store the new one
        delete _ptr;
        _ptr = ptr;
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
    }

    ObserverPointer<T> create_observer()
    {
        _alive = new bool(true);  // Creates a thing to track on the heap
        return ObserverPointer<T>(&_ptr, _alive);
    }

    template<class U>
    ObserverPointer<U> create_derived_observer();

private:

// Functions

    void move(OwningPointer & p)
    {
        replace(p._ptr);

        // The moved one though is dead now
        if (p._alive != nullptr)
            *p._alive= false;

        p._ptr      = nullptr;
        p._alive     = nullptr;
    }

// Member variables 

    // Tracks the owned pointer for the observers
    bool* _alive        = nullptr;
                                
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
    _alive = new bool(true);
    return ObserverPointer<U>(static_cast<U**>(_ptr, _alive));
}
}