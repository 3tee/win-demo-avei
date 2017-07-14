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
		return Utf8Str("���׵���: ��װ�������ʼ�����ӷ������ͨ����Ƶ����Ľӿڣ�����ֻ��Ҫ����Ƶ���뷿����豸ʹ��\n\n��ο��� Importer");
		break;
	case IEE_IMPORT_RAW:
		return Utf8Str("ԭʼ���ݵ���: �������Ա��û��������豸����PCM����Ƶ��YUV����Ƶ���������û����Ա�׼��ʽ���յ��������Ƶ\n\n��ο��� ImportRaw");
		break;
	case IEE_IMPORT_CODED:
		return Utf8Str("�������ݵ���: �������Ա��û��������豸����AAC����Ƶ��H.264����Ƶ���������û����Ա�׼��ʽ���յ��������Ƶ\n\n��ο��� ImportCoded");
		break;
	case IEE_IMPORT_RTSP:
		return Utf8Str("RTSP����: �ͻ�������RTSP������(��IPC)������Ƶ�������ڱ��ؽ��벥�ţ�Ҳ���Խ�����(��������ѽ���)���뷿��\n\n��ο��� ImportRTSP");
		break;
	case IEE_EXPORT_FILE:
		return Utf8Str("�������ļ�: �������Ѿ�����������Ƶ���ڱ���¼�Ƴ�MP4(H.264+AAC)��WebM(VP8+Opus)�ļ�\n\n��ο��� ExportFile");
		break;
	case IEE_EXPORT_CALLBACK:
		return Utf8Str("�������ص�����: �������Ѿ�����������Ƶ��(������)���ڱ���ͨ���ص��ӿڵ�����Ӧ�ò����ɴ���\n\n��ο��� ExportCallback");
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
		statusBar()->showMessage(Utf8Str("�����ʼ���ɹ�"), 2000);
		m_ui.startBtn->setEnabled(true);
	}
	else {
		statusBar()->showMessage(Utf8Str("�����ʼ��ʧ��"));
		QMessageBox::warning(this, Utf8Str("����"), Utf8Str("�����ʼ��ʧ��"));
	}
}

void  Avei::onGetRoomResult(uint32 callId, Result result)
{
	if (RT_SUCCEEDED(result)) {
		statusBar()->showMessage(Utf8Str("�������"), 2000);
		startTheExample();
		m_ui.startBtn->setEnabled(true);
	}
	else {
		QMessageBox::warning(this, Utf8Str("����"), Utf8Str("����Ų����ڣ����ȵ�����ŷ���!"));
	}
	m_ui.schedulePushButton->setEnabled(true);
}

void  Avei::onScheduleResult(uint32 callId, Result result, const tee3::RoomId& roomId)
{
	if (RT_SUCCEEDED(result)) {
		m_ui.roomLineEdit->setText(roomId.c_str());
		statusBar()->showMessage(Utf8Str("���ŷ���ɹ�!"), 2000);
	}
	else {
		QString msg = Utf8Str("��������ʧ��: ");
		msg += "%1";
		QMessageBox::warning(this, Utf8Str("����"), msg.arg(IAVDEngine::getErrorMessage(result).c_str()));
	}
	m_ui.schedulePushButton->setEnabled(true);
	m_ui.startBtn->setEnabled(true);
}

void Avei::scheduleRoom()
{
	QString roomId = m_ui.roomLineEdit->text();
	if (roomId.isEmpty()) {
		statusBar()->showMessage(Utf8Str("�����뷿�����"), 2000);
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
		QMessageBox::warning(this, Utf8Str("����"), Utf8Str("���ŷ���ʧ��"));
		return;
	}

	m_ui.schedulePushButton->setEnabled(false);
	m_ui.startBtn->setEnabled(false);
}

void Avei::start()
{
	QString roomId = m_ui.roomLineEdit->text();
	if (roomId.isEmpty()) {
		statusBar()->showMessage(Utf8Str("�����뷿�����"), 2000);
		return;
	}

	if (!((m_currExample > IEE_NONE) && (m_currExample < IEE_MAX))) {
		statusBar()->showMessage(Utf8Str("��ѡ��һ��ʾ��"), 2000);
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
		statusBar()->showMessage(Utf8Str("��ѡ��һ��ʾ��"), 2000);
		break;
	}
}
