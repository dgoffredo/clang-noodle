#lang racket

; $ clang -Xclang -ast-dump=json -c -o /dev/null corpus/loop.cpp >corpus/loop.ast.json
; $ racket convert-json.rkt <corpus/loop.ast.json

(require json)

(define (ast-json->sexpr json)
  (define recur ast-json->sexpr)
  (match json
    [(hash-table ('kind kind) ((or 'name 'value 'opcode) name) ('inner kids) _ ...)
     (list* (string->symbol kind) name (map recur kids))]

    [(hash-table ('kind kind) ('inner kids) _ ...)
     (cons (string->symbol kind) (map recur kids))]
    
    [(hash-table ('kind kind) ((or 'name 'value 'opcode) name) _ ...)
     (list (string->symbol kind) name)]

    [(hash-table ('kind kind)
        ('referencedDecl (hash-table ('name name) _ ...)))
     (list (string->symbol kind) name)]

    [(hash-table ('kind kind) _ ...)
     (list (string->symbol kind))]
    
    [other
     (list 'unexpected (~s other))]))

(pretty-write (ast-json->sexpr (read-json)))
