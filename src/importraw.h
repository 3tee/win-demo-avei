#pragma once

#include <memory>
#include <QDialog>
#include "ui_importraw.h"
#include "console.h"
#include "importbase.h"
#include "api/audiocapture.h"
#include "api/videocapture.h"

//
// ԭʼ���ݵ����ӳ���
//
// ����:
//   1: ���뷿��;
//   2: ��ʼ����;
//   3: �������ݡ�
//     3.1: ������Ƶ;
//     3.2: ������Ƶ��
//   (ע: ��������Avei�������г�ʼ��)
//
class ImportRaw : public ImportBase, public tee3::avd::FakeVideoCapturer::Listener
{
	Q_OBJECT

public:
	ImportRaw(const QString& roomId, QWidget *parent = Q_NULLPTR);
	~ImportRaw();

private:
	void start(bool status);

	bool startImport();
	void stopImport();

	bool sendVideoFrame();
	bool sendAudioFrame();

private:
	virtual bool joinRoom();
	virtual void handleJoin();
	virtual void finish();

	virtual void timerEvent(QTimerEvent *event);

	// FakeVideoCapturer::Listener
	virtual bool OnStart();
	virtual void OnStop();

private:
	Ui::ImportRawDialog m_ui;
	int m_vCapTimerId;
	int m_aCapTimerId;
	quint8* m_frameBuf;
	int m_bufLen;
	quint8 m_color;
	QFile m_afile;
};
