#include "FixedPoint.h"
#include "FloatingPoint.h"

void RunProgram(int argc, char** argv) {

    if (argv[1][0] == 'h' || argv[1][0] == 'f') {
        RunFloatingPoint(argc, argv);
    } else {
        RunFixedPoint(argc, argv);
    }

}


int main(int argc, char** argv) {
    RunProgram(argc, argv);
    return 0;
}