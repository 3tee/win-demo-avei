#include "Room.h"
#include <api/musermanager.h>
#include "console.h"
#include "audio.h"
#include "video.h"
#include "localrecord.h"

using std::shared_ptr;

Room::Room(IRoomSink* sink)
 : m_sink(sink),
   m_room(nullptr)
{
}

Room::~Room()
{
	uninit();
}

int Room::init(shared_ptr<Audio> audio,
			   shared_ptr<Video> video)
{
	m_audio = audio;
	m_video = video;
	return 0;
}

int Room::uninit()
{
	if (!m_room) {
		m_audio.reset();
		m_video.reset();
		return -1;
	}

	m_video->uninit();
	m_audio->uninit();
	if (m_room->isWorking())
		m_room->leave(AVD_OK);
	m_room->setListener(nullptr);
	m_room->release();
	m_room = nullptr;
	m_roomId = "";
	m_audio.reset();
	m_video.reset();
	return 0;
}

const tee3::RoomId& Room::id() const
{
	return m_room->getRoomId();
}

void Room::initModules()
{
	m_video->init(m_room);
	m_audio->init(m_room);
}

using tee3::RoomId;
using tee3::avd::IRoom;
using tee3::avd::User;

bool Room::join(const tee3::RoomId& roomId)
{
	User user;
	user.userId = "1001";
	user.userName = "demo";

	m_room = IRoom::obtain(roomId, user.userId);
	if (!m_room) {
		return false;
	}
	m_room->setListener(this);
	m_roomId = roomId;

	initModules();

	Result ret = m_room->join(user, "", nullptr);

	return RT_SUCCEEDED(ret);
}

bool Room::leave()
{
	CONSOLE_LOG("Leave room.");
	m_room->leave(AVD_OK);
	return true;
}

void Room::onJoinResult(Result result)
{
	m_sink->onJoinResult(result);
}

void Room::onLeaveIndication(Result reason, const tee3::UserId& fromId)
{
}

void Room::onPublicData(const char* data, uint32 len, const tee3::UserId& fromId)
{
}

void Room::onPrivateData(const char* data, uint32 len, const tee3::UserId& fromId)
{
}

void Room::onAppDataNotify(const tee3::String& key, const tee3::String& value)
{
}

void Room::onRoomStatusNotify(tee3::avd::RoomStatus status)
{
}

void Room::onConnectionStatus(tee3::avd::ConnectionStatus status)
{
}


//
// ExportRoom
//
ExportRoom::ExportRoom(IRoomSink* sink)
: Room(sink)
{
}

int ExportRoom::init(shared_ptr<Audio> audio,
	shared_ptr<Video> video,
	shared_ptr<LocalRecord> record)
{
	Room::init(audio, video);
	m_record = record;
	return 0;
}

void ExportRoom::initModules()
{
	Room::initModules();
	m_record->init(m_room);
}
