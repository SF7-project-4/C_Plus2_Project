#include "pch.h"
#include "framework.h"
#include "CAMTEST1.h"
#include "CAMTEST1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
std::vector<std::string> classNames;  // YOLO 클래스 이름 저장용

BEGIN_MESSAGE_MAP(CCAMTEST1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CCAMTEST1Dlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CCAMTEST1Dlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_L, &CCAMTEST1Dlg::OnBnClickedButtonL)
	ON_BN_CLICKED(IDC_BUTTON_GRAB, &CCAMTEST1Dlg::OnBnClickedButtonGrab)
	ON_BN_CLICKED(IDC_BUTTON_RELEASE, &CCAMTEST1Dlg::OnBnClickedButtonRelease)
	ON_BN_CLICKED(IDC_BUTTON_R, &CCAMTEST1Dlg::OnBnClickedButtonR)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, &CCAMTEST1Dlg::OnBnClickedButtonAuto)
	ON_BN_CLICKED(IDC_BUTTON_EMERGENCY, &CCAMTEST1Dlg::OnBnClickedButtonEmergency)
	//ON_WM_DESTROY()
	//ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_AUTO2, &CCAMTEST1Dlg::OnBnClickedButtonAuto2)
END_MESSAGE_MAP()

using namespace cv;
using namespace cv::dnn;  // ✅ 이게 중요합니다

#include <fstream>  // ✅ 파일 체크용 헤더

bool fileExists(const std::string& filename) {
	std::ifstream file(filename);
	return file.good();
}


using namespace cv;
using namespace std;

CCAMTEST1Dlg::CCAMTEST1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CAMTEST1_DIALOG, pParent), serial(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	selectedCamIndex = 0;
}

CCAMTEST1Dlg::~CCAMTEST1Dlg()
{
	if (serial) {
		serial->Close();
		delete serial;
	}
	if (cap.isOpened()) {
		cap.release();
	}
}


void CCAMTEST1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


// ✅ 외장 웹캠을 우선 선택 (없으면 내장 웹캠 사용)
int CCAMTEST1Dlg::FindExternalCameraIndex()
{
	int maxCameras = 5;  // 최대 5개의 웹캠 검색
	int internalCam = -1; // 내장 웹캠 (보통 0번)
	int externalCam = -1; // 외부 웹캠 (USB 웹캠)

	for (int i = 0; i < maxCameras; i++)
	{
		VideoCapture tempCam;
		if (tempCam.open(i))
		{
			cout << "웹캠 감지됨: " << i << endl;

			// ✅ 내장 웹캠(보통 0번)과 외부 웹캠을 구분
			if (i == 0)
				internalCam = i;  // 내장 웹캠 저장
			else
				externalCam = i;  // 외장 웹캠 저장

			tempCam.release();
		}
	}

	// ✅ 외부 웹캠이 있으면 우선 사용, 없으면 내장 웹캠 사용
	if (externalCam != -1) return externalCam;
	if (internalCam != -1) return internalCam;

	AfxMessageBox(_T("사용 가능한 웹캠이 없습니다!"));
	return 0;  // 기본 내장 웹캠 사용
}

// ✅ 다이얼로그 초기화
BOOL CCAMTEST1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LoadYOLO();  // YOLO 로드

	selectedCamIndex = FindExternalCameraIndex();

	// 🔸 웹캠 열고 첫 프레임을 가져와 해상도 확인
	cap.open(selectedCamIndex);
	cap >> frame;
	cap.release();

	int camWidth = frame.cols > 0 ? frame.cols : 640;
	int camHeight = frame.rows > 0 ? frame.rows : 480;

	// 🔸 Picture Control 크기를 웹캠 해상도로 조정
	CRect rect;
	GetDlgItem(IDC_PICTURE)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.right = rect.left + camWidth * 0.9;
	rect.bottom = rect.top + camHeight * 0.9;
	GetDlgItem(IDC_PICTURE)->MoveWindow(&rect, TRUE);

	// Picture Control 포인터 연결
	pImageCtrl = (CStatic*)GetDlgItem(IDC_PICTURE);
	if (!pImageCtrl) {
		AfxMessageBox(_T("Picture Control을 찾을 수 없습니다!"));
		return FALSE;
	}

	serial = new SerialPort("COM4", 9600);
	if (!serial->Open()) {
		AfxMessageBox(_T("아두이노 연결 실패 (COM4)"));
	}
	else {
		AfxMessageBox(_T("아두이노 연결 성공 (COM4)"));
	}

	// stop 버튼 이미지 추가
	CRect rect2;
	GetDlgItem(IDC_BUTTON_EMERGENCY)->GetWindowRect(&rect2);
	ScreenToClient(&rect2);

	// 기존 버튼 제거 (선택사항: 디자인한 버튼 숨기기)
	GetDlgItem(IDC_BUTTON_EMERGENCY)->ShowWindow(SW_HIDE);

	// 비트맵 버튼 생성 (같은 위치에)
	m_btnEmergency.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rect2, this, IDC_BUTTON_EMERGENCY);

	// 이미지 로드
	m_btnEmergency.LoadBitmaps(IDB_BITMAP2, IDB_BITMAP3); // 상태별로 다양하게 설정 가능


	// 크기에 맞게 조정 (옵션)
	m_btnEmergency.SizeToContent();


	// 반시계방향 회전 버튼 이미지 추가
	CRect rect3;
	GetDlgItem(IDC_BUTTON_L)->GetWindowRect(&rect3);
	ScreenToClient(&rect3);

	// 기존 버튼 제거 (선택사항: 디자인한 버튼 숨기기)
	GetDlgItem(IDC_BUTTON_L)->ShowWindow(SW_HIDE);

	// 비트맵 버튼 생성 (같은 위치에)
	m_btnL.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rect3, this, IDC_BUTTON_L);

	// 이미지 로드
	m_btnL.LoadBitmaps(IDB_BITMAP4, IDB_BITMAP5); // 상태별로 다양하게 설정 가능


	// 크기에 맞게 조정 (옵션)
	m_btnL.SizeToContent();

	// 시계방향 회전 버튼 이미지 추가
	CRect rect4;
	GetDlgItem(IDC_BUTTON_R)->GetWindowRect(&rect4);
	ScreenToClient(&rect4);

	// 기존 버튼 제거 (선택사항: 디자인한 버튼 숨기기)
	GetDlgItem(IDC_BUTTON_R)->ShowWindow(SW_HIDE);

	// 비트맵 버튼 생성 (같은 위치에)
	m_btnR.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rect4, this, IDC_BUTTON_R);

	// 이미지 로드
	m_btnR.LoadBitmaps(IDB_BITMAP6, IDB_BITMAP7); // 상태별로 다양하게 설정 가능


	// 크기에 맞게 조정 (옵션)
	m_btnR.SizeToContent();


	// 시계방향 자동 회전 버튼 이미지 추가
	CRect rect5;
	GetDlgItem(IDC_BUTTON_AUTO)->GetWindowRect(&rect5);
	ScreenToClient(&rect5);

	// 기존 버튼 제거 (선택사항: 디자인한 버튼 숨기기)
	GetDlgItem(IDC_BUTTON_AUTO)->ShowWindow(SW_HIDE);

	// 비트맵 버튼 생성 (같은 위치에)
	m_btnAuto.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rect5, this, IDC_BUTTON_AUTO);

	// 이미지 로드
	m_btnAuto.LoadBitmaps(IDB_BITMAP8, IDB_BITMAP9); // 상태별로 다양하게 설정 가능


	// 크기에 맞게 조정 (옵션)
	m_btnAuto.SizeToContent();

	// 반시계방향 자동 회전 버튼 이미지 추가
	CRect rect6;
	GetDlgItem(IDC_BUTTON_AUTO2)->GetWindowRect(&rect6);
	ScreenToClient(&rect6);

	// 기존 버튼 제거 (선택사항: 디자인한 버튼 숨기기)
	GetDlgItem(IDC_BUTTON_AUTO2)->ShowWindow(SW_HIDE);

	// 비트맵 버튼 생성 (같은 위치에)
	m_btnAuto2.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rect6, this, IDC_BUTTON_AUTO2);

	// 이미지 로드
	m_btnAuto2.LoadBitmaps(IDB_BITMAP10, IDB_BITMAP11); // 상태별로 다양하게 설정 가능


	// 크기에 맞게 조정 (옵션)
	m_btnAuto2.SizeToContent();

	// 잡기 버튼 이미지 추가
	CRect rect7;
	GetDlgItem(IDC_BUTTON_GRAB)->GetWindowRect(&rect7);
	ScreenToClient(&rect7);

	// 기존 버튼 제거 (선택사항: 디자인한 버튼 숨기기)
	GetDlgItem(IDC_BUTTON_GRAB)->ShowWindow(SW_HIDE);

	// 비트맵 버튼 생성 (같은 위치에)
	m_btngrab.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rect7, this, IDC_BUTTON_GRAB);

	// 이미지 로드
	m_btngrab.LoadBitmaps(IDB_BITMAP12, IDB_BITMAP13); // 상태별로 다양하게 설정 가능


	// 크기에 맞게 조정 (옵션)
	m_btngrab.SizeToContent();

	// 놓기 버튼 이미지 추가
	CRect rect8;
	GetDlgItem(IDC_BUTTON_RELEASE)->GetWindowRect(&rect8);
	ScreenToClient(&rect8);

	// 기존 버튼 제거 (선택사항: 디자인한 버튼 숨기기)
	GetDlgItem(IDC_BUTTON_RELEASE)->ShowWindow(SW_HIDE);

	// 비트맵 버튼 생성 (같은 위치에)
	m_btnrelease.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rect8, this, IDC_BUTTON_RELEASE);

	// 이미지 로드
	m_btnrelease.LoadBitmaps(IDB_BITMAP14, IDB_BITMAP15); // 상태별로 다양하게 설정 가능


	// 크기에 맞게 조정 (옵션)
	m_btnrelease.SizeToContent();

	// 화면 출력 버튼 이미지 추가
	CRect rect9;
	GetDlgItem(IDC_BUTTON_START)->GetWindowRect(&rect9);
	ScreenToClient(&rect9);

	// 기존 버튼 제거 (선택사항: 디자인한 버튼 숨기기)
	GetDlgItem(IDC_BUTTON_START)->ShowWindow(SW_HIDE);

	// 비트맵 버튼 생성 (같은 위치에)
	m_btnstart.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rect9, this, IDC_BUTTON_START);

	// 이미지 로드
	m_btnstart.LoadBitmaps(IDB_BITMAP16, IDB_BITMAP17); // 상태별로 다양하게 설정 가능


	// 크기에 맞게 조정 (옵션)
	m_btnstart.SizeToContent();

	// 화면 출력 정지 버튼 이미지 추가
	CRect rect10;
	GetDlgItem(IDC_BUTTON_STOP)->GetWindowRect(&rect10);
	ScreenToClient(&rect10);

	// 기존 버튼 제거 (선택사항: 디자인한 버튼 숨기기)
	GetDlgItem(IDC_BUTTON_STOP)->ShowWindow(SW_HIDE);

	// 비트맵 버튼 생성 (같은 위치에)
	m_btnstop.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rect10, this, IDC_BUTTON_STOP);

	// 이미지 로드
	m_btnstop.LoadBitmaps(IDB_BITMAP18, IDB_BITMAP19); // 상태별로 다양하게 설정 가능


	// 크기에 맞게 조정 (옵션)
	m_btnstop.SizeToContent();

	return TRUE;
}

// ✅ 웹캠 프레임 갱신 (YOLO 제거)
void CCAMTEST1Dlg::UpdateFrame()
{
	if (!cap.isOpened() || pImageCtrl == nullptr) {
		AfxMessageBox(_T("웹캠 또는 Picture Control이 초기화되지 않았습니다!"));
		return;
	}

	cap >> frame;
	if (frame.empty()) return;

	flip(frame, frame, 1);  // 셀카처럼 좌우 반전

	// 🔸 YOLO 처리용 복사본 생성
	Mat yoloFrame = frame.clone();
	DetectYOLO(yoloFrame);  // YOLO 적용


	// 🔸 YOLO 적용된 프레임을 사용
	frame = yoloFrame;

	// 🔸 화면 표시용으로 BGRA 변환 (YOLO는 BGR)
	cvtColor(frame, frame, COLOR_BGR2BGRA);

	// 🔸 Picture Control 크기에 맞게 리사이즈
	CRect rect;
	pImageCtrl->GetClientRect(&rect);
	resize(frame, frame, Size(rect.Width(), rect.Height()));

	CImage img;
	img.Create(frame.cols, frame.rows, 32);

	uchar* pDst = (uchar*)img.GetBits();
	if (!pDst) return;

	int stride = img.GetPitch();
	for (int y = 0; y < frame.rows; y++) {
		uchar* pSrc = frame.ptr<uchar>(y);
		memcpy(pDst + y * stride, pSrc, frame.cols * 4);
	}

	CDC* pDC = pImageCtrl->GetDC();
	if (!pDC) return;

	img.StretchBlt(pDC->m_hDC, 0, 0, rect.Width(), rect.Height(), 0, 0, frame.cols, frame.rows);
	pImageCtrl->ReleaseDC(pDC);
}

void CCAMTEST1Dlg::SendCommandToArduino(const char* command)
{
	if (serial && serial->Open()) {
		serial->Write(command, strlen(command));
	}
}


// ✅ 웹캠 시작
void CCAMTEST1Dlg::OnBnClickedButtonStart()
{
	if (!cap.open(selectedCamIndex))
	{
		AfxMessageBox(_T("웹캠을 열 수 없습니다!"));
		return;
	}

	isRunning = true;
	AfxBeginThread(CameraThread, this);
}

// ✅ 웹캠 종료
void CCAMTEST1Dlg::OnBnClickedButtonStop()
{
	if (isRunning)
	{
		isRunning = false;
		Sleep(100);
		cap.release();
		Invalidate();
	}
}

// ✅ 웹캠 쓰레드 실행
UINT CCAMTEST1Dlg::CameraThread(LPVOID pParam)
{
	CCAMTEST1Dlg* pDlg = (CCAMTEST1Dlg*)pParam;
	while (pDlg->isRunning)
	{
		pDlg->UpdateFrame();
		Sleep(30);
	}
	return 0;
}

// ✅ OnPaint() 정의
void CCAMTEST1Dlg::OnPaint()
{
	if (IsIconic()) // 창이 최소화되었을 때
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ✅ OnQueryDragIcon() 정의
HCURSOR CCAMTEST1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ✅ OnSysCommand() 정의
void CCAMTEST1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


void CCAMTEST1Dlg::LoadYOLO()
{
	std::string cfgPath = "C:\\Users\\Administrator\\source\\repos\\CAMTEST1\\yolov4-tiny.cfg";
	std::string weightsPath = "C:\\Users\\Administrator\\source\\repos\\CAMTEST1\\yolov4-tiny.weights";

	if (!fileExists(cfgPath) || !fileExists(weightsPath)) {
		AfxMessageBox(_T("YOLOv4 cfg 또는 weights 파일이 없습니다!"));
		return;
	}

	try {
		yoloNet = readNetFromDarknet(cfgPath, weightsPath);
	}
	catch (const cv::Exception& e) {
		AfxMessageBox(CA2T(e.what()));
		return;
	}

	std::ifstream classFile("C:\\Users\\Administrator\\source\\repos\\CAMTEST1\\coco.names");
	if (!classFile.is_open()) {
		AfxMessageBox(_T("클래스 이름 파일(coco.names)을 열 수 없습니다!"));
		return;
	}

	classNames.clear();
	std::string line;
	while (getline(classFile, line)) {
		classNames.push_back(line);
	}

	yoloNet.setPreferableBackend(DNN_BACKEND_OPENCV);
	yoloNet.setPreferableTarget(DNN_TARGET_CPU);

	AfxMessageBox(_T("YOLOv4-tiny 모델이 성공적으로 로드되었습니다!"));
}


void CCAMTEST1Dlg::DetectYOLO(Mat& img)
{
	Mat blob;
	cvtColor(img, img, COLOR_BGRA2BGR);
	blobFromImage(img, blob, 1 / 255.0, Size(416, 416), Scalar(), true, false);  // YOLOv4 기본 사이즈

	yoloNet.setInput(blob);
	std::vector<Mat> outputs;
	yoloNet.forward(outputs, yoloNet.getUnconnectedOutLayersNames());

	float confThreshold = 0.6;
	float nmsThreshold = 0.4;

	std::vector<int> classIds;
	std::vector<float> confidences;
	std::vector<Rect> boxes;

	for (auto& output : outputs)
	{
		for (int i = 0; i < output.rows; ++i)
		{
			Mat scores = output.row(i).colRange(5, output.cols);
			Point classIdPoint;
			double confidence;
			minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);

			if (confidence > confThreshold)
			{
				int centerX = (int)(output.at<float>(i, 0) * img.cols);
				int centerY = (int)(output.at<float>(i, 1) * img.rows);
				int width = (int)(output.at<float>(i, 2) * img.cols);
				int height = (int)(output.at<float>(i, 3) * img.rows);
				int left = centerX - width / 2;
				int top = centerY - height / 2;

				classIds.push_back(classIdPoint.x);
				confidences.push_back((float)confidence);
				boxes.push_back(Rect(left, top, width, height));
			}
		}
	}

	std::vector<int> indices;
	dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

	for (int i : indices)
	{
		Rect box = boxes[i];
		std::string label = classNames[classIds[i]];

		// ✅ "bottle"일 때만 자동 동작 실행
		if (label == "bottle") {
			rectangle(img, box, Scalar(0, 255, 0), 2);
			putText(img, label, Point(box.x, box.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2);
			OnBnClickedButtonAuto();
		}
		if (label == "cell phone") {
			rectangle(img, box, Scalar(255, 0, 0), 2);
			putText(img, label, Point(box.x, box.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2);
			OnBnClickedButtonAuto2();
		}
		if (label == "person") {
			rectangle(img, box, Scalar(0, 0, 255), 2);
			putText(img, label, Point(box.x, box.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2);
		}
	}
}
void CCAMTEST1Dlg::OnBnClickedButtonL()
{
	SendCommandToArduino("m1\n");
}

void CCAMTEST1Dlg::OnBnClickedButtonR()
{
	SendCommandToArduino("m2\n");
}

void CCAMTEST1Dlg::OnBnClickedButtonGrab()
{
	SendCommandToArduino("servo_0\n");
}

void CCAMTEST1Dlg::OnBnClickedButtonRelease()
{
	SendCommandToArduino("servo_180\n");
}


void CCAMTEST1Dlg::OnBnClickedButtonAuto()
{
	int moveDelay = 700; // 1초 (필요시 조정 가능)
	int moveDelay2 = 5000; // 1초 (필요시 조정 가능)
	SendCommandToArduino("servo_0\n");
	Sleep(moveDelay);

	SendCommandToArduino("m1\n");
	Sleep(moveDelay);
	SendCommandToArduino("stop\n");
	Sleep(moveDelay);


	SendCommandToArduino("servo_180\n");
	Sleep(moveDelay);

	SendCommandToArduino("m2\n");
	Sleep(moveDelay2);
	SendCommandToArduino("stop\n");
	Sleep(moveDelay);
}

void CCAMTEST1Dlg::OnBnClickedButtonEmergency()
{
	SendCommandToArduino("stop\n");
}

void CCAMTEST1Dlg::OnBnClickedButtonAuto2()
{
	int moveDelay = 600; // 1초 (필요시 조정 가능)
	int moveDelay2 = 5000; // 1초 (필요시 조정 가능)
	SendCommandToArduino("servo_0\n");
	Sleep(moveDelay);

	SendCommandToArduino("m2\n");
	Sleep(moveDelay2);
	SendCommandToArduino("stop\n");
	Sleep(moveDelay);

	SendCommandToArduino("servo_180\n");
	Sleep(moveDelay);

	SendCommandToArduino("m1\n");
	Sleep(moveDelay);
	SendCommandToArduino("stop\n");
	Sleep(moveDelay);

}
