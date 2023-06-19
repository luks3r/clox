//
// Created by Sergei Lukaushkin on 17.06.2023.
//

#include <iostream>
#include <cstdlib>
#include <map>
#include "compiler.hh"
#include "scanner.hh"
#include "value.hh"
#include "config.hh"

#if defined(DEBUG_PRINT_CODE)

#include "debug.hh"

#endif

class Precedence {
public:
    enum Value : int32_t {
        NONE,
        ASSIGNMENT,
        OR,
        AND,
        EQUALITY,
        COMPARISON,
        TERM,
        FACTOR,
        UNARY,
        CALL,
        PRIMARY,
    };

    Precedence() = default;

    constexpr Precedence(Value predecence) : value(predecence) {}

    constexpr Precedence(int32_t predecence) : value(static_cast<Value>(predecence)) {}


    constexpr explicit operator Value() const { return value; }

    explicit operator bool() const = delete;

    constexpr bool operator==(Precedence a) const { return value == a.value; }

    constexpr bool operator!=(Precedence a) const { return value != a.value; }

    constexpr Precedence operator+(int32_t a) const { return value + a; }

    constexpr auto operator<=>(Precedence a) const {
        if (value < a.value) return -1;
        if (value > a.value) return 1;
        return 0;
    }

private:
    Value value;
};

typedef void (*ParseFn)();

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

typedef struct Parser {
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

Parser parser;
Chunk *compilingChunk;

static void unary();

static void binary();

static void expression();

static void advance();

static void number();

static void grouping();

static void literal();

static void string();

static void consume(TokenType type, const char *message);

template<typename T>
static void emitBytes(T byte);

template<typename T, typename... Args>
static void emitBytes(T byte, Args... bytes);

static void emitReturn();

static void emitConstant(Value value);

static void endCompiler();

static uint8_t makeConstant(Value value);

static ParseRule *getRule(TokenType type);

static void parsePrecedence(Precedence precedence);

static Chunk *currentChunk();

static void errorAt(Token *token, const char *message);

static void error(const char *message);

static void errorAtCurrent(const char *message);

std::unordered_map<TokenType, ParseRule> rules{
        {TokenType::LEFT_PAREN,    {grouping, nullptr, Precedence::NONE}},
        {TokenType::RIGHT_PAREN,   {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::LEFT_BRACE,    {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::RIGHT_BRACE,   {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::COMMA,         {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::DOT,           {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::MINUS,         {unary,    binary,  Precedence::TERM}},
        {TokenType::PLUS,          {nullptr,  binary,  Precedence::TERM}},
        {TokenType::SEMICOLON,     {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::SLASH,         {nullptr,  binary,  Precedence::FACTOR}},
        {TokenType::STAR,          {nullptr,  binary,  Precedence::FACTOR}},
        {TokenType::BANG,          {unary,    nullptr, Precedence::NONE}},
        {TokenType::BANG_EQUAL,    {nullptr,  binary,  Precedence::EQUALITY}},
        {TokenType::EQUAL,         {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::EQUAL_EQUAL,   {nullptr,  binary,  Precedence::EQUALITY}},
        {TokenType::GREATER,       {nullptr,  binary,  Precedence::COMPARISON}},
        {TokenType::GREATER_EQUAL, {nullptr,  binary,  Precedence::COMPARISON}},
        {TokenType::LESS,          {nullptr,  binary,  Precedence::COMPARISON}},
        {TokenType::LESS_EQUAL,    {nullptr,  binary,  Precedence::COMPARISON}},
        {TokenType::IDENTIFIER,    {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::STRING,        {string,   nullptr, Precedence::NONE}},
        {TokenType::NUMBER,        {number,   nullptr, Precedence::NONE}},
        {TokenType::AND,           {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::CLASS,         {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::ELSE,          {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::FALSE,         {literal,  nullptr, Precedence::NONE}},
        {TokenType::FOR,           {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::FUN,           {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::IF,            {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::NIL,           {literal,  nullptr, Precedence::NONE}},
        {TokenType::OR,            {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::PRINT,         {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::RETURN,        {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::SUPER,         {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::THIS,          {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::TRUE,          {literal,  nullptr, Precedence::NONE}},
        {TokenType::VAR,           {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::WHILE,         {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::ERROR,         {nullptr,  nullptr, Precedence::NONE}},
        {TokenType::TOKEN_EOF,     {nullptr,  nullptr, Precedence::NONE}},
};


static Chunk *currentChunk() {
    return compilingChunk;
}

static void errorAt(Token *token, const char *message) {
    if (parser.panicMode) return;
    parser.panicMode = true;
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TokenType::TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TokenType::ERROR) {
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}

static void error(const char *message) {
    errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char *message) {
    errorAt(&parser.current, message);
}

static void advance() {
    parser.previous = parser.current;

    for (;;) {
        parser.current = scanToken();
        if (parser.current.type != TokenType::ERROR) break;
        errorAtCurrent(parser.current.start);
    }
}

static void consume(TokenType type, const char *message) {
    if (parser.current.type == type) {
        advance();
        return;
    }
    errorAtCurrent(message);
}

static uint8_t makeConstant(Value value) {
    int32_t constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }
    return (uint8_t) constant;
}

template<typename T>
static void emitBytes(T byte) {
    writeChunk(currentChunk(), static_cast<uint8_t>(byte), parser.previous.line);
}

template<typename T, typename... Args>
static void emitBytes(T byte, Args... bytes) {
    writeChunk(currentChunk(), static_cast<uint8_t>(byte), parser.previous.line);
    emitBytes(bytes...);
}

static void emitReturn() {
    emitBytes(OpCode::RETURN);
}

static void emitConstant(Value value) {
    emitBytes(OpCode::CONSTANT, makeConstant(value));
}

static void endCompiler() {
    emitReturn();
#if defined(DEBUG_PRINT_CODE)
    if (!parser.hadError) {
        disassembleChunk(currentChunk(), "code");
    }
#endif
}

static void parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == nullptr) {
        error("Expect expression.");
        return;
    }

    prefixRule();

    while (precedence <= getRule(parser.current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }
}

static void expression() {
    parsePrecedence(Precedence::ASSIGNMENT);
}

static void grouping() {
    expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
}

static void number() {
    double value = strtod(parser.previous.start, nullptr);
    emitConstant(Value(value));
}

static void unary() {
    TokenType operatorType = parser.previous.type;
    parsePrecedence(Precedence::UNARY);

    switch (operatorType) {
        case TokenType::BANG:
            emitBytes(OpCode::NOT);
            break;
        case TokenType::MINUS:
            emitBytes(OpCode::NEGATE);
            break;
        default:
            return;
    }
}

static void literal() {
    switch (parser.previous.type) {
        case TokenType::FALSE:
            emitBytes(OpCode::FALSE);
            break;
        case TokenType::NIL:
            emitBytes(OpCode::NIL);
            break;
        case TokenType::TRUE:
            emitBytes(OpCode::TRUE);
            break;
        default:
            return;
    }
}

static void binary() {
    TokenType operatorType = parser.previous.type;
    ParseRule *rule = getRule(operatorType);
    parsePrecedence((Precedence) (rule->precedence + 1));
    switch (operatorType) {
        case TokenType::PLUS:
            emitBytes(OpCode::ADD);
            break;
        case TokenType::MINUS:
            emitBytes(OpCode::SUBTRACT);
            break;
        case TokenType::STAR:
            emitBytes(OpCode::MULTIPLY);
            break;
        case TokenType::SLASH:
            emitBytes(OpCode::DIVIDE);
            break;
        case TokenType::BANG_EQUAL:
            emitBytes(OpCode::EQUAL, OpCode::NOT);
            break;
        case TokenType::EQUAL_EQUAL:
            emitBytes(OpCode::EQUAL);
            break;
        case TokenType::GREATER:
            emitBytes(OpCode::GREATER);
            break;
        case TokenType::GREATER_EQUAL:
            emitBytes(OpCode::LESS, OpCode::NOT);
            break;
        case TokenType::LESS:
            emitBytes(OpCode::LESS);
            break;
        case TokenType::LESS_EQUAL:
            emitBytes(OpCode::GREATER, OpCode::NOT);
            break;
        default:
            return;
    }
}

static void string() {
    emitConstant(Value(copyString(parser.previous.start + 1, parser.previous.length - 2)));
}

static ParseRule *getRule(TokenType type) {
    return &rules[type];
}

bool compile(const char *source, Chunk *chunk) {
    initScanner(source);
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    expression();
    consume(TokenType::TOKEN_EOF, "Expect end of expression.");
    endCompiler();
    return !parser.hadError;
}
