//
// Created by Sergei Lukaushkin on 19.06.2023.
//

#include <libc.h>
#include "object.hh"
#include "memory.hh"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj *allocateObject(size_t size, ObjectType type) {
    Obj *object = static_cast<Obj *>(reallocate(nullptr, 0, size));
    object->type = type;
    return object;
}

static ObjString *allocateString(char *chars, int length) {
    ObjString *string = ALLOCATE_OBJ(ObjString, ObjectType::STRING);
    string->length = length;
    string->chars = chars;
    return string;
}

ObjString *copyString(const char *chars, int length) {
    char *heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(heapChars, length);
}

ObjString *takeString(char *chars, int length) {
    return allocateString(chars, length);
}
