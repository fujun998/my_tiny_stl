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
        using iterator = T *;
        using const_iterator = const T *;

    private:
        std::allocator<T> _alloc;
        T *_first{nullptr};
        size_t _capacity{0}, _size{0};
        constexpr bool chk_n_alloc(size_t);

    public:
        constexpr vec() noexcept : vec(0){};
        constexpr explicit vec(size_t);
        template <class It>
        constexpr vec(It, It);
        constexpr vec(std::initializer_list<T> list) : vec(list.begin(), list.end()) {}
        constexpr vec(const vec<T> &s) : vec(s.begin(), s.end()) {}
        constexpr vec(vec<T> &&) noexcept;
        ~vec() noexcept;

        constexpr size_t size() const noexcept { return _size; }
        constexpr size_t capacity() const noexcept { return _capacity; }

        constexpr T *data() noexcept { return _first; }
        constexpr const T *data() const noexcept { return _first; }

        constexpr T &operator[](size_t n) noexcept { return _first[n]; }
        constexpr const T &operator[](size_t n) const noexcept { return _first[n]; }

        template <typename... Args>
        constexpr T &emplace_back(Args &&...);
        constexpr void push_back(T &&value) { emplace_back(std::move(value)); }
        constexpr void push_back(const T &value) { emplace_back(value); }

        constexpr void reserve(size_t);

        // iterators

        constexpr const_iterator cbegin() const noexcept { return _first; }
        constexpr const_iterator cend() const noexcept { return _first + _size; }
        constexpr const_iterator begin() const noexcept { return cbegin(); }
        constexpr const_iterator end() const noexcept { return cend(); }
        constexpr iterator begin() noexcept { return const_cast<iterator>(cbegin()); }
        constexpr iterator end() noexcept { return const_cast<iterator>(cend()); }
    };

    // constructors and destructor

    template <typename T>
    constexpr vec<T>::vec(size_t capacity)
    {
        chk_n_alloc(capacity);
    }

    template <typename T>
    template <class It>
    constexpr vec<T>::vec(It first, It last)
    {
        using category = typename std::iterator_traits<It>::iterator_category;
        static_assert(std::is_base_of_v<std::input_iterator_tag, category>);
        if constexpr (std::is_base_of_v<std::random_access_iterator_tag, category>)
        {
            _size = last - first;
            chk_n_alloc(_size);
            std::uninitialized_copy(first, last, _first);
        }
        else
        {
            for (; first != last; ++first)
                emplace_back(*first);
        }
    }

    template <typename T>
    constexpr vec<T>::vec(vec<T> &&s) noexcept : _first(s._first), _size(s._size), _capacity(s._capacity)
    {
        s._first = nullptr;
        s._size = 0;
        s._capacity = 0;
    }

    template <typename T>
    inline vec<T>::~vec() noexcept
    {
        std::destroy_n(_first, _size);
        _alloc.deallocate(_first, _capacity);
    }

    // public functions

    template <typename T>
    template <typename... Args>
    constexpr T &vec<T>::emplace_back(Args &&...args)
    {
        if (_capacity == 0)
            chk_n_alloc(1);
        else if (_size == _capacity)
            reserve(_capacity * 2);
        _alloc.construct(_first + _size, std::forward<Args>(args)...);
        return _first[_size++];
    }

    template <typename T>
    constexpr void vec<T>::reserve(size_t new_capacity)
    {
        T *original_first = _first;
        size_t original_capacity{_capacity};
        if (chk_n_alloc(new_capacity))
        {
            std::uninitialized_move_n(original_first, _size, _first);
            std::destroy_n(original_first, _size);
            _alloc.deallocate(original_first, original_capacity);
        }
    }

    // helper functions

    template <typename T>
    constexpr bool vec<T>::chk_n_alloc(size_t new_capacity)
    {
        if (new_capacity <= _capacity)
            return false;

        _capacity = new_capacity;
        _first = _alloc.allocate(_capacity);
        return true;
    }
}