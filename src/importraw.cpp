#include "importraw.h"
#include "common.h"
#include "api/combase.h"
#include "api/videocapture.h"
#include "api/videorender.h"
#include "api/audiocapture.h"

using tee3::avd::FakeVideoCapturer;
using tee3::avd::IVideoFrame;
using tee3::avd::FakeAudioCapturer;

using std::make_shared;

ImportRaw::ImportRaw(const QString& roomId, QWidget *parent)
 : ImportBase(roomId, parent),
   m_vCapTimerId(-1),
   m_aCapTimerId(-1),
   m_frameBuf(nullptr),
   m_bufLen(-1),
   m_color(0)
{
	m_ui.setupUi(this);
	init(m_ui);

	m_ui.startButton->setEnabled(false);
	connect(m_ui.startButton, &QPushButton::clicked, this, &ImportRaw::start);
}

ImportRaw::~ImportRaw()
{
}

bool ImportRaw::joinRoom()
{
	m_room = make_shared<Room>(this);
	m_audio = make_shared<Audio>(m_room);
	m_video = make_shared<Video>(m_room, this);
	m_room->init(m_audio, m_video);

	// step 1: join room.
	m_room->join(m_roomId.toStdString());

	return true;
}

void ImportRaw::handleJoin()
{
	m_ui.startButton->setEnabled(true);
}

void ImportRaw::finish()
{
	stopImport();
	if (m_vCapturer) {
		FakeVideoCapturer::Destroy(m_vCapturer);
		m_vCapturer = nullptr;
	}
	if (m_aCapturer) {
		m_aCapturer->enable(false);
		m_aCapturer = nullptr;
	}
}

bool ImportRaw::startImport()
{
	// step 2: open file, create fake capturer, pub video & open audio, start timer

	m_vCapturer = FakeVideoCapturer::Create(this);
	tee3::avd::Camera cam;
	cam.id = "rawVideo";
	tee3::avd::CameraCapability cap(1920, 1080, 20);
	cam.publishedQualities.setStreamOptions(tee3::avd::stream_main, cap, tee3::avd::codec_vp8);
	m_video->pub(cam, m_vCapturer);
	m_vCapTimerId = startTimer(50);

	m_aCapturer = FakeAudioCapturer::Instance();
	m_aCapturer->enable(true);
	m_aCapTimerId = startTimer(10);
	m_afile.setFileName("audio_long16.pcm");
	m_afile.open(QIODevice::ReadOnly);
	CONSOLE_LOG("Importing audio_long16.pcm.");
	m_audio->open();
	return true;
}

void ImportRaw::stopImport()
{
	if (m_vCapTimerId != -1) {
		m_video->unpubAll();
		killTimer(m_vCapTimerId);
		m_vCapTimerId = -1;
		m_color = 0;
	}
	if (m_aCapTimerId != -1) {
		m_audio->close();
		killTimer(m_aCapTimerId);
		m_aCapTimerId = -1;
	}
}

void ImportRaw::start(bool status)
{
	if (status) {
		prepare();
		startImport();
		startTimingTimer(m_ui);
		m_ui.startButton->setText(Utf8Str("Í£Ö¹"));
	}
	else {
		stopImport();
		stopTimingTimer();
		m_ui.startButton->setText(Utf8Str("¿ªÊ¼"));
	}
}

void ImportRaw::timerEvent(QTimerEvent *event)
{
	int id = event->timerId();
	if (id == m_timingTimerId) {
		handleTimingTick(m_ui);
		handleCount();
		return;
	}
	else if (id == m_vCapTimerId) {
		sendVideoFrame();
		return;
	}
	else if (id == m_aCapTimerId) {
		sendAudioFrame();
		return;
	}
}

bool ImportRaw::sendVideoFrame()
{
	if (!m_vCapturer || !m_vCapturer->isRunning())
		return false;

	uint32 format = tee3::avd::FOURCC_I420;
	int w = 1920;
	int h = 1080;
	if (m_bufLen != IVideoFrame::SizeOf(w, h)) {
		m_bufLen = IVideoFrame::SizeOf(w, h);
		delete[] m_frameBuf;
		m_frameBuf = new uint8[m_bufLen];
		FillMemory(m_frameBuf, w * h, 128);
	}

	FillMemory(m_frameBuf + w * h, w * h / 4, m_color+100);
	FillMemory(m_frameBuf + w * h + w * h / 4, w * h / 4, m_color+200);
	++m_color;

	// step 3.1: import video.
	m_vCapturer->inputCapturedFrame(0, format, w, h, m_frameBuf, m_bufLen, 0, false);
	incVideoCnt();

	return true;
}

bool ImportRaw::sendAudioFrame()
{
	const int samplerate = 16000;
	const int channels = 1;
	const int pcmsize = samplerate / 100 * channels * 2;
	uint8_t pcmbufer[pcmsize] = { 0 };

	int n = m_afile.read(reinterpret_cast<char*>(pcmbufer), pcmsize);
	if (n == 0) {
		m_afile.reset();
		CONSOLE_LOG("Audio file end.");
	}

	// step 3.2: import audio.
	m_aCapturer->inputCapturedFrame(0, samplerate, channels, pcmbufer, pcmsize);
	incAudioCnt();

	return true;
}

bool ImportRaw::OnStart()
{
	return true;
}

void ImportRaw::OnStop()
{
}
