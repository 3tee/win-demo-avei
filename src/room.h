#pragma once

#include <memory>
#include <QObject>
#include <api/room.h>

class Audio;
class Video;
class LocalRecord;

class IRoomSink
{
public:
	virtual void onJoinResult(Result result) = 0;

protected:
	virtual ~IRoomSink() {}
};

class Room : public QObject, public tee3::avd::IRoom::IListener
{
	Q_OBJECT

public:
	Room(IRoomSink* sink);
	~Room();

	int init(std::shared_ptr<Audio> audio,
		std::shared_ptr<Video> video);
	int uninit();

	const tee3::RoomId& id() const;

public slots:
	bool join(const tee3::RoomId& roomId);
	bool leave();

protected:
	virtual void initModules();

private:
	virtual void onJoinResult(Result result);
	virtual void onLeaveIndication(Result reason, const tee3::UserId& fromId);
	virtual void onPublicData(const char* data, uint32 len, const tee3::UserId& fromId);
	virtual void onPrivateData(const char* data, uint32 len, const tee3::UserId& fromId);
	virtual void onAppDataNotify(const tee3::String& key, const tee3::String& value);
	virtual void onRoomStatusNotify(tee3::avd::RoomStatus status);
	virtual void onConnectionStatus(tee3::avd::ConnectionStatus status);

protected:
	IRoomSink* m_sink;
	tee3::RoomId m_roomId;
	tee3::avd::IRoom *m_room;
	std::shared_ptr<Audio> m_audio;
	std::shared_ptr<Video> m_video;
};

class ExportRoom : public Room
{
public:
	ExportRoom(IRoomSink* sink);

	int init(std::shared_ptr<Audio> audio,
		std::shared_ptr<Video> video,
		std::shared_ptr<LocalRecord> record);

private:	
	virtual void initModules();

private:
	std::shared_ptr<LocalRecord> m_record;
};
