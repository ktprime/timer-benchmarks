//https://github.com/p-ranav/small_vector
#pragma once
#include <array>
#include <vector>

namespace sv {

template <class T, std::size_t N, class Allocator = std::allocator<T>> class small_vector {
  std::array<T, N> stack_;
  std::vector<T, Allocator> heap_;
  int size_{0};

public:
  typedef T value_type;
  typedef std::size_t size_type;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef Allocator allocator_type;
  typedef T* pointer;
  typedef const T* const_pointer;

  small_vector() = default;

  explicit small_vector(size_type count, const T &value = T(), const Allocator &alloc = Allocator()) {
    if (count <= N) {
      stack_.fill(value);
    } else {
      // use heap
      heap_ = std::move(std::vector<T>(count, value, alloc));
    }
    size_ = count;
  }

  small_vector(const small_vector &other, const Allocator &alloc = Allocator())
      : stack_(other.stack_), heap_(other.heap_, alloc), size_(other.size_) {}

  small_vector(small_vector &&other, const Allocator &alloc = Allocator())
      : stack_(std::move(other.stack_)), heap_(std::move(other.heap_), alloc),
        size_(other.size_) { other.size_ = 0; }

  small_vector(std::initializer_list<T> initlist, const Allocator &alloc = Allocator()) {
    const auto input_size = initlist.size();
    if (input_size <= N) {
      std::copy(initlist.begin(), initlist.end(), stack_.begin());
    } else {
      heap_ = std::move(std::vector<T>(initlist, alloc));
    }
    size_ = input_size;
  }

  small_vector &operator=(const small_vector &rhs) {
    stack_ = rhs.stack_;
    heap_ = rhs.heap_;
    size_ = rhs.size_;
    return *this;
  }

  small_vector &operator=(small_vector &&rhs) {
    if (rhs.size_ <= N)
     std::move(rhs.stack_.begin(), rhs.stack_.begin() + rhs.size_, stack_.begin());
    else
     heap_ = std::move(rhs.heap_);
    size_ = rhs.size_;
    rhs.size_ = 0;
    return *this;
  }

  small_vector &operator=(std::initializer_list<value_type> rhs) {
    if (rhs.size() <= N) {
      stack_ = rhs;
    } else {
      heap_ = rhs;
    }
    size_ = rhs.size();
  }

  allocator_type get_allocator() const noexcept { return heap_.get_allocator(); }

  reference at(size_type pos) {
    if (size_ <= N) {
      return stack_.at(pos);
    } else {
      return heap_.at(pos);
    }
  }

  const_reference at(size_type pos) const {
    if (size_ <= N) {
      return stack_.at(pos);
    } else {
      return heap_.at(pos);
    }
  }

  reference operator[](size_type pos) {
    if (size_ <= N) {
      return stack_[pos];
    } else {
      return heap_[pos];
    }
  }

  const_reference operator[](size_type pos) const {
    if (size_ <= N) {
      return stack_[pos];
    } else {
      return heap_[pos];
    }
  }

  reference front() {
    if (size_ <= N) {
      return stack_.front();
    } else {
      return heap_.front();
    }
  }

  const_reference front() const {
    if (size_ <= N) {
      return stack_.front();
    } else {
      return heap_.front();
    }
  }

  reference back() {
    if (size_ <= N) {
      return stack_[size_ - 1];
    } else {
      return heap_[size_ - 1];
    }
  }

  const_reference back() const {
    if (size_ <= N) {
      return stack_[size_ - 1];
    } else {
      return heap_[size_ - 1];
    }
  }

  pointer data() noexcept {
    if (size_ <= N) {
      return stack_.data();
    } else {
      return heap_.data();
    }
  }

  const_pointer data() const noexcept {
    if (size_ <= N) {
      return stack_.data();
    } else {
      return heap_.data();
    }
  }

  bool empty() const { return size_ == 0; }

  size_type size() const { return size_; }

  void shrink_to_fit() {
    if (heap_.size() > 0) {
      heap_.shrink_to_fit();
    }
  }

  void push_back(const T &value) {
    if (size_ < N) {
      stack_[size_++] = value;
    } else {
      if (size_++ == N) {
        // move everything to heap
        std::move(stack_.begin(), stack_.end(), std::back_inserter(heap_));
      }
      heap_.push_back(value);
    }
  }

  void push_back(T &&value) {
    if (size_ < N) {
      stack_[size_++] = std::move(value);
    } else {
      if (size_++ == N) {
        // move everything to heap
        std::move(stack_.begin(), stack_.end(), std::back_inserter(heap_));
      }
      heap_.emplace_back(std::move(value));
    }
  }

  void pop_back() {
    if (size_-- > N) {
      // currently using heap
      heap_.pop_back();
      // now check if all data can fit on stack
      // if so, move back to stack
      if (size_ == N) {
        std::move(heap_.begin(), heap_.end(), stack_.begin());
        heap_.clear();
      }
    }
  }

  void clear()
  {
      if (heap_.size() > 0)
          heap_.clear();
      size_ = 0;
  }

  void swap(small_vector &other) noexcept {
    std::swap(stack_, other.stack_);
    std::swap(heap_, other.heap_);
    std::swap(size_, other.size_);
  };

  class iterator {
    pointer ptr_;

  public:
    typedef iterator self_type;
    typedef T value_type;
    typedef T &reference;
//    typedef T *pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    iterator(pointer ptr) : ptr_(ptr) {}
    self_type operator++() {
      ptr_++;
      return *this;
    }
    self_type operator++(int) {
      self_type i = *this;
      ptr_++;
      return i;
    }
    reference operator*() { return *ptr_; }
    pointer operator->() { return ptr_; }
    bool operator==(const self_type &rhs) { return ptr_ == rhs.ptr_; }
    bool operator!=(const self_type &rhs) { return ptr_ != rhs.ptr_; }
  };

  class const_iterator {
    const_pointer ptr_;

  public:
    typedef const_iterator self_type;
    typedef T value_type;
    typedef T &reference;
    //typedef T *pointer;
    typedef int difference_type;
    typedef std::forward_iterator_tag iterator_category;
    const_iterator(const_pointer ptr) : ptr_(ptr) {}
    self_type operator++() {
      ptr_++;
      return *this;
    }
    self_type operator++(int) {
      self_type i = *this;
      ptr_++;
      return i;
    }
    const value_type &operator*() { return *ptr_; }
    const pointer operator->() { return ptr_; }
    bool operator==(const self_type &rhs) { return ptr_ == rhs.ptr_; }
    bool operator!=(const self_type &rhs) { return ptr_ != rhs.ptr_; }
  };

  iterator begin() {
    if (size_ <= N) {
      return iterator(stack_.data());
    } else {
      return iterator(heap_.data());
    }
  }

  iterator end() {
    if (size_ <= N) {
      return iterator(stack_.data() + size_);
    } else {
      return iterator(heap_.data() + size_);
    }
  }

  const_iterator begin() const {
    if (size_ <= N) {
      return const_iterator(stack_.data());
    } else {
      return const_iterator(heap_.data());
    }
  }

  const_iterator end() const {
    if (size_ <= N) {
      return const_iterator(stack_.data() + size_);
    } else {
      return const_iterator(heap_.data() + size_);
    }
  }
};

} // namespace sv
