#include "AppException.h"

AppException::AppException(const std::string &message)
    : std::runtime_error{message}
{}
