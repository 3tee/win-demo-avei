#ifndef RTC_FakeVideoCapturer_H_
#define RTC_FakeVideoCapturer_H_

#include "common.h"
#include "videorender.h"

namespace tee3 {
	namespace avd {

	/**
	* 视频流导入接口.
	*
	* 视频导入相关功能接口。
	*/
	class RT_API_EXPORT FakeVideoCapturer {
	public:
		/**
		* 视频流导入回调接口.
		*
		* 视频导入回调相关功能接口。
		*/
		class Listener {
		public:
			/** 视频流采集开始回调接口
			*
			* @return 开始成功或者失败，预留。
			* @note SDK开始视频采集触发此接口，返回值目前不关心，为以后预留。
			*/
			virtual bool OnStart() = 0;
			/** 视频流采集结束回调接口
			*
			* @note SDK停止视频采集触发此接口。
			*/
			virtual void OnStop() = 0;
		};

	public:
		/** 创建一个FOURCC_I420视频采集器对象
		*
		* @param[in] *listener 回调指针。
		* @param[in] isScreen 是否是桌面共享。
		* @return 视频采集器对象指针。
		* @note 接口默认创建的是FOURCC_I420 类型的视频采集器。
		*/
		static	FakeVideoCapturer* Create(FakeVideoCapturer::Listener* listener, bool isScreen = false);

		/** 创建一个视频采集器对象
		*
		* @param[in] *listener 回调指针。
		* @param[in] fourFormat 将要传入的视频数据类型。
		* @param[in] isScreen 是否是桌面共享。
		* @return 视频采集器对象指针。
		* @note 除了FOURCC_H264和其他FourCC不能混淆；其他的FourCC可以混淆导入。
		*/
		static	FakeVideoCapturer* Create(FakeVideoCapturer::Listener* listener, FourCC fourFormat, bool isScreen = false);

		/** 销毁一个视频采集器对象
		*
		* @param[in] *capturer 视频采集器对象指针。
		*/
		static	void Destroy(FakeVideoCapturer* capturer);

		/** 判断Capturer是否正在运行
		* @return 是否在运行。
		*/
		virtual bool isRunning() = 0;
		/** 给Capturer输入原始图像
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
		virtual Result inputCapturedFrame(
			uint64 timestamp_ns, uint32 format, int w, int h, const uint8* data, size_t len, int rotation, bool mirror) = 0;
		/** 给Capturer输入编码后的数据
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
		virtual Result inputEncodedFrame(
			uint64 timestamp_ns, int w, int h, const uint8* sample, size_t sample_size) = 0;

		virtual FourCC format() = 0;
	protected:
		virtual ~FakeVideoCapturer() {};
	};

	} // namespace avd
} // namespace tee3

#endif //RTC_FakeVideoCapturer_H_
