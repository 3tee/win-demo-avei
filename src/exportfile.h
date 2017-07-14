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
// 导出到文件子程序
//
// 步骤:
//   1: 加入房间；
//   2: 开始导出。
//     2.1: 创建Recorder，在LocalRecordFile::prepare中实现；
//     2.2: 根据过滤条件调用相关record接口。
//   (注: 引擎已在Avei主程序中初始化)
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
