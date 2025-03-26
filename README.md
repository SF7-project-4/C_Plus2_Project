# 🦾C++기반 아두이노를 활용한 객체 인식 및 물건 자동 분류 로봇팔

## 1. 프로젝트 개요 및 주제 선정 배경
- 프로젝트 목표
  - 아두이노 로봇을 과학상자로 제작하고, C++ 기반 MFC GUI로 연동 및 통제하는 프로그램 구현
  - YOLO 기반으로 객체를 인식하고, 인식한 물체에 따라 자동으로 기능을 수행하는 프로그램 구현
- 주제 선정 이유
  - 로봇 팔은 스마트팩토리 구성에 기본적인 요소로 실제 구현을 통하여 실무 능력을 향상시킨다.
  - 기본 기능에 AI를 추가 도입하여 실제 현장에서 사용 가능한 수준의 로봇을 구현해 본다.
    
## 2. 주요 기술 스택
- 프로그램 언어:  C++
- IDE: Visual studio 2022
- SW/HW 제작: 아두이노, 과학상자 6호

  

## 3. 팀 구성 및 역할 분담
| 이름 | 역할 | 주요 업무 | GitHub 프로필 |
|------|------|----------|------------|
| **유승태** | 팀장 | 계획서 작성, 하드웨어 제작, 소프트웨어 제작 | [![GitHub](https://img.shields.io/badge/GitHub-Profile-black?logo=github)](https://github.com/Yoo-Seung-Tae) |
| **한용찬** | 팀원 | 계획서 작성, 하드웨어 제작, 소프트웨어 제작 | [![GitHub](https://img.shields.io/badge/GitHub-Profile-black?logo=github)](https://github.com/gksdydcks) |

## 4. 개발 일정
| 마일스톤 | 목표 날짜 | 설명 |
|------------|-------------|---------------------------------|
| 사전 기획 | 2025-03-19 | 프로젝트 기획, 주제 선정, git 생성 |
| 아두이노, 웹캠 연결 | 2025-03-19 ~ 03-20 | 아두이노-PC 연결, MFC GUI 구상, 웹캠 연결 |
| AI, 로봇팔 작동 기능 구현 | 2025-03-21 ~ 03-21 | c++기반 openCV에 yolo 탑재, 로봇팔 작동 버튼 및 기능 구현 |
| AI적용 로봇팔 작동 기능 추가 | 2025-03-22 ~ 03-24 | 인식된 객체에 따른 각각 다른 동작 기능 추가  |
| 오류 수정 | 2025-03-25 ~ 03-26 | 오류 수정 및 정리 |
| 시연 | 2025-03-27 | 프로젝트 시연 |
  
## 5. 개발 프로세스

- 개발 순서
  
  ![Image](https://github.com/user-attachments/assets/ae18a861-51e8-4e36-886b-4385b18c8f1a)
     
  
- 플로우 차트
  
   ![Image](https://github.com/user-attachments/assets/3c8df252-c954-4844-8dfd-3db2889784ba)

- 핵심 기능 리스트

| 기능 이름 | 설명 | 우선순위 | 담당자 |
| --- | --- | --- | --- |
| MFC 화면 출력 | MFC 기반 GUI 출력 | medium | 유승태, 한용찬 |
| X축 이동 | 로봇 팔을 위, 아래로 이동 | High | 유승태, 한용찬 |
| 집게 조작 | 조작을 통한 물건 잡기 및 놓기 | High | 유승태, 한용찬 |
| 웹캠 연결 | 작업 현장 확인 및 이미지 입력 | medium | 유승태, 한용찬 |
| 정지 | 모든 기능 즉시 정지 | Low | 유승태, 한용찬 |
| AI 기능 구현 | 객체 인식하여 인식된 객체에 따른 자동 운행 | high | 유승태, 한용찬 |
  
- 시스템 구성
  
   ![Image](https://github.com/user-attachments/assets/268e4ad3-9c45-4f36-b59c-b2adfca4ac93) 



## 6. GUI
- 구상 GUI
  
  ![Image](https://github.com/user-attachments/assets/d731cc2c-6f0c-410e-9959-34a6e5af4fa2)
  
- 구현 GUI
  
  ![Image](https://github.com/user-attachments/assets/3be6cc8c-9093-4beb-8ff1-29a1153e9c8c)


## 7. 시연 영상


## 8. 참고 자료 및 링크

깃허브 주소: https://github.com/SF7-project-4/C_Plus2_Project

과학상자: https://sciencebox.co.kr/bbs/board.php?bo_table=manual&wr_id=7&device=pc






