#ifndef RT_VideoMixer_H_
#define RT_VideoMixer_H_

#include "common.h"
#include "videorender.h"

namespace tee3 {
	namespace avd {
		class ILivecast;
		class IVideoRender;

		/**
		* 视频混屏模块接口.
		*
		* 视频混屏模块定义房间中视频混屏、布局、订阅功能接口。
		*/
		class RT_API_EXPORT IMVideoMixer {
		public:
			/** 设置直播视频参数
			*
			* @param[in] layout 各子视频在直播视频中的布局类型
			* @param[in] width  直播视频图像宽
			* @param[in] height 直播视频图像高
			* @return 返回错误代码。
			*/
			virtual Result setMixerTypeaSize(MixerVideoLayoutType layout, int width, int height) = 0;
			/** 设置直播视频合屏时采用的拉伸方式
			* @param[in] scale	各子视频合屏时采用的拉伸方式，参考ScaleType
			* @return 返回错误代码。
			*/
			virtual Result setScaleType(IVideoRenderView::ScalingType scale) = 0;
			/** 设置直播视频背景图片
			* @param[in] background 直播背景图
			* @return 返回错误代码。
			*/
			virtual Result setBackgroud(const String& background) = 0;
			/** 清理掉所有的混屏视频，包括主视频；如果需要再次开启合屏，需要重新设置直播视频和Overlay视频。
			*
			* @return 返回错误代码。
			*/
			virtual void   clearSubVideos() = 0;

			//自动布局(layout auto)：设置自动布局参数和主视频后，sdk自动来布局子视频组合成直播视频
			/** 切换直播视频的主视频（若主视频掉线，本地主播视频切为主视频）
			*
			* @param[in] userId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @return 返回错误代码。
			* @note 直播视频的主视频在vl_auto_overlay模式下可以做切换，缺省为主播视频
			*/
			virtual Result setMainVideo(const UserId& userId) = 0;
			/** 直播视频自动布局的布局参数设置
			*
			* @param[in] isTopdown  各子视频在直播视频上的排列顺序，true: 从上往下（左到右）排列； false: 从下往上（左到右）排列
			* @param[in] w			各子视频相对直播视频的宽度比例，取值0.0~1.0，内部取值为：width (直播视频）x W
			* @param[in] h			各子视频相对直播视频的高度比例，取值0.0~1.0，内部取值为：height(直播视频）x H
			* @param[in] x_begin    初始子视频在直播视频的左上角左边位置，即x坐标，取值0.0~1.0，内部取值为：width (直播视频）x x_begin
			* @param[in] y_begin    初始子视频在直播视频的左上角上边位置，即y坐标，取值0.0~1.0，内部取值为：height(直播视频）x y_begin
			* @return 返回错误代码。
			* @note 缺省设置为：vl_auto_overlay布局类型，从下往上排列; w=h=0.3f; x_begin=0.7f; y_begin=0.7f;
			*/
			virtual Result autoLayoutParams(bool isTopdown, float w, float h, float x_begin, float y_begin) = 0;

			//手动布局(layout manual)：完全手动布局整个直播视频，可以将一个个子视频排布在直播视频的任意位置
			/** 添加视频到直播视频特定位置
			*
			* @param[in] userId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @param[in] zorder 多个视频覆盖时，前后位置，zorder越小，视频越在前面，后面会被覆盖；取值0-n
			* @param[in] x 此视频在直播视频的左上角左边位置，即x坐标，取值0.0~1.0，内部取值为：width (直播视频）x X
			* @param[in] y 此视频在直播视频的左上角上边位置，即y坐标，取值0.0~1.0，内部取值为：height(直播视频）x Y
			* @param[in] w 此视频在直播视频的宽度比例，取值0.0~1.0，内部取值为：width (直播视频）x W
			* @param[in] h 此视频在直播视频的高度比例，取值0.0~1.0，内部取值为：height(直播视频）x H
			* @return 返回错误代码。
			* @sa addMixerOverlayVideo
			*/
			virtual Result addSubVideo(const UserId& userId) = 0;
			virtual Result addSubVideo(const UserId& userId, int zorder, float x, float y, float w, float h) = 0;
			/** 更新附加视频在直播视频上显示的位置
			*
			* @param[in] userId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @param[in] zorder 多个视频覆盖时，前后位置，zorder越小，视频越在前面，后面会被覆盖；取值0-n
			* @param[in] x 此视频在直播视频的左上角左边位置，即x坐标，取值0.0~1.0，内部取值为：width (直播视频）x X
			* @param[in] y 此视频在直播视频的左上角上边位置，即y坐标，取值0.0~1.0，内部取值为：height(直播视频）x Y
			* @param[in] w 此视频在直播视频的宽度比例，取值0.0~1.0，内部取值为：width (直播视频）x W
			* @param[in] h 此视频在直播视频的高度比例，取值0.0~1.0，内部取值为：height(直播视频）x H
			* @return 返回错误代码。
			*/
			virtual Result updateSubVideo(const UserId& userId, int zorder, float x, float y, float w, float h) = 0;
			/** 移除直播视频上的某一视频
			*
			* @param[in] userId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @return 返回错误代码。
			*/
			virtual void   removeSubVideo(const UserId& userId) = 0;

		protected:
			virtual ~IMVideoMixer() {}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_VideoMixer_H_
