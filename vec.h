#pragma once
#include <memory>
#include <initializer_list>
#include <iterator>

template <typename T>
class vec
{
    using size_t = std::size_t;

private:
    std::allocator<T> _alloc;
    T *_p;
    size_t _capacity, _size;
    constexpr bool chk_n_alloc(size_t);
    constexpr bool chk_n_realloc(size_t);

public:
    constexpr vec() : vec(0){};
    constexpr explicit vec(size_t);
    template <class Iter>
    constexpr vec(Iter begin, Iter end);
    constexpr vec(std::initializer_list<T> list) : vec(list.begin(), list.end()) {}
    constexpr vec(const vec<T> &s) : vec(s._p, s._p + s._size) {}
    constexpr vec(vec<T> &&);
    ~vec();
    constexpr size_t size() { return _size; }
    constexpr size_t capacity() { return _capacity; }
    constexpr T *begin() { return _p; }
    constexpr T *end() { return _p + _size; }
    constexpr void push_back(T &&);
    constexpr void push_back(const T &value);
};

template <typename T>
constexpr vec<T>::vec(size_t capacity) : _size(0), _capacity(capacity)
{
    chk_n_alloc(capacity);
}

template <typename T>
constexpr vec<T>::vec(vec<T> &&s) : _p(s._p), _size(s._size), _capacity(s._capacity)
{
    s._p = nullptr;
    s._size = 0;
    s._capacity = 0;
}

template <typename T>
inline vec<T>::~vec()
{
    std::destroy_n(_p, _size);
    _alloc.deallocate(_p, _capacity);
}

template <typename T>
template <class Iter>
constexpr vec<T>::vec(Iter begin, Iter end) : _size(std::distance(begin, end))
{
    chk_n_alloc(_size);
    std::uninitialized_copy_n(begin, _size, _p);
}

template <typename T>
constexpr void vec<T>::push_back(T &&value)
{
    chk_n_realloc(_size + 1);
    _alloc.construct(_p + _size, std::forward<T>(value));
    _size++;
}

template <typename T>
constexpr void vec<T>::push_back(const T &value)
{
    push_back(value);
}

// helper functions

template <typename T>
constexpr bool vec<T>::chk_n_alloc(size_t new_size)
{
    bool ret {false};

    if (_capacity == 0 && new_size > 0)
    {
        _capacity = new_size;
        ret = true;
    }
    else while (new_size > _capacity)
        {
            _capacity *= 2;
            ret = true;
        }

    if (ret) _p = _alloc.allocate(_capacity);
    return ret;
}

template <typename T>
constexpr bool vec<T>::chk_n_realloc(size_t new_size)
{
    T *s = _p;
    size_t o_size{_size}, o_capacity{_capacity};
    if (chk_n_alloc(new_size))
    {
        std::uninitialized_move_n(s, o_size, _p);
        std::destroy_n(s, o_size);
        _alloc.deallocate(s, o_capacity);
        return true;
    }
    else
        return false;
}
