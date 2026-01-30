#include <iostream>
#include <DSA/Array.h>
using namespace DSA;

// TBD: a simple test struct delete later... ... 

int main() {
    std::printf("Test CArray<float> \n");
    CArray<float> cArr;
    cArr.resize(5);
    for (int i = 0; i < cArr.size(); ++i) {
        cArr[i] = i * 10.12f;
    }
    std::cout << "Array contents: ";
    for (int i = 0; i < cArr.size(); ++i) {
        std::cout << cArr[i] << " ";
    }
    std::cout << std::endl;

    
    std::printf("Test Array<int> \n");
    Array<int> arr;
    arr.resize(5);
    for (int i = 0; i < arr.size(); ++i) {
        arr[i] = i * 10;
    }
    std::cout << "Array contents: ";
    for (int i = 0; i < arr.size(); ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
