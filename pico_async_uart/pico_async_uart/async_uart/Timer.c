#include "AsyncUart.h"

// [define] 
#define TIMER_CALLBACK_PERIOD   1 // 定期タイマコールバックの周期(ms)

// [ファイルスコープ変数の宣言]
static repeating_timer_t f_stTimer = {0};       // 定期タイマコールバック登録時に渡すパラメータ
static ULONG f_timerCnt_stabilizationWait = 0;  // 起動してからの安定待ち時間のタイマカウント

// [関数プロトタイプ宣言]
static bool Timer_PeriodicCallback(repeating_timer_t *rt);

// 定期タイマコールバック
static bool Timer_PeriodicCallback(repeating_timer_t *pstTimer) 
{
    // [起動してからの安定待ち時間のタイマカウント]
    if (f_timerCnt_stabilizationWait < ASUART_STABILIZATION_WAIT_TIME) {
        f_timerCnt_stabilizationWait++;
    }
 
    return true; // keep repeating
}

// 起動してからの安定待ち時間が経過したかどうかを取得
bool TIMER_IsStabilizationWaitTimePassed()
{
    return (f_timerCnt_stabilizationWait >= ASUART_STABILIZATION_WAIT_TIME) ? true : false;
}

// タイマーを初期化
void TIMER_Init()
{
    // 定期タイマコールバックの登録
    add_repeating_timer_ms(TIMER_CALLBACK_PERIOD, Timer_PeriodicCallback, NULL, &f_stTimer);
}