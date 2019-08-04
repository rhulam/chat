#ifndef CHAT_SHARED_H
#define CHAT_SHARED_H

#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <climits>
#include <thread>
#include <exception>
#include <vector>
#include <cstring>
#include <unordered_map>
#include <algorithm>

extern const int RWCreateMode;
enum {Write, Create};

std::string defaultFifoName();
std::string exitMessage();
std::string exitDefine();
std::string pipeWritingError();
std::string pipeReadingError();
std::vector<std::string> tags();
int openFifo(const std::string& fifoName, const int& openMode);
int createFifo(const std::string& fifoName, const int& createMode, const int& openMode);

#endif //CHAT_SHARED_H
