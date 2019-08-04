#include "shared/shared.h"

bool stop = false;

void manageUserInput(const int writeFifoFd, const std::string& userName)
{
    std::string input;
    std::cout << exitMessage() << std::endl;

    while (std::getline(std::cin, input))
    {
        if (input == exitDefine())
        {
            break;
        }
        else
        {
            auto size = tags()[Write].length() * 2 + userName.length();

            if (input.length() > PIPE_BUF - size)
                input.resize(PIPE_BUF - size);

            std::string newMessage = tags()[Write] + userName + tags()[Write] + input;

            if (write(writeFifoFd, newMessage.c_str(), newMessage.length()) == -1)
            {
                break;
            }
        }
    }

    stop = true;
}

std::string readName()
{
    std::string name;

    do {

        std::cout << "Enter your name" << std::endl;

        if (!std::getline(std::cin, name))
            throw std::runtime_error("Cin error");

    } while (name.empty());

    if (name.length() > PIPE_BUF - tags()[Create].length() * 2)
        name.resize(PIPE_BUF - tags()[Create].length() * 2);

    return name;
}

std::string createUniqueName(const std::string& name)
{
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds> (
            std::chrono::system_clock::now().time_since_epoch()
            );

    return name + "_" + std::to_string(ms.count());
}

void registerNewUser(const std::string& userName, const int& writeFifoFd)
{
    std::string request = tags()[Create] + userName + tags()[Create];

    if (write(writeFifoFd, request.c_str(), request.length()) == -1)
        throw std::runtime_error(pipeWritingError());
}

void manageInputMessages(const int& readFifoFd)
{
    int readReturn;
    char readBuff[PIPE_BUF];

    while (true)
    {
        if (stop)
            break;

        readReturn = read(readFifoFd, readBuff, PIPE_BUF);
        if (readReturn > 0)
        {
            std::cout << readBuff << std::endl;
            std::memset(readBuff, 0, sizeof(readBuff));
        }
        else if (readReturn == -1 && errno != EAGAIN)
        {
            throw std::runtime_error(pipeReadingError());
        }
    }
}

int main()
{
    int writeFifoFd {0};
    int readFifoFd {0};
    std::thread inputReadThread;
    std::string pipeName {0};

    try {
        writeFifoFd = openFifo(defaultFifoName(), O_WRONLY);

        const std::string& userName = createUniqueName(readName());
        pipeName = defaultFifoName() + userName;
        readFifoFd = createFifo(pipeName, RWCreateMode, O_RDONLY | O_NONBLOCK);

        registerNewUser(userName, writeFifoFd);

        inputReadThread = std::thread(manageUserInput, writeFifoFd, std::cref(userName));

        manageInputMessages(readFifoFd);

    } catch (const std::exception& ex) {

        std::cerr << ex.what() << std::endl;
    }

    close(writeFifoFd);
    close(readFifoFd);
    unlink(pipeName.c_str());
    
    if(inputReadThread.joinable())
        inputReadThread.join();

    return 0;
}