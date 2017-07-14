#pragma once

#include <memory>
#include <QThread>
#include <QMutex>
#include "ui_importer.h"
#include "api/avimporter.h"

class AVReader : public QThread
{
public:
	AVReader();

	void init(tee3::avd::IAVImporter* mi);
	bool go();
	void stop();

	uint32_t count() const;

protected:
	void inc();

protected:
	tee3::avd::IAVImporter* m_modImporter;
	bool m_stop;
	uint32_t m_count;
	mutable QMutex m_cntMutex;
};

class H264Reader : public AVReader
{
public:
	H264Reader();

private:
	virtual void run();
};

class PcmReader : public AVReader
{
public:
	PcmReader();

private:
	virtual void run();
};

class ConsoleInitializer;

//
// 简易导入子程序
//
// 步骤:
//   1: 初始化引擎;
//   2: 加入房间;
//   3: 开始导入。
//     3.1: 导入视频;
//     3.2: 导入音频。
//   (注: 引擎已在Avei主程序中初始化)
//
class Importer : public QDialog,
				 public tee3::avd::IAVImporter::EngineListener,
				 public tee3::avd::IAVImporter::IListener
{
public:
	Importer(const QString& roomId, QWidget* parent = Q_NULLPTR);
	~Importer();

private:
	bool init();
	void uninit();

	bool start(bool status);
	bool join();

	bool startImportTimer();
	void stopImportTimer();

private:
	// EngineListener
	virtual void onInitResult(Result result);

	// IListener
	virtual void onStatus(Result result);
	virtual void onError(Result reason);
	virtual void onJoinResult(Result result);

	virtual void timerEvent(QTimerEvent *event);

private:
	Ui::ImporterDialog m_ui;
	std::unique_ptr<ConsoleInitializer> m_consoleInit;
	QString m_roomId;
	QTime m_startTime;
	int m_timerId;
	tee3::avd::IAVImporter* m_modImporter;
	H264Reader m_264Reader;
	PcmReader  m_pcmReader;
};
