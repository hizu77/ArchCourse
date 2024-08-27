#include "Calculator.h"


int main(int argc, char** argv) {
    Options options = Parse(argc, argv);
    Result result = Calculate(options);
    PrintResult(options.output, result);
    return 0;
}
