
#pragma once
#include <opencv2/opencv.hpp>  // OpenCV 기본 헤더 추가
#include <afxwin.h>  // MFC 기본 헤더
#include <windows.h>
#include "SerialPort.h"


using namespace cv;

class CCAMTEST1Dlg : public CDialogEx
{
public:
    cv::dnn::Net yoloNet;            // ✅ 1. YOLO 모델 객체 추가
    void LoadYOLO();                 // ✅ 2. YOLO 모델 로드 함수 선언
    void DetectYOLO(cv::Mat& img);   // ✅ 3. YOLO 감지 함수 선언


    CCAMTEST1Dlg(CWnd* pParent = nullptr);  // 생성자
    ~CCAMTEST1Dlg();


    // ✅ 변수 초기화 추가
    VideoCapture cap = VideoCapture();   // 웹캠 객체
    CStatic* pImageCtrl = nullptr; // Picture Control 변수
    Mat frame = Mat();          // OpenCV 프레임 저장
    bool isRunning = false;     // 웹캠 실행 여부
    int selectedCamIndex = 0; // 선택된 웹캠 인덱스
    SerialPort* serial;


    // ✅ 웹캠 처리 함수
    static UINT CameraThread(LPVOID pParam);  // 웹캠 쓰레드
    void UpdateFrame();  // 화면 갱신
    void SendCommandToArduino(const char* command);
    int FindExternalCameraIndex(); // 웹캠 자동 선택

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CAMTEST1_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);  // UI 요소와 변수 연결
    CBitmapButton m_btnEmergency;
    CBitmapButton m_btnL;
    CBitmapButton m_btnR;
    CBitmapButton m_btnAuto;
    CBitmapButton m_btnAuto2;
    CBitmapButton m_btngrab;
    CBitmapButton m_btnrelease;
    CBitmapButton m_btnstart;
    CBitmapButton m_btnstop;

protected:
    HICON m_hIcon;

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();

    afx_msg void OnBnClickedButtonStart();  // 웹캠 시작 버튼
    afx_msg void OnBnClickedButtonStop();   // 웹캠 종료 버튼

    //afx_msg void OnDestroy();
    //afx_msg void OnClose();    // 창 닫기 (X 버튼 등) 시 호출

    DECLARE_MESSAGE_MAP()


public:
    afx_msg void OnBnClickedButtonL();
    afx_msg void OnBnClickedButtonGrab();
    afx_msg void OnBnClickedButtonRelease();
    afx_msg void OnBnClickedButtonR();
    afx_msg void OnBnClickedButtonAuto();
    afx_msg void OnBnClickedButtonEmergency();
    afx_msg void OnBnClickedButtonAuto2();
};