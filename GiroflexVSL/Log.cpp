#include "Log.h"

std::fstream Log::logfile;
std::fstream Log::updateFile;
bool Log::deepLogEnabled = false;
bool Log::useUpdateLog = false;

LogFile logFile;

LogFile Log::Level(eLogLevel level)
{
    logFile.level = level;

    auto date = FormatDate();

    if(level == eLogLevel::LOG_BOTH || level == eLogLevel::LOG_NORMAL)
    {
        logfile << date << " ";
    }

    if(Log::useUpdateLog)
    {
        bool logToUpdate = level == eLogLevel::LOG_BOTH || level == eLogLevel::LOG_UPDATE;
        if(level == eLogLevel::LOG_DEEP_UPDATE && deepLogEnabled) logToUpdate = true;

        if(logToUpdate)
        {
            updateFile << date << " ";
        }
    }

    return logFile;
}

void Log::Open(std::string folderPath, std::string fileName, bool createUpdateLog)
{
    useUpdateLog = createUpdateLog;

    logfile.open((folderPath + "/" + fileName + ".log"), std::fstream::out | std::fstream::trunc);

    if(createUpdateLog)
        updateFile.open((folderPath + "/" + fileName + "_update.log"), std::fstream::out | std::fstream::trunc);
}

const char* Log::FormatDate()
{
    std::time_t currentTime = std::time(nullptr);

    std::tm* localTime = std::localtime(&currentTime);
    
    char buffer[256];
    
    strftime(buffer, sizeof(buffer), "[%d/%m %H:%M:%S]", localTime);

    return buffer;
}