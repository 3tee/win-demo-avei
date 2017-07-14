//#include "log2ui.h"
#include <QtGui/qpainter.h>
#include <QtGui/qbitmap.h>
#include "tchar.h"
#include "videoview.h"

static bool g_isRegistRenderWndClasss = false;
const LPCTSTR VideoRenderWndClass = TEXT("VideoRenderClass");
///
CVideoView::CVideoView(QWidget* parent)
: QWinHost(parent), m_hwnd(NULL), _parent(parent),
	_hasBorder(true), m_videoRendererView(NULL), m_videoRenderer(NULL) {
	this->setAttribute(Qt::WA_PaintOnScreen, true);
	this->setFocusPolicy(Qt::StrongFocus);
	this->show();
}
CVideoView::~CVideoView() {
	if (m_videoRendererView) {
		m_videoRendererView->StopRender(0);
		m_videoRendererView->DeleteVideoRender(0);
		IVideoRenderView::DestroyVideoRenderView(m_videoRendererView);
		m_videoRendererView = NULL;
	}
}
void CVideoView::updateUserName(const QString& user_name) {
	_user_name = user_name;
}

void CVideoView::paintEvent(QPaintEvent *event) {
	QPainter dc(this);
	dc.fillRect(geometry(), Qt::black);

	//QFont font(dc.font());
	//font.setPixelSize(15);
	//font.setFamily(QString("Microsoft YaHei"));
	//QPen pen(QColor(255,0,0));
	//dc.setPen(pen);
	//dc.setFont(font);
	//dc.drawText(10, 0, rect().width()-10, 30, Qt::AlignVCenter || Qt::AlignRight, _user_name);
	//QRect borderRect(0, 0, rect().width() - 1, rect().height() - 1);
}

IVideoRender* CVideoView::GetRender() {
	return m_videoRenderer;
}
bool CVideoView::SetScalingType(IVideoRenderView::ScalingType type) {
	return m_videoRendererView->SetScalingType(0, type);
}
void CVideoView::stop()
{
	m_videoRendererView->StopRender(0);
}
HWND CVideoView::createWindow(HWND parent, HINSTANCE instance) {
	RegistRenderWndClass();
	// Create the main window.
	HWND hwnd = CreateWindowEx(
		0, // no extended styles
		VideoRenderWndClass, // class name
		TEXT("VideoRenderTest Window"), // window name
		WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP, // overlapped window
		CW_USEDEFAULT, // horizontal position
		0, // vertical position
		400, // width
		300, // height
		(HWND)parent, // no parent or owner window
		(HMENU)NULL, // class menu used
		instance, // instance handle
		NULL); // no window creation data
	m_videoRendererView = IVideoRenderView::CreateVideoRenderView(1, hwnd, false, false);
	m_videoRenderer = m_videoRendererView->CreateVideoRender(0, 0, IVideoRenderView::Scale_Aspect_Fit, 0.0f, 0.0f, 1.0f, 1.0f);
	m_videoRendererView->StartRender(0);
	m_hwnd = hwnd;
	return m_hwnd;
}
#if QT_VERSION >= 0x050000
#define QT_WA(unicode, ansi) unicode
#endif
LRESULT CALLBACK CVideoView::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	QWidget *widget = QWidget::find((WId)GetParent(hWnd));
	CVideoView *window = qobject_cast<CVideoView*>(widget);

	if (window) switch (message) {
	case WM_SETFOCUS:
		//window->message("SetFocus for Win32 window!", 1000);
		window->focusInEvent(NULL);
		break;
	case WM_KILLFOCUS:
		window->focusOutEvent(NULL);
		//window->message("KillFocus for Win32 window!", 1000);
		break;
	case WM_MOUSEMOVE:
		//window->message("Moving the mouse, aren't we?", 200);
		break;
	case WM_LBUTTONDBLCLK:
		window->mouseDoubleClickEvent(NULL);
		break;
	case WM_LBUTTONDOWN:
		window->mousePressEvent(NULL);
		break;
	case WM_LBUTTONUP:
		window->mouseReleaseEvent(NULL);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
void CVideoView::RegistRenderWndClass() {
	if (!g_isRegistRenderWndClasss) {
		HINSTANCE hinst = GetModuleHandle(0);
		WNDCLASSEX wcx;
		wcx.hInstance = hinst;
		wcx.lpszClassName = VideoRenderWndClass;
		wcx.lpfnWndProc = (WNDPROC)WndProc;
		wcx.style = CS_DBLCLKS;
		wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcx.lpszMenuName = NULL;
		wcx.cbSize = sizeof(WNDCLASSEX);
		wcx.cbClsExtra = 0;
		wcx.cbWndExtra = 0;
		wcx.hbrBackground = GetSysColorBrush(COLOR_3DFACE);

		// Register our window class with the operating system.
		// If there is an error, exit program.
		if (!RegisterClassEx(&wcx)) {
			MessageBox(0, TEXT("Failed to register window class!"), TEXT("Error!"), MB_OK | MB_ICONERROR);
			return;
		}
		g_isRegistRenderWndClasss = true;
	}
}

void CVideoView::mouseDoubleClickEvent(QMouseEvent* event){
	emit doubleClick();
}
void CVideoView::mousePressEvent(QMouseEvent *event){
}
void CVideoView::mouseReleaseEvent(QMouseEvent *event){
	emit click();
}
void CVideoView::resizeEvent(QResizeEvent* event) {
	QWinHost::resizeEvent(event);
	if (m_videoRendererView) {
		m_videoRendererView->UpdateWindowSize();
	}
}
