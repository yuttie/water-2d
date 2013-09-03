/*
 *******************************************************************************
 *
 *  option_parser.h - MeshWater-2D-Float
 *
 *  Copyright (C) 2006-2006 Yuta Taniguchi
 *******************************************************************************
 */

#ifndef __OPTION_PARSER_H
#define __OPTION_PARSER_H

#include "main.h"
typedef enum __ParseResult {
    OPTPSR_SUCCESS,
    OPTPSR_SUCCESS_EXIT,
    OPTPSR_FAILED
} ParseResult;

/* 起動時引数解析関数 */
ParseResult parseOption(Config *config, int argc, char **argv);
#endif


