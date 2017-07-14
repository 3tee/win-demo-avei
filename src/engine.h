#pragma once

#include <QDebug>
#include <QDialog>
#include "api/avdengine.h"

class IEngineSink
{
public:
	virtual void onInitResult(Result result) = 0;
	virtual void onGetRoomResult(uint32 callId, Result result) = 0;
	virtual void onScheduleResult(uint32 callId, Result result, const tee3::RoomId& roomId) = 0;

protected:
	virtual ~IEngineSink() {}
};

class Engine : public tee3::avd::IAVDEngine::IListener
{
public:
	static Engine& instance();

public:
	bool init(IEngineSink *sink);
	void uninit();
	bool getRoom(uint32 callId, const tee3::RoomId& roomId);
	bool schedule(uint32 callId, const tee3::avd::RoomInfo& roomInfo);

private:
	virtual void onInitResult(Result result);
	virtual void onUninitResult(Result reason);
	virtual void onGetRoomResult(uint32 callId, Result result, const tee3::avd::RoomInfo& room);
	virtual void onFindRoomsResult(uint32 callId, Result result, const tee3::avd::RoomInfosType& rooms);
	virtual void onScheduleRoomResult(uint32 callId, Result result, const tee3::RoomId& roomId);
	virtual void onCancelRoomResult(uint32 callId, Result result, const tee3::RoomId& roomId);
	virtual void onGetUsersCountResult(uint32 callId, Result result, uint32 usersCount, const tee3::RoomId& roomId);

private:
	Engine();
	~Engine();

private:
	IEngineSink *m_sink;
};
