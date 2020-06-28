#ifndef _LOG_H_
#define _LOG_H_

#include <stdint.h>
#include <pthread.h>
#include "log_utils.h"

//syslog 与spplog 冲突
//#include <syslog.h>

/**
 * syslog 日志宏
 * priority:
 *  LOG_EMERG      system is unusable
 *  LOG_ALERT      action must be taken immediately
 *  LOG_CRIT       critical conditions
 *  LOG_ERR        error conditions
 *  LOG_WARNING    warning conditions
 *  LOG_NOTICE     normal, but significant, condition
 *  LOG_INFO       informational message
 *  LOG_DEBUG      debug-level message
 */
#define SYS_LOG(priority, fmt, ...) \
    syslog(priority, fmt, ##__VA_ARGS__)

/**
 * 打开syslog, 可以不调用，使用默认。
 */
#define OPEN_SYS_LOG(ident, option, facility) \
    openlog(ident, option, facility)

/**
 * 写日志宏
 */
#define LOG(level, fmt, ...) \
    do \
    { \
        if (Log::Instance()->GetLogLevel() <= level) \
        { \
            Log::Instance()->Record(level, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
        } \
    } \
    while(0)
/**
 * 写二进制日志宏
 */
#define BINARY_LOG(level, data, len) \
    do \
    { \
        if (Log::Instance()->GetLogLevel() <= level) \
        { \
            Log::Instance()->Record(level, __FILE__, __LINE__, LogUtils::Bin2HexLog(data, len).c_str()); \
        } \
    } \
    while(0)

#define RUN_LOG(fmt, ...) \
    do \
    { \
        RunLog::Instance()->Record(INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } \
    while(0)

#define STAT_LOG(fmt, ...) \
    do \
    { \
        StatLog::Instance()->Write(fmt, ##__VA_ARGS__); \
    } \
    while(0)

// 自定义LOG的宏
#define LOG_SEV(log, level, fmt, ...)  \
    do \
    { \
        if (Log::Instance()->GetLogLevel() <= level) \
        { \
            log->Record(level, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
        } \
    } \
    while(0)



//最大路径长度
const uint32_t ITPUB_MAX_PATH_LEN = 256;
//最大文件SIZE为1G
const uint32_t ITPUB_MAX_FILE_SIZE = (1 << 30);
//默认最大文件编号
const uint32_t ITPUB_MAX_FILE_NO = 99;
//原子日志长度，可保证不乱序
const uint32_t ITPUB_ATOM_LOG_LEN = 4096;
//日志最大长度，超过4K的一条日志无法保证原子性
const uint32_t ITPUB_MAX_LOG_LEN = 1024 * 1024;
//默认文件SIZE为100M
const uint32_t ITPUB_DEFAULT_FILE_SIZE = 100 * 1024 * 1024;
//默认文件编号
const uint32_t ITPUB_DEFAULT_FILE_NO = 9;
//默认日志目录
const char* const ITPUB_DEFAULT_LOG_PATH = "../log";
//同步文件
const char* const ITPUB_LOCK_FILE = ".lock";
//默认日志后缀名
const char* const ITPUB_LOG_SUFFIX = ".log";

/**
 * 日志级别
 */
enum LOG_LEVEL
{
    TRACE = 0, //!< TRACE
    DEBUG,     //!< DEBUG
    INFO,      //!< INFO
    WARN,      //!< WARN
    ERROR,     //!< ERROR
    FATAL,     //!< FATAL
    NONE       //!< NONE 当要禁止写任何日志的时候,设置日志级别为NONE
};

/**
 * 日志类
 */
class Log
{
public:
    Log();
    virtual ~Log();
    /**
     * 日志类单例
     * @return 日志类单例
     */
    inline static Log* Instance()
    {
        if (NULL == s_custom_log)
        {
            void* thread_log = pthread_getspecific(s_instance.m_thread_log);
            //线程的局部存储 ， c++11  thread_local
            if (NULL != thread_log)
            {
                return (Log*) thread_log;
            }

            return &s_instance; //返回单例
        }
        return s_custom_log;
    }

    /**
     * 设置定制的Log
     * @param log Log对象
     */
    static void SetCustomLog(Log* log);

    /**
     * 设置线程级定制的Log
     * @param log Log对象
     */
    void SetThreadLog(Log* log);

    bool IsThreadLog()
    {
        return pthread_getspecific(s_instance.m_thread_log) != NULL;
    }

    /**
     * 记录日志
     * @param log_level 日志记录
     * @param file 文件名
     * @param line 行号
     * @param fmt 日志内容
     */
    virtual void Record(int log_level, const char* file, int line,
            const char *fmt, ...);

    /**
     * 设置日志配置
     * @param min_log_level 日志级别，默认TRACE
     * @param log_path      日志路径，默认../log
     * @param name_prefix   日志前缀，默认进程名
     * @param max_file_size 每个日志文件的最大长度, 默认100M
     * @param max_file_no   备份日志文件最大个数，默认9个
     * @return 0成功, 否则失败
     */
    int Set(int min_log_level, const char* log_path, const char* name_prefix,
            uint32_t max_file_size = ITPUB_DEFAULT_FILE_SIZE,
            uint32_t max_file_no = ITPUB_DEFAULT_FILE_NO);

    /**
     * 设置日志级别
     * @param min_log_level 日志级别
     */
    void SetLogLevel(int min_log_level);

    /**
     * 获取当前日志级别
     * @return 当前日志级别
     */
    inline int GetLogLevel() const
    {
        return m_min_log_level;
    }

    /**
     * 获取当前程序名
     * @return 当前日志级别
     */
    inline const char* GetAppName() const
    {
        return m_app_name;
    }
    /**
     * 获取当前线程ID
     * @return 获取当前线程ID
     */
    pid_t GetTid() const;
protected:
    virtual void Output(const char* out, int out_len);
    virtual void RollFile();
    virtual void DefaultInit();
    static void PrintDumpLog(int sigo);

protected:
    static Log* s_custom_log;
    static Log s_instance;
    int m_min_log_level;
    char m_log_file[ITPUB_MAX_PATH_LEN];
    char m_lock_file[ITPUB_MAX_PATH_LEN];
    char m_app_name[ITPUB_MAX_PATH_LEN];
    uint32_t m_max_file_size;
    uint32_t m_max_file_no;
    // 线程级日志控制，用于染色日志
    pthread_key_t m_thread_log;
};

class RunLog: public Log
{
public:
    static RunLog* Instance()
    {
        return &s_run_log;
    }
    RunLog();
    ~RunLog();
private:
    static RunLog s_run_log;
};

class StatLog: public Log
{
public:
    static StatLog* Instance()
    {
        return &s_stat_log;
    }
    StatLog();
    ~StatLog();
    virtual void Write(const char *fmt, ...);
private:
    static StatLog s_stat_log;
};



#endif
