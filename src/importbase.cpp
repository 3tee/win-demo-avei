#include <QMessageBox>
#include "importbase.h"
#include "common.h"
#include "api/audiocapture.h"
#include "api/videocapture.h"

using tee3::avd::FakeVideoCapturer;
using tee3::avd::FakeAudioCapturer;

ImportBase::ImportBase(const QString& roomId, QWidget *parent)
: QDialog(parent),
  m_roomId(roomId),
  m_timingTimerId(-1),
  m_aCount(0),
  m_vCount(0),
  m_vCapturer(nullptr),
  m_aCapturer(nullptr)
{
	connect(this, &QDialog::finished, this, &ImportBase::finished);
}

ImportBase::~ImportBase()
{
	if (m_room.get()) {
		//m_room->leave();
		//m_room.reset();
	}
}

void ImportBase::prepare()
{
	m_aCount = 0;
	m_vCount = 0;
}

void ImportBase::incAudioCnt()
{
	++m_aCount;
}

void ImportBase::incVideoCnt()
{
	++m_vCount;
}

#if 0
void ImportBase::onInitResult(Result result)
{
	if (RT_SUCCEEDED(result)) {
		QString msg = "Init engine success.";
		CONSOLE_LOG(msg.arg(result));
		joinRoom();
	}
	else {
		QString msg = "Init engine failed (%1).";
		CONSOLE_LOG_CRITICAL(msg.arg(result));
		QMessageBox::warning(this, Utf8Str("出错"), Utf8Str("引擎初始化失败"));
		reject();
	}
}
#endif

void ImportBase::onJoinResult(Result result)
{
	if (RT_FAILED(result)) {
		QString msg = "Join failed (%1).";
		CONSOLE_LOG_CRITICAL(msg.arg(result));
		return;
	}

	CONSOLE_LOG_IMPORTANT("Join success.");

	handleJoin();
}

void ImportBase::onCamera(const tee3::String& username, const tee3::String& devname)
{
}

void ImportBase::onCameraRemoved(int index)
{
}

void ImportBase::finished()
{
	finish();
	if (m_vCapturer) {
		FakeVideoCapturer::Destroy(m_vCapturer);
		m_vCapturer = nullptr;
	}
	if (m_aCapturer) {
		m_aCapturer->enable(false);
		m_aCapturer = nullptr;
	}
}
