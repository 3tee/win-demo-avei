#ifndef RT_AVDLive_H_
#define RT_AVDLive_H_

#include "common.h"
#include "avdengine.h"
#include "room.h"

namespace tee3 {
	namespace avd {
		/**
		* 直播频道数据结构.
		*
		* 直播频道数据定义了一个直播频道的基本信息。
		*/
		struct LiveInfo {
			LiveId id;						/**< 频道的ID */
			String name;					/**< 直播频道的名称 */
			String description;				/**< 直播频道描述 */
			String tags;					/**< 直播标记 */
			UserId userId;					/**< 用户号，可不存在，存在时与audioType，videoType一起使用；直播此用户的相关音频和视频数据 */
			RecordAudioType audioType;		/**< 音频类型 */
			RecordVideoType videoType;		/**< 视频类型 */
			String createTime;				/**< 频道创建起始时间 */
			uint32 duration;				/**< 频道从创建到现在的时长 */
			String playerPassword;			/**< 播放密码 */
			RoomId roomId;					/**< 频道关联房间号 */
			LiveStatus status;				/**< 直播状态 */
			String publishurl;				/**< 推流地址，创建时可带上，若存在，服务器优先推送此地址 */
			String rtmpurl;					/**< 播放rtmp链接 */
			String hlsurl;					/**< 播放hls链接 */
		public:
			LiveInfo() :audioType(ra_user_single), videoType(rv_main), duration(0), status(ls_created){}
			bool isValid() const { return !id.empty(); }
		};
		const String toString(const LiveInfo&  obj);

		/**
		* 直播频道信息集合类型.
		*/
		typedef std::vector<LiveInfo> LiveInfosType;

		/**
		* 直播频道管理接口.
		*
		* 直播频道管理定义引擎中直播频道的创建、删除、信息获取和频道控制等功能。
		* 直播频道：类似电视频道，能够承载一路视频和音频的一个逻辑通道；直播源通过直播频道发布视频，直播播放端通过直播频道订阅和观看直播音视频。
		*/
		class RT_API_EXPORT IAVDLive {
		public:
			/**
			* 直播频道回调接口类
			*
			* 直播频道回调接口定义了直播频道管理操作的异步返回。
			*/
			class IListener {
			public:
				/// 异步返回
				/** 获取当前正在进行房间列表的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] total  总共直播数。
				* @param[in] begin  当前起始序号。
				* @param[in] items 房间信息集合。
				*
				* @sa getWorkingRooms
				*/
				virtual void onWorkingRooms(uint32 callId, Result result, uint32 total, uint32 begin, const RoomInfosType& items) = 0;
				/** 获取特定房间已经发布的视频列表
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] roomId 房间号。
				* @param[in] items 已发布的视频信息集合。
				*
				* @sa getWorkingRooms
				*/
				virtual void onPublishedVideos(uint32 callId, Result result, const RoomId& roomId, const VideoDevicesType& items) = 0;

				///
				/** 创建直播频道的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] info 直播频道信息。
				*
				* @sa createLive
				*/
				virtual void onCreateLive(uint32 callId, Result result, const LiveInfo& info) = 0;
				/** 为特定直播频道设置当前视频源的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] lvbc_id 频道号。
				* @param[in] selectedVideo 已发布的视频Id。
				*
				* @sa selectMedia4Live
				*/
				virtual void onSelectMedia4Live(uint32 callId, Result result, const LiveId& lvbc_id, const DeviceId& selectedVideo) = 0;
				/** 创建特定用户直播频道的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] info 录制信息。
				*
				* @sa createUserLive
				*/
				virtual void onCreateUserLive(uint32 callId, Result result, const LiveInfo& info) = 0;
				/** 停止特定直播频道的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] lvbc_id 频道号。
				*
				* @sa stopLive
				*/
				virtual void onStopLive(uint32 callId, Result result, const LiveId& lvbc_id) = 0;
				/** 获取特定频道详细信息的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] info 直播频道信息。
				*
				* @sa getLiveInfo
				*/
				virtual void onLiveInfo(uint32 callId, Result result, const LiveInfo& info) = 0;
				/** 获取当前正在直播频道列表的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] total  总共直播数。
				* @param[in] begin  当前起始序号。
				* @param[in] items 直播频道信息集合。
				*
				* @sa getLiveInfos
				*/
				virtual void onLiveInfos(uint32 callId, Result result, uint32 total, uint32 begin, const LiveInfosType& items) = 0;
				/** 删除特定频道的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] lvbc_id 频道号。
				*
				* @sa deleteLive
				*/
				virtual void onDeleteLive(uint32 callId, Result result, const LiveId& lvbc_id) = 0;
			protected:
				virtual ~IListener() {}
			};
		public:
			/** 获取当前正在进行房间列表
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  begindex 分页的起始索引。
			* @param[in]  ncount 分页的请求数量。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onWorkingRooms 中返回。
			* @sa onWorkingRooms
			*/
			virtual Result getWorkingRooms(uint32& callId, uint32 begindex, uint32 ncount) = 0;
			/** 获取特定房间已经发布的视频列表
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  roomId 房间号。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onPublishedVideos 中返回。
			* @sa onPublishedVideos
			*/
			virtual Result getPublishedVideos(uint32& callId, const RoomId& roomId) = 0;
		public:
			/** 获取引擎的直播功能接口
			*
			* @return 返回直播接口。
			*/
			static IAVDLive* getLive(IAVDEngine *engine);
			/** 释放功能对象
			*/
			static void release();
			/** 设置直播功能回调
			*
			* @param[in] *listener 回调指针。
			* @return 返回错误代码。
			*/
			virtual Result setListener(IListener* listener) = 0;
			/** 创建直播频道
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in,out] info 中roomId，name必须设置；创建成功时id会设置成新创建的直播频道Id。
			* @note 为异步操作，操作结果在 onCreateLive 中返回。
			* @sa onCreateLive
			*/
			virtual Result createLive(uint32& callId, const LiveInfo& info) = 0;
			/** 为特定直播频道设置当前视频源
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  lvbc_id 频道号。
			* @param[in]  roomId 视频相关房间号。
			* @param[in]  selectedVideo 已发布的视频Id。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onSelectVideo4Live 中返回。
			* @sa onSelectMedia4Live
			*/
			virtual Result selectMedia4Live(uint32& callId, const LiveId& lvbc_id, const RoomId& roomId, const DeviceId& selectedVideo) = 0;
			/** 创建特定用户直播频道
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  info 中roomId，name, userid必须设置；创建成功时id会设置成新创建的录制Id，在异步返回中返回。
			* @return 返回错误代码。
			* @note 参数结构中的音频类型缺省为：ra_user_single
			视频类型缺省为：rv_main
			*		为异步操作，操作结果在 onCreateUserLive 中返回。
			* @sa onCreateUserLive
			*/
			virtual Result createUserLive(uint32& callId, const LiveInfo& info) = 0;
			/** 暂停特定频道
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  lvbc_id 频道号。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onStopLive 中返回。
			* @sa onStopLive
			*/
			virtual Result stopLive(uint32& callId, const LiveId& lvbc_id) = 0;
			/** 获取特定频道详细信息
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  lvbc_id 频道号。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onLiveInfo 中返回。
			* @sa onLiveInfo
			*/
			virtual Result getLiveInfo(uint32& callId, const LiveId& lvbc_id) = 0;
			/** 获取当前正在直播的频道列表
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  begindex 分页的起始索引。
			* @param[in]  ncount 分页的请求数量。
			* @param[in]  filter 查询条件，以json表示，如：{ "roomId":"hafds2324" }。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onLiveInfos 中返回。
			* @sa onLiveInfos
			*/
			virtual Result findLiveInfos(uint32& callId, uint32 begindex, uint32 ncount, const String& filter) = 0;

			/** 删除特定频道
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  lvbc_id 频道号。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onDeleteLive 中返回。
			* @sa onDeleteLive
			*/
			virtual Result deleteLive(uint32& callId, const LiveId& lvbc_id) = 0;
		protected:
			virtual ~IAVDLive() {}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_AVDLive_H_
