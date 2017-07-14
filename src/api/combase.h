#ifndef RT_COMMON_BASE_H
#define RT_COMMON_BASE_H
#define __BUILD_LIVECAST__

#include "defines.h"
#include "errorcode.h"

typedef int				AVDHandle;				/**< 句柄类型  */
typedef int				Result;					/**< 返回值类型  */

#define Max_RoomId		64						/**< 房间Id最大字符长度：服务器缺省是40 */
#define Max_UserId		40						/**< 用户Id最大字符长度 */
#define Max_UserData	128						/**< 用户数据最大字符长度 */
#define Max_DeviceId	256						/**< 设备Id最大字符长度 */
#define Max_StreamId    64						/**< 媒体流Id最大字符长度 */
#define Max_UserName	32						/**< 用户名最大字符长度 */
#define Max_DeviceName	64						/**< 设备名最大字符长度 */
#define Max_DeviceDescription	128				/**< 设备描述最大字符长度 */
#define Max_RoomName	256						/**< 房间名最大字符长度 */
#define Max_Password	16						/**< 密码最大字符长度 */
#define Max_Message		256						/**< 消息最大字符长度 */
#define Max_Address		64						/**< 最大地址长度 */
#define Max_StringEnum	32						/**< 最大字符串枚举长度 */
#define Max_UrlLen		256						/**< 最大url的字符长度 */
#define Max_TimeLen		64						/**< 最大时间字符串长度 */

//// Define platform specific window types.
#if defined(RT_LINUX) && !defined(RT_ANDROID)
typedef unsigned long Window;  // Avoid include <X11/Xlib.h>.
#elif defined(RT_WIN32)
// We commonly include win32.h in webrtc/base so just include it here.
#ifndef NOMINMAX 
	#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>  // Include HWND, HMONITOR.
#elif defined(RT_MAC) && !defined(RT_IOS)
typedef unsigned int CGWindowID;
typedef unsigned int CGDirectDisplayID;
#endif

///以下为一组二进制操作函数
/** 判断uint32的某特定位是否已经改变
* @param[in] oldValue 原值。
* @param[in] newValue 新值。
* @param[in] mask 特定位标志。
* @return 返回是否改变。
*/
#define testFlagChange(oldValue,newValue,mask) \
	(0 != ((oldValue^newValue) & mask))

/** 判断uint32的某特定位是否为1
* @param[in] uValue 测试值。
* @param[in] mask 特定位标志。
* @return 返回是否为1。
*/
#define isFlagSet(uValue,mask) \
	(0 != (uValue & mask))
/** 设置uint32的某特定位为1
* @param[in,out] uValue 设置值。
* @param[in] mask 特定位标志。
*/
#define avdSetFlag(uValue,mask) \
	uValue |= mask;
/** 清除uint32的某特定位为0
* @param[in,out] uValue 设置值。
* @param[in] mask 特定位标志。
*/
#define clearFlag(uValue,mask) \
	uValue &= ~mask;

/** 获取uint32的某些特定位
* @param[in,out] uValue 设置值。
* @param[in] mask 特定位标志。
*/
#define getFlag(uValue,mask) \
	uValue &mask;

/** 取反uint32的某特定位
* @param[in,out] uValue 设置值。
* @param[in] mask 特定位标志。
*/
#define xorFlag(uValue,mask) \
	uValue ^= mask;


/////////////////////////////////
#if defined(__cplusplus) || defined(WEBRTC_IOS)

#define NAMESPACE_TEE3_BEGIN	\
namespace tee3 {

#define NAMESPACE_TEE3_END		\
}

#define NAMESPACE_TEE3_AVD_BEGIN\
	NAMESPACE_TEE3_BEGIN		\
	namespace avd {

#define NAMESPACE_TEE3_AVD_END	\
	}							\
	NAMESPACE_TEE3_END


#define USE_NAMESPACE_TEE3		\
	using namespace tee3;

#define USE_NAMESPACE_TEE3_AVD	\
	using namespace tee3::avd;

#else
	#define NAMESPACE_TEE3_BEGIN
	#define NAMESPACE_TEE3_END
	#define NAMESPACE_TEE3_AVD_BEGIN
	#define NAMESPACE_TEE3_AVD_END
	#define USE_NAMESPACE_TEE3
	#define USE_NAMESPACE_TEE3_AVD

#endif //__cplusplus

NAMESPACE_TEE3_AVD_BEGIN

/**
* 设备状态.
*/
enum DeviceStatus
{
	ds_none,								/**< 无设备状态（如设备被拔出等） */
	ds_ready,								/**< 设备初始化状态 （如设备插入或从发布状态关闭发布） */
	ds_published,							/**< 设备打开发布状态  */
	ds_muted,								/**< 设备静默状态  */
};
typedef DeviceStatus MicrophoneStatus;		/**< 麦克风状态  */
typedef DeviceStatus CameraStatus;			/**< 摄像头状态  */
typedef DeviceStatus ScreenStatus;			/**< 屏幕窗口共享状态  */


/**
* 视频质量类型.
*/
enum VideoQuality {
	quality_low = (1 << 0),					/**< 流畅  */
	quality_normal = (1 << 1),				/**< 标清  */
	quality_high = (1 << 2),				/**< 高清  */
};	
/**
* 视频宽高比类型.
*/
enum VideoRatio {
	ratio_4_3,								/**< 4:3视频  */
	ratio_16_9,								/**< 16:9视频  */
	ratio_cif,								/**< CIF比例视频  */
	ratio_default = ratio_4_3,
};
/**
* 视频角度类型.
*/
enum VideoRotation {
	rotation_0 = 0,								
	rotation_90 = 90,							
	rotation_180 = 180,	
	rotation_270 = 270,							
	rotation_default = rotation_0,
};
/**
* 视频编解码器类型.
*/
enum VideoCodec {
	codec_vp8,								/**< VP8编解码器类型  */
	codec_h264,								/**< H264编解码器类型  */
	codec_vp9,								/**< VP9编解码器类型  */
	codec_default = codec_vp8,
};
/**
* 视频主辅流类型.
*/
enum StreamType {
	stream_main,							/**< 主流  */
	stream_assist1,							/**< 辅流1  */
	stream_assist2,							/**< 辅流2  */
};
/**
* 摄像头类型.
*/
enum CameraType {
	ct_unknown,								/**< 未知类型  */
	ct_front,								/**< 前置摄像头  */
	ct_back,								/**< 后置摄像头  */
};

enum ScreenType {
	st_unknown,								/**< 未知类型  */
	st_desktop ,							/**< 桌面  */
	st_application,							/**< 应用  */
};

/** 房间模式 */
enum RoomMode {
	clear_set = (1 << 0),					// use uint32ernal
	rm_mcu = (1 << 31),						/**< mcu模式  */
	rm_p2p = rm_mcu + clear_set,			/**< p2p模式  */
	rm_free = (1 << 30),					/**< 自由发言模式  */
	rm_host = rm_free + clear_set,			/**< 主持人模式  */
	rm_temperary = (1 << 29),				/**< 临时房间模式  */
	rm_persisist = rm_temperary + clear_set,/**< 永久房间模式  */
};
/** 房间状态 */
enum RoomStatus {
	rs_scheduled,							/**< 已经安排好  */
	rs_opening,								/**< 正在进行中  */
	rs_locked,								/**< 已锁定房间：锁定房间后，新用户无法加入房间  */
	rs_closed,								/**< 已关闭房间  */
};
/** 房间网络状态 */
enum ConnectionStatus {
	cs_ready,								/**< 初始状态  */
	cs_connecting,							/**< 正在连接服务器  */
	cs_connected,							/**< 已经连接上服务器  */
	cs_connectFailed,						/**< 连接服务器失败  */
};
/**< 音频文件类型  */
enum FileFormats {
	kFileFormatWavFile = 1,					/**< WavFile  */
	kFileFormatPcm16kHzFile = 7,			/**< Pcm16kHzFile  */
	kFileFormatPcm8kHzFile = 8,				/**< Pcm8kHzFile  */
	kFileFormatPcm32kHzFile = 9				/**< Pcm32kHzFile  */
};
/**
* 用户状态结构.
*
* 用户状态定义了用户的各种状态集合信息。
*/
struct UserStatus {
public:
	/** 用户状态位 */
	enum Status {
		microphone_has = 1 << 31,			/**< 拥有麦克风设置位  */
		microphone_on = 1 << 30,			/**< 麦克风打开设置位  */
		camera_has = 1 << 29,				/**< 拥有摄像头设置位  */
		camera_on = 1 << 28,				/**< 摄像头打开设置位  */
		screen_on = 1 << 25,				/**< 屏幕窗口共享设置位  */

		status_all = 0xFFFFFFFF,			/**< 所有用户状态位  */
	};
public:
	/** 判断是否有麦克风设备 */
	bool hasMicrophone() const { return isFlagSet(status, microphone_has); }
	/** 判断是否有摄像头设备 */
	bool hasCamera() const { return isFlagSet(status, camera_has); }
	/** 判断是否麦克风已打开 */
	bool isMicrophoneOn() const { return isFlagSet(status, microphone_on); }
	/** 判断是否摄像头已打开 */
	bool isCameraOn() const { return isFlagSet(status, camera_on); }
	/** 判断是否屏幕窗口共享已开启 */
	bool isScreenOn() const { return isFlagSet(status, screen_on); }
	/** 设置用户状态
	*
	* @param[in] flag 添加的用户状态。
	*
	* @note 添加用户状态是在原有用户状态上添加用户状态；
	* @sa Status
	*/
	void setStatus(Status flag) { avdSetFlag(status, flag); }
	/** 清理用户状态
	*
	* @param[in] flag 清理的用户状态。
	*
	* @note 可以清理部分用户状态；
	* @sa Status
	*/
	void clearStatus(Status flag) { clearFlag(status, flag); }
public:
	uint32  status;						/**< 用户状态集合  */
public:
	UserStatus(uint32 val) : status(val) {}
	UserStatus() : status(0) {}
	~UserStatus() {}
    bool operator==(const UserStatus& from) const { return status == from.status; };
};

/** 引擎选项
*
* @note 引擎选项枚举列出了用于设置引擎的选项，在以下函数中使用。
* @sa IAVDEngine.setOption
* @sa IAVDEngine.getOption
*/
enum EngineOption {
	eo_camera_mode_frontback, 			/**< 值类型为bool, 字符类型为"true" or "false"：表示摄像头使用模式：前后置摄像头模式(true)和多摄像头模式（false）。
										多摄像头模式：本机的所有摄像头将展现在房间中；可以同时预览发布多个摄像头；桌面端本地应用和网页端应用多采用此种方式。
										前后置摄像头模式：本机的前后置摄像头将作为一个摄像头展现在房间中；同一时刻只能打开一个摄像头；普通移动端采用此种方式。
										*/
	eo_camera_capability_default,		/**< 值类型为CameraCapability, 字符类型为"{\"width\":640,\"height\":480,\"maxFPS\":10}"：表示摄像头缺省采集参数。当前系统默认的采集
										参数为：windows桌面版和web网页版{640，480，10}；android/ios移动版{320，240，10}。
										*/
	eo_video_quality_publish_default,	/**< 值类型为VideoQuality, 字符类型为"low" or "normal" or "high"：表示摄像头发布缺省采用的视频质量，与camera_capability_default的设置相互影响。
										当前系统默认的采集参数为：windows桌面版和web网页版 普通质量；android/ios移动版 流畅质量。
										*/
	eo_video_ration_default,			/**< 值类型为VideoRatio, 字符类型为"4_3" or "16_9" or "cif"：表示视频缺省宽高比。
										当前系统默认的采集参数为：4:3。
										*/
	eo_video_quality_subscribe_default,	/**< 值类型为VideoQuality, 字符类型为"low" or "normal" or "high"：表示订阅视频缺省采用的视频质量。
										当前系统默认的采集参数为：windows桌面版和web网页版 普通质量；android/ios移动版 流畅质量。
										*/
	eo_video_publish_svc_default,		/**< 值类型为bool, 字符类型为"true" or "false"：表示视频发布时缺省是否采用多流。
										当前系统默认的采集参数为：false。
										*/
	eo_video_codec_priority,			/**< 值类型为VideoCodec, 字符类型为"h264"，"vp8"，"vp9"：表示视频编码优先采用h264，vp8，vp9编解码。
										当前系统默认参数值为：vp8。
										*/
	eo_demo_urlbase_liverecord,			/**< 值类型为String：表示演示直播和录制的服务器地址，只能获取。
										*/
	eo_data_channel_tcp_priority,		/**< 值类型为bool, 字符类型为"true" or "false"：表示房间中数据通道用的网络连接类型：TCP优先(true)和UDP优先（false），
										缺省为UDP优先。 当前如果选TCP优先，将只是用TCP数据连接，UDP即使设置了不使用。
										*/
	eo_network_proxy,					/**< 值类型为bool, 字符类型为"true" or "false"：表示启用或不启用，建议局域网或者简单的环境不使用代理。
										当前系统默认参数值为：true。
										*/
	eo_video_resolution_16balign,		/**< 值类型为bool, 字符类型为"true" or "false"：表示启用或不启用视频分辨率16字节严格对齐模式；在16字节对齐模式下，分辨率的宽和高都会向对齐的分辨率靠，
										如1080-->1088,540-->544,180-->176等，有些硬件设备（某些android手机）只支持16字节对齐的硬件编解码，android上可以考虑开启此选项来做更好的设备兼容性和编码效率。
										当前系统默认参数值为：false。
										--------- 10
										*/
	eo_video_swapwh_by_rotation,		/**< 值类型为bool, 字符类型为"true" or "false"：表示视频publish或preview时设置的分辨率根据输入视频旋转角度变换宽高：变换宽高(true)和不变换宽高严格按设置分辨率裁剪拉伸（false），
										* 缺省为变换宽高。
										* 未实现。
										*/
	eo_video_codec_hw_priority,			/**< 值类型为bool, 字符类型为"true" or "false"：表示是否优先使用硬件编码：优先使用硬件编码(true)和优先使用软件编码（false），
										* 缺省为硬件编码优先。
										* 未实现。
										*/

	eo_audio_aec_LowLatencyModeDelayEstimate, /**< android系统可用；值类型为int, 取值：[30, 400]; 字符类型数值字符串如"50"：表示AEC延时下限，用于回音消除。
										* 缺省为 80； 回音消除算法与设备的麦克风和扬声器距离，及会场大小都有关系，有可能需要特别调试。
										*/
	eo_audio_aec_HighLatencyModeDelayEstimate, /**< android系统可用；值类型为int, 取值：[30, 400]; 字符类型数值字符串如"50"：表示AEC延时上限，用于回音消除。
										* 缺省为 160； 回音消除算法与设备的麦克风和扬声器距离，及会场大小都有关系，有可能需要特别调试。
										*/

	eo_audio_agc_PlayoutGainMultipleValue, /**< 值类型为float, 取值：[0.1, 10.0]; 字符类型数值字符串如"2.0"：表示语音播放时的增益倍数。 
										* 缺省为 1.0。
										*/
					
	eo_audio_aec_AudioDelayOffset,		/**< 值类型为int, 取值：[0, 500]; 字符类型数值字符串如"50"：表示AEC估计延时，用于回音消除。
										* 缺省为 0； 回音消除算法与设备的麦克风和扬声器距离，及会场大小都有关系，有可能需要特别调试。  no use in ios.
										* | delay | in ms between AnalyzeReverseStream() receiving a far-end frame and 
										* ProcessStream() receiving a near-end frame containing the corresponding echo.
										* On the client-side this can be expressed as
										*		delay = (t_render - t_analyze) + (t_process - t_capture)
										*/
    eo_audio_aec_Enable,				/**< 值类型为bool, 字符类型为"true" or "false"：表示启用或不启用, 一般情况下都需要回音消除算法，当需要录制音乐的时候，建议关闭。
                                        当前系统默认参数值为：true。
                                        */
	eo_audio_aec_DAEcho_Enable,			/**< 值类型为bool, 字符类型为"true" or "false"：表示在回音消除时启用或不启用延时消除算法，此算法较耗费cpu，但效果会较好，移动端缺省不启用，如果特殊需要就启用。
										当前系统默认参数值为：false。 no use in ios.
										*/
	eo_audio_noiseSuppression_Enable,	/**< 值类型为bool, 字符类型为"true" or "false"：表示在音频采集时是否使用噪音抑制，抑制后会消除部分噪音。
										当前系统默认参数值为：true。
										*/
	eo_audio_autoGainControl_Enable,	/**< 值类型为bool, 字符类型为"true" or "false"：表示在音频采集时是否启用自动增益，增益打开后会自动调整麦克风采集音频大小。
										当前系统默认参数值为：true。 
										*/
	eo_audio_highpassFilter_Enable,		/**< 值类型为bool, 字符类型为"true" or "false"：表示在音频采集时是否启用高通滤波，一些尖锐的声音会被过滤掉。
										当前系统默认参数值为：false。
										*/
	eo_mcu_cluster_route_params,		/**< 值类型为json String：设置分布式集群部署中加房间时mcu服务器的分配路由参数。 
										* 取值格式如下：{"ip_tag":"local","idc_code":""}
										* ip_tag  对应于 rtc_node_addr 中的tag标志，用于区分同一台服务器的多网卡地址，可以自定义，然后在参数中传入。举例的话，比如 'local','internal','dianxin','liantong'
										* idc_code 对应于 rtc_node 中的 idc_code标志，用于区分不同的服务器，唯一，可以自定义。比如北京服务器设置为'beijing'，杭州的设置为'hangzhou'，然后在参数中传入，用于定位到服务器。
										*/

	eo_livecast_appmanageroom = 1000,   /**< 值类型为bool, 字符类型为"true" or "false"：设置房间中发布视频时是否发布混屏后视频；系统默认为true。 no use in ios.
                                        */
/**< 下面的选项预留测试使用，分别表示字符串，整形等自定义参数
	*/
	eo_test_rtc_string_1	  = 2000, 
	eo_test_rtc_int_1, 
};

/** 房间选项
*
* @note 房间选项枚举列出了用于设置房间的选项，在以下函数中使用。
* @sa IRoom.setOption
* @sa IRoom.getOption
*/
enum RoomOption {
	ro_video_drop_data,					/**< 值类型为bool, 字符类型为"true" or "false"：表示接收到视频数据后不解码直接丢弃，一般用于模拟客服端做并发测试。
										 * 默认值为false，设置为true后将丢弃该房间所有接收到的视频数据。
										 * 必须在join调用之前设置才有效。
										 */
	ro_audio_drop_data,					/**< 值类型为bool, 字符类型为"true" or "false"：表示接收到音频数据后不解码直接丢弃，一般用于模拟客服端做并发测试。
										 * 默认值为false，设置为true后将丢弃该房间所有接收到的音频数据。
										 * 必须在join调用之前设置才有效。
										 */
	ro_media_use_dtls,					/**< 值类型为bool, 字符类型为"true" or "false"：表示音视频是否启用DTLS加密，DTLS加密数据更加安全，但是会加重Server
										 * 的负担，在UDP丢包严重的网络环境下，媒体链接需要的时间会更多（30%丢包，链接时间10s-40s）。
										 * 默认值为true，使用的是DTLS加密方式。
										 * 必须在join调用之前设置才有效。
										 */
	ro_room_reconnect_times,			/**< 值类型为int32, 字符类型为"3" or "2"等数字字符：设置每次断线后重连次数。系统默认为重连3次，如需改变需要应用层自己设置。
										 * 在join之前或者之后设置都有效。
										 */
	ro_room_connect_timeout,			/**< 值类型为uint32,  字符类型为"3000" or "5000" ：设置链接超时时间，单位毫秒，必须是正数，系统默认是5000毫秒。
										 * 在join之前或者之后设置都有效。
										 */
	ro_audio_subscribe_mode,			/**< 值类型为String, 音频订阅模式：字符类型为"autosubscribe" or "nosubscribe"：分别表示自动订阅房间中全部打开的音频 
										 * or 不自动订阅音频；缺省为"autosubscribe"自动订阅模式。
										 * 未实现。
										 */
	ro_audio_option_codec,				/**< 值类型为String, 音频编码算法：字符类型为"ISAC" or "opus" or "AAC"：分别表示isac语音编码 和 opus音乐编码 和 AAC语音编码；缺省为"ISAC"。
										 * 必须在join调用之前设置才有效。
										 */
	ro_audio_mixerdata_callback_buffered,/**< 值类型为bool, 字符类型为"true" or "false"：表示房间中混音数据回调是否缓冲为1024sample后回调：缓冲(true)和不缓冲（false），
										  *	缺省为缓冲。
										  * 在join之前或者之后设置都有效。
										  */
	ro_video_default_camera,			/**< 值类型为枚举CameraType, 字符类型为"front" or "back"：表示前后置摄像头模式时房间中默认使用的摄像头：前置(front)和后置（back），
										 *	缺省为前置。
										 * 必须在join调用之前设置才有效。
										 */
	ro_room_auto_rejoin,				/**< 值类型为int32, 字符类型为"true" or "false"：在重连失败之后是否尝试自动重新加入房间，系统默认为true。
										 * 在join之前或者之后设置都有效。
										 */
	ro_room_rejoin_times,				/**< 值类型为uint32, 字符类型为"3" or "2"等数字字符：设置断线重连失败后自动加入的次数，在启用ro_room_auto_rejoin为true后有效。
										 * 系统默认为3，尝试3次重新加入，如果都失败将通过onConnectionStatus接口返回cs_connectFailed状态。
										 * 在join之前或者之后设置都有效。
										 */

	ro_screen_share_fps,				/**< 值类型为uint32, 字符类型为"3" or "10"等数字字符：设置共享屏幕时的帧率，系统默认为5。
										*/
	ro_screen_share_audio,				/**< 值类型为bool, 字符类型为"true" or "false"：设置共享屏幕时的是否共享扬声器的声音；系统默认为false。暂时没实现。
										*/


	ro_video_mixer_published=1000,		/**< 值类型为bool, 字符类型为"true" or "false"：设置简易房间中发布视频时是否发布混屏后视频；系统默认为false。
										*/
};

/** 录制音频类型定义 */
enum RecordAudioType {
	ra_none,							/**< 无音频  */
	ra_user_single,						/**< 同视频用户的单用户音频  */
	ra_user_all,						/**< 整个房间中的所有音频混音  */
};
/** 录制视频类型定义 */
enum RecordVideoType {
	rv_none,							/**< 无视频  */
	rv_main,							/**< 视频主流  */
	rv_ass1,							/**< 视频辅流1  */
	rv_ass2,							/**< 视频辅流2  */
};
/** 媒体文件类型定义 */
enum MediaFileType {
	mt_mp4,								/**< MP4 文件格式，支持h264视频和aac音频封装  */
	mt_webm,							/**< WebM文件格式，支持vp8视频和opus音频封装  */
};
/** 旁路录制状态定义 */
enum RecordStatus {
	rs_created,							/**< 创建录制  */
	rs_recording,						/**< 正在录制（如果相应视频和音频流上传服务器，就会保持到录制中；如没数据也无录制） */
	rs_availble,						/**< 录制完成可查看  */
	rs_removed,							/**< 录制被删除  */
	rs_exception,						/**< 录制异常（如不完整等） */
};
/** 旁路直播频道状态定义 */
enum LiveStatus {
	ls_created,							/**< 创建直播 */
	ls_living,							/**< 正在直播 */
	ls_stopped,							/**< 直播暂停  */
	ls_removed,							/**< 直播被删除  */
	ls_exception,						/**< 直播异常 */
};

/** 客户端直播状态定义 */
enum LivecastStatus {
	lc_init,							/**< 初始状态 */
	lc_connecting,						/**< 正在连接服务器 */
	lc_ready,							/**< 视频可以预览，已经准备好可以直播  */
	lc_livecast,						/**< 正在直播  */
	lc_reconnecting,					/**< 正在与服务器重连接  */
};

/** 视频混频布局类型 */
enum MixerVideoLayoutType {
	vl_auto_overlay,					/**< 自动布局-窗口中重叠视频 */
	vl_auto_segment,					/**< 自动布局-分割窗口排列视频 */
	vl_manual,							/**< 手动布局  */
};

/** AVD功能点定义 */
enum FunctionType {
	ft_none			= 0,				/**< 无功能点  */
	ft_roommanager  = (1 << 0),			/**< 透明通道：一对多，一对一  */
	ft_datachannel	= (1 << 1),			/**< 透明通道：一对多，一对一  */
	ft_chat			= (1 << 2),			/**< 聊天：房间内公聊，私聊 */
	ft_audio		= (1 << 3),			/**< 语音  */
	ft_video		= (1 << 4),			/**< 普通视频（640x480及以下） */
	ft_video_hd		= (1 << 5),			/**< 高清视频 */
	ft_screen		= (1 << 6),			/**< 屏幕共享：应用，桌面 */
	ft_annotation	= (1 << 7),			/**< 注释白板 */
	ft_p2p			= (1 << 10),		/**< P2P房间 */
	ft_record_audio	= (1 << 16),		/**< 录制音频 */
	ft_record_video = (1 << 17),		/**< 录制视频 */
	ft_livebroadcast = (1 << 18),		/**< 旁路直播 */
	//
	ft_platform_android = (1 << 20),	/**< 平台：android系统 */
	ft_platform_ios = (1 << 21),		/**< 平台：ios系统 */
	ft_platform_win = (1 << 22),		/**< 平台：windows桌面系统 */
	ft_platform_mac = (1 << 23),		/**< 平台：mac ox桌面系统 */
	ft_platform_linux = (1 << 24),		/**< 平台：linux桌面系统 */
	ft_platform_web = (1 << 25),		/**< 平台：web js系统 */

	ft_platform_all = ft_platform_android + ft_platform_ios + ft_platform_win + ft_platform_mac + ft_platform_linux + ft_platform_web,
	ft_basic = ft_datachannel + ft_chat + ft_audio + ft_video,
	ft_standard = ft_basic + ft_screen + ft_p2p + ft_record_audio + ft_record_video,
	ft_all = ft_platform_all + ft_standard + ft_video_hd + ft_annotation + ft_livebroadcast + ft_roommanager,
};


/**
* 音频编码信息
*
* 在声音回调的时候使用，明确回调数据的编码信息。
*/
struct AudioCodecInst {
	int pltype;
	char plname[32];
	int plfreq;
	int pacsize;
	int channels;
	int rate;  // bits/sec unlike {start,min,max}Bitrate elsewhere in this file!

	bool operator==(const AudioCodecInst& other) const {
		return pltype == other.pltype &&
			plfreq == other.plfreq &&
			pacsize == other.pacsize &&
			channels == other.channels &&
			rate == other.rate;
	}

	bool operator!=(const AudioCodecInst& other) const {
		return !(*this == other);
	}

	AudioCodecInst& operator=(const AudioCodecInst& other) {
		if (*this != other) {
			pltype = other.pltype;
			//memcpy(plname, other.plname, 32);
			for (int i = 0; i < sizeof(plname); ++i) {
				plname[i] = other.plname[i];
			}
			plfreq = other.plfreq;
			pacsize = other.pacsize;
			channels = other.channels;
			rate = other.rate;
		}
		return *this;
	}
};
/**
* 支持的音频编码格式
*
* 在声音回调的时候使用，以下为目前支持的编码格式。
*/
const AudioCodecInst kTbPcm16Bwb = { 108, "L16", 16000, 320, 1, 256000 }; // default audio data
const AudioCodecInst kTbPcm44Bwb = { 130, "L16", 44100, 882, 1, 512000 }; // for aac encoder
const AudioCodecInst kTbPcm48Bwb = { 131, "L16", 48000, 960, 1, 512000 };
const AudioCodecInst kTbPcm16Stereo = { 112, "L16", 16000, 320, 2, 256000 };
const AudioCodecInst kTbPcm44Stereo = { 135, "L16", 44100, 882, 2, 512000 };
const AudioCodecInst kTbPcm48Stereo = { 136, "L16", 48000, 960, 2, 512000 };
const AudioCodecInst kTbPcmu = { 0, "PCMU", 8000, 160, 1, 64000 };
const AudioCodecInst kTbPcma = { 8, "PCMA", 8000, 160, 1, 64000 };
const AudioCodecInst kTbPcmuStereo = { 110, "PCMU", 8000, 160, 2, 64000 };
const AudioCodecInst kTbPcmaStereo = { 118, "PCMA", 8000, 160, 2, 64000 };
const AudioCodecInst kTbIsacWb = { 103, "ISAC", 16000, 480, 1, 32000 };
const AudioCodecInst kTbIsacSwb = { 104, "ISAC", 32000, 960, 1, 56000 };
const AudioCodecInst kTbIlbc = { 102, "ILBC", 8000, 240, 1, 13300 };
const AudioCodecInst kTbOpus = { 120, "opus", 48000, 960, 2, 64000 };

/**
* 网络统计信息结构.
*
* 网络统计信息结构定义了网络传输中的统计信息。
*/
struct NetworkStats {
	int bps_received;				/**< 当前接收带宽: 20s计算  */
	int bps_sent;					/**< 当前发送带宽: 20s计算  */
	int lostpercent;				/**< 当前丢包率: 20s计算  */
	int64 bytes_received;			/**< 总共已接收字节数  */
	int64 bytes_sent;				/**< 总共已发送字节数  */
	int64 packets_lost;				/**< 总共已丢包数  */
	int64 packets_received;			/**< 总共已接收数据包  */
	int64 packets_sent;				/**< 总共已发送数据包  */

	NetworkStats() : bps_received(0), bps_sent(0), lostpercent(0)
		, bytes_received(0), bytes_sent(0)
		, packets_lost(0), packets_received(0), packets_sent(0) {
	}
	virtual void Clear() {
		bps_received = 0;
		bps_sent = 0;
		lostpercent = 0;
		bytes_received = 0;
		bytes_sent = 0;
		packets_lost = 0;
		packets_received = 0;
		packets_sent = 0;
	}
};

#define FOURCC(a, b, c, d) ( \
	(static_cast<uint32>(a)) | (static_cast<uint32>(b) << 8) | \
	(static_cast<uint32>(c) << 16) | (static_cast<uint32>(d) << 24))

enum FourCC {
	FOURCC_I420 = FOURCC('I', '4', '2', '0'),
	FOURCC_YV12 = FOURCC('Y', 'V', '1', '2'),
	FOURCC_NV21 = FOURCC('N', 'V', '2', '1'),
	FOURCC_NV12 = FOURCC('N', 'V', '1', '2'),
	FOURCC_YUY2 = FOURCC('Y', 'U', 'Y', '2'),
	FOURCC_UYVY = FOURCC('U', 'Y', 'V', 'Y'),
	FOURCC_MJPG = FOURCC('M', 'J', 'P', 'G'),
	FOURCC_ARGB = FOURCC('A', 'R', 'G', 'B'),
	FOURCC_24BG = FOURCC('2', '4', 'B', 'G'),
	// h264
	FOURCC_H264 = FOURCC('H', '2', '6', '4'),
};


enum VideoEncodedType {
	VET_YUV420 = 0,
	VET_VP8 = 1,
	VET_H264 = 2,
	VET_VP9 = 3,
	VET_H265 = 4,
};
enum AudioEncodedType {
	AET_PCM = 0,
	AET_AAC = 1,
	AET_VORBISE = 2,
	AET_OPUS = 3,
	AET_ISAC = 4,
	AET_PCMA = 5,
	AET_PCMU = 6,
};
enum AudioRecordedType {
	ART_NONE = 0,
	ART_ALL = 1,
	ART_ONLY_SELF = 2,
	ART_ONLY_OTHERS = 3,
};

NAMESPACE_TEE3_AVD_END

#endif//RT_COMMON_BASE_H
