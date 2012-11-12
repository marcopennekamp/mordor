#ifndef MORDOR_INTERNAL_UTILS_ARRAY_H_
#define	MORDOR_INTERNAL_UTILS_ARRAY_H_

#include <mordor/def.h>


namespace mordor {

template<typename T>
class Array {
  private:
    T* array_;
    size_t size_;

  public:
    explicit Array (size_t size) {
        array_ = new T[size];
        size_ = size;
    }

    inline ~Array () {
        delete[] array_;
    }

    inline void Init (T value) {
        for (size_t i = 0; i < size_; ++i) {
            this[i] = value;
        }
    }

    inline void SetMemory (mordor_u8 value) {
        memset (array_, value, size_ * sizeof (T));
    }

    inline T& operator[] (size_t index) const {
        return array_[index];
    }

    inline T* array () const {
        return array_;
    }

    inline size_t size () const {
        return size_;
    }
};

}


#endif  /* MORDOR_INTERNAL_UTILS_DYNAMICARRAY_H_ */