#pragma once

namespace character
{
    inline bool HasDoubleByteString(std::string str)
    {
        while (!str.empty())
        {
            if (IsDBCSLeadByte(str[0]) != 0)
            {
                return true;
            }
            str.erase(0, 1);
        }

        return false;
    }

    inline bool IsOnlyNumberString(std::string& str)
    {
        return std::all_of(str.cbegin(), str.cend(), isdigit);
    }
}