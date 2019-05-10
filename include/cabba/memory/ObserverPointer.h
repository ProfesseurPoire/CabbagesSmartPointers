#pragma once

#include <assert.h>

namespace cabba
{

/*
 * @brief   An observerPointer keeps tracks of the content of an OwningPointer,
 *          without owning the data. If the owning pointer goes out of scope
 *          or changes it's content, the ObserverPointer is notified.
 *
 *          To make that possible, the observerPointer stores a pointer to the
 *          type of point of the OwningPointer. It's the OwningPointer 
 *          responsability to update this pointer whenever it's content gets
 *          changed.
 *
 *          The ObserverPointer also stores how much copy of itself have been
 *          made, though the "_count" variable. It'll only delete the 
 *          T** pointer once every other ObserverPointer has been deleted
 */
template<class T>
class ObserverPointer
{
public:

    template <class U>
    friend class OwningPointer;

// Lifecycle

    ObserverPointer()                           = default;
    ~ObserverPointer()                          {destroy(); }
    ObserverPointer(const ObserverPointer& p)   {copy(p);}
    ObserverPointer(ObserverPointer&& obs)      {move(obs)}

// Assignment operators

    ObserverPointer& operator=(ObserverPointer&& obs){ move(obs);return *this;}

    ObserverPointer& operator=(const ObserverPointer& p)
    {
        copy(p);
        return *this;
    }

// Operators

    /*
     * @brief   Checks if the observer actually points to a OwningPointer
     */
    operator bool()const 
    { 
        // If _alive is equals to nullptr that means the Observer isn't
        // linked to an OwningPointer
        if (_alive == nullptr)
            return false;

        // Then if points to something, we check if it's alive
        return *_alive != false; 
    }

    T* operator->()
    {
        if (_alive != nullptr)
            if (*_alive != false)
                return *_lived;
        return nullptr;
    }

    /*
     * @brief   Returns a reference to the pointed object
     *
     * @pre     Must be pointing to an actual object
     */
    T& operator*()
    {
        if (_alive != nullptr)      // If this was registered to point to smt
            if (*alive != false)    // If that something is still alive
                if (*_lived != nullptr) // if what's pointed is different to null
                    return **_lived;
    }

// Functions

    /*
     * @brief   Returns how many observer are currently looking at the same
     *          thing
     *
     * @return  Returns -1 if the observer isn't looking at a owning ptr
     */
    int count()const
    {
        if (_count == nullptr)
            return -1;
        return *_count;
    }

protected:

    void copy(const ObserverPointer& cpy)
    {
        if (cpy._count == nullptr)
            return;

        _count = cpy._count;
        (*_count)++;
        _lived = cpy._lived;
        _alive = cpy._alive;
    }

    void move(ObserverPointer& obs)
    {
        _count = obs._count;
        _lived = obs._lived;
        _alive = obs._alive;

        obs._count = nullptr;
        obs._lived = nullptr;
        obs._alive = nullptr;
    }

    void destroy()
    {
        if (_alive == nullptr)  // Means it doesn't point to anything
            return;             // and has not been initialized

        (*_count)--;

        if (*_count == 0)
        {
            delete _count;
            delete _alive;
        }
    }

    ObserverPointer(T** ptr, bool* alive)
        :_count(new int(1)), _lived(ptr), _alive(alive) {}

    int*    _count = nullptr;
    bool*   _alive = nullptr;
    T**     _lived = nullptr;
};
}