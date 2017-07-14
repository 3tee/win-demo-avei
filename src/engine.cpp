#include <QMessageBox>
#include "engine.h"
#include "common.h"
#include "console.h"
#include "api/avdengine.h"

using tee3::avd::IAVDEngine;

Engine& Engine::instance()
{
	static Engine s_engine;
	return s_engine;
}

Engine::Engine()
 : m_sink(nullptr)
{
}

Engine::~Engine()
{
	IAVDEngine::Instance()->uninit();
}

bool Engine::init(IEngineSink *sink)
{
	Result rv = IAVDEngine::Instance()->init(
		this, tee3_avd_server, tee3_app_key, tee3_secret_key);
	if (RT_FAILED(rv)) {
		QString msg = "Init engine failed (%1).";
		CONSOLE_LOG_CRITICAL(msg.arg(rv));
		return false;
	}

	m_sink = sink;

	return true;
}

void Engine::uninit()
{
	IAVDEngine::Instance()->uninit();
}

bool Engine::getRoom(uint32 callId, const tee3::RoomId& roomId)
{
	return RT_SUCCEEDED(IAVDEngine::Instance()->getRoomByRoomId(callId, roomId));
}

bool Engine::schedule(uint32 callId, const tee3::avd::RoomInfo& roomInfo)
{
	return RT_SUCCEEDED(IAVDEngine::Instance()->scheduleRoom(callId, roomInfo));
}

void Engine::onInitResult(Result result)
{
	if(m_sink) m_sink->onInitResult(result);
}

void Engine::onUninitResult(Result reason)
{
	QString msg = "Uninit reason (%1).";
	CONSOLE_LOG(msg.arg(reason));
}

void Engine::onGetRoomResult(uint32 callId, Result result, const tee3::avd::RoomInfo& room)
{
	if (m_sink) m_sink->onGetRoomResult(callId, result);
}

void Engine::onFindRoomsResult(uint32 callId, Result result, const tee3::avd::RoomInfosType& rooms)
{
}

void Engine::onScheduleRoomResult(uint32 callId, Result result, const tee3::RoomId& roomId)
{
#if 0
	QString msg = "Create room callid=%1 result=%2 roomid=%3";
	CONSOLE_LOG(msg.arg(callId).arg(result).arg(roomId.c_str()));
#endif

#if 0
	if (callId != m_callId)
		return;
#endif

	if (m_sink) m_sink->onScheduleResult(callId, result, roomId);

#if 0
	if (RT_FAILED(result)) {
		CONSOLE_LOG(QString("Create room result %1").arg(result));
		return;
	}
#endif

	//m_roomInfo.roomId = roomId;

	//ui.usr1_join_btn->setEnabled(true);
	//ui.usr2_join_btn->setEnabled(true);
}

void Engine::onCancelRoomResult(uint32 callId, Result result, const tee3::RoomId& roomId)
{
}

void Engine::onGetUsersCountResult(uint32 callId, Result result, uint32 usersCount, const tee3::RoomId& roomId)
{
}
