(bind "forLoop" forStmt
  (hasLoopInit
    (declStmt
      (hasSingleDecl
        (bind "initVarName" varDecl
          (hasInitializer (ignoringParens (integerLiteral (equals 0))))))))
  (hasIncrement
    (unaryOperator
      (hasOperatorName "++")
      (hasUnaryOperand
        (declRefExpr
          (to
            (bind "incVarName" varDecl (hasType (isInteger))))))))
  (hasCondition
    (ignoringParens
      (binaryOperator
        (hasOperatorName "<")
        (hasLHS
          (ignoringParenImpCasts
            (declRefExpr
              (to
                (bind "condVarName" varDecl (hasType (isInteger)))))))
        (hasRHS
          (expr (hasType (isInteger))))))))
