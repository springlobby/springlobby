#ifndef SPRINGLOBBY_HEADERGUARD_MATH_H
#define SPRINGLOBBY_HEADERGUARD_MATH_H

#include <algorithm>

template <typename T> T clamp(const T var,const T min,const T max)
{
    return ( (var < min) ? min : ( var > max ) ? max : var );
}

template<typename T>
T min(T a, T b, T c)
{
    return std::min(a, std::min(b, c));
}

#endif // SPRINGLOBBY_HEADERGUARD_MATH_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/
