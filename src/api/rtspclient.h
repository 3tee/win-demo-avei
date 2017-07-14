#ifndef RTC_RtspClient_H_
#define RTC_RtspClient_H_

#include "common.h"

namespace tee3 {
	namespace avd {
		/**
		* Rtsp客户端.
		*
		*/
		class FakeAudioCapturer;
		class FakeVideoCapturer;
		class IVideoRender;
		class RT_API_EXPORT RtspClient {
		public:
			/**
			* 回调接口.
			*
			*/
			class IVideoListener {
			public:
				/// 异步返回
				/** 视频编码回调接口
				*
				* @param[in] type 编码类型。
				* @param[in] timestamp_ns 时间戳。
				* @param[in] width	宽度。
				* @param[in] height	高度。
				* @param[in] isKeyFrame  是否是关键帧。
				* @param[in] data 视频数据地址。
				* @param[in] len 视频数据字节长度。
				*
				* @sa
				*/
				virtual void videoEncodedOut(VideoEncodedType type, uint64 timestamp_ns, unsigned int width, unsigned int height, bool isKeyFrame, const uint8 *data, unsigned int len) = 0;

				/** 视频YUV数据回调接口
				*
				* @param[in] timestamp_ns 时间戳。
				* @param[in] dataY	Y数据地址。
				* @param[in] dataU	U数据地址。
				* @param[in] dataV	V数据地址。
				* @param[in] width	宽度。
				* @param[in] height	高度。
				* @param[in] strideY	Y数据单行像素长度。
				* @param[in] strideU	U数据单行像素长度。
				* @param[in] strideV	V数据单行像素长度。
				* @param[in] rotation	图像角度。
				*
				* @sa
				*/
				virtual void videoYUVOut(uint64 timestamp_ns,
					const uint8_t* dataY,
					const uint8_t* dataU,
					const uint8_t* dataV,
					int width,
					int height,
					int strideY,
					int strideU,
					int strideV,
					int rotation) = 0;


			};
			class IAudioListener {
			public:
				/// 异步返回
				/** 音频编码回调接口
				*
				* @param[in] type 编码类型。
				* @param[in] timestamp_ns 时间戳。
				* @param[in] sampleRate	采样率。
				* @param[in] channels	通道数。
				* @param[in] data  音频数据地址。
				* @param[in] len 音频数据字节长度。
				*
				* @sa
				*/
				virtual void audioEncodedOut(AudioEncodedType type, uint64 timestamp_ns, int sampleRate, int channels, const uint8 *data, unsigned int len) = 0;
				/** 音频PCM原始数据回调接口
				*
				* @param[in] timestamp_ns 时间戳。
				* @param[in] sampleRate	采样率。
				* @param[in] channels	通道数。
				* @param[in] data  音频数据地址。
				* @param[in] len 音频数据字节长度。
				*
				* @sa
				*/
				virtual void audioPCMOut(uint64 timestamp_ns, int sampleRate, int channels, const uint8 *data, unsigned int len) = 0;
			};

		public:
			/** 创建一个Rtsp客户端
			*
			* @sa
			*/
			static	RtspClient* create();

			/** 销毁一个Rtsp客户端
			*
			* @sa
			*/
			static	void destroy(RtspClient*);

			/** 开始接收流
			*
			* @param[in] uri rtsp流地址。
			* @param[in] username	认证用户名。
			* @param[in] password	认证密码。
			* @sa
			*/
			virtual Result start(const String& uri, String username = "", String password = "") = 0;
			/** 停止接收流
			*
			* @sa
			*/
			virtual Result stop() = 0;

			/** 设置视频流回调接口
			*
			* @param[in] listener 视频流回调接口指针。
			* @note 默认回调Encoded数据，如果需要YUV数据需要调用enableVideoCallbackYUV。
			* @sa enableVideoCallbackEncoded
			* @sa enableVideoCallbackYUV
			* @return 错误码。
			*/
			virtual Result setVideoListener(IVideoListener* listener) = 0;

			/** 设置音频流回调接口
			*
			* @param[in] listener 音频流回调接口指针。
			* @note 默认回调Encoded数据，如果需要PCM数据需要调用enableAudioCallbackPCM。
			* @sa enableAudioCallbackEncoded
			* @sa enableAudioCallbackPCM
			* @return 错误码。
			*/
			virtual Result setAudioListener(IAudioListener* listener) = 0;


			/** 设置渲染接口指针
			*
			* @param[in] render 渲染接口指针。
			* @sa
			*/
			virtual Result setRender(IVideoRender* render) = 0;

			/** 设置模拟视频采集接口指针
			*
			* @param[in] capture 模拟视频采集接口指针。
			* @note 由于默认回调Encoded数据，只能使用导入编码数据的VideoCapture，如果需要传入的是导入YUV数据的FakeVideoCapture需要调用enableVideoCallbackYUV，并且调用enableVideoCallbackEncoded(false)。
			* @sa enableVideoCallbackEncoded
			* @sa enableVideoCallbackYUV
			* @return 错误码。
			*/
			virtual Result setVideoCapture(tee3::avd::FakeVideoCapturer* capture) = 0;
			/** 设置模拟音频采集接口指针
			*
			* @param[in] capture 模拟音频采集接口指针。
			* @note 由于音频导入只能导入一种数据，PCM或者编码后的数据，所以enableAudioCallbackEncoded enableAudioCallbackPCM只能有一种生效，或者将默认导入encoded数据。
			* @note PCM：enableAudioCallbackEncoded(false)  enableAudioCallbackPCM(true); 导入encoded数据：enableAudioCallbackEncoded(true)  enableAudioCallbackPCM(false);
			* @sa enableVideoCallbackEncoded
			* @sa enableVideoCallbackYUV
			* @return 错误码。
			*/
			virtual Result setAudioCapture(tee3::avd::FakeAudioCapturer* capture) = 0;

			/** 控制是否回调编码过的视频数据
			*
			* @param[in] callbackEncodedData 功能开启或关闭。
			* @note 默认为true，enableVideoCallbackYUV和enableVideoCallbackEncoded功能可同时开启。
			* @sa videoEncodedOut
			* @sa enableVideoCallbackYUV
			* @return 错误码。
			*/
			virtual Result enableVideoCallbackEncoded(bool callbackEncodedData = true) = 0;
			/** 控制是否回调解码后的视频数据
			*
			* @param[in] callbackYUVData 功能开启或关闭。
			* @note 默认为false, enableVideoCallbackYUV(true)调用后videoYUVOut接口才会有数据回调。
			* @sa videoYUVOut
			* @sa enableVideoCallbackYUV
			* @return 错误码。
			*/
			virtual Result enableVideoCallbackYUV(bool callbackYUVData = true) = 0;
			/** 控制是否回调编码过的音频数据
			*
			* @param[in] callbackEncodedData 功能开启或关闭。
			* @note 默认为true，enableAudioCallbackEncoded和enableAudioCallbackPCM功能可同时开启。
			* @sa audioEncodedOut
			* @sa enableAudioCallbackPCM
			* @return 错误码。
			*/
			virtual Result enableAudioCallbackEncoded(bool callbackEncodedData = true) = 0;
			/** 控制是否回调解码后的音频数据
			*
			* @param[in] callbackPcmData 功能开启或关闭。
			* @note 默认为false, enableAudioCallbackPCM(true)调用后audioPCMOut接口才会有数据回调。
			* @sa audioPCMOut
			* @sa enableAudioCallbackEncoded
			* @return 错误码。
			*/
			virtual Result enableAudioCallbackPCM(bool callbackPcmData = true) = 0;
		protected:
			virtual ~RtspClient() {};
		};

	} // namespace avd
} // namespace tee3

#endif //RTC_RtspClient_H_
