#ifndef __EL_SMART_PTR_HEADER_H__
#define __EL_SMART_PTR_HEADER_H__


namespace el {

// abstruct base class for destructor of smart pointer
class RefPointer : private UnCopyable {
public:
  RefPointer(void) {}
  virtual ~RefPointer(void) {}

  virtual void Destroy(void) = 0;
};


template <typename T>
class RefPtrDelete : public RefPointer {
  T* ptr_;
public:
  explicit RefPtrDelete(T* p) 
    : ptr_(p) {
  }

  ~RefPtrDelete(void) {
  }

  virtual void Destroy(void) override {
    if (nullptr != ptr_)
      delete ptr_;
  }
};

template <typename T, typename D> 
class RefPtrDestructor : public RefPointer {
  T* ptr_;
  D  dtor_;
public:
  explicit RefPtrDestructor(T* p, D d) 
    : ptr_(p) 
    , dtor_(d) {
  }

  ~RefPtrDestructor(void) {
  }

  virtual void Destroy(void) override {
    if (nullptr != ptr_)
      dtor_(ptr_);
  }
};



// SmartPtr
//
// An enhanced relative of smart pointer with reference 
// counted copy semantics.
//
// This object pointed to is deleted when the last SmartPtr 
// pointing to it is destroyed reset.
template <typename T, typename Locker = DummyLock> 
class SmartPtr {
  T*                  ptr_;
  RefPointer*         ref_ptr_;
  RefCounter<Locker>* ref_count_;

  typedef SmartPtr<T, Locker> SelfType;
public:
  SmartPtr(void) 
    : ptr_(nullptr) 
    , ref_ptr_(nullptr) 
    , ref_count_(nullptr) {
  }

  template <typename Y>
  explicit SmartPtr(Y* p) 
    : ptr_(p) 
    , ref_ptr_(new RefPtrDelete<T>(ptr_)) 
    , ref_count_(new RefCounter<Locker>(1)) {
  }

  template <typename Y, typename D> 
  explicit SmartPtr(Y* p, D d) 
    : ptr_(p) 
    , ref_ptr_(new RefPtrDestructor<T, D>(ptr_, d))
    , ref_count_(new RefCounter<Locker>(1)) {
  }

  ~SmartPtr(void) {
    if (nullptr != ref_count_) {
      if (0 == --*ref_count_) {
        ref_ptr_->Destroy();
        ptr_ = nullptr;

        delete ref_ptr_;
        ref_ptr_ = nullptr;

        delete ref_count_;
        ref_count_ = nullptr;
      }
    }
  }

  SmartPtr(const SmartPtr& x) 
    : ptr_(x.ptr_) 
    , ref_ptr_(x.ref_ptr_) 
    , ref_count_(x.ref_count_) {
    if (nullptr != ref_count_)
      ++*ref_count_;
  }

  template <typename Y>
  SmartPtr(const SmartPtr<Y, Locker>& x) 
    : ptr_(x.Get()) 
    , ref_ptr_(x.GetRefPointer())
    , ref_count_(x.GetRefCounter()) {
    if (nullptr != ref_count_)  
      ++*ref_count_;
  }

  SmartPtr& operator=(const SmartPtr& x) {
    if (this != &x)
      SelfType(x).Swap(*this);

    return *this;
  }

  template <typename Y>
  SmartPtr& operator=(const SmartPtr<Y, Locker>& x) {
    if ((void*)this != (void*)&x)
      SelfType(x).Swap(*this);

    return *this;
  }
public:
  inline T* Get(void) const {
    return ptr_;
  }
  
  inline RefPointer* GetRefPointer(void) const {
    return ref_ptr_;
  }

  inline RefCounter<Locker>* GetRefCounter(void) const {
    return ref_count_;
  }

  inline void Reset(void) {
    SelfType().Swap(*this);
  }

  template <typename Y>
  inline void Reset(Y* p) {
    SelfType(p).Swap(*this);
  }

  template <typename Y, typename D>
  inline void Reset(Y* p, D d) {
    SelfType(p, d).Swap(*this);
  }

  inline T& operator*(void) const {
    return *ptr_;
  }

  inline T* operator->(void) const {
    return ptr_;
  }
private:
  void Swap(SmartPtr<T, Locker>& x) {
    std::swap(ptr_, x.ptr_);
    std::swap(ref_ptr_, x.ref_ptr_);
    std::swap(ref_count_, x.ref_count_);
  }
};

template <typename T, typename Y, typename Locker> 
inline bool operator==(
    const SmartPtr<T, Locker>&x, const SmartPtr<Y, Locker>& y) {
  return x.Get() == y.Get();
}

template <typename T, typename Y, typename Locker> 
inline bool operator!=(
    const SmartPtr<T, Locker>& x, const SmartPtr<Y, Locker>& y) {
  return x.Get() != y.Get();
}

}


#endif  // __EL_SMART_PTR_HEADER_H__
