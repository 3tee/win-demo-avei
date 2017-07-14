#pragma once

#include "ui_importrtsp.h"
#include "importbase.h"

NAMESPACE_TEE3_AVD_BEGIN
class RtspClient;
NAMESPACE_TEE3_AVD_END

//
// RTSP�����ӳ���
//
// ����:
//   1: ���뷿��;
//   2: ����RTSP�����õ����ӿڡ�
//   (ע: ��������Avei�������г�ʼ��)
//
class ImportRTSP : public ImportBase, public tee3::avd::FakeVideoCapturer::Listener
{
public:
	ImportRTSP(const QString& roomId, QWidget *parent = Q_NULLPTR);
	~ImportRTSP();

private:
	void start(bool status);

private:
	virtual bool joinRoom();
	virtual void handleJoin();
	virtual void finish();

	virtual void timerEvent(QTimerEvent *event);

	// FakeVideoCapturer::Listener
	virtual bool OnStart();
	virtual void OnStop();

private:
	bool startImport();
	bool stopImport();

private:
	Ui::ImportRtspDialog m_ui;
	tee3::avd::RtspClient* m_rtsp;
};
