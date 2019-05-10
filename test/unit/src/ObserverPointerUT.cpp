#include <cabba/test/test.h>

#include <cabba/memory/ObserverPointer.h>
#include <cabba/memory/OwningPointer.h>

using namespace cabba;
using namespace cabba::test;

struct Bar
{
    Bar() = default;
    Bar(int v) :_val(v) {};
    Bar(int v, int r) : _val(v*r) {};
    ~Bar() 
    { 
        if(_val_destructor!=nullptr)
            *_val_destructor = _val;
    }

    int _val = 3;
    int * _val_destructor = nullptr;
};

class ObserverPointerUT : public cabba::test::Test
{
    public: 

    // We first create 2 owning pointer
    OwningPointer<Bar> bar;
    OwningPointer<Bar> bar2;

    void set_up()override
    {
        bar  = make_owning_pointer<Bar>(10, 3);
        bar2 = make_owning_pointer <Bar>(5, 2);
    }

};

TEST_F(ObserverPointerUT, empty_observer)
{
    ObserverPointer<Bar> obs1;

    assert_that(obs1.operator bool(),   equals(false));
    assert_that(obs1.operator bool(),   equals(false));
    assert_that(obs1.count(),           equals(-1));
    assert_that(obs1.operator->() == nullptr, equals(true));
}

TEST_F(ObserverPointerUT, owning_pointer_out_of_scope)
{
    ObserverPointer<Bar> obs1;
    OwningPointer<Bar> * bar = new OwningPointer<Bar>(new Bar(34));
    obs1 = bar->create_observer();

    assert_that(obs1.operator bool(), equals(true));
    assert_that(obs1->_val, equals(34));

    delete bar;

    assert_that(obs1.operator bool(), equals(false));
}

TEST_F(ObserverPointerUT, creation)
{
    // We make 1 observer from bar, and copy it in obs2
    ObserverPointer<Bar> obs1 = bar.create_observer();
    ObserverPointer<Bar> obs2 = obs1;

    // We make sure they both points to the same thing
    assert_that(obs1->_val, equals(30));
    assert_that(obs2->_val, equals(30));

    // We make sure there's actually 2 observer registered in the heap
    assert_that(obs1.count(), equals(2));
    assert_that(obs2.count(), equals(2));
    

    assert_that(obs1.operator bool(), equals(true));
    assert_that(obs2.operator bool(), equals(true));

    bar.replace(nullptr);

    assert_that(obs1.operator->() == nullptr, equals(true));

    assert_that(obs1.operator bool(), equals(true));
    assert_that(obs2.operator bool(), equals(true));

    bar = std::move(bar2);

    assert_that(obs1->_val, equals(10));

}

