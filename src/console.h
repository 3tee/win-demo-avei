#pragma once

#include <QMutex>
#include <QString>
#include <api/combase.h>

class QPlainTextEdit;

class Console
{
public:
	static Console* instance();

public:
	bool init(QPlainTextEdit *w);
	bool uninit();

	virtual bool log(const QString& msg, const QString& color = "");
	virtual bool logSafe(const QString& msg, const QString& color = "");

protected:
	Console();
	virtual ~Console();

private:
	QPlainTextEdit *m_wind;
	QMutex m_mutex;
};

class ConsoleInitializer
{
public:
	ConsoleInitializer(QPlainTextEdit *w);
	~ConsoleInitializer();
};

#define CONSOLE_LOG(...) Console::instance()->log(__VA_ARGS__)
#define CONSOLE_LOG_DETAIL(msg) Console::instance()->log(msg, "grey")
#define CONSOLE_LOG_IMPORTANT(msg) Console::instance()->log(msg, "magenta")
#define CONSOLE_LOG_CRITICAL(msg) Console::instance()->log(msg, "red")

#define CONSOLE_SAFE_LOG(...) Console::instance()->logSafe(__VA_ARGS__)
#define CONSOLE_SAFE_LOG_DETAIL(msg) Console::instance()->logSafe(msg, "grey")
#define CONSOLE_SAFE_LOG_IMPORTANT(msg) Console::instance()->logSafe(msg, "magenta")
#define CONSOLE_SAFE_LOG_CRITICAL(msg) Console::instance()->logSafe(msg, "red")
