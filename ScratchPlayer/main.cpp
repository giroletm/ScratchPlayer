#include <iostream>

#include "Executor.h"

int main(int argc, char* argv[])
{
    Executor *exec = new Executor("Test3.sb3");

    delete exec;

    return 0;
}