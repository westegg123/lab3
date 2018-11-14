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

extern bp_t BP;
extern CPU_State CURRENT_STATE;
#define BTB_SIZE 1024
#define PHT_SIZE 256

uint32_t get_8_pc_bits(uint32_t aPC) {
	return get_instruction_segment(2, 9, aPC);
}

uint32_t get_BTB_index(uint32_t aPC) {
	return get_instruction_segment(2, 11, aPC);
}

void update_GHR(int aIsConditional, int aBranchTaken) {
	if (aIsConditional == 0) {
		return;
	}

	// printf("UPDATE GHR\n");
	if (aBranchTaken == 1) /*taken*/ {
		BP.gshare.GHR = ((BP.gshare.GHR << 1) | 1);
	} else if (aBranchTaken == 0) /*not taken*/ {
		BP.gshare.GHR = ((BP.gshare.GHR << 1));
	}
	// printf("GHR: %x\n", BP.gshare.GHR);
}

void update_PHT(uint32_t aPC, int aIsConditional, int aBranchTaken) {
	if (aIsConditional == 0) {
		return;
	}

	// printf("UPDATE PHT\n");
	uint32_t myPHTIndex = (get_8_pc_bits(aPC) ^ BP.gshare.GHR); 
	if (aBranchTaken == 1) {
		if (BP.gshare.PHT[myPHTIndex] < 3) {
			BP.gshare.PHT[myPHTIndex] += 1;
		}
	} else if (aBranchTaken == 0) {
		if (BP.gshare.PHT[myPHTIndex] > 0 ) {
			BP.gshare.PHT[myPHTIndex] -= 1;
		}
	}
}

void update_BTB(uint64_t aAddressTag, uint64_t aTargetBranch, int aIsConditional, int aIsValid) {
	// printf("UPDATE BTB\n");
	uint32_t myBTBIndex = get_BTB_index(aAddressTag);
	BP.BTB[myBTBIndex].branch_target = aTargetBranch;
	BP.BTB[myBTBIndex].address_tag = aAddressTag;
	BP.BTB[myBTBIndex].conditional = aIsConditional;
	BP.BTB[myBTBIndex].valid = aIsValid;
}

int should_take_branch(int aSaturatingCounter) {
	if (aSaturatingCounter == 3 || aSaturatingCounter == 2) {
		return 1;
	}
	return 0;
}

void bp_predict() {
    print_bp(BP);
    uint64_t myPCPrediction = CURRENT_STATE.PC + 4;
    gshare_t myGshare = BP.gshare;
    BTB_entry_t myBTB_entry = BP.BTB[get_BTB_index(CURRENT_STATE.PC)];

    // if (BP.BTB[myBTB_index].branch_target != 0) {
    // 	if ((BP.BTB[myBTB_index].conditional == 1) || 
    // 		should_take_branch(myGshare.PHT[myGshare.GHR ^ get_8_pc_bits(CURRENT_STATE.PC)])) {
    // 		// printf("BTB HIT!\n");
    // 		myPCPrediction = BP.BTB[myBTB_index].branch_target;
    // 	}
    // }

    printf("This is my index: %d\n", get_BTB_index(CURRENT_STATE.PC));
    if (myBTB_entry.valid == 1 && myBTB_entry.address_tag == CURRENT_STATE.PC) {
		if (myBTB_entry.conditional == 0) {
			printf("BTB HIT! (conditional)\n");
			myPCPrediction = myBTB_entry.branch_target;
		} else {
			if (should_take_branch(myGshare.PHT[(myGshare.GHR ^ get_8_pc_bits(CURRENT_STATE.PC))])) {
				printf("BTB HIT! (CONDITIONAL)\n");
				myPCPrediction = myBTB_entry.branch_target;
			}
		}
    }

    printf("PREDICTING: %lx\n", myPCPrediction);
    CURRENT_STATE.PC = myPCPrediction;
}

bp_t intialize_bp(bp_t BP) {
	BP.gshare.GHR = 0;

	for (int i = 0; i < PHT_SIZE; i++) {
		BP.gshare.PHT[i] = 0;
	}

	for (int i = 0; i < BTB_SIZE; i++) {
		BP.BTB[i].address_tag = 0;
		BP.BTB[i].valid = 0;
		BP.BTB[i].conditional = 0;
		BP.BTB[i].branch_target = 0;
	}
	return BP;
}

void print_BTB(bp_t BP) {
	for (int i = 0; i < BTB_SIZE; i++) {
		if (BP.BTB[i].valid) {
			printf("Index %d:\n 	Address_tag: %lx \n", i, BP.BTB[i].address_tag);
			printf("	Valid bit: %d, Conditional Bit; %d\n", BP.BTB[i].valid, BP.BTB[i].conditional);
			printf("	Branch Target: %lx\n", BP.BTB[i].branch_target);
		}
	}
}

void print_Gshare(bp_t BP) {
	printf("GSHARE\n");
	printf("GHR: %x\n", BP.gshare.GHR);
	for (int i = 0; i < PHT_SIZE; i++) {
		if (BP.gshare.PHT[i]) {
			printf("Index %d: %d\n", i, BP.gshare.PHT[i]);
		}
	}
}

void print_bp(bp_t BP) {
	print_Gshare(BP);
	print_BTB(BP);
}

void bp_update(uint64_t aAddressTag, uint64_t aTargetBranch, int aIsConditional, int aIsValid, int aBranchTaken) {
    /* Update BTB */
	update_BTB(aAddressTag, aTargetBranch, aIsConditional, aIsValid);

    /* Update gshare directional predictor */
    update_PHT(aAddressTag, aIsConditional, aBranchTaken);

    /* Update global history register */
    update_GHR(aIsConditional, aBranchTaken);

}
