#include <iostream>
#include <vector>
#include "vec.h"
// #include "add.h"
using namespace std;
using namespace tiny_stl;

struct foo
{
    int x;
    foo(int n) noexcept : x{n} { printf("#%d is constructed.\n", x); }
    foo(const foo &s) noexcept : x{s.x} { printf("#%d is copied.\n", x); }
    foo(foo &&s) noexcept : x{s.x} { printf("#%d is moved.\n", x); }
    //~foo() noexcept { printf("#%d is destructed.\n", x); }
};

int main()
{
    vec<foo> a{1};
    a.push_back(2);
    cout<<a.emplace_back(3).x<<'\n';

    cout << a.size() << ' ' << a.capacity() << '\n';

    a.reserve(5);

    cout << a.size() << ' ' << a.capacity() << '\n';

    for (auto &x : a)
        cout << x.x << ' ';
    cout << '\n';

    // copy test
    
    auto b{(a)};
    b.push_back(4);
    cout<<b.emplace_back(5).x<<'\n';
    
    cout << b.size() << ' ' << b.capacity() << '\n';
         
    for (auto &x : b)
        cout << x.x << ' ';
    cout << '\n';

    // move test
    
    auto c{move(b)};
    c.push_back(6);
    cout<<c.emplace_back(7).x<<'\n';
    
    cout << c.size() << ' ' << c.capacity() << '\n';
         
    for (auto &x : c)
        cout << x.x << ' ';
    cout << '\n';
}
