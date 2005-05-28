/*
 *******************************************************************************
 *
 *  calcfunc.c - Water-2d
 *
 *  Copyright  2004  Yuta Taniguchi
 *******************************************************************************
 */


/* Include Files */
#include <stdbool.h>
#include "main.h"
#include "calcfunc.h"

/* Global Variable */
extern ProgConfig   g_Conf;         // プログラムの設定
extern PosData     *g_pNextData;    // 次の水面データ
extern PosData     *g_pCrntData;    // 今の水面データ
extern PosData     *g_pPrevData;    // 前の水面データ
extern PosData     *g_pRiplData;    // 現在の波紋データへのポインタ
extern Uint16      *g_pRfraTbl;     // 屈折による変移量テーブル
extern SDL_Surface *g_pScreen;      // スクリーンサーフェス
extern SDL_Surface *g_pBgImage;     // 背景サーフェス
extern void (*pCalcFunc)();         // 水面データ更新関数


/* MMX */
void CalculateMMX()
{
    // 飽和処理用上限/下限配列
    int maxHeight = (g_Conf.depthRes - 1) / 2;
    PosData upperLimit[4] = { maxHeight,  maxHeight,  maxHeight,  maxHeight};
    PosData lowerLimit[4] = {-maxHeight, -maxHeight, -maxHeight, -maxHeight};
    Sint16 v = g_Conf.attRate * 32768;
    Sint16 mulnum[4] = {v, v, v, v};
    
    // 初期位置×に移動
    // 壁壁壁壁壁
    // 壁×水水水
    // 壁水水水水
    // 壁水水水水
    int SurfaceWidth = g_Conf.widthRes;
    int SurfaceHeight = g_Conf.heightRes;
    PosData *nextData = g_pNextData;
    PosData *crntData = g_pCrntData;
    PosData *prevData = g_pPrevData;
    nextData += 1 * (SurfaceWidth + 2) + 1;
    crntData += 1 * (SurfaceWidth + 2) + 1;
    prevData += 1 * (SurfaceWidth + 2) + 1;

    for (int y = 0; y < SurfaceHeight; y++)
    {
        for (int x = 0; x < SurfaceWidth; x += 4)
        {
            asm volatile (
                // ■□■□ここから計算処理□■□■
                // crntDataの上下左右を加算
                "movq -2(%%esi), %%mm0\n"              // crntDataの左を読み込む
                "paddsw 2(%%esi), %%mm0\n"             // crntDataの右を加算
                "paddsw (%%esi, %%ebx, 2), %%mm0\n"    // crntDataの下を加算
                "neg %%ebx\n"                          // オフセットの2の補数をとる
                "paddsw (%%esi, %%ebx, 2), %%mm0\n"    // crntDataの上を加算
                
                // 加算したものを2分の1にする（0方向に丸める版）
                "movq %%mm0, %%mm1\n"      // コピー
                "psrlw $15, %%mm1\n"       // 最上位ビットを抽出
                "paddsw %%mm1, %%mm0\n"    // 最上位ビットを加算
                "psraw $1, %%mm0\n"        // 算術右シフト演算
                
                // oldWaterの中央を減算
                "psubsw (%%eax), %%mm0\n"
                
                // ■□■□ここから飽和処理□■□■
                // 上限越えチェック用配列を読み込む
                "movq (%%ecx), %%mm5\n"
                "movq %%mm0, %%mm6\n"       // 比較で上書きされるのでコピー
                
                // 上限を越えている箇所を探す（マスク作成）
                "pcmpgtw %%mm5, %%mm0\n"    // 上限を越えている所が1、他は0になる
                
                // 飽和させる 
                "pand %%mm0, %%mm5\n"       // 配列を上限越え箇所のみにする
                "pandn %%mm6, %%mm0\n"      // 上限越え箇所を0にする
                "por %%mm5, %%mm0\n"        // 論理和合成
                
                // 下限越えチェック用配列を読み込む
                "movq (%%edx), %%mm5\n"
                "movq %%mm5, %%mm6\n"       // 比較で上書きされるのでコピー
                
                // 下限を越えている箇所を探す（マスク作成）
                "pcmpgtw %%mm0, %%mm6\n"    // 下限を越えている所が1になる
                
                // 飽和させる
                "pand %%mm6, %%mm5\n"       // 配列を下限越え箇所のみにする
                "pandn %%mm0, %%mm6\n"      // 下限越え箇所を0にする
                "por %%mm5, %%mm6\n"        // 論理和合成
                
                // メモリに書き戻す
                "movq %%mm6, (%%edi)\n"
            :
            : "S" (crntData),              // 今の水面
              "D" (nextData),              // 新しい水面
              "a" (prevData),              // 古い水面
              "b" (SurfaceWidth + 2),      // 水面データの幅
              "c" (upperLimit),            // オーバーチェック用配列
              "d" (lowerLimit)             // アンダーチェック用配列
            : "memory");
            // <<< 1 - 符号付き16bit整数に対するn/32768倍アルゴリズム >>>
            // n倍して、上位2バイトと下位2バイトを得る。
            // 1.n=32768を掛ける(1倍の例)
            //                   SHHHHHHH LLLLLLLL
            // 2.上位2バイトを左シフト、下位2バイトを右シフト
            // S0HHHHHH HLLLLLLL L0000000 00000000
            // 3.上位2バイトと下位2バイトをOR合成
            // SHHHHHHH LLLLLLL0 00000000 0000000L
            // 4.完成
            //                   SHHHHHHH LLLLLLLL
            // FPS=76.08
            asm volatile (
                // ■□■□ここから波を静める処理□■□■
                // mm0 
                "movq  (%%edi), %%mm0\n"     // 水面の高さ
                "movq  (%%esi), %%mm1\n"     // 掛ける数 n
                "movq   %%mm1,  %%mm2\n"     // 掛ける数をコピー
                "pmulhw %%mm0,  %%mm1\n"     // 掛けて上位2バイトを取得
                "pmullw %%mm0,  %%mm2\n"     // 掛けて下位2バイトを取得
                "psllw  $1,     %%mm1\n"     // 上位2バイトを左へ1シフト
                "psrlw  $15,    %%mm2\n"     // 下位2バイトを右へ15シフト
                "por    %%mm2,  %%mm1\n"     // 上位バイトと下位バイトをOR合成
                "movq   %%mm1, (%%edi)\n"    // メモリに書き戻す
            :
            : "S" (mulnum),
              "D" (nextData)
            : "memory");
            // ポインタを進める
            nextData += 4;
            crntData += 4;
            prevData += 4;
        }
        // ポインタを進める
        nextData += 2;
        crntData += 2;
        prevData += 2;
    }
    
    // MMX終了
    asm ("emms\n");
}


