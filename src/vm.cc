//
// Created by Sergei Lukaushkin on 17.06.2023.
//

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "vm.hh"
#include "value.hh"
#include "config.hh"
#include "debug.hh"
#include "compiler.hh"
#include "memory.hh"

VM vm;

static void resetStack() {
    vm.stackTop = vm.stack;
}

void initVM() {
    resetStack();
}

void freeVM() {

}

InterpretResult interpret(const char *source) {
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk)) {
        freeChunk(&chunk);
        return InterpretResult::COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}

static void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

static Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

static Value peek(int32_t distance) {
    return vm.stackTop[-1 - distance];
}

static void concatenate() {
    ObjString *b = pop().asString();
    ObjString *a = pop().asString();

    int length = a->length + b->length;
    char *chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';
    ObjString *result = takeString(chars, length);
    push(Value(result));
}

static void runtimeError(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int32_t line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}

InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op)                          \
    do {                                                  \
        if (!peek(0).isNumber() || !peek(1).isNumber()) { \
            runtimeError("Operands must be numbers.");    \
            return InterpretResult::RUNTIME_ERROR;        \
        }                                                 \
        double b = pop().asNumber();                      \
        double a = pop().asNumber();                      \
        push(Value(a op b));                          \
    } while (0)

    for (;;) {
#if defined(DEBUG_TRACE_EXECUTION)
        printf("         ");
        for (Value *slot = vm.stack; slot < vm.stackTop; slot++) {
            printf("[ ");
            (*slot).print();
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (int32_t) (vm.ip - vm.chunk->code));
#endif
        OpCode instruction;
        switch (instruction = static_cast<OpCode>(READ_BYTE())) {
            case OpCode::RETURN:
                pop().print();
                printf("\n");
                return InterpretResult::OK;
            case OpCode::ADD:
                if (peek(0).isString() && peek(1).isString()) {
                    concatenate();
                } else if (peek(0).isNumber() && peek(1).isNumber()) {
                    double b = pop().asNumber();
                    double a = pop().asNumber();
                    push(Value(a + b));
                } else {
                    runtimeError("Operands must be two numbers or two strings.");
                    return InterpretResult::RUNTIME_ERROR;
                }
                break;
            case OpCode::SUBTRACT:
                BINARY_OP(-);
                break;
            case OpCode::MULTIPLY:
                BINARY_OP(*);
                break;
            case OpCode::DIVIDE:
                BINARY_OP(/);
                break;
            case OpCode::NEGATE:
                if (!peek(0).isNumber()) {
                    runtimeError("Operand must be a number.");
                    return InterpretResult::RUNTIME_ERROR;
                }
                push(Value(-pop().asNumber()));
                break;
            case OpCode::CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OpCode::NIL:
                push(Value());
                break;
            case OpCode::TRUE:
                push(Value(true));
                break;
            case OpCode::FALSE:
                push(Value(false));
                break;
            case OpCode::NOT:
                push(Value(pop().isFalsey()));
                break;
            case OpCode::EQUAL: {
                Value b = pop();
                Value a = pop();
                push(Value(a == b));
                break;
            }
            case OpCode::GREATER:
                BINARY_OP(>);
                break;
            case OpCode::LESS:
                BINARY_OP(<);
                break;
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

