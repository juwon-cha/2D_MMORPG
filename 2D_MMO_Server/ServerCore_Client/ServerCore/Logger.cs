using System;
using System.Collections.Generic;
using System.Text;

namespace ServerCore
{
    // DLL 외부(Unity)에서 로그 처리 함수를 등록할 수 있도록 통로를 열어주는 클래스
    public static class Logger
    {
        // 일반 로그와 에러 로그를 위한 Action 델리게이트 정의
        // Action<string>은 string 타입의 인자 하나를 받는 함수를 의미
        public static Action<string> OnLog;
        public static Action<string> OnLogError;

        // DLL 내부에서는 이 함수들을 호출하여 로그를 남김
        public static void Log(string message)
        {
            // OnLog에 함수가 등록되어 있다면(Unity 쪽에서 연결했다면) 실행
            OnLog?.Invoke(message);
        }

        public static void LogError(string message)
        {
            // OnLogError에 함수가 등록되어 있다면 실행
            OnLogError?.Invoke(message);
        }
    }
}
