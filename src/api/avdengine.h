#ifndef RT_AVDEngine_H_
#define RT_AVDEngine_H_

#include "common.h"
#include "room.h"

namespace tee3 {
	namespace avd {
		/**
		* AVD引擎接口类
		*
		* AVD引擎接口定义了引擎全局初始化操作和房间安排、查询、删除等操作。
		*/
		class RT_API_EXPORT IAVDEngine {
		public:
			/**
			* AVD引擎回调接口类
			*
			* AVD引擎回调接口定义了引擎接口操作的异步返回。
			*/
			class IListener {
			public:
				/// 异步返回
				/** 初始化引擎操作异步返回
				*
				* @param[in] result 初始化错误代码。
				* @sa init
				*/
				virtual void onInitResult(Result result) = 0;

				/** 反初始化引擎通知
				*
				* @param[in] reason 反初始化错误代码。
				* @note 此函数不是引擎的uninit的异步返回，uninit是同步操作。此函数是SDK内部检测到引擎出现了问题后通知应用层的回调。
				*/
				virtual void onUninitResult(Result reason) = 0;


				/** 获取房间信息操作异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 获取房间错误代码。
				* @param[in] room 房间信息。
				*
				* @sa getRoomByRoomId
				* @sa getRoomByAppRoomId
				*/
				virtual void onGetRoomResult(uint32 callId, Result result, const RoomInfo& room) = 0;
				/** 查询房间信息操作异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 查询房间错误代码。
				* @param[in] rooms 房间信息列表。
				*
				* @sa findRoom
				*/
				virtual void onFindRoomsResult(uint32 callId, Result result, const RoomInfosType& rooms) = 0;
				/** 安排房间操作异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 安排房间错误代码。
				* @param[in] roomId 房间Id。
				*
				* @sa scheduleRoom
				*/
				virtual void onScheduleRoomResult(uint32 callId, Result result, const RoomId& roomId) = 0;
				/** 取消房间操作异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 取消房间错误代码。
				* @param[in] roomId 房间Id。
				*
				* @sa cancelRoom
				*/
				virtual void onCancelRoomResult(uint32 callId, Result result, const RoomId& roomId) = 0;
				/** 获取房间中参会者人数异步返回
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] usersCount 房间参会者人数。
				* @param[in] roomId 房间Id。
				* @sa getUsersCount
				*/
				virtual void onGetUsersCountResult(uint32 callId, Result result, uint32 usersCount, const RoomId& roomId) = 0;
			protected:
				virtual ~IListener() {
				}
			};
		public:
			/** 获取AVD引擎对象
			*
			* @note AVD引擎是一个单例对象，一个进程空间只存在一个引擎。
			* @return 返回AVD引擎。
			*/
			static IAVDEngine* Instance();
			/** 释放AVD引擎对象
			*/
			virtual void release() = 0;
		public:
			/** 初始化引擎
			*
			* @param[in] *listener 引擎回调指针。
			* @param[in] severuri 服务器uri。
			* @param[in] appkey 叁体公司分发给应用公司的应用秘钥。
			* @param[in] secretkey 叁体公司分发给应用公司的安全秘钥。
			* @note AVD引擎在做任何功能操作前，必须先要初始化引擎成功。引擎初始化是一个异步操作，调用此接口后等待回调中函数onInitResult回调，判断是否初始化成功。
			* @return 返回错误代码。
			*/
			virtual Result init(
				IListener* listener, const String& severuri, const String& appkey, const String& secretkey) = 0;

			/** 初始化引擎
			*
			* @param[in] *listener 引擎回调指针。
			* @param[in] severuri 服务器uri。
			* @param[in] token 认证后的凭证。
			* @note AVD引擎在做任何功能操作前，必须先要初始化引擎成功。引擎初始化是一个异步操作，调用此接口后等待回调中函数onInitResult回调，判断是否初始化成功。
			* @return 返回错误代码。
			*/
			virtual Result init(IAVDEngine::IListener* listener, const String& severuri, const String& token) = 0;

			/** 初始化引擎
			*
			* @param[in] *listener 引擎回调指针。
			* @param[in] severuri 服务器uri。
			* @param[in] oemName 叁体公司Open-AVD产品OEM的厂家名称。
			* @note AVD引擎在做任何功能操作前，必须先要初始化引擎成功。引擎初始化是一个异步操作，调用此接口后等待回调中函数onInitResult回调，判断是否初始化成功。
			* @return 返回错误代码。
			*/
			virtual Result initWithOEM(
				IListener* listener, const String& severuri, const String& oemName) = 0;

			/** 反初始化引擎
			*
			* @note AVD反初始化后，引擎中涉及的所有房间对象都无效，引擎上的功能函数都无法正常使用。
			* @return 返回错误代码。
			*/
			virtual Result uninit() = 0;

			/** 判断引擎是否正常工作
			*
			* @return 返回引擎是否正常工作。
			*/
			virtual bool isWorking() const = 0;

			//
			/** 根据房间Id获取房间信息
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in] roomId 房间Id。
			*
			* @note 获取房间信息为异步操作，调用此接口后等待回调中函数onGetRoomResult回调，判断是否获取成功。
			* @return 返回错误代码。
			*/
			virtual Result getRoomByRoomId(uint32& callId, const String& roomId) = 0;

			typedef void(*FuncGetRoomResult) (uint32 callId, Result result, const RoomInfo& room);
			virtual Result getRoomByRoomId(uint32& callId, const String& roomId, FuncGetRoomResult func) = 0;

			/** 根据应用层房间Id获取房间信息
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in] appRoomId 应用层房间Id。
			*
			* @note 获取房间信息为异步操作，调用此接口后等待回调中函数onGetRoomResult回调，判断是否获取成功。
			* @return 返回错误代码。
			*/
			virtual Result getRoomByAppRoomId(uint32& callId, const String& appRoomId) = 0;
			virtual Result getRoomByAppRoomId(uint32& callId, const String& appRoomId, FuncGetRoomResult func) = 0;

			/** 查找房间信息
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in] filter 查找条件：json字符串，用房间信息结构中的字段“fieldname=value,...”来表示。
			*
			* @note 查找房间信息为异步操作，调用此接口后等待回调中函数onFindRoomsResult回调，判断是否查找成功。
			* @return 返回错误代码。
			*/
			virtual Result findRooms(uint32& callId, const String& filter) = 0;
			/** 安排房间
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in] roomInfo 房间信息。
			*
			* @note 安排房间为异步操作，调用此接口后等待回调中函数onScheduleRoomResult回调，判断是否安排成功。
			* RoomInfo 中  
			room.roomName  = "xxx";			//房间名称，一般需要填写。
			room.appRoomId = "xxxxxx";		//房间应用层Id，应用层创建房间时设置，应用层可用于标示房间； 必须填写，而且要确保唯一。
			room.maxAttendee = 5;			//房间最大用户数限制, 根据实际需要填写。	
			room.maxAudio = 3;				//房间最大音频数限制, 根据实际需要填写。
			room.maxVideo = 2;				//房间最大视频数限制, 根据实际需要填写。
			room.setRoomMode(tee3::avd::rm_mcu); //房间模式，设置为mcu模式
			以上参数必须要填写， 其他参数可以根据情况做设置。
			*
			* @return 返回错误代码。
			*/
			virtual Result scheduleRoom(uint32& callId, const RoomInfo& roomInfo) = 0;

			typedef void(*FuncScheduleRoomResult) (uint32 callId, Result result, const RoomId& roomId);
			virtual Result scheduleRoom(uint32& callId, const RoomInfo& roomInfo, FuncScheduleRoomResult func) = 0;

			/** 删除房间
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in] roomId 房间Id。
			*
			* @note 删除房间为异步操作，调用此接口后等待回调中函数onCancelRoomResult回调，判断是否取消成功。
			* @return 返回错误代码。
			*/
			virtual Result cancelRoom(uint32& callId, const RoomId& roomId) = 0;

			/** 根据房间号获取房间人数
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in] roomId 房间Id。
			*
			* @note 获取房间人数为异步操作，调用此接口后等待回调中函数onGetUsersCountResult回调，判断是否取消成功。
			* @return 返回错误代码。
			*/
			virtual Result getUsersCount(uint32& callId, const RoomId& roomId) = 0;

		public:
			/** 设置引擎日志配置
			*
			* @param[in] params 日志等级[sensitive verbose info warning error none ]，以上只可取一个值；[debug]是否在调试输出；两个值时中间空格分隔。
			* @param[in] filename 日志文件名，若无日志文件名，则不写文件。
			*
			* @return 返回错误代码。
			*/
			static Result setLogParams(const String& params, const String& filename);

			/** 上传当前日志文件
			* @param[in] rooturl url路径。
			*
			* @return 返回错误代码。
			*/
			static Result uploadLogFile(const String& rooturl);

			/** 获取引擎版本号
			*
			* @return 返回引擎版本号。
			*/
			static String getVersion();
			/** 根据错误代码获取关联的错误信息描述
			* @param[in] code 错误代码。
			* @return 返回错误信息描述。
			*/
			static String getErrorMessage(Result code);

			/** 判断是否拥有功能
			* @param[in] ft 功能枚举。
			* @return 返回是否拥有。
			*/
			virtual bool hasFunction(FunctionType ft) const = 0;
			/** 获取功能集
			* @return 返回功能集。
			* @sa FunctionType
			*/
			virtual uint32 getFunctions() const = 0;
		public:
			/** 设置引擎选项
			*
			* @param[in] type 引擎选项类型。
			* @param[in] *value 引擎选项值指针。
			*
			* @return 返回错误代码。
			*/
			virtual Result setOption(EngineOption type, void* value) = 0;
			virtual Result setOption(EngineOption type, const String& value) = 0;
			/** 获取引擎选项
			*
			* @param[in] type 引擎选项类型。
			* @param[out] *value 引擎选项返回值指针。
			*
			* @return 返回错误代码。
			*/
			virtual Result getOption(EngineOption type, void* value) = 0;
			virtual String getOption(EngineOption type) = 0;

		public:
			/** 设置全局用户信息
			*
			* @param[in] info 用户信息。
			*
			* @return 返回错误代码。
			* @note 全局用户信息设置后，所有房间或直播房间加入都使用此用户信息
			*/
			virtual Result setUserInfo(const User& info) = 0;
			/** 获取全局用户信息
			* @return 返回用户信息。
			*/
			virtual const User& getUserInfo() const = 0;

			virtual const String& getTempDir() const = 0;
		protected:
			virtual ~IAVDEngine() {
			}
		};

	} // namespace avd
} // namespace tee3

#endif//RT_AVDEngine_H_
