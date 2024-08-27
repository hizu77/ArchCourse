#include "ProgramController.h"
using namespace std;
int main(int argc, char** argv)
{
    ProgramController controller;
    controller.GetParameters(argc, argv);
    controller.RunProgram();
    return 0;
}