# What?

과거 브레인서버 미니런처에서 사용되었던 플러그인 소스입니다.

(구버전 스타크래프트 기준이라 최신 버전에서는 작동되지 않음)



# 기능

* DLL 형식
  * 런처에서 시도한 경우 런처와의 통신
  * 스타크래프트에 주입된 경우 아래의 기능을 수행

* 스타크래프트에서는
  * 함수 후킹 (https://github.com/microsoft/Detours 사용)
  * 게임시작시 무결성 주장 (타 게이머들과)
  * 해킹 툴 모니터링
  * 검증 가능한 캡쳐 생성
    * 스테가노그래피
    * 참고
      * https://ko.wikipedia.org/wiki/스테가노그래피
      * https://www.bloter.net/archives/291280
  * 리버싱 방지
    - Themida 정식 구매 후 사용
    - https://www.oreans.com/themida.php
    - Themida/licensed.png



# 빌드 방법

* Open bhdll/bhdll/bhdll.sln
* Visual Studio 2017 Community Edition에서 최종 빌드 테스트 했습니다.



# 기능 상세

#### 함수 후킹 (https://github.com/microsoft/Detours 사용)

- 스크린 캡쳐 이벤트
  - lib_star_capture :: __StormOrdinalOriginal342HookedAfter()
- 화면 그리기 이벤트
  - dllmain.cpp :: __TXTFUNCTIONDetour()
- 텍스트 그리기 이벤트
  - dllmain.cpp :: __DRAWFUNCTIONDetour()



#### 게임시작시 본인의 무결성 주장 (상대 게이머들과)

- 참고함수
  - dllmain.cpp :: startSign()
  - dllmain.cpp :: checkSign()
- 게임시작이 인식되면
  - 채팅창으로 자신의 "랜덤 값" 전송
- 상대방의 "랜덤 값"를 받으면
  - "랜덤 값"을 런처버전에 따른 고유한 방법으로 변조하여 "변조된 랜덤값"회신
- "변조된 랜덤값"을 되돌려 받고서
  - 내가 계산한 예상된 값과 같으면
  - 상대방이 해킹방지가 되어있는 미니런처 사용 중으로 인식



#### 해킹 툴 모니터링

* 참고 함수
  * dllmain.cpp :: threadMaphack()
* (사전작업)
  * 수집된 스타핵을 실행시켜서 메모리 덤핑
  * 메모리 덤핑을 "정상 메모리 스냅샷"과 "플러그인 로딩시의 스냅샷" 비교
  * 변조 패턴 저장(bhdll/parse/patch.h)
* 쓰레드 생성
  - 메모리에서 해킹툴의 변조 패턴 로딩
  - 쓰레드에서 주기적으로 현재 프로세스 메모리 스냅샷
  - <해킹툴의 변조 패턴>과 <메모리 스냅샷>을 비교
* 50%이상 변조 패턴이 일치하면
  - 해킹으로 인식
* 해킹으로 인식되면
  - 현재 접속이 브레인서버인 경우 서버로 신고
  - 게임 채팅창에 해킹툴 사용 중이라 자백 채팅
  - 게임 종료



#### 검증 가능한 캡쳐 생성 

- 참고 함수
  - dllmain.cpp :: __hookedCapture()
- 캡쳐가 발생하면
  - 기본 캡쳐 이미지를 수정하여, "검증 가능한 캡쳐 이미지"를 별도 생성
- "검증 가능한 캡쳐 이미지"는 미니런쳐를 통해 검증
  
  
  
# 노트

#### 지금 소스를 공개하는 이유는?

정리하지 않아 컴퓨터에서 방치되다가 어느 순간 사라지는 것보다는, 그래도 가치가 있지 않을까 싶어서 올려봅니다.

#### 소스가 너무 구려요

되돌아 보건데 "스크립트 키디" 수준 때 작성했습니다. 후후.  
또한 지금 굳이 리팩토링 해야할 이유가 없어 하지 않았습니다.

#### 이 소스에 가치가 있을까요?

물론 스타크래프트 최신버전의 적용도 되어있지 않아 가치는 매우 낮습니다.  
단지 다른 초보자분들이 우연히 원하는 코드 조각을 찾게되는 낮을 확률만 기대합니다.  

#### 브레인서버 비화가 알고 싶어요. (너가 그 놈이구나?)

그때는 어려서 부끄러웠던 일이 많네요. 혹 물어보시더라도 답변드리기 어려우니 양해부탁드립니다.

  

# 라이센스

* Lib 폴더의 각 라이브러리는 해당 폴더 혹은 파일에 명시된 라이센스를 따릅니다.
* Themida 는 Oreans Technologies 가 저작권을 가집니다.
* MS Detours 는 MIT 라이센스를 따릅니다. (https://github.com/microsoft/Detours)
* 이 외는 MIT 라이센스를 따릅니다.
