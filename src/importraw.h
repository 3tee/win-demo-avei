#pragma once

#include <memory>
#include <QDialog>
#include "ui_importraw.h"
#include "console.h"
#include "importbase.h"
#include "api/audiocapture.h"
#include "api/videocapture.h"

//
// 原始数据导入子程序
//
// 步骤:
//   1: 加入房间;
//   2: 开始导入;
//   3: 发送数据。
//     3.1: 发送视频;
//     3.2: 发送音频。
//   (注: 引擎已在Avei主程序中初始化)
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
