#ifndef RT_UserManager_H_
#define RT_UserManager_H_

#include "common.h"

namespace tee3 {
	namespace avd {
		class IRoom;
		/**
		* 用户数据结构.
		*
		* 用户数据定义了用户的基本信息。
		*/
		class User {
		public:
			UserId userId;							/**< 用户Id，应用层可设置 */
			UserStatus status;						/**< 用户状态集合  */
			String userName;						/**< 用户名称，应用层设置  */
			String userData;						/**< 用户数据，应用层设置  */
			uint32 nodeId;
		public:
			bool isValid() const { return !userId.empty(); }
			User() : nodeId(0) {}
		};
		/**< 用户集合类型 */
		typedef std::vector<User> UsersType;

		/**
		* 用户管理接口类
		*
		* 用户管理接口定义了房间内所有用户和用户列表相关的功能操作。
		*/
		class RT_API_EXPORT IMUserManager {
		public:
			/**
			* 用户管理回调接口类
			*
			* 用户管理回调接口定义了房间用户和用户列表操作的异步返回，房间用户通知。
			*/
			class IListener {
			public:
				/// 通知
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
				/** 用户信息更改通知
				*
				* @param[in] user 用户信息数据。
				*
				* @note 某用户调用updateUser更改自己信息后，房间内所有用户会接收到此通知
				* @sa updateUser
				*/
				virtual void onUserUpdateNotify(const User& user) = 0;
				/** 用户状态更改通知
				*
				* @param[in] status 当前用户状态。
				* @param[in] fromId 关联的用户Id。
				*
				* @note 某用户调用updateSelfUserStatus更改自己状态后，房间内所有用户会接收到此通知；
				*       此通知从onUserUpdateNotify中分离出来，因用户状态更改较为频繁
				* @sa updateSelfUserStatus
				* @sa onUserUpdateNotify
				*/
				virtual void onUserStatusNotify(UserStatus status, const UserId& fromId) = 0;
				/** 用户应用层数据更改通知
				*
				* @param[in] userData 当前用户应用层数据。
				* @param[in] fromId 关联的用户Id。
				*
				* @note 某用户调用updateSelfUserData更改自己应用层数据后，房间内所有用户会接收到此通知
				*       此通知从onUserUpdateNotify中分离出来，因应用层数据用户较为关注
				* @sa updateSelfUserData
				* @sa onUserUpdateNotify
				*/
				virtual void onUserDataNotify(const String& userData, const UserId& fromId) = 0;

			protected:
				virtual ~IListener() {}
			};
		public:
			/** 获取房间内用户管理模块接口
			*
			* @param[in] room 房间对象指针。
			*
			* @return 返回用户管理模块接口指针。
			*/
			static IMUserManager* getUserManager(IRoom* room);

			//
			/** 设置用户管理回调接口
			*
			* @param[in] *listener 回调接口指针。
			*
			* @return 返回错误代码。
			* @see IListener
			*/
			virtual Result setListener(IListener* listener) = 0;

			/** 更新本用户信息
			*
			* @param[in] user 用户信息。
			*
			* @return 返回错误代码。
			*/
			virtual Result updateUser(const User& user) = 0;
			/** 判断用户是否存在
			*
			* @param[in] userId 用户Id。
			*
			* @return 是否存在用户。
			*/
			virtual bool hasUser(const UserId& userId) const = 0;
			/** 获取关联Id的用户信息
			*
			* @param[in] userId 用户Id。
			*
			* @return 返回关联Id的用户信息。
			*/
			virtual const User& getUser(const UserId& userId) const = 0;
			/** 获取主持人信息
			* @return 返回主持人信息。
			*/
			virtual const User& getHost() const = 0;
			/** 获取本用户信息
			* @return 返回本用户信息。
			*/
			virtual const User& getSelfUser() const = 0;
			/** 获取本用户Id
			* @return 返回本用户Id。
			*/
			virtual const UserId& getSelfUserId() const = 0;
			/** 获取本用户应用层数据
			* @return 返回本用户应用层数据。
			*/
			virtual const String& getSelfUserData() const = 0;
			/** 更改本用户的应用层数据
			*
			* @param[in] userData 本用户应用层数据。
			* @return 返回错误代码。
			*/
			virtual Result updateSelfUserData(const String& userData) = 0;
			/** 更改本用户的用户状态
			*
			* @param[in] status 用户状态。
			* @return 返回错误代码。
			*/
			virtual Result updateSelfUserStatus(uint32 status) = 0;
			/** 获取房间中的用户数量
			* @return 返回本用户数量。
			*/
			virtual uint32 getParticipantsCount() const = 0;
			/** 获取房间中的用户列表
			*
			* @param[in] begindex 起始索引。
			* @param[in,out] ncount 请求数量；实际返回数量。
			* @param[out] users 用户列表集合。
			* @note 用户列表不包含本用户。
			* @return 返回错误代码。
			*/
			virtual Result getParticipants(uint32 begindex, uint32& ncount, UsersType& users) = 0;

		protected:
			virtual ~IMUserManager() {}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_UserManager_H_
