#ifndef RT_AVImporter_H
#define RT_AVImporter_H

#include "common.h"
#include "errorcode.h"

namespace tee3 {
	namespace avd {
	class User;

	/**
	* 音视频流导入接口.
	*
	* 完成独立的音视频导入功能接口。
	*/
	class RT_API_EXPORT IAVImporter {
	public:
		/**
		* 引擎操作回调接口.
		*/
		class EngineListener {
		public:
			/** 初始化引擎操作异步返回
			*
			* @param[in] result 初始化错误代码。
			* @sa init
			*/
			virtual void onInitResult(Result result) = 0;
		protected:
			virtual ~EngineListener() {}
		};

		/**
		* 音视频流导入回调接口.
		*/
		class IListener {
		public:
			/** 导入状态通知
			*
			* @param[in] result 当前网络状态。
			*/
			virtual void onStatus(Result result) = 0;

			/** 错误通知
			*
			* @param[in] reason 错误状态。
			* @note 
			*		Err_Importer_Video_Failure
			*		Err_Importer_Audio_Failure
			*		Err_User_DuplicateJoin
			*		Err_Room_OutofVideo
			*		Err_Room_OutofAudio
			*		Err_Room_Close
			*		Err_Network_Failure
			*		Err_Importer_BeKicked
			*/
			virtual void onError (Result reason) = 0;

			/// 异步返回
			/** 加入房间操作异步返回
			*
			* @param[in] result 加入错误代码。
			* @return  0 成功
			*		Err_Stream_Connection_TimeOut		房间加会超时：可能原因有服务器没返回信令握手等
			*		Err_Rest_GetMcu_Failure				集群中获取可用mcu失败；
			*		Err_Net_ChannelSignal_Failure		房间信令连接失败，检查服务器地址和端口，及本地网络；
			*		Err_Net_ChannelData_Failure			房间数据连接失败（未实现）
			* @sa join
			*/
			virtual void onJoinResult(Result result) = 0;
		protected:
			virtual ~IListener() {}
		};

	public:
		/** 初始化引擎
		*
		* @param[in] severuri 服务器uri。
		* @param[in] appkey 叁体公司分发给应用公司的应用秘钥。
		* @param[in] secretkey 叁体公司分发给应用公司的安全秘钥。
		* @param[in] cb 引擎初始化回调函数指针。
		* @note AVD引擎在做任何功能操作前，必须先要初始化引擎成功。引擎初始化是一个异步操作，调用此接口后等待回调中函数initresult回调，判断是否初始化成功。
		* @return 返回错误代码。
		*/
		static Result initEngine(EngineListener* cb, const String& severuri, const String& appkey, const String& secretkey);

		/** 反初始化引擎
		*
		* @note AVD反初始化后，引擎中涉及的所有房间对象都无效，引擎上的功能函数都无法正常使用。
		* @return 返回错误代码。
		*/
		static Result uninitEngine();

		static bool   isEngineWorking();

	public:
		/** 创建音视频流导入对象
		*
		* @param[in] roomId 房间Id。
		* @param[in] *listener 导流回调接口。
		* @return 返回视频流导入对象。
		*/
		static IAVImporter* obtain(const RoomId& roomId);
		/** 释放音视频流导入对象
		*
		*@note 释放音视频流导入对象后，App层不能再使用当前音视频流导入对象指针，所有引用处均需设置为NULL。
		*/
		virtual void release() = 0;
	public:
		/** 设置音视频流导入回调接口
		*
		* @param[in] *listener 回调接口指针。
		*
		* @return 返回错误代码。
		* @see IListener
		*/
		virtual Result setListener(IListener* listener) = 0;
		/** 获取房间Id
		*
		* @return 返回房间Id。
		*/
		virtual const RoomId& getRoomId() const = 0;
		/** 加入房间
		*
		* @param[in,out] user 用户信息：用户Id(不能包含'{'、'}'字符)，用户名，用户数据等；当应用层不传入user.userId时，SDK将创建一个新的guid作为user.userId，同时返回给应用层。
		* @note 加入房间时，可以指定结果单独回调接口；如果指定，则在指定单独接口回调，不会在Listener的onJoinResult回调。
		* @return 返回错误代码。
		* @sa User
		*/
		virtual Result join(const User& user) = 0;

		/** 启用导入音频
		*
		* @param[in] enable 启用。
		* @return 返回错误代码。
		*/
		virtual Result enableAudio(bool enable) = 0;
		/** 启用导入视频
		*
		* @param[in] enable 启用。
		* @return 返回错误代码。
		*/
		virtual Result enableVideo(bool enable) = 0;

		/** 判断房间是否正在运行
		* @return 是否在运行。
		*/
		virtual bool isWorking() const = 0;

		/// audio
		/** 音频流导入PCM原始数据
		*
		* @param[in] timestamp_ns 时间戳, nanos second。
		* @param[in] sampleRate 音频采样率
		* @param[in] channels 音频采集通道数
		* @param[in] data 音频内容数据指针
		* @param[in] len  音频内容大小
		* @return 返回错误代码：
		* 	AVD_OK						成功
		* @note sampleRate和channels可以变化(但是不能频繁变化，如每次都不一样，内部每次变化的时候需要重新处理缓冲区)，每次输入的音频采样数据不能太多，最大不超过4K。
		*/
		virtual Result audio_inputPCMFrame(
			uint64 timestamp_ns, int sampleRate, int channels, const uint8* data, size_t len) = 0;

		/** 音频流导入AAC编码后的数据
		*
		* @param[in] timestamp_ns 时间戳, nanos second。
		* @param[in] sampleRate 音频采样率
		* @param[in] channels 音频采集通道数
		* @param[in] data 音频内容数据指针
		* @param[in] len  音频内容大小
		* @param[in] samplesOfEncodedData  音频编码数据包含的采样数（一般情况AAC每次编码都是1024 * channels个采样）
		* @return 返回错误代码：
		* 	AVD_OK						成功
		*/
		virtual Result audio_inputAACFrame(
			uint64 timestamp_ns, int sampleRate, int channels, const uint8* data, size_t len, int samplesOfEncodedData) = 0;

		/// video
		/** 视频流导入原始图像
		*
		* @param[in] timestamp_ns 时间戳, nanos second
		* @param[in] format 原始图像格式，见FourCC枚举
		* @param[in] w 图像宽度
		* @param[in] h 图像高度
		* @param[in] data 图像内容数据指针
		* @param[in] len  图像内容大小
		* @param[in] rotation 图像需要旋转的角度，顺时针方向，取值：0,90,180,270
		* @param[in] mirror 镜面图像，左右翻转
		* @return 返回错误代码：
		* 	AVD_OK						成功
		* 	RTC_ERR_VIDEO_CONVERT_FAILED	转换图像格式失败，有可能是内存分配失败，可考虑重启程序。
		*	Err_Invalid_Arg				参数不合法，可忽略
		*	Err_Wrong_Status			状态不对，可忽略
		*	
		*/
		virtual Result video_inputRAWFrame(
			uint64 timestamp_ns, uint32 format, int w, int h, const uint8* data, size_t len, int rotation, bool mirror) = 0;
		/** 视频流导入h264编码后的数据
		*
		* @param[in] timestamp_ns 时间戳, nanos second
		* @param[in] w 图像宽度
		* @param[in] h 图像高度
		* @param[in] sample  图像内容数据指针
		* @param[in] sample_size 图像内容大小
		* @return 返回错误代码：
		* 	AVD_OK						成功
		*	Err_Invalid_Arg				参数不合法，可忽略
		*	Err_Wrong_Status			状态不对，可忽略
		*
		*/
		virtual Result video_input264Frame(
			uint64 timestamp_ns, int w, int h, const uint8* sample, size_t sample_size) = 0;

	protected:
		virtual ~IAVImporter() {};
	};

	} // namespace avd
} // namespace tee3

#endif //RT_AVImporter_H
