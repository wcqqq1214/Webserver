#ifndef __SYLAR_LOG_H
#define __SYLAR_LOG_H

#include <string>
#include <stdint.h>
#include <memory>
#include <list>

namespace sylar {

// 日志事件
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    
    LogEvent();
private:
    const char* m_file = nullptr;   // 文件名
    int32_t m_line = 0;             // 行号
    uint32_t m_elapse = 0;          // 程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;        // 线程 id
    uint32_t m_fiberId = 0;         // 协程 id
    uint64_t m_time;                // 时间戳
    std::string m_content;
};

// 日志级别
class LogLevel {
public:
    enum Level {
        DEBUG = 1,
        INFO = 2,
        WARNING = 3,
        ERROR = 4,
        FATAL = 5
    };
};

// 日志格式器
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;

    std::string format(LogEvent::ptr event);
private:

};

// 日志输出器
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;

    virtual ~LogAppender() {}

    void log(LogLevel::Level level, LogEvent::ptr event);
private:
    LogLevel::Level m_level;
};

// 日志记录器
class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string& name = "root");

    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warning(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const { return m_level; }
private:
    std::string m_name;                         // 日志名称
    LogLevel::Level m_level;                    // 日志级别
    std::list<LogAppender::ptr> m_appenders;    // Appender 集合   
};

// 输出到控制台的 Appender
class StdoutLogAppender : public LogAppender {

};

// 输出到文件的 Appender
class FileLogAppender : public LogAppender {

};

}

#endif
