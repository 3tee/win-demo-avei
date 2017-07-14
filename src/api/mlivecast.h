#ifndef RT_IMLivecast_H_
#define RT_IMLivecast_H_

#include "common.h"
#include "room.h"

namespace tee3 {
	namespace avd {

		class RT_API_EXPORT IMLivecast {
		public:
			class IListener {
			public:
				virtual void onLivecastStatus(LivecastStatus status) = 0;
			protected:
				virtual ~IListener() {}
			};
		public:
//			static IMLivecast* getLivecast(IRoom* room);
			virtual Result setListener(IListener* listener) = 0;
			virtual Result startPublish(const String& url) = 0;
			virtual Result stopPublish() = 0;
			// 自动布局合屏窗口，设置布局参数

			// 手动布局合屏窗口
		protected:
			virtual ~IMLivecast() {}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_IMLivecast_H_
