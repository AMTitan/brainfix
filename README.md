# Brainfix
BrainFix is a compiler/language that takes a C-style language (although the syntax slowly evolved to something very close to Javascript for some reason) and compiles this
into [BrainF\*ck](https://esolangs.org/wiki/Brainfuck), an esoteric programming language consisting of only 8 operations.

To run the resulting Brainf\*ck code, you can use any (third party) BrainF\*ck interpreter. However, this project includes its own interpreter (`bfint`) which supports different cell-sizes and random number generation, working nicely in tandem with the `bfx` compiler. Both `bfx` and `bfint` will be built by the included Makefile; see instructions below.

## Bisonc++ and Flexc++
The lexical scanner and parser that are used to parse the Brainfix language are generated by [Bisonc++](https://fbb-git.gitlab.io/bisoncpp/)
and [Flexc++](https://fbb-git.gitlab.io/flexcpp/) (not to be confused with bison++ and flex++). This software is readily available in the
Debian repositories, but not necessarily needed to build this project. The sourcefiles generated by these programs, based on the current grammar and lexer specifications, are included in the source-folder.

## Building
To build the project, simply cd into the root folder of this project and run make:

```
git clone https://github.com/jorenheit/brainfix
cd brainfix
make
```

To install the resulting binaries and standard library files to your PATH, run as root:
```
make install
```
By default, this will copy the binaries (bfx and bfint) to /usr/local/bin and the library files to /usr/include/bfx. These directories can be changed by editing the first couple of lines in the Makefile.

If for some reason you want to change the grammar specification, you can let bisonc++ and flexc++ regenerate the sourcecode for the scanner (file: lexer) and parser (file: grammar) by running

```
make regenerate
```

To remove all object files, run
```
make clean
```

To undo all actions performed by `make install`, run as root
```
make uninstall
```


## Usage
### Using the `bfx` executable
Building the project results will produce the compiler executable `bfx`. The syntax for invoking the compiler can be inspected by running `bfx -h`:

```
Usage: bfx [options] <target(.bfx)>
Options:
-h                  Display this text.
-t [Type]           Specify the number of bytes per BF-cell, where [Type] is one of
                    int8, int16 and int32 (int8 by default).
-I [path to folder] Specify additional include-path.
                      This option may appear multiple times to specify multiple folders.
-O0                 Do NOT do any constant expression evaluation.
-O1                 Do constant expression evaluation (default).
--random            Enable random number generation (generates the ?-symbol).
                      Your interpreter must support this extension!
--no-bcr            Disable break/continue/return statements for more compact output.
--no-multiple-inclusion-warning
                    Do not warn when a file is included more than once, or when files 
                      with duplicate names are included.
-o [file, stdout]   Specify the output stream/file (default stdout).

Example: bfx -o program.bf -O1 -I ~/my_bfx_project -t int16 program.bfx
```

### Using the `bfint` executable
To run the resulting BF, call the included interpreter or any other utility that was designed to run or compile BF. When using the included `bfint` interpreter, its syntax can be inspected by running `bfint -h`:

```
$ bfint -h
Usage: bfint [options] <target(.bf)>
Options:
-h                  Display this text.
-t [Type]           Specify the number of bytes per BF-cell, where [Type] is one of
                    int8, int16 and int32 (int8 by default).
-n [N]              Specify the number of cells (30,000 by default).
-o [file, stdout]   Specify the output stream (defaults to stdout).

--random            Enable Random Brainf*ck extension (support ?-symbol)
Example: bfint --random -t int16 -o output.txt program.bf
```

### The type of a BrainF\*ck cell
The type of the BF cell that is assumed during compilation with `bfx` can be specified using the `-t` option and will specify the size of the integers on the BF tape. By default, this is a single byte (8-bits). Other options are `int16` and `int32`. All generated BF-algorithms work with any of these architectures, so changing the type will not result in different BF-code. It will, however, allow the compiler to issue a warning if numbers are used throughout the program that exceed the maximum value of a cell. The same flag can be specified to `bfint`. This will change the size of the integers that the interpreter is operating on. For example, executing the `+` operation on a cell with value 255 will result in overflow (and wrap around to 0) when the interpreter is invoked with `-t int8` but not when it's invoked with `-t int16`. 

### Constant Evaluation (`O0` vs `O1`)
By default (option `-O1`), the compiler will perform as many calculations as it can at compile-time. For instance, consider the following expression:

```javascript
let n = 3 + 4;
```
The value 7 will be stored internally, but no BF-code will be generated because the value of `n` is not used to do anything. However, when the compiler is called with the `O0`-flag, every operation is translated directly to BF-code. In case of the example above, this will result in the following operations:

1. Create a cell containing the number 3.
2. Create a cell containing the number 4.
3. Perform the addition algorithm on these cells.

For obvious reasons, running in `O0`-mode will lead to significantly larger output. For example, the 'Hello World'-example below, when run in `O0`-mode, results in a total of approximately 12,000 BF-operations. In the default `O1`-mode, it's less than 1,200 operations.

### Example: Hello World
Every programming language tutorial starts with a "Hello, World!" program of some sort. This is no exception:

```javascript
// File: hello.bfx
include "std.bfx"

function main()
{
    println("Hello, World!");
}
```
#### Comments
The program starts with an end-of-line comment (C-comment blocks between `/*` and `*/` are also allowed) and then
includes the standard-library which is included with this project. This exposes (among other things, see below) some basic IO-facilities the sourcefile.

#### `main`
Next, the main-function is defined. Every valid BFX-program should contain a `main` function which takes no arguments and does not return anything. The order in which the functions are defined in a BFX-file does not matter; the compiler will always try to find main and use this as the entrypoint.

#### Compiling
In `main()`, the function `println()` from the IO library is called to print the argument and a newline to the console. The IO library is part of the standard library, which is located in the `std` folder, which we have to pass as the include-path.

Let's compile this example (assuming `bfx` and `bfint` are in your PATH):

```
$ bfx -o hello.bf -I std/ hello.bfx
$ bfint hello.bf
Hello, World!
```

## Target Architecture
The compiler targets the canonical BrainF*ck machine, where cells are unsigned integers of the type specified as the argument to the `-t` flag. At the start of the program, it is assumed that all cells are zero-initialized and the pointer is pointing at cell 0. Furthermore, it is assumed that the tape-size is, for all intents and purposes, infinitely long. The produced BF consists of only the 8 classic BF-commands and an optional RNG command, as per the [Random Brainfix extension](https://esolangs.org/wiki/Random_Brainfuck) (to allow for simple games).

| Command | Effect |
| --- | --- |
| `>` | Move pointer to the right. |
| `<` | Move pointer to the left. |
| `+` | Increase value pointed to by 1. |
| `-` | Decrease value pointed to by 1. |
| `[` | If current value is nonzero, continue. Otherwise, skip to matching `]` |
| `]` | If current value is zero, continue. Otherwise, go back to matching `[` |
| `.` | Output current byte to stdout |
| `,` | Read byte from stdin and store it in the current cell |
| `?` | Spawn a random number in the current cell (non-standard!) |


## Language
### Functions
A BrainFix program consists of a series of functions (one of which is called `main()`). Apart from global variable declarations, `const` declarations, user-defined types (structs) and file inclusion (more on those later), no other syntax is allowed at global scope. In other words: BF code is only generated in function-bodies.

A function without a return-value is defined like we saw in the 'Hello World' example and may take any number of parameters. For example:

```javascript
function foo(x, y, z)
{
    // body
}
```

When a function has a return-value, the syntax becomes:

```javascript
function ret = bar(x, y, z)
{
    // body --> must contain instantiation of 'ret' !
}
```

It does not matter where a function is defined with respect to the call:
```javascript
function foo()
{
    let x = 31;
    let y = 38;

    let nice = bar(x, y); // works, even if bar is defined below
}

function z = bar(x, y)
{
    let z = x + y; // return variable is instantiated here
}
```

#### Value and Reference Semantics
By default, all arguments are passed by value to a function: every argument is copied into the local scope of the function. Modifications to the arguments will therefore have no effect on the corresponding variables in the calling scope.

```javascript
function foo()
{
    let x = 2;
    bar(x);

    // x == 2, still
}

function bar(x)
{
    ++x;
}
```

However, BrainFix supports reference semantics as well. Parameters prefixed by `&` (like in C++) are passed by reference to the function. This will prevent the copy from taking place and will therefore be faster than passing it by value. However, it also introduces the possibility of subtle bugs, which is why it's not the default mode of operation.

```javascript

function foo()
{
    let x = 2;
    bar(x);

    // x == 3 now
}

function bar(&x)
{
    ++x;
}

```

#### Recursion
Unfortunately, recursion is not allowed in BrainFix. Most compilers implement function calls as jumps. However, this is not possible in BF code because there is no JMP instruction that allows us to jump to arbitrary places in the code. It should be possible in principle, but would be very hard to implement (and would probably require a lot more memory to accomodate the algorithms that could make it happen). Therefore, the compiler will throw an error when recursion is detected.

### Variable Declarations
New variables are declared using the `let` keyword and can from that point on only be accessed in the same scope; this includes the scope of `if`, `for` and `while` statements. At the declaration, the size (or type, see below) of the variable can be specified using square brackets. Variables declared without a size-specifier are allocated as having size 1. It's also possible to let the compiler deduce the size of the variable by adding empty brackets `[]` to the declaration. In this case, the variable must be initialized in the same statement in order for the compiler to know its size. After the declaration, only same-sized variables can be assigned to eachother, in which case the elements of the right-hand-side will be copied into the corresponding left-hand-side elements. There is one exception to this rule: an single value (size 1) can be assigned to an array as a means to initialize or refill the entire array with this value.

```javascript
function main()
{
    let x;                // size 1, not initialized
    let y = 2;            // size 1, initialized to 2
    let [10] array1;      // array of size 10, not initialized
    let [] str = "Hello"; // the size of str is deduced as 5 and initialized to "Hello"

    let [y] array2;       // ERROR: size of the array must be a number or const
    let [10] str2 = str;  // ERROR: size mismatch in assignment
    let [10] str3 = '0';  // OK: str3 is now a string of ten '0'-characters 
}
```

#### Initializing Variables
In the example above, we see how a string is used to initialize an array-variable. Other ways to initialize arrays all involve the `#` symbol to indicate an array-literal. In each of these cases, the size-specification can be empty, as the compiler is able to figure out the resulting size from its initializer.

```javascript
function main()
{
    let v1 = 1;
    let v2 = 2;
    let zVal = 42;

    let []x = #{v1, v2, 3, 4, 5}; // initializer-list
    let []y = #[5];               // 5 elements, all initialized to 0
    let []z = #[5, zVal];         // 5 elements, all initialized to 42

    let [zVal] arr;               // ERROR: size-specifier is not const
}
```
Size specifications must be known at compiletime; see the section on the `const` keyword below on how to define named compile-time constants.

#### Structs
In addition to declaring a variable by specifying its size, its type can be specified using the `struct` keyword and a previously defined struct-identifier. The definition of this `struct` must appear somewhere at global scope and can contain fields of any type, including arrays and other user defined types.

```javascript
struct Vec3
{
    x, y, z;
}; // semicolon is optional

struct Particle
{
    [struct Vec3] pos, [struct Vec3] vel; // nested structs
    id;                                   // fields specified over multiple lines
}; 

function print_vec3(&v)
{
    printc('(');
    printd(v.x); prints(", ");
    printd(v.y); prints(", ");
    printd(v.z); prints(")\n");
}

function main()
{
    let [struct Particle] p;

    p.id = 42;

    p.pos.x = 1;
    p.pos.y = 2;
    p.pos.z = 3;

    p.vel = Vec3{4, 5, 6};  // initializing using an anonymous Vec3 instance

    print_vec3(p.pos);
    print_vec3(p.vel);
}
```

#### Numbers
Only positive integers are supported; the compiler will throw an error on the use of the unary minus sign. A warning is issued when the program contains numbers that exceed the range of the specified type (e.g. 255 for the default `int8` type).

#### Indexing
Once a variable is declared as an array, it can be indexed using the familiar index-operator `[]`. Elements can be both accessed and changed using this operator. When the index to the array can be resolved at compile-time, the compiler will check if it is within bounds. Otherwise, it's up to the programmer to make sure the index is within the size of the indexed variable.

```javascript
function main()
{
    let [] arr = #(42, 69, 123);

    ++arr[0];     // 42  -> 43
    --arr[1];     // 69  -> 68
    arr[2] = 0;   // 123 -> 0

    arr[5] = 'x'; // Error will be reported!

    let n = scand();
    arr[n] = 'y'; // Anything may happen ...
}
```

#### Passing array-elements by reference
Operating on arrays using the index-operator usually works as expected. However, when an array-element is accessed through the index-operator (without operating on it) and passed to a function, the result of this expression might be temporary copy of the actual element, depending on whether the index was resolved at compile-time. If it was, the behaviour is as expected and an actual reference to the indexed element will be passed to the function. If however, the index cannot be resolved at compile-time, a temporary value containing a **copy** of the element is returned by the index operator. This is because the position of the BF-pointer has to be known at all times, even when the index is a runtime variable (for example determined by user-input). This leads to different semantics in both cases, which could be confusing. Consider the following example to illustrate the two cases:

```javascript
function modify(&x)
{
    ++x;
}

function main()
{
    let [5] arr = #{1, 2, 3, 4, 5};

    let n = 3;         // known at compiletime
    let m = scand();   // known at runtime

    ++arr[m];          // Works, even though m is a runtime variable

    modify(arr[n]);    // Fine: modified the 3rd element in-place
    modify(arr[m]);    // Fail: did not modify arr at all
}
```

To be safe, one should always pass the array and its index as seperate arguments to an element-modifying function, unless you're sure that it will only be called in a constant evaluation context. For example:

```javascript
function modify(&arr, &idx)
{
    ++arr[idx];
}

function main()
{
    let [5] arr = #{1, 2, 3, 4, 5};

    let m = scand();   // known at runtime
    modify(arr, m);    // works now
}
```

#### `sizeof()`
The `sizeof()` operator (it's not really a function, as it's a compiler intrinsic and not defined in terms of the BrainFix language itself) returns the size of a variable and can be used, for example, to loop over an array (more on control-flow in the relevant sections below). 

```javascript
function looper(arr)
{
    for (let i = 0; i != sizeof(arr); ++i)
        printd(arr[i]);
}
```

#### Constants
BrainFix provides a simple way to define constants in your program, using the `const` keyword. `const` declarations can only appear at global scope. Throughout the program, occurrences of the variable are replaced at compiletime by the literal value they've been assigned. This means that `const` variables can be used as array-sizes (which is their most common usecase):

```javascript
const SIZE = 10;

function main()
{
    let [] arr1 = #[SIZE, 42]; 
    let [] arr2 = #[SIZE, 69];

    arr1 = arr2; // guaranteed to work, sizes will always match
}
```

#### Global Variables
It's also possible to define runtime variables at global scope, using the `global` keyword. A global variable cannot be initialized in this declaration, so it's common to define an initializing function that's called from `main`. 

```javascript
include "std.bfx"

global x, y;      // single global variables
global [5]vec;    // global array of 5 cells

struct S
{
    x, y;
};

global [struct S] s;

function init()
{
    x = 42;
    y = 69;
    s = S{10, 20};
}

function main()
{
    init();
    
    printd(x);   endl();
    printd(y);   endl();
    printd(s.x); endl();
    printd(s.y); endl();
}
```

### Operators
The following operators are supported by BrainFix:

| Operator | Description |
| --- | --- |
| `++`  |  post- and pre-increment |
| `--`   |  post- and pre-decrement |
| `+`    |  add |
| `-`    |  subtract |
| `*`    |  multiply |
| `/`    |  divide |
| `%`    |  modulo |
| `^`    |  power |
| `+=`   |  add to left-hand-side (lhs), returns lhs |
| `-=`   |  subtract from lhs, returns lhs |
| `*=`   |  multiply lhs by rhs, returns lhs |
| `/=`   |  divide lhs by rhs, returns lhs |
| `%=`   |  calculate the remainder of (lhs / rhs) and assign it to lhs |
| `/=%`  |  divide lhs by rhs; returns the remainder |
| `%=/`  |  assign the remainder to lhs; returns the result of the division |
| `^=`   |  raises lhs to power rhs; returns the result |
| `&&`   |  logical AND |
| `\|\|` |  logical OR |
| `!`    |  (unary) logical NOT |
| `==`   |  equal to |
| `!=`   |  not equal to |
| `<`    |  less than |
| `>`    |  greater than |
| `<=`   |  less than or equal to |
| `>=`   |  greater than or equal to |

#### The div-mod and mod-div operators
Most of these operators are commonplace and use well known notation. The exception might be the div-mod and mod-div operators, which were added as a small optimizing feature. The BF-algorithm that is implemented to execute a division, calculates the remainder in the process. These operators reflect this fact, and let you collect both results in a single operation.

```javascript
function divModExample()
{
    let x = 42;
    let y = 5;

    let z = (x /=% y);

    // x -> x / y (8) and
    // z -> x % y (2)
}

function modDivExample()
{
    let x = 42;
    let y = 5;

    let z = (x %=/ y);
	
    // x -> x % y (2) and
    // z -> x / y (8)
}
```

### Flow
There are 4 ways to control flow in a BrainFix-program: `if` (-`else`), `switch`, `for` and `while`. Each of these uses similar syntax as to what we're familiar with from other C-like programming languages. Below we will see an example for each of these statements.

#### C-style for-loops
The first kind of `for`-loop has the classic C-syntax we all know and love. You specify the initialization-expression (cannot currently be an empty expression), the loop-condition and the increment-statement:

```javascript
// Single statement, no curly braces necessary
for (let i = 0; i != 10; ++i)
    printd(i);

// Compound statement
for (let i = 0; i != 10; ++i)
{
    let j = i + 10;
    printd(j);
    endl();
}
```

#### Range-based for-loops
It is common to iterate over the elements of an array. In some cases, where you don't need the index of the elements as an accessible variable, you can specify the array you need to iterate over without using an index:

```javascript
let [] array = #{1, 2, 3, 4, 5};

for (let x: array)
{
    printd(x);
    endl();
}
```

```javascript
let [] array = #{1, 2, 3, 4, 5};

for (let &x: array)
    ++x;  // increment all elements in-place
```

The loop-variable can also be declared as a reference to modify the array-elements in-place. This only works for loops that will be unrolled (e.g. for arrays of size 20 or less).  

#### While-loops
The while-loop also has syntax familiar from other C-style languages. It needs no further introduction:

```javascript
let [] str = "Hello World";
let i = 0;
while (str[i] != 'W')
{
    printc(str[i++]);
}
endl();
```

#### If-statements
The if-syntax is again identical to that of C. It supports arbitrarily long if-else ladders:

```javascript
let x = scand();
if (x < 10)
    println("Small");
else if (x < 20)
    println("Medium");
else
{
    println("Large!")
}
```

#### Switch Statements
In BrainFix, a `switch` statement is simply a syntactic alternative to an `if-else` ladder. Most compiled languages like C and C++ will generate code that jumps to the appropriate case-label (which therefore has to be constant expression), which in many cases is faster than the equivalent `if-else` ladder. In BrainF*ck, this is difficult to implement due to the lack of jump-instructions.

Each label has to be followed by either a single or compound statement, of which only the body of the first match will be executed (it's not possible to 'fall through' cases). The `break` and `continue` control statements will be seen as local to the enclosing scope around the switch (like any old `if`-statement). A `break` statement is therefore not required in the body of a case and in fact will probably have different semantics compared to what you're used to. Beware! If you need to skip part of the switch-body, consider using `continue` instead. For more information on `break`, `continue` and `return`, see below.

```javascript
while (true)
{
    prints("Enter a number 0-3, or 9 to quit: ");
    let x = scand();
    switch (x)
    {
        case 0: println("Zero");
        case 1: println("One");
        case 2: println("Two");
        case 3: println("Three");
        case 9:
        {
            println("Quitting ...");
            break; // will break out of the while!
        }
        default: println("Not sure ...");
    }
}
```

Unlike in C, case labels in Brainfix do not have to be constant expressions, which allows us to write code like this (although it might not be recommended):

```javascript
let x = scand();
let y = scand();

switch (x)
{
    case y: println("Same");
    default: println("Different");
}
```    

#### Preventing Loop Unrolling with `for*` and `while*`
Within a runtime-evaluated loop, the compiler can't make any assumptions about the values of each of the variables, so it has to output BF-algorithms for each of the operations in the body of the loop. Because of this, loops generally yield great amounts of BF code. To reduce the size of the output, the compiler will by default try to unroll loops by evaluating the body and condition for as long as it can. When the loop can't be fully unrolled (because the stop-condition can only be known at runtime) or the number of iterations exceeds 20, it will fall back on generating code for executing the loop at runtime. 

However, loop-unrolling can take a long time, resulting in long compilation times for loops with large bodies or many iterations. Especially in the latter case, it can take some time before the compiler realizes it shouldn't unroll the loop at all (since it has to evaluate the body 20 times before coming to this conclusion). To indicate to the compiler that it shouldn't attempt to unroll the loop, we can use `for*` and `while*` instead. For range-based for-loops, the compiler can predetermine the number of iterations; this cannot be changed in the body of the loop. Therefore, it can decide beforehand whether or not to unroll the loop without any compilation time-penalty. Using `for*` will overrule the decision of the compiler and will force it to not unroll, but this will not result in faster compilation times.

```javascript
function main()
{
    // The compiler will try to unroll this, even though we can see
    // that it will take 100 iterations (> 50).
    for (let i = 0; i != 100; ++i)
    {
        printd(i);
        endl();
    }

    // Use for* instead: same resulting BF-code, faster compilation
    for* (let i = 0; i != 100; ++i)
    {
        printd(i);
        endl();
    }

    // Will not compile any faster, but won't unroll:
    let [] arr = #{1, 2, 3, 4};
    for* (let x: arr)
    {
        printd(x)
        endl();
    }

    // Will produce a warning: runtime-loops cannot be iterated by reference:
    for* (let &x: arr)
        ++x;
}
```


#### Break, Continue and Return
By default, the familiar `break`, `continue` and `return` statements are supported to control the flow of your program. In contrast to many other languages, these statements are supported in *any* context; not necessarily in loops.

##### `break`
Will force flow to break out of the enclosing scope (skipping all statements until the closing brace). When issued at function-scope-level, it will return from the function.

```javascript
for (let i = 0; i != 10; ++i)
{
    if (i == 5)
        break;

    printd(i);
}
// Prints "01234"
```

##### `continue`
Like `break`, a `continue` statement will skip all statements until the closing brace. However, at this point flow will restore and (if issued in a loop-context) the next iteration will be executed as normal. Again, if `continue` is used at function-scope-level, it will simply return from the function.

```javascript
for (let i = 0; i != 10; ++i)
{
    printd(i);
    if (i > 5)
        continue;

    printd(i);
}
// Prints "0011223344556789"
```

##### `return`
At any scope-level, return will immediately return from the enclosing function-scope. If the function returns a value, it needs to be declared (and initialized, probably) before this point. Beware that even though the return-value might have been declared at the scope of the return-statement, a return-value *must* be declared at function-scope.

```javascript
function x = get()
{
    let x;
    while (true)
    {
        // let x = scand(); --> x not declared at function-scope
        x = scand();
        if (x > 10)
            return;
    }
}
```
##### Compiler option: `--no-bcr`
Because BF does not support arbitrary jumps in code, the break, continue and return directives (bcr) have been implemented through a pair of flags that need to be checked continuously in order to determine whether a statement needs to be executed. Effectively, this means that every line of code will be wrapped in an if-statement. Especially when compiling without constant-evaluation (`-O0`) or when constant evaluation is not possible due to user-input dependencies, this will lead to a lot of code-bloat and therefore very large BF-output. The compiler-flag `--no-bcr` will disable support for break, continue and return; using these directives will in that case produce a compiler error.

### File Inclusion
The compiler accepts only 1 sourcefile, but the `include` keyword can be used to organize your code among different  files. Even though the inner workings are not exactly the same as the C-preprocessor, the semantics pretty much are. When an include directive is encountered, the lexical scanner is simply redirected to that file and continues scanning the original file when it has finished scanning the included one.

#### Duplicate and Circular Inclusions
Files, or files with the same filename, can only be included once. When a multiple inclusion (which could be circular) is detected, a warning is issued. Two files with the same filename, even when they are different files in different folders and with different contents, will be treated as the same file: only the file that was first encountered is parsed and a warning will be issued. This warning can be suppressed with the `--no-multiple-inclusion-warning` flag.

### Standard Library
The standard library provides some useful functions in two categories: IO and mathematics. Below is a list of provided functions that you can use to make your programs interactive and mathematical. Also, in the "stdbool.bfx" headerfile, the constants `true` and `false` are defined to `1` and `0` respectively.

#### IO
All functions below are defined in `BFX_INCLUDE/stdio.bfx`:

|     function     | description  |
| ---------------- | ------------ |
|   `printc(x)`	   | Print `x` as ASCII character |
|   `printd(x)`	   | Print `x` as decimal (at most 3 digits) |
|   `printd_4(x)`  | Print `x` as decimal (at most 4 digits) |
|   `prints(str)`  | Print string (stop at `\0` or end of the string) |
|   `println(str)` | Same as `prints()` but including newline |
|   `print_vec(v)` | Print formatted vector, including newline: `(v1, v2, v3, ..., vN)` |
|   `endl()`	   | Print a newline (same as `printc('\n')`) |
|   `scanc()`	   | Read a single byte from stdin |
|   `scand()`	   | Read at most 3 bytes from stdin and convert to decimal |
|   `scand_4()`	   | Read at most 4 bytes from stdin and convert to decimal |
|   `scans(buf)`   | Read string from stdin. `sizeof(buf)` determines maximum number of bytes to read |
|   `to_int(str)`  | Converts string to int (at most 3 digits) |
|   `to_int_4(str)`  | Converts string to int (at most 4 digits) |
|   `to_string(x)` | Converts int to string (at most 3 digits) |
|   `to_string_4(x)` | Converts int to string (at most 4 digits) |
|   `to_binary_str(x)` | Returns binary representation of `x` as a string |
|   `to_hex_str(x)` | Returns hexadecimal representation of `x` as a string |

##### Big Numbers
On the default architecture, where the cells are only 1 byte long, values can never grow beyond 255. It is therefore sufficient to assume that number will never grow beyond 3 digits. However, when the target architecture contains larger cells, the functions suffixed with `_4` can be used to extend some facilities to 4 digits. Printing and scanning even larger digits is also possible, but functions to this end are not provided by the standard library for the simple reason that these functions would be terribly slow and impractical.

#### Math
All functions below are defined in `BFX_INCLUDE/stdmath.bfx`:

|     function     | description  |
| ---------------- | ------------ |
|   `sqrt(x)`	   | Calculate the square root of x, rounded down |
|   `factorial(n)` | Calculate n! (overflows for n > 5) |
|   `min(x,y)`     | Returns the minimum of x and y |
|   `max(x,y)`     | Returns the maximum of x and y |
|   `to_binary_array(x)` | Returns 8-bit binary representation of x as array of 0's and 1's |
|   `from_binary_array(x)` | Takes a binary array and converts it back to 8-bit integer |
|   `bit8_or(x, y)` | Returns bitwise OR of `x` and `y` (8-bit) |
|   `bit8_and(x, y)` | Returns bitwise AND of `x` and `y` (8-bit) |
|   `bit8_xor(x, y)` | Returns bitwise XOR of `x` and `y` (8-bit) |
|   `bit8_shift_left(x, n)` | Shift the binary representation of `x` by `n` bits to the left. |
|   `bit8_shift_right(x, n)` | Shift the binary representation of `x` by `n` bits to the right. |
|   `rand()` | Generate random number (see below). |

Each of the functions above returns the resulting value: their arguments are never modified, even if they are taken by reference (for optimization purposes).


#### Pseudorandom Numbers
To generate random numbers, BrainFix makes use of the [Random Brainfix extension](https://esolangs.org/wiki/Random_Brainfuck), where a `?` symbol tells the interpreter to generate a random number. The way this random number is generated is therefore fully up to the interpreter itself and cannot even be seeded from within BrainFix. If the `rand()`-function is used in a program that is compiled without the `--random` option, a warning will be issued and compilation will proceed as normal. The included interpreter supports this extension (if the `--random` flag is passed in to `bfint`) and will generate a random number uniformly in the range of possible cell-values (0-255 in the single byte case). It is up to the programmer to manipulate the result of `rand()` into the desired range. A common way to do this is by using the modulo-operator as in the example below.


```javascript
include "std.bfx"

function main()
{
    for*(let i = 0; i != 20; ++i)
    {
        switch(rand() % 3)
        {
            case 0: println("ROCK!");
            case 1: println("PAPER!");
            case 2: println("SCISSORS!");
        }
    }
}

```


