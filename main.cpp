
#include "FileSubscriber.h"
#include "CoutSubscriber.h"
#include "CommandPackage.h"


int main(int argc, char *argv[])
{
    auto blockCommandCount = 0;
    std::string command;

    if( argc > 1 )
    {
        blockCommandCount =  std::atoi(argv[1]);
    }
    else
    {
        blockCommandCount = 3;
    }

    auto commandPackage = std::make_shared<CommandPackage>(blockCommandCount);

    auto fileSubscriber = std::make_shared<FileSubscriber<2>>();
    auto coutSubscriber = std::make_shared<CoutSubscriber<1>>();

    commandPackage->addSubscriber(fileSubscriber);
    commandPackage->addSubscriber(coutSubscriber);

    while( std::getline(std::cin, command) && command.size() > 0)
    {
        commandPackage->addCommand(command);
    }

    commandPackage->update();

    commandPackage->printMetrix();
    coutSubscriber->printMetrix();
    fileSubscriber->printMetrix();

    return 0;
}