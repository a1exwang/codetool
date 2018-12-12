#pragma once

#include <codetool/utils/source_file.hpp>

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>
#include <iostream>

using namespace std;
using namespace clang;

namespace codetool {
class FindUsagePair : public RecursiveASTVisitor<FindUsagePair> {
public:
  FindUsagePair(SourceManager &sourceManager)
      :sourceManager(sourceManager) {}

  bool VisitMemberExpr(MemberExpr *exp) {
    auto baseType = exp->getBase()->getType();
    auto memberName = exp->getMemberNameInfo().getAsString();
    auto isArray = exp->isArrow();
    QualType type;
    auto This = exp->getBase();
    if (This->getType().getTypePtr()->isPointerType()) {
      type = This->getType().getTypePtr()->getAs<PointerType>()->getPointeeType();
    } else {
      type = This->getType();
    }
    cout << R"({"usage_type": "member", "start": ")" << LocationString(exp->getLocStart(), sourceManager)
        << R"(", "end": ")" << LocationString(exp->getLocEnd(), sourceManager)
        << R"(", "type_name": ")" << type.getAsString() << R"(", "member_name": ")" << memberName << "\"}" << endl;
    return true;
  }

  bool VisitCXXOperatorCallExpr(CXXOperatorCallExpr *exp) {
    cout << R"({"usage_type": "cxx-operator-call", "start": ")" << LocationString(exp->getLocStart(), sourceManager)
        << R"(", "end": ")" << LocationString(exp->getLocEnd(), sourceManager)
        << R"(", "operator": ")" << clang::getOperatorSpelling(exp->getOperator()) << R"(", "type_name": ")" << exp->getArg(0)->getType().getAsString() << "\"}" << endl;
    return true;
  }

  bool VisitCallExpr(CallExpr *exp) {
    FunctionDecl *fn = exp->getDirectCallee();
    if (fn) {
      cout << R"({"usage_type": "call)"
           << R"(", "start": ")" << LocationString(exp->getLocStart(), sourceManager)
           << R"(", "end": ")" << LocationString(exp->getLocEnd(), sourceManager)
           << R"(", "function": ")" << fn->getDeclName().getAsString()
           << R"(", "function_type": ")" << fn->getType().getAsString() << "\"}" << endl;
    }

    return true;
  }

  bool VisitDeclRefExpr(DeclRefExpr *exp) {
    cout << R"({"usage_type": "decl-ref", "start": ")" << LocationString(exp->getLocStart(), sourceManager)
        << R"(", "end": ")" << LocationString(exp->getLocEnd(), sourceManager)
        << R"(", "name": ")" << exp->getNameInfo().getAsString()
        << R"(", "decl_location": ")" << LocationString(exp->getDecl()->getLocStart(), sourceManager)
        << R"(", "type": ")" << exp->getType().getAsString() << "\"}" << endl;
    return true;
  }

  SourceManager &sourceManager;
};
}