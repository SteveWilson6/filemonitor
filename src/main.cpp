#include <iostream>
#include "../include/filemonitor.hpp"
#include <fmt/core.h>

using namespace std;
using namespace wilson;

int main(int argc, char** argv)
{
    filemonitor fm;
    fm.add_watch("/home/wilson/code_study/cppstudy/filemonitor/test.txt", IN_CLOSE_WRITE);

    auto modified = fm.get_modified();
    if (modified.empty()) 
    {
        fmt::print("{}\n", "nothing modified");
    }
    for (const auto& [name, event, _]: modified)
    {
        (void)_;
        fmt::print("name = {}, event = {}\n", name, to_string(event));
    }

    return 0;
}
