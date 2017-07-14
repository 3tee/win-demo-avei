#ifndef RTDEBUG_H
#define RTDEBUG_H

#define RT_LOG_MODULE_TRACE 
//#define RT_DISABLE_TRACE	

#ifndef RT_DISABLE_TRACE
#include "avdbase/RtLogger.h"
//
// Parameters Descriptions:
//   [module] module number, uint32 type;
//   [desc  ] module description that will be displayed in log output to tell which module 
//            it belong to, const char* type, must be less than 64 bytes in length;
//   [str   ] log content, const char* type;
//
//#define RT_LOG_REGISTER_MODULE(module, desc)	//RtLog::RegisterModule(module, desc)

#define RT_ERROR_LOG(module, str)			RT_LOG(rtcbase::LS_ERROR, module, str)
#define RT_WARNING_LOG(module, str)			RT_LOG(rtcbase::LS_WARNING, module, str)
#define RT_INFO_LOG(module, str)			RT_LOG(rtcbase::LS_INFO, module, str)
#ifdef _DEBUG
#define RT_STATE_LOG(module, str)			RT_LOG(rtcbase::LS_VERBOSE, module, str)
#else
#define RT_STATE_LOG(module, str)			RT_LOG(rtcbase::LS_VERBOSE, module, str)
#endif


#define RT_ERROR_LOG_THIS(module, str)		RT_LOG(rtcbase::LS_ERROR, module, str << " this=" << this)
#define RT_WARNING_LOG_THIS(module, str)	RT_LOG(rtcbase::LS_WARNING, module, str << " this=" << this)
#define RT_INFO_LOG_THIS(module, str)		RT_LOG(rtcbase::LS_INFO, module, str << " this=" << this)
#define RT_STATE_LOG_THIS(module, str)		RT_LOG(rtcbase::LS_VERBOSE, module, str << " this=" << this)

/*-----------------------------------------------------------------------------------*/

#define LOG_ERROR(str)			RT_ERROR_LOG(RT_LOG_MODULE_TRACE, str)
#define LOG_WARNING(str)		RT_WARNING_LOG(RT_LOG_MODULE_TRACE, str)
#define LOG_INFO(str)			RT_INFO_LOG(RT_LOG_MODULE_TRACE, str)
#define LOG_STATE(str)			RT_STATE_LOG(RT_LOG_MODULE_TRACE, str)

#define LOG_ERROR_T(str)		LOG_ERROR(str << " this=" << this)
#define LOG_WARNING_T(str)		LOG_WARNING(str << " this=" << this)
#define LOG_INFO_T(str)			LOG_INFO(str << " this=" << this)
#define LOG_STATE_T(str)		LOG_STATE(str << " this=" << this)

#define LOG_ERROR_F(str)		LOG_ERROR(__FUNCTION__ << ": " << str)
#define LOG_WARNING_F(str)		LOG_WARNING(__FUNCTION__ << ": " << str)
#define LOG_INFO_F(str)			LOG_INFO(__FUNCTION__ << ": " << str)
#define LOG_STATE_F(str)		LOG_STATE(__FUNCTION__ << ": " << str)

#define LOG_ERROR_T_F(str)		LOG_ERROR(__FUNCTION__ << ": " << str << " this=" << this)
#define LOG_WARNING_T_F(str)	LOG_WARNING(__FUNCTION__ << ": " << str << " this=" << this)
#define LOG_INFO_T_F(str)		LOG_INFO(__FUNCTION__ << ": " << str << " this=" << this)
#define LOG_STATE_T_F(str)		LOG_STATE(__FUNCTION__ << ": " << str << " this=" << this)
#else//!RT_DISABLE_TRACE

//#define RT_LOG_REGISTER_MODULE(module, desc) 

#define RT_ERROR_LOG(module, str)
#define RT_WARNING_LOG(module, str)
#define RT_INFO_LOG(module, str)
#define RT_STATE_LOG(module, str)

#define RT_ERROR_LOG_THIS(module, str)
#define RT_WARNING_LOG_THIS(module, str)
#define RT_INFO_LOG_THIS(module, str)
#define RT_STATE_LOG_THIS(module, str)

/*-----------------------------------------------------------------------------------*/

#define LOG_ERROR(str) 
#define LOG_WARNING(str) 
#define LOG_INFO(str) 
#define LOG_STATE(str) 
#define RT_FUNC_TRACE(str) 

#define LOG_ERROR_T(str) 
#define LOG_WARNING_T(str) 
#define LOG_INFO_T(str) 
#define LOG_STATE_T(str) 

#define LOG_ERROR_F(str)
#define LOG_WARNING_F(str)
#define LOG_INFO_F(str)
#define LOG_STATE_F(str)

#define LOG_ERROR_T_F(str)
#define LOG_WARNING_T_F(str)
#define LOG_INFO_T_F(str)
#define LOG_STATE_T_F(str)
#endif // RT_DISABLE_TRACE


#define RT_DIAGNOSE_TRACE(str)	

#endif // RTDEBUG_H
