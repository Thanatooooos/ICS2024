/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool has_error = false;

  // 检查 GPR x1 ~ x31（x0 应始终为 0，可跳过或单独检查）
  for (int i = 1; i < 32; i++) {
    if (ref_r->gpr[i] != cpu.gpr[i]) {
      printf(ANSI_FMT("GPR MISMATCH", ANSI_FG_RED) ": ");
      printf("gpr[%d] = 0x%08x (ref), 0x%08x (dut) ", i, ref_r->gpr[i], cpu.gpr[i]);
      printf(ANSI_FMT("at pc = 0x%08x\n", ANSI_FG_BLUE), pc);
      has_error = true;
    }
  }

  // 检查 x0 是否为 0（DUT 侧）
  if (cpu.gpr[0] != 0) {
    printf(ANSI_FMT("GPR[0] SHOULD BE 0!", ANSI_FG_RED) " ");
    printf("gpr[0] = 0x%08x (dut), but must be 0\n", cpu.gpr[0]);
    has_error = true;
  }

  // 检查 PC
  if (ref_r->pc != cpu.pc) {
    printf(ANSI_FMT("PC MISMATCH", ANSI_FG_RED) ": ");
    printf("pc = 0x%08x (ref), 0x%08x (dut) ", ref_r->pc, cpu.pc);
    printf(ANSI_FMT("at instruction pc = 0x%08x\n", ANSI_FG_BLUE), pc);
    has_error = true;
  }

  return !has_error;  // 无错误返回 true，否则 false
}


void isa_difftest_attach() {
}
