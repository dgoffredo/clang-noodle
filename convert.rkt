#lang racket

(define (ast-matcher->c++ expr)
  (define recur ast-matcher->c++)
  (match expr
    [(list 'bind (? string? name) (? symbol? matcher) args ...)
     (string-append (recur (cons matcher args)) ".bind(" (recur name) ")")]
  
    [(list (? symbol? matcher) arg args ...)
     (let ([args++
             (apply string-append
               (for/list ([term args])
                 (string-append ", " (recur term))))])
       (string-append (recur matcher) "(" (recur arg) args++ ")"))]
    
    [(list (? symbol? matcher))
     (string-append (recur matcher) "()")]
    
    [atom (~s atom)]))

(displayln (ast-matcher->c++ (read)))
