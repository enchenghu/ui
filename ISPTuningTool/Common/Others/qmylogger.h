/**
 * @file QMyLogger.h
 * @author Mihaly Ma (le.ma@smartsenstech.com)
 * @brief spdlog库封装
 * @version 0.1
 * @date 2023-08-08
 *
 * @copyright Copyright (c) 2022 思特威（上海）电子科技股份有限公司
 *
 */
#ifndef QMYLOGGER_H
#define QMYLOGGER_H

#include <QCoreApplication>
#include <memory>
#include <spdlog/spdlog-inl.h>

/**
 * @brief 日志库封装
 * @details 初始化日志库，读取配置文件，创建日志对象
 *
 */
class QMyLogger
{
public:
    ~QMyLogger();
    static std::shared_ptr<spdlog::logger> DevLogger();

private:
    QMyLogger();
    static std::shared_ptr<spdlog::logger> pGetLogger(const char *name);
};

/**
 * @brief 函数进出日志
 * @details 用于函数进出日志
 *
 */
class CTraceInOut
{
public:
    CTraceInOut(std::shared_ptr<spdlog::logger> pLogger, const char *szFile, const int iLine, const char *szFunction);
    ~CTraceInOut();

    template <typename T>
    T tTraceOut(const T &tRet, const int iLine)
    {
        if (m_pLogger)
        {
            (m_pLogger)->log(spdlog::source_loc{ m_szFile, iLine, m_szFunction }, spdlog::level::trace, "Leave! Return value: {}", tRet);
        }
        m_bSkipOutTrace = true;
        return tRet;
    }

private:
    std::shared_ptr<spdlog::logger> m_pLogger;
    const char                     *m_szFile;
    const int                       m_iLine;
    const char                     *m_szFunction;
    bool                            m_bSkipOutTrace;
};

// 日志宏定义
/// @def CREATE_TRACE_IN_OUT(logger)
/// @brief 通过宏定义创建函数进出日志对象
#define CREATE_TRACE_IN_OUT(logger) CTraceInOut __traceInOut(logger, __FILE__, __LINE__, __FUNCTION__);
/// @def TRACE_RETURN(retVal)
/// @brief 通过宏定义记录函数返回值
#define TRACE_RETURN(retVal)        return __traceInOut.tTraceOut(retVal, __LINE__);
/// @def LOG_TRACE(logger, ...)
/// @brief 通过宏定义输出trace日志
#define LOG_TRACE(logger, ...)                                                                                                             \
if (logger)                                                                                                                            \
        SPDLOG_LOGGER_TRACE(logger, ##__VA_ARGS__);
/// @def LOG_DEBUG(logger, ...)
/// @brief 通过宏定义输出debug日志
#define LOG_DEBUG(logger, ...)                                                                                                             \
if (logger)                                                                                                                            \
    SPDLOG_LOGGER_DEBUG(logger, ##__VA_ARGS__)
/// @def LOG_INFO(logger, ...)
/// @brief 通过宏定义输出info日志
#define LOG_INFO(logger, ...)                                                                                                              \
    if (logger)                                                                                                                            \
    SPDLOG_LOGGER_INFO(logger, ##__VA_ARGS__)
/// @def LOG_WARNING(logger, ...)
/// @brief 通过宏定义输出warning日志
#define LOG_WARNING(logger, ...)                                                                                                           \
    if (logger)                                                                                                                            \
    SPDLOG_LOGGER_WARN(logger, ##__VA_ARGS__)
/// @def LOG_ERROR(logger, ...)
/// @brief 通过宏定义输出error日志
#define LOG_ERROR(logger, ...)                                                                                                             \
    if (logger)                                                                                                                            \
    SPDLOG_LOGGER_ERROR(logger, ##__VA_ARGS__)

/// @def L_TRACE_FUNC
/// @brief 通过宏定义创建内部用函数进出日志对象
#define L_TRACE_FUNC       CREATE_TRACE_IN_OUT(QMyLogger::DevLogger())
/// @def L_TRACE(...)
/// @brief 通过宏定义输出内部用trace日志
#define L_TRACE(...)       LOG_TRACE(QMyLogger::DevLogger(), ##__VA_ARGS__)
/// @def L_DEBUG(...)
/// @brief 通过宏定义输出内部用debug日志
#define L_DEBUG(...)       LOG_DEBUG(QMyLogger::DevLogger(), ##__VA_ARGS__)
/// @def L_INFO(...)
/// @brief 通过宏定义输出内部用info日志
#define L_INFO(...)        LOG_INFO(QMyLogger::DevLogger(), ##__VA_ARGS__)
/// @def L_WARNING(...)
/// @brief 通过宏定义输出内部用warning日志
#define L_WARNING(...)     LOG_WARNING(QMyLogger::DevLogger(), ##__VA_ARGS__)
/// @def L_ERROR(...)
/// @brief 通过宏定义输出内部用error日志
#define L_ERROR(...)       LOG_ERROR(QMyLogger::DevLogger(), ##__VA_ARGS__)

#endif // QMYLOGGER_H
