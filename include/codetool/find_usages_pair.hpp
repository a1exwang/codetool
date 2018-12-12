#pragma once

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>
#include <iostream>
#include <codetool/visitors/find_usage_pair.hpp>


namespace codetool {

class FindUsagesConsumer : public ASTConsumer {
  CompilerInstance &Instance;
public:
  FindUsagesConsumer(CompilerInstance &Instance);
  void HandleTranslationUnit(ASTContext& context) override;
};

class FindUsagesAction : public PluginASTAction {
public:
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return llvm::make_unique<FindUsagesConsumer>(CI);
  }
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &arg) override { return true; };
};

}
