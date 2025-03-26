# C++_Project

### 웹캠 연결 순서
1. openCV 설치
2. 프로젝트 속성페이지
![Image](https://github.com/user-attachments/assets/0ca8071a-11d6-475f-af3d-432eb71c8eb4)
3. C/C++ 에서 추가 포함 디렉터리에 opencv\build\include 폴더 주소 입력
![Image](https://github.com/user-attachments/assets/72b6893a-4e97-40c4-bb00-ed927a13ace7)
4. 링커 > 추가 라이브러리 디렉터리에서 opencv\build\x64\vc16\lib 폴더 주소 입력
![Image](https://github.com/user-attachments/assets/b020f49b-45dc-4b1b-9454-1a2e4bc1acd4)
5. 링커 > 입력에서 추가 종속석에 opencv_world4110d.lib 입력
![Image](https://github.com/user-attachments/assets/acaa8cf9-8740-4cd1-9b59-05dc21c3ac25)
6. 왼쪽 위 구성 Releas로 변경 및 opencv_world4110.lib 입력
![Image](https://github.com/user-attachments/assets/74fb7678-4b63-42b3-8ad0-f00ea90b390d)
7. 윈도우 탐색기>환경 변수 편집> Path >새로추가>opencv\build\x64\vc16\bin 파일 주소 넣기
![Image](https://github.com/user-attachments/assets/40812874-a4b0-4a29-8635-3f2c5426b3c9)
