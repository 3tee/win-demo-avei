
#ifndef RTCDEFINES_H
#define RTCDEFINES_H

//////////////////////////////////////////////////////////////////////
// First definition: choose OS
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
  #ifndef RT_WIN32
    #define RT_WIN32
  #endif // RT_WIN32
#endif // WIN32

#ifdef UNIX
  #ifndef RT_UNIX
    #define RT_UNIX
  #endif // RT_UNIX
#endif // UNIX

#ifdef LINUX
  #ifndef RT_LINUX
    #define RT_LINUX
  #endif // RT_LINUX
  #ifndef RT_UNIX
    #define RT_UNIX
  #endif // RT_UNIX
#endif // LINUX

#ifdef ANDROID
  #ifndef RT_ANDROID
	#define RT_ANDROID
  #endif // RT_LINUX
  #ifndef RT_UNIX
	#define RT_UNIX
  #endif // RT_UNIX
#endif // ANDROID

#ifdef MACOS
  #ifndef RT_MACOS
    #define RT_MACOS
  #endif // RT_MACOS
#endif // MACOS

#ifdef IOS
#ifndef RT_IOS
#define RT_IOS
#endif // RT_IOS
#endif // IOS


#if (defined(RT_LINUX) && !defined(RT_ANDROID)) || defined(RT_WIN32) || (defined(RT_MAC) && !defined(RT_IOS))
#ifndef RT_DESKTOP
#define RT_DESKTOP
#endif // RT_DESKTOP
#endif

//////////////////////////////////////////////////////////////////////
// OS API definition
//////////////////////////////////////////////////////////////////////

#ifdef RT_MACOS
	#define	_BSD_TIME_T_	long			/* time() */
	typedef	_BSD_TIME_T_	time_t;
	#ifndef MachOSupport
	# define socklen_t int
	#endif	//MachOSupport
	#define	EINPROGRESS	36		/* Operation now in progress */
	#define EINTR		4
	#define EPERM		1
	typedef int sem_t;
	#define	RLIMIT_NOFILE	8
	//#define RT_HAS_BUILTIN_ATOMIC_OP 1
	enum
	{
	  PTHREAD_MUTEX_TIMED_NP,
	  PTHREAD_MUTEX_RECURSIVE_NP,
	  PTHREAD_MUTEX_ERRORCHECK_NP,
	  PTHREAD_MUTEX_ADAPTIVE_NP
	  
	  , PTHREAD_MUTEX_FAST_NP = PTHREAD_MUTEX_ADAPTIVE_NP
	};

	#ifndef MachOSupport
	#include "ioccom.h"
	#endif	//MachOSupport

	/* Generic file-descriptor ioctl's. */
	#define	FIOCLEX		 _IO('f', 1)		/* set close on exec on fd */
	#define	FIONCLEX	 _IO('f', 2)		/* remove close on exec */
	#define	FIONREAD	_IOR('f', 127, int)	/* get # bytes to read */
	#define	FIONBIO		_IOW('f', 126, int)	/* set/clear non-blocking i/o */
	#define	FIOASYNC	_IOW('f', 125, int)	/* set/clear async i/o */
	#define	FIOSETOWN	_IOW('f', 124, int)	/* set owner */
	#define	FIOGETOWN	_IOR('f', 123, int)	/* get owner */
	#define	FIODTYPE	_IOR('f', 122, int)	/* get d_type */

 //for TCP	
 	#ifndef MachOSupport
	#define	TCP_NODELAY	0x01	/* don't delay send to coalesce packets */
	#endif
	
	#ifndef MachOSupport
	#define	TCP_MAXSEG	0x02	/* set maximum segment size */
	#endif
	
	#define TCP_NOPUSH	0x04	/* don't push last block of write */
	#define TCP_NOOPT	0x08	/* don't use TCP options */
	
	#ifndef MachOSupport
	#define TCP_KEEPALIVE	0x10	/* idle time used when SO_KEEPALIVE is enabled */
	#endif
//pthread
	
	#define PTHREAD_CREATE_JOINABLE      1
	#define PTHREAD_CREATE_DETACHED      2

	#define PTHREAD_INHERIT_SCHED        1
	#define PTHREAD_EXPLICIT_SCHED       2

	#define PTHREAD_CANCEL_ENABLE        0x01  /* Cancel takes place at next cancellation point */
	#define PTHREAD_CANCEL_DISABLE       0x00  /* Cancel postponed */
	#define PTHREAD_CANCEL_DEFERRED      0x02  /* Cancel waits until cancellation point */
	#define PTHREAD_CANCEL_ASYNCHRONOUS  0x00  /* Cancel occurs immediately */

	/* We only support PTHREAD_SCOPE_SYSTEM */
	#define PTHREAD_SCOPE_SYSTEM         1
	#define PTHREAD_SCOPE_PROCESS        2

	/* We only support PTHREAD_PROCESS_PRIVATE */
	#define PTHREAD_PROCESS_SHARED         1
	#define PTHREAD_PROCESS_PRIVATE        2

	//extern CHARSET_INFO *default_charset_info;
	//#define my_ctype	(default_charset_info->ctype)
	//#define	isspace(c)	((my_ctype+1)[(uchar) (c)] & _S)
	// temp define for compiler
	
  	#ifndef MachOSupport
    struct timespec {
			time_t  tv_sec;         /* seconds */
			long    tv_nsec;        /* and nanoseconds */  
	  };

	//#define EAGAIN	  35
	//#define EWOULDBLOCK EAGAIN
	#define EWOULDBLOCK 35
	#endif	//MachOSupport
#endif

#ifdef RT_WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
 #define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _SCL_SECURE_NO_DEPRECATE
  #define _SCL_SECURE_NO_DEPRECATE
#endif

#define _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#ifdef RT_SOLARIS
  #define INADDR_NONE             0xffffffff
#endif


#ifdef RT_WIN32
  #if defined (_LIB) || (RT_OS_BUILD_LIB) 
    #define RT_API_EXPORT
  #else 
    #if defined (_USRDLL) || (RT_OS_BUILD_DLL)||(_WINDLL)
      #define RT_API_EXPORT __declspec(dllexport)
    #else 
      #define RT_API_EXPORT __declspec(dllimport)
    #endif // _USRDLL || RT_OS_BUILD_DLL
  #endif // _LIB || RT_OS_BUILD_LIB
 
  #ifdef RT_API_EXPORT_NONEED		// for not expect plugin exports api add by oz
	#undef  RT_API_EXPORT
	#define RT_API_EXPORT
  #endif

#else
  #define RT_API_EXPORT 
#endif // !RT_WIN32


#if defined (RT_WIN32)
  #define RT_OS_SEPARATE '\\'
#elif defined (RT_UNIX) || defined(RT_MACOS) || defined(RT_IOS)
  #define RT_OS_SEPARATE '/'
#endif

#define RT_BIT_ENABLED(dword, bit) (((dword) & (bit)) != 0)
#define RT_BIT_DISABLED(dword, bit) (((dword) & (bit)) == 0)
#define RT_BIT_CMP_MASK(dword, bit, mask) (((dword) & (bit)) == mask)
#define RT_SET_BITS(dword, bits) (dword |= (bits))
#define RT_CLR_BITS(dword, bits) (dword &= ~(bits))

//////////////////////////////////////////////////////////////////////
// Assert
//////////////////////////////////////////////////////////////////////

#ifdef RT_WIN32
  #include <crtdbg.h>
  #ifdef _DEBUG
    #define RT_DEBUG
  #endif // _DEBUG

  #if defined (RT_DEBUG)
    #define RT_ASSERTE _ASSERTE
  #endif // RT_DEBUG
#endif // RT_WIN32

#ifdef RT_UNIX
  #include <assert.h>
  #if defined (RT_DEBUG) && !defined (RT_DISABLE_ASSERTE)
    #define RT_ASSERTE assert
  #endif // RT_DEBUG
#endif // RT_UNIX



#ifdef RT_DISABLE_ASSERTE
#include "debug.h"
#ifdef RT_ASSERTE
#undef RT_ASSERTE
#endif
#define RT_ASSERTE(expr) \
  do { \
  if (!(expr)) { \
		LOG_ERROR(__FILE__ << ":" << __LINE__ << " Assert failed: " << #expr); \
  } \
  } while (0)
#endif // RT_DISABLE_ASSERTE

#ifndef RT_ASSERTE
#define RT_ASSERTE(expr) 
#endif // RT_ASSERTE

  //#define RT_ASSERTE_THROW RT_ASSERTE

#ifdef RT_DISABLE_ASSERTE
#define RT_ASSERTE_RETURN(expr, rv) \
  do { \
  if (!(expr)) { \
  LOG_ERROR(__FILE__ << ":" << __LINE__ << " Assert failed: " << #expr); \
  return rv; \
  } \
  } while (0)

#define RT_ASSERTE_RETURN_VOID(expr) \
  do { \
  if (!(expr)) { \
		LOG_ERROR(__FILE__ << ":" << __LINE__ << " Assert failed: " << #expr); \
  return; \
  } \
  } while (0)
#else
#define RT_ASSERTE_RETURN(expr, rv) \
  do { \
	  RT_ASSERTE((expr)); \
  if (!(expr)) { \
		LOG_ERROR(__FILE__ << ":" << __LINE__ << " Assert failed: " << #expr); \
		  return rv; \
  } \
  } while (0)

#define RT_ASSERTE_RETURN_VOID(expr) \
  do {  \
  RT_ASSERTE((expr)); \
	  if (!(expr)) { \
				LOG_ERROR(__FILE__ << ":" << __LINE__ << " Assert failed: " << #expr); \
			  return; \
	  } \
  } while (0)

#endif // RT_DISABLE_ASSERTE

// mainly copied from ace/Basic_Types.h
// Byte-order (endian-ness) determination.
# if defined (BYTE_ORDER)
#   if (BYTE_ORDER == LITTLE_ENDIAN)
#     define RT_LITTLE_ENDIAN 0x0123
#     define RT_BYTE_ORDER RT_LITTLE_ENDIAN
#   elif (BYTE_ORDER == BIG_ENDIAN)
#     define RT_BIG_ENDIAN 0x3210
#     define RT_BYTE_ORDER RT_BIG_ENDIAN
#   else
#     error: unknown BYTE_ORDER!
#   endif /* BYTE_ORDER */
# elif defined (_BYTE_ORDER)
#   if (_BYTE_ORDER == _LITTLE_ENDIAN)
#     define RT_LITTLE_ENDIAN 0x0123
#     define RT_BYTE_ORDER RT_LITTLE_ENDIAN
#   elif (_BYTE_ORDER == _BIG_ENDIAN)
#     define RT_BIG_ENDIAN 0x3210
#     define RT_BYTE_ORDER RT_BIG_ENDIAN
#   else
#     error: unknown _BYTE_ORDER!
#   endif /* _BYTE_ORDER */
# elif defined (__BYTE_ORDER)
#   if (__BYTE_ORDER == __LITTLE_ENDIAN)
#     define RT_LITTLE_ENDIAN 0x0123
#     define RT_BYTE_ORDER RT_LITTLE_ENDIAN
#   elif (__BYTE_ORDER == __BIG_ENDIAN)
#     define RT_BIG_ENDIAN 0x3210
#     define RT_BYTE_ORDER RT_BIG_ENDIAN
#   else
#     error: unknown __BYTE_ORDER!
#   endif /* __BYTE_ORDER */
# else /* ! BYTE_ORDER && ! __BYTE_ORDER */
  // We weren't explicitly told, so we have to figure it out . . .
#   if defined (i386) || defined (__i386__) || defined (_M_IX86) || \
     defined (vax) || defined (__alpha) || defined (__LITTLE_ENDIAN__) ||\
     defined (ARM) || defined (_M_IA64)
    // We know these are little endian.
#     define RT_LITTLE_ENDIAN 0x0123
#     define RT_BYTE_ORDER RT_LITTLE_ENDIAN
#   else
    // Otherwise, we assume big endian.
#     define RT_BIG_ENDIAN 0x3210
#     define RT_BYTE_ORDER RT_BIG_ENDIAN
#   endif
# endif /* ! BYTE_ORDER && ! __BYTE_ORDER */

#ifdef RT_WIN32
#	define RT_LL_PREFIX "I64"
#else
#	define RT_LL_PREFIX "ll"
#endif


//#include <stddef.h>  // for NULL, size_t
#if !(defined(_MSC_VER) && (_MSC_VER < 1600))
#include <stdint.h>  // for uintptr_t
#endif

#if !defined(INT_TYPES_DEFINED)
#define INT_TYPES_DEFINED
#ifdef COMPILER_MSVC
typedef unsigned __int64 uint64;
typedef __int64 int64;
#ifndef INT64_C
#define INT64_C(x) x ## I64
#endif
#ifndef UINT64_C
#define UINT64_C(x) x ## UI64
#endif
#define INT64_F "I64"
#else  // COMPILER_MSVC
// On Mac OS X, cssmconfig.h defines uint64 as uint64_t
// TODO(fbarchard): Use long long for compatibility with chromium on BSD/OSX.
#if defined(WEBRTC_MAC) && !defined(WEBRTC_IOS)
typedef uint64_t uint64;
typedef int64_t int64;
#ifndef INT64_C
#define INT64_C(x) x ## LL
#endif
#ifndef UINT64_C
#define UINT64_C(x) x ## ULL
#endif
#define INT64_F "l"
#elif defined(__LP64__)
typedef unsigned long uint64;  // NOLINT
typedef long int64;  // NOLINT
#ifndef INT64_C
#define INT64_C(x) x ## L
#endif
#ifndef UINT64_C
#define UINT64_C(x) x ## UL
#endif
#define INT64_F "l"
#else  // __LP64__
typedef unsigned long long uint64;  // NOLINT
typedef long long int64;  // NOLINT
#ifndef INT64_C
#define INT64_C(x) x ## LL
#endif
#ifndef UINT64_C
#define UINT64_C(x) x ## ULL
#endif
#define INT64_F "ll"
#endif  // __LP64__
#endif  // COMPILER_MSVC
typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;  // NOLINT
typedef short int16;  // NOLINT
typedef unsigned char uint8;
typedef signed char int8;
#endif  // INT_TYPES_DEFINED


#endif // !RTCDEFINES_H
