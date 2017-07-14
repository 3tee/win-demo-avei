#ifndef RT_Screen_H_
#define RT_Screen_H_

#include "common.h"
#include "room.h"
#include "videocapture.h"

namespace tee3 {
	namespace avd {
		class IRoom;
		class IVideoRender;

		/**
		* 房间中屏幕窗口共享模块接口.
		*
		* 房间屏幕窗口共享模块定义房间中屏幕窗口共享相关功能接口。
		*/
		class RT_API_EXPORT IMScreen : public IModule {
		public:
			/**
			* 房间中屏幕窗口共享模块回调接口.
			*
			* 屏幕窗口共享模块回调接口定义了房间中屏幕窗口共享相关的通知和异步操作返回。
			*/
			class IListener {
			public:
				/// 通知
				/** 屏幕窗口状态更改通知
				*
				* @param[in] status 屏幕窗口状态。
				* @param[in] fromId 屏幕窗口Id，唯一标示一个屏幕或者一个应用窗口。
				* @note 当屏幕窗口状态更改后，房间内所有用户接收到此通知。
				*/
				virtual void onScreenStatusNotify(ScreenStatus status, const DeviceId& fromId) = 0;
				/** 屏幕窗口数据更改通知
				*
				* @param[in] level 屏幕窗口整形数据。
				* @param[in] description 屏幕窗口字符串数据。
				* @param[in] fromId 屏幕窗口Id，唯一标示一个屏幕窗口。
				* @note 当屏幕窗口数据更改后，房间内所有用户接收到此通知, notify when publishLocalScreen(Camera), updateScreenData。
				*/
				virtual void onScreenDataNotify(int level, const String& description, const DeviceId& fromId) = 0;
				/** 屏幕窗口视频发布通知
				*
				* @param[in] screen 屏幕窗口信息，屏幕信息中level和description为应用层数据，应用层可以使用这些字段保存次屏幕窗口相关的应用逻辑数据，方便标示、订阅视频。
				*
				* @note 当屏幕窗口视频发布时，房间内所有用户接收到此通知；因通知的重要程度，将从onScreenStatusNotify中分离出此状态。
				* @sa publishScreen
				*/
				virtual void onPublishScreenNotify(const ScreenWindow& screen) = 0;
				/** 屏幕窗口视频取消发布通知
				*
				* @param[in] screen 屏幕窗口信息。
				*
				* @note 当屏幕窗口视频取消发布时，房间内所有用户接收到此通知；因通知的重要程度，将从onScreenStatusNotify中分离出此状态。
				* @sa unpublishScreen
				*/
				virtual void onUnpublishScreenNotify(const ScreenWindow& screen) = 0;

				/// 异步返回
				/** 本用户订阅屏幕窗口异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] fromId 屏幕窗口Id，唯一标示一个屏幕或者一个应用窗口。
				*
				* @sa subscribe
				*/
				virtual void onSubscribeScreenResult(Result result, const DeviceId& fromId) = 0;
				/** 本用户取消订阅屏幕窗口异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] fromId 屏幕窗口Id，唯一标示一个屏幕或者一个应用窗口。
				*
				* @sa unsubscribe
				*/
				virtual void onUnsubscribeScreenResult(Result result, const DeviceId& fromId) = 0;
				/** 本用户发布屏幕窗口异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] fromId 屏幕窗口Id，唯一标示一个屏幕或者一个应用窗口。
				*
				* @sa publishScreen
				*/
				virtual void onPublishScreenResult(Result result, const DeviceId& fromId) = 0;
				/** 本用户取消发布屏幕窗口异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] fromId 屏幕窗口Id，唯一标示一个屏幕或者一个应用窗口。
				*
				* @sa unpublishScreen
				*/
				virtual void onUnpublishScreenResult(Result result, const DeviceId& fromId) = 0;

			protected:
				virtual ~IListener() {
				}
			};
		public:
			/** 获取房间内屏幕窗口共享模块接口
			*
			* @param[in] room 房间对象指针。
			* @return 返回屏幕窗口共享模块接口指针。
			*/
			static IMScreen* getScreen(IRoom* room);
			//
			/** 设置房间屏幕窗口共享模块回调接口
			*
			* @param[in] *listener 远程屏幕窗口共享模块回调接口指针。
			* @return 返回错误代码。
			* @sa IListener
			*/
			virtual Result setListener(IListener* listener) = 0;
			/** 预览共享屏幕窗口视频
			*
			* @param[in] device 屏幕窗口信息。
			* @param[in] render 屏幕窗口信息。
			* @return 返回错误代码。
			*/
			virtual Result previewScreen(const ScreenWindow& device, IVideoRender* render) = 0;
			/** 取消预览共享屏幕窗口视频
			*
			* @return 返回错误代码。
			*/
			virtual Result unpreviewScreen() = 0;
			/** 发布共享屏幕窗口视频
			*
			* @param[in] screen 屏幕窗口信息，用户可以在信息结构的level和description字段中放置应用层逻辑数据，方便视频描述和订阅。
			* @return 返回错误代码。
			* @note 发布操作为异步操作，操作结果在 onPublishScreenResult 中返回。
			* @sa onPublishScreenResult
			*/
			virtual Result publishScreen(const ScreenWindow& screen) = 0;

			// Define platform specific window types.
#if defined(RT_DESKTOP)
			/** 设置共享屏幕时排除共享的窗口列表
			*
			* @param[in] excluded 排除共享的窗口列表。
			* @return 返回错误代码。
			*/
			virtual Result setExcludedWindows(std::vector<WindowId>& excluded) = 0;
#endif//RT_DESKTOP

			/** 发布模拟屏幕窗口视频
			*
			* @param[in,out] fakeDevice 模拟屏幕窗口信息，接口会生成deviceId，生成规则："userId_"+（用户输入的deviceId), 此处deviceId不能包含'_'和'{'、'}'此三个字符。
			* @param[in] *capture 模拟屏幕窗口视频输入接口。
			* @return 返回错误代码。
			* @note 发布操作为异步操作，操作结果在 onPublishLocalResult 中返回。
			* 模拟屏幕窗口用于在房间中导入特定已经存在的视频数据流作为共享屏幕，比如视频文件、录播系统视频流等。
			* @sa onPublishLocalResult
			* @sa FakeVideoCapturer
			*/
			virtual Result publishScreen(ScreenWindow& fakeDevice, FakeVideoCapturer* capture) = 0;
			/** 取消发布共享屏幕视频
			*
			* @return 返回错误代码。
			* @note 取消发布操作为异步操作，操作结果在 onUnpublishScreenResult 中返回。
			* @sa onUnpublishScreenResult
			*/
			virtual Result unpublishScreen() = 0;
			/** 共屏屏幕窗口视频切换另外一屏幕窗口
			*
			* @param[in] deviceId 屏幕窗口Id。
			* @return 返回错误代码。
			*/
			virtual Result swithToScreen(const DeviceId& deviceId) = 0;
			/** 共屏屏幕窗口数据更改
			*
			* @param[in] deviceId 屏幕窗口Id，唯一标示一个屏幕窗口。
			* @param[in] level 屏幕窗口整形数据。
			* @param[in] description 屏幕窗口字符串数据。
			* @sa onScreenDataNotify
			*/
			virtual Result updateScreenData(const DeviceId& deviceId, int level, const String& description) = 0;
			/** 订阅共屏屏幕窗口视频
			*
			* @param[in] deviceId 屏幕窗口Id，唯一标示一路共享的屏幕窗口视频。
			* @return 返回错误代码。
			* @note 订阅操作为异步操作，操作结果在 onSubscribeResult 中返回。
			* @sa onSubscribeResult
			*/
			virtual Result subscribe(const DeviceId& deviceId) = 0;
			/** 取消订阅共屏屏幕窗口视频
			*
			* @param[in] deviceId 屏幕窗口Id，唯一标示一路共享的屏幕窗口视频。
			* @return 返回错误代码。
			* @note 取消订阅操作为异步操作，操作结果在 onUnsubscribeResult 中返回。
			* @sa onUnsubscribeResult
			*/
			virtual Result unsubscribe(const DeviceId& deviceId) = 0;
			/** 获取已共享屏幕窗口信息
			*
			* @param[in,out] items 屏幕窗口集合存放列表。
			* @return 返回错误代码。
			*/
			virtual Result getPublishedScreens(ScreensType& items) const = 0;
			/** 获取已订阅屏幕窗口信息
			*
			* @param[in,out] items 屏幕窗口集合存放列表。
			* @return 返回错误代码。
			*/
			virtual Result getSubscribedScreens(ScreensType& items) const = 0;
			/** 判断屏幕窗口是否已经被本用户订阅
			* @param[in] fromId 屏幕窗口Id。
			* @return 返回是否是已订阅屏幕窗口。
			*/
			virtual bool isScreenSubscribed(const DeviceId& fromId) const = 0;

			/// render screen
			/** 关联共享的屏幕窗口视频数据和显示对象，在render中显示视频
			*
			* @param[in] deviceId 屏幕窗口Id，唯一标示一路共享的屏幕窗口视频。
			* @param[in] *render 显示对象接口指针。
			*
			* @note 视频数据可以在多个显示对象上显示，而一个显示对象某一时刻只能显示一路视频。
			*		在屏幕窗口视频发布到房间中，或者远端屏幕窗口视频订阅成功后，调用此接口完成视频与显示对象的对接，从而在显示对象中显示视频。
			* @return 返回错误代码。
			* @sa IVideoRender
			*/
			virtual Result attachRender(const DeviceId& deviceId, IVideoRender* render) = 0;
			/** 解除屏幕窗口视频的所有显示对象关联
			*
			* @param[in] deviceId 屏幕窗口Id，唯一标示一路共享的屏幕窗口视频。
			* @return 返回错误代码。
			* @sa IVideoRender
			*/
			virtual Result detachRender(const DeviceId& deviceId) = 0;
			/** 解除某一显示对象与屏幕窗口视频的关联
			*
			* @param[in] *render 显示对象接口指针。
			* @sa IVideoRender
			*/
			virtual Result detachRender(IVideoRender* render) = 0;

			/// screen manager
			/** 获取本机屏幕窗口信息列表
			*
			* @param[in,out] items 屏幕窗口集合存放列表。
			* @return 返回错误代码。
			*/
			virtual Result getScreenWindows(ScreensType& items) = 0;

#if defined(RT_DESKTOP)
			/// 注释
			/**
			* 共享端： 显示注释工具条；首次准备透明窗口；app应用限制注释区域；接收注释端数据，回放注释端数据；
			* 浏览端： 观看桌面视频
			* 注释端： 显示注释工具条；准备透明窗口；限制注释区域；    注释-->（收集鼠标+注释状态+用户信息）--> 发送给共享端；
			*
			*/

			/** 开启注释
			*
			* @param[in] screenId 已经publish的ScreenWindow 的ID。
			* @return 返回错误代码。
			* @note 
			* 共享端调用的前置条件是：screenId指向的ScreenWindow已共享
			* 浏览端调用的前置条件是：screenId指向的ScreenWindow已订阅，并且已在Render中渲染，
			* 内部会根据Render确定注释在本地显示的窗口，调用startOtherAnnotation(screenId, render)
			* 调用该接口，工具条是SDK内部已实现，如果需要自己定制工具条 请参照mannotation.h文件
			* @sa startOtherAnnotation
			*/
			virtual Result startAnnotation(const DeviceId& screenId) = 0;
			/** 关闭注释
			*
			* @param[in] screenId 已经publish的ScreenWindow 的ID。
			* @return 返回错误代码。
			* @note
			* 只是停止注释，内部不会调用UnpublishScreen，
			*/
			virtual Result stopAnnotation(const DeviceId& screenId) = 0;

			/** 开始浏览端注释
			*
			* @param[in] screenId 要注释的，已经publish的ScreenWindow 的ID。
			* @param[in] render 已经publish的Screen所在的渲染窗口，而不是新建的渲染窗口。
			* @return 返回错误代码。
			* @note
			* screenId指向的ScreenWindow已订阅，并且会在Render中渲染
			*/
			virtual Result startOtherAnnotation(const DeviceId& screenId, IVideoRender* render) = 0;
#endif//RT_DESKTOP

		protected:
			virtual ~IMScreen() {
			}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_Screen_H_
