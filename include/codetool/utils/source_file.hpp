#pragma once

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <sstream>
#include <filesystem>

namespace codetool {

static std::string LocationString(const clang::SourceLocation &location, const clang::SourceManager &sm) {
  clang::SourceLocation SpellingLoc = sm.getSpellingLoc(location);
  clang::PresumedLoc PLoc = sm.getPresumedLoc(SpellingLoc);
  std::stringstream ss;
  std::filesystem::path p(PLoc.getFilename());
  std::string abs_path;
  try {
    if (p.is_absolute()) {
      abs_path = std::filesystem::canonical(p);
    } else {
      abs_path = std::filesystem::canonical(std::filesystem::relative(p));
    }
  } catch (const std::filesystem::filesystem_error &e) {
    abs_path = PLoc.getFilename();
  }
  ss << abs_path << ":" << PLoc.getLine() << ":" << PLoc.getColumn();

  auto full_path = std::filesystem::relative(p);
  return ss.str();
}

}
