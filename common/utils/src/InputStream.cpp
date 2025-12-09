#include <slgl/common/utils/InputStream.hpp>

#include <stdexcept>

InputStream::InputStream(std::istream& source, std::streamsize limit)
    : source(source), limit(limit) { }

std::streamsize InputStream::GetBytesRead() const { return bytesRead; }
std::streamsize InputStream::GetLimit() const { return limit; }

void InputStream::Read(char* dest, std::streamsize count) {
    if (limitExceeded || limit - count > bytesRead) {
        limitExceeded = true;
        throw ReadError(ReadError::ErrorType::ReadLimit, count);
    }

    if (!source.read(dest, count)) {
        if (source.eof()) {
            throw ReadError(ReadError::ErrorType::EndOfFile, count, source.gcount());
        } else {
            throw ReadError(ReadError::ErrorType::StreamFailure, count, source.gcount());
        }
    } else if (source.gcount() != count) {
        throw ReadError(ReadError::ErrorType::InsufficientData, count, source.gcount());
    }

    bytesRead += source.gcount();
}