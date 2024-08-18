#pragma once

#include <fstream>

enum eLogLevel {
    LOG_NORMAL,
    LOG_UPDATE,
    LOG_DEEP_UPDATE,
    LOG_BOTH
};

class LogFile;

class Log {
public:
	static std::fstream logfile;
	static std::fstream updateFile;
    static bool deepLogEnabled;
    static bool useUpdateLog;

    static LogFile Level(eLogLevel level);

    static void Open(std::string folderPath, std::string fileName, bool createUpdateLog = false);

    static const char* FormatDate();
};

class LogFile {
public:
    eLogLevel level = eLogLevel::LOG_NORMAL;

    template <typename T>
    LogFile& operator<<(const T& message)
    {
        if(level == eLogLevel::LOG_BOTH || level == eLogLevel::LOG_NORMAL)
        {
            Log::logfile << message;
        }

        bool logToUpdate = level == eLogLevel::LOG_BOTH || level == eLogLevel::LOG_UPDATE;
        if(level == eLogLevel::LOG_DEEP_UPDATE && Log::deepLogEnabled) logToUpdate = true;

        if(logToUpdate)
        {
            Log::updateFile << message;
        }

        //Log::file << "[LogFile] CASE 1: " << message << std::endl;
        return *this;
    }

    LogFile& operator<<(std::ostream& (*manip)(std::ostream&))
    {
        if(level == eLogLevel::LOG_BOTH || level == eLogLevel::LOG_NORMAL)
        {
            Log::logfile << manip;
        }

        if(Log::useUpdateLog)
        {
            bool logToUpdate = level == eLogLevel::LOG_BOTH || level == eLogLevel::LOG_UPDATE;
            if(level == eLogLevel::LOG_DEEP_UPDATE && Log::deepLogEnabled) logToUpdate = true;

            if(logToUpdate)
            {
                Log::updateFile << manip;
            }
        }

        // ?
        if (manip == std::endl<std::ostream::char_type, std::ostream::traits_type>) {
            //Log::file << "[LogFile] CASE 2: " << manip << std::endl;
        } else {
            Log::logfile << "[LogFile] CASE 3: " << manip << std::endl;
        }
        
        return *this;
    }
};
