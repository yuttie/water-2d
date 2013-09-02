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

/* ��ư���������ϴؿ� */
ParseResult parseOption(Config *config, int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'h': // �Ȥ�����ɽ�����ƽ�λ
                printf(MSG_HELP);
                return OPTPSR_SUCCESS_EXIT;
                break;
            case 'v': // ��Ĺ��ɽ����Ԥ�
                config->isVerbose = true;
                break;
            case 'f': // �ե륹���꡼��⡼�ɤ�����
                config->isFullScreen = true;
                break;
            case 'a': // ����θ���Ψ�����
                config->attRate = GET_OPT_ARG(atof);
                break;
            case 'r': // ������礭�������
                config->rippleRadius = GET_OPT_ARG(atoi);
                break;
            case 'i': // �طʤΥե�����̾�����
                config->imagePath = GET_OPT_ARG();
                break;
            default: // �����ʰ��������ꤵ�줿��硢���顼ɽ���򤷤ƽ�λ
                fprintf(stderr, MSG_ERROR_INVALID_OPTION, argv[i]);
                return OPTPSR_FAILED;
            }
        }
        else { // '-'�ǻϤޤ�ʤ��������ʰ��������ꤵ�줿���
            // ���顼ɽ���򤷤ƽ�λ
            fprintf(stderr, MSG_ERROR_INVALID_OPTION, argv[i]);
            return OPTPSR_FAILED;
        }
    }
    return OPTPSR_SUCCESS;
}


