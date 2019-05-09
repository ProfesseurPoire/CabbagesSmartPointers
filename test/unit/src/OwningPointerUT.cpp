#include <cabba/test/test.h>
#include <cabba/memory/OwningPointer.h>

using namespace cabba::test;
using namespace cabba;

struct Foo
{
    Foo() = default;
    Foo(int v) :value(v){}

    Foo(int v, int r) :value(v*r) {}

    ~Foo() 
    {
        if (deleted != nullptr)
            *deleted = value;
    }

    int value = 5;
    int* deleted=nullptr;
};

Param& operator<<(Param& p, Foo f)
{
    //printf("%i", c.value);
    return p;
}

class OwningPointerUT : public Test
{
    public:

};

TEST_F(OwningPointerUT, empty_ptr)
{
    cabba::OwningPointer<Foo> ptr;

    assert_that(ptr.operator bool(), equals(false));
}

TEST_F(OwningPointerUT, creation)
{
    cabba::OwningPointer<Foo> ptr (new Foo());

    assert_that(ptr.operator bool(), equals(true));
    assert_that(ptr->value, equals(5));
}

TEST_F(OwningPointerUT, variadic_creation)
{
    cabba::OwningPointer<Foo> ptr = cabba::make_owning_pointer<Foo>(10, 3);

    assert_that(ptr.operator bool(), equals(true));
    assert_that(ptr->value, equals(30));
}

TEST_F(OwningPointerUT, release)
{
    auto ptr = make_owning_pointer<Foo>(10, 3);

    assert_that(ptr.operator bool(), equals(true));
    assert_that(ptr->value, equals(30));

    Foo* f = ptr.get();
    ptr.release();

    assert_that(ptr.operator bool(), equals(false));
    assert_that(ptr.get()==nullptr, equals(true));
}

TEST_F(OwningPointerUT, destroy)
{
    auto ptr = make_owning_pointer<Foo>(10, 3);
    int val;
    ptr.get()->deleted = &val;
    ptr.destroy();

    // Means that the first object managed by the pointer really went
    // through the destructors
    assert_that(val, equals(30));
    assert_that(ptr.operator bool(), equals(false));
}

TEST_F(OwningPointerUT, replace)
{              
    auto ptr =  make_owning_pointer<Foo>(10, 3);

    int val;
    ptr.get()->deleted = &val;
    ptr.replace(new Foo(4));

    // Means that the first object managed by the pointer really went
    // through the destructors
    assert_that(val, equals(30));
    assert_that(ptr.get()->value, equals(4));
}

TEST_F(OwningPointerUT, destruction)
{
    // Making sure Foo's destructor is correctly called
    auto* ptr = new OwningPointer<Foo>(new Foo(10,3));

    int val;
    ptr->get()->deleted = &val;
    delete ptr;

    assert_that(val, equals(30));
}

TEST_F(OwningPointerUT, move)
{
    OwningPointer<Foo> p (new Foo(4));
    auto ptr = make_owning_pointer<Foo>(10, 3);

    p = std::move(ptr);

    assert_that(ptr.operator bool(), equals(false));
    assert_that(ptr.get() == nullptr, equals(true));

    assert_that(p.get()->value, equals(30));
}