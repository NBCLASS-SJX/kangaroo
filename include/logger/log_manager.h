// =====================================================================================
//  Copyright (C) 2024 by Jiaxing Shao. All rights reserved
//
//  文 件 名:  log_manager.h
//  作    者:  Jiaxing Shao, 980853650@qq.com
//  创建时间:  2024-05-24 18:06:04
//  描    述:
// =====================================================================================

#pragma once

#include "log_instance.h"
#include "log_queue.h"

#include <fstream>
#include <map>
#include <string>
#include <thread>

namespace kgr {
namespace log {

static constexpr int LOG_LEVEL_DEBUG = 0;
static constexpr int LOG_LEVEL_INFO  = 1;
static constexpr int LOG_LEVEL_WARN  = 2;
static constexpr int LOG_LEVEL_ERROR = 3;

class LogManager : public LogInstance<LogManager>
{
public:
    LogManager();
    virtual ~LogManager();

public:
    bool setLogFilepath(std::string filename);
    void setLogKeepDays(int keepDays);
    void setLoggingLevel(int level);
    void logRecord(int level, const char *format, ...);

private:
    void openLogFile(int level);
    bool removeLogFiles();
    void loggerWorkerThread();

private:
    std::string                 m_filepath;
    std::string                 m_filename;
    std::map<int, std::fstream> m_logStream;

private:
    bool        m_bWriteLog; // 是否写入日志文件
    bool        m_bRunning;  // 运行状态
    int         m_logLevel;  // 记录的日志级别
    int         m_keepDays;  // 日志保留天数
    uint64_t    m_lastTime;  // 上次日志创建时间
    LogQueue   *m_logQueue;
    std::thread m_thHandle;

public:
    friend LogInstance<LogManager>;
};
} // namespace log
} // namespace kgr