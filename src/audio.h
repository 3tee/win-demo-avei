#pragma once

#include <memory>
#include <api/maudio.h>
#include "room.h"

class Audio : public tee3::avd::IMAudio::IListener
{
public:
	Audio(std::weak_ptr<Room> room);
	~Audio();

	bool init(tee3::avd::IRoom* room);
	bool uninit();

	bool open();
	bool close();

private:
	virtual void onMicrophoneStatusNotify(tee3::avd::MicrophoneStatus status, const tee3::UserId& fromUserId);
	virtual void onAudioLevelMonitorNotify(const tee3::avd::AudioInfo& info);
	virtual void onOpenMicrophoneResult(Result result);
	virtual void onCloseMicrophoneResult(Result result);

private:
	tee3::avd::IMAudio *m_modAudio;
	std::weak_ptr<Room> m_room;
};

