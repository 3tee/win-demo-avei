#ifndef RT_Chat_H_
#define RT_Chat_H_

#include "common.h"
#include "room.h"

namespace tee3 {
	namespace avd {
		class IRoom;

		/**
		* 聊天消息结构.
		*
		* 聊天消息定义了一次传输聊天的信息。
		*/
		struct Message {
			uint32	 timestamp;						/**< 消息时间戳，对应用层是本地时间，传输中用GMT时间 */
			UserId   fromId;						/**< 消息发送者Id */
			String	 fromName;						/**< 消息发送者名 */
			String   message;						/**< 消息内容 */
		};
		/**
		* 聊天消息列表类型.
		*/
		typedef std::vector<Message> MessagesType;

		/**
		* 房间中聊天模块接口.
		*
		* 房间聊天模块定义房间中公聊和私聊等聊天功能接口。
		*/
		class RT_API_EXPORT IMChat : public IModule {
		public:
			/**
			* 房间中聊天模块回调接口.
			*
			* 聊天模块回调接口定义了房间中聊天相关的通知。
			*/
			class IListener {
			public:
				/// 通知
				/** 接收到公聊消息通知
				*
				* @param[in] message 消息内容。
				* @note 当用户发送公聊消息后，房间内所有用户除发送者外都将接收到此通知。
				* @sa Message
				* @sa sendPublicMessage
				*/
				virtual void onPublicMessage(const Message& message) = 0;
				/** 接收到私聊消息通知
				*
				* @param[in] message 消息内容。
				* @note 当用户发送私聊消息后，只有消息目的用户接收到此消息通知。
				* @sa Message
				* @sa sendPrivateMessage
				*/
				virtual void onPrivateMessage(const Message& message) = 0;
			protected:
				virtual ~IListener() {
				}
			};

			/** 获取房间内聊天模块接口
			*
			* @param[in] room 房间对象。
			* @return 聊天模块接口。
			* @sa IRoom
			*/
			static IMChat* getChat(IRoom* room);

			/** 设置房间聊天模块回调接口
			*
			* @param[in] *listener 聊天模块回调接口指针。
			* @return 返回错误代码。
			* @sa IListener
			*/
			virtual Result setListener(IListener* listener) = 0;
			/** 发送公聊消息
			*
			* @param[in] message 消息内容。
			*
			* @return 返回错误代码。
			*/
			virtual Result sendPublicMessage(const String& message) = 0;
			/** 发送私聊消息
			*
			* @param[in] message 消息内容。
			* @param[in] toId 私聊消息目的用户Id。
			*
			* @return 返回错误代码。
			*/
			virtual Result sendPrivateMessage(const String& message, const UserId& toId) = 0;

			/** 获取公聊历史消息记录
			*
			* @param[in] latest 是否是最新记录（即反序）。
			* @param[in] begindex 消息起始索引。
			* @param[in,out] ncount 请求消息数量；实际消息返回数量。
			* @param[out] msgs 保存消息列表。
			*
			* @return 返回错误代码。
			*/
			virtual Result getPublicHistoryMessage(bool latest, uint32 begindex, uint32& ncount, MessagesType& msgs) = 0;
			/** 获取私聊历史消息记录
			*
			* @param[in] userId 聊天对方的用户Id。
			* @param[in] latest 是否是最新记录（即反序）。
			* @param[in] begindex 消息起始索引。
			* @param[in,out] ncount 请求消息数量；实际消息返回数量。
			* @param[out] msgs 保存消息列表。
			*
			* @return 返回错误代码。
			*/
			virtual Result getPrivateHistoryMessage(const UserId& userId, bool latest, uint32 begindex, uint32& ncount, MessagesType& msgs) = 0;

		protected:
			virtual ~IMChat() {
			}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_Chat_H_
