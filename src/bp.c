/*
 * ARM pipeline timing simulator
 *
 * CMSC 22200, Fall 2016
 * Gushu Li and Reza Jokar
 */
#include "helper.h"
#include "pipe.h"
#include "bp.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

uint32_t get_BTB_index(CPU_State aCPU_State) {
	return get_instruction_segment(2, 11, aCPU_State.PC);
}

void update_BTB() {
	printf("UPDATE BTB\n");
}

void update_GHR() {
	printf("UPDATE GHR\n");
}

void update_PHT() {
	printf("UPDATE PHT\n");
}

int should_take_branch(int aSaturatingCounter) {
	if (aSaturatingCounter == 3 || aSaturatingCounter == 2) {
		return 1;
	}
	return 0;
}

void bp_predict(CPU_State aCPU_State, bp_t aBP) {
    /* Predict next PC */
    uint64_t myPCPrediction = aCPU_State.PC + 4;
    uint32_t myBTB_index = get_BTB_index(aCPU_State);
    gshare_t myGshare = aBP.gshare;
    if (aBP.BTB[myBTB_index].branch_target != 0) {
    	if ((aBP.BTB[myBTB_index].unconditional == 1) || should_take_branch(myGshare.PHT[myGshare.GHR ^ aCPU_State.PC])) {
    		myPCPrediction = aBP.BTB[myBTB_index].branch_target;
    	}
    }
    aCPU_State.PC = myPCPrediction;
}


void bp_update(CPU_State aCPU_State, bp_t aBP) {
    /* Update BTB */
	update_BTB();

    /* Update global history register */
    update_GHR();

    /* Update gshare directional predictor */
    update_PHT();
}
