#ifndef RT_Room_H
#define RT_Room_H

#include "common.h"

namespace tee3 {
	namespace avd {
		class User;
		class IMUserManager;


		/**
		* 房间接口类
		*
		* 房间接口定义了房间对象创建和释放、房间功能操作。
		*/
		class RT_API_EXPORT IRoom {
		public:

			/**
			* 加入房间回调接口
			* @param[in] result 加入错误代码。
			* @sa join
			*/
			typedef  void(*FuncJoinResult) (const RoomId& roomId, Result result);

			/**
			* 房间回调接口类
			*
			* 房间回调接口定义了房间对象操作的异步返回，房间通知，房间指示。
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
				* reason: 
				* Err_User_DuplicateJoin			用户Id在别处登录，先前同一Id被踢出；
				* Err_User_DataConnection_Failed	数据链接连接失败，用户无法正常使用房间，用户被服务器踢出；
				* 其他								kickoutUser 传入的错误码，被其他用户踢出，应用层自己定义；
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

				/** 房间应用层数据更改通知
				*
				* @param[in] key 应用层数据关键字。
				* @param[in] value 应用层数据关键字相关数据内容。
				*
				* @sa updateAppData
				*/
				virtual void onAppDataNotify(const String& key, const String& value) = 0;

				/** 房间状态通知
				*
				* @param[in] status 房间状态。
				*/
				virtual void onRoomStatusNotify(RoomStatus status) = 0;

				/** 房间网络状态通知
				*
				* @param[in] status 当前网络状态。
				*/
				virtual void onConnectionStatus(ConnectionStatus status) = 0;
			protected:
				virtual ~IListener() {}
			};
		public:
			/** 获取或创建房间接口
			*
			* @param[in] roomId 房间Id。
			* @note 只支持单用户应用，不支持多用户应用。
			* @return 返回房间接口指针。
			*/
			static IRoom* obtain(const RoomId& roomId);
			/** 获取或创建房间接口
			*
			* @param[in] roomId 房间Id。
			* @param[in] userId 用户Id。
			* @note 支持一个进程中：一个房间加入多个用户的应用，如性能测试工具、代理网关等应用；房间由房间Id和用户Id组合来唯一标识。
			*						要注意此函数与obtain(const RoomId& roomId)的区别，在多用户应用中必须调用此函数获取房间，而单用户
			*						应用可以调用任意一个函数来使用。
			* @return 返回房间接口指针。
			*/
			static IRoom* obtain(const RoomId& roomId, const UserId& userId);
			/** 释放房间接口
			*
			*@note 释放房间后，App层不能再使用当前房间指针，所有引用房间指针处均需设置为NULL。
			*      不能在onLeaveIndication等回调函数中来调用此函数，会引起死锁。
			*/
			virtual void release() = 0;

		public:
			/** 设置房间回调接口
			*
			* @param[in] *listener 回调接口指针。
			*
			* @return 返回错误代码。
			* @see IListener
			*/
			virtual Result setListener(IListener* listener) = 0;

			/** 当前用户加入房间
			*
			* @param[in,out] user 用户信息：用户Id(不能包含'{'、'}'字符)，用户名，用户数据等；当应用层不传入user.userId时，SDK将创建一个新的guid作为user.userId，同时返回给应用层。
			* @param[in] password 密码：如果是主持人请用主持人密码；如果是其他参会者，请用房间密码。
			* @param[in] joinresult 加房间回调接口。
			* @note 加入房间时，可以指定结果单独回调接口；如果指定，则在指定单独接口回调，不会在Listener的onJoinResult回调。
			* @return 返回错误代码。
			* @sa User
			*/
			virtual Result join(const User& user, const String& password, FuncJoinResult joinresult) = 0;

			/** 当前用户离开房间
			*
			* @param[in] reason 用户离开房间的原因代码，正常离开为AVD_OK。
			*
			* @note 当前用户离开房间后，当前房间对象和房间的各功能模块都处于离会状态，功能操作无效。
			*      不能在onLeaveIndication等回调函数中来调用此函数，会引起死锁。
			* @return 返回错误代码。
			*/
			virtual Result leave(Result reason) = 0;				// leave myself

			/** 关闭房间
			*
			* @note 只有主持人（host）才能关闭房间。
			* @return 返回错误代码。
			*/
			virtual Result close() = 0;

			/** 重新加入当前房间
			*
			* @note 当用户主动离开房间或者中间网络断线后离开房间后，可以重新调用此函数加入房间。
			* @return 返回错误代码。
			*/
			virtual Result reJoin() = 0;

			/** 重置房间中网络连接
			*
			* @note 当上层检测到网络切换等情况下，可以调用此接口进行网络连接重置。
			* @return 返回错误代码。
			*/
			virtual Result reConnect() = 0;

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

			/** 更新房间状态
			*
			* @param[in] status 房间状态。
			*
			* @return 返回错误代码。
			* @sa RoomStatus
			*/
			virtual Result updateRoomStatus(RoomStatus status) = 0;

			/** 获取房间状态
			*
			* @return 返回房间状态。
			* @sa RoomStatus
			*/
			virtual RoomStatus getRoomStatus() const = 0;

			/** 获取房间Id
			*
			* @return 返回房间Id。
			*/
			virtual const RoomId& getRoomId() const = 0;

			/** 获取房间应用层Id
			*
			* @note 房间应用层Id在应用层安排当前房间的时候由应用层来指定，方便应用层查找、标示房间。
			* @return 返回房间应用层Id。
			*/
			virtual const String& getAppRoomId() const = 0;

			/** 获取房间名
			*
			* @return 返回房间名。
			*/
			virtual const String& getRoomName() const = 0;

			/** 获取房间主题
			*
			* @return 返回房间主题。
			*/
			virtual const String& getRoomTopic() const = 0;

			/** 获取房间信息
			*
			* @return 返回房间信息。
			* @sa RoomInfo
			*/
			virtual const RoomInfo& getRoomInfo() const = 0;

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

			/** 返回应用层的房间数据
			*
			* @param[in] key 房间数据关键字。
			*
			* @note 应用层的房间数据方便应用层在房间中保留关于房间的一些业务逻辑数据，可以获取和修改应用层的房间数据。
			* @return 返回应用层的相应关键字的房间数据。
			*/
			virtual String getAppData(const String& key) const = 0;
			/** 修改应用层的房间数据
			*
			* @param[in] key 房间数据关键字。
			* @param[in] value 房间数据对应关键字的信息内容
			*
			* @return 返回错误代码。
			*/
			virtual Result updateAppData(const String& key, const String& value) = 0;
			/** 创建模拟设备Id，在导入音视频时可用
			*
			* @param[in] fakeId 自定义Id。
			* @return 模拟设备Id。
			* @note 模拟摄像头信息，接口会生成deviceId，生成规则："userId_"+（用户输入的fakeId), 此处deviceId不能包含'_'和'{'、'}'此三个字符。
			* @sa previewLocalCamera
			* @sa publishLocalCamera
			*/
			virtual DeviceId createFakeDeviceID(const DeviceId& fakeId) = 0;

			virtual bool isWorking() const = 0;

			/** 当前用户加入房间
			*
			* @param[in,out] user 用户信息：用户Id(不能包含'{'、'}'字符)，用户名，用户数据等；当应用层不传入user.userId时，SDK将创建一个新的guid作为user.userId，同时返回给应用层。
			* @param[in] password 密码：如果是主持人请用主持人密码；如果是其他参会者，请用房间密码。
			* @param[in] joinresult 加房间回调接口。
			* @param[in] roomToken 房间token。
			* @note 加入房间时，可以指定结果单独回调接口；如果指定，则在指定单独接口回调，不会在Listener的onJoinResult回调。
			* @return 返回错误代码。
			* @sa User
			*/
			virtual Result join(const User& user, const String& password, FuncJoinResult joinresult, const String& roomToken) = 0;

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
			/** 获取某路媒体（音频或者视频）的流量统计信息
			* @param[in] deviceId 设备Id。
			* @return 返回媒体统计信息。
			* @sa MediaStats
			*/
			virtual const MediaStats& getMediaStats(const DeviceId& deviceId) const = 0;
			virtual const MediaStats& getAudioStats() = 0;

		public:
			/** 设置房间选项
			*
			* @param[in] type 房间选项类型。
			* @param[in] *value 房间选项值指针。
			*
			* @return 返回错误代码。
			*/
			virtual Result setOption(RoomOption type, void* value) = 0;
			virtual Result setOption(RoomOption type, const String& value) = 0;
			/** 获取房间选项
			*
			* @param[in] type 房间选项类型。
			* @param[out] *value 房间选项返回值指针。
			*
			* @return 返回错误代码。
			*/
			virtual Result getOption(RoomOption type, void* value) = 0;
			virtual String getOption(RoomOption type) = 0;

		protected:
			virtual ~IRoom() {}
		};

		/**
		* 房间模块接口类
		*
		* 房间模块接口定义了房间模块公共的操作。
		*/
		class RT_API_EXPORT IModule {
		public:
			/** 获取房间接口指针
			*
			* @return 返回房间接口指针。
			* @sa IRoom
			*/
			virtual IRoom* getRoom() = 0;

			/** 获取房间用户管理模块指针
			*
			* @return 返回用户管理模块指针。
			* @sa IMUserManager
			*/
			virtual IMUserManager* getUserManager() = 0;

			/** 判断是否是自己的用户Id
			* @param[in] userId 用户Id
			* @return 返回是否是自己。
			*/
			virtual bool isSelfUser(const UserId& userId) const = 0;

			/** 判断设备Id是否是属于自己的
			* @param[in] deviceId 设备Id
			* @return 返回设备Id是否是属于自己的。
			*/
			virtual bool isSelfDevice(const DeviceId& deviceId) const = 0;

			/** 获取设备Id的关联用户Id
			* @param[in] deviceId 设备Id
			* @return 返回用户Id。
			*/
			virtual const UserId getOwnerId(const DeviceId& deviceId) const = 0;
			/** 获取设备Id的关联用户名称
			* @param[in] deviceId 设备Id
			* @return 返回用户名称。
			*/
			virtual const String& getOwnerName(const DeviceId& deviceId) const = 0;

		protected:
			virtual ~IModule() {}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_Room_H
