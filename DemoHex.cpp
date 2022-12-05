#include <iostream>
#include <iomanip>
#include <cstdint>
using namespace std;


int main()
{
    union { double d; uint64_t u; } tmp;
    double x = -0.15625;
    tmp.d = x;
    cout << hex << tmp.u << endl;
    return 0;
}


