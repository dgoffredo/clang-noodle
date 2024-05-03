#include "clang/Frontend/FrontendActions.h" // clang::SyntaxOnlyAction
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h" // llvm::cl::extrahelp
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::tooling;
using namespace llvm;

using namespace clang;
using namespace clang::ast_matchers;

/*
StatementMatcher LoopMatcher =
  forStmt(hasLoopInit(declStmt(hasSingleDecl(varDecl(
    hasInitializer(integerLiteral(equals(0)))))))).bind("forLoop");

(.bind "forLoop" forStmt
  (hasLoopInit
    (declStmt
      (hasSingleDecl
        (varDecl
          (hasInitializer
            (integerLiteral (equals 0))))))))
*/

StatementMatcher LoopMatcher =
    forStmt(hasLoopInit(declStmt(
                hasSingleDecl(varDecl(hasInitializer(integerLiteral(equals(0))))
                                  .bind("initVarName")))),
            hasIncrement(unaryOperator(
                hasOperatorName("++"),
                hasUnaryOperand(declRefExpr(
                    to(varDecl(hasType(isInteger())).bind("incVarName")))))),
            hasCondition(ignoringParenImpCasts(binaryOperator(
                hasOperatorName("<"),
                hasLHS(ignoringParenImpCasts(declRefExpr(
                    to(varDecl(hasType(isInteger())).bind("condVarName"))))),
                hasRHS(expr(hasType(isInteger()))))))).bind("forLoop");

/*
forStmt(hasLoopInit(declStmt(
            hasSingleDecl(varDecl(hasInitializer(integerLiteral(equals(0))))
                              .bind("initVarName")))),
        hasIncrement(unaryOperator(
            hasOperatorName("++"),
            hasUnaryOperand(declRefExpr(
                to(varDecl(hasType(isInteger())).bind("incVarName")))))),
        hasCondition(binaryOperator(
            hasOperatorName("<"),
            hasLHS(ignoringParenImpCasts(declRefExpr(
                to(varDecl(hasType(isInteger())).bind("condVarName"))))),
            hasRHS(expr(hasType(isInteger())))))).bind("forLoop");

(.bind "forLoop" forStmt
  (hasLoopInit
    (declStmt
      (hasSingleDecl
        (.bind "initVarName" varDecl
          (hasInitializer (integerLiteral (equals 0)))))))
  (hasIncrement
    (unaryOperator
      (hasOperatorName "++")
      (hasUnaryOperand
        (declRefExpr
          (to
            (.bind "incVarName" varDecl (hasType (isInteger))))))))
  (hasCondition
    (binaryOperator
      (hasOperatorName "<")
      (hasLHS
        (ignoringParenImpCasts
          (declRefExpr
            (to
              (.bind "condVarName" varDecl (hasType (isInteger)))))))
      (hasRHS
        (expr (hasType (isInteger)))))))
*/

class LoopPrinter : public MatchFinder::MatchCallback {
public :
  void run(const MatchFinder::MatchResult &Result) override {
    if (const ForStmt *FS = Result.Nodes.getNodeAs<clang::ForStmt>("forLoop"))
      FS->dumpColor();
  }
};

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    // Fail gracefully for unsupported options.
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser& OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  LoopPrinter Printer;
  MatchFinder Finder;
  Finder.addMatcher(LoopMatcher, &Printer);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}
