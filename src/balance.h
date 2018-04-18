#ifndef BALANCE_H
#define BALANCE_H

#include <sstream>

class Balance
{
public:
    long long value;

    Balance()
        : value(0) {};

    Balance(long long value)
        : value(value) {};

    Balance(std::string input)
    {
        value = 0;

        bool point;
        for (const auto &it: input)
            if ('0' <= it && it <= '9')
                value = value * 10 + (it - '0');
            else if (it == '.')
                point = true;

        if (!point)
            value *= 100;
    }

    std::string toString() const
    {
        std::ostringstream stream;
        stream << value / 100 << "."; 

        if (value % 100 < 10)
            stream << '0';

        stream << value % 100 << '\n';

        return stream.str();
    }
};

#endif
