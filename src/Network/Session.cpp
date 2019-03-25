#include <utility>

#include "Session.h"
#include "Exceptions/ESessionUserIsNull.h"

Session::Session(std::string token, User user, App &app)
    : mToken{std::move(token)},
      mUser{std::move(user)}
{
    if (!mUser) {
        throw ESessionUserIsNull{"the session object must have non-null user/owner"};
    }
}

std::string Session::GetToken()
{
    return mToken;
}

std::chrono::system_clock::time_point Session::GetLastActiveAt()
{
    return mLastActiveAt;
}

void Session::SetLastActiveAt(std::chrono::system_clock::time_point timePoint)
{
    mLastActiveAt = timePoint;
}
