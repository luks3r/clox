//
// Created by Sergei Lukaushkin on 19.06.2023.
//

#ifndef CLOX_OBJECT_H
#define CLOX_OBJECT_H

enum struct ObjectType {
    STRING,
};

struct Obj {
    ObjectType type;
};

struct ObjString {
    struct Obj obj;
    int length;
    char *chars;
};

struct ObjString *takeString(char *chars, int length);

struct ObjString *copyString(const char *chars, int length);

#endif //CLOX_OBJECT_H
