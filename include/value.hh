//
// Created by Sergei Lukaushkin on 17.06.2023.
//

#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include <cstdint>
#include <iostream>
#include "object.hh"
#include "memory.hh"

enum struct ValueType {
    BOOL,
    NIL,
    NUMBER,
    OBJECT,
};

class Value {
public:
    ValueType type;
    union {
        bool boolean;
        double number;
        Obj *obj;
    } as;

    explicit Value(bool boolean) : type(ValueType::BOOL), as({.boolean = boolean}) {}

    explicit Value(double number) : type(ValueType::NUMBER), as({.number = number}) {}

    explicit Value(Obj *obj) : type(ValueType::OBJECT), as({.obj = obj}) {}

    explicit Value(ObjString *obj) : type(ValueType::OBJECT), as({.obj = (Obj *) obj}) {}

    explicit Value() : type(ValueType::NIL), as({.number = 0}) {}


    constexpr bool isFalsey() const {
        return isNil() || (isBool() && !asBool());
    }

    constexpr bool is(ValueType type) const { return this->type == type; }

    constexpr bool isBool() const { return is(ValueType::BOOL); }

    constexpr bool isNil() const { return is(ValueType::NIL); }

    constexpr bool isNumber() const { return is(ValueType::NUMBER); }

    constexpr bool isObject() const { return is(ValueType::OBJECT); }

    constexpr bool isString() const { return isObjType(ObjectType::STRING); }

    Obj *asObject() const { return as.obj; }

    bool asBool() const { return as.boolean; }

    double asNumber() const { return as.number; }

    ObjString *asString() const { return (ObjString *) (asObject()); }

    const char *asCString() const { return ((ObjString *) asObject())->chars; }

    auto operator==(Value a) const {
        if (type != a.type) return false;
        switch (type) {
            case ValueType::BOOL:
                return asBool() == a.asBool();
            case ValueType::NIL:
                return true;
            case ValueType::NUMBER:
                return asNumber() == a.asNumber();
            case ValueType::OBJECT: {
                ObjString *aString = asString();
                ObjString *bString = a.asString();
                return aString->length == bString->length &&
                       memcmp(aString->chars, bString->chars, aString->length) == 0;
            }
            default:
                return false;
        }
    }

    void print() {
        switch (type) {
            case ValueType::BOOL:
                printf(asBool() ? "true" : "false");
                break;
            case ValueType::NIL:
                printf("nil");
                break;
            case ValueType::NUMBER:
                printf("%g", asNumber());
                break;
            case ValueType::OBJECT:
                printObject();
                break;
        }
    }

private:
    inline bool isObjType(ObjectType type) const {
        return isObject() && asObject()->type == type;
    }

    void printObject() const {
        switch (asObject()->type) {
            case ObjectType::STRING:
                std::cout << asString()->chars << std::endl;
                break;
        }
    }
};

struct ValueArray {
    int32_t capacity;
    int32_t count;
    Value *values;

    ValueArray() : values(nullptr), capacity(0), count(0) {}

    ~ValueArray() {
        reallocate(values, sizeof(Value) * capacity, 0);
    }

    void free() {
        reallocate(values, sizeof(Value) * capacity, 0);
        values = nullptr;
        capacity = 0;
        count = 0;
    }

    void write(Value value) {
        if (capacity < count + 1) {
            int32_t oldCapacity = capacity;
            capacity = GROW_CAPACITY(oldCapacity);
            values = GROW_ARRAY(Value, values, oldCapacity, capacity);
        }

        values[count] = value;
        count++;
    }
};

void initValueArray(ValueArray *array);

void writeValueArray(ValueArray *array, Value value);

void freeValueArray(ValueArray *array);

#endif //CLOX_VALUE_H
