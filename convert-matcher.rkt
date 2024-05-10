#lang racket

(define (ast-matcher->c++ expr)
  (define recur ast-matcher->c++)
  (match expr
    [(list 'bind (? string? name) (? symbol? matcher) args ...)
     (string-append (recur (cons matcher args)) ".bind(" (recur name) ")")]

    [(list (? symbol? matcher) args ...)
      (string-append (recur matcher) "(" (string-join (map recur args) ", ") ")")]
    
    [atom (~s atom)]))

(displayln (ast-matcher->c++ (read)))
