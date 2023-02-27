#pragma once
#include <memory>
#include <initializer_list>
#include <iterator>

namespace tiny_stl
{
    template <typename T>
    class vec
    {
        using size_t = std::size_t;

    private:
        std::allocator<T> _alloc;
        T *_p{nullptr};
        size_t _capacity{0}, _size{0};
        constexpr bool chk_n_alloc(size_t);

    public:
        constexpr vec() noexcept : vec(0){};
        constexpr explicit vec(size_t);
        template <class Iter>
        constexpr vec(Iter begin, Iter end);
        constexpr vec(std::initializer_list<T> list) : vec(list.begin(), list.end()) {}
        constexpr vec(const vec<T> &s) : vec(s._p, s._p + s._size) {}
        constexpr vec(vec<T> &&) noexcept;
        ~vec() noexcept;

        constexpr size_t size() const noexcept { return _size; }
        constexpr size_t capacity() const noexcept { return _capacity; }
        constexpr T *begin() const noexcept { return _p; }
        constexpr T *end() const noexcept { return _p + _size; }
        constexpr T &operator[](size_t n) const noexcept { return _p[n]; }
        //constexpr const T &operator[](size_t n) const noexcept { return _p[n]; }
        
        constexpr void push_back(T &&);
        //constexpr void push_back(const T &value) { push_back(value); }

        constexpr bool reserve(size_t);
    };

    // constructors and deconstructor

    template <typename T>
    constexpr vec<T>::vec(size_t capacity)
    {
        chk_n_alloc(capacity);
    }

    template <typename T>
    template <class Iter>
    constexpr vec<T>::vec(Iter begin, Iter end) : _size(std::distance(begin, end))
    {
        chk_n_alloc(_size);
        std::uninitialized_copy_n(begin, _size, _p);
    }

    template <typename T>
    constexpr vec<T>::vec(vec<T> &&s) noexcept : _p(s._p), _size(s._size), _capacity(s._capacity)
    {
        s._p = nullptr;
        s._size = 0;
        s._capacity = 0;
    }

    template <typename T>
    inline vec<T>::~vec() noexcept
    {
        std::destroy_n(_p, _size);
        _alloc.deallocate(_p, _capacity);
    }

    // public functions

    template <typename T>
    constexpr void vec<T>::push_back(T &&value)
    {
        if(_capacity==0)
            chk_n_alloc(1);
        else if(_size==_capacity)
            reserve(_capacity * 2);
        _alloc.construct(_p + _size, std::forward<T>(value));
        _size++;
    }

    template <typename T>
    constexpr bool vec<T>::reserve(size_t new_capacity)
    {
        T *s = _p;
        size_t o_capacity{_capacity};
        if (chk_n_alloc(new_capacity))
        {
            std::uninitialized_move_n(s, _size, _p);
            std::destroy_n(s, _size);
            _alloc.deallocate(s, o_capacity);
            return true;
        }
        else
            return false;
    }

    // helper functions

    template <typename T>
    constexpr bool vec<T>::chk_n_alloc(size_t new_capacity)
    {
        if(new_capacity<=_capacity)
            return false;

        _capacity = new_capacity;
        _p = _alloc.allocate(_capacity);
        return true;
    }
}
