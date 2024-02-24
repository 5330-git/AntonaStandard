#include <iostream>
#include <vector>
#include "Utilities/Antona_foreach.h"
#include <CPS/Macros.h>
using namespace std;
using namespace AntonaStandard::Utilities;
int main() {
    int a[] = {1,2,3};
    const char* s[] = {"hello","world!","!"};
    vector<int> b = {1,2,3};
    for(auto i:a){
        cout<<i<<" ";
    }
    cout<<endl;
    for(auto i:r_wrap(a)){
        cout<<i<<" ";
    }
    cout<<endl;
    for(auto i:r_wrap(b)){
        cout<<i<<" ";
    }
    cout<<endl;
    for(auto i:r_wrap(s)){
        cout<<i<<" ";
    }
    cout<<endl;
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        system("pause");
    #endif
	return 0;   
}
/*
1 2 3 
3 2 1
3 2 1
! world! hello
*/