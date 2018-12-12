//===- PrintFunctionNames.cpp ---------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
#include <codetool/find_usages_pair.hpp>

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

using namespace clang;
using namespace std;

static FrontendPluginRegistry::Add <codetool::FindUsagesAction> X("find-usages", "find all usages of a struct field");

codetool::FindUsagesConsumer::FindUsagesConsumer(CompilerInstance &Instance)
    : Instance(Instance) {}

void codetool::FindUsagesConsumer::HandleTranslationUnit(ASTContext &context) {
  codetool::FindUsagePair v(Instance.getSourceManager());
  v.TraverseDecl(context.getTranslationUnitDecl());
}
