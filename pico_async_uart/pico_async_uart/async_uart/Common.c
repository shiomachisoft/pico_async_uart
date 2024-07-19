#include "AsyncUart.h" 

// [ファイルスコープ変数]
static critical_section_t f_stSpinLock = {0};   // スピンロック
static ST_QUE f_aQue[CMN_QUE_KIND_NUM] = {0}; 	// キューの配列
static UCHAR f_aQueData_uartSend[ASUART_QUE_DATA_MAX_UART] = {0}; // UART送信キューのデータ配列
static UCHAR f_aQueData_uartRecv[ASUART_QUE_DATA_MAX_UART] = {0}; // UART受信キューのデータ配列

// エンキュー
bool CMN_Enqueue(ULONG iQue, PVOID pData, ULONG size) 
{
	bool bRet = false;
	ST_QUE *pstQue = &f_aQue[iQue];
	UCHAR* puchr;

	CMN_EntrySpinLock(); // スピンロックを獲得

	if ((pstQue->head == (pstQue->tail + 1) % pstQue->max)) { 
		// キューが満杯の場合	
		// 無処理
	}
	else {
		if (TIMER_IsStabilizationWaitTimePassed()) { 
			// 起動してからの安定待ち時間が経過していた場合

			// キューイング
			switch (iQue) {
			case CMN_QUE_KIND_UART_SEND: 	// UART送信
			case CMN_QUE_KIND_UART_RECV: 	// UART受信 
				puchr = (UCHAR*)pstQue->pBuf;
				memcpy(&puchr[pstQue->tail], pData, size);
				break; 
			default:
				// ここに来ない
				break;				
			}	
			pstQue->tail = (pstQue->tail + 1) % pstQue->max;
		}
		else {
			// 破棄
		}
		bRet = true;
	}

	CMN_ExitSpinLock(); // スピンロックを解放

	return bRet;
}

// デキュー
bool CMN_Dequeue(ULONG iQue, PVOID pData, ULONG size)
{
	bool bRet = false;
	ST_QUE *pstQue = &f_aQue[iQue];	
	UCHAR* puchr;
	
	CMN_EntrySpinLock(); // スピンロックを獲得

    if (pstQue->head == pstQue->tail) {
		// キューが空の場合
		// 無処理
	}
	else {
		// デキュー
		switch (iQue) {
		case CMN_QUE_KIND_UART_SEND: 	// UART送信
		case CMN_QUE_KIND_UART_RECV: 	// UART受信	
			puchr = (UCHAR*)pstQue->pBuf;
			memcpy(pData, &puchr[pstQue->head], size);
			break; 
		default:
			// ここに来ない
			break;				
		}	
		pstQue->head = (pstQue->head + 1) % pstQue->max;
		bRet = true;
	}

	CMN_ExitSpinLock(); // スピンロックを解放

    return bRet;
}

// スピンロックを獲得
// スピンロックはCPU間排他をしつつ割り込みを禁止にする場合に使用する。
// CPU間排他だけならミューテックスを使用すること。
// Picoのcritical_section(spin lock)とmutexの定義は下記。
// https://www.raspberrypi.com/documentation/pico-sdk/high_level.html#pico_sync
// critical_section(spin lock):
// Critical Section API for short-lived mutual exclusion safe for IRQ and multi-core. 
// mutex:
// Mutex API for non IRQ mutual exclusion between cores. 
void CMN_EntrySpinLock()
{
	critical_section_enter_blocking(&f_stSpinLock);
}

// スピンロックを解放
void CMN_ExitSpinLock()
{
	critical_section_exit(&f_stSpinLock);
}

// 共通ライブラリを初期化
void CMN_Init()
{
	// [変数を初期化]
	critical_section_init(&f_stSpinLock);
	f_aQue[CMN_QUE_KIND_UART_SEND].pBuf = (PVOID)f_aQueData_uartSend;
	f_aQue[CMN_QUE_KIND_UART_RECV].pBuf = (PVOID)f_aQueData_uartRecv;
	f_aQue[CMN_QUE_KIND_UART_SEND].max  = ASUART_QUE_DATA_MAX_UART;
	f_aQue[CMN_QUE_KIND_UART_RECV].max  = ASUART_QUE_DATA_MAX_UART;
}