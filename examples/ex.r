f <- function(x) x
g <- function(y) f(y)

test1 <- function() g(2)

ex2 <- function() {
	stop("!")
}

ex1 <- function() {
	ex2()
}

testEx1 <- function() {
	tryCatch(ex1(),
		error = function(c) "error",
		warning = function(c) "warning",
		message = function(c) "message"
	)
}

testEx2 <- function() {
	try(ex1())
}

dddFun <- function(x, ...) {
	dots <- list(...)
	print(dots)
}

dddOnlyFun <- function(...) {
	dots <- list(...)
	print(dots)
}

testDdd <- function() {
	dddFun(1, 2, 3, 4)
}

curry <-
	function(x1)
		function(x2)
			function(x3)
				function(x4)
					function(x5)
						function(x6)
							function(x7)
								function(x8)
									function(x9)
										function(x10)
											x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8 + x9 + x10

curry_eval <- function() curry(1)(2)(3)(4)(5)(6)(7)(8)(9)(10)