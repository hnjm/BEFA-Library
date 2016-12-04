#include <befa.hpp>


struct ArgPrinter
    : public symbol_table::VisitorBase {
  VISIT_ALL(arg) {
    if (arg) printf("%s, ", arg->getName().c_str());
  }
};

int main(int argc, const char **argv) {
  assert(argc == 2 && "missing path parameter");

  // open file + check if file is valid
  auto file = ExecutableFile::open(*(argv + 1));

  assert(file.isValid() && "file is not valid");

//  file.disassembly().subscribe([](ExecutableFile::instruction_type i) {
//    printf("%lx %d %s\n",
//           ptr_lock(i.getParent()->getParent())->getAddress(),
//           i.getParent()->getId(), i.getDecoded().c_str());
//  });
//  printf("\n");

//  file.symbols()
//      .subscribe([](std::pair<
//          ExecutableFile::symbol_type,
//          std::vector<std::pair<
//              std::shared_ptr<ExecutableFile::basic_block_type>,
//              std::vector<ExecutableFile::instruction_type>
//          >>
//          > symbol) {
//
//      });
  std::string old_symbol;
  file.basic_block()
      .subscribe([&old_symbol](std::pair<
          std::shared_ptr<ExecutableFile::basic_block_type>,
          std::vector<ExecutableFile::instruction_type>
      > basic_block) {
        auto bb = basic_block.first;
        auto symbol = ptr_lock(bb->getParent());
        if (symbol->getName() != old_symbol) {
          printf("Symbol %s <0x%08lx>:\n", symbol->getName().c_str(),
                 symbol->getAddress());
          old_symbol = symbol->getName();
        }
        printf("    BasicBlock #0x%08lx\n", bb->getId());
        for (auto &instr : basic_block.second) {
          printf(
              "      <%lX>: %s ",
              instr.getAddress(),
              instr.parse()[0].c_str()
          );
          ArgPrinter arg_printer;
          for (auto &arg : instr.getArgs())
            arg->accept(arg_printer);
          printf("\n");
        }
      });
  file.runDisassembler();
  return 0;
}