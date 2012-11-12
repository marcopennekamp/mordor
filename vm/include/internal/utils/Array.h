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
    inline Array () { }

    inline explicit Array (const size_t size) {
        Create (size);
    }

    inline ~Array () {
        Delete ();
    }

    inline void Create (const size_t size) {
        array_ = new T[size];
        size_ = size;
    }

    inline void Delete () {
        delete[] array_;
    }

    inline void Set (const T value) {
        for (size_t i = 0; i < size_; ++i) {
            this[i] = value;
        }
    }

    inline void SetMemory (const mordor_u8 value) {
        memset (array_, value, size_ * sizeof (T));
    }

    inline T& operator[] (const size_t index) const {
        return array_[index];
    }

    inline T* array () const {
        return array_;
    }

    inline const size_t size () const {
        return size_;
    }
};

}


#endif  /* MORDOR_INTERNAL_UTILS_ARRAY_H_ */