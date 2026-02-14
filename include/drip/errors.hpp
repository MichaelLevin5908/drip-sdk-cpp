#ifndef DRIP_ERRORS_HPP
#define DRIP_ERRORS_HPP

#include <stdexcept>
#include <string>

namespace drip {

/**
 * Base exception for all Drip SDK errors.
 */
class DripError : public std::runtime_error {
public:
    DripError(const std::string& message, int status_code, const std::string& code = "")
        : std::runtime_error(message)
        , status_code_(status_code)
        , code_(code)
    {}

    /** HTTP status code (0 for network/local errors). */
    int status_code() const { return status_code_; }

    /** Error code from the API (e.g., "TIMEOUT", "UNAUTHORIZED"). */
    const std::string& code() const { return code_; }

private:
    int status_code_;
    std::string code_;
};

/**
 * Thrown when the API returns 401 Unauthorized.
 */
class AuthenticationError : public DripError {
public:
    explicit AuthenticationError(const std::string& message = "Invalid or missing API key")
        : DripError(message, 401, "UNAUTHORIZED")
    {}
};

/**
 * Thrown when the API returns 404 Not Found.
 */
class NotFoundError : public DripError {
public:
    explicit NotFoundError(const std::string& message = "Resource not found")
        : DripError(message, 404, "NOT_FOUND")
    {}
};

/**
 * Thrown when the API returns 429 Too Many Requests.
 */
class RateLimitError : public DripError {
public:
    explicit RateLimitError(const std::string& message = "Rate limit exceeded")
        : DripError(message, 429, "RATE_LIMITED")
    {}
};

/**
 * Thrown on request timeout.
 */
class TimeoutError : public DripError {
public:
    explicit TimeoutError(const std::string& message = "Request timed out")
        : DripError(message, 408, "TIMEOUT")
    {}
};

/**
 * Thrown on network/connection errors.
 */
class NetworkError : public DripError {
public:
    explicit NetworkError(const std::string& message = "Network error")
        : DripError(message, 0, "NETWORK_ERROR")
    {}
};

} // namespace drip

#endif // DRIP_ERRORS_HPP
