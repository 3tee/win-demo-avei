#include <QMessageBox>
#include <QDialog>
#include <QSizePolicy>
#include "Avei.h"
#include "common.h"
#include "exportfile.h"
#include "exportcallback.h"
#include "importer.h"
#include "importraw.h"
#include "importcoded.h"
#include "importrtsp.h"

Avei::Avei(QWidget *parent)
 : QMainWindow(parent),
   m_currExample(IEE_NONE),
   m_callId(0)
{
	m_ui.setupUi(this);

	m_ui.roomLineEdit->setFocus();

	connect(m_ui.importListWidget, &QListWidget::itemPressed, this, &Avei::handleImportChanged);
	connect(m_ui.importListWidget, &QListWidget::itemSelectionChanged, this, &Avei::handleImportChanged);
	connect(m_ui.exportListWidget, &QListWidget::itemPressed, this, &Avei::handleExportChanged);
	connect(m_ui.exportListWidget, &QListWidget::itemSelectionChanged, this, &Avei::handleExportChanged);

	connect(m_ui.schedulePushButton, &QPushButton::clicked, this, &Avei::scheduleRoom);
	connect(m_ui.startBtn, &QPushButton::clicked, this, &Avei::start);

	m_ui.startBtn->setEnabled(false);
	Engine::instance().init(this);
}

Avei::~Avei()
{
	Engine::instance().uninit();
}

void Avei::handleImportChanged()
{
	if (m_ui.importListWidget->selectedItems().empty())
		return;
	m_ui.exportListWidget->clearSelection();
	setCurrentExample(IEE_IMPORT_MIN, m_ui.importListWidget->currentRow());
}

void Avei::handleExportChanged()
{
	if (m_ui.exportListWidget->selectedItems().empty())
		return;
	m_ui.importListWidget->clearSelection();
	setCurrentExample(IEE_EXPORT_MIN, m_ui.exportListWidget->currentRow());
}

QString exampleDesc(ImportExportExample e)
{
	switch (e)
	{
	case IEE_IMPORT_SIMPLE:
		return Utf8Str("简易导入: 封装了引擎初始化，加房间和普通音视频导入的接口，方便只需要音视频导入房间的设备使用\n\n请参考类 Importer");
		break;
	case IEE_IMPORT_RAW:
		return Utf8Str("原始数据导入: 房间中以本用户的虚拟设备导入PCM的音频及YUV的视频流，其他用户可以标准方式接收导入的音视频\n\n请参考类 ImportRaw");
		break;
	case IEE_IMPORT_CODED:
		return Utf8Str("编码数据导入: 房间中以本用户的虚拟设备导入AAC的音频及H.264的视频流，其他用户可以标准方式接收导入的音视频\n\n请参考类 ImportCoded");
		break;
	case IEE_IMPORT_RTSP:
		return Utf8Str("RTSP导入: 客户端连接RTSP服务器(或IPC)拉音视频流，可在本地解码播放，也可以将数据(编码过或已解码)导入房间\n\n请参考类 ImportRTSP");
		break;
	case IEE_EXPORT_FILE:
		return Utf8Str("导出到文件: 房间中已经发布的音视频可在本地录制成MP4(H.264+AAC)或WebM(VP8+Opus)文件\n\n请参考类 ExportFile");
		break;
	case IEE_EXPORT_CALLBACK:
		return Utf8Str("导出到回调函数: 房间中已经发布的音视频流(编码后的)可在本地通过回调接口导出给应用层自由处理\n\n请参考类 ExportCallback");
		break;
	default:
		break;
	}
	return "NONE";
}

void Avei::setCurrentExample(ImportExportExample base, int index)
{
	if (base != IEE_IMPORT_MIN && base != IEE_EXPORT_MIN)
		return;
	m_currExample = static_cast<ImportExportExample>(base + index);
	m_ui.descLabel->setText(exampleDesc(m_currExample));
}

void Avei::onInitResult(Result result)
{
	if (RT_SUCCEEDED(result)) {
		statusBar()->showMessage(Utf8Str("引擎初始化成功"), 2000);
		m_ui.startBtn->setEnabled(true);
	}
	else {
		statusBar()->showMessage(Utf8Str("引擎初始化失败"));
		QMessageBox::warning(this, Utf8Str("出错"), Utf8Str("引擎初始化失败"));
	}
}

void  Avei::onGetRoomResult(uint32 callId, Result result)
{
	if (RT_SUCCEEDED(result)) {
		statusBar()->showMessage(Utf8Str("房间存在"), 2000);
		startTheExample();
		m_ui.startBtn->setEnabled(true);
	}
	else {
		QMessageBox::warning(this, Utf8Str("提醒"), Utf8Str("房间号不存在，请先点击安排房间!"));
	}
	m_ui.schedulePushButton->setEnabled(true);
}

void  Avei::onScheduleResult(uint32 callId, Result result, const tee3::RoomId& roomId)
{
	if (RT_SUCCEEDED(result)) {
		m_ui.roomLineEdit->setText(roomId.c_str());
		statusBar()->showMessage(Utf8Str("安排房间成功!"), 2000);
	}
	else {
		QString msg = Utf8Str("创建房间失败: ");
		msg += "%1";
		QMessageBox::warning(this, Utf8Str("提醒"), msg.arg(IAVDEngine::getErrorMessage(result).c_str()));
	}
	m_ui.schedulePushButton->setEnabled(true);
	m_ui.startBtn->setEnabled(true);
}

void Avei::scheduleRoom()
{
	QString roomId = m_ui.roomLineEdit->text();
	if (roomId.isEmpty()) {
		statusBar()->showMessage(Utf8Str("请输入房间号先"), 2000);
		return;
	}

	++m_callId;

	RoomInfo ri;
	ri.roomName = "avei";
	ri.appRoomId = "aveiapp";
	ri.maxAttendee = 10;
	ri.maxAudio = 10;
	ri.maxVideo = 20;
	ri.setRoomMode(tee3::avd::rm_mcu);

	bool r = Engine::instance().schedule(m_callId, ri);
	if (!r) {
		QMessageBox::warning(this, Utf8Str("出错"), Utf8Str("安排房间失败"));
		return;
	}

	m_ui.schedulePushButton->setEnabled(false);
	m_ui.startBtn->setEnabled(false);
}

void Avei::start()
{
	QString roomId = m_ui.roomLineEdit->text();
	if (roomId.isEmpty()) {
		statusBar()->showMessage(Utf8Str("请输入房间号先"), 2000);
		return;
	}

	if (!((m_currExample > IEE_NONE) && (m_currExample < IEE_MAX))) {
		statusBar()->showMessage(Utf8Str("请选择一个示例"), 2000);
		return;
	}

	++m_callId;
	Engine::instance().getRoom(m_callId, roomId.toStdString());

	m_ui.schedulePushButton->setEnabled(false);
	m_ui.startBtn->setEnabled(false);
}

void Avei::startTheExample()
{
	QString roomId = m_ui.roomLineEdit->text();

	switch (m_currExample)
	{
	case IEE_IMPORT_SIMPLE:
	{
		Importer impt(roomId);
		impt.exec();
		break;
	}
	case IEE_IMPORT_RAW:
	{
		ImportRaw imrw(roomId);
		imrw.exec();
		break;
	}
	case IEE_IMPORT_CODED:
	{
		ImportCoded imcd(roomId);
		imcd.exec();
		break;
	}
	case IEE_IMPORT_RTSP:
	{
		ImportRTSP imrs(roomId);
		imrs.exec();
		break;
	}
	case IEE_EXPORT_FILE:
	{
		ExportFile exfl(roomId);
		exfl.exec();
		break;
	}
	case IEE_EXPORT_CALLBACK:
	{
		ExportCallback excb(roomId);
		excb.exec();
		break;
	}
	default:
		statusBar()->showMessage(Utf8Str("请选择一个示例"), 2000);
		break;
	}
}
