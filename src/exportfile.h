#pragma once

#include <memory>
#include <QDialog>
#include <QTime>
#include "ui_exportfile.h"
#include "engine.h"
#include "room.h"
#include "audio.h"
#include "video.h"
#include "localrecord.h"
#include "console.h"

//
// �������ļ��ӳ���
//
// ����:
//   1: ���뷿�䣻
//   2: ��ʼ������
//     2.1: ����Recorder����LocalRecordFile::prepare��ʵ�֣�
//     2.2: ���ݹ��������������record�ӿڡ�
//   (ע: ��������Avei�������г�ʼ��)
//
class ExportFile : public QDialog, public IRoomSink, public IVideoSink
{
	Q_OBJECT

public:
	ExportFile(const QString& roomId, QWidget *parent = Q_NULLPTR);
	~ExportFile();

private:
	// IRoomEvents
	virtual void onJoinResult(Result result);

	// IVideoSink
	virtual void onCamera(const tee3::String& username, const tee3::String& devname);
	virtual void onCameraRemoved(int index);

	virtual void timerEvent(QTimerEvent *event);

private:
	bool joinRoom();
	bool start();
	void stop();
	void selectDir();
	void openDir();

private:
	Ui::ExportFileDialog m_ui;
	std::unique_ptr<ConsoleInitializer> m_consoleInit;
	QString m_roomId;
	std::shared_ptr<ExportRoom> m_room;
	std::shared_ptr<Audio> m_audio;
	std::shared_ptr<Video> m_video;
	std::shared_ptr<LocalRecordFile> m_record;
	QTime m_startTime;
	int m_timerId;
};
