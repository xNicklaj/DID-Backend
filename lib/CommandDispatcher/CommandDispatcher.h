#ifndef COMMAND_DISPATCHER_H
#define COMMAND_DISPATCHER_H

#include <Arduino.h>

// Function pointer for commands: accepts arguments as a String
typedef void (*CommandCallback)(String args);

struct CommandNode {
    String name;
    CommandCallback callback;
    CommandNode* next;
};

class CommandDispatcher {
private:
    CommandNode* head;
    String inputBuffer;

public:
    CommandDispatcher();

    // API: Register a new command string and its handler function
    void registerCommand(String name, CommandCallback func);

    bool execute(String cmdName, String args);

private:

};

#endif