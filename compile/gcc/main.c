/*cheat on gnu c extensions both on gcc and libc (called glibc in its gnu version)

non gnu specific  features (ex: ansi c, posix) will not be put here.
the latest stable version of those standards will be considered.

you can disable all non gnu specific languages features with flags like `-ansi or -std=c99`

this will not however stop defining certain GNU specific preprocessor macros such as `__GNUC__`

obviously, it is always better if you avoid using those features,
but you may encounter them in linux specific projects, such as the linux kernel itself for example.

gnu extensions have a large chance of being implemented
in future ansi c versions (but sometimes in a modified form)
because of the large influence of gcc.
*/

/*
#glibc

    name for the gnu implementation of the c standard library

    the standards only specifies the interface, not exact implementation of compiler and algorithms

    glibc is one of important implementation

    stdlib does not come with gcc: you could in theory choose between different implementations.

    - ubuntu

        - headers for glibc are on `/usr/include`. do `locate /stdio.h`

        - lib for glibc are on `/usr/lib/i386-linux-gnu`. do `locate /libc.a`

        - the ubuntu package is called `libc6-dev`. `dpkg -l | grep libc`

    - docs

        <http://www.gnu.org/software/libc/manual/html_mono/libc.html>
*/

#include <assert.h>
#include <complex.h>    //complex integer types
#include <math.h>
#include <stdarg.h>    //..., va_list, va_start, va_arg, va_end
#include <stdio.h>
#include <stdlib.h>

int nested()
{
    return 0;
}

//#__attribute__

    char not_aligned16 = 0;
    char aligned16 __attribute__ ((aligned (16))) = 0;

    int sprintf_wrapper( char *s, int useless, const char *fmt, int useless2, ...)
    {
        int ret;
        va_list args;

        va_start( args, useless2 );
        ret = vsprintf( s, fmt, args );
        va_end( args );
        return ret;
    }

    //format

        /*
        3 says: the 3rd argument is the format string
        5 says: the va_list starts at the 5th argument

        Declaration and definition *must* be separated.
        */
        int sprintf_wrapper_attr( char *s, int useless, const char *fmt, int useless2, ... ) __attribute__((format(printf, 3, 5)));

        int sprintf_wrapper_attr( char *s, int useless, const char *fmt, int useless2, ... )
        {
            int ret;
            va_list args;

            va_start( args, useless2 );
            ret = vsprintf( s, fmt, args );
            va_end( args );
            return ret;
        }

    /*
    #noreturn

        It is possible that the function makes the program exit and therefore does not return.

        Makes compiler ommit "possible no return" warnings.

        Used on glibc exit and abort:

            extern void exit(int)   __attribute__((noreturn));
            extern void abort(void) __attribute__((noreturn));
    */

        void exitnow()
        {
            exit( EXIT_SUCCESS );
        }

        //warning: control reaches end of non void function
        /*
        int noreturn_possible( int n )
        {
            if ( n > 0 )
                exitnow();
            else
                return 0;
        }
        */

        void exitnow_attr() __attribute__((noreturn));

        void exitnow_attr()
        {
            exit( EXIT_SUCCESS );
        }

        int noreturn_possible_attr( int n )
        {
            if ( n > 0 )
                exitnow_attr();
            else
                return 0;
        }

        /*
        Does not emmit a warning because the libc exit has the `noreturn` attribute.
        */
        int noreturn_possible_exit( int n )
        {
            if ( n > 0 )
                exit(EXIT_SUCCESS);
            else
                return 0;
        }

    //const

        int next( int cur )
        {
            return cur + 1;
        }

        int next_const( int cur ) __attribute__((const));

        int next_const( int cur )
        {
            return cur + 1;
        }

int main( int argc, char** argv )
{

    /*
    #binary int literals

        start with `0b`:
    */
    {
        assert( 0b10000 == 16 );
    }

    /*
    #complex integer

        C99 has floating point complex numbers, but no integer complex numbers
    */
    {
        int complex z = 1 + 1*I;
        int complex z2 = 1 - 1*I;
        assert( z + z2 == 2  );
    }

    /*
    #nested function

         is a function defined inside another function (the outter function here is `main`)

         this is not a redefinition because it is inside a parenthesis block
         much like a variable definition inside a parenthesis block
    */

        int nested()
        {
            return 1;
        }

        {

            int nested()
            {
                return 2;
            }

            //this would cause a redefinition error:

                //int nested()
                //{
                //    return 2;
                //}

            //like variable redefinitions, the nested version overrides all external version
            //which have become completelly innacessible

            assert( nested() == 2 );
        }

    /*
    #preprocessor defines

        only gnu extensions are considered here

        full list: <http://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html#Common-Predefined-Macros>

        view all macros that would be automatically defined:

            cpp -dM /dev/null

        TODO there are some missing! where is `__i386__` documented for example?
    */

    /*
    automatically defined on gcc even if `-std=cXX -pedantic-erors`:

    contains major version number
    */

#ifdef __GNUC__
        printf( "__GUNC__ = %d\n", __GNUC__ );
#endif

        /*
        automatically defined if the compiler is told to use strict ansi c features and no extensions
        this is triggered by options such as `-std=c99` or `-ansi`

        don't be surprised if this does not appear when compiling this file
        since strict ansi compliance would mean other features of this file would need
        to be broken such as nested functions
        */

#ifdef __STRICT_ANSI__
        puts( "__STRICT_ANSI__" );
#endif

        /*
        gcc defines architecture macros TODO where?

        you can find a list of those macros: <http://sourceforge.net/p/predef/wiki/Architectures/>
        */

#ifdef __i386__
        puts( "__i386__" );
#endif

        /*
        gcc defines OS macros TODO where?
        */

#ifdef __linux__
        puts( "__linux__" );
#endif

    /*
    #__attribute__

        Specifies special attributes of functions or data.

        There are three types of attributes:

        - functions: http://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
        - variables: http://gcc.gnu.org/onlinedocs/gcc/Variable-Attributes.html#Variable-Attributes
        - types: http://gcc.gnu.org/onlinedocs/gcc/Type-Attributes.html#Type-Attributes

        Can be used to:

        - do error checking at compile time that would not be possible otherwise:

            - format
            - noreturn

        - control certain aspects of low level assembly code output.

            For example, `aligned` controls data alignment on the text section.

        - help the compiler optimize by giving it extra information:

            - const
            - hot

    */
    {
        /*
        #format

            If this is used, gcc can check if printf format strings are correct because of the use of attributes,
            and emmit errors otherwise.
        */
        {
            char s[32];
            sprintf_wrapper( s, 0, "%c", 0, 'a' );
            assert( s[0] == 'a' );

            sprintf_wrapper_attr( s, 0, "%c", 0, 'b' );
            assert( s[0] == 'b' );

            /*
            With `__attribute__((format,X,Y))` the compile time error checking gets done.
            */
            {
                //compile error check not done
                //could segfault at runtime
                if ( 0 )
                {
                    sprintf_wrapper( s, 0, "%s", 0 );
                }

                //compile error check is done
                {
                    //sprintf_wrapper_attr( s, 0, "%s", 0 );
                }
            }
        }

        /*
        #aligned

            Aligns variables on X bit lines.

            This may be required for certain processor specific functions.

            The generated gas assembly code should mark this alignment with the `.align` directive.
        */
        {
            assert( aligned16 == 0 );
            assert( not_aligned16 == 0 );
        }

        /*
        #const

            A function marked const may be optimized in the sense that the compiler calculates its value at compile time,
            or chaches its result of each calculation.

            A function can only be marked const if:

            - its return value is only a function of its arguments, and not of any global or static function variable
            - the function has no desired side effect besides returning the value

            Marking a function which does one of the above const will lead to serious hard to find bugs.
        */
        {
            assert( next( 0 ) == 1 );
            assert( next( 0 ) == 1 );
            assert( next_const( 0 ) == 1 );
            assert( next_const( 0 ) == 1 );
        }

        /*
        #section
        */
        {
        }
    }

    /*
    #inline assembly

        #sources

            - great intro: <http://www.ibm.com/developerworks/library/l-ia/index.html>

        can be used if you really, really want to optimize at the cost of:

        - architecture dependance
        - tying you to gcc

        if you use this, do it like the linux kernel and separate different architecture
        code in different dirs.

        general syntax:

            asm (
                "movl %1, %%eax;"   //commands string
                "movl %%eax, %0;"
                : "=X" (y),   //outputs
                  "=X" (z)
                : "X" (x)    //inputs
                : "X" (x)
                : "%eax"     //clobbered registers
            );

        where:

        - commands: actual gas code into a single c string. Remember: each command has to end in newline or `;`.
        - outputs: start with `=`. gcc has to enforce is that at the end of the `asm` block that those values are set.
        - inputs:
        - clobbered registers:

            registers that may be modified explicitly in the assembly code.

            normally, users have no direct access to registers,
            so gcc is free to optimize by leaving values in those registers for later use.

            this tells gcc not to leave values in the listed since those may be modified.

            ex:

                mov $0, %eax

            clearly clobbers eax, so you would need to list eax in the clobber list.

            Note that certain instructions clobber registers
            even if they are not explicitly written in the code.

        both inputs and outputs are constrats. `X` will indicate the constraint type

        TODO0 #__asm__ vs asm
        TODO0 #asmlinkage
    */

        /*
        examples of inline assembly in i386

        this is the main place for contains more comments and explanations
        if other archs are also exemplified
        */

#ifdef __i386__

        /*
        #m constraint

            instructs gcc to store keep value of given expressions into RAM

            this is the most basic way to get/set values of c variables in assembly code
        */
        {
            int in = 1;
            int out = 0;
            //out = in
            asm (
                "movl %1, %%eax;"
                "movl %%eax, %0"
                : "=m" (out)
                : "m" (in)
                : "%eax"      /* eax will be modified, so we have to list it in the clobber list */
            );
            assert( out == 1 );
        }

        //no input
        {
            int out = 0;
            //out = 1
            asm (
                "movl $1, %0"
                : "=m" (out)
            );
            assert( out == 1 );
        }

        /* simple exaple using floats */
        {
            float in = 1.0;
            float out = 0.0;
            //out = -in
            asm (
                "flds %1;"
                "fchs;"
                "fstps %0;"
                : "=m" (out)
                : "m" (in)
            );
            assert( out == -1.0 );
        }

        /* input and ouput can be the same memory location */
        {
            float x = 1.0;
            //x = -x
            asm (
                "flds %1;"
                "fchs;"
                "fstps %0;"
                : "=m" (x)
                : "m" (x)
            );
            assert( x == -1.0 );
        }

        /*
        #register constraints

            tell gcc to automatically read memory into registers or write registers into memory

            this is more precise and complicated than using `m`

            - r: gcc chooses any free register
            - a: %eax
            - b: %ebx
            - c: %ecx
            - d: %edx
            - S: %esi
            - D: %edi
            - 0: matching register
        */

        /*
        r example

        gcc will automatically put the value of `in` from ram into a register for us
        and `out` from a register into ram at the end

        note how we can do an `inc` operation directly on `%1` and `%0`
        so they must both already be inside a registers as expected

        gcc just makes sure they are written from/to memory before/after the operations
        */
        {
            int in = 0;
            int out = 0;
            //out = in + 2
            asm (
                "incl %1;"
                "movl %1, %0;"
                "incl %0;"
                : "=r" (out)
                : "r" (in)
            );
            assert( out == 2 );
        }

        /*
        #matching constraint

        are digits

        specifies that an input/output has the same constraint as another one

        often used when we want a single variable to be both input and output
        and minimize the use of new registers
        */
        {
            int x = 0;
            asm (
                    "incl %0"
                    : "=r" (x)
                    : "0" (x) /* x has the same constraint
                                 as constraint 0 (`r`)*/
            );
            assert( x == 1 );
        }

        /*
        #specific register constraints

        if you look at the generated assembly code,
        you will see that x was put into eax
        */
        {
            int x = 0;
            asm (
                    "incl %0"
                    : "=a" (x)
                    : "0" (x)
            );
            assert( x == 1 );
        }
#endif

    return EXIT_SUCCESS;
}
