#include <stdio.h>
#include <mama/mamacpp.h>

using namespace Wombat;

int main(int argc, char* argv[])
{
    mama_status status;

    mamaBridge bridge = Mama::loadBridge ("qpid");

    printf("MAMA VERSION: %s\n", Mama::getVersion(bridge));
}
