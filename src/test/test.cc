#include <iostream>
#include <vector>
#include <cstring>

using namespace std;
#include <boost/function.hpp>
// #include <functional>
#include <boost/bind.hpp>

void print(int i){
    cout<<i<<endl;
}

typedef boost::function <void()> hand;

int main()
{

    return 0;
}
