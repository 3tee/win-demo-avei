#ifndef RT_ILivecast_H_
#define RT_ILivecast_H_

#include "common.h"
#include "easyroom.h"
#include "videorender.h"
#include "videocapture.h"
#include "audiocapture.h"

namespace tee3 {
	namespace avd {

		/**
		* 直播接口类
		*
		* 直播接口定义了直播对象创建和释放、直播功能操作。
		*/
		class RT_API_EXPORT ILivecast {
		public:
			/**
			* 直播回调接口类
			*
			* 直播回调接口定义了直播对象操作的异步返回，直播状态通知等。
			*/
			class IListener {
			public:
				/// 通知
				/** 直播流状态更改通知
				*
				* @param[in] status 当前直播状态。
				* @note 开始推流时，会每间隔 3s 调用该回调方法来反馈该 3s 内的流状态，包括视频帧率、音频帧率、音视频总码率
				*/
				virtual void onStatus(LivecastStatus status) = 0;
				/** 直播网络状态通知
				*
				* @param[in] status 当前网络状态。
				*/
				virtual void onConnectionStatus(ConnectionStatus status) = 0;
				/** 间隔 3s 调用该回调方法来反馈该 3s 内的流状态，包括视频帧率、音视频总码率
				*
				* @param[in] videofps 视频帧率。
				* @param[in] avbps	  音视频总码率: kbps。
				*/
				virtual void onStreamlast3Stats(int videofps, int avbps) = 0;
				/** 直播错误通知
				*
				* @param[in] error 错误码
				* @param[in] message 错误消息
				*/
				virtual void onError(Result error, const String& message) = 0;
			protected:
				virtual ~IListener() {}
			};

		public:
			/** 设置直播Id
			* @return 返回错误代码。
			* @note 直播Id唯一标识一个直播，设置后才能使用直播接口。
			*/
			static Result setLivecastId(const RoomId& castId);
			static const RoomId& getLivecastId();
			/** 获取直播接口
			* @return 返回直播接口指针。
			* @note 当前需要先设置直播Id，后才能使用下面所有接口。
			* @sa setLivecastId
			*/
			static ILivecast* Instance();
			/** 释放直播接口
			*/
			static void       release();

			/** 直播对象初始化
			*
			* @param[in] *listener 回调接口指针。
			* @return 返回错误代码。
			* @sa IListener
			*/
			virtual Result init(IListener* listener) = 0;
			/** 直播对象反初始化
			* @return 返回错误代码。
			*/
			virtual Result uninit() = 0;

		public:
			/** 获取直播状态
			* @return 返回当前状态。
			* @sa LivecastStatus
			*/
			virtual LivecastStatus getStatus() const = 0;

			/** 设置视频显示窗口
			* @param[in] *render 本地预览render接口。
			* @return 返回错误代码。
			* @sa IVideoRender
			*/
			virtual Result setDisplayView(IVideoRender* render) = 0;

			/** 设置视频摄像头采集、推流等参数
			* @param[in] param 视频参数。
			* @return 返回错误代码。
			*/
			virtual Result setVideoParams(const VideoParams& param) = 0;
			virtual VideoParams getVideoParams() const = 0;
			/** 设置音频麦克风采集、推流等参数
			* @param[in] param 音频参数。
			* @return 返回错误代码。
			*/
			virtual Result setAudioParams(const AudioParams& param) = 0;
			virtual AudioParams getAudioParams() const = 0;

			/** 设置视频外部导入数据源
			*
			* @param[in] *vcapturer 视频外部导入数据源。
			* @return 返回错误代码。
			* @sa FakeVideoCapturer
			*/
			virtual Result setVideoSource(FakeVideoCapturer* vcapturer) = 0;
			/** 设置音频外部导入数据源
			*
			* @param[in] *acapturer 音频外部导入数据源。
			* @return 返回错误代码。
			* @sa FakeAudioCapturer
			*/
			virtual Result setAudioSource(FakeAudioCapturer* acapturer) = 0;

			/** 预览本地缺省摄像头视频
			* @return 返回错误代码。
			*/
			virtual Result startPreview() = 0;
			/** 取消预览缺省摄像头视频
			* @return 返回错误代码。
			*/
			virtual Result stopPreview() = 0;
			/** 视频切换摄像头
			* @return 返回错误代码。
			*/
			virtual Result switchCamera() = 0;

			/** 判断本地麦克风是否静默
			* @return 返回静默状态。
			*/
			virtual bool isMicrophoneMute() const = 0;
			/** 本地麦克风静默
			* @return 返回错误代码。
			*/
			virtual Result muteMicrophone() = 0;
			/** 本地麦克风静默取消
			* @return 返回错误代码。
			*/
			virtual Result unmuteMicrophone() = 0;

			/** 抓取直播图像
			*
			* @param[in] fileName 图像保存地址，绝对地址。
			* @param[in] quality  图像质量：0-100, 0最差，100最好。
			* @return 返回错误代码。
			* @note 目前在直播状态下，播放端无法抓取视频（rtmp播放器播放的时候）
			*/
			virtual Result captureImage(const String& fileName, int quality) = 0;

			/** 屏幕共享且作为视频源
			*
			* @param[in] isOn 是否打开或关闭桌面共享。
			* @return 返回错误代码。
			*/
			virtual Result shareScreen(bool isOn) = 0;

			/** 开始直播
			*
			* @param[in] pushUrl 直播rtmp推流地址。
			* @return 返回错误代码。
			*/
			virtual Result publisRTMP(const String& pushUrl) = 0;
			/** 停止直播
			*
			* @return 返回错误代码。
			*/
			virtual Result unpublish() = 0;

		public:
			/** 设置直播选项
			*
			* @param[in] type 直播选项类型。
			* @param[in] value 选项内容，根据选项说明填入。
			* @return 返回错误代码。
			*/
			virtual Result setOption(RoomOption type, const String& value) = 0;
			/** 获取直播选项
			*
			* @param[in] type 直播选项类型。
			* @return 返回选项内容，根据选项说明解析。
			*/
			virtual String getOption(RoomOption type) = 0;

		public:
			/** 判断是否正在连麦中
			* @return 是否正在连麦。
			*/
			virtual bool isInRoom() const = 0;
			/** 直播开始连麦
			*
			* @param[in] room 互动房间接口。
			* @return 返回错误代码。
			* @note   互动房间通过创建IEasyRoom操作，需要先创建IEasyRoom接口，然后与直播接口进行挂接 
			* 直播互动挂接，系统内部会实现以下缺省操作：
			*				1. 自动挂接直播的采集音视频流到房间的采集音视频流
			*				2. 自动挂接房间的合屏视频流和混音音频流到直播推送音视频流
			* @sa IEasyRoom
			*/
			virtual Result attachRoom(IEasyRoom* room) = 0;
			/** 直播停止连麦
			* @return 返回错误代码。
			*/
			virtual Result detachRoom() = 0;

		protected:
			virtual ~ILivecast() {}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_ILivecast_H_
