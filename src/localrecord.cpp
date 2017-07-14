#include <QDir>
#include <QDateTime>
#include "localrecord.h"
#include "console.h"
#include "room.h"
#include "video.h"

using tee3::String;
using std::shared_ptr;

//
// StreamEater
//
StreamEater::StreamEater()
: m_vc(0),
  m_ac(0)
{
}

StreamEater::~StreamEater()
{
}

void StreamEater::videoStreamOut(const String& recHandle, uint64 timestamp_ns, unsigned int w, unsigned int h, bool isKeyFrame, const uint8 *data, unsigned int len)
{
	QString msg = "v h=%1 t=%2 w=%3 h=%4 k=%5 l=%6";
	CONSOLE_LOG_DETAIL(msg.arg(recHandle.c_str()).arg(timestamp_ns).arg(w).arg(h).arg(isKeyFrame).arg(len));
}

void StreamEater::audioStreamOut(const String& recHandle, uint64 timestamp_ns, int sampleRate, int channels, const uint8 *data, unsigned int len)
{
	if (m_ac++ % 100)
		return;
	QString msg = "a h=%1 t=%2 r=%3 c=%4 l=%5";
	CONSOLE_LOG_DETAIL(msg.arg(recHandle.c_str()).arg(timestamp_ns).arg(sampleRate).arg(channels).arg(len));
}

//
// LocalRecord
//
LocalRecord::LocalRecord(std::weak_ptr<Room> room, std::weak_ptr<Video> video)
: m_modRecord(nullptr),
  m_room(room),
  m_video(video),
  m_recordId(-1)
{
}

LocalRecord::~LocalRecord()
{
	if (m_modRecord)
		stop();
}

bool LocalRecord::init(tee3::avd::IRoom* room)
{
	auto rec = tee3::avd::IMLocalRecord::getRecord(room);
	if (!rec) {
		return false;
	}

	m_modRecord = rec;
	return true;
}

bool LocalRecord::start(RecordFilter f, int index)
{
	// step 2.1
	if (!prepare(f, index))
		return false;

	String uid, did;
	if (f >= RF_AUDIO) {
		shared_ptr<Video> video = m_video.lock();
		if (!video.get())
			return false;

		VideoInfo vi = video->getInfo(index);
		if (vi.userId.empty() && vi.deviceId.empty())
			return false;

		uid = vi.userId;
		did = vi.deviceId;
	}

	// step 2.2
	String id = std::to_string(m_recordId);
	Result r = -1;
	switch (f)
	{
	case RF_ALL_AUDIO:
		CONSOLE_LOG("Record all audio.");
		r = m_modRecord->selectAllAudio4Recorder(id);
		break;
	case RF_ALL_BUT_ME_AUDIO:
		CONSOLE_LOG("Record all but me audio.");
		r = m_modRecord->selectAllAudioWithoutMe4Recorder(id);
		break;
	case RF_AUDIO:
		CONSOLE_LOG(QString("Record one audio uid=%1.").arg(uid.c_str()));
		r = m_modRecord->selectAudio4Recorder(id, uid);
		break;
	case RF_VIDEO:
		CONSOLE_LOG(QString("Record one video did=%1.").arg(did.c_str()));
		r = m_modRecord->selectVideo4Recorder(id, did);
		break;
	default:
		break;
	}

	return RT_SUCCEEDED(r);
}

void LocalRecord::stop()
{
	if (m_recordId < 0)
		return;
	m_modRecord->stopRecorder(std::to_string(m_recordId));
}

//
// LocalRecordStream
//
LocalRecordStream::LocalRecordStream(std::weak_ptr<Room> room, std::weak_ptr<Video> video)
: LocalRecord(room, video)
{
}

bool LocalRecordStream::prepare(RecordFilter f, int index)
{
	String id = std::to_string(++m_recordId);

	Result r = m_modRecord->createRecorder(&m_streamEater, id);
	if (RT_FAILED(r))
		return false;

	CONSOLE_LOG(QString("record id %1.").arg(m_recordId));
	return true;
}

//
// LocalRecordFile
//
LocalRecordFile::LocalRecordFile(std::weak_ptr<Room> room, std::weak_ptr<Video> video)
: LocalRecord(room, video)
{
}

void LocalRecordFile::setDestDir(const QString& dir)
{
	m_destDir = dir;
}

const QString& LocalRecordFile::destDir() const
{
	return m_destDir;
}

QString getRecordName(RecordFilter f)
{
	switch (f)
	{
	case RF_ALL_AUDIO:
		return "allaudio";
		break;
	case RF_ALL_BUT_ME_AUDIO:
		return "allbutmeaudio";
		break;
	case RF_AUDIO:
		return "audio";
		break;
	case RF_VIDEO:
		return "video";
		break;
	default:
		break;
	}
	return "unknown";
}

bool LocalRecordFile::prepare(RecordFilter f, int index)
{
	String id = std::to_string(++m_recordId);

	QString path = QDir::currentPath();
	if (!m_destDir.isEmpty())
		path = m_destDir;
	if (!path.endsWith('/'))
		path += '/';

	shared_ptr<Room> room = m_room.lock();
	if (!room.get())
		return false;

	QString fileName = room->id().c_str();
	fileName += '-' + getRecordName(f);

	CONSOLE_LOG(QString("room id %1, record name %2.").arg(room->id().c_str()).arg(getRecordName(f)));

	QString sfx = ".webm";
	if (f >= RF_AUDIO) {
		shared_ptr<Video> video = m_video.lock();
		if (!video.get())
			return false;
		VideoInfo vi = video->getInfo(index);

		// deduce record file suffix from video codec
		if ((f == RF_VIDEO) && (vi.videoCodec == tee3::avd::codec_h264))
			sfx = ".mp4";

		QString userName = vi.userName.c_str();
		userName.replace(':', '_');
		userName.replace(' ', "");
		QString devName = vi.deviceName.c_str();
		devName.replace(':', '_');
		devName.replace(' ', "");

		fileName += '-' + userName;
		if (f == RF_VIDEO)
			fileName += '-' + devName;
	}

	QDateTime now = QDateTime::currentDateTime();

	fileName += '-' + now.toString("yyyyMMddhhmmss") + sfx;

	QString file = path + fileName;

	CONSOLE_LOG(QString("Record to %1.").arg(file));

	Result r = m_modRecord->createRecorder(file.toStdString(), id);
	if (RT_FAILED(r))
		return false;

	CONSOLE_LOG(QString("record id %1.").arg(m_recordId));
	return true;
}
