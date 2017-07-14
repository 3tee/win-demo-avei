#ifndef RT_AVDRecord_H_
#define RT_AVDRecord_H_

#include "common.h"
#include "avdengine.h"

namespace tee3 {
	namespace avd {
		/**
		* 录制数据结构.
		*
		* 录制数据定义了一个录制的基本信息。
		*/
		struct RecordInfo {
			RecordId id;					/**< 录制ID */
			String name;					/**< 录制的名称 */
			String description;				/**< 录制描述 */
			String tags;					/**< 录制标记 */
			UserId userId;					/**< 用户号，可不存在，存在时与audioType，videoType一起使用；录制此用户的相关音频和视频数据 */
			MediaFileType   fileType;		/**< 媒体文件类型 */
			RecordAudioType audioType;		/**< 音频类型 */
			RecordVideoType videoType;		/**< 视频类型 */
			String url;						/**< 播放录制链接 */
			uint64 size;					/**< 录制文件大小 */
			String createTime;				/**< 录制创建起始时间 */
			uint64 duration;				/**< 录制从创建到现在的时长 */
			RecordStatus status;			/**< 录制状态 */
			RoomId roomId;					/**< 频道关联房间号 */
		public:
			RecordInfo() :fileType(mt_mp4), audioType(ra_user_single), videoType(rv_main), size(0), duration(0), status(rs_created){}
			bool isValid() const { return !id.empty(); }
		};
		typedef RecordInfo* LPRecordInfo;
		const String toString(const RecordInfo&  obj);

		/**
		* 录制信息集合类型.
		*/
		typedef std::vector<RecordInfo> RecordInfosType;

		/**
		* 旁路录制管理接口.
		*
		* 旁路录制管理定义引擎中录制的创建、删除、信息获取和录制控制等功能，该录制成功后文件存储在RecorderServer上，而不是本地。
		*/
		class RT_API_EXPORT IAVDRecord {
		public:
			/**
			* 旁路录制回调接口类
			*
			* 旁路录制回调接口定义了录制管理操作的异步返回。
			*/
			class IListener {
			public:
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
				/** 创建录制的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] info 录制信息。
				*
				* @sa createRecord
				*/
				virtual void onCreateRecord(uint32 callId, Result result, const RecordInfo& info) = 0;
				/** 为特定录制设置当前视频源的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] recordId 录制号。
				* @param[in] selectedVideo 已发布的视频Id。
				*
				* @sa selectVideo4Record
				*/
				virtual void onSelectMedia4Record(uint32 callId, Result result, const RecordId& recordId, const DeviceId& selectedVideo) = 0;
				/** 创建特定用户录制的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] info 录制信息。
				*
				* @sa createUserRecord
				*/
				virtual void onCreateUserRecord(uint32 callId, Result result, const RecordInfo& info) = 0;
				/** 停止特定录制的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] recordId 录制号。
				*
				* @sa stopRecord
				*/
				virtual void onStopRecord(uint32 callId, Result result, const RecordId& recordId) = 0;
				
				///
				/** 获取特定录制详细信息的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] info 录制信息。
				*
				* @sa getRecordInfo
				*/
				virtual void onRecordInfo(uint32 callId, Result result, const RecordInfo& info) = 0;
				/** 获取服务器上录制列表的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] total  总共直播数。
				* @param[in] begin  当前起始序号。
				* @param[in] items 录制信息集合。
				*
				* @sa getWorkingRecords
				*/
				virtual void onRecordInfos(uint32 callId, Result result, uint32 total, uint32 begin, const RecordInfosType& items) = 0;
				/** 删除特定录制的异步返回
				*
				* @param[in] callId 调用Id。
				* @param[in] result 错误代码。
				* @param[in] recordId 录制号。
				*
				* @sa deleteRecord
				*/
				virtual void onDeleteRecord(uint32 callId, Result result, const RecordId& recordId) = 0;
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
			* @note 暂时未实现； 为异步操作，操作结果在 onWorkingRooms 中返回。
			* @sa onWorkingRooms
			*/
			virtual Result getWorkingRooms(uint32& callId, uint32 begindex, uint32 ncount) = 0;
			/** 获取特定房间已经发布的视频列表
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  roomId 房间号。
			* @return 返回错误代码。
			* @note 暂时未实现； 为异步操作，操作结果在 onPublishedVideos 中返回。
			* @sa onPublishedVideos
			*/
			virtual Result getPublishedVideos(uint32& callId, const RoomId& roomId) = 0;
		public:
			/** 获取引擎的旁路录制功能接口
			*
			* @return 返回旁路录制接口。
			*/
			static IAVDRecord* getRecord(IAVDEngine *engine);
			/** 释放功能对象
			*/
			static void release();
			/** 设置录制功能回调
			*
			* @param[in] *listener 回调指针。
			* @return 返回错误代码。
			*/
			virtual Result setListener(IListener* listener) = 0;	
			//
			/** 创建一个录制
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  info 中roomId，name必须设置；创建成功时id会设置成新创建的录制Id，在异步返回中返回。
			* @note 暂时未实现； 为异步操作，操作结果在 onCreateRecord 中返回。
			* @sa onCreateRecord
			*/
			virtual Result createRecord(uint32& callId, const RecordInfo& info) = 0;
			/** 为特定录制设置当前视频源
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in] recordId 录制号。
			* @param[in] roomId 视频相关房间号。
			* @param[in] selectedVideo 已发布的视频Id。
			* @return 返回错误代码。
			* @note 暂时未实现； 为异步操作，操作结果在 onSelectMedia4Record 中返回。
			* @sa onSelectMedia4Record
			*/
			virtual Result selectMedia4Record(uint32& callId, const RecordId& recordId, const RoomId& roomId, const DeviceId& selectedVideo) = 0;
			/** 创建特定用户录制
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  info 中roomId，name, userid, fileType必须设置；创建成功时id会设置成新创建的录制Id，在异步返回中返回。
			* @return 返回错误代码。
			* @note 参数结构中的音频类型缺省为：ra_user_single
								视频类型缺省为：rv_main
			*		为异步操作，操作结果在 onCreateUserRecord 中返回。
			* @sa onCreateUserRecord
			*/
			virtual Result createUserRecord(uint32& callId, const RecordInfo& info) = 0;
			/** 停止特定录制
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  recordId 录制号。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onStopRecord 中返回。
			* @sa onStopRecord
			*/
			virtual Result stopRecord(uint32& callId, const RecordId& recordId) = 0;

			/** 获取特定录制详细信息
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  recordId 录制号。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onRecordInfo 中返回。
			* @sa onRecordInfo
			*/
			virtual Result getRecordInfo(uint32& callId, const RecordId& recordId) = 0;
			/** 获取服务器上的录制列表
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  begindex 分页的起始索引。
			* @param[in]  ncount 分页的请求数量。
			* @param[in]  filter 查询条件，以json表示，如：{ "roomId":"hafds2324" }。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onRecordInfos 中返回。
			* @sa onRecordInfos
			*/
			virtual Result findRecordInfos(uint32& callId, uint32 begindex, uint32 ncount, const String& filter) = 0;
			/** 删除特定录制
			*
			* @param[in,out] callId 调用Id，用于区别多次调用，在回调中会带上此调用Id，便于应用层区别每次调用；若调用时为0，内部产生一个序号。
			* @param[in]  recordId 录制号。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onDeleteRecord 中返回。
			* @sa onDeleteRecord
			*/
			virtual Result deleteRecord(uint32& callId, const RecordId& recordId) = 0;
		protected:
			virtual ~IAVDRecord() {}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_AVDRecord_H_
