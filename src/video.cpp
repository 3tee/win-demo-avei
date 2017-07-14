#include <QDebug>
#include "video.h"
#include "Room.h"
#include "console.h"

using std::weak_ptr;

Video::Video(weak_ptr<Room> room, IVideoSink* sink)
 : m_modVideo(nullptr),
   m_room(room),
   m_sink(sink)
{
}

Video::~Video()
{
#if 0
	if (m_modVideo) {
		m_modVideo->setListener(nullptr);
		m_modVideo = nullptr;
	}
#endif
}

bool Video::init(tee3::avd::IRoom* room)
{
	m_modVideo = tee3::avd::IMVideo::getVideo(room);
	m_modVideo->setListener(this);
	return true;
}

bool Video::previewLocal()
{
	CONSOLE_LOG("Preview local camera.");
	m_modVideo->previewLocalCamera(m_localRender);
	return true;
}

bool Video::unpreviewLocal()
{
	CONSOLE_LOG("Unpreview local camera");
	return RT_SUCCEEDED(m_modVideo->unpreviewLocalCamera());
}

bool Video::uninit()
{
	if (!m_modVideo)
		return false;

	m_modVideo->unpreviewLocalCamera();
	m_modVideo->unpublishLocalCamera();
	m_modVideo->unsubscribe(m_remoteId);
	m_remoteId = "";
	m_modVideo->setListener(nullptr);
	m_modVideo = nullptr;
	return true;
}

VideoInfo Video::getInfo(int index)
{
	if (index < 0 || index >= m_videoInfos.size())
		return VideoInfo();

	return m_videoInfos[index];
}

bool Video::sub(int index)
{
	VideoInfo info = getInfo(index);
	return m_modVideo->subscribe(info.deviceId) == AVD_OK;
}

bool Video::pub()
{
	return m_modVideo->publishLocalCamera() == AVD_OK;
}

bool Video::pub(tee3::avd::Camera& fakeDevice, tee3::avd::FakeVideoCapturer* capture)
{
	return m_modVideo->publishLocalCamera(fakeDevice, capture);
}

bool Video::unpub()
{
	return m_modVideo->unpublishLocalCamera() == AVD_OK;
}

void Video::unpubAll()
{
	DeviceIds::const_iterator it = m_pubDevs.begin();
	for (; it != m_pubDevs.end(); ++it)
		m_modVideo->unpublishLocalCamera(*it);
	m_pubDevs.clear();
}

void Video::onCameraStatusNotify(tee3::avd::CameraStatus status, const tee3::DeviceId& fromId)
{
	QString msg = "onCameraStatusNotify status=%1 id=%2";
	CONSOLE_LOG_DETAIL(msg.arg(status).arg(fromId.c_str()));

	if (status != tee3::avd::ds_none)
		return;

	auto it = std::find_if(m_videoInfos.begin(), m_videoInfos.end(), [&](const VideoInfo& vi) { return vi.deviceId == fromId; });
	if (it != m_videoInfos.end()) {
		CONSOLE_LOG_DETAIL("Found device");
		if (m_sink)
			m_sink->onCameraRemoved(std::distance(m_videoInfos.begin(), it));
		m_videoInfos.erase(it);
	}
}

void Video::onCameraDataNotify(int level, const tee3::String& description, const tee3::DeviceId& fromId)
{
	QString msg = "onCameraDataNotify desc=%1 id=%2";
	CONSOLE_LOG_DETAIL(msg.arg(description.c_str()).arg(fromId.c_str()));
}

void Video::onPublishCameraNotify(const tee3::avd::Camera& camera)
{
	tee3::DeviceId did = camera.id;
	tee3::String dn = camera.name;
	tee3::UserId uid = m_modVideo->getOwnerId(did);
	tee3::String un = m_modVideo->getOwnerName(did);
	tee3::avd::VideoCodec vc = camera.publishedQualities.getStreamCodec(tee3::avd::stream_main);

	QString msg = "camera publish did=%1 dn=%2 uid=%3 un=%4";
	CONSOLE_LOG_DETAIL(msg.arg(did.c_str()).arg(dn.c_str()).arg(uid.c_str()).arg(un.c_str()));

	m_videoInfos.push_back(VideoInfo(un, uid, dn, did, vc));

	VideoInfo vi = m_videoInfos[m_videoInfos.size() - 1];
	if(m_sink) m_sink->onCamera(un, dn);

	if (m_modVideo->isSelfDevice(camera.id))
		return;

#if 0
	m_module->subscribe(camera.id);
	m_remoteId = camera.id;
#endif
}

void Video::onUnpublishCameraNotify(const tee3::avd::Camera& camera)
{
	if (m_modVideo->isSelfDevice(camera.id))
		return;

	tee3::DeviceId did = camera.id;
	tee3::String dn = camera.name;
	tee3::UserId uid = m_modVideo->getOwnerId(did);
	tee3::String un = m_modVideo->getOwnerName(did);
	tee3::avd::VideoCodec vc = camera.publishedQualities.getStreamCodec(tee3::avd::stream_main);

	QString msg = "camera unpublish did=%1 dn=%2 uid=%3 un=%4";
	CONSOLE_LOG_DETAIL(msg.arg(did.c_str()).arg(dn.c_str()).arg(uid.c_str()).arg(un.c_str()));

	auto it = std::find_if(m_videoInfos.begin(), m_videoInfos.end(), [&](const VideoInfo& vi) { return vi.deviceId == did; });
	if (it != m_videoInfos.end()) {
		CONSOLE_LOG_DETAIL("Found device");
		if (m_sink)
			m_sink->onCameraRemoved(std::distance(m_videoInfos.begin(), it));
		m_videoInfos.erase(it);
	}

	//m_modVideo->unsubscribe(camera.id);
}

void Video::onSubscribeResult(Result result, const tee3::DeviceId& fromId)
{
	if (RT_SUCCEEDED(result)) {
		//m_modVideo->attachRender(fromId, m_remoteRender);
	}
}

void Video::onUnsubscribeResult(Result result, const tee3::DeviceId& fromId)
{
	QString msg = "onUnsubscribeResult result=%1 id=%2";
	CONSOLE_LOG_DETAIL(msg.arg(result).arg(fromId.c_str()));
}

void Video::onPublishLocalResult(Result result, const tee3::DeviceId& fromId)
{
	QString msg = "onPublishLocalResult result=%1 id=%2";
	CONSOLE_LOG_DETAIL(msg.arg(result).arg(fromId.c_str()));
	if (RT_SUCCEEDED(result))
		m_pubDevs.push_back(fromId);
}

void Video::onUnpublishLocalResult(Result result, const tee3::DeviceId& fromId)
{
	QString msg = "onUnpublishLocalResult result=%1 id=%2";
	CONSOLE_LOG_DETAIL(msg.arg(result).arg(fromId.c_str()));
}
