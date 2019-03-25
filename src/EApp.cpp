#include "EApp.h"

const char *EApp::what() const noexcept
{
    return "An error occurred while executing the application";
}
