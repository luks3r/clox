//
// Created by Sergei Lukaushkin on 17.06.2023.
//

#ifndef CLOX_DEBUG_H
#define CLOX_DEBUG_H

#include "chunk.h"

void disassembleChunk(Chunk *chunk, const char *name);

int32_t disassembleInstruction(Chunk *chunk, int32_t offset);

#endif //CLOX_DEBUG_H
