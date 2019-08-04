#include "shared.h"

const int RWCreateMode = 0666;

std::vector<std::string> tags()
{
    return {"<write>", "<create>"};
}

std::string defaultFifoName()
{
    return "/tmp/myfifo";
}

std::string exitMessage()
{
    return "Write \"exit\" to escape";
}
std::string exitDefine()
{
    return "exit";
}

std::string pipeWritingError()
{
    return "Cannot write to pipe";
}

std::string pipeReadingError()
{
    return "Cannot read from pipe";
}

int openFifo(const std::string& fifoName, const int& openMode)
{
    int fd = open(fifoName.c_str(), openMode);

    if (fd == -1)
        throw std::runtime_error("Cannot open " + fifoName);

    return fd;
}

int createFifo(const std::string& fifoName, const int& createMode, const int& openMode)
{
    mkfifo(fifoName.c_str(), createMode);
    return openFifo(fifoName, openMode);
}