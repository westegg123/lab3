/*
 * CMSC 22200
 *
 * ARM pipeline timing simulator
 *
 * Reza Jokar and Gushu Li, 2016
 */

#include "pipe.h"
#include "shell.h"
#include "bp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

int CYCLES = 0;
bp_t BRANCH_PREDICTOR;

/* global pipeline state */
CPU_State CURRENT_STATE;

void pipe_init()
{
    memset(&CURRENT_STATE, 0, sizeof(CPU_State));
    CURRENT_STATE.PC = 0x00400000;
}

void pipe_cycle()
{
	printf("\n\n--GSHARE--\n");
	printf("GHR: %x\n", BRANCH_PREDICTOR.gshare.GHR);
	printf("FIRST PHR: %x\n", BRANCH_PREDICTOR.gshare.PHT[0]);

	printf("\n\n--BTB--\n");
	printf("ADDR TAG: %x\n", BRANCH_PREDICTOR.BTB[0].address_tag);
	printf("UNCONDITIONAL: %d\n", BRANCH_PREDICTOR.BTB[0].unconditional);
	printf("VALID: %d\n", BRANCH_PREDICTOR.BTB[0].valid);
	printf("TARGET: %x\n", BRANCH_PREDICTOR.BTB[0].branch_target);
	if (CYCLES == 10) {
		RUN_BIT = 0;
	}
	CYCLES += 1;
	pipe_stage_wb();
	pipe_stage_mem();
	pipe_stage_execute();
	pipe_stage_decode();
	pipe_stage_fetch();
}



void pipe_stage_wb()
{
}

void pipe_stage_mem()
{
}

void pipe_stage_execute()
{
}

void pipe_stage_decode()
{
}

void pipe_stage_fetch()
{
}
