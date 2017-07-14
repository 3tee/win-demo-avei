#pragma once

#include <QFile>
#include "ui_importcoded.h"
#include "importbase.h"

//
// �������ݵ����ӳ���
//
// ����:
//   1: ���뷿��;
//   2: ��ʼ����;
//   3: �������ݡ�
//     3.1: ������Ƶ;
//     3.2: ������Ƶ��
//   (ע: ��������Avei�������г�ʼ��)
//
class ImportCoded : public ImportBase, public tee3::avd::FakeVideoCapturer::Listener
{
public:
	ImportCoded(const QString& roomId, QWidget *parent = Q_NULLPTR);
	~ImportCoded();

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
	Ui::ImportCodedDialog m_ui;
	int m_vCapTimerId;
	int m_aCapTimerId;
	QFile m_vfile;
	QByteArray m_vdata;
	int m_vpos;
	QFile m_afile;
};
