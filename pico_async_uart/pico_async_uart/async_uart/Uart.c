#include "AsyncUart.h" 

// [define]
#define UART_IRQ_PRIORITY 0 // 割り込みの優先度(0=最優先)

// [ファイルスコープ変数]
static bool f_isSentFirstByte= false;   // 1byte目を送信済みか否か
static ULONG f_lastErrorBits = 0;       // 最後に発生したUARTエラー

// [関数プロトタイプ宣言]
static void UART_Interrupt();
static void UART_SendFirstbyte();
static inline bool UART_Send();
static void UART_Init();

// 非同期UARTのメイン処理
void ASUART_Main()
{
    // 1byte目のUART送信
    UART_SendFirstbyte(); 
}

// 送信キューに送信データを格納する
int ASUART_SendBuf(PVOID pBuf, int size)
{
    int i;
    UCHAR* pDataAry = (UCHAR*)pBuf;

    for (i = 0; i < size; i++) 
    {
        // 送信データをエンキュー
        if (!CMN_Enqueue(CMN_QUE_KIND_UART_SEND, (PVOID)&pDataAry[i], sizeof(UCHAR))) {
            break; // キューが満杯
        }
    }

    return i;
}

// 受信キューから受信データを取り出す
int ASUART_RecvBuf(PVOID pBuf, int size)
{
    int i;
    UCHAR* pDataAry = (UCHAR*)pBuf;

    for (i = 0; i < size; i++) 
    {
        // 受信データをデキュー
        if (!CMN_Dequeue(CMN_QUE_KIND_UART_RECV, (PVOID)&pDataAry[i], sizeof(UCHAR))) {
            break; // キューが空
        }
    }    

    return i;
}

// 最後のUARTエラー(bits)を取得
ULONG ASUART_GetLastErrorBits()
{
    return f_lastErrorBits;
}

// 最後のUARTエラー(bits)をクリア
void ASUART_ClearLastErrorBits()
{
    f_lastErrorBits = 0;
}

// 非同期UARTを初期化
void ASUART_Init()
{
	// 共通ライブラリを初期化
	CMN_Init();	

	// UARTを初期化
	UART_Init();	

	// タイマーを初期化
	TIMER_Init();	
}

// UART割り込み
static void UART_Interrupt() 
{   
    io_rw_32 dr;  // Data Register:UARTDR
    io_rw_32 rsr; // Receive Status Register/Error Clear Register:UARTRSR/UARTECR
    io_rw_32 ris = uart_get_hw(ASUART_ID)->ris; // 割り込みステータスレジスタ

    if (ris & UART_UARTRIS_RXRIS_BITS) {
        // UART受信割り込みの場合

        while  (uart_is_readable(ASUART_ID)) { // UARTの受信データがまだ存在する場合
            
            // UARTの受信データ(1byte)を取り出し
            dr = uart_get_hw(ASUART_ID)->dr;

            // UARTエラーを取得      
            rsr = uart_get_hw(ASUART_ID)->rsr;
            // rsrのビット
            // 0x00000008 Overrun error
            // 0x00000004 Break error
            // 0x00000002 Parity error
            // 0x00000001 Framing error            
            rsr &= 0x0000000F;

            // UARTエラーをクリア
            uart_get_hw(ASUART_ID)->rsr = ~rsr;
  
            if (rsr) { // UARTエラーが発生している場合
                if (TIMER_IsStabilizationWaitTimePassed()) { 
                    // 起動してからの安定待ち時間が経過していた場合
                    f_lastErrorBits = rsr;
                }        
            }        
            else {
                // UART受信データ1byteのエンキュー
                (void)CMN_Enqueue(CMN_QUE_KIND_UART_RECV, (PVOID)&dr, sizeof(UCHAR));
            }
        }
    }

    if (ris & UART_UARTRIS_TXRIS_BITS) {
        // UART送信割り込みの場合
   
        hw_clear_bits(&uart_get_hw(ASUART_ID)->imsc, 1 << UART_UARTIMSC_TXIM_LSB);

        // 次の1byteのUART送信
        if (!UART_Send()) { 
            // UART送信データのキューが空の場合(UART送信データがもう無い場合) 
            f_isSentFirstByte = false; // 1byte目は未送信    
        }  
    }
}

// 1byte目のUART送信
static void UART_SendFirstbyte()
{
    if (!f_isSentFirstByte) {  // 1byte目をまだ送信済みではない場合
        if (uart_is_writable(ASUART_ID)) { // UART送信可能な場合
            // UART送信キューから送信データ取り出し⇒UART送信
            (void)UART_Send();
        }
    }
}

// UART送信データ取り出し⇒UART送信
static inline bool UART_Send()
{
    UCHAR data;
    bool bRet = false; // 送信成功か否か

     // UART送信データ1byteのデキュー
    if (CMN_Dequeue(CMN_QUE_KIND_UART_SEND, &data, sizeof(UCHAR))) {   
        f_isSentFirstByte = true; // 1byteを送信済み
        // UART送信(1byte)
        uart_get_hw(ASUART_ID)->dr = (io_rw_32)data;
        hw_set_bits(&uart_get_hw(ASUART_ID)->imsc, 1 << UART_UARTIMSC_TXIM_LSB);
        bRet = true;
    }

    return bRet;
}

// UARTを初期化
static void UART_Init()
{
    // UARTを初期化
    uart_init(ASUART_ID, ASUART_BAUD_RATE);  // ボーレート
    // ピンの機能設定
    gpio_set_function(ASUART_TX, GPIO_FUNC_UART);
    gpio_set_function(ASUART_RX, GPIO_FUNC_UART);
    // CTS/RTSを無効に設定
    uart_set_hw_flow(ASUART_ID, false, false);
    // 通信設定
    uart_set_format(ASUART_ID, ASUART_DATA_BITS, ASUART_STOP_BITS, ASUART_PARITY);
    // UARTのFIFOを無効に設定
    uart_set_fifo_enabled(ASUART_ID, false);
    // 割り込み設定
    irq_set_exclusive_handler(ASUART_IRQ, UART_Interrupt);
    irq_set_priority(ASUART_IRQ, UART_IRQ_PRIORITY); // 割り込みの優先度
    irq_set_enabled(ASUART_IRQ, true);    
    uart_set_irq_enables(ASUART_ID, true, true); // UARTのRX・TX割り込みを有効
}

