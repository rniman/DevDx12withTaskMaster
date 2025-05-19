
Read file: Src/Core/client.cpp
아래는 DirectX 12 프로젝트의 TaskMaster 기반 단계별 진행 과정과 `client.cpp`의 최종 구조에 대한 한글 요약 문서입니다.

---

## DirectX 12 프로젝트 TaskMaster 기반 진행 요약

### 1. 프로젝트 초기화 및 구조 설계
- **TaskMaster**로 프로젝트를 관리하며, 폴더 구조를 역할별로 분리  
  (예: `Src/Core`, `Src/Renderer`, `Src/Utils` 등)
- `pch.h`/`pch.cpp`를 `Utils`로 이동, Visual Studio에서 PCH 경로 및 include 경로, 미리 컴파일된 헤더 설정 등 C++ 프로젝트 세팅을 완료

### 2. 기본 환경 구축 및 빌드
- Git 저장소 생성, DirectX 12 SDK 설치, Visual Studio 프로젝트 생성
- 빌드 환경 설정 및 `client.cpp`를 진입점으로 지정  
  (main 또는 WinMain 함수에서 윈도우 생성 및 DirectX 12 초기화 코드 작성)

### 3. DirectX 12 초기화 및 파란색 창 출력
- Win32 윈도우 생성
- DXGI 팩토리, 디바이스, 커맨드 큐, 스왑체인, RTV, 커맨드 리스트, 펜스 등 DirectX 12 필수 객체 생성
- 메시지 루프에서 파란색으로 화면을 클리어하는 코드 구현  
  → 정상적으로 파란색 창이 출력됨을 확인

### 4. TaskMaster 단계별 진행 및 완료 처리
- TaskMaster에 각 단계별 Task(윈도우 생성, DX12 초기화, RTV/Viewport 설정, Vertex Buffer/Shader 생성, PSO 구성, Draw, 동기화, Present 등)를 등록하고 순차적으로 진행
- 각 단계별로 `client.cpp`의 실제 구현을 확인하고, 완료된 단계는 TaskMaster에서 `done` 처리
- 삼각형 그리기, 셰이더 작성, PSO 생성, DrawInstanced 호출, OMSetRenderTargets 등 DirectX 12 파이프라인의 핵심 구현을 완료

### 5. 코드 리팩터링 및 분리/복구
- 코드 구조 개선을 위해 `InitD3D12`, `RenderLoop` 등 함수 분리 및 `Renderer.h`/`Renderer.cpp`로 모듈화 시도
- 이후 코드가 망가진 듯 하여, `client.cpp`를 함수 분리 이전(모든 코드가 WinMain 내부에 있는 상태)으로 복구
- 중복 선언/구현, 불필요한 include, 임시 주석 등도 모두 정리하여 최신 정상 상태로 유지

---

## client.cpp 최종 구조 요약

- **헤더 포함**
  - `pch.h`, DirectX 12 관련 헤더, Win32 프로시저 등 필수 헤더만 포함

- **Win32 프로시저**
  - 윈도우 메시지 처리를 위한 WndProc 함수 정의

- **WinMain 함수**
  - **윈도우 생성**: Win32 API로 윈도우 생성 및 등록
  - **DirectX 12 객체 생성**:  
    - DXGI 팩토리, 디바이스, 커맨드 큐, 스왑체인, RTV, 커맨드 리스트, 펜스 등 모든 DirectX 12 필수 객체를 WinMain 내부에서 직접 생성 및 초기화
  - **Vertex Buffer/Shader/PSO 생성**:  
    - 삼각형을 그리기 위한 Vertex Buffer, 셰이더, PSO(Pipeline State Object) 생성 및 바인딩
  - **렌더링 루프**:  
    - 메시지 루프 내에서 DrawInstanced 호출, OMSetRenderTargets, Present, 펜스 동기화 등 모든 렌더링 및 동기화 로직을 직접 처리
  - **정리 및 종료**:  
    - 리소스 해제 및 프로그램 종료 처리

---

## 전체 진행 핵심 요약

- **DirectX 12의 기본 파이프라인**(윈도우 생성, 디바이스/스왑체인/RTV/PSO/Draw/Present 등)을 순차적으로 구현
- **TaskMaster**로 각 단계별 Task를 관리하고, 완료 시점마다 `done` 처리
- **코드 리팩터링**(함수 분리/복구) 및 최종적으로 모든 코드를 WinMain 내부에 직접 구현하는 구조로 정리
- **최종 client.cpp**는 WinMain 내부에 DirectX 12 전체 초기화 및 렌더링 루프가 집약된 형태로, 불필요한 코드 없이 최신 정상 상태로 유지

---