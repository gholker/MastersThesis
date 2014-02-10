#ifndef TOOLS
#define TOOLS

#include <string>
using namespace std;

string int_to_string( int i )
{
    string s = "";
    if( i < 0 )
    {
        s += "-";
        i *= -1;
    }
    int t = i;
    int c = -1;
    while( t > 0 )
    {
        t /= 10;
        c++;
    }
    if( c == -1 )
        s += "0";
    else
    {
        t = 1;
        while( c > 0 )
        {
            t *= 10;
            c--;
        }
        
        while( t > 0 )
        {
            int temp = i / t;
            s += '0' + temp;
            i %= t;
            t /= 10;
        }
    }
    return s;
}

#endif
