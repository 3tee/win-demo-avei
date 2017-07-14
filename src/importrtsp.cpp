#include "importrtsp.h"
#include "common.h"
#include <memory>
#include "api/audiocapture.h"
#include "api/videocapture.h"
#include "api/rtspclient.h"

using tee3::avd::FakeVideoCapturer;
using tee3::avd::FakeAudioCapturer;

using std::make_shared;

ImportRTSP::ImportRTSP(const QString& roomId, QWidget *parent)
 : ImportBase(roomId, parent),
   m_rtsp(nullptr)
{
	m_ui.setupUi(this);
	init(m_ui);
	m_ui.startButton->setEnabled(false);
	connect(m_ui.startButton, &QPushButton::clicked, this, &ImportRTSP::start);
}

ImportRTSP::~ImportRTSP()
{
	stopImport();
}

void ImportRTSP::start(bool status)
{
	if (status) {
		prepare();
		if (!startImport()) {
			m_ui.startButton->setChecked(false);
			return;
		}
		startTimingTimer(m_ui);
		m_ui.startButton->setText(Utf8Str("Í£Ö¹"));
	}
	else {
		stopImport();
		stopTimingTimer();
		m_ui.startButton->setText(Utf8Str("¿ªÊ¼"));
	}
}

void ImportRTSP::finish()
{
	if (m_ui.startButton->isChecked())
		stopImport();
}

bool ImportRTSP::joinRoom()
{
	m_room = make_shared<Room>(this);
	m_audio = make_shared<Audio>(m_room);
	m_video = make_shared<Video>(m_room, this);
	m_room->init(m_audio, m_video);

	// step 1: join room.
	m_room->join(m_roomId.toStdString());

	return true;
}

void ImportRTSP::handleJoin()
{
	m_ui.startButton->setEnabled(true);
}

void ImportRTSP::timerEvent(QTimerEvent *event)
{
	int id = event->timerId();
	if (id == m_timingTimerId) {
		handleTimingTick(m_ui);
		return;
	}
}

bool ImportRTSP::startImport()
{
	// step 2: create fake capturer, pub video & open audio, start rtsp, set capture & render.

	m_vCapturer = FakeVideoCapturer::Create(this, tee3::avd::FOURCC_H264);
	tee3::avd::Camera cam;
	cam.id = "fakertsp";
	cam.name = "fake rtsp";
	cam.publishedQualities.setStreamOptions(tee3::avd::stream_main, tee3::avd::quality_high, tee3::avd::codec_h264);
	m_video->pub(cam, m_vCapturer);

	m_aCapturer = FakeAudioCapturer::Instance();
	m_aCapturer->enable(true);
	m_audio->open();

	m_rtsp = tee3::avd::RtspClient::create();
	Result r = m_rtsp->start("rtsp://192.168.1.121:554/hikvision://192.168.1.121:8000:0:0", "admin", "Hik12345");
	if (RT_FAILED(r)) {
		CONSOLE_LOG_CRITICAL("RTSP start failed.");
		return false;
	}
	m_rtsp->setVideoCapture(m_vCapturer);
	m_rtsp->setAudioCapture(m_aCapturer);
	m_rtsp->setRender(m_ui.rtspWnd->GetRender());

	return true;
}

bool ImportRTSP::stopImport()
{
	if (!m_rtsp)
		return false;

	m_rtsp->stop();
	m_video->unpubAll();
	return true;
}

bool ImportRTSP::OnStart()
{
	return true;
}

void ImportRTSP::OnStop()
{
}
