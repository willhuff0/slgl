#pragma once

#include <istream>
#include <ios>
#include <limits>
#include <string>

class ReadError : public std::exception {
public:
    enum class ErrorType {
        EndOfFile,
        InsufficientData,
        StreamFailure,
        ReadLimit,
    };

private:
    ErrorType type_;
    std::string message_;

public:
    explicit ReadError(ErrorType type, size_t expected, size_t actual = 0) : type_(type) {
        std::string typeStr;
        switch(type) {
            case ErrorType::EndOfFile: typeStr = "End of file"; break;
            case ErrorType::InsufficientData: typeStr = "Insufficient data"; break;
            case ErrorType::StreamFailure: typeStr = "Stream failure"; break;
            case ErrorType::ReadLimit: typeStr = "Read limit"; break;
        }

        message_ = typeStr;
        message_ += " (expected " + std::to_string(expected) + " bytes";
        if (actual > 0) message_ += ", got " + std::to_string(actual) + " bytes";
        message_ += ")";
    }

    [[nodiscard]] const char* what() const noexcept override {
        return message_.c_str();
    }

    [[nodiscard]] ErrorType type() const noexcept {
        return type_;
    }
};

class InputStream {
public:
    InputStream(std::istream& source, std::streamsize limit = std::numeric_limits<std::streamsize>::max());

    std::streamsize GetBytesRead() const;
    std::streamsize GetLimit() const;

    void Read(char* dest, std::streamsize count);

private:
    std::istream& source;
    std::streamsize limit;
    std::streamsize bytesRead = 0;
    bool limitExceeded = false;
};