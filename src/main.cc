#include <cstdio>
#include <cstdlib>
#include "vm.hh"

void repl();

void runFile(const char *path);

int main(int argc, const char *argv[]) {
    initVM();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        fprintf(stderr, "Usage: clox [path]\n");
    }

    freeVM();
    return 0;
}

static char *readFile(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == nullptr) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char *buffer = (char *) malloc(fileSize + 1);
    if (buffer == nullptr) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

void runFile(const char *path) {
    char *source = readFile(path);
    InterpretResult result = interpret(source);
    free(source);

    if (result == InterpretResult::COMPILE_ERROR) exit(65);
    if (result == InterpretResult::RUNTIME_ERROR) exit(70);
}

void repl() {
    char line[1024];
    for (;;) {
        std::cout << "> ";
        if (!fgets(line, sizeof(line), stdin)) {
            std::cout << std::endl;
            break;
        }

        interpret(line);
    }
}
