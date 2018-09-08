//
// Created by Yaroslav239 on 24.06.2018.
//

#include "vector_opt.h"


vector_opt::~vector_opt() {
    if (_is_ptr) {
        _is_ptr = false;
        data.ptr.reset(); ////???
    }
}

vector_opt::vector_opt()
        : _is_ptr(false), _size(0), _capacity(4), _real_data(data.small) {}

vector_opt::vector_opt(size_t size, vector_opt::ui value = 0)
        : vector_opt() {
    reserve(size);
    std::fill(_real_data,_real_data + size,value);
    _size = size;
}

vector_opt::vector_opt(const std::initializer_list<vector_opt::ui> &list)
        : vector_opt() {
    reserve(list.size());
    _size = list.size();
    std::copy(list.begin(), list.end(), _real_data);
}

vector_opt::vector_opt(const vector_opt &vec)
        :vector_opt() {
    _size = vec._size;
    _is_ptr = vec._is_ptr;
    _capacity = vec._capacity;
    if (vec._is_ptr) {
        new(&data.ptr) std::shared_ptr<ui>(vec.data.ptr);
        _real_data = data.ptr.get();
    } else {
        std::copy(vec.data.small,vec.data.small + vec._size,data.small);
        _real_data = data.small;
    }
}

void vector_opt::reserve(size_t capacity) {
    /*if (capacity > 4) {
        int xxx = 0;
        xxx++;
    }*/
    if (capacity < get_capacity() + 1) {
        return;
    }
    capacity = std::max(capacity, get_capacity() * ENLARGE);
    _capacity = capacity;
    if (_is_ptr) {
        auto p = data.ptr;
        data.ptr.reset(new ui[capacity]);
        std::copy(p.get(), p.get() + _size, data.ptr.get());
    } else {
        _is_ptr = true;
        auto new_data = new ui[capacity]; ////
        std::copy(_real_data, _real_data + _size, new_data);
        new(&data.ptr)std::shared_ptr<ui>(new_data,D());
    }
    _real_data = data.ptr.get();
}

void vector_opt::resize(size_t size) {
    resize(size,0);
}

void vector_opt::resize(size_t size, vector_opt::ui value ) {
    reserve(size);
    if (size > _size)
        std::fill(_real_data + _size, _real_data + size, value);
    _size = size;
}

size_t vector_opt::get_capacity() {
    return _capacity;
}

//!!!!!!!!!!!!!
vector_opt::ui* vector_opt::copy_and_get() {
    if (!_is_ptr || data.ptr.unique()) {
        return _real_data;
    }
    auto new_cap = std::min(get_capacity(), _size * ENLARGE);
    _capacity = new_cap;
    data.ptr.reset(new ui[new_cap]);
    std::copy(_real_data, _real_data + _size, data.ptr.get());
    return _real_data = data.ptr.get();
}

void vector_opt::push_back(vector_opt::ui x) {
    copy_and_get();
    if (get_capacity() < _size + 1)
        reserve(get_capacity() * ENLARGE);
    _real_data[_size++] = x;
}

vector_opt::ui &vector_opt::operator[](size_t i) {
    copy_and_get();
    return _real_data[i];
}

vector_opt &vector_opt::operator=(const vector_opt &other) noexcept {
    _size = other._size;
    _capacity = other._capacity;
    if (other._is_ptr) {
        if (_is_ptr)
            data.ptr = other.data.ptr;
        else
            new(&data.ptr)std::shared_ptr<ui>(other.data.ptr);
        _real_data = data.ptr.get();
    } else {
        if (_is_ptr)
            data.ptr.reset();
        std::copy(other.data.small, other.data.small + other._size, data.small);
        _real_data = data.small;
    }
    _is_ptr = other._is_ptr;
    return *this;
}

void vector_opt::pop_back() {
    _size--;
}

const vector_opt::ui &vector_opt::operator[](size_t i) const {
    return _real_data[i];
}

size_t vector_opt::size() const {
    return _size;
}

void vector_opt::clear() {
    _size = 0;
}

vector_opt::ui vector_opt::back() const {
    return _real_data[_size - 1];
}

vector_opt::ui *vector_opt::get_data() const {
    return _real_data;
}
