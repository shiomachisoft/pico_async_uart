#ifndef COMMON_H
#define COMMON_H

#include "AsyncUart.h" 

// [列挙体]
// キューの種類
typedef enum _E_CMN_QUE_KIND { 
    CMN_QUE_KIND_UART_SEND,     // UART送信
    CMN_QUE_KIND_UART_RECV,     // UART受信 
    CMN_QUE_KIND_NUM            // キューの種類の数
} E_CMN_QUE_KIND;

#pragma pack(1)

// [構造体]
// キュー
typedef struct _ST_QUE {
    ULONG head; // 先頭
    ULONG tail; // 末尾
    ULONG max;  // キューのデータ配列の要素数
    PVOID pBuf; // キューのデータ配列へのポインタ
} ST_QUE;

#pragma pack()

// [関数プロトタイプ宣言]
bool CMN_Enqueue(ULONG iQue, PVOID pData, ULONG size);
bool CMN_Dequeue(ULONG iQue, PVOID pData, ULONG size);
void CMN_EntrySpinLock();
void CMN_ExitSpinLock();
void CMN_Init();

#endif

