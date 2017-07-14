#include <QPlainTextEdit>
#include <QScrollBar>
#include <QMutexLocker>
#include "console.h"

Console* Console::instance()
{
	static Console cnsl;
	return &cnsl;
}

Console::Console()
 : m_wind(nullptr)
{
}

Console::~Console()
{
}

bool Console::init(QPlainTextEdit *w)
{
	if (!w)
		return false;

	m_wind = w;
	w->setReadOnly(true);
	return true;
}

bool Console::uninit()
{
	m_wind = nullptr;
	return true;
}

bool Console::log(const QString& msg, const QString& color)
{
	if (!m_wind)
		return false;

	if (color.isEmpty()) {
		m_wind->appendHtml(msg);
	}
	else {
		QString m = "<font color = \"%1\"> %2</font>";
		m_wind->appendHtml(m.arg(color).arg(msg));
	}
	m_wind->verticalScrollBar()->setSliderPosition(m_wind->verticalScrollBar()->maximum());

	return true;
}

bool Console::logSafe(const QString& msg, const QString& color)
{
	QMutexLocker lock(&m_mutex);
	return log(msg, color);
}

ConsoleInitializer::ConsoleInitializer(QPlainTextEdit *w)
{
	Console::instance()->init(w);
}

ConsoleInitializer::~ConsoleInitializer()
{
	Console::instance()->uninit();
}
