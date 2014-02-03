#include "instruction.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <pin.H>
extern "C" {
#include <xed-interface.h>
}

#include "stuffs.h"

/*====================================================================================================================*/

extern boost::log::sources::severity_logger<boost::log::trivial::severity_level> log_instance;
extern boost::shared_ptr< boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> > log_sink;

/*====================================================================================================================*/

inline std::string contained_image_name(ADDRINT ins_addr)
{
//   for (IMG img = APP_ImgHead(); IMG_Valid(img); img = IMG_Next(img)) 
//   {
//     for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) 
//     {
//       if ((ins_addr >= SEC_Address(sec)) && 
//           (ins_addr < SEC_Address(sec) + SEC_Size(sec))) 
//       {
//         return IMG_Name(img);
//       }
//     }
//   }
  IMG ins_img = IMG_FindByAddress(ins_addr);
  std::string img_name = "";
  
  if (IMG_Valid(ins_img)) 
  {
    img_name = IMG_Name(ins_img);
  }

  return img_name;
}

/*====================================================================================================================*/

instruction::instruction()
{
}

/*====================================================================================================================*/

instruction::instruction(const INS& ins)
{


  this->address         = INS_Address(ins);
  this->disass          = INS_Disassemble(ins);
  this->category        = static_cast<xed_category_enum_t>(INS_Category(ins));
  this->opcode          = INS_Opcode(ins);
  this->contained_image = contained_image_name(this->address);
  this->contained_function = RTN_FindNameByAddress(this->address);
  
//   if (INS_IsDirectBranchOrCall(ins))
//   {
//     this->contained_function = RTN_FindNameByAddress(INS_DirectBranchOrCallTargetAddress(ins));
//   }
//   else
//   {
//     this->contained_function = "";
//   }

  if (INS_IsMemoryRead(ins)) 
  {
    this->mem_read_size = INS_MemoryReadSize(ins);
  } 
  else 
  {
    this->mem_read_size = 0;
  }

  if (INS_IsMemoryWrite(ins)) 
  {
    this->mem_written_size = INS_MemoryWriteSize(ins);
  } 
  else 
  {
    this->mem_written_size = 0;
  }

  UINT32 reg_id;
  REG    reg;

  UINT32 max_num_rregs = INS_MaxNumRRegs(ins);
  for (reg_id = 0; reg_id < max_num_rregs; ++reg_id) 
  {
    reg = INS_RegR (ins, reg_id);
    if (reg != REG_INST_PTR) 
    {
      if (INS_IsRet(ins) && (reg == REG_STACK_PTR)) 
      {
        //
      } 
      else 
      {
        this->src_regs.insert(reg);
      }
    }
  }

  UINT32 max_num_wregs = INS_MaxNumWRegs(ins);
  for (reg_id = 0; reg_id < max_num_wregs; ++reg_id) 
  {
    reg = INS_RegW (ins, reg_id);
    if ((reg != REG_INST_PTR ) || INS_IsBranchOrCall (ins) || INS_IsRet (ins)) 
    {
      if ((reg == REG_STACK_PTR) && INS_IsRet (ins)) 
      {
        //
      } 
      else 
      {
        this->dst_regs.insert (reg);
      }
    }
  }

  this->has_mem_read2 = INS_HasMemoryRead2(ins);

//   if ((this->opcode == XED_ICLASS_PUSH) || (this->opcode == XED_ICLASS_POP))
//   {
//     std::set<REG> common_regs;
//     std::set<REG> new_src_regs;
//     std::set<REG> new_dst_regs;
//
//     std::set_intersection(this->src_regs.begin(), this->src_regs.end(), this->dst_regs.begin(), this->dst_regs.end(),
//                           std::inserter(common_regs, common_regs.begin()));
//     std::set_difference(this->src_regs.begin(), this->src_regs.end(), common_regs.begin(), common_regs.end(),
//                         std::inserter(new_src_regs, new_src_regs.begin()));
//     std::set_difference(this->dst_regs.begin(), this->dst_regs.end(), common_regs.begin(), common_regs.end(),
//                         std::inserter(new_dst_regs, new_dst_regs.begin()));
//
//     this->src_regs.swap(new_src_regs);
//     this->dst_regs.swap(new_dst_regs);
//   }
}

/*====================================================================================================================*/

instruction::instruction(const instruction& other)
{
  this->address           = other.address;
  this->disass            = other.disass;
  this->category          = other.category;

  this->contained_function  = other.contained_function;
  this->contained_image     = other.contained_image;

  this->mem_read_size     = other.mem_read_size;
  this->mem_written_size  = other.mem_written_size;

  this->src_regs          = other.src_regs;
  this->dst_regs          = other.dst_regs;
  this->src_mems          = other.src_mems;
  this->dst_mems          = other.dst_mems;

  this->has_mem_read2 = other.has_mem_read2;
}

/*====================================================================================================================*/

instruction& instruction::operator=(const instruction& other)
{
  this->address           = other.address;
  this->disass            = other.disass;
  this->category          = other.category;

  this->contained_function  = other.contained_function;
  this->contained_image     = other.contained_image;

  this->mem_read_size     = other.mem_read_size;
  this->mem_written_size  = other.mem_written_size;

  this->src_regs          = other.src_regs;
  this->dst_regs          = other.dst_regs;
  this->src_mems          = other.src_mems;
  this->dst_mems          = other.dst_mems;

  this->has_mem_read2 = other.has_mem_read2;

  return *this;
}
