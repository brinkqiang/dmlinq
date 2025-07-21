#include "dmlinq.h"

int main(int argc, char* argv[]) {
    Idmlinq* module = dmlinqGetModule();
    if (module) {
        module->Release();
    }
    return 0;
}