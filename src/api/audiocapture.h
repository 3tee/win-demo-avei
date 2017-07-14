#ifndef RTC_FakeAudioCapturer_H_
#define RTC_FakeAudioCapturer_H_

#include "common.h"
namespace tee3 {
	namespace avd {
	/**
	* 音频流导入接口.
	*
	* 音频导入相关功能接口。
	*/
	class RT_API_EXPORT FakeAudioCapturer {
	public:
		/** 获取音频采集器单例对象
		*
		* @return 音频采集器对象指针。
		*/
		static	FakeAudioCapturer* Instance();

		/** 销毁一个音频采集器对象
		*
		* @param[in] *capturer 音频采集器对象指针。
		*/
		static	void Destroy(FakeAudioCapturer* capturer);
		static  bool hasUsedCapturer();

		/** 判断Capturer是否正在运行
		* @return 是否在运行。
		*/
		virtual bool isRunning() const = 0;

		/** 给Capturer输入PCM原始数据
		*
		* @param[in] timestamp_ns 时间戳, nanos second。
		* @param[in] sampleRate 音频采样率
		* @param[in] channels 音频采集通道数
		* @param[in] data 音频内容数据指针
		* @param[in] len  音频内容大小
		* @return 返回错误代码：
		* 	AVD_OK						成功
		* @note sampleRate和channels可以变化(但是不能频繁变化，如每次都不一样，内部每次变化的时候需要重新处理缓冲区)，每次输入的音频采样数据不能太多，最大不超过4K。
		*	
		*/
		virtual Result inputCapturedFrame(
			uint64 timestamp_ns, int sampleRate, int channels, const uint8* data, size_t len) = 0;

		/** 给Capturer输入AAC编码后的数据
		*
		* @param[in] timestamp_ns 时间戳, nanos second。
		* @param[in] sampleRate 音频采样率
		* @param[in] channels 音频采集通道数
		* @param[in] data 音频内容数据指针
		* @param[in] len  音频内容大小
		* @param[in] samplesOfEncodedData  音频编码数据包含的采样数（一般情况AAC每次编码都是1024 * channels个采样）
		* @return 返回错误代码：
		* 	AVD_OK						成功
		*
		*/
		virtual Result inputAACFrame(
			uint64 timestamp_ns, int sampleRate, int channels, const uint8* data, size_t len, int samplesOfEncodedData) = 0;

		/** 当前AVD引擎使用音频导入作为缺省音频源
		* @return 返回错误代码：
		* 	AVD_OK						成功
		*/
		virtual Result enable(bool isEnable) = 0;

	protected:
		virtual ~FakeAudioCapturer() {};
	};

	} // namespace avd
} // namespace tee3

#endif //RTC_FakeAudioCapturer_H_
