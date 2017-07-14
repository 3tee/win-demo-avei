#ifndef RT_WhiteBoard_H_
#define RT_WhiteBoard_H_

#include "common.h"
#include "room.h"
namespace tee3 {
	namespace avd {
		class IMAnnotation;
		class IRoom;
		/**
		* 白板信息结构.
		*/
		typedef struct  tagWhiteboardInfo {
			DeviceId	id;
			String			title;
			String			description;
			bool			isShared;
			tagWhiteboardInfo() :isShared(false) {}
		}Whiteboard;
		/**
		* 白板集合类型.
		*/
		typedef std::vector<Whiteboard> WhiteboardsType;

		class RT_API_EXPORT IMWhiteboard : public IModule{
		public:
			//class IListener {
			//public:
			//	virtual void onShareWhiteboardResult(Result rv, WhiteboardId id) = 0;
			//	virtual void onCloseWhiteboardResult(Result rv, WhiteboardId id) = 0;
			//protected:
			//	virtual ~IListener() {}
			//};
			/** 获取白板模块接口
			*
			* @param[in] room 房间对象指针。
			*
			* @return 返回白板模块接口指针。
			*/
			static IMWhiteboard* getWhiteboard(IRoom* room);
			virtual Result shareWhiteboard(Whiteboard& wb) = 0;
			virtual Result closeWhiteboard(DeviceId id) = 0;
			virtual IMAnnotation* getAnnotation(DeviceId id) = 0;

			virtual Result setBackground(DeviceId id, String filePath) = 0;
			virtual Result setBackground(DeviceId id, uint8 r, uint8 g, uint8 b, uint8 a) = 0;
			virtual Result setWidth(DeviceId id, uint32 w) = 0;
			virtual Result setHeight(DeviceId id, uint32 h) = 0;
			virtual Result getSharedWhiteboards(WhiteboardsType& wbs) = 0;
		protected:
			virtual ~IMWhiteboard() {}
		};
	}
}
#endif
