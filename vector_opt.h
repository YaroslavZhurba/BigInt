//
// Created by Yaroslav239 on 24.06.2018.
//

#ifndef BIGINT_VECTOR_OPT_H
#define BIGINT_VECTOR_OPT_H

#include <iostream>
#include <cassert>
#include <memory>

class vector_opt {
public:
    typedef unsigned int ui;

    vector_opt();

    vector_opt(size_t size);

    vector_opt(size_t size, ui val);

    vector_opt(const std::initializer_list<ui> &list);

    ~vector_opt();

    vector_opt(const vector_opt &other) ;

    void reserve(size_t capacity);

    void resize(size_t size, ui value);

    void resize(size_t size);

    void push_back(ui x);

    void pop_back();

    size_t get_capacity();

    size_t size() const;

    void clear();

    const ui &operator[](size_t i) const;

    ui &operator[](size_t i);

    ui back() const;

    vector_opt &operator=(const vector_opt &other) noexcept;

    ui *get_data() const;

    ui* copy_and_get();

private:
    static const size_t SMALL_SIZE = 4;
    static const size_t ENLARGE = 2;
    bool _is_ptr;
    size_t _size;
    size_t _capacity;
    ui *_real_data;



//    struct big_data {
//        size_t capacity;
//        std::shared_ptr<ui> ptr;
//    };

    struct D {
        void operator()(ui* p) const {
            delete[] p;
        }
    };
    union union_data {
        ui small[SMALL_SIZE]{};
//        big_data big;
        std::shared_ptr<ui> ptr;

        union_data() {}

        ~union_data() {}
    } data;
};

#endif //BIGINT_VECTOR_OPT_H
