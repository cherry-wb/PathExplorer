/*
 * Copyright (C) 2013  Ta Thanh Dinh <thanhdinh.ta@inria.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "checkpoint.h"
#include "../analysis/dataflow.h"
#include "../main.h"

namespace engine
{

using namespace analysis;

/**
 * @brief a checkpoint is created before the instruction (pointed by the current address) executes. 
 * 
 * @param current_address the address pointed by the instruction pointer (IP).
 * @param current_context the cpu context (values of registers) when the IP is at this address.
 */
checkpoint::checkpoint(CONTEXT* current_context)
{
  // the current cpu context,
  PIN_SaveContext(current_context, &(this->cpu_context));
  
  // and the current memory state
  boost::unordered_map<ADDRINT, UINT8>::iterator memvalue_iter;
  ADDRINT mem_addr;
  for (memvalue_iter = original_memvalue_at.begin(); memvalue_iter != original_memvalue_at.end();
       ++memvalue_iter)
  {
    mem_addr = memvalue_iter->first;
    this->memory_state[mem_addr].first() = memvalue_iter->second;
    this->memory_state[mem_addr].second() = *(reinterpret_cast<UINT8*>(mem_addr));
  }
}


/**
 * @brief the checkpoint stores the original values at memory addresses before the executed 
 * instruction overwrites these values.
 * 
 * @param memory_written_address the beginning address that will be written.
 * @param memory_written_length the length of written addresses.
 * @return void
 */
void checkpoint::log_before_execution(ADDRINT memory_written_address, UINT8 memory_written_length)
{
	ADDRINT mem_addr;
  ADDRINT upper_bound_address = memory_written_address + memory_written_length;
  
  for (mem_addr = memory_written_address; mem_addr < upper_bound_address; ++mem_addr) 
  {
    // log the original value at this written address
    if (this->memory_change_log.find(mem_addr) == this->memory_change_log.end()) 
    {
      this->memory_change_log[mem_addr] = *(reinterpret_cast<UINT8*>(mem_addr));
    }
  }
  
  return;
}

} // end of engine namespace
