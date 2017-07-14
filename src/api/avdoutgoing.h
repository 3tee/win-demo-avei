#ifndef RT_AVDOutgoing_H_
#define RT_AVDOutgoing_H_

#include "common.h"
#include "avdengine.h"
#include "musermanager.h"

namespace tee3 {
	namespace avd {
		/**
		* 外呼设备接入管理接口.
		*
		* 外呼设备接入管理定义引擎中房间呼叫rtsp流，h323和sip设备等功能。
		* 同一房间内，同一user_address只有一个呼入；不同房间中，同一user_address的呼入可以同时存在。
		*/
		class RT_API_EXPORT IAVDOutgoing {
		public:
			/**
			* 外呼设备接入回调接口类
			*
			* 外呼设备接入回调接口定义了引擎接口操作的异步返回。
			*/
			class IListener {
				/// 异步返回
			public:
				/** 外呼用户创建操作异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 用户创建错误代码。
				* @param[in] roomId 房间Id。
				* @param[in] userId 用户Id。
				* @param[in] user_address 用户真实地址
				* @sa createOutgoingUser
				*/
				virtual void onCreateOutgoingUser(uint32 callId, Result result, const RoomId& roomId, const UserId& userid, const String& user_address) = 0;
				/** 外呼用户删除操作异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 用户删除错误代码。
				* @param[in] roomId 房间Id。
				* @param[in] userId 用户Id。
				* @param[in] user_address 用户真实地址
				* @sa destoryOutgoingUser
				*/
				virtual void onDestoryOutgoingUser(uint32 callId, Result result, const RoomId& roomId, const UserId& userid, const String& user_address) = 0;
				/** 外呼用户列表查询操作异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 用户列表查询错误代码。
				* @param[in] roomId 房间Id。
				*
				* @sa getOutgoingUsers
				*/
				virtual void onGetOutgoingUsers(uint32 callId, Result result, const RoomId& roomId, const UsersType& users) = 0;

			protected:
				virtual ~IListener() {
				}
			};
		public:
			/** 获取外呼设备接入接口
			*
			* @return 返回外呼设备接入接口。
			*/
			static IAVDOutgoing* getOutgoing(IAVDEngine *engine);
			/** 释放功能对象
			*/
			static void release();
			/** 设置外呼设备接入功能回调
			*
			* @param[in] *listener 回调指针。
			* @return 返回错误代码。
			*/
			virtual Result setListener(IListener* listener) = 0;

		public:
			// 外呼接口
			/** 外呼用户创建
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in] roomId 房间Id。
			* @param[in] user 用户信息，参考Room.join中对user的说明。
			* @param[in] user_address 用户真实地址，根据外呼用户的类型有区别：
			* 如rtsp为“rtsp://192.168.0.1:6549/avd”
			*	sip 为“sip://192.168.0.1:6549/avd"
			*	h323为“h323://192.168.0.1:6549/avd"
			* @param[in] loginName 用户真实地址的登陆名。
			* @param[in] loginPass 用户真实地址的登陆密码。
			* @note外呼用户创建为异步操作，调用此接口后等待回调中函数onCreateOutgoingUserResult回调，判断是否获取成功。
			*	   外呼用户包括： rtsp外呼；h323外呼；sip外呼等；
			* @return 返回错误代码。
			* @sa onCreateOutgoingUser
			*/
			virtual Result createOutgoingUser(uint32& callId, const RoomId& roomId, const User& user,
				const String& user_address, const String& loginName, const String& loginPass) = 0;
			/** 外呼用户删除
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in] roomId 房间Id。
			* @param[in] userId 用户Id。
			* @param[in] user_address 用户真实地址。
			* @note外呼用户删除为异步操作，调用此接口后等待回调中函数onDestoryOutgoingUser回调，判断是否获取成功。
			* @return 返回错误代码。
			* @sa onDestoryOutgoingUser
			*/
			virtual Result destoryOutgoingUser(uint32& callId, const RoomId& roomId, const UserId& userid, const String& user_address) = 0;
			/** 外呼用户列表查询
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in] roomId 房间Id，此参数可为空，如果为空，返回整个服务器上的外呼用户列表；否则返回某一房间的外呼用户列表。
			* @note外呼用户列表查询为异步操作，调用此接口后等待回调中函数onGetOutgoingUsers回调，判断是否获取成功。
			* @return 返回错误代码。
			* @sa onGetOutgoingUsers
			*/
			virtual Result getOutgoingUsers(uint32& callId, const RoomId& roomId) = 0;

		protected:
			virtual ~IAVDOutgoing() {
			}
		};
	} // namespace avd
} // namespace tee3
#endif//RT_AVDOutgoing_H_
