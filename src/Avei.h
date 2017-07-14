#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_avei.h"
#include "engine.h"

enum ImportExportExample
{
	IEE_NONE,
	IEE_IMPORT_MIN,
	IEE_IMPORT_SIMPLE = IEE_IMPORT_MIN,
	IEE_IMPORT_RAW,
	IEE_IMPORT_CODED,
	IEE_IMPORT_RTSP,
	IEE_EXPORT_MIN,
	IEE_EXPORT_FILE = IEE_EXPORT_MIN,
	IEE_EXPORT_CALLBACK,
	IEE_MAX
};

//
// ����/����������
//
// ����: �ṩ����������ת���ӳ���
//
// ����:
//   1. ��ʼ��SDK����;  
//   2. �����ӳ���ǰ��鷿���Ƿ����;
//   3. ��Ҫʱ���ŷ��䡣
//
class Avei : public QMainWindow, public IEngineSink
{
    Q_OBJECT

public:
    Avei(QWidget *parent = Q_NULLPTR);
	~Avei();

private:
	void handleExportChanged();
	void handleImportChanged();

	void setCurrentExample(ImportExportExample base, int index);

	void scheduleRoom();
	void start();
	void startTheExample();

private:
	// IEngineSink
	virtual void onInitResult(Result result);
	virtual void onGetRoomResult(uint32 callId, Result result);
	virtual void onScheduleResult(uint32 callId, Result result, const tee3::RoomId& roomId);

private:
	Ui::AveiClass m_ui;
	ImportExportExample m_currExample;
	uint32 m_callId;
};
