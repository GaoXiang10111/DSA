#include <iostream>
#include <cstdio>
#include <DSA/DSA.h>
#include <DSA/SharedPtr.h>
#include <DSA/ClassRegistry.h>
#include <DSA/ClassID.h>
using namespace DSA;

using namespace DSA;

// TBD: a simple test struct delete later... ... 
struct DSA_Export GNode
{
    int value;
    GNode() : value(0) {}
    virtual ~GNode() {}
public:
	// ========= Common class interfaces  =========================
	typedef NewClassID<8372,0,DSA::ID_DLL> IDClass;
	typedef GNode BaseClass;
	// Run-time Stream I/O
	bool put(      DSA::Stream& /*stream*/, DSA::UChar /*ver*/) const {return false;}
	bool get(const DSA::Stream& /*stream*/, DSA::UChar /*ver*/)       {return false; }
	// ============================================================
    static DSA::RegClass<GNode> regGNode;
};


int main() {
    std::printf("Hello, World!\n");
    std::cout << "Starts here." << std::endl;
    // Test code to be filled in
    SPtr<GNode> pGNode;
    pGNode.make();
    if (!pGNode.isNull()) {
        std::cout << "GNode instance created successfully." << std::endl;
    } else {
        std::cout << "Failed to create GNode instance." << std::endl;
    }
    std::cout << "Ends here." << std::endl;
    return 0;
}
