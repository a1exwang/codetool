/**sdfkjjjjj`j
 * This file comes from https://github.com/nsumner/clang-plugins-demo
 *
Original LICENSE:

Copyright (c) 2017 Nick Sumner

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject
to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

https://en.wikipedia.org/wiki/MIT_License
 */

#include "clang/Tooling/CompilationDatabase.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/raw_ostream.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"

#include <memory>
#include <string>
#include <unistd.h>

#include "codetool/find_usages_pair.hpp"

using namespace llvm;


static cl::OptionCategory printFunctionsCategory{"print-functions options"};

static cl::opt<std::string> databasePath{"p",
                                         cl::desc{"Path to compilation database"},
                                         cl::Optional,
                                         cl::cat{printFunctionsCategory}};

static cl::opt<std::string> directCompiler{cl::Positional,
                                           cl::desc{"[-- <compiler>"},
                                           cl::cat{printFunctionsCategory},
                                           cl::init("")};

static cl::list<std::string> directArgv{cl::ConsumeAfter,
                                        cl::desc{"<compiler arguments>...]"},
                                        cl::cat{printFunctionsCategory}};


class CommandLineCompilationDatabase : public clang::tooling::CompilationDatabase {
private:
  clang::tooling::CompileCommand compileCommand;
  std::string sourceFile;

public:
  CommandLineCompilationDatabase(llvm::StringRef sourceFile,
                                 std::vector<std::string> commandLine)
      : compileCommand(".", sourceFile, std::move(commandLine), "dummy.o"),
        sourceFile{sourceFile}
  { }

  std::vector<clang::tooling::CompileCommand>
  getCompileCommands(llvm::StringRef filePath) const override {
    if (filePath == sourceFile) {
      return {compileCommand};
    }
    return {};
  }

  std::vector<std::string>
  getAllFiles() const override {
    return {sourceFile};
  }

  std::vector<clang::tooling::CompileCommand>
  getAllCompileCommands() const override {
    return {compileCommand};
  }
};


std::unique_ptr<clang::tooling::CompilationDatabase>
createDBFromCommandLine(llvm::StringRef compiler,
                        llvm::ArrayRef<std::string> commandLine,
                        std::string &errors) {
  auto source = std::find(commandLine.begin(), commandLine.end(), "-c");
  if (source == commandLine.end() || ++source == commandLine.end()) {
    errors = "Command line must contain '-c <source file>'";
    return {};
  }
  llvm::SmallString<128> absolutePath(*source);
  llvm::sys::fs::make_absolute(absolutePath);

  std::vector<std::string> args;
  if (compiler.endswith("++")) {
    args.push_back("c++");
  } else {
    args.push_back("cc");
  }

  args.insert(args.end(), commandLine.begin(), commandLine.end());
  return std::make_unique<CommandLineCompilationDatabase>(absolutePath,
                                                          std::move(args));
}


static std::unique_ptr<clang::tooling::CompilationDatabase>
getCompilationDatabase(std::string &errors) {
  using Database = clang::tooling::CompilationDatabase;
  if (!directCompiler.empty()) {
    return createDBFromCommandLine(directCompiler, directArgv, errors);
  } else if (!databasePath.empty()) {
    return Database::autoDetectFromDirectory(databasePath, errors);
  } else {
    char buffer[256];
    if (!getcwd(buffer, 256)) {
      llvm::report_fatal_error("Unable to get current working directory.");
    }
    return Database::autoDetectFromDirectory(buffer, errors);
  }
}


static void
processFile(clang::tooling::CompilationDatabase const &database,
            std::string& file) {
  clang::tooling::ClangTool tool{database, file};
  tool.appendArgumentsAdjuster(clang::tooling::getClangStripOutputAdjuster());

  /**
   * NOTE: add default include path
   */
  auto f = [](const clang::tooling::CommandLineArguments &args, StringRef Filename) -> clang::tooling::CommandLineArguments {
    clang::tooling::CommandLineArguments ret = args;
    ret.push_back("-isystem");
    ret.push_back("/usr/lib/clang/7.0.0/include");
    return ret;
  };
  tool.appendArgumentsAdjuster(f);

  auto frontendFactory =
      clang::tooling::newFrontendActionFactory<codetool::FindUsagesAction>();
  tool.run(frontendFactory.get());
}


static void processDatabase(clang::tooling::CompilationDatabase const &database) {
  auto files = database.getAllFiles();
  for (auto &file : files) {
    processFile(database, file);
  }
}


int main(int argc, char const **argv) {
  sys::PrintStackTraceOnErrorSignal(argv[0]);
  llvm::PrettyStackTraceProgram X(argc, argv);
  llvm_shutdown_obj shutdown;

  cl::HideUnrelatedOptions(printFunctionsCategory);
  cl::ParseCommandLineOptions(argc, argv);

  std::string error;
  auto compilationDB = getCompilationDatabase(error);
  if (!compilationDB) {
    llvm::errs() << "Error while trying to load a compilation database:\n"
                 << error << "\n";
    return -1;
  }

  processDatabase(*compilationDB);

  return 0;
}

