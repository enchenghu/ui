/**
 * @file QMyLogger.cpp
 * @author Mihaly Ma (le.ma@smartsenstech.com)
 * @brief spdlog库封装
 * @version 0.1
 * @date 2023-08-08
 *
 * @page 内部日志说明
 * -# 默认不开启日志, 如需开启日志, 请在DLL目录下创建SCNetSDK.conf文件, 并在其中添加以下内容
 * @code
 * dev_level=0
 * @endcode
 * -# level为日志等级, 0 ~ 4 为 Trace, Debug, Info, Warn, Error
 * -# 日志文件夹为DLL目录下的SCNetSDKLog
 * -# 日志文件名为 SCNetSDKDev_YYYYmmddHHMMss.log
 * -# 日志文件大小为10MB, 超过后会自动创建新的日志文件
 * -# 一次运行中会创建最多10个日志文件, 超过后最早的日志文件会被删除

 * @copyright Copyright (c) 2022 思特威（上海）电子科技股份有限公司
 *
 */
#include "qmylogger.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <sstream>

// 日志定义
const char     LOG_FOLDER[]           = "Log";                  ///< 日志文件夹
const char     LOG_DEVELOP_NAME[]     = "ISPTuningToolDev";     ///< 开发日志
const char     LOG_CONFIG_FILE[]      = "Log.conf";             ///< 配置文件
const char     LOG_CONFIG_DEV_LEVEL[] = "level";            ///< 开发日志级别
const char     LOG_CONFIG_LOG_SIZE[]  = "log_size";             ///< 日志文件大小
const char     LOG_CONFIG_LOG_FILES[] = "log_files";            ///< 日志文件个数
const uint64_t LOG_MAX_SIZE           = 1024 * 1024 * 100;      ///< 100M
const uint32_t LOG_MAX_FILES          = 100;                    ///< 100个文件

/**
 * @brief 析构函数
 * @details 关闭spdlog并清理资源
 *
 */
QMyLogger::~QMyLogger()
{
    spdlog::shutdown();
}

/**
 * @brief 获取Dev日志对象
 *
 * @return std::shared_ptr<spdlog::logger>
 */
std::shared_ptr<spdlog::logger> QMyLogger::DevLogger()
{
    static std::shared_ptr<spdlog::logger> pLogger = pGetLogger(LOG_DEVELOP_NAME);
    return pLogger;
}

/**
 * @brief 初始化日志库
 * @details 读取配置文件，创建日志对象
 *
 */
QMyLogger::QMyLogger()
{
    std::string strPath = QCoreApplication::applicationDirPath().toStdString() + "/";

    // Read Config
    std::ifstream ifs(strPath + LOG_CONFIG_FILE);
    if (!ifs.is_open())
    {
        return;
    }
    spdlog::level::level_enum devLogLevel = spdlog::level::off;
    uint64_t                  uLogSize  = LOG_MAX_SIZE;
    uint32_t                  uMaxFiles = LOG_MAX_FILES;
    // Parse Config line by line, case insensitive
    std::string strLine;
    while (std::getline(ifs, strLine))
    {
        if (strLine.empty())
        {
            continue;
        }
        std::istringstream iss(strLine);
        std::string        strKey;
        std::string        strValue;
        if (std::getline(iss, strKey, '=') && std::getline(iss, strValue))
        {
            std::transform(strKey.begin(), strKey.end(), strKey.begin(), ::tolower);
            if (strKey == LOG_CONFIG_DEV_LEVEL)
            {
                devLogLevel = static_cast<spdlog::level::level_enum>(std::stoi(strValue));
            }
            else if (strKey == LOG_CONFIG_LOG_SIZE)
            {
                uLogSize = std::stoull(strValue);
            }
            else if (strKey == LOG_CONFIG_LOG_FILES)
            {
                uMaxFiles = std::stoul(strValue);
            }
        }
    }
    ifs.close();
    if (devLogLevel == spdlog::level::off)
    {
        return;
    }
    strPath += LOG_FOLDER;
    strPath += "\\";

	std::filesystem::create_directory(strPath);

	std::chrono::system_clock::time_point now  = std::chrono::system_clock::now();
    std::time_t                           time = std::chrono::system_clock::to_time_t(now);
    std::tm                               tm_info;
    localtime_s(&tm_info, &time);
    std::ostringstream oss;
    oss << std::put_time(&tm_info, "%Y%m%d%H%M%S");
    std::string formattedDate = oss.str();

    // Init Logger
    if (devLogLevel != spdlog::level::off)
    {
        auto dev_logger = spdlog::rotating_logger_mt<spdlog::async_factory>(
            LOG_DEVELOP_NAME, strPath + "Log_" + formattedDate + ".log", uLogSize, uMaxFiles);
        dev_logger->set_level(devLogLevel);
        dev_logger->set_pattern("[%Y/%m/%d %H:%M:%S.%e][%l][thread %t][%s:%#][%!()] %v");
    }
    spdlog::flush_every(std::chrono::seconds(1));
}

/**
 * @brief 获取日志对象
 *
 * @param name 日志对象名称
 * @return std::shared_ptr<spdlog::logger>
 */
std::shared_ptr<spdlog::logger> QMyLogger::pGetLogger(const char *name)
{
    static QMyLogger logger; // init once
    return spdlog::get(name);
}

/**
 * @brief 构造函数
 * @details 记录函数进入日志
 *
 * @param pLogger 日志对象
 * @param szFile 文件名
 * @param iLine 行号
 * @param szFunction 函数名
 */
CTraceInOut::CTraceInOut(std::shared_ptr<spdlog::logger> pLogger, const char *szFile, const int iLine, const char *szFunction)
    : m_pLogger(pLogger)
    , m_szFile(szFile)
    , m_iLine(iLine)
    , m_szFunction(szFunction)
    , m_bSkipOutTrace(false)
{
    if (m_pLogger)
        (m_pLogger)->log(spdlog::source_loc{ szFile, m_iLine, szFunction }, spdlog::level::trace, "Enter!");
}

/**
 * @brief 析构函数
 * @details 记录函数离开日志
 *
 */
CTraceInOut::~CTraceInOut()
{
    if (m_pLogger && !m_bSkipOutTrace)
    {
        (m_pLogger)->log(spdlog::source_loc{ m_szFile, 1, m_szFunction }, spdlog::level::trace, "Leave!");
    }
}
