//
// Created by Sergei Lukaushkin on 17.06.2023.
//

#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include <stdint.h>
#include "value.h"

typedef enum {
    OP_RETURN,
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_NOT,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
} OpCode;

typedef struct {
    int32_t count;
    int32_t capacity;
    uint8_t *code;
    int32_t *lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);

void freeChunk(Chunk *chunk);

void writeChunk(Chunk *chunk, uint8_t byte, int32_t line);

int addConstant(Chunk *chunk, Value value);

#endif //CLOX_CHUNK_H
