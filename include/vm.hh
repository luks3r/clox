//
// Created by Sergei Lukaushkin on 17.06.2023.
//

#ifndef CLOX_VM_H
#define CLOX_VM_H

#include "chunk.hh"

#define STACK_MAX 256

struct VM {
    Chunk *chunk;
    uint8_t *ip;
    Value stack[STACK_MAX];
    Value *stackTop;
};

enum struct InterpretResult {
    OK,
    COMPILE_ERROR,
    RUNTIME_ERROR,
};

void initVM();

void freeVM();

InterpretResult interpret(const char *source);

InterpretResult run();

#endif //CLOX_VM_H
