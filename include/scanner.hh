//
// Created by Sergei Lukaushkin on 17.06.2023.
//

#ifndef CLOX_SCANNER_H
#define CLOX_SCANNER_H


enum struct TokenType : u_int32_t {
    // Single-character
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS,
    SEMICOLON, SLASH, STAR,

    // One-two character
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    AND, CLASS, ELSE, FALSE,
    FOR, FUN, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS,
    TRUE, VAR, WHILE,

    ERROR, TOKEN_EOF
};

struct Token {
    TokenType type;
    const char *start;
    int32_t length;
    int32_t line;
};

void initScanner(const char *source);

Token scanToken();

#endif //CLOX_SCANNER_H
