#pragma once

#include <chrono>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

using uchar = unsigned char;

class Timer {
public:
    Timer(std::string_view message = "", std::ostream &out_stream = std::cerr) : 
        message(message),
        out_stream(out_stream),
        start(std::chrono::high_resolution_clock::now())
    {
    }

    ~Timer() {
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        out_stream << message << duration.count() << " microseconds\n";
    }

private:
    std::string_view message; 
    std::ostream& out_stream;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

template <typename Out, typename In>
Out saturate_cast(In x) {
    if (x < std::numeric_limits<Out>::min())
        return std::numeric_limits<Out>::min();
    if (x > std::numeric_limits<Out>::max())
        return std::numeric_limits<Out>::max();
    return x;
}


// opencv BORDER_REFLECT_101 behavior
int reflect(int val, int len) {
    if (val < 0) 
        return -val;
    if (val >= len)
        return len - (val - len) - 2;
    return val;
}
