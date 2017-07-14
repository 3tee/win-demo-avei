#ifndef RT_EasyRoom_H
#define RT_EasyRoom_H

#include "common.h"
#include "videorender.h"
#include "videocapture.h"
#include "audiocapture.h"
#include "musermanager.h"
#include "mvideomixer.h"
#include "maudio.h"

namespace tee3 {
	namespace avd {
		class User;

		/**
		* 简易房间接口类
		*
		* 简易房间接口定义了独立的简易房间对象创建和释放、房间功能操作。
		*/
		class RT_API_EXPORT IEasyRoom {
		public:
			/**
			* 加入房间回调接口
			* @param[in] result 加入错误代码。
			* @sa join
			*/
			typedef  void(*FuncJoinResult) (const RoomId& roomId, Result result);

			/**
			* 异步操作类型.
			*/
			enum OperateType {
				ot_publishMicrophone = 1,		/**< 房间中分享本地音频  */
				ot_unpublishMicrophone = 2,		/**< 房间中关闭分享本地音频   */
				ot_publishCamera = 3,			/**< 房间中分享本地视频  */
				ot_unpublishCamera = 4,			/**< 房间中关闭分享本地视频  */
				ot_attachRender = 5,			/**< 房间中订阅显示视频  */
				ot_detachRender = 6,			/**< 房间中取消订阅显示视频  */
			};

			/**
			* 简易房间回调接口类
			*
			* 简易房间回调接口定义了房间对象操作的异步返回，房间通知，房间指示。
			*/
			class IListener {
			public:
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
				/// 指示
				/** 指示用户离开房间
				*
				* @param[in] reason 离开房间理由代码，正常离开为AVD_OK。
				* @param[in] fromId 指示发起用户Id。
				* @note SDK内部已经开始释放房间资源，应用层可以不调用leave
				*/
				virtual void onLeaveIndication(Result reason, const UserId& fromId) = 0;

				/// 通知
				/** 透明通道，接收到广播数据通知
				*
				* @param[in] *data 接收到的数据指针。
				* @param[in] len 接收到的数据长度。
				* @param[in] fromId 发送方用户Id。
				*
				* @sa sendPublicData
				*/
				virtual void onPublicData(const char* data, uint32 len, const UserId& fromId) = 0;

				/** 透明通道，接收到私有数据通知
				*
				* @param[in] *data 接收到的数据指针。
				* @param[in] len 接收到的数据长度。
				* @param[in] fromId 发送方用户Id。
				*
				* @sa sendPrivateData
				*/
				virtual void onPrivateData(const char* data, uint32 len, const UserId& fromId) = 0;

				/** 房间网络状态通知
				*
				* @param[in] status 当前网络状态。
				*/
				virtual void onConnectionStatus(ConnectionStatus status) = 0;

				///
				/** 用户加入房间通知
				*
				* @param[in] user 用户信息数据。
				*
				* @note 某用户调用房间中的join加入房间后，房间内所有用户会接收到此通知
				* @sa join
				*/
				virtual void onUserJoinNotify(const User& user) = 0;
				/** 用户离开房间通知
				*
				* @param[in] user 用户信息数据。
				*
				* @note 某用户调用房间中的leave离开房间后，房间内所有用户会接收到此通知
				* @sa leave
				*/
				virtual void onUserLeaveNotify(const User& user) = 0;

				///
				/** 麦克风状态更改通知
				*
				* @param[in] status 麦克风状态。
				* @param[in] fromId 设备关联用户Id。
				* @note 当麦克风状态更改后，房间内所有用户接收到此通知。
				* @sa openMicrophone
				* @sa closeMicrophone
				*/
				virtual void onMicrophoneStatusNotify(MicrophoneStatus status, const UserId& fromId) = 0;
				/** 摄像头状态更改通知
				*
				* @param[in] status 摄像头状态。
				* @param[in] fromId 设备关联用户Id。
				* @note 当摄像头状态更改后，房间内所有用户接收到此通知, notify when publishCamera, unpublishCamera。
				*/
				virtual void onCameraStatusNotify(CameraStatus status, const UserId& fromId) = 0;

				/// 异步返回
				/** 异步返回
				* @param[in] type 操作类型。
				* @param[in] result 操作错误代码。
				* @note 当异步操作时,异步返回结果。
				*/
				virtual void onOperateResult(OperateType type, Result result) = 0;
			protected:
				virtual ~IListener() {}
			};

		///
		public:
			/** 获取或创建房间接口
			*
			* @param[in] roomId 房间Id。
			* @note 只支持单用户应用，不支持多用户应用。
			* @return 返回房间接口指针。
			*/
			static IEasyRoom* obtain(const RoomId& roomId);
			/** 释放房间接口
			*
			*@note 释放房间后，App层不能再使用当前房间指针，所有引用房间指针处均需设置为NULL。
			*/
			virtual void release() = 0;

		public: /// 房间加入退出逻辑
			/** 设置房间回调接口
			*
			* @param[in] *listener 回调接口指针。
			*
			* @return 返回错误代码。
			* @see IListener
			*/
			virtual Result setListener(IListener* listener) = 0;
			/** 加入房间
			*
			* @param[in,out] user 用户信息：用户Id(不能包含'{'、'}'字符)，用户名，用户数据等；当应用层不传入user.userId时，SDK将创建一个新的guid作为user.userId，同时返回给应用层。
			* @param[in] joinresult 加房间回调接口。
			* @note 加入房间时，可以指定结果单独回调接口；如果指定，则在指定单独接口回调，不会在Listener的onJoinResult回调。
			* @return 返回错误代码。
			* @sa User
			*/
			virtual Result join(const User& user, FuncJoinResult joinresult) = 0;
			virtual bool   isWorking() const = 0;
			/** 当前用户离开房间
			*
			* @param[in] reason 用户离开房间的原因代码，正常离开为AVD_OK。
			*
			* @note 当前用户离开房间后，当前房间对象和房间的各功能模块都处于离会状态，功能操作无效。
			* @return 返回错误代码。
			*/
			virtual Result leave(Result reason) = 0;				// leave myself
			/** 房间中踢出用户
			*
			* @param[in] reason 踢出用户的理由代码。
			* @param[in] userId 被踢出用户Id。
			*
			* @note 被踢用户将会在onLeaveIndication中接收到回调，被踢用户直接调用 leave 离开房间即可。
			* @return 返回错误代码。
			* @sa onLeaveIndication
			*/
			virtual Result kickoutUser(Result reason, const UserId& userId) = 0;  // leave someone else, who's onLeaveIndication

			/** 透明通道，发送广播数据
			*
			* @param[in] *data 发送的数据指针。
			* @param[in] len 发送的数据长度。
			*
			* @note 此透明通道用于承载应用层数据，广播给所有房间内用户。
			* @return 返回错误代码。
			*/
			virtual Result sendPublicData(const char* data, uint32 len) = 0;

			/** 透明通道，发送私有数据
			*
			* @param[in] *data 发送的数据指针。
			* @param[in] len 发送的数据长度。
			* @param[in] toId 目的用户Id。
			*
			* @note 此透明通道用于承载应用层数据，发送给房间内某一特定用户。
			* @return 返回错误代码。
			*/
			virtual Result sendPrivateData(const char* data, uint32 len, const UserId& toId) = 0;

		public: /// 房间信息和用户信息操作
			/** 获取房间Id
			*
			* @return 返回房间Id。
			*/
			virtual const RoomId& getRoomId() const = 0;
			/** 获取房间信息
			*
			* @return 返回房间信息。
			* @sa RoomInfo
			*/
			virtual const RoomInfo& getRoomInfo() const = 0;
			/** 获取本用户信息
			* @return 返回本用户信息。
			*/
			virtual const User& getSelfUser() const = 0;
			/** 获取本用户Id
			* @return 返回本用户Id。
			*/
			virtual const UserId& getSelfUserId() const = 0;
			virtual uint32 getParticipantsCount() const = 0;
			/** 获取房间中的用户列表
			* @param[in] begindex 起始索引。
			* @param[in,out] ncount 请求数量；实际返回数量。
			* @param[out] users 用户列表集合。
			* @note 用户列表不包含本用户。
			* @return 返回错误代码。
			*/
			virtual Result getParticipants(uint32 begindex, uint32& ncount, UsersType& users) = 0;

		public: /// 音视频操作
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
			/** 开启直播（主播）或对讲（观众）前: 设置视频的输入数据源
			*
			* @param[in] *vcapturer 视频的输入数据源。
			* @return 返回错误代码。
			* @sa FakeVideoCapturer
			*/
			virtual Result setVideoSource(FakeVideoCapturer* vcapturer) = 0;
			/** 开启直播（主播）或对讲（观众）前: 设置音频的输入数据源
			*
			* @param[in] *acapturer 音频的输入数据源。
			* @return 返回错误代码。
			* @sa FakeAudioCapturer
			*/
			virtual Result setAudioSource(FakeAudioCapturer* acapturer) = 0;

			/** 打开本地麦克风发布到房间中
			*
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onOpenMicrophoneResult 中返回。
			* 只支持单个音频输入设备，如果设置了 FakeAudioCapturer优先使用外部导入音频； 
			* 如果没有设置外部导入音频源，根据设置使用缺省音频设备。
			* @sa onOpenMicrophoneResult
			*/
			virtual Result publishMicrophone() = 0;
			/** 关闭本地麦克风发布到房间中
			*
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onCloseMicrophoneResult 中返回。
			* @sa onCloseMicrophoneResult
			*/
			virtual Result unpublishMicrophone() = 0;

			/** 预览本地缺省摄像头视频
			*
			* @param[in] *render 显示对象接口指针
			* @note 本地视频在发布到会议前，可以调用此接口进行视频预览；视频发布到会议中后，需要使用attachRender接口来进行视频显示，此时此接口无效。
			* 只支持单个视频输入设备，如果设置了 FakeVideoCapturer 优先使用外部导入视频；
			* 如果没有设置外部导入视频源，根据设置使用缺省视频设备。
			* @return 返回错误代码。
			* @sa IVideoRender
			*/
			virtual Result previewCamera(IVideoRender* render) = 0;
			/** 取消预览缺省摄像头视频
			* @return 返回错误代码。
			*/
			virtual Result unpreviewCamera() = 0;
			/** 发布缺省摄像头视频
			*
			* @return 返回错误代码。
			* @note 发布操作为异步操作，操作结果在 onPublishResult 中返回。
			* @sa onPublishResult
			*/
			virtual Result publishCamera() = 0;
			/** 取消发布缺省摄像头视频
			*
			* @return 返回错误代码。
			* @note 取消发布操作为异步操作，操作结果在 onUnpublishResult 中返回。
			* @sa onUnpublishResult
			*/
			virtual Result unpublishCamera() = 0;
			/** 关联视频数据和显示对象，在render中显示视频
			*
			* @param[in] userId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @param[in] *render 显示对象接口指针。
			*
			* @note 视频数据可以在多个显示对象上显示，而一个显示对象某一时刻只能显示一路视频。
			*		在本地视频发布到房间中，或者远端视频订阅成功后，调用此接口完成视频与显示对象的对接，从而在显示对象中显示视频。
			* @return 返回错误代码。
			* @sa IVideoRender
			*/
			virtual Result attachRender(const UserId& userId, IVideoRender* render) = 0;
			/** 解除摄像头视频数据的所有显示对象关联
			*
			* @param[in] userId 摄像头Id，唯一标示一路共享的摄像头视频。
			* @return 返回错误代码。
			*/
			virtual Result detachRender(const UserId& userId) = 0;
			/** 视频切换摄像头
			* @return 返回错误代码。
			*/
			virtual Result switchCamera() = 0;

			/** 判断本地麦克风是否静默
			* @return 返回静默状态。
			*/
			virtual bool ismuteMicrophone() const = 0;
			/** 本地麦克风静默
			* @return 返回错误代码。
			*/
			virtual Result muteMicrophone() = 0;
			/** 本地麦克风静默取消
			* @return 返回错误代码。
			*/
			virtual Result unmuteMicrophone() = 0;

		public: /// 音视频导出操作
			/** 设置获取混音数据回调接口
			* @param[in] listener 混音数据回调指针。
			* @param[in] sampleRate 导出混音数据的采样率：16000,44100,48000。
			* @return 返回错误代码。
			* @sa IAudioRender
			* @sa ro_audio_mixerdata_callback_buffered 单次混音数据回调“10ms数据”还是“1024samples数据”
			* @note 如果音视频同时混合出流，建议先设置视频，再设置音频的回调接口，有助于音视频时间戳对齐。
			*/
			virtual Result setAudioMixerDataListener(IAudioRender* listener, uint32 sampleRate) = 0;
			/** 设置获取视频数据回调接口,此数据仅回调一份数据：数据为主视频+各个Overlay的视频，视频大小为主视频大小；如使用ILivecast接口，不需要设置此接口。
			*
			* @return 返回错误代码。
			* @sa IVideoRender
			* @note 如果音视频同时混合出流，建议先设置视频，再设置音频的回调接口，有助于音视频时间戳对齐。
			*/
			virtual Result setVideoMixerDataListener(IVideoRender* listener) = 0;
			/** 获取视频混频布局接口
			* @return 混频布局接口
			*/
			virtual IMVideoMixer* getVideoMixer() = 0;

		public: /// 房间统计信息
			/** 当前房间启用或关闭流量统计功能
			* @param[in] isEnable 启用或关闭。
			* @return 返回错误代码：
			* 	AVD_OK						成功
			*/
			virtual Result enableStats(bool isEnable) = 0;
			/** 获取房间的流量统计信息
			* @return 返回房间流量统计信息。
			* @sa RoomStats
			*/
			virtual const RoomStats& getRoomStats() const = 0;
		public:
			/** 设置房间选项
			*
			* @param[in] type 房间选项类型。
			* @param[in] value 房间选项值字符串。
			* @return 返回错误代码。
			*/
			virtual Result setOption(RoomOption type, const String& value) = 0;
			/** 获取房间选项
			*
			* @param[in] type 房间选项类型。
			* @return 返回房间选项字符串。
			*/
			virtual String getOption(RoomOption type) = 0;

		protected:
			virtual ~IEasyRoom() {}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_EasyRoom_H
