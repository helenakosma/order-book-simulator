#pragma once
#include <array>
#include <cassert>
#include <cstddef>

template<typename T, std::size_t N>
class SlabAlloc {
public:
    SlabAlloc() {
        for (std::size_t i = 0; i < N; ++i)
            free_stack_[i] = &pool_[i];
        top_ = N;
    }

    T* alloc() {
        assert(top_ > 0 && "slab exhausted");
        return free_stack_[--top_];
    }

    void free(T* p) {
        assert(top_ < N && "double free or slab overflow");
        free_stack_[top_++] = p;
    }

    std::size_t available() const { return top_; }

private:
    std::array<T, N>  pool_;
    std::array<T*, N> free_stack_;
    std::size_t       top_;
};
