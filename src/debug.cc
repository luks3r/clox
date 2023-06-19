//
// Created by Sergei Lukaushkin on 17.06.2023.
//

#include <cstdio>
#include "debug.hh"

int32_t simpleInstruction(const char *name, int32_t offset) {
    printf("%s\n", name);
    return offset + 1;
}

int32_t constantInstruction(const char *name, Chunk *chunk, int32_t offset) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    chunk->constants.values[constant].print();
    printf("'\n");
    return offset + 2;
}

void disassembleChunk(Chunk *chunk, const char *name) {
    printf("== %s ==\n", name);
    for (int32_t offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

int32_t disassembleInstruction(Chunk *chunk, int32_t offset) {
    printf("%04d ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk->lines[offset]);
    }
    auto instruction = static_cast<OpCode>(chunk->code[offset]);
    switch (instruction) {
        case OpCode::RETURN:
            return simpleInstruction("RETURN", offset);
        case OpCode::ADD:
            return simpleInstruction("ADD", offset);
        case OpCode::SUBTRACT:
            return simpleInstruction("SUBTRACT", offset);
        case OpCode::MULTIPLY:
            return simpleInstruction("MULTIPLY", offset);
        case OpCode::DIVIDE:
            return simpleInstruction("DIVIDE", offset);
        case OpCode::NEGATE:
            return simpleInstruction("NEGATE", offset);
        case OpCode::CONSTANT:
            return constantInstruction("CONSTANT", chunk, offset);
        case OpCode::NIL:
            return simpleInstruction("NIL", offset);
        case OpCode::TRUE:
            return simpleInstruction("TRUE", offset);
        case OpCode::FALSE:
            return simpleInstruction("FALSE", offset);
        case OpCode::NOT:
            return simpleInstruction("NOT", offset);
        case OpCode::EQUAL:
            return simpleInstruction("EQUAL", offset);
        case OpCode::GREATER:
            return simpleInstruction("GREATER", offset);
        case OpCode::LESS:
            return simpleInstruction("LESS", offset);
        default:
            printf("Unknown opcode %hhu\n", static_cast<uint8_t>(instruction));
            return offset + 1;
    }
}

