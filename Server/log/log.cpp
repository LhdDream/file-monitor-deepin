#include "log.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>

static char s_log_level[][8] =
{ "Trace", "Debug", "Info ", "Warn ", "Error", "Fatal" };

class FileLock
{
public:
    explicit FileLock(const char* file_name)
    {
        m_fd = open(file_name, O_CREAT | O_RDWR, 0666);
        if (m_fd >= 0)
        {
            ::flock(m_fd, LOCK_EX);
        }
    }
    ~FileLock()
    {
        if (m_fd >= 0)
        {
            ::flock(m_fd, LOCK_UN);
            close(m_fd);
        }
    }
    int m_fd;
};

Log Log::s_instance;
Log* Log::s_custom_log = NULL;

Log::Log()
{
    DefaultInit();
}

Log::~Log()
{

}

pid_t Log::GetTid() const
{
    return syscall(SYS_gettid);
}

void Log::DefaultInit()
{
    m_min_log_level = NONE;
    m_max_file_size = ITPUB_DEFAULT_FILE_SIZE;
    m_max_file_no = ITPUB_DEFAULT_FILE_NO;
    memset(m_log_file, 0, sizeof(m_log_file));
    memset(m_lock_file, 0, sizeof(m_lock_file));

    pid_t pid = getpid();
    char buf[ITPUB_MAX_PATH_LEN];
    char exe[ITPUB_MAX_PATH_LEN];
    snprintf(exe, ITPUB_MAX_PATH_LEN, "/proc/%d/exe", pid);

    ssize_t count = readlink(exe, buf, ITPUB_MAX_PATH_LEN);
    if (count < 0 || count >= ITPUB_MAX_PATH_LEN)
    {
        printf("readlink error:%s\n", strerror(errno));
        return;
    }

    buf[count] = '\0';
    char *app = strrchr(buf, '/');
    if (NULL == app)
    {
        app = buf;
    }
    else
    {
        app++;
    }
    snprintf(m_log_file, ITPUB_MAX_PATH_LEN, "%s/%s%s", ITPUB_DEFAULT_LOG_PATH,
            app, ITPUB_LOG_SUFFIX);
    snprintf(m_lock_file, ITPUB_MAX_PATH_LEN, "%s/%s", ITPUB_DEFAULT_LOG_PATH,
            ITPUB_LOCK_FILE);

    if (access(ITPUB_DEFAULT_LOG_PATH, 0) == -1)
    {
        mkdir(ITPUB_DEFAULT_LOG_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    strncpy(m_app_name, app, ITPUB_MAX_PATH_LEN);

    m_min_log_level = TRACE;

    pthread_key_create(&m_thread_log, NULL);
    pthread_setspecific(m_thread_log, NULL);

    // 捕获coredump信号，打印堆栈日志
    signal(SIGSEGV, Log::PrintDumpLog);
}

void Log::PrintDumpLog(int sigo)
{
    std::string ss;
    LogUtils::DumpTrace(ss);
    RUN_LOG("%s", ss.c_str());
    signal(SIGSEGV, SIG_DFL);
    raise(sigo);
}

void Log::SetThreadLog(Log* log)
{
    pthread_setspecific(m_thread_log, log);
}

void Log::SetCustomLog(Log* log)
{
    s_custom_log = log;
}

int Log::Set(int min_log_level, const char* log_path, const char* name_prefix,
        uint32_t max_file_size, uint32_t max_file_no)
{
    if (min_log_level < TRACE || min_log_level > NONE)
    {
        return -1;
    }

    if (NULL == log_path || NULL == name_prefix || max_file_size == 0
            || max_file_no == 0)
    {
        return -1;
    }

    m_min_log_level = min_log_level;
    snprintf(m_log_file, ITPUB_MAX_PATH_LEN, "%s/%s%s", log_path, name_prefix,
            ITPUB_LOG_SUFFIX);
    snprintf(m_lock_file, ITPUB_MAX_PATH_LEN, "%s/%s", log_path,
            ITPUB_LOCK_FILE);
    m_max_file_size = max_file_size;
    m_max_file_no = max_file_no;
    if (m_max_file_size > ITPUB_MAX_FILE_SIZE)
    {
        m_max_file_size = ITPUB_MAX_FILE_SIZE;
    }

    if (m_max_file_no > ITPUB_MAX_FILE_NO)
    {
        m_max_file_no = ITPUB_MAX_FILE_NO;
    }
    return 0;
}

void Log::SetLogLevel(int min_log_level)
{
    if (min_log_level >= TRACE && min_log_level <= NONE)
    {
        m_min_log_level = min_log_level;
    }
}

void Log::Record(int log_level, const char* file, int line, const char *fmt,
        ...)
{
    if (log_level < m_min_log_level || log_level >= NONE)
    {
        return;
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm ptm;
    char cur_time[64] =
    { 0 };
    localtime_r(&tv.tv_sec, &ptm);
    strftime(cur_time, sizeof(cur_time), "%Y-%m-%d %H:%M:%S", &ptm);

    char out[ITPUB_ATOM_LOG_LEN];
    int log_head_len = snprintf(out, sizeof(out), "%s.%06ld %-5d %s <%s:%d> ",
            cur_time, tv.tv_usec, GetTid(), s_log_level[log_level], file, line);
    if (log_head_len == -1 || log_head_len >= ITPUB_ATOM_LOG_LEN)
    {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    int log_body_len = vsnprintf(out + log_head_len,
            ITPUB_ATOM_LOG_LEN - log_head_len, fmt, ap);
    va_end(ap);

    int log_len = log_head_len + log_body_len;

    if (log_body_len == -1)
    {
        return;
    }
    else if (log_len < ITPUB_ATOM_LOG_LEN)
    {
        out[log_len] = '\n';
        return Output(out, log_len + 1);
    }
    else if (log_len >= ITPUB_MAX_LOG_LEN)
    {
        // 超过1M的日志只输出4K
        out[ITPUB_ATOM_LOG_LEN - 1] = '\n';
        return Output(out, ITPUB_ATOM_LOG_LEN);
    }
    else
    {
        // 大于4K小于1M的日志输出不保证原子性
        char* log = new char[log_len + 1];
        memcpy(log, out, log_head_len);
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(log + log_head_len, log_len + 1 - log_head_len, fmt, ap);
        va_end(ap);
        log[log_len] = '\n';
        Output(log, log_len + 1);
        delete[] log;
    }
}

void Log::Output(const char* out, int out_len)
{
    RollFile();
    int fd = open(m_log_file, O_CREAT | O_RDWR | O_APPEND, 0666);
    if (fd >= 0)
    {
        write(fd, out, out_len);
        close(fd);
    }
}

void Log::RollFile()
{
    struct stat statfile;
    if (stat(m_log_file, &statfile) == -1 || statfile.st_size < m_max_file_size)
    {
        return;
    }

    FileLock fl(m_lock_file);
    if (stat(m_log_file, &statfile) == -1 || statfile.st_size < m_max_file_size)
    {
        return;
    }

    char new_file[ITPUB_MAX_PATH_LEN];
    char old_file[ITPUB_MAX_PATH_LEN];

    /* 循环的将log_filecount - 1  -> log_filecount
     ...
     log_name.2 -> log_name.3
     log_name.1 -> log_name.2*/
    int i;
    for (i = m_max_file_no - 1; i > 0; i--)
    {
        snprintf(old_file, sizeof(old_file), "%s.%d", m_log_file, i);

        if (stat(old_file, &statfile) == 0)
        {
            snprintf(new_file, sizeof(new_file), "%s.%d", m_log_file, i + 1);
            rename(old_file, new_file);
        }
    }

    snprintf(new_file, sizeof(new_file), "%s.1", m_log_file);
    rename(m_log_file, new_file);
}

///////////////////////////////////////////////////////////////////////////////

RunLog RunLog::s_run_log;
RunLog::RunLog()
{
    // 修改运行日志名
    snprintf(m_log_file, ITPUB_MAX_PATH_LEN, "%s/%s%s", ITPUB_DEFAULT_LOG_PATH,
            m_app_name, ".run");
    m_min_log_level = TRACE;
}
RunLog::~RunLog()
{
}

StatLog StatLog::s_stat_log;
StatLog::StatLog()
{
    // 修改统计日志名
    snprintf(m_log_file, ITPUB_MAX_PATH_LEN, "%s/%s%s", ITPUB_DEFAULT_LOG_PATH,
            m_app_name, ".stat");
}
StatLog::~StatLog()
{
}
void StatLog::Write(const char *fmt, ...)
{
    char out[ITPUB_ATOM_LOG_LEN];
    va_list ap;
    va_start(ap, fmt);
    int log_len = vsnprintf(out, ITPUB_ATOM_LOG_LEN, fmt, ap);
    va_end(ap);
    if (log_len == -1)
    {
        return;
    }
    else if (log_len < ITPUB_ATOM_LOG_LEN)
    {
        out[log_len] = '\n';
        return Output(out, log_len + 1);
    }
    else
    {
        out[ITPUB_ATOM_LOG_LEN - 1] = '\n';
        return Output(out, ITPUB_ATOM_LOG_LEN);
    }
}




