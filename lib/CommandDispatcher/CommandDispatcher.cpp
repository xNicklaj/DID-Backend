#include "CommandDispatcher.h"

CommandDispatcher::CommandDispatcher() : head(nullptr), inputBuffer("") {}

void CommandDispatcher::registerCommand(String name, CommandCallback func) {
    CommandNode* newNode = new CommandNode;
    newNode->name = name;
    newNode->name.toUpperCase();
    newNode->callback = func;
    newNode->next = head;
    head = newNode;
}

bool CommandDispatcher::execute(String cmdName, String args) {
    CommandNode* current = head;
    bool found = false;
    while (current != nullptr) {
        if (current->name == cmdName) {
            current->callback(args);
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found) {
        //Serial.print("Unknown command: ");
        //Serial.println(cmdName);
    }
    return found;
}
