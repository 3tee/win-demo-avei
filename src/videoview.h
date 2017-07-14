#ifndef __USE_VideoView_H__
#define __USE_VideoView_H__

#include "qtsolutions/qtwinmigrate/src/qwinhost.h"
#include <qt_windows.h>
#include "api/videorender.h"

using namespace tee3;
using namespace tee3::avd;
class IVideoRenderer;

class CVideoView : public QWinHost
{
public:
	Q_OBJECT
public:
	CVideoView(QWidget* parent);
	virtual ~CVideoView();
	static void RegistRenderWndClass();
public:
	IVideoRender* GetRender();
	void updateUserName(const QString& user_name);
	bool SetScalingType(IVideoRenderView::ScalingType type);
	void stop();
public:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent* event);
	void setBorder(bool hasBorder);
	QPaintEngine * paintEngine() const { return 0; }
	HWND createWindow(HWND parent, HINSTANCE instance);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void focusOutEvent(QFocusEvent *event){}
signals:
	void doubleClick();
	void click();
protected:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	HWND			m_hwnd;
	QString			_id;
	QString			_user_name;
	QWidget*		_parent;
	bool			_hasBorder;
	tee3::avd::IVideoRenderView* m_videoRendererView;
	tee3::avd::IVideoRender*	 m_videoRenderer;
};

#endif //__USE_VideoView_H__
