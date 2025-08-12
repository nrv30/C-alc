# cli(C)alc
* a cli-calculator in c

## About

* implementation of shunting yard algorithim for operator precedence
    * <https://www.youtube.com/watch?v=ceu-7gV1wd0&t=2512s>
    * Using array-based stack & queue
* supports basic operations ``+,-,*,/,()``
* support exponent with ``**``

Build Example: ``gcc -I ../include calc.c stack.c queue.c -o calc``