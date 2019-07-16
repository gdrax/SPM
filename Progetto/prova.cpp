#include "utils.hpp"
#include <iostream>

int main(int argc, char const *argv[])
{
    for (auto i=0; i<100; i++) {
        cout << random01() << endl;
    }
    return 0;
}
