#pragma once

#include <QFile>
#include "ui_importcoded.h"
#include "importbase.h"

//
// 编码数据导入子程序
//
// 步骤:
//   1: 加入房间;
//   2: 开始导入;
//   3: 发送数据。
//     3.1: 发送视频;
//     3.2: 发送音频。
//   (注: 引擎已在Avei主程序中初始化)
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
