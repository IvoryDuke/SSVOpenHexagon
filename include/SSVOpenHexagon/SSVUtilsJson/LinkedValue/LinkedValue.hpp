// Copyright (c) 2013-2020 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: https://opensource.org/licenses/AFL-3.0

#pragma once

#include "SSVOpenHexagon/SSVUtilsJson/Utils/Main.hpp"

namespace ssvuj
{

template <typename T>
class LinkedValue
{
private:
    const char* name;
    T value;

public:
    constexpr explicit LinkedValue(const char* mLinkedName) : name{mLinkedName}
    {
    }

    [[nodiscard]] operator T&() noexcept
    {
        return value;
    }

    [[nodiscard]] operator const T&() const noexcept
    {
        return value;
    }

    auto& operator=(const T& mValue)
    {
        value = mValue;
        return *this;
    }

    void syncFrom(const Obj& mObj)
    {
        extr(mObj, name, value);
    }

    void syncTo(Obj& mObj) const
    {
        arch(mObj, name, value);
    }
};

} // namespace ssvuj
