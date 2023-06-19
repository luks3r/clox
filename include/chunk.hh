//
// Created by Sergei Lukaushkin on 17.06.2023.
//

#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "value.hh"

enum struct OpCode : uint8_t {
    RETURN,
    NEGATE,
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    CONSTANT,
    NIL,
    TRUE,
    FALSE,
    NOT,
    EQUAL,
    GREATER,
    LESS,
};

struct Chunk {
    int32_t count;
    int32_t capacity;
    uint8_t *code;
    int32_t *lines;
    ValueArray constants;
};

void initChunk(Chunk *chunk);

void freeChunk(Chunk *chunk);

void writeChunk(Chunk *chunk, uint8_t byte, int32_t line);

int addConstant(Chunk *chunk, Value value);

#endif //CLOX_CHUNK_H
