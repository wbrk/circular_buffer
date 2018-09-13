#ifndef CIRCULARBUFFER_H_INCLUDED
#define CIRCULARBUFFER_H_INCLUDED

#include <stddef.h>

#include <vector>

/*
    Simple circular buffer of bytes backed by a single continuous memory region.
*/
class CircularBuffer {
public:
    CircularBuffer() = delete;
    CircularBuffer(const CircularBuffer &) = delete;
    CircularBuffer(CircularBuffer &&) = delete;

    /*
        Construct a buffer of size 'size'.
    */
    explicit CircularBuffer(size_t size) : mStorage(size, 0) {}

    /*
        Get size of buffer.
    */
    size_t size() const { return mStorage.size(); }

    /*
        Get number of bytes available to be read by read().
    */
    size_t available() const { return mAvailable; }

    /*
        Get number of bytes that buffer is ready to receive by write().
    */
    size_t free() const { return size() - available(); }

    /*
        Check if buffer was overwritten since last read(), discard() or since
        last call to this method. Resets state to not overwritten.
    */
    bool overwritten();

    /*
        Copy 'length' bytes of data from 'data' to buffer. If 'length' exceeds
        size(), buffer will be cleared, last 'size()' bytes of data will be
        copied to the buffer and 'overwritten' flag will be set. If 'length'
        exceeds free() but doesn't exceed 'size()', least recent part of buffer
        will be overwritten with 'data', read pointer will be moved forward
        for length - free() bytes and 'overwritten' flag will be set.

        'data' must be a valid pointer to a memory region.

        Returns number of bytes that were copied to buffer.
    */
    size_t write(const unsigned char *data, size_t length);

    /*
        Copy 'length' bytes of data from buffer to 'data'. If 'length' exceeds
        available()', only available() bytes will be copied to buffer. Resets
        state to not overwritten.

        'data' must be a valid pointer to a memory region.

        Returns number of bytes that were copied from buffer.
    */
    size_t read(unsigned char *data, size_t length);

    /*
        Discard 'length' bytes of data. The effects are same to read(), but no
        data is copied.

        Returns number of bytes that were discarded.
    */
    size_t discard(size_t length);

    /*
        Find the start of the first occurence of substring 'data' of length 'length'
        in the buffer.

        Perfomance notes:
        This method implements naive string-searching algorithm, i.e. time
        complexity is O(nm). This class is written in assumption that 'lenght'
        is small (~2) and needle is located near to beginning of haystack,
        so even naive approach is enough. If plugin prototype will show
        performance slowdown due to this method, consider writing iterator and
        using std::search.

        'data' must be a valid pointer to a memory region.

        Returns offset from beginning of buffer to the first occurence of 'data',
        or -1 if 'length' exceeds 'available' or if the substring is not found.
    */
    int search(const unsigned char *data, size_t length) const;

    /*
        Access item at index position 'i' in the buffer.

        'i' must be a valid index position in the buffer (i.e., 0 <= i < available()).
        No bounds checking is performed.

        Returns reference to an item.
    */
    unsigned char &operator[](int i);
    const unsigned char &operator[](int i) const;
    const unsigned char &at(int i) const { return (*this)[i]; }

private:
    std::vector<unsigned char> mStorage;

    bool mOverwritten = false;
    size_t mReadPtr = 0;
    size_t mWritePtr = 0;
    size_t mAvailable = 0;
};

#endif // CIRCULARBUFFER_H_INCLUDED
