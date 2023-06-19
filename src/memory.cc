//
// Created by Sergei Lukaushkin on 17.06.2023.
//

#include <cstdlib>
#include "memory.hh"

void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
    (void) oldSize; // TODO: remove and implement
    if (newSize == 0) {
        free(pointer);
        return nullptr;
    }

    void *result = realloc(pointer, newSize);
    if (result == nullptr) exit(1);
    return result;
}
