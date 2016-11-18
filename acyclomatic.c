/*============================================================================================
* This is an example of a program where the cyclomatic complexity of each function equals 1:
* - all loops are converted into recursive function calls,
* - all branching statements are replaced with arithmetic and logical calculations
*
* This program converts an arithmetical expression into the postfix form
*
* Expression should be composed of following characters:
*   alphabetical low-case characters 'a'-'z',
*   additive low-priority operations '+' and '-',
*   multiplicative high-priority operations '*' and '/',
*   round brackets '(' and ')'.
*
* Example:
*   a+(b+c*d)*e+f/g+h   ->   abcd*+e*+fg/+h+
*
* Copyright 2006-2016 Mikhail Durnev (mdurnev@gmail.com). Released under the GPLv3
============================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* function prototypes */
int  check_interval(int value, int low, int high);

void convert(char* str);
void handle_error(char* str);
void handle_end(char* str);
void handle_symbol(char* str);
void handle_add_sub(char* str);
void handle_mul_div(char* str);
void handle_open_bracket(char* str);
void handle_close_bracket(char* str);

void print_nothing(char c);
void print_char(char c);
void print_symbol(char c);

void push(char c, char** stack_top);
char pop(char** stack_top);
char pop_mul_div(char** stack_top);
void pop0(char** stack_top);

/* data types */
typedef void (*ACTION)(char* str);
typedef void (*PRINT_ACTION)(char str);


/* function arrays */

/* global call table */
ACTION call_table[] =
{
    handle_error,
    handle_end,
    handle_symbol,
    handle_add_sub,
    handle_mul_div,
    handle_open_bracket,
    handle_close_bracket
};

/* print call table */
PRINT_ACTION print_call_table[] =
{
    print_nothing,
    print_char
};


/* constants */
#define ASCII_MAX 127

/* following constants are indexes in global call table for corresponding functions */
#define HANDLE_ERROR 0
#define HANDLE_END 1
#define HANDLE_SYMBOL 2
#define HANDLE_ADD_SUB 3
#define HANDLE_MUL_DIV 4
#define HANDLE_OPEN_BRKT 5
#define HANDLE_CLOSE_BRKT 6

#define MAX_STACK_DEPTH 64

/* global variables */
char* stack_bottom = 0;
char* stack_top = 0;

/********************************************
* Program entry point
********************************************/
void main(int argc, char* argv[])
{
    /* get arithmetical expression */
    char* input_str = argv[argc - 1];

    /* display the expression */
    printf("INPUT:  %s\n", input_str);

    /* prepare display for output */
    printf("OUTPUT: ");

    /* initialize stack */
    stack_bottom = malloc(MAX_STACK_DEPTH);
    stack_top = stack_bottom;
    *stack_top = 0;

    /* start recursive conversion */
    convert(input_str);

    /* deallocate stack */
    free(stack_bottom);
}

/********************************************
* Recursive conversion routine
********************************************/
void convert(char* str)
{
    static int call_index; /* it is static to reduce stack usage */
    call_index = 0;

    /* all the checks below are mutually exclusive */

    /* check for end of string */
    call_index = check_interval(*str, '\0', '\0') * HANDLE_END;

    /* check for symbol a-z */
    call_index += check_interval(*str, 'a', 'z') * HANDLE_SYMBOL;

    /* check for arithmetical operations */
    call_index += check_interval(*str, '+', '+') * HANDLE_ADD_SUB;
    call_index += check_interval(*str, '-', '-') * HANDLE_ADD_SUB;
    call_index += check_interval(*str, '*', '*') * HANDLE_MUL_DIV;
    call_index += check_interval(*str, '/', '/') * HANDLE_MUL_DIV;

    /* check for brackets */
    call_index += check_interval(*str, '(', '(') * HANDLE_OPEN_BRKT;
    call_index += check_interval(*str, ')', ')') * HANDLE_CLOSE_BRKT;

    call_table[call_index](str);
}

/********************************************
* This function returns 1 if:
*  low <= value <= high
* Otherwise it returns 0
********************************************/
int  check_interval(int value, int low, int high)
{
    /* if value lies between low and high then                          */
    /* both high - value and value - low are greater than or equal to 0 */
    /* use | to mix them together                                       */
    /* then take inverted sign bit as result                            */
    return ~(((high - value) | (value - low)) >> (sizeof(int) * 8 - 1)) & 1;
}

/********************************************
* This function indicates errors in the
* input expression
********************************************/
void handle_error(char* str)
{
    printf("\nError in the expression\n");
}

/********************************************
* This function handles the end of
* input expression
********************************************/
void handle_end(char* str)
{
    /* stack may contain only up to 2 operations */
    /* and we should extract all of them if any  */
    print_symbol(pop(&stack_top)); /* extract and print *, /, + or - */
    print_symbol(pop(&stack_top)); /* extract and print + or - */

    printf("\n");
}

/********************************************
* This function handles a symbol a-z from
* input expression
********************************************/
void handle_symbol(char* str)
{
    /* display the symbol */
    printf("%c", *str);

    convert(++str);
}

/********************************************
* This function handles + or - from
* input expression
********************************************/
void handle_add_sub(char* str)
{
    /* stack may contain only up to 2 operations */
    /* and we should extract all of them if any  */
    print_symbol(pop(&stack_top)); /* extract and print *, /, + or - */
    print_symbol(pop(&stack_top)); /* extract and print + or - */

    /* then we have to push current operation */
    push(*str, &stack_top);

    convert(++str);
}

/********************************************
* This function handles * or / from
* input expression
********************************************/
void handle_mul_div(char* str)
{
    /* stack may contain only 1 operation or nothing */
    /* and we should extract it if available         */
    print_symbol(pop_mul_div(&stack_top)); /* extract and print * or / */

    /* then we have to push current operation */
    push(*str, &stack_top);

    convert(++str);
}

/********************************************
* This function handles open brackets from
* input expression
********************************************/
void handle_open_bracket(char* str)
{
    /* place delimiter 0 in the stack to separate the bracketed expression */
    push(0, &stack_top);

    /* convert the bracketed expression separately */
    convert(++str);
}

/********************************************
* This function handles close brackets from
* input expression
********************************************/
void handle_close_bracket(char* str)
{
    /* stack may contain only up to 2 operations */
    /* and we should extract all of them if any  */
    print_symbol(pop(&stack_top)); /* extract and print *, /, + or - */
    print_symbol(pop(&stack_top)); /* extract and print + or - */

    /* remove delimiter */
    pop0(&stack_top);

    /* continue convertion */
    convert(++str);
}

/********************************************
* Just a dummy function
********************************************/
void print_nothing(char c)
{
}

/********************************************
* Prints the symbol
********************************************/
void print_char(char c)
{
    printf("%c", c);
}

/********************************************
* Checks and prints the symbol
********************************************/
void print_symbol(char c)
{
    int call_index = check_interval(c, 1, ASCII_MAX);

    print_call_table[call_index](c);
}

/********************************************
* Pushes data into the stack
********************************************/
void push(char c, char** stack_top)
{
    ++(*stack_top);
    **stack_top = c;
}

/********************************************
* Pops data from the stack
********************************************/
char pop(char** stack_top)
{
    char c = **stack_top;

    /* do not change stack pointer if we are at the bottom, */
    /* i.e. if we found 0 */
    *stack_top -= (check_interval(c, 0, 0) ^ 1);

    return c;
}

/********************************************
* Pops * or / only
********************************************/
char pop_mul_div(char** stack_top)
{
    int cond = 0;

    char c = **stack_top;

    cond = check_interval(c, '*', '*') | check_interval(c, '/', '/');
    /* cond equals 1 if this is * or / */
    /* otherwise cond equals 0         */

    /* validate symbol multiplying it by cond (1 or 0) */
    c *= cond;

    /* use cond also to change stack pointer */
    *stack_top -= cond;

    return c;
}

/********************************************
* Pops 0 from the stack
* to go below the bottom
********************************************/
void pop0(char** stack_top)
{
    *stack_top -= check_interval(**stack_top, 0, 0);
}
