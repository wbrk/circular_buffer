#include "circularbuffer.h"

#include <assert.h>
#include <string.h>

#include <algorithm>

bool CircularBuffer::overwritten() {
    bool rv = mOverwritten;
    mOverwritten = false;
    return rv;
}

size_t CircularBuffer::write(const unsigned char *data, size_t length) {
    assert(data);

    if (length > size()) {
        mOverwritten = (mAvailable != 0);
        mReadPtr = mWritePtr = 0;
        mAvailable = size();

        std::copy((data + length) - size(), data + length, mStorage.begin());
        return mAvailable;
    }

    if (mWritePtr + length > size()) { // Buffer will wrap on write
        auto firstChunkLength = size() - mWritePtr;
        auto secondChunkLength = length - firstChunkLength;

        memcpy(&mStorage[mWritePtr], data, firstChunkLength);
        memcpy(&mStorage[0], data + firstChunkLength, secondChunkLength);
    } else {
        memcpy(&mStorage[mWritePtr], data, length);
    }

    mWritePtr = (mWritePtr + length) % size();
    mAvailable += length;

    // Buffer overflow: move mReadPtr further for amount of bytes that don't
    // fit into buffer.
    if (mAvailable > size()) {
        mReadPtr = (mReadPtr + mAvailable - size()) % size();
        mOverwritten = true;
        mAvailable = size();
    }

    assert(mAvailable <= size());
    assert(mReadPtr < size());
    assert(mWritePtr < size());
    return length;
}

size_t CircularBuffer::read(unsigned char *data, size_t length) {
    assert(data);

    mOverwritten = false;
    length = std::min(length, mAvailable);

    if (mReadPtr + length > size()) { // Buffer will wrap on read
        auto firstChunkLength = size() - mReadPtr;
        auto secondChunkLength = length - firstChunkLength;

        memcpy(data, &mStorage[mReadPtr], firstChunkLength);
        memcpy(data + firstChunkLength, &mStorage[0], secondChunkLength);
    } else {
        memcpy(data, &mStorage[mReadPtr], length);
    }

    mReadPtr = (mReadPtr + length) % size();
    mAvailable -= length;

    assert(mAvailable <= size());
    assert(mReadPtr < size());
    assert(mWritePtr < size());
    return length;
}

size_t CircularBuffer::discard(size_t length) {
    mOverwritten = false;
    length = std::min(length, mAvailable);

    mReadPtr = (mReadPtr + length) % size();
    mAvailable -= length;

    assert(mAvailable <= size());
    assert(mReadPtr < size());
    assert(mWritePtr < size());
    return length;
}

int CircularBuffer::search(const unsigned char *data, size_t length) const {
    assert(data);

    if (length > available()) {
        return -1;
    }

    for (size_t i = 0; i <= (available() - length); ++i) {
        bool matches = true;
        for (size_t j = 0; j < length; ++j) {
            if (at(i + j) != data[j]) {
                matches = false;
                break;
            }
        }

        if (matches) {
            return i;
        }
    }

    return -1;
}

unsigned char &CircularBuffer::operator[](int i) {
    const CircularBuffer &thizz = static_cast<const CircularBuffer &>(*this);
    return const_cast<unsigned char &>(thizz[i]);
}

const unsigned char &CircularBuffer::operator[](int i) const {
    assert(mAvailable > 0);
    assert(i >= 0);
    assert((unsigned) i < mAvailable);

    i = (mReadPtr + i) % size();
    return mStorage[i];
}
