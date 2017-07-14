#ifndef RT_Video_H_
#define RT_Video_H_

#include "common.h"
#include "room.h"
#include "videocapture.h"

namespace tee3 {
	namespace avd {
		class IRoom;
		class IVideoRender;

		/**
		* 视频设备管理接口.
		*
		* 视频设备管理接口定义房间中视频设备相关功能接口。
		*/
		class RT_API_EXPORT ICameraDevice {
		public:
			/**
			* 视频设备管理回调接口.
			*
			* 视频设备管理回调接口定义了视频设备相关通知。
			*/
			class IListener {
			public:
				/// 通知
				/** 摄像头插入通知
				*
				* @param[in] item 摄像头信息。
				* @note 当系统中有摄像头设备插入时，此回调触发。
				*/
				virtual void onCameraPlugin(const Camera& item) = 0;
				/** 摄像头拔出通知
				*
				* @param[in] item 摄像头信息。
				* @note 当系统中有摄像头设备拔出时，此回调触发。
				*/
				virtual void onCameraPlugout(const Camera& item) = 0;
			protected:
				virtual ~IListener() {}
			};
		public:
			/** 设置视频设备管理回调接口
			*
			* @param[in] *listener 视频设备管理回调接口指针。
			* @return 返回错误代码。
			* @sa IListener
			*/
			virtual Result setListener(IListener* listener) = 0;

			/** 获取指定设备Id的摄像头信息
			*
			* @param[in] deviceId 设备Id。
			* @return 返回摄像头信息。
			* @note 可以返回本地用户和远端用户的摄像头信息。
			*/
			virtual const Camera& getCamera(const DeviceId& deviceId) const = 0;
			/** 获取指定设备Id的摄像头类型
			*
			* @param[in] deviceId 设备Id。
			* @return 返回摄像头类型。
			* @note 可以返回本地用户和远端用户的摄像头信息。
			*/
			virtual CameraType getCameraType(const DeviceId& deviceId) const = 0;
			/** 获取指定设备Id的摄像头采集参数列表
			*
			* @param[in] deviceId 设备Id。
			* @return 返回采集参数列表。
			* @sa CapabilitiesType
			*/
			virtual CapabilitiesType getCameraCapability(const DeviceId& deviceId) const = 0;
			///local
			/** 获取本机缺省摄像头信息
			* @return 返回摄像头信息。
			*/
			virtual const Camera& getDefaultCamera() const = 0;
			/** 设置本机缺省摄像头信息
			*
			* @param[in] deviceId 设备Id。
			* @return 返回错误代码。
			*/
			virtual Result setDefaultCamera(const DeviceId& deviceId) = 0;
			/** 设置本机缺省摄像头信息
			*
			* @param[in] type 摄像头类型。
			* @return 返回错误代码。
			*/
			virtual Result setDefaultCamera(CameraType type) = 0;
			/** 获取机摄像头设备列表
			*
			* @param[in,out] items 摄像头设备列表。
			* @return 返回错误代码。
			*/
			virtual Result getLocalCameras(CamerasType& items) const = 0;
			/** 获取指定类型的本机摄像头信息
			*
			* @param[in] type 摄像头类型。
			* @return 返回摄像头设备信息。
			* @sa CameraType
			*/
			virtual const Camera& getLocalCamera(CameraType type) const = 0;
			
			///remote 
			/** 获取远端用户的摄像头设备列表
			*
			* @param[in] userId 用户Id。
			* @param[in,out] items 摄像头设备列表。
			* @return 返回错误代码。
			*/
			virtual Result getRemoteCameras(const UserId& userId, CamerasType& items) const = 0;

		protected:
			virtual ~ICameraDevice() {}
		};

		/**
		* 房间中视频模块接口.
		*
		* 房间视频模块定义房间中视频发布、订阅功能接口。
		*/
		class RT_API_EXPORT IMVideo : public IModule {
		public:
			/**
			* 房间中视频模块回调接口.
			*
			* 视频模块回调接口定义了房间中视频相关的通知和视频异步操作返回。
			*/
			class IListener {
			public:
				/// 通知
				/** 摄像头状态更改通知
				*
				* @param[in] status 摄像头状态。
				* @param[in] fromId 摄像头Id，唯一标示一个摄像头。
				* @note 当摄像头状态更改后，房间内所有用户接收到此通知, notify when publishLocalCamera,publishRemoteCamera。
				*/
				virtual void onCameraStatusNotify(CameraStatus status, const DeviceId& fromId) = 0;
				/** 摄像头数据更改通知
				*
				* @param[in] level 摄像头整形数据。
				* @param[in] description 摄像头字符串数据。
				* @param[in] fromId 摄像头Id，唯一标示一个摄像头。
				* @note 当摄像头数据更改后，房间内所有用户接收到此通知, notify when publishLocalCamera(Camera), updateCameraData。
				*/
				virtual void onCameraDataNotify(int level, const String& description, const DeviceId& fromId) = 0;
				/** 摄像头视频发布通知
				*
				* @param[in] camera 摄像头信息，摄像头信息中level和description为应用层数据，应用层可以使用这些字段保存次摄像头视频相关的应用逻辑数据，方便标示、订阅视频。
				*
				* @note 当摄像头视频发布时，房间内所有用户接收到此通知；因通知的重要程度，将从onCameraStatusNotify中分离出此状态。
				* @sa publishLocalCamera
				*/
				virtual void onPublishCameraNotify(const Camera& camera) = 0;
				/** 摄像头视频取消发布通知
				*
				* @param[in] camera 摄像头信息。
				*
				* @note 当摄像头视频取消发布时，房间内所有用户接收到此通知；因通知的重要程度，将从onCameraStatusNotify中分离出此状态。
				* @sa unpublishLocalCamera
				*/
				virtual void onUnpublishCameraNotify(const Camera& camera) = 0;

				/// 异步返回
				/** 本用户订阅视频异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] fromId 摄像头Id，唯一标示一路视频。
				*
				* @sa subscribe
				*/
				virtual void onSubscribeResult(Result result, const DeviceId& fromId) = 0;
				/** 本用户取消订阅视频异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] fromId 摄像头Id，唯一标示一路视频。
				*
				* @sa unsubscribe
				*/
				virtual void onUnsubscribeResult(Result result, const DeviceId& fromId) = 0;
				/** 本用户发布摄像头视频异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] fromId 摄像头Id，唯一标示一路视频。
				*
				* @sa publishLocalCamera
				*/
				virtual void onPublishLocalResult(Result result, const DeviceId& fromId) = 0;
				/** 本用户取消发布摄像头视频异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] fromId 摄像头Id，唯一标示一路视频。
				*
				* @sa unpublishLocalCamera
				*/
				virtual void onUnpublishLocalResult(Result result, const DeviceId& fromId) = 0;

			protected:
				virtual ~IListener() {}
			};
		public:
			/** 获取房间内视频模块接口
			*
			* @param[in] room 房间对象指针。
			*
			* @return 返回视频模块接口指针。
			*/
			static IMVideo* getVideo(IRoom* room);
			//
			/** 设置视频模块回调接口
			*
			* @param[in] *listener 视频模块回调接口指针。
			* @return 返回错误代码。
			* @sa IListener
			*/
			virtual Result setListener(IListener* listener) = 0;
			/** 设置视频动态码率调整的范围，最小和最大比特率，
			*
			* @param[in]  deviceId 视频设备对应ID。
			* @param[in] minBitrateBps 最小比特率,单位bps
			* @param[in] maxBitrateBps 最大比特率,单位bps
			* @return 返回错误代码。
			* @note 例如30k/s = 30*8*1000 bps(bits/s)。当min和max相等时，码率固定，将不会动态调整。
			*		需要在视频publish成功后才能做设置。
			* 参考码流：
			*	1920x1080		[2000*1000, 4000*1000]
			*	1280x720		[1000*1000, 2000*1000]
			*	640x480			[500*1000,  1000*1000]
			*	320x240			[250*1000,	500*1000]
			*/
			virtual Result setVideoBitrate(const DeviceId& deviceId, int minBitrateBps, int maxBitrateBps) = 0;

			/// main(default) camera operate
			virtual bool   isLocalCameraInPreview(const DeviceId& fromId) = 0;
			/** 预览本地缺省摄像头视频
			*
			* @param[in] *render 显示对象接口指针
			* @note 本地视频在发布到会议前，可以调用此接口进行视频预览；视频发布到会议中后，需要使用attachRender接口来进行视频显示，此时此接口无效。
			* @return 返回错误代码。
			* @sa IVideoRender
			*/
			virtual Result previewLocalCamera(IVideoRender* render) = 0;
			/** 预览模拟摄像头视频
			*
			* @param[in,out] fakeDevice 模拟摄像头信息，接口会生成deviceId，生成规则："userId_"+（用户输入的deviceId), 此处deviceId不能包含'_'和'{'、'}'此三个字符。
			* @param[in] *capture 模拟摄像头视频输入接口。
			* @param[in] *render 显示对象接口指针
			* @return 返回错误代码。
			* 模拟摄像头用于在房间中导入特定已经存在的视频数据流，比如视频文件、录播系统视频流等。
			* @sa FakeVideoCapturer
			* @note render 允许被设置为NULL。
			* @note fakeDevice.id 会按照3tee设备id规则重新构造成。
			*/
			virtual Result previewLocalCamera(Camera& fakeDevice, FakeVideoCapturer* capture, IVideoRender* render) = 0;
			/** 取消预览缺省摄像头视频
			* @return 返回错误代码。
			*/
			virtual Result unpreviewLocalCamera() = 0;
			/** 更新正在使用的摄像头的视频质量（分辨率和帧率）
			* @param[in] deviceId 摄像头Id。
			* @param[in] quality 视频质量。
			* @return 返回错误代码。
			*/
			virtual Result updateCaptureCapability(const DeviceId& deviceId, CameraCapability& quality) = 0;

			/** 发布缺省摄像头视频
			*
			* @return 返回错误代码。
			* @note 发布操作为异步操作，操作结果在 onPublishLocalResult 中返回。
			* @sa onPublishLocalResult
			*/
			virtual Result publishLocalCamera() = 0;
			/** 取消发布缺省摄像头视频
			*
			* @return 返回错误代码。
			* @note 取消发布操作为异步操作，操作结果在 onUnpublishLocalResult 中返回。
			* @sa onUnpublishLocalResult
			*/
			virtual Result unpublishLocalCamera() = 0;

			/// 多摄像头操作模式：能同时打开多个摄像头，不区分前置与后置摄像头。						*************************
			/** 预览本地某一摄像头视频
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路摄像头视频。
			* @param[in] *render 显示对象接口指针
			* @return 返回错误代码。
			* @sa IVideoRender
			*/
			virtual Result previewLocalCamera(const DeviceId& deviceId, IVideoRender* render) = 0;
			virtual Result previewLocalCamera(const Camera& device, IVideoRender* render) = 0;
			/** 取消预览摄像头视频
			* @param[in] deviceId 摄像头Id。
			* @return 返回错误代码。
			*/
			virtual Result unpreviewLocalCamera(const DeviceId& deviceId) = 0;
			/** 发布摄像头视频
			*
			* @param[in] device 摄像头信息。
			* @return 返回错误代码。
			* @note 发布操作为异步操作，操作结果在 onPublishLocalResult 中返回。
			* @sa onPublishLocalResult
			*/
			virtual Result publishLocalCamera(const Camera& device) = 0;
			/** 发布模拟摄像头视频
			*
			* @param[in,out] fakeDevice 模拟摄像头信息，接口会生成deviceId，生成规则："userId_"+（用户输入的deviceId), 此处deviceId不能包含'_'和'{'、'}'此三个字符。
			* @param[in] *capture 模拟摄像头视频输入接口。
			* @return 返回错误代码。
			* @note 发布操作为异步操作，操作结果在 onPublishLocalResult 中返回。
			* 模拟摄像头用于在房间中导入特定已经存在的视频数据流，比如视频文件、录播系统视频流等。
			* @sa onPublishLocalResult
			* @sa FakeVideoCapturer
			*/
			virtual Result publishLocalCamera(Camera& fakeDevice, FakeVideoCapturer* capture) = 0;
			/** 取消发布摄像头视频
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路摄像头视频。
			* @return 返回错误代码。
			* @note 取消发布操作为异步操作，操作结果在 onUnpublishLocalResult 中返回。
			* @sa onUnpublishLocalResult
			*/
			virtual Result unpublishLocalCamera(const DeviceId& deviceId) = 0;
			/** 切换视频源
			*
			* @param[in] fromId 原摄像头Id。
			* @param[in] toId 切换后摄像头Id。
			* @return 返回错误代码。
			*/
			virtual Result switchLocalCamera(const DeviceId& fromId, const DeviceId& toId) = 0;

			/// 前后置摄像头操作模式：每一时刻只能打开前后或者后置摄像头中的一路，可以做前后切换。		*************************
			/** 预览本机前置或者后置摄像头视频
			*
			* @param[in] type 摄像头类型。
			* @param[in] *render 显示对象接口指针
			* @note 在前后置摄像头操作模式下有效；摄像头模式设置在AVDEngine的SetOption接口中实现；取消预览接口为unpreviewLocalCamera，无需参数。
			* @return 返回错误代码；若不支持此操作返回 Err_Not_Available。
			* @sa IVideoRender
			* @sa CameraType
			*/
			virtual Result previewLocalCamera(CameraType type, IVideoRender* render) = 0;
			/** 发布特定类型摄像头视频
			*
			* @param[in] type 摄像头类型。
			* @return 返回错误代码；若不支持此操作返回 Err_Not_Available。
			* @note 发布操作为异步操作，操作结果在 onPublishLocalResult 中返回。在前后置摄像头操作模式下有效；取消预览接口为unpublishLocalCamera，无需参数。
			* @sa onPublishLocalResult
			*/
			virtual Result publishLocalCamera(CameraType type) = 0;
			/** 切换特定类型摄像头视频源
			*
			* @param[in] toType 摄像头类型。
			* @note 在前后置摄像头操作模式下有效。
			* @return 返回错误代码；若不支持此操作返回 Err_Not_Available。
			*/
			virtual Result switchToLocalCamera(CameraType toType) = 0;
			/** 获取当前使用的摄像头类型
			*
			* @note 在前后置摄像头操作模式下有效。
			* @return 返回摄像头类型。
			*/
			virtual CameraType getCurrentCameraType() const = 0;
			/** 获取正在使用的摄像头的采集信息
			*
			* @param[in] deviceId 摄像头设备ID。
			* @return 返回采集参数。
			*/
			virtual CameraCapability getRealCaptureCapability(const DeviceId& deviceId) = 0;
			/** 摄像头数据更改
			*
			* @param[in] deviceId 摄像头Id，唯一标示一个摄像头。
			* @param[in] level 摄像头整形数据。
			* @param[in] description 摄像头字符串数据。
			* @sa onCameraDataNotify
			*/
			virtual Result updateCameraData(const DeviceId& deviceId, int level, const String& description) = 0;
			/** 已经published或preview的视频暂停：视频卡住，且也不发送到网络上
			*
			* @param[in] deviceId 摄像头设备ID。
			* @return 返回错误代码。
			*/
			virtual Result muteLocalCamera(const DeviceId& deviceId) = 0;
			/** 取消视频暂停
			*
			* @param[in] deviceId 摄像头设备ID。
			* @return 返回错误代码。
			* @note muteLocalCamera
			*/
			virtual Result unmuteLocalCamera(const DeviceId& deviceId) = 0;

			/// subscribe and publish list
			/** 订阅摄像头视频
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路摄像头视频。
			* @return 返回错误代码。
			* @note 订阅操作为异步操作，操作结果在 onSubscribeResult 中返回。
			* 订阅视频规则：
			*	有清晰度要求，选择此清晰度下视频；
			*	如无清晰度要求，优先选择硬件编码码流视频；
			*	无硬件编码码流，选择缺省设置清晰度；
			*	同一清晰度下有两路码流，按是否支持硬件编码来选择码流。
			* @sa onSubscribeResult
			*/
			virtual Result subscribe(const DeviceId& deviceId) = 0;
			/** 订阅摄像头视频指定视频质量视频
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路摄像头视频。
			* @param[in] qualityType 指定视频质量。
			* @return 返回错误代码。
			* @note 订阅说明如同名接口所描述。
			*/
			virtual Result subscribe(const DeviceId& deviceId, VideoQuality qualityType) = 0;
			/** 取消订阅摄像头视频
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路摄像头视频。
			* @return 返回错误代码。
			* @note 取消订阅操作为异步操作，操作结果在 onUnsubscribeResult 中返回。
			* @sa onUnsubscribeResult
			*/
			virtual Result unsubscribe(const DeviceId& deviceId) = 0;
			/** 获取房间中已发布视频摄像头列表
			*
			* @param[in,out] items 摄像头设备列表。
			* @return 返回错误代码。
			* @note 摄像头列表包含本地发布的摄像头和房间中其他用户的所有正在发布的摄像头，摄像头视频要在界面上显示，远端摄像头需要首先做subcribe；
			*		而本地摄像头显示时不需要subscribe，应用层开发时需要注意。
			*/
			virtual Result getPublishedCameras(CamerasType& items) const = 0;
			/** 判断摄像头是否已经发布到房间中
			* @param[in] fromId 摄像头Id，唯一标示一路摄像头视频。
			* @return 返回是否是已发布的摄像头。
			*/
			virtual bool isCameraPublished(const DeviceId& fromId) const = 0;
			/** 获取本用户订阅的摄像头视频列表
			*
			* @param[in,out] items 摄像头设备列表。
			* @return 返回错误代码。
			*/
			virtual Result getSubscribedCameras(CamerasType& items) const = 0;
			/** 判断摄像头是否已经被本用户订阅
			* @param[in] fromId 摄像头Id，唯一标示一路摄像头视频。
			* @return 返回是否是已订阅的摄像头。
			*/
			virtual bool isCameraSubscribed(const DeviceId& fromId) const = 0;
			/// remote camera control
			/** 命令远端用户发布某一摄像头视频
			* @param[in] device 远端摄像头信息。
			* @return 返回错误代码。
			*/
			virtual Result remotecmdPublishCamera(const Camera& device) = 0;
			/** 命令远端用户取消发布摄像头视频
			* @param[in] deviceId 摄像头Id。
			* @return 返回错误代码。
			*/
			virtual Result remotecmdUnpublishCamera(const DeviceId& deviceId) = 0;

			/// render video
			/** 关联视频数据和显示对象，在render中显示视频
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @param[in] *render 显示对象接口指针。
			*
			* @note 视频数据可以在多个显示对象上显示，而一个显示对象某一时刻只能显示一路视频。
			*		在本地视频发布到房间中，或者远端视频订阅成功后，调用此接口完成视频与显示对象的对接，从而在显示对象中显示视频。
			* @return 返回错误代码。
			* @sa IVideoRender
			*/
			virtual Result attachRender(const DeviceId& deviceId, IVideoRender* render) = 0;
			/** 解除摄像头视频数据的所有显示对象关联
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @return 返回错误代码。
			*/
			virtual Result detachRender(const DeviceId& deviceId) = 0;
			/** 解除某一显示对象与摄像头视频数据的关联
			*
			* @param[in] *render 显示对象接口指针。
			* @sa IVideoRender
			*/
			virtual Result detachRender(IVideoRender* render) = 0;

			/// device manager
			/** 获取摄像头管理接口
			*
			* @return 返回摄像头管理接口指针。
			* @sa ICameraDevice
			*/
			virtual ICameraDevice* getCameraDevice() = 0;

			///
			/** 设置获取视频数据回调接口,此数据仅回调一份数据：数据为主视频+各个Overlay的视频，视频大小为主视频大小；如使用ILivecast接口，不需要设置此接口。
			*
			* @return 返回错误代码。
			* @sa IVideoRender
			* @note 如果音视频同时混合出流，建议先设置视频，再设置音频的回调接口，有助于音视频时间戳对齐。
			*/
			virtual Result setMixerDataListener(IVideoRender* listener) = 0;
			/** 设置回调视频数据的视频主视频
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @return 返回错误代码。
			* @note 设置混屏的主视频等效于，setMixerVideoSize为主视频大小，同时addMixerOverlayVideo将主视频加入到混屏中，且充满全屏。
			*/
			virtual Result setMixerMainVideo(const DeviceId& deviceId) = 0;
			/** 设置混频后视频数据的尺寸
			*
			* @param[in] width 视频图像宽。
			* @param[in] height 视频图像高。
			* @param[in] background 背景图。
			* @return 返回错误代码。
			* @sa addMixerOverlayVideo
			*/
			virtual Result setMixerVideoSize(int width, int height, const String& background) = 0;
			/** 添加视频到主视频特定位置
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @param[in] zorder 多个视频覆盖时，前后位置，zorder越小，视频越在下层，会被覆盖；取值0-10
			* @param[in] x 此视频在左上角左边位置，即x坐标，取值0.0~1.0，内部取值为：width * X
			* @param[in] y 此视频在左上角上边位置，即y坐标，取值0.0~1.0，内部取值为：height * Y
			* @param[in] w 此视频宽度比例，取值0.0~1.0，内部取值为：width * W
			* @param[in] h 此视频高度比例，取值0.0~1.0，内部取值为：height * H
			* @return 返回错误代码。
			* @sa addMixerOverlayVideo
			*/
			virtual Result addMixerOverlayVideo(const DeviceId& deviceId, int zorder, float x, float y, float w, float h) = 0;
			/** 添加视频到主视频特定位置
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @param[in] zorder 多个视频覆盖时，前后位置，zorder越小，视频越在下层，会被覆盖；取值0-10
			* @param[in] x 此视频在左上角左边位置，即x坐标，
			* @param[in] y 此视频在左上角上边位置，即y坐标，
			* @param[in] w 此视频宽度, 0时取视频本身宽度
			* @param[in] h 此视频高度, 0时取视频本身高度
			* @return 返回错误代码。
			* @sa addMixerOverlayVideo
			*/
			virtual Result addMixerOverlayVideoPixel(const DeviceId& deviceId, int zorder, int x, int y, int w, int h) = 0;
			/** 更新附加视频在主视频上显示的位置
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @param[in] zorder 多个视频覆盖时，前后位置，zorder越小，视频越在下层，会被覆盖；取值0-10
			* @param[in] x 此视频在左上角左边位置，即x坐标，取值0.0~1.0，内部取值为：width * X
			* @param[in] y 此视频在左上角上边位置，即y坐标，取值0.0~1.0，内部取值为：height * Y
			* @param[in] w 此视频宽度比例，取值0.0~1.0，内部取值为：width * W
			* @param[in] h 此视频高度比例，取值0.0~1.0，内部取值为：height * H
			* @return 返回错误代码。
			*/
			virtual Result updateMixerOverlayVideo(const DeviceId& deviceId, int zorder, float x, float y, float w, float h) = 0;
			/** 更新附加视频在主视频上显示的位置
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @param[in] zorder 多个视频覆盖时，前后位置，zorder越小，视频越在下层，会被覆盖；取值0-10
			* @param[in] x 此视频在左上角左边位置，即x坐标，
			* @param[in] y 此视频在左上角上边位置，即y坐标，
			* @param[in] w 此视频宽度, 0时取视频本身宽度
			* @param[in] h 此视频高度, 0时取视频本身高度
			* @return 返回错误代码。
			*/
			virtual Result updateMixerOverlayVideoPixel(const DeviceId& deviceId, int zorder, int x, int y, int w, int h) = 0;
			/** 移除主视频上的某一视频
			*
			* @param[in] deviceId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @return 返回错误代码。
			*/
			virtual Result removeMixerOverlayVideo(const DeviceId& deviceId) = 0;
			/** 清理掉所有的混屏视频，包括主视频；如果需要再次开启合屏，需要重新设置主视频和Overlay视频。
			*
			* @return 返回错误代码。
			*/
			virtual Result clearMixerVideos() = 0;

			//Interface for test
			virtual bool isCameraWait(const DeviceId& fromId, const String& opt) const {return false;}
			virtual bool isCameraOngoing(const DeviceId& fromId, const String& opt) const {return false;}

		protected:
			virtual ~IMVideo() {}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_Video_H_
