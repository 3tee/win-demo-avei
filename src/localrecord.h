#pragma once

#include <memory>
#include <QObject>
#include "api/mlocalrecord.h"

class StreamEater : public tee3::avd::IMLocalRecord::StreamOut
{
public:
	StreamEater();
	virtual ~StreamEater();

private:
	virtual void videoStreamOut(const tee3::String& recHandle, uint64 timestamp_ns, unsigned int w, unsigned int h, bool isKeyFrame, const uint8 *data, unsigned int len);
	virtual void audioStreamOut(const tee3::String& recHandle, uint64 timestamp_ns, int sampleRate, int channels, const uint8 *data, unsigned int len);

private:
	uint32 m_vc;
	uint32 m_ac;
};

class Room;
class Video;

enum RecordFilter
{
	RF_ALL_AUDIO,
	RF_ALL_BUT_ME_AUDIO,
	RF_AUDIO,
	RF_VIDEO
};

class LocalRecord : public QObject
{
	Q_OBJECT

public:
	LocalRecord(std::weak_ptr<Room> room, std::weak_ptr<Video> video);
	~LocalRecord();

	bool init(tee3::avd::IRoom* room);

	bool start(RecordFilter f, int index = -1);

	void stop();

	virtual bool prepare(RecordFilter f, int index) = 0;

protected:
	tee3::avd::IMLocalRecord* m_modRecord;
	std::weak_ptr<Room> m_room;
	std::weak_ptr<Video> m_video;
	int m_recordId;
};

class LocalRecordStream : public LocalRecord
{
public:
	LocalRecordStream(std::weak_ptr<Room> room, std::weak_ptr<Video> video);

private:
	virtual bool prepare(RecordFilter f, int index);

private:
	StreamEater m_streamEater;
};

class LocalRecordFile : public LocalRecord
{
public:
	LocalRecordFile(std::weak_ptr<Room> room, std::weak_ptr<Video> video);

	void setDestDir(const QString& dir);
	const QString& destDir() const;

private:
	virtual bool prepare(RecordFilter f, int index);

private:
	QString m_destDir;
};
