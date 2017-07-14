#pragma once

#include <deque>
#include <memory>
#include <api/mvideo.h>

class Room;

struct VideoInfo
{
	VideoInfo(const tee3::String& un = "", const tee3::UserId& uid = "",
			  const tee3::String& dn = "", const tee3::DeviceId& did = "",
			  tee3::avd::VideoCodec vc = tee3::avd::codec_default)
			  : userName(un), userId(uid), deviceName(dn), deviceId(did), videoCodec(vc)
	{
	}

	tee3::String userName;
	tee3::UserId userId;
	tee3::String deviceName;
	tee3::DeviceId deviceId;
	tee3::avd::VideoCodec videoCodec;
};
typedef std::deque<VideoInfo> VideoInfos;

class IVideoSink
{
public:
	virtual void onCamera(const tee3::String& username, const tee3::String& devname) = 0;
	virtual void onCameraRemoved(int index) = 0;

protected:
	virtual ~IVideoSink() {}
};

class Video : public tee3::avd::IMVideo::IListener
{
public:
	Video(std::weak_ptr<Room> room, IVideoSink* sink);
	~Video();

	bool init(tee3::avd::IRoom* room);
	bool uninit();

	VideoInfo getInfo(int index);

	bool previewLocal();
	bool unpreviewLocal();

	bool pub();
	bool pub(tee3::avd::Camera& fakeDevice, tee3::avd::FakeVideoCapturer* capture);
	bool unpub();
	void unpubAll();

	bool sub(int index);

private:
	virtual void onCameraStatusNotify(tee3::avd::CameraStatus status, const tee3::DeviceId& fromId);
	virtual void onCameraDataNotify(int level, const tee3::String& description, const tee3::DeviceId& fromId);
	virtual void onPublishCameraNotify(const tee3::avd::Camera& camera);
	virtual void onUnpublishCameraNotify(const tee3::avd::Camera& camera);
	virtual void onSubscribeResult(Result result, const tee3::DeviceId& fromId);
	virtual void onUnsubscribeResult(Result result, const tee3::DeviceId& fromId);
	virtual void onPublishLocalResult(Result result, const tee3::DeviceId& fromId);
	virtual void onUnpublishLocalResult(Result result, const tee3::DeviceId& fromId);

private:
	tee3::avd::IVideoRender* m_localRender;
	tee3::avd::IVideoRender* m_remoteRender;
	tee3::avd::IMVideo* m_modVideo;
	std::weak_ptr<Room> m_room;
	tee3::DeviceId m_remoteId;
	VideoInfos m_videoInfos;
	IVideoSink* m_sink;

	typedef std::vector<tee3::DeviceId> DeviceIds;
	DeviceIds m_pubDevs;
};
