#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QDesktopServices>
#include "exportfile.h"
#include "console.h"
#include "common.h"

using std::make_shared;

ExportFile::ExportFile(const QString& roomId, QWidget *parent)
: QDialog(parent),
  m_roomId(roomId),
  m_timerId(-1)
{
	m_ui.setupUi(this);

	m_consoleInit.reset(new ConsoleInitializer(m_ui.consoleTextEdit));

	// step 1
	joinRoom();

	m_ui.startButton->setEnabled(false);
	m_ui.stopButton->setEnabled(false);
	connect(m_ui.startButton, &QPushButton::clicked, this, &ExportFile::start);
	connect(m_ui.stopButton, &QPushButton::clicked, this, &ExportFile::stop);
	connect(m_ui.selectDirButton, &QPushButton::clicked, this, &ExportFile::selectDir);
	connect(m_ui.openDirButton, &QPushButton::clicked, this, &ExportFile::openDir);

	QButtonGroup *bg = m_ui.filterButtonGroup;
	bg->setId(m_ui.allAudioRadioButton, RF_ALL_AUDIO);
	bg->setId(m_ui.allButMeAudioRadioButton, RF_ALL_BUT_ME_AUDIO);
	bg->setId(m_ui.selectAudioRadioButton, RF_AUDIO);
	bg->setId(m_ui.selectVideoRadioButton, RF_VIDEO);
	m_ui.allAudioRadioButton->setChecked(true);
}

ExportFile::~ExportFile()
{
}

void ExportFile::onJoinResult(Result result)
{
	if (RT_FAILED(result)) {
		QString msg = "ExportFile join failed (%1).";
		CONSOLE_LOG_CRITICAL(msg.arg(result));
		return;
	}

	CONSOLE_LOG_IMPORTANT("Join success.");
	m_ui.startButton->setEnabled(true);
}

void ExportFile::onCamera(const tee3::String& username, const tee3::String& devname)
{
	QString devItem = "%1 : %2";
	m_ui.videoList->addItem(devItem.arg(username.c_str()).arg(devname.c_str()));
}

void ExportFile::onCameraRemoved(int index)
{
	m_ui.videoList->takeItem(index);
}

void ExportFile::timerEvent(QTimerEvent *event)
{
	QTime now = QTime::currentTime();
	QTime zero(0, 0, 0);
	QTime elps = zero.addMSecs(m_startTime.msecsTo(now));
	m_ui.timeLabel->setText(elps.toString("hh:mm:ss"));
}

bool ExportFile::joinRoom()
{
	m_room = make_shared<ExportRoom>(this);
	m_audio = make_shared<Audio>(m_room);
	m_video = make_shared<Video>(m_room, this);
	m_record = std::make_shared<LocalRecordFile>(m_room, m_video);
	m_room->init(m_audio, m_video, m_record);

	m_room->join(m_roomId.toStdString());

	return true;
}

bool ExportFile::start()
{
	if (!m_record.get())
		return false;

	CONSOLE_LOG("Start recording.");

	int chid = m_ui.filterButtonGroup->checkedId();
	RT_ASSERTE(chid != -1);

	RecordFilter rf = static_cast<RecordFilter>(chid);

	int index = -1;
	if (rf >= RF_AUDIO) {
		QList<QListWidgetItem*> items = m_ui.videoList->selectedItems();
		if (items.empty()) {
			CONSOLE_LOG_CRITICAL("Please select one video.");
			return false;
		}

		index = m_ui.videoList->row(items.first());
	}

	// step 2
	bool r = m_record->start(rf, index);
	if (!r) {
		CONSOLE_LOG_CRITICAL("Start record failed");
		return false;
	}

	m_video->sub(index);
	m_ui.startButton->setEnabled(false);
	m_ui.stopButton->setEnabled(true);

	m_startTime = QTime::currentTime();
	m_timerId = startTimer(1000);
	m_ui.timeLabel->setText("00:00:00");

	return true;
}

void ExportFile::stop()
{
	CONSOLE_LOG("Stop Recording.");
	killTimer(m_timerId);
	m_record->stop();
	m_ui.startButton->setEnabled(true);
	m_ui.stopButton->setEnabled(false);
}

void ExportFile::selectDir()
{
	QString currDir = QDir::currentPath();
	QString destDir = QFileDialog::getExistingDirectory(this, Utf8Str("选择目标文件夹"), currDir);
	CONSOLE_LOG(QString("Selected record dir: '%1'").arg(destDir));
	if (destDir.isEmpty())
		return;
	m_record->setDestDir(destDir);
}

void ExportFile::openDir()
{
	QString dir = m_record->destDir();
	if (dir.isEmpty())
		dir = QDir::currentPath();
	QDesktopServices::openUrl(QUrl(dir));
}
