#include <QTime>
#include <QMessageBox>
#include <QMutexLocker>
#include "importer.h"
#include "api/musermanager.h"
#include "console.h"
#include "common.h"

AVReader::AVReader()
: m_stop(false),
  m_count(0)
{
}

void AVReader::init(tee3::avd::IAVImporter* mi)
{
	m_modImporter = mi;
}

bool AVReader::go()
{
	m_stop = false;
	m_count = 0;
	start();
	return true;
}

void AVReader::stop()
{
	m_stop = true;
}

uint32_t AVReader::count() const
{
	QMutexLocker lock(&m_cntMutex);
	return m_count;
}

void AVReader::inc()
{
	QMutexLocker lock(&m_cntMutex);
	m_count++;
}

const quint32 ReadBufLen(16 * 1024);

const char H264StartCode[] = { 0x00, 0x00, 0x00, 0x01 };

H264Reader::H264Reader()
{
}

void H264Reader::run()
{
	QFile f("video_raw.h264");
	CONSOLE_SAFE_LOG_DETAIL(QString("h264 file size %1.").arg(f.size()));
	if (!f.open(QIODevice::ReadOnly)) {
		CONSOLE_SAFE_LOG_CRITICAL("Open h264 file failed.");
		return;
	}

	QByteArray head = QByteArray::fromRawData(H264StartCode, 4);

	QByteArray data = f.read(ReadBufLen);

	int p = 0;

	while (!m_stop) {
		QTime st = QTime::currentTime();

		int pb = data.indexOf(head, p);
		int pe = data.indexOf(head, pb+1);
		if (pe < 0) {
			QByteArray d = f.read(ReadBufLen);
			if (d.isEmpty())
				break;
			data = data.right(data.size() - p);
			data.append(d);
			p = 0;
			continue;
		}

		QString msg = "Importing from %1 to %2 len %3 (%4 . %5).";
		// CONSOLE_SAFE_LOG(msg.arg(pb).arg(pe).arg(pe - pb).arg(+(uint8_t)data[pb]).arg(+(uint8_t)data[pe-1]));

		// step 3.1: import video.
		m_modImporter->video_input264Frame(0, 0, 0, reinterpret_cast<quint8*>(data.data()+pb), pe - pb);
		inc();

		p = pe;

		QTime et = QTime::currentTime();
		int df = st.msecsTo(et);
		if ((df < 30) && ((30 - df) > 10))
			msleep(30-df);
	}

	f.close();
}

PcmReader::PcmReader()
{
}

void PcmReader::run()
{
	QFile f("audio_long16.pcm");
	CONSOLE_SAFE_LOG_DETAIL(QString("pcm file size %1.").arg(f.size()));

	if (!f.open(QIODevice::ReadOnly)) {
		CONSOLE_SAFE_LOG_CRITICAL("Open pcm file failed.");
		return;
	}

	const int samplerate = 16000;
	const int channels = 1;
	const int pcmsize = samplerate / 100 * channels * 2;
	quint8 pcmbufer[pcmsize] = { 0 };

	uint64 initTime = 100;
	while(!m_stop) {
		QTime st = QTime::currentTime();

		int n = f.read(reinterpret_cast<char*>(pcmbufer), pcmsize);
		if (n == 0)
			f.reset();

		// step 3.2: import audio.
		m_modImporter->audio_inputPCMFrame(0, samplerate, channels, pcmbufer, pcmsize);
		inc();

		QTime et = QTime::currentTime();
		int df = st.msecsTo(et);
		if (df < 10)
			msleep(10 - df);
	}
}

using tee3::avd::IAVImporter;

Importer::Importer(const QString& roomId, QWidget* parent)
 : QDialog(parent),
   m_roomId(roomId),
   m_timerId(-1),
   m_modImporter(nullptr)
{
	m_ui.setupUi(this);
	m_consoleInit.reset(new ConsoleInitializer(m_ui.consoleTextEdit));

	m_ui.startButton->setEnabled(false);
	connect(m_ui.startButton, &QPushButton::clicked, this, &Importer::start);

	CONSOLE_SAFE_LOG("Here comes Importer.");
	init();
}

Importer::~Importer()
{
	uninit();
}

bool Importer::init()
{
	// step 1: init engine if necessary.
	//         We can pass this, as the main program has initialized engine already.
	if (!IAVImporter::isEngineWorking()) {
		Result r = IAVImporter::initEngine(this, tee3_avd_server, tee3_app_key, tee3_secret_key);
		if (RT_FAILED(r)) {
			QString msg = "Call init engine failed: %1.";
			CONSOLE_SAFE_LOG_CRITICAL(msg.arg(r));
			return false;
		}
		return true;
	}

	// step 2a: join room now if the engine has been initialized already.
	return join();
}

void Importer::uninit()
{
	m_pcmReader.stop();
	m_264Reader.stop();
	stopImportTimer();

	if (m_modImporter) {
		m_modImporter->release();
		m_modImporter = nullptr;
	}
}

bool Importer::join()
{
	m_modImporter = IAVImporter::obtain(m_roomId.toStdString());
	if (!m_modImporter) {
		CONSOLE_SAFE_LOG_CRITICAL(QString("Can not obtain importer for room: %1").arg(m_roomId));
		return false;
	}

	m_modImporter->setListener(this);

	m_modImporter->enableVideo(true);
	m_modImporter->enableAudio(true);

	m_264Reader.init(m_modImporter);
	m_pcmReader.init(m_modImporter);

	tee3::avd::User u;
	u.userId = "importuser";
	u.userName = "import user";
	m_modImporter->join(u);

	return true;
}

bool Importer::start(bool status)
{
	if (status) {
		// step 3: start importing.
		m_264Reader.go();
		m_pcmReader.go();
		startImportTimer();
		m_ui.startButton->setText(Utf8Str("停止"));
	}
	else {
		m_264Reader.stop();
		m_pcmReader.stop();
		stopImportTimer();
		m_ui.startButton->setText(Utf8Str("开始"));
	}
	return true;
}

bool Importer::startImportTimer()
{
	m_timerId = startTimer(1000);
	m_startTime = QTime::currentTime();
	m_ui.timeLabel->setText("00:00:00");
	return true;
}

void Importer::stopImportTimer()
{
	if (m_timerId != -1) {
		killTimer(m_timerId);
		m_timerId = -1;
	}
}

void Importer::onInitResult(Result result)
{
	if (RT_SUCCEEDED(result)) {
		CONSOLE_SAFE_LOG_IMPORTANT("Init engine success.");

		// step 2b： join room after init engine.
		join();
	}
	else {
		QString msg = "Init engine failed: %1.";
		CONSOLE_SAFE_LOG_CRITICAL(msg.arg(result));
		QMessageBox::warning(this, Utf8Str("出错"), Utf8Str("引擎初始化失败"));
		reject();
	}
}

void Importer::onStatus(Result result)
{
	CONSOLE_SAFE_LOG(QString("onStatus %1.").arg(result));
}

void Importer::onError(Result reason)
{
	CONSOLE_SAFE_LOG_CRITICAL(QString("onError %1.").arg(reason));
}

void Importer::onJoinResult(Result result)
{
	if (RT_SUCCEEDED(result)) {
		CONSOLE_SAFE_LOG_IMPORTANT("Join success.");
		m_ui.startButton->setEnabled(true);
	}
	else {
		CONSOLE_SAFE_LOG_CRITICAL(QString("Join failed %1.").arg(result));
	}
}

void Importer::timerEvent(QTimerEvent *event)
{
	QTime now = QTime::currentTime();
	QTime zero(0, 0, 0);
	QTime elps = zero.addMSecs(m_startTime.msecsTo(now));
	m_ui.timeLabel->setText(elps.toString("hh:mm:ss"));

	QString msg = "Imported a:%1 v:%2.";
	CONSOLE_SAFE_LOG_DETAIL(msg.arg(m_pcmReader.count()).arg(m_264Reader.count()));
}
