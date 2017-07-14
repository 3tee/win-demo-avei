#ifndef RT_AVDScreenRecorder_H_
#define RT_AVDScreenRecorder_H_

#include "api/defines.h"
#include "api/common.h"
namespace tee3 {
	namespace avd {

		/**
		* 屏幕录制接口.
		*
		* 屏幕录制接口提供录制屏幕，录制应用所在区域，直接录制应用，录制系统播放声音（不能区分应用）等功能。
		* 操作接口包括：获取可录制对象信息，开始录制，暂停录制，停止录制，设置录制帧率，设置视频编码格式，设置音频编码格式，设置音频采集参数（采样率，通道数，每帧所占用比特位）
		*/
		class RT_API_EXPORT IAVDScreenRecorder {
		public:

			struct IMG {

			};
			struct ScreenImgType {
				ScreensType screen;
				IMG img;
			};
			typedef std::vector<ScreenImgType> ScreensImgType;

			static IAVDScreenRecorder* create();
			static void destroy(IAVDScreenRecorder* recorder);
			//Listener 
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
				virtual void videoEncodedOut(VideoEncodedType format, uint64 timestamp_ns, unsigned int width, unsigned int height, bool isKeyFrame, const uint8 *data, unsigned int len) = 0;

				/** 视频采集YUV数据回调接口
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
				/** 音频采集回调接口
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

			virtual Result setVideoListener(IVideoListener* listener) = 0;
			virtual Result setAudioListener(IAudioListener* listener) = 0;

			//Setting interface
			virtual Result setRecorderFile(std::string filePath) = 0;
			virtual Result setVideoFps(uint8 fps) = 0;
			virtual Result setVideoBitRate(uint32 bitRate) = 0;
			//virtual Result setVideoFrameRate(uint8 frameRate) = 0;
			virtual Result setAudioInfo(uint32 frameRate, uint8 channel, uint8 bitPerSample) = 0;
			virtual Result setVideoEncodedType(VideoEncodedType format) = 0;
			virtual Result setAudioEncodedType(AudioEncodedType format) = 0;
			virtual Result setRecordedAudio(AudioRecordedType recordedAudio) = 0;
			virtual Result setRecordedScreen(const ScreenWindow& screen) = 0;
			//Control interface
			virtual Result getScreenWindows(ScreensType& wnds) = 0;
			virtual Result getScreenWindows(ScreensImgType& wnds) = 0;
			virtual Result startScreenRecord() = 0;
			virtual Result stopScreenRecord() = 0;
			virtual Result pauseScreenRecord() = 0;
			virtual Result restoreScreenRecord() = 0;
		protected:
			virtual ~IAVDScreenRecorder() {}
		};
	}
}

#endif//RT_AVDScreenRecorder_H_
