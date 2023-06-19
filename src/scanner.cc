//
// Created by Sergei Lukaushkin on 17.06.2023.
//

#include <cstring>
#include "scanner.hh"


struct Scanner {
    const char *start;
    const char *current;
    int32_t line;
};

Scanner scanner;

void initScanner(const char *source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isAtEnd() {
    return *scanner.current == '\0';
}

static Token makeToken(TokenType type) {
    Token token{};
    token.type = type;
    token.start = scanner.start;
    token.length = (int32_t) (scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

static Token errorToken(const char *message) {
    Token token{};
    token.type = TokenType::ERROR;
    token.start = message;
    token.length = (int32_t) strlen(message);
    token.line = scanner.line;
    return token;
}

char advance() {
    scanner.current++;
    return scanner.current[-1];
}

static bool match(char expected) {
    if (isAtEnd()) return false;
    if (*scanner.current != expected) return false;
    scanner.current++;
    return true;
}

static char peek() {
    return *scanner.current;
}

static char peekNext() {
    if (isAtEnd()) return '\0';
    return scanner.current[1];
}

static void skipWhitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                scanner.line++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
            default:
                return;
        }
    }
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static Token number() {
    while (isDigit(peek())) advance();
    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();
    }
    return makeToken(TokenType::NUMBER);
}

static Token string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') scanner.line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    advance();
    return makeToken(TokenType::STRING);
}

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

static TokenType checkKeyword(int32_t start, int32_t length, const char *rest, TokenType type) {
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }
    return TokenType::ERROR;
}

static TokenType identifierType() {
    switch (scanner.start[0]) {
        case 'a':
            return checkKeyword(1, 2, "nd", TokenType::AND);
        case 'c':
            return checkKeyword(1, 4, "lass", TokenType::CLASS);
        case 'e':
            return checkKeyword(1, 3, "lse", TokenType::ELSE);
        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a':
                        return checkKeyword(2, 3, "lse", TokenType::FALSE);
                    case 'o':
                        return checkKeyword(2, 1, "r", TokenType::FOR);
                    case 'u':
                        return checkKeyword(2, 1, "n", TokenType::FUN);
                }
            }
            break;
        case 'i':
            return checkKeyword(1, 1, "f", TokenType::IF);
        case 'n':
            return checkKeyword(1, 2, "il", TokenType::NIL);
        case 'o':
            return checkKeyword(1, 1, "r", TokenType::OR);
        case 'p':
            return checkKeyword(1, 4, "rint", TokenType::PRINT);
        case 'r':
            return checkKeyword(1, 5, "eturn", TokenType::RETURN);
        case 's':
            return checkKeyword(1, 4, "uper", TokenType::SUPER);
        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'h':
                        return checkKeyword(2, 2, "is", TokenType::THIS);
                    case 'r':
                        return checkKeyword(2, 2, "ue", TokenType::TRUE);
                }
            }
        case 'v':
            return checkKeyword(1, 2, "ar", TokenType::VAR);
        case 'w':
            return checkKeyword(1, 4, "hile", TokenType::WHILE);
    }
    return TokenType::IDENTIFIER;
}

static Token identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();
    return makeToken(identifierType());
}

Token scanToken() {
    skipWhitespace();
    scanner.start = scanner.current;
    if (isAtEnd()) return makeToken(TokenType::TOKEN_EOF);
    char c = advance();
    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();
    switch (c) {
        case '(':
            return makeToken(TokenType::LEFT_PAREN);
        case ')':
            return makeToken(TokenType::RIGHT_PAREN);
        case '{':
            return makeToken(TokenType::LEFT_BRACE);
        case '}':
            return makeToken(TokenType::RIGHT_BRACE);
        case ';':
            return makeToken(TokenType::SEMICOLON);
        case ',':
            return makeToken(TokenType::COMMA);
        case '.':
            return makeToken(TokenType::DOT);
        case '-':
            return makeToken(TokenType::MINUS);
        case '+':
            return makeToken(TokenType::PLUS);
        case '/':
            return makeToken(TokenType::SLASH);
        case '*':
            return makeToken(TokenType::STAR);
        case '!':
            return makeToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '=':
            return makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '<':
            return makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>':
            return makeToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case '"':
            return string();
        default:
            return errorToken("Unexpected character.");
    }
}
