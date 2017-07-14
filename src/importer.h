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
// ���׵����ӳ���
//
// ����:
//   1: ��ʼ������;
//   2: ���뷿��;
//   3: ��ʼ���롣
//     3.1: ������Ƶ;
//     3.2: ������Ƶ��
//   (ע: ��������Avei�������г�ʼ��)
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
