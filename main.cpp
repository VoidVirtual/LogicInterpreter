#include <iostream>
#include <fstream>
#include "Interpreter.h"
#include "Parser.h"
int main(int argc, char* argv[])
{
    std::ifstream in("input.txt");
    Interpreter I;
    I.ScanFacts(in);
    I.ScanRules(in);
    I.UpdateFacts();
    in.close();
    I.PrintNewFacts();
}
