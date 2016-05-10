#pragma once

#include <pthread.h>
#include <utility>

namespace cs540 {
  // Ref count object
  struct Ref {
    Ref() : count(1), lock(PTHREAD_MUTEX_INITIALIZER) {}
    virtual ~Ref() {
      pthread_mutex_unlock(&lock);
    };
    unsigned count;
    pthread_mutex_t lock;
  };
  template <typename U>
  struct RefImpl : Ref {
    RefImpl():
      Ref(),
      ptr_(nullptr) {}
    RefImpl(U *p):
      Ref(),
      ptr_(p) {}
    ~RefImpl() {
      delete ptr_;
    }
    RefImpl(const RefImpl &) = delete;
    RefImpl(RefImpl &&) = delete;
    RefImpl& operator=(const RefImpl &) = delete;
    RefImpl& operator=(RefImpl &&) = delete;
    U *ptr_;
  };

  template <typename T>
  class SharedPtr {
  public:
    // Constructors
    SharedPtr();
    template <typename U>
    explicit SharedPtr(U *);

    SharedPtr(const SharedPtr &);
    template <typename U>
    SharedPtr(const SharedPtr<U> &);

    SharedPtr(SharedPtr &&);
    template <typename U>
    SharedPtr(SharedPtr<U> &&);

    SharedPtr &operator=(const SharedPtr &);
    template <typename U>
    SharedPtr<T> &operator=(const SharedPtr<U> &);

    SharedPtr &operator=(SharedPtr &&);
    template <typename U>
    SharedPtr<T> &operator=(SharedPtr<U> &&);

    ~SharedPtr();

    // Modifiers
    void reset();

    template <typename U>
    void reset(U *p);

    // Observers
    T *get() const;
    T &operator*() const;
    T *operator->() const;

    explicit operator bool() const {
      return ptr_ != nullptr;
    }

    // Friends
    template <typename U>
    friend class SharedPtr;

    template <typename V, typename U>
    friend SharedPtr<V> dynamic_pointer_cast(const SharedPtr<U> &sp);

  private:
    // Ref count operators
    void up();
    void down();

  private:
  private:
    Ref *ref_;
    T *ptr_;
  };

  template <typename T>
  SharedPtr<T>::SharedPtr():
    ref_(nullptr),
    ptr_(nullptr) {}

  template <typename T>
  template <typename U>
  SharedPtr<T>::SharedPtr(U *up):
    ref_(new RefImpl<U>(up)),
    ptr_(static_cast<T *>(up)) {}

  template <typename T>
  SharedPtr<T>::SharedPtr(const SharedPtr &sp):
    ref_(sp.ref_),
    ptr_(sp.ptr_) {
    up();
  }

  template <typename T>
  template <typename U>
  SharedPtr<T>::SharedPtr(const SharedPtr<U> &sp):
    ref_(sp.ref_),
    ptr_(static_cast<T *>(sp.ptr_)) {
    up();
  }

  template <typename T>
  SharedPtr<T>::SharedPtr(SharedPtr &&sp) :
    ref_(std::move(sp.ref_)),
    ptr_(std::move(sp.ptr_)) {
    sp.ref_ = nullptr;
    sp.ptr_ = nullptr;
  }

  template <typename T>
  template <typename U>
  SharedPtr<T>::SharedPtr(SharedPtr<U> &&sp):
    ref_(std::move(sp.ref_)),
    ptr_(static_cast<T *>(std::move(sp.ptr_))) {
    sp.ref_ = nullptr;
    sp.ptr_ = nullptr;
  }

  template <typename T>
  SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr &sp) {
    down();
    ref_ = sp.ref_;
    ptr_ = sp.ptr_;
    up();
    return *this;
  }

  template <typename T>
  template <typename U>
  SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<U> &sp) {
    if (dynamic_cast<T *>(sp.ptr_) == ptr_) return *this;

    down();
    ref_ = sp.ref_;
    ptr_ = static_cast<T *>(sp.ptr_);
    up();
    return *this;
  }

  template <typename T>
  SharedPtr<T> &SharedPtr<T>::operator=(SharedPtr &&sp) {
    if (&sp == this) return *this;

    down();
    ref_ = std::move(sp.ref_);
    ptr_ = std::move(sp.ptr_);
    sp.ref_ = nullptr;
    sp.ptr_ = nullptr;
    return *this;
  }

  template <typename T>
  template <typename U>
  SharedPtr<T> &SharedPtr<T>::operator=(SharedPtr<U> &&sp) {
    if (&sp == this) return *this;

    down();
    ref_ = std::move(sp.ref_);
    ptr_ = static_cast<T *>(std::move(sp.ptr_));
    sp.ref_ = nullptr;
    sp.ptr_ = nullptr;
    return *this;
  }

  template <typename T>
  SharedPtr<T>::~SharedPtr() {
    down();
  }

  // Modifiers

  template <typename T>
  void SharedPtr<T>::reset() {
    down();
    ref_ = nullptr;
    ptr_ = nullptr;
  }

  template <typename T>
  template <typename U>
  void SharedPtr<T>::reset(U *p) {
    down();
    ref_ = new RefImpl<U>(p);
    ptr_ = static_cast<T *>(p);
  }

  // Observers

  template <typename T>
  T *SharedPtr<T>::get() const {
    return ptr_;
  }

  template <typename T>
  T &SharedPtr<T>::operator*() const {
    return *ptr_;
  }

  template <typename T>
  T *SharedPtr<T>::operator->() const {
    return ptr_;
  }

  // Friends
  template <typename T1, typename T2>
  bool operator==(const SharedPtr<T1> &a, const SharedPtr<T2> &b) {
    return a.get() == b.get();
  }

  template <typename T>
  bool operator==(const SharedPtr<T> &a, std::nullptr_t) {
    return a.get() == nullptr;
  }

  template <typename T>
  bool operator==(std::nullptr_t, const SharedPtr<T> &a) {
    return a.get() == nullptr;
  }

  template <typename T1, typename T2>
  bool operator!=(const SharedPtr<T1> &a, const SharedPtr<T2> &b) {
    return !(a == b);
  }

  template <typename T>
  bool operator!=(const SharedPtr<T> &a, std::nullptr_t b) {
    return !(a == b);
  }

  template <typename T>
  bool operator!=(std::nullptr_t a, const SharedPtr<T> &b) {
    return !(a == b);
  }

  template <typename T, typename U>
  SharedPtr<T> static_pointer_cast(const SharedPtr<U> &sp) {
    return SharedPtr<T>(sp);
  }

  template <typename T, typename U>
  SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &sp) {
    SharedPtr<T> osp;
    osp.ptr_ = dynamic_cast<T *>(sp.ptr_);
    if (osp) {
      osp.ref_ = sp.ref_;
      osp.up();
    }
    return osp;
  }

  template <typename T>
  void SharedPtr<T>::up() {
    if (ptr_ && ref_) {
      pthread_mutex_lock(&ref_->lock);
      ref_->count++;
      pthread_mutex_unlock(&ref_->lock);
    }
  }

  template <typename T>
  void SharedPtr<T>::down() {
    if (ptr_ && ref_) {
      pthread_mutex_lock(&ref_->lock);
      ref_->count--;
      if (ref_->count == 0) {
        delete ref_;
        ref_ = nullptr;
        ptr_ = nullptr;
        return;
      }
      pthread_mutex_unlock(&ref_->lock);
    }
  }
}
