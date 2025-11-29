#ifndef COMMAND_DISPATCHER_H
#define COMMAND_DISPATCHER_H

#include <Arduino.h>

// Function pointer for commands: accepts arguments as a String
typedef void (*CommandCallback)(String args);

struct CommandNode {
    String name;
    // number of characters expected as parameters after the command
    int paramLength;
    CommandCallback callback;
    CommandNode* next;
};

class CommandDispatcher {
private:
    CommandNode* head;
    String inputBuffer;

public:
    CommandDispatcher();

    // API: Register a new command string, its handler function, and expected parameter length
    // `paramLength` defaults to 0 (no parameters)
    void registerCommand(String name, CommandCallback func, int paramLength = 0);

    bool execute(String cmdName, String args);

private:

};

#endif