#ifndef RT_ILocalRecord_H_
#define RT_ILocalRecord_H_

#include "common.h"
#include "room.h"

namespace tee3 {
	namespace avd {

		/**
		* 录制管理接口，录制文件到本地.
		*
		* 录制管理定义引擎中录制的创建、停止、录制控制等功能。
		*/
		class RT_API_EXPORT IMLocalRecord : public IModule {
		public:
			/**
			* 录制中录制音视频流回调接口.
			*/
			class RT_API_EXPORT StreamOut {
			public:
				/** 视频流回调接口
				*
				* @param[in] recHandle 录制模块句柄。
				* @param[in] timestamp_ns 时间戳, nanos second
				* @param[in] w 图像宽度
				* @param[in] h 图像高度
				* @param[in] data 图像内容数据指针
				* @param[in] len  图像内容大小
				* @note 视频流的数据格式为h264或者vp8等编码后格式，具体由视频发布端发布时确定。
				* 视频时间戳和音频时间戳的回调是独立的，回调时没有前后关系，即视频回调某一时间戳A后，音频后续回调的时间戳B可能比A小；反之亦然。
				* 音视频的数据回调函数中不能堵塞，不能做费时操作。
				* @sa avd_localrecord_createRecorder
				*/
				virtual void videoStreamOut(const String& recHandle, uint64 timestamp_ns, unsigned int w, unsigned int h, bool isKeyFrame, const uint8 *data, unsigned int len) = 0;

				/** 音频流回调接口
				*
				* @param[in] recHandle 录制模块句柄。
				* @param[in] timestamp_ns 时间戳, nanos second。
				* @param[in] sampleRate 音频采样率
				* @param[in] channels 音频采集通道数
				* @param[in] data 音频内容数据指针
				* @param[in] len  音频内容大小
				* @note 音频流的数据格式为PCM 16位。
				* 视频时间戳和音频时间戳的回调是独立的，回调时没有前后关系，即视频回调某一时间戳A后，音频后续回调的时间戳B可能比A小；反之亦然。
				* 音视频的数据回调函数中不能堵塞，不能做费时操作。
				* @sa avd_localrecord_createRecorder
				*/
				virtual void audioStreamOut(const String& recHandle, uint64 timestamp_ns, int sampleRate, int channels, const uint8 *data, unsigned int len) = 0;
			protected:
				virtual ~StreamOut() {}
			};
			/** 获取房间内本地录制模块接口
			*
			* @param[in] room 房间对象指针。
			*
			* @return 返回本地录制模块接口指针。
			*/
			static IMLocalRecord* getRecord(IRoom* room);
			/** 创建一个录制容器
			*
			* @param[in] filePath 录制文件地址。
			* @param[in] recorderId 外部分配录制ID，必须保证房间内该录制模块中具有唯一性，如果是null或""将内部分配一个UUID，并通过return 返回。
			* @param[in] continueDecode SDK是否继续内部解码（若要用SDK内部的视频渲染等功能，则需要解码）。
			* @return 有效的本地录制ID，如果返回null或者""，标示创建失败。
			*/
			virtual Result createRecorder(const String& filePath, String& recorderId, bool continueDecode=true) = 0;
			/** 创建一个录制容器
			*
			* @param[in] StreamOut 录制内容输出接口指针。
			* @param[in] recorderId 外部分配录制ID，必须保证房间内该录制模块中具有唯一性，如果是null或""将内部分配一个UUID，并通过return 返回。
			* @param[in] continueDecode SDK是否继续内部解码（若要用SDK内部的视频渲染等功能，则需要解码）。
			* @return 有效的本地录制ID，如果返回null或者""，标示创建失败。
			*/
			virtual Result createRecorder(StreamOut* out, String& recorderId, bool continueDecode=true) = 0;
			/** 为录制容器选择房间内的某路视频
			*
			* @param[in] recorderId 录制ID。
			* @param[in] deviceId 视频所对应的设备ID。
			*
			* @return 返回错误代码。
			* @note 如果deviceId设备对应的视频有多路流pub或sub，将按照MainStream > Assistant1Stream > Assistant2Stream的优先级选择。如果是单流，这里选中的是pub或sub的那路流
			*	
			*/
			virtual Result selectVideo4Recorder(const String& recorderId, const String& deviceId) = 0;
			/** 为录制容器选择房间内的某路音频
			*
			* @param[in] recorderId 录制ID。
			* @param[in] userId 音频所属的用户ID。
			*
			* @return 返回错误代码。
			*/
			virtual Result selectAudio4Recorder(const String& recorderId, const UserId& userId) = 0;
			/** 为某个录制容器选择录制房间内的所有音频
			*
			* @param[in] recorderId 录制ID。
			*
			* @return 返回错误代码。
			*/
			virtual Result selectAllAudio4Recorder(const String& recorderId) = 0;
			/** 为某个录制容器选择录制房间内的除了我自己外的所有音频
			*
			* @param[in] recorderId 录制ID。
			*
			* @return 返回错误代码。
			*/
			virtual Result selectAllAudioWithoutMe4Recorder(const String& recorderId) = 0;
			/** 停止该录制模块中的某个录制
			*
			* @param[in] recorderId 录制ID。
			*
			* @return 返回消息列表。
			*/
			virtual Result stopRecorder(const String& recorderId) = 0;
			/** 停止该录制模块中所有录制
			*
			* @return 返回错误代码。
			*/
			virtual Result stopRecorderAll() = 0;
		protected:
			virtual ~IMLocalRecord() {}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_ILocalRecord_H_
