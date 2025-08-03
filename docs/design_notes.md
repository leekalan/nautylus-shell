# Design Notes

## A Functional Shell

The shell will use a funcitonal framework, meaning that commands operate upon arguments and a stream. This will allow for more functionality without scripting and needing to write a large amount of code.

## Input

Input will be split off from the shell such that once the command is entered it will be passed as a string to the parser. The parser will then split the string into tokens and pass them to the executor.

### Testing

As input cannot reasonably be tested using unit tests, manual tests will be done to check that the input is correctly read in and behaviour such as backspace and escape sequences are handled correctly.
