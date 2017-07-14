#include <memory>
#include "importcoded.h"
#include "api/videorender.h"
#include "api/audiocapture.h"
#include "api/videocapture.h"
#include "common.h"
#include "aacextractor.h"

using std::make_shared;

using tee3::avd::FakeVideoCapturer;
using tee3::avd::IVideoFrame;
using tee3::avd::FakeAudioCapturer;

ImportCoded::ImportCoded(const QString& roomId, QWidget *parent)
 : ImportBase(roomId, parent),
   m_vCapTimerId(-1),
   m_aCapTimerId(-1),
   m_vpos(0)
{
	m_ui.setupUi(this);
	init(m_ui);
	m_ui.startButton->setEnabled(false);
	connect(m_ui.startButton, &QPushButton::clicked, this, &ImportCoded::start);
}

ImportCoded::~ImportCoded()
{
}

bool ImportCoded::joinRoom()
{
	// step 1: join room.
	m_room = make_shared<Room>(this);
	m_audio = make_shared<Audio>(m_room);
	m_video = make_shared<Video>(m_room, this);
	m_room->init(m_audio, m_video);

	m_room->join(m_roomId.toStdString());

	return true;
}

void ImportCoded::handleJoin()
{
	m_ui.startButton->setEnabled(true);
}

void ImportCoded::finish()
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

bool ImportCoded::startImport()
{
	// step 2: open file, create fake capturer, pub video & open audio, start timer

	// video
	// v1. open file
	m_vfile.setFileName("video_raw.h264");
	if (!m_vfile.open(QIODevice::ReadOnly)) {
		CONSOLE_LOG_CRITICAL("Open video_raw.h264 failed.");
		return false;
	}

	// v2. create capturer & publish
	m_vCapturer = FakeVideoCapturer::Create(this, tee3::avd::FOURCC_H264);
	tee3::avd::Camera cam;
	cam.id = "h264";
	cam.name = "h264 video";
	tee3::avd::CameraCapability cap(1920, 1080, 20);
	cam.publishedQualities.setStreamOptions(tee3::avd::stream_main, cap, tee3::avd::codec_h264);
	m_video->pub(cam, m_vCapturer);

	// v3. start timer
	m_vCapTimerId = startTimer(30);

	// audio
	m_afile.setFileName("test.aac");
	if (!m_afile.open(QIODevice::ReadOnly)) {
		CONSOLE_LOG_CRITICAL("Open test.aac failed.");
		return false;
	}
	CONSOLE_LOG("Importing test.aac, video_raw.h264");

	m_aCapturer = FakeAudioCapturer::Instance();
	m_aCapturer->enable(true);
	m_audio->open();

	m_aCapTimerId = startTimer(10);

	return true;
}

void ImportCoded::stopImport()
{
	if (m_vCapTimerId != -1) {
		m_video->unpubAll();
		killTimer(m_vCapTimerId);
		m_vCapTimerId = -1;
	}
	if (m_aCapTimerId != -1) {
		m_audio->close();
		killTimer(m_aCapTimerId);
		m_aCapTimerId = -1;
	}
}

void ImportCoded::start(bool status)
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

void ImportCoded::timerEvent(QTimerEvent *event)
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

const quint32 ReadBufLen(16 * 1024);
const char H264StartCode[] = { 0x00, 0x00, 0x00, 0x01 };

bool ImportCoded::sendVideoFrame()
{
	if (!m_vCapturer || !m_vCapturer->isRunning())
		return false;

	QByteArray head = QByteArray::fromRawData(H264StartCode, 4);

onceagain:
	if (m_vdata.isEmpty()) {
		QByteArray d = m_vfile.read(ReadBufLen);
		if (d.isEmpty() || d.size() < 100) {
			return false;
		}
		m_vdata = d;
	}

	int pb = m_vdata.indexOf(head, m_vpos);
	int pe = m_vdata.indexOf(head, pb + 1);
	if (pe < 0) {
		QByteArray d = m_vfile.read(ReadBufLen);
		if (d.isEmpty()) {
			m_vfile.reset();
			m_vdata.clear();
			m_vpos = 0;
			goto onceagain;
		}
		if (m_vdata.size() == (m_vpos + 1))
			m_vdata = d;
		else
			m_vdata = m_vdata.right(m_vdata.size() - m_vpos).append(d);
		m_vpos = 0;
		goto onceagain;
	}

	m_vpos = pe;

	QString msg = "Importing from %1 to %2 len %3 (%4 . %5).";
	// CONSOLE_LOG_DETAIL(msg.arg(pb).arg(pe).arg(pe - pb).arg(+(uint8_t)m_vdata[pb]).arg(+(uint8_t)m_vdata[pe-1]));

	// step 3.1: import video.
	m_vCapturer->inputEncodedFrame(0, 1920, 1080, reinterpret_cast<quint8*>(m_vdata.data() + pb), pe - pb);
	incVideoCnt();

	return true;
}

bool ImportCoded::sendAudioFrame()
{
	const int samplerate = 44100;
	const int channels = 2;

onceagain:
	char head[7] = { 0 };
	int n = m_afile.read(head, sizeof(head));
	if ((n == 0) || (n != sizeof(head))) {
		CONSOLE_LOG("Read aac head failed.");
		m_afile.reset();
		goto onceagain;
	}

	AACExtractor aac(reinterpret_cast<uint8_t*>(head));

	if (aac.hasCrc()) {
		uint16_t crc;
		n = m_afile.read(reinterpret_cast<char*>(&crc), sizeof(crc));
		if ((n == 0) || (n != sizeof(crc))) {
			CONSOLE_LOG("Read aac crc failed.");
			m_afile.reset();
			goto onceagain;
		}
	}

	size_t len = aac.dataLen();
	uint8_t buf[2048] = { 0 };
	n = m_afile.read(reinterpret_cast<char*>(buf), len);
	if ((n == 0) || (n != len)) {
		CONSOLE_LOG("Read aac data failed.");
		m_afile.reset();
		goto onceagain;
	}

	// step 3.2: import audio.
	m_aCapturer->inputAACFrame(0, samplerate, channels, buf, len, 2048);
	incAudioCnt();

	return true;
}

bool ImportCoded::OnStart()
{
	return true;
}

void ImportCoded::OnStop()
{
}
