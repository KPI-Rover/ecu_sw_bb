#ifndef LOGGINGINCLUDES_H
#define LOGGINGINCLUDES_H

#pragma once
#include <glog/logging.h>

#define LOG_DEBUG VLOG(1) << "[DEBUG] "
#define LOG_INFO LOG(INFO) << "[INFO] "
#define LOG_WARNING LOG(WARNING) << "[WARNING] "
#define LOG_ERROR LOG(ERROR) << "[ERROR] "
#define LOG_FATAL LOG(FATAL) << "[FATAL ERROR] "

#endif
