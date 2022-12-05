#include "Date.h"
#include <iomanip>
#include <iostream>
//minirisk::Date;

using namespace minirisk;

void test1(const int seed)
{
    srand(seed); //set seed

    int d,m,y,leap;
    for (int i = 0; i < 1000; i++) {
        y = rand() % 1500 + 1000;
        m = rand() % 15+1;
        leap = (y % 4 != 0) ? false : (y % 100 != 0) ? true : (y % 400 != 0) ? false : true;
        
        if (y > 2200 || y < 1900 || m > 12) {
            d = rand() % 31;
        }
        else {
            switch (m) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                d = rand() % 3 + 31;
            case 2:
                if (leap) {
                    d = rand() % 3 + 29;
                }
                else {
                    d = rand() % 3 + 28;
                }
            default:
                d = rand() % 3 + 30;
            }     
        }    
        try {
            Date a(y, m, d);
        }
        catch (...) {
            std::cout << "Error in Date: " << y << "-" << m << "-" << d << std::endl;
        }
    } 
    std::cout << "Test 1 finish" << std::endl;
}

void test2()
{
    int d, m, y, leap, tmp;
    std::string b,c;
    
    for (y = 1900; y <= 2199; y++) {
        leap = (y % 4 != 0) ? false : (y % 100 != 0) ? true : (y % 400 != 0) ? false : true;

        for (m = 1; m <= 12; m++) {

            if (m == 2) {
                tmp = 28+leap;
            }
            else if (m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12) {
                tmp = 31;
            }
            else {
                tmp = 30;
            }

            for (d = 1; d <= tmp; d++) {
                Date a(y, m, d);
                b =  a.to_string(false);
                std::ostringstream osm;
                std::ostringstream osd;
                osm << std::setw(2) << std::setfill('0') << static_cast<unsigned>(m);
                osd << std::setw(2) << std::setfill('0') << static_cast<unsigned>(d);

                c = std::to_string(y) + osm.str() + osd.str();

                MYASSERT(!c.compare(b), "Conversion fail for: " << c << " and " << b);
                
            }
            
        }
    }
    std::cout << "Test 2 finish" << std::endl;
}

void test3()
{
    int d, m, y, leap, cur_month, tmp = -1;

    for (y = 1900; y <= 2199; y++) {
        leap = (y % 4 != 0) ? false : (y % 100 != 0) ? true : (y % 400 != 0) ? false : true;

        for (m = 1; m <= 12; m++) {

            if (m == 2) {
                cur_month = 28 + leap;
            }
            else if (m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12) {
                cur_month = 31;
            }
            else {
                cur_month = 30;
            }

        
            for (d = 1; d <= cur_month; d++) {
                Date a(y, m, d);
                MYASSERT(!(a.get_serial() - tmp != 1), "Error in neighbour date: " << a.get_serial() << tmp);
                tmp += 1;

            }

        }
        
    }
    std::cout << "Test 3 finish" << std::endl;
}


int main()
{
    const int seed = 1;
    test1(seed);
    test2();
    test3();
    std::cout << "Success" << std::endl; 
    std::cout << "Seed used: " << seed << std::endl;
    return 0;
}

