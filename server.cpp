#include "shared/shared.h"

std::unordered_map<std::string, int> pipes;
bool stop = false;

void waitForExit()
{
    std::string input;
    std::cout << exitMessage() << std::endl;

    while (std::getline(std::cin, input))
    {
        if (input == "exit")
            break;
    }
    stop = true;
}

void createNewUsersFifo(const std::string& uniqueUserName)
{
    pipes.insert(std::make_pair(uniqueUserName, openFifo(defaultFifoName() + uniqueUserName, O_WRONLY)));
}

void broadcast(const std::string& uniqueUserName, const std::string& message)
{
    std::string name = uniqueUserName.substr(0, uniqueUserName.find('_'));

    for(auto& elem : pipes)
    {
        if (elem.first != uniqueUserName)
        {
            write(elem.second, (name + ":" + message).c_str(), message.length() + name.length() + 1);
        }
    }
}

void manageMessagesInput(const int& readFifoFd)
{
    char newMessage[PIPE_BUF];
    int readReturn;

    while (true)
    {
        if (stop)
        {
            break;
        }

        readReturn = read(readFifoFd, newMessage, PIPE_BUF);

        if (readReturn > 0)
        {
            std::string newMessageString(newMessage);
            std::string::size_type firstTagPos;

            if ((firstTagPos = newMessageString.find(tags()[Create])) == 0)
            {
                auto lastTagPos = newMessageString.find(tags()[Create], tags()[Create].length());
                std::string uniqueUserName = newMessageString.substr(firstTagPos, lastTagPos - firstTagPos);

                createNewUsersFifo(uniqueUserName.substr(tags()[Create].length()));
            }
            else if ((firstTagPos = newMessageString.find(tags()[Write])) == 0)
            {
                auto lastTagPos = newMessageString.find(tags()[Write], 1);
                std::string uniqueUserName = newMessageString.substr(firstTagPos, lastTagPos - firstTagPos);
                std::string message = newMessageString.substr (lastTagPos + tags()[Write].length());

                broadcast(uniqueUserName.substr(tags()[Write].length()), message);
            }

            std::memset(newMessage, 0, sizeof(newMessage));
        }
        else if (readReturn == -1 && errno != EAGAIN)
        {
            throw std::runtime_error(pipeReadingError());
        }
    }
}

int main()
{
    int readFifoFd {0};
    std::thread exitThread;

    try {

        readFifoFd = createFifo(defaultFifoName(), RWCreateMode, O_RDONLY | O_NONBLOCK);
        exitThread = std::thread(waitForExit);

        manageMessagesInput(readFifoFd);

    } catch (const std::exception& ex) {

        std::cerr << ex.what() << std::endl;
    }

    close(readFifoFd);
    unlink(defaultFifoName().c_str());

    for(auto& elem : pipes)
    {
        close(elem.second);
    }

    if (exitThread.joinable())
        exitThread.join();

    return 0;
}