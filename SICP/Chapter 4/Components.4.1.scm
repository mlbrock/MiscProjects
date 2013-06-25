; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Shims...
(define true #t)
(define false #f)
;
; Needed to avoid hiding of apply
(define apply-in-underlying-scheme apply)
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; eval
; Eval takes as arguments an expression and an environment. It classifies the
; expression and directs its evaluation. Eval is structured as a case analysis
; of the syntactic type of the expression to be evaluated. In order to keep
; the procedure general, we express the determination of the type of an
; expression abstractly, making no commitment to any particular representation
; for the various types of expressions. Each type of expression has a predicate
; that tests for it and an abstract means for selecting its parts. This
; abstract syntax makes it easy to see how we can change the syntax of the
; language by using the same evaluator, but with a different collection of
; syntax procedures.
;
;	o Primitive expressions
;		o For self-evaluating expressions, such as numbers, eval
;		  returns the expression itself.
;		o l Eval must look up variables in the environment to find
;		  their values.
;	o Special forms
;		o For quoted expressions, eval returns the expression that
;		  was quoted.
;		o An assignment to (or a definition of) a variable must
;		  recursively call eval to compute the new value to be
;		  associated with the variable. The environment must be
;		  modified to change (or create) the binding of the variable.
;		o An if expression requires special processing of its parts,
;		  so as to evaluate the consequent if the predicate is true,
;		  and otherwise to evaluate the alternative.
;		o A lambda expression must be transformed into an applicable
;		  procedure by packaging together the parameters and body
;		  specified by the lambda expression with the environment of
;		  the evaluation.
;		o A begin expression requires evaluating its sequence of
;		  expressions in the order in which they appear.
;		o A case analysis (cond) is transformed into a nest of if
;		  expressions and then evaluated.
;	o Combinations
;		o For a procedure application, eval must recursively evaluate
;		  the operator part and the operands of the combination. The
;		  resulting procedure and arguments are passed to apply, which
;		  handles the actual procedure application.
(define (eval exp env)
	(cond ((self-evaluating? exp) exp)
		((variable? exp) (lookup-variable-value exp env))
		((quoted? exp) (text-of-quotation exp))
		((assignment? exp) (eval-assignment exp env))
		((definition? exp) (eval-definition exp env))
		((if? exp) (eval-if exp env))
		((lambda? exp)
		 (make-procedure (lambda-parameters exp)
				(lambda-body exp)
				env))
		((begin? exp)
		 (eval-sequence (begin-actions exp) env))
		((cond? exp) (eval (cond->if exp) env))
		((application? exp)
		 (Xapply (eval (operator exp) env)
			(list-of-values (operands exp) env)))
		(else
		 (error "Unknown expression type -- EVAL" exp))))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; apply    
; Apply takes two arguments, a procedure and a list of arguments to which the
; procedure should be applied. Apply classifies procedures into two kinds: It
; calls apply-primitive-procedure to apply primitives; it applies compound
; procedures by sequentially evaluating the expressions that make up the body
; of the procedure. The environment for the evaluation of the body of a
; compound procedure is constructed by extending the base environment carried
; by the procedure to include a frame that binds the parameters of the
; procedure to the arguments to which the procedure is to be applied.
(define (Xapply procedure arguments)
	(cond ((primitive-procedure? procedure)
		(apply-primitive-procedure procedure arguments))
		((compound-procedure? procedure)
		 (eval-sequence
			(procedure-body procedure)
			(extend-environment
				(procedure-parameters procedure)
				arguments
				(procedure-environment procedure))))
			(else
			(error
			 "Unknown procedure type -- APPLY" procedure))))
;

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; list-of-values
; When eval processes a procedure application, it uses list-of-values to
; produce the list of arguments to which the procedure is to be applied.
; List-of-values takes as an argument the operands of the combination. It
; evaluates each operand and returns a list of the corresponding values
(define (list-of-values exps env)
	(if (no-operands? exps)
		'()
		(cons (eval (first-operand exps) env)
			(list-of-values (rest-operands exps) env))))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; eval-if
; Evaluates the predicate part of an if expression in the given environment.
; If the result is true, eval-if evaluates the consequent, otherwise it
; evaluates the alternative.
(define (eval-if exp env)
	(if (true? (eval (if-predicate exp) env))
		(eval (if-consequent exp) env)
		(eval (if-alternative exp) env)))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; eval-sequence
; Eval-sequence is used by apply to evaluate the sequence of expressions in a
; procedure body and by eval to evaluate the sequence of expressions in a begin
; expression. It takes as arguments a sequence of expressions and an
; environment, and evaluates the expressions in the order in which they occur.
; The value returned is the value of the final expression.
(define (eval-sequence exps env)
	(cond ((last-exp? exps) (eval (first-exp exps) env))
		(else (eval (first-exp exps) env)
			(eval-sequence (rest-exps exps) env))))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; eval-assignment
; Handles assignments to variables. It calls eval to find the value to be
; assigned and transmits the variable and the resulting value to
; set-variable-value! to be installed in the designated environment.
(define (eval-assignment exp env)
	(set-variable-value! (assignment-variable exp)
				(eval (assignment-value exp) env)
				env)
	'ok)

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; eval-definition
; Definitions of variables are handled in a similar manner to assignments
; above in eval-assignment.
(define (eval-definition exp env)
	(define-variable! (definition-variable exp)
			(eval (definition-value exp) env)
			env)
	'ok)

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; self-evaluating?
; The only self-evaluating items are numbers and strings.
(define (self-evaluating? exp)
	(cond ((number? exp) true)
		((string? exp) true)
		(else false)))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; variable?
; Variables are represented by symbols.
(define (variable? exp) (symbol? exp))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; quoted?
; Quotations have the form (quote <text-of-quotation>).
(define (quoted? exp)
	(tagged-list? exp 'quote))

(define (text-of-quotation exp) (cadr exp))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; tagged-list?
; Quoted? is defined in terms of the procedure tagged-list?, which identifies
; lists beginning with a designated symbol.
(define (tagged-list? exp tag)
	(if (pair? exp)
		(eq? (car exp) tag)
		false))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; assignment?
; Assignments have the form (set! <var> <value>).
(define (assignment? exp)
	(tagged-list? exp 'set!))

(define (assignment-variable exp) (cadr exp))

(define (assignment-value exp) (caddr exp))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; define
; Definitions have the form:
; (define <var> <value>)
; or the form:
; (define (<var> <parameter1> ... <parametern>)
; 	<body>)
; The latter form (standard procedure definition) is syntactic sugar for
; (define <var>
; 	(lambda (<parameter1> ... <parametern>)
; 		<body>))
; The corresponding syntax procedures are the following:
(define (definition? exp)
	(tagged-list? exp 'define))

(define (definition-variable exp)
	(if (symbol? (cadr exp))
		(cadr exp)
		(caadr exp)))

(define (definition-value exp)
	(if (symbol? (cadr exp))
		(caddr exp)
		(make-lambda (cdadr exp) ; formal parameters
			(cddr exp)))) ; body

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; lambda
; Lambda expressions are lists that begin with the symbol lambda.
(define (lambda? exp) (tagged-list? exp 'lambda))

(define (lambda-parameters exp) (cadr exp))

(define (lambda-body exp) (cddr exp))

; We also provide a constructor for lambda expressions, which is used by
; definition-value, above:

(define (make-lambda parameters body)
	(cons 'lambda (cons parameters body)))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; if?
; Conditionals begin with if and have a predicate, a consequent, and an
; (optional) alternative. If the expression has no alternative part, we
; provide false as the alternative.
(define (if? exp) (tagged-list? exp 'if))
(define (if-predicate exp) (cadr exp))
(define (if-consequent exp) (caddr exp))
(define (if-alternative exp)
	(if (not (null? (cdddr exp)))
		(cadddr exp)
		'false))
; We also provide a constructor for if expressions, to be used by cond->if to
; transform cond expressions into if expressions:
(define (make-if predicate consequent alternative)
	(list 'if predicate consequent alternative))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; begin
; Begin packages a sequence of expressions into a single expression. We include
; syntax operations on begin expressions to extract the actual sequence from
; the begin expression, as well as selectors that return the first expression
; and the rest of the expressions in the sequence.
(define (begin? exp) (tagged-list? exp 'begin))
(define (begin-actions exp) (cdr exp))
(define (last-exp? seq) (null? (cdr seq)))
(define (first-exp seq) (car seq))
(define (rest-exps seq) (cdr seq))

; We also include a constructor sequence->exp (for use by cond->if) that
; transforms a sequence into a single expression, using begin if necessary:
(define (sequence->exp seq)
	(cond ((null? seq) seq)
		((last-exp? seq) (first-exp seq))
		(else (make-begin seq))))
(define (make-begin seq) (cons 'begin seq))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Other compound expressions...
; A procedure application is any compound expression that is not one of the
; above expression types. The car of the expression is the operator, and the
; cdr is the list of operands:
(define (application? exp) (pair? exp))
(define (operator exp) (car exp))
(define (operands exp) (cdr exp))
(define (no-operands? ops) (null? ops))
(define (first-operand ops) (car ops))
(define (rest-operands ops) (cdr ops))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Derived expressions...
; Some special forms in our language can be defined in terms of expressions
; involving other special forms, rather than being implemented directly. One
; example is cond, which can be implemented as a nest of if expressions. For
; example, we can reduce the problem of evaluating the expression
; (cond ((> x 0) x)
; 	((= x 0) (display 'zero) 0)
; 	(else (- x)))
; to the problem of evaluating the following expression involving if and begin
; expressions:
; (if (> x 0)
; 	x
; 	(if (= x 0)
; 		(begin (display 'zero)
; 			0)
; 		(- x)))
; 
; Implementing the evaluation of cond in this way simplifies the evaluator
; because it reduces the number of special forms for which the evaluation
; process must be explicitly specified.
;
; We include syntax procedures that extract the parts of a cond expression,
; and a procedure cond->if that transforms cond expressions into if
; expressions. A case analysis begins with cond and has a list of
; predicate-action clauses. A clause is an else clause if its predicate is
; the symbol else.
(define (cond? exp) (tagged-list? exp 'cond))
(define (cond-clauses exp) (cdr exp))
(define (cond-else-clause? clause)
	(eq? (cond-predicate clause) 'else))
(define (cond-predicate clause) (car clause))
(define (cond-actions clause) (cdr clause))
(define (cond->if exp)
	(expand-clauses (cond-clauses exp)))

(define (expand-clauses clauses)
	(if (null? clauses)
		'false ; no else clause
		(let ((first (car clauses))
			(rest (cdr clauses)))
		(if (cond-else-clause? first)
			(if (null? rest)
				(sequence->exp (cond-actions first))
				(error "ELSE clause isn't last -- COND->IF"
					clauses))
			(make-if (cond-predicate first)
				(sequence->exp (cond-actions first))
				(expand-clauses rest))))))
; Expressions (such as cond) that we choose to implement as syntactic
; transformations are called derived expressions. Let expressions are also
; derived expressions (see exercise 4.6).

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 4.1.3 Evaluator Data Structures
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Testing of predicates
(define (true? x)
	(not (eq? x false)))
;
(define (false? x)
	(eq? x false))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Representing procedures
; To handle primitives, we assume that we have available the following
; procedures:
; 	(apply-primitive-procedure <proc> <args>)
; applies the given primitive procedure to the argument values in the list
; <args> and returns the result of the application.
; 	(primitive-procedure? <proc>)
; tests whether <proc> is a primitive procedure.
;
; These mechanisms for handling primitives are further described in section
; 4.1.4.
; Compound procedures are constructed from parameters, procedure bodies, and
; environments using the constructor make-procedure:
(define (make-procedure parameters body env)
	(list 'procedure parameters body env))
;
(define (compound-procedure? p)
	(tagged-list? p 'procedure))
;
(define (procedure-parameters p) (cadr p))
;
(define (procedure-body p) (caddr p))
;
(define (procedure-environment p) (cadddr p))

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Operations on Environments
; The evaluator needs operations for manipulating environments. As explained
; in section 3.2, an environment is a sequence of frames, where each frame is
; a table of bindings that associate variables with their corresponding values.
; We use the following operations for manipulating environments:
;	(lookup-variable-value <var> <env>)
; returns the value that is bound to the symbol <var> in the environment <env>,
; or signals an error if the variable is unbound.
;	(extend-environment <variables> <values> <base-env>)
; returns a new environment, consisting of a new frame in which the symbols in
; the list <variables> are bound to the corresponding elements in the list
; <values>, where the enclosing environment is the environment <base-env>.
;	(define-variable! <var> <value> <env>)
; adds to the first frame in the environment <env> a new binding that
; associates the variable <var> with the value <value>.
;	(set-variable-value! <var> <value> <env>)
; changes the binding of the variable <var> in the environment <env> so that
; the variable is now bound to the value <value>, or signals an error if the
; variable is unbound.
;
; To implement these operations we represent an environment as a list of
; frames. The enclosing environment of an environment is the cdr of the list.
; The empty environment is simply the empty list.
;
(define (enclosing-environment env) (cdr env))
(define (first-frame env) (car env))
(define the-empty-environment '())
;
; Each frame of an environment is represented as a pair of lists: a list of
; the variables bound in that frame and a list of the associated values.
(define (make-frame variables values)
	(cons variables values))
(define (frame-variables frame) (car frame))
(define (frame-values frame) (cdr frame))
(define (add-binding-to-frame! var val frame)
	(set-car! frame (cons var (car frame)))
	(set-cdr! frame (cons val (cdr frame))))
;
; To extend an environment by a new frame that associates variables with
; values, we make a frame consisting of the list of variables and the list of
; values, and we adjoin this to the environment. We signal an error if the
; number of variables does not match the number of values.
(define (extend-environment vars vals base-env)
	(if (= (length vars) (length vals))
		(cons (make-frame vars vals) base-env)
		(if (< (length vars) (length vals))
			(error "Too many arguments supplied" vars vals)
			(error "Too few arguments supplied" vars vals))))
; To look up a variable in an environment, we scan the list of variables in
; the first frame. If we find the desired variable, we return the corresponding
; element in the list of values. If we do not find the variable in the current
; frame, we search the enclosing environment, and so on. If we reach the empty
; environment, we signal an ``unbound variable'' error.
;
(define (lookup-variable-value var env)
	(define (env-loop env)
		(define (scan vars vals)
			(cond ((null? vars)
				(env-loop (enclosing-environment env)))
				((eq? var (car vars))
				(car vals))
				(else (scan (cdr vars) (cdr vals)))))
			(if (eq? env the-empty-environment)
				(error "Unbound variable" var)
				(let ((frame (first-frame env)))
					(scan (frame-variables frame)
						(frame-values frame)))))
			(env-loop env))
; To set a variable to a new value in a specified environment, we scan for
; the variable, just as in lookup-variable-value, and change the corresponding
; value when we find it.
(define (set-variable-value! var val env)
	(define (env-loop env)
		(define (scan vars vals)
			(cond ((null? vars)
				(env-loop (enclosing-environment env)))
				((eq? var (car vars))
				(set-car! vals val))
				(else (scan (cdr vars) (cdr vals)))))
			(if (eq? env the-empty-environment)
				(error "Unbound variable -- SET!" var)
				(let ((frame (first-frame env)))
					(scan (frame-variables frame)
						(frame-values frame)))))
			(env-loop env))
;
; To define a variable, we search the first frame for a binding for the
; variable, and change the binding if it exists (just as in
; set-variable-value!). If no such binding exists, we adjoin one to the
; first frame.
(define (define-variable! var val env)
	(let ((frame (first-frame env)))
		(define (scan vars vals)
			(cond ((null? vars)
				(add-binding-to-frame! var val frame))
				((eq? var (car vars))
				(set-car! vals val))
				(else (scan (cdr vars) (cdr vals)))))
			(scan (frame-variables frame)
				(frame-values frame))))
; The method described here is only one of many plausible ways to represent
; environments. Since we used data abstraction to isolate the rest of the
; evaluator from the detailed choice of representation, we could change the
; environment representation if we wanted to. (See exercise 4.11.) In a
; production-quality Lisp system, the speed of the evaluator's environment
; operations -- especially that of variable lookup -- has a major impact on
; the performance of the system. The representation described here, although
; conceptually simple, is not efficient and would not ordinarily be used in
; a production system.

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 4.1.4 Running the Evaluator as a Program
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define primitive-procedures
	(list (list 'car car)
		(list 'cdr cdr)
		(list 'cons cons)
		(list 'null? null?)
;        	<more primitives>
		(list '+ +)
		(list '- -)
;        	<add yet more primitives here>
		))
(define (primitive-procedure-names)
	(map car
		primitive-procedures))
(define (primitive-procedure-objects)
	(map (lambda (proc) (list 'primitive (cadr proc)))
		primitive-procedures))
; There must be a binding for each primitive procedure name, so that when eval
; evaluates the operator of an application of a primitive, it will find an
; object to pass to apply. We thus set up a global environment that associates
; unique objects with the names of the primitive procedures that can appear in
; the expressions we will be evaluating. The global environment also includes
; bindings for the symbols true and false, so that they can be used as
; variables in expressions to be evaluated.
(define (setup-environment)
	(let ((initial-env
		(extend-environment (primitive-procedure-names)
			(primitive-procedure-objects)
			the-empty-environment)))
		(define-variable! 'true true initial-env)
		(define-variable! 'false false initial-env)
		initial-env))
	(define the-global-environment (setup-environment))
;(display "--------------->")(display setup-environment)(newline)
; It does not matter how we represent the primitive procedure objects, so
; long as apply can identify and apply them by using the procedures
; primitive-procedure? and apply-primitiveprocedure.
; We have chosen to represent a primitive procedure as a list beginning with
; the symbol primitive and containing a procedure in the underlying Lisp that
; implements that primitive.
(define (primitive-procedure? proc)
	(tagged-list? proc 'primitive))
(define (primitive-implementation proc) (cadr proc))
; Setup-environment will get the primitive names and implementation procedures
; from a list:
; --- (define primitive-procedures
; --- 	(list (list 'car car)
; --- 		(list 'cdr cdr)
; --- 		(list 'cons cons)
; --- 		(list 'null? null?)
; --- ;        	<more primitives>
; --- 		))
; --- (define (primitive-procedure-names)
; --- 	(map car
; --- 		primitive-procedures))
; --- (define (primitive-procedure-objects)
; --- 	(map (lambda (proc) (list 'primitive (cadr proc)))
; --- 		primitive-procedures))
; To apply a primitive procedure, we simply apply the implementation procedure
; to the arguments, using the underlying Lisp system:
(define (apply-primitive-procedure proc args)
	(apply-in-underlying-scheme
	(primitive-implementation proc) args))
;
; For convenience in running the metacircular evaluator, we provide a driver
; loop that models the read-evalprint loop of the underlying Lisp system. It
; prints a prompt, reads an input expression, evaluates this expression in the
; global environment, and prints the result. We precede each printed result by
; an output prompt so as to distinguish the value of the expression from other
; output that may be printed.
(define input-prompt ";;; M-Eval input:")
(define output-prompt ";;; M-Eval value:")
(define (driver-loop)
	(prompt-for-input input-prompt)
	(let ((input (read)))
		(let ((output (eval input the-global-environment)))
			(announce-output output-prompt)
			(user-print output)))
	(driver-loop))
(define (prompt-for-input string)
	(newline) (newline) (display string) (newline))
(define (announce-output string)
	(newline) (display string) (newline))
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Can't use this with CodePad as its input is the text-box.
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; We use a special printing procedure, user-print, to avoid printing the
; environment part of a compound procedure, which may be a very long list
; (or may even contain cycles).
(define (user-print object)
	(if (compound-procedure? object)
		(display (list 'compound-procedure
			(procedure-parameters object)
			(procedure-body object)
			'<procedure-env>))
		(display object)))
; Now all we need to do to run the evaluator is to initialize the global
; environment and start the driver loop.
; Here is a sample interaction:
;(define the-global-environment (setup-environment))
;(driver-loop)
; --- ;;; M-Eval input:
; --- (define (append x y)
; --- (if (null? x)
; --- y
; --- (cons (car x)
; --- (append (cdr x) y))))
; --- ;;; M-Eval value:
; --- ok
; --- ;;; M-Eval input:
; --- (append '(a b c) '(d e f))
; --- ;;; M-Eval value:
; --- (a b c d e f)
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Some tests...
; -----------------------------------------------------------------------------
(define (EmitSep)
	(begin
		(display "---------------------------------------------------")
		(newline)
	)
)
; -----------------------------------------------------------------------------
; ---- self-evaluating?
(display "Simple terms are self-evaluating...")(newline)
(display "Number: ")(display (eval 42 '()))(newline)
(display "String: ")(display (eval "Hello world!" '()))(newline)
(EmitSep)
; -----------------------------------------------------------------------------
; --- make-frame
; --- frame-variables
; --- frame-values
(display "Printing a frame looks wrong, but isn't:")(newline)
(define MyTestFrame (make-frame '(a b c) '(1 2 3)))
(display "MyTestFrame       : ")(display MyTestFrame)
(display " --- looks like a list of four elements.")(newline)
(display "MyTestFrame/names : ")(display (frame-variables MyTestFrame))(newline)
(display "MyTestFrame/values: ")(display (frame-values MyTestFrame))(newline)
(EmitSep)
; -----------------------------------------------------------------------------
; --- add-binding-to-frame
(display "The frame we just constructed:")(newline)
(display "MyTestFrame/old   : ")(display MyTestFrame)(newline)
(display "Now going to add a binding to the frame for name 'x'...")(newline)
(add-binding-to-frame! 'x 42 MyTestFrame)
(display "MyTestFrame/new   : ")(display MyTestFrame)(newline)
; --- Construct an environment (a list of frames)...
(display "Now going to construct an environment from this frame, An ")
(display "environment is just a list of frames (order determines nesting).")
(newline)
(define MyTestEnv (cons MyTestFrame '()))
(display "MyTestEnv         : ")(display MyTestEnv)(newline)
(EmitSep)
; -----------------------------------------------------------------------------
; --- lookup-variable-value
(display "--- Now looking up c in the new environment... = ")
(display (lookup-variable-value 'c MyTestEnv))(newline)
(display "--- Now looking up x in the new environment... = ")
(display (lookup-variable-value 'x MyTestEnv))(newline)
(EmitSep)
; -----------------------------------------------------------------------------
; Create the environment we'll use subsequently...
(display "Now creating a real environment with built-ins and stuff...")(newline)
(define the-global-environment (setup-environment))
(display "ENV SETUP : ")(display setup-environment)(newline)
(display "ENV GLOBAL: ")(display the-global-environment)(newline)
(EmitSep)
; -----------------------------------------------------------------------------
(display "We can evaluate literal expressions in-line:")(newline)
(display "EVAL TEST 1: ")(display "(+ 4200 66600)")(newline)
(display "           : ")(display (eval '(+ 4200 66600) the-global-environment))(newline)
(EmitSep)
; -----------------------------------------------------------------------------
(display "Or we can use stored expression strings:")(newline)
(define input-0 '(+ 420 6660))
(display "EVAL TEST 2: ")(display "(+ 420 6660)")(newline)
(display "           : ")(display (eval input-0 the-global-environment))(newline)
(EmitSep)
; -----------------------------------------------------------------------------
(display "Or we can set variables in our environment with literals:")(newline)
(define input-0 '(define ZZZ 42))
(display "EVAL TEST 3: ")(display input-0)(newline)
(display "           : ")(display (eval input-0 the-global-environment))(newline)
(display "           : ")(display (eval 'ZZZ the-global-environment))(newline)
(EmitSep)
; -----------------------------------------------------------------------------
(display "And modify their values with expressions:")(newline)
(define input-0 '(define ZZZ (+ ZZZ 666)))
(display "EVAL TEST 4: ")(display input-0)(newline)
(display "           : ")(display (eval input-0 the-global-environment))(newline)
(display "           : ")(display (eval 'ZZZ the-global-environment))(newline)
(EmitSep)
; -----------------------------------------------------------------------------
; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(display "The End")(newline)

