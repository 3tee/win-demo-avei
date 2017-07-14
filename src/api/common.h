#ifndef RT_COMMON_H
#define RT_COMMON_H

#include "combase.h"
#include <string>
#include <vector>
#include <sstream>

NAMESPACE_TEE3_BEGIN
typedef std::string String;					/**< tee3定义的String类型 */
typedef String		RoomId;					/**< 房间号类型，唯一标识房间内一个房间，长度不大于Max_RoomId定义；定义如下：[RId_P|T_H|F_P|M_type_guid]；各分段的含义为： */
typedef String		DeviceId;				/**< 设备号类型，唯一标识房间内一个设备，长度不大于Max_DeviceId定义；定义如下：userId+"_"+md5[os.getdeviceid() or os.getsystemid()] */
typedef String		UserId;					/**< 用户号类型，唯一标识房间内一个用户，长度不大于Max_UserId定义；由应用层定义传递给AVD SDK，要求保证房间内唯一  */
typedef String		StreamId;				/**< 媒体流Id类型，SDK内部使用 */
typedef DeviceId	RecordId;				/**< 录制ID类型  */
typedef DeviceId	LiveId;					/**< 直播频道ID类型  */
typedef DeviceId	AnnotationId;			/**< 直播频道ID类型  */
///
extern RT_API_EXPORT const UserId	AVD_userId_allUser;			/**< AVD定义的虚假用户Id，用于标识房间中所有用户  */
extern RT_API_EXPORT const UserId	AVD_userId_allUserWithoutMe;/**< AVD定义的虚假用户Id，用于标识房间中除了自己外的所有用户  */
NAMESPACE_TEE3_END

NAMESPACE_TEE3_AVD_BEGIN

/**
* 房间信息结构.
*
* 房间信息定义了安排房间时的所有信息。
*/
struct RoomInfo {
	RoomInfo() : startTime(0), duration(0), bandwidth(0),
		maxAttendee(0), maxAudio(0), maxVideo(0), roomMode(0), status(rs_scheduled) {}
	RoomId roomId;							/**< 房间Id，SDK生成，唯一标示一个房间；长度取Max_RoomId */
	String roomName;						/**< 房间名，应用层创建房间时设置 */
	String appRoomId;						/**< 房间应用层Id，应用层创建房间时设置，应用层可用于标示房间；长度取Max_RoomId */
	String roomTopic;						/**< 房间主题，应用层创建房间时设置 */
	String ownerId;							/**< 房间创建者UserId；长度取Max_UserId */
	String hostId;							/**< 房间主持人UserId；长度取Max_UserId */
	String hostPassword;					/**< 房间主持人密码；长度取Max_Password */
	String confPassword;					/**< 房间密码，必须与主持人密码不同；长度取Max_Password */
	uint32 startTime;						/**< 房间起始时间 */
	uint32 duration;						/**< 房间延续时间,单位是秒 */
	uint32 bandwidth;						/**< 房间带宽限制,单位是kps */
	uint32 maxAttendee;						/**< 房间最大用户数限制 */
	uint32 maxAudio;						/**< 房间最大音频数限制,即同时打开话筒数 */
	uint32 maxVideo;						/**< 房间最大视频数限制,即同时打开摄像头数 */
	uint32 roomMode;						/**< 房间模式集，可选参考RoomMode，缺省为p2p */
	RoomStatus status;
public:
	/** 判断是mcu模式房间 */
	bool isMcu() const { return isFlagSet(roomMode, rm_mcu); }
	/** 判断是自由发言模式房间 */
	bool isFree() const { return isFlagSet(roomMode, rm_free); }
	/** 判断是临时房间 */
	bool isTemperary() const { return isFlagSet(roomMode, rm_temperary); }
	/** 设置房间模式
	* @param[in] flag 设置值。
	* @note 每个模式必须单独设置,不能通过 | 操作合并设置。
	*/
	void setRoomMode(RoomMode flag) {
		if (isFlagSet(flag, clear_set)) {
			uint32 dwFlag = flag;
			clearFlag(dwFlag, clear_set);
			clearFlag(roomMode, dwFlag);
		}
		else {
			avdSetFlag(roomMode, flag);
		}
	}
};
/**
* 房间信息集合类型.
*/
typedef std::vector<RoomInfo> RoomInfosType;

/**
* 设备信息结构.
*
* 设备信息结构定义了设备基本信息，此处设备包括麦克风、扬声器、摄像头和屏幕窗口。
*/
struct Device {
	DeviceId id;							/**< 设备Id，唯一标识一个设备；产生规则：音频设备为与用户相关唯一号；视频设备userId+deviceId(MD5(idd) or mobileId+0(front)1(back)) */
	String   name;							/**< 设备名称，应用层可用于设备显示  */
	DeviceStatus status;					/**< 设备状态  */
	Device() :status(tee3::avd::ds_none) {}
	/** 判断设备是否有效 */
	bool isValid() const { return !id.empty(); }
	bool operator ==(const Device& right) const { return this->id == right.id; }
};

/**
* 视频采集信息结构.
*
* 视频采集结构定义了视频采集相关参数，包含视频分辨率和帧率。
*/
struct CameraCapability {
	uint32 width;							/**< 视频宽度  */
	uint32 height;							/**< 视频高度  */
	uint32 maxFPS;							/**< 视频帧率  */
	uint32 rotation;						/**< 视频角度  */
	CameraCapability() :width(0), height(0), maxFPS(0), rotation(0) {}
	CameraCapability(uint32 w, uint32 h, uint32 fps) :width(w), height(h), maxFPS(fps), rotation(0) {}
	CameraCapability(uint32 w, uint32 h, uint32 fps, uint32 rotation) :width(w), height(h), maxFPS(fps), rotation(rotation) {}
	/** 判断分辨率是否有效 */
	bool isValid() const { return 0 != width && 0 != height; }
	bool operator==(const CameraCapability& o) const {
		return (width == o.width) && (height == o.height) && (maxFPS == o.maxFPS) && (rotation == o.rotation);
	}
	CameraCapability& operator=(const CameraCapability& o) {
		if (this != &o) {
			width = o.width;
			height = o.height;
			maxFPS = o.maxFPS;
			rotation = o.rotation;
		}
		return *this;
	}
	friend std::ostream& operator <<(std::ostream& ost, const CameraCapability& o) {
		ost << "cap:[" << "width = " << o.width << ",height = " << o.height << ",maxFPS = " << o.maxFPS << ",rotation = " << o.rotation << "],";
		return ost;
	}
};
/**
* 扬声器信息结构.
*/
struct Speaker : public Device {};
/**
* 扬声器集合类型.
*/
typedef std::vector<Speaker> SpeakersType;
/**
* 麦克风信息结构.
*/
struct Microphone : public Device {};
/**
* 麦克风集合类型.
*/
typedef std::vector<Microphone> MicrophonesType;

/**
* 视频采集集合类型.
*/
typedef std::vector<CameraCapability> CapabilitiesType;

/**
* 视频设备选项类.
*
* 视频设备选项类定义了视频设备本身所固有的视频质量、分辨率等参数集合。
*/
class RT_API_EXPORT VideoDeviceOptions {
public:
	VideoDeviceOptions();
	VideoDeviceOptions(uint32 aoptionset);
	/** 判断设备是否支持指定视频质量
	* @param[in] value 指定视频质量。
	* @return 返回是否支持。
	*/
	bool hasQuality(VideoQuality value) const;
	bool hasCapability(CameraCapability cap) const;
	/** 获取设备支持的分辨率集合
	* @return 返回分辨率集合。
	*/
	CapabilitiesType getCapabilities() const;
	/** 设置设备支持的分辨率集合，SDK内部使用
	* @param[in] caps 分辨率集合。
	* @return 返回设置是否成功。
	*/
	Result setCapabilities(const CapabilitiesType& caps);
public:
	/** 获取指定分辨率的视频质量分类，SDK内部使用
	* @param[in] cap 指定分辨率。
	* @return 返回视频质量。
	*/
	static VideoQuality getQualityByCapability(const CameraCapability& cap);
	/** 获取指定分辨率的宽高比类型，SDK内部使用
	* @param[in] cap 指定分辨率。
	* @return 返回宽高比类型。
	*/
	static VideoRatio getRatioByCapability(const CameraCapability& cap);
	/** 获取指定视频质量和宽高比的分辨率，SDK内部使用
	* @param[in] quality 指定清晰度。
	* @param[in] ratio 宽高比类型。
	* @return 返回分辨率。
	*/
	static CameraCapability getDefaultCapability(VideoQuality quality, VideoRatio ratio);
public:
	uint32 optionset;	// 位解析：0-1：宽高比例；2-4：视频质量；5-26: high:5-12, normal:13-20, low:21-28；
};

/**
* 视频发布选项类.
*
* 视频发布选项类定义了某视频设备在视频发布时视频流指定的视频质量、分辨率、宽高比和编码方式等参数集合。
*/
class RT_API_EXPORT PublishVideoOptions {
public:
	PublishVideoOptions();
	PublishVideoOptions(uint32 aoptionset);
public:
	/** 判断视频发布中是否支持指定视频质量
	* @param[in] value 指定视频质量。
	* @return 返回是否支持。
	*/
	bool hasQuality(VideoQuality value) const;
	/** 获取视频发布中采用的宽高比类型
	* @return 返回视频宽高比类型。
	*/
	VideoRatio getRatio() const;
	/** 设置视频发布中采用的宽高比类型
	* @param[in] value 指定视频宽高比。
	*/
	void setRatio(VideoRatio value);
	/** 获取视频发布中采用的角度信息类型
	* @return 返回视频角度信息类型。
	*/
	VideoRotation getRotation() const;
	/** 设置视频发布中采用的角度信息类型
	* @param[in] value 指定角度信息。
	*/
	void setRotation(VideoRotation value);
public: // Stream options
	/** 设置指定流的视频发布参数
	*
	* @param[in] type 流类型。
	* @param[in] quality 视频质量。
	* @param[in] codec 编码类型。
	*
	* @note 当前每个摄像头设备根据机器性能最多可以发布三路视频流，分别为主流、辅流1和辅流2，
	* 每路视频流的参数可以根据应用场景设置。chrome浏览器当前仅支持VP8编码；移动端对H264的支持
	* 较好，PC端两种编码都支持良好。
	* @return 返回错误代码。
	*/
	Result setStreamOptions(StreamType type, VideoQuality quality, VideoCodec codec = codec_default);
	/** 设置指定流的视频发布参数
	*
	* @param[in] type 流类型。
	* @param[in] cap 视频分辨率帧率。
	* @param[in] codec 编码类型。
	* @return 返回错误代码。
	*/
	Result setStreamOptions(StreamType type, const CameraCapability& cap, VideoCodec codec = codec_default);
	/** 获取指定流的分辨率帧率 */
	CameraCapability getStreamCapability(StreamType type) const;
	/** 获取指定流的视频质量 */
	VideoQuality getStreamQuality(StreamType type) const;
	/** 获取指定流的编码参数 */
	VideoCodec getStreamCodec(StreamType type) const;
	/** 获取指定流的payload参数，SDK内部使用 */
	int getStreamPayload(StreamType type) const;
public:
	/** 判断视频发布中是否有主流，SDK内部使用 */
	bool hasMainStream() const;
	/** 判断视频发布中是否有辅流1，SDK内部使用 */
	bool hasAssist1() const;
	/** 判断视频发布中是否有辅流2，SDK内部使用 */
	bool hasAssist2() const;
	/** 规范化视频发布选项，SDK内部使用 */
	void normalize(uint32 deviceOptions);
	/** 判断指定流是否支持硬件编码，SDK内部使用 */
	bool isCodecHWSupported(StreamType type) const;
	/** 判断指定流是否支持编码，SDK内部使用 */
	bool isCodecSupported(StreamType type) const;
	/** 获取视频发布中当前入会者优先订阅流，SDK内部使用
	*
	* @param[in] quality 指定视频质量。
	* @param[in] mustQuality 是否必须选指定的视频质量。
	*
	* @note 获取视频发布中当前入会者优先订阅流取决于入会者客户端的情况，可以参考IMVideo.subscribe中的描述。
	* @return 返回视频流。
	*/
	StreamType getPrioritySubscribeStream(VideoQuality quality, bool mustQuality) const;
public:
	uint32 optionset; // 位解析：0-1：宽高比例；2-4：缺省视频质量；5-26: 主流 5-12(0-1 编码，2-7 序号)，辅流1 13-20，辅流2 20-28；
};

/**
* 视频类设备结构.
*
* 视频类设备结构定义了视频类设备的基本信息，主要包含摄像头和屏幕窗口设备。
*/
struct VideoDevice : public Device {
	VideoDevice() :Device(), level(0) {}
	int		level;							/**< 应用层对设备设置的数据，可用于表示视频权重等, 应用层开发可用  */
	String	description;					/**< 应用层对设备的描述，应用层开发可用  */
	VideoDeviceOptions  deviceQualities;	/**< 视频设备质量类型集：摄像头通知时包含摄像头支持的视频质量集合  */
	PublishVideoOptions publishedQualities;	/**< 视频发布质量类型集：设备发布时包含的发布视频的质量集合 */
public:
	/** 判断视频设备是否已处于发布状态 */
	bool isPublished() const {
		return ds_published == status || ds_muted == status;
	}
};
/**
* 视频设备集合类型.
*/
typedef std::vector<VideoDevice> VideoDevicesType;

/**
* 屏幕窗口设备信息结构.
*/
struct ScreenWindow : public VideoDevice {
	ScreenType type;						/**< 窗口类型  */
public:
	ScreenWindow() :type(tee3::avd::st_unknown) {}
};
/**
* 屏幕窗口集合类型.
*/
typedef std::vector<ScreenWindow> ScreensType;

/**
* 视频摄像头信息结构.
*
* 视频摄像头结构定义了摄像头相关参数，主要是摄像头描述信息和视频分辨率信息。
*/
struct Camera : public VideoDevice {
	CameraType type;						/**< 摄像头类型  */
public:
	Camera() :type(tee3::avd::ct_unknown) {}
};
/**
* 视频摄像头集合类型.
*/
typedef std::vector<Camera> CamerasType;

/**
* 语音激励信息结构.
*
* 语音激励信息信息定义了当前房间中本地语音输入和输出能量，以及远端用户的语音能量信息。
* 语音能量幅度：0-9，其中0为无能量，9为最大能量
*/
struct AudioInfo {
	int input_level;						/**< 本地语音输入能量 */
	int output_level;						/**< 本地语音输出能量 */

	typedef std::vector<std::pair<UserId, int> > RemoteUser2LevelType;
	RemoteUser2LevelType active_streams;	/**< 远端用户的语音能量对应表，只包含能量大于0的用户 */

	AudioInfo() :input_level(0), output_level(0) {}
	AudioInfo(int il, int ol, const RemoteUser2LevelType& streams)
		: input_level(il), output_level(ol), active_streams(streams) {}
};

/**
* 房间统计信息结构.
*
* 房间统计信息结构定义了房间级的网络，媒体等统计信息。
*/
struct RoomStats : public NetworkStats {
	int available_receive_bandwidth;/**< 估计接收带宽  */
	int available_send_bandwidth;	/**< 估计发送带宽  */
	tee3::String local_address;		/**< 本地地址  */
	tee3::String remote_address;	/**< 远端地址  */
	tee3::String transport_type;	/**< 传输类型：udp : tcp  */
	RoomStats() :available_receive_bandwidth(0), available_send_bandwidth(0) {}
	void Clear() {
		NetworkStats::Clear();
		available_receive_bandwidth = 0;
		available_send_bandwidth = 0;
		local_address.clear();
		remote_address.clear();
		transport_type.clear();
	}
};

/**
* 媒体统计信息结构.
*
* 媒体统计信息结构定义了媒体级的网络，媒体等统计信息。
*/
struct MediaStats : public NetworkStats {
	tee3::String media_type;		/**< 媒体类型：audio : video  */
	tee3::String codec_name;		/**< 媒体编码名称  */
	int audio_input_level;			/**< 音频输入音量  */
	int audio_output_level;			/**< 音频输出音量  */
	int frame_width;				/**< 视频宽度  */
	int frame_height;				/**< 视频高度  */
	int frame_rate;					/**< 视频帧率  */

	MediaStats() :audio_input_level(0), audio_output_level(0)
		, frame_width(0), frame_height(0), frame_rate(0) {}
	void Clear() {
		NetworkStats::Clear();
		audio_input_level = 0;
		audio_output_level = 0;
		frame_width = 0;
		frame_height = 0;
		frame_rate = 0;
		media_type.clear();
		codec_name.clear();
	}
};
extern RT_API_EXPORT const RoomStats g_emptyRoomStats;
extern RT_API_EXPORT const MediaStats g_emptyMediaStats;
RT_API_EXPORT const String toString(const RoomStats& obj);
RT_API_EXPORT const String toString(const MediaStats& obj);

//#if defined(RT_DESKTOP)
class WindowId {
public:
	// Define WindowT for each platform.
#if defined(RT_LINUX) && !defined(RT_ANDROID)
	typedef Window WindowT;
#elif defined(RT_WIN32)
	typedef HWND WindowT;
#elif defined(RT_MAC) && !defined(RT_IOS)
	typedef CGWindowID WindowT;
#else
	typedef unsigned int WindowT;
#endif

	static WindowId Cast(uint64 id) {
#if defined(RT_WIN32)
		return WindowId(reinterpret_cast<WindowId::WindowT>(id));
#else
		return WindowId(static_cast<WindowId::WindowT>(id));
#endif
	}

	static uint64 Format(const WindowT& id) {
#if defined(RT_WIN32)
		return static_cast<uint64>(reinterpret_cast<uintptr_t>(id));
#else
		return static_cast<uint64>(id);
#endif
	}

	WindowId() : id_(0) {}
	WindowId(const WindowT& id) : id_(id) {}  // NOLINT
	const WindowT& id() const { return id_; }
	bool IsValid() const { return id_ != 0; }
	bool Equals(const WindowId& other) const {
		return id_ == other.id();
	}

private:
	WindowT id_;
};
typedef std::vector<WindowId> WindowIds;
class DesktopId {
public:
	// Define DesktopT for each platform.
#if defined(RT_LINUX) && !defined(RT_ANDROID)
	typedef Window DesktopT;
#elif defined(RT_WIN32)
	typedef HMONITOR DesktopT;
#elif defined(RT_MAC) && !defined(RT_IOS)
	typedef CGDirectDisplayID DesktopT;
#else
	typedef unsigned int DesktopT;
#endif

	static DesktopId Cast(int id, int index) {
#if defined(RT_WIN32)
		return DesktopId(reinterpret_cast<DesktopId::DesktopT>(id), index);
#else
		return DesktopId(static_cast<DesktopId::DesktopT>(id), index);
#endif
	}

	DesktopId() : id_(0), index_(-1) {}
	DesktopId(const DesktopT& id, int index)  // NOLINT
		: id_(id), index_(index) {}
	const DesktopT& id() const { return id_; }
	int index() const { return index_; }
	bool IsValid() const { return index_ != -1; }
	bool Equals(const DesktopId& other) const {
		return id_ == other.id() && index_ == other.index();
	}

private:
	// Id is the platform specific desktop identifier.
	DesktopT id_;
	// Index is the desktop index as enumerated by each platform.
	// Desktop capturer typically takes the index instead of id.
	int index_;
};

//#endif

//#if defined(RT_ANNOTATION)
enum AnnotationToolType {
	annotation_tool_mouse = 1 << 0,
	annotation_tool_line = 1 << 1,
	annotation_tool_rectangle = 1 << 2,
	annotation_tool_ellipse = 1 << 3,
	annotation_tool_polyline = 1 << 4,
	annotation_tool_polygon = 1 << 5,
	annotation_tool_hlight_point = 1 << 6,
	annotation_tool_hlight_texttag = 1 << 7,
	annotation_tool_eraser_line = 1 << 8,
	annotation_tool_eraser_rectangle = 1 << 9,
	annotation_tool_rhomb = 1 << 10,
	annotation_tool_arrow = 1 << 11,
	annotation_tool_success = 1 << 12,
	annotation_tool_failure = 1 << 13,
};
extern RT_API_EXPORT const uint32 AVD_supportedAnnotationTools;
enum LineArrowType {
	line_arrow_none = 1,
	line_arrow_begin = 1 << 1,
	line_arrow_end = 1 << 2,
	line_arrow_2 = line_arrow_begin | line_arrow_end,
};

enum ClearType {
	annotation_clear_all = 1 << 8,
	annotation_clear_myall = 1 << 9,
	annotation_clear_others = 1 << 10,
};
//#endif


/**
* 视频参数结构.
*
* 视频参数结构定义了视频采集、传输等相关参数信息。
*/
typedef CameraCapability VideoParams;

/**
* 音频参数结构.
*
* 音频参数结构定义了音频采集、传输等相关参数信息。
*/
struct AudioParams {
	uint32 channel;							/**< 通道数，默认1  */
	uint32 sampleRate;						/**< 采样率，默认44100  */
	uint32 bitrate;							/**< 编码比特率，默认96kbps  */
	AudioParams() :channel(1), sampleRate(44100), bitrate(96000) {}
	AudioParams(uint32 c, uint32 s, uint32 b) :channel(c), sampleRate(s), bitrate(b) {}

	friend std::ostream& operator <<(std::ostream& ost, const AudioParams& o) {
		ost << "audioParams:[" << "channel=" << o.channel << ",sampleRate=" << o.sampleRate << ",bitrate=" << o.bitrate << "],";
		return ost;
	}
};

NAMESPACE_TEE3_AVD_END

#endif//RT_COMMON_H
