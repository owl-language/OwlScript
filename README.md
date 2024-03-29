# Owlscript

With Lexical scoping, dynamic typing, first class functions, recursion, lambdas, and built in lists,
you'd be forgiven for assuming OwlScript is yet another lisp. Influenced by the ease and flexability of 
perl of and the spartan syntax of python, OwlScript is a language that's quick to learn and easy to use
- without sacraficing power.

Owlscript has a terse grammar, where variables are declared by use - no 'let' or 'var' keywords. 
'if' and 'loop' statements allow for easy control flow of Owlscript programs.

    def fib(m) {
        if (m < 2) {
            return 1;
        } 
        return fib(m-1) + fib(m-2);
    }
    i := 1;
    loop (i < 14) {
        print fib(i);
        i := i + 1;
    }

    
Lists are a built in data type in owlscript, with wrtie access at both ends, it can be used as a stack
or queue through the use of the push/pop/append statements, and indexed access is also supported.
