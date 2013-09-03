/*
 *******************************************************************************
 *
 *  option_parser.c - MeshWater-2D-Float
 *
 *  Copyright (C) 2006-2006 Yuta Taniguchi
 *******************************************************************************
 */

#include <stdlib.h>
#include "main.h"
#include "option_parser.h"

#define GET_OPT_ARG(f) ((argv[i][2] == '\0') ?\
                              f(&argv[++i][0]) : f(&argv[i][2]))

/* 起動時引数解析関数 */
ParseResult parseOption(Config *config, int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'h': // 使い方を表示して終了
                printf(MSG_HELP);
                return OPTPSR_SUCCESS_EXIT;
                break;
            case 'v': // 冗長な表示を行う
                config->isVerbose = true;
                break;
            case 'f': // フルスクリーンモードに設定
                config->isFullScreen = true;
                break;
            case 'a': // 波紋の減衰率を取得
                config->attRate = GET_OPT_ARG(atof);
                break;
            case 'r': // 波紋の大きさを取得
                config->rippleRadius = GET_OPT_ARG(atoi);
                break;
            case 'i': // 背景のファイル名を取得
                config->imagePath = GET_OPT_ARG();
                break;
            default: // 不正な引数が指定された場合、エラー表示をして終了
                fprintf(stderr, MSG_ERROR_INVALID_OPTION, argv[i]);
                return OPTPSR_FAILED;
            }
        }
        else { // '-'で始まらない、不正な引数が指定された場合
            // エラー表示をして終了
            fprintf(stderr, MSG_ERROR_INVALID_OPTION, argv[i]);
            return OPTPSR_FAILED;
        }
    }
    return OPTPSR_SUCCESS;
}


