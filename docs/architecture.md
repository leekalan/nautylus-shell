# Architecture

## Overview

This project is a Unix-like functional programming shell implemented in C, focused on modularity and extensibility. The shell handles user input, parses commands, executes them (with support for scripting and external commands), manages environment variables, and handles job control features like foreground/background processes.

## Modules


| Module   | Description                     | Status  |
| ---------- | --------------------------------- | --------- |
| `main`   | Shell entry point and main loop | 🔲 Done |
| `input`  | Raw input processing            | 🔲 Done |
| `parser` | Command parsing                 | 🔲 Done |
| `exec`   | Command execution               | 🔲 Done |
| ...      | ...                             | ...     |

## Component Responsibilities

### `main.c`

- Sets up shell environment
- Enters main loop
- Handles exit/cleanup routines
- Manages global signal setup

### `input.c`

- Reads input from an iostream into a buffer
- Handles:
    - continuation lines, `\`
    - quotations
    - escaped characters

### `parser.c`

- Parses input into tokens
- Handles arguments and flags

### `exec.c`

- Executes commands
- Handles:
    - built-in commands
    - external commands
    - redirection
    - pipes
    - job control
