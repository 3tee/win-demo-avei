#ifndef RTC_VideoRender_H_
#define RTC_VideoRender_H_

#include "common.h"

namespace tee3 {
	namespace avd {
		class IVideoRender;
		class IVideoFrame;
		class IMAnnotation;

		/**
		* 外部对象销毁通知接口
		*
		* 当对象被销毁的时候，通知所有引用的对象，取消引用关系。
		*/
		class RT_API_EXPORT DestroyNotify {
		public:
			class IListener {
			public:
				virtual void OnDestroy(DestroyNotify* destroy) = 0;
			};
			typedef std::vector<IListener*> ListenersType;

			/** 设置渲染对象销毁回调指针
			*
			* @param[in] pNotify 销毁回调指针。
			*/
			virtual Result AddDestroyNotify(IListener* pListener) {
				for (ListenersType::size_type i = 0; i < pListeners_.size(); ++i) {
					if (pListener == pListeners_[i]) {
						return AVD_OK;
					}
				}
				pListeners_.push_back(pListener);
				return AVD_OK;
			}

			/** 删除渲染对象销毁回调指针
			*
			* @param[in] pNotify 销毁回调指针。
			*/
			virtual Result RemoveDestroyNotify(IListener* pListener) {
				for (ListenersType::iterator it = pListeners_.begin(); it != pListeners_.end(); it++) {
					if ((*it) == pListener) {
						pListeners_.erase(it);
						break;
					}
				}
				return AVD_OK;
			}
			virtual ~DestroyNotify() {
				ListenersType listeners = pListeners_;
				pListeners_.clear();
				for (ListenersType::const_iterator it = listeners.begin(); it != listeners.end(); it++) {
					(*it)->OnDestroy(this);
				}
				listeners.clear();
			}
		protected:
			ListenersType	pListeners_;
		};

		/**
		* 视频渲染接口类
		*
		* 视频渲染定义了视频的渲染中涉及的视频参数变化、视频帧数据、视频抓图等相关接口。
		*/
		class RT_API_EXPORT IVideoRender : public DestroyNotify {
		public:

			/** 视频宽高变化调用
			*
			* @param[in] width  视频宽度。
			* @param[in] height 视频高度。
			*/
			virtual void SetSize(int width, int height) = 0;
			/** 视频帧数据到达调用
			*
			* @param[in] frame IVideoFrame图像对象指针。
			*/
			virtual void RenderFrame(const IVideoFrame* frame) = 0;
			//
			/** 启停图像抓取功能
			*
			* @param[in] enable 开启或者停止。
			* @return 返回错误代码。
			* @note 渲染的时候，正常情况下不缓存图像； 若需要capture才做缓存；因EnableCapture后，要下帧图像放到缓存中才能做Save2Image。
			* 故若需要capture时，在attachRender后就直接EnableCapture较好。
			*/
			virtual Result EnableCapture(bool enable) {
				return Err_Not_Implemented;
			}
			/** 抓取图像
			*
			* @param[in] fileName 图像保存地址，绝对地址, 支持".bmp和.jpg"。
			* @param[in] quality  图像质量：0-100, 0最差，100最好。
			* @return 返回错误代码。
			*/
			virtual Result CaptureImage(const String& fileName, int quality) {
				return Err_Not_Implemented;
			}

			/** 获取render所在的WindowId
			*
			* @param[in,out] id  显示对象句柄。
			* @return 返回错误代码。
			*/
			virtual Result GetWindowId(WindowId& id) const {
				return Err_Not_Implemented;
			}

			/** 获取图像大小
			*
			* @param[out] width  图像宽度。
			* @param[out] height  图像高度。
			* @return 返回错误代码。
			*/
			virtual Result GetSize(int& width, int& height) const {
				return Err_Not_Implemented;
			}

			/** 设置注释接口
			*
			* @param[out] atn  注释模块接口。
			* @return 返回错误代码。
			*/
			virtual Result SetAnnotation(tee3::avd::IMAnnotation* atn) {
				return Err_Not_Implemented;
			}

			/** 获取注释接口
			*
			* @param[out] atn  注释模块接口。
			* @return 返回错误代码。
			*/
			virtual Result GetAnnotation(tee3::avd::IMAnnotation** atn) {
				return Err_Not_Implemented;
			}
			/** IVideoFrame图像保存为jpeg图像功能
			*
			* @param[in] frame IVideoFrame图像对象指针。
			* @param[in] jpegorbmpFileName 图像保存地址，绝对地址, 支持".bmp和.jpg"。
			* @param[in] quality  图像质量：0-100, 0最差，100最好。
			* @return 返回错误代码。
			*/
			static Result Save2Image(const IVideoFrame* frame, const String& jpegorbmpFileName, int quality/*0-100, higher is better*/);
		protected:
			// The destructor is protected to prevent deletion via the interface.
			// This is so that we allow reference counted classes, where the destructor
			// should never be public, to implement the interface
		protected:
			virtual ~IVideoRender() {

			}
		};

		class RT_API_EXPORT IVideoRenderView {
		public:
			enum ScalingType {
				Scale_Fill = 1, // video frame is scaled to to fill the size of the view. Video aspect ratio is changed if necessary.
				Scale_Aspect_Fit = 2, // video frame is scaled to fit the size of the view by maintaining the aspect ratio (black borders may be displayed).
				Scale_Aspect_Full = 3, // video frame is scaled to fill the size of the view by maintaining the aspect ratio. Some portion of the video frame may be clipped.
			};
		public:
			static IVideoRenderView* CreateVideoRenderView(const int32 id,
				void* window,
				const bool fullscreen, bool isgdi = false);
			static void DestroyVideoRenderView(IVideoRenderView* module);

			virtual IVideoRender*
				CreateVideoRender(const uint32 renderId,
				const uint32 zOrder,
				const float left, const float top,
				const float right, const float bottom) = 0;

			virtual IVideoRender* CreateVideoRender(const uint32 renderId,
				const uint32 zOrder,
				ScalingType scalingType,
				const float left, const float top,
				const float right, const float bottom) = 0;

			virtual int32
				DeleteVideoRender(const uint32 renderId) = 0;

			virtual int32 StartRender(const uint32 renderId) = 0;

			virtual int32 StopRender(const uint32 renderId) = 0;

			/*
			 *   Set the renderer ScalingType
			 */
			virtual bool SetScalingType(const uint32 renderId, ScalingType type) = 0;

			/*
			 *   Get the renderer ScalingType
			 */
			virtual ScalingType	RenderScalingType(const uint32 renderId) = 0;

			virtual int32 UpdateWindowSize() = 0;

			virtual int32 SetBitmap(const void* bitMap,
				const uint8 pictureId,
				const void* colorKey,
				const float left, const float top,
				const float right, const float bottom) = 0;

			virtual int32 SetText(const uint8 textId,
				const char* text,
				const int32 textLength,
				const uint32 textColorRef,
				const uint32 backgroundColorRef,
				const float left, const float top,
				const float right, const float bottom) = 0;

			virtual int32 SetText(const uint8 textId,
				const char* text,
				const int32 textLength,
				const uint32 textColor,
				const float left, const float top,
				const float rigth, const float bottom,
				const int32 format,
				const int32 height,
				const uint32 width,
				const uint32 weight,
				const bool italic,
				const char* faceName,
				const int32 faceNameSize) = 0;
		protected:
			virtual ~IVideoRenderView() {};

		};
		enum {
			ROTATION_0 = 0,
			ROTATION_90 = 90,
			ROTATION_180 = 180,
			ROTATION_270 = 270
		};

		// Represents a YUV420 (a.k.a. I420) video frame.
		class RT_API_EXPORT IVideoFrame {
		public:
			virtual ~IVideoFrame() {}

			virtual bool InitToBlack(int w, int h, size_t pixel_width,
				size_t pixel_height, int64 elapsed_time,
				int64 time_stamp) = 0;
			// Creates a frame from a raw sample with FourCC |format| and size |w| x |h|.
			// |h| can be negative indicating a vertically flipped image.
			// |dw| is destination width; can be less than |w| if cropping is desired.
			// |dh| is destination height, like |dw|, but must be a positive number.
			// Returns whether the function succeeded or failed.
			virtual bool Reset(uint32 fourcc, int w, int h, int dw, int dh, uint8 *sample,
				size_t sample_size, size_t pixel_width,
				size_t pixel_height, int64 elapsed_time, int64 time_stamp,
				int rotation) = 0;

			// Basic accessors.
			virtual size_t GetWidth() const = 0;
			virtual size_t GetHeight() const = 0;
			size_t GetChromaWidth() const { return (GetWidth() + 1) / 2; }
			size_t GetChromaHeight() const { return (GetHeight() + 1) / 2; }
			size_t GetChromaSize() const { return GetUPitch() * GetChromaHeight(); }
			// These can return NULL if the object is not backed by a buffer.
			virtual const uint8 *GetYPlane() const = 0;
			virtual const uint8 *GetUPlane() const = 0;
			virtual const uint8 *GetVPlane() const = 0;
			virtual uint8 *GetYPlane() = 0;
			virtual uint8 *GetUPlane() = 0;
			virtual uint8 *GetVPlane() = 0;

			virtual int32 GetYPitch() const = 0;
			virtual int32 GetUPitch() const = 0;
			virtual int32 GetVPitch() const = 0;

			// Returns the handle of the underlying video frame. This is used when the
			// frame is backed by a texture. The object should be destroyed when it is no
			// longer in use, so the underlying resource can be freed.
			virtual void* GetNativeHandle() const = 0;

			// For retrieving the aspect ratio of each pixel. Usually this is 1x1, but
			// the aspect_ratio_idc parameter of H.264 can specify non-square pixels.
			virtual size_t GetPixelWidth() const = 0;
			virtual size_t GetPixelHeight() const = 0;

			virtual int64 GetElapsedTime() const = 0;
			virtual int64 GetTimeStamp() const = 0;
			virtual void SetElapsedTime(int64 elapsed_time) = 0;
			virtual void SetTimeStamp(int64 time_stamp) = 0;

			// Indicates the rotation angle in degrees.
			virtual int GetRotation() const = 0;

			// Make a shallow copy of the frame. The frame buffer itself is not copied.
			// Both the current and new IVideoFrame will share a single reference-counted
			// frame buffer.
			virtual IVideoFrame *Copy() const = 0;

			// Since IVideoFrame supports shallow copy and the internal frame buffer might
			// be shared, in case IVideoFrame needs exclusive access of the frame buffer,
			// user can call MakeExclusive() to make sure the frame buffer is exclusive
			// accessable to the current object.  This might mean a deep copy of the frame
			// buffer if it is currently shared by other objects.
			virtual bool MakeExclusive() = 0;

			// Writes the frame into the given frame buffer, provided that it is of
			// sufficient size. Returns the frame's actual size, regardless of whether
			// it was written or not (like snprintf). If there is insufficient space,
			// nothing is written.
			virtual size_t CopyToBuffer(uint8 *buffer, size_t size) const = 0;

			// Writes the frame into the given planes, stretched to the given width and
			// height. The parameter "interpolate" controls whether to interpolate or just
			// take the nearest-point. The parameter "crop" controls whether to crop this
			// frame to the aspect ratio of the given dimensions before stretching.
			virtual bool CopyToPlanes(
				uint8* dst_y, uint8* dst_u, uint8* dst_v,
				int32 dst_pitch_y, int32 dst_pitch_u, int32 dst_pitch_v) const = 0;

			// Writes the frame into the target IVideoFrame.
			virtual void CopyToFrame(IVideoFrame* target) const = 0;

			// Converts the I420 data to RGB of a certain type such as ARGB and ABGR.
			// Returns the frame's actual size, regardless of whether it was written or
			// not (like snprintf). Parameters size and stride_rgb are in units of bytes.
			// If there is insufficient space, nothing is written.
			virtual size_t ConvertToRgbBuffer(uint32 to_fourcc, uint8 *buffer,
				size_t size, int stride_rgb) const = 0;

			// Writes the frame into the given planes, stretched to the given width and
			// height. The parameter "interpolate" controls whether to interpolate or just
			// take the nearest-point. The parameter "crop" controls whether to crop this
			// frame to the aspect ratio of the given dimensions before stretching.
			virtual void StretchToPlanes(
				uint8 *y, uint8 *u, uint8 *v, int32 pitchY, int32 pitchU, int32 pitchV,
				size_t width, size_t height, bool interpolate, bool crop) const = 0;

			// Writes the frame into the target IVideoFrame, stretched to the size of that
			// frame. The parameter "interpolate" controls whether to interpolate or just
			// take the nearest-point. The parameter "crop" controls whether to crop this
			// frame to the aspect ratio of the target frame before stretching.
			virtual void StretchToFrame(IVideoFrame *target, bool interpolate,
				bool crop) const = 0;

			// Stretches the frame to the given size, creating a new IVideoFrame object to
			// hold it. The parameter "interpolate" controls whether to interpolate or
			// just take the nearest-point. The parameter "crop" controls whether to crop
			// this frame to the aspect ratio of the given dimensions before stretching.
			virtual IVideoFrame *Stretch(size_t w, size_t h, bool interpolate,
				bool crop) const = 0;

			// Sets the video frame to black.
			virtual bool SetToBlack() = 0;

			// Scale copy
			virtual IVideoFrame *CopyRange(int x, int y, int w, int h) const = 0;

			// Tests if sample is valid.  Returns true if valid.
			static bool Validate(uint32 fourcc, int w, int h, const uint8 *sample,
				size_t sample_size);

			// Size of an I420 image of given dimensions when stored as a frame buffer.
			static size_t SizeOf(size_t w, size_t h) {
				return w * h + ((w + 1) / 2) * ((h + 1) / 2) * 2;
			}
		};

	} // namespace avd
} // namespace tee3
#endif//RTC_VideoRender_H_
