#include "CommandDispatcher.h"

CommandDispatcher::CommandDispatcher() : head(nullptr), inputBuffer("") {}

void CommandDispatcher::registerCommand(String name, CommandCallback func, int paramLength) {
    CommandNode* newNode = new CommandNode;
    newNode->name = name;
    newNode->name.toUpperCase();
    newNode->callback = func;
    newNode->paramLength = paramLength;
    newNode->next = head;
    head = newNode;
}

bool CommandDispatcher::execute(String cmdName, String args) {
    CommandNode* current = head;
    bool found = false;
    while (current != nullptr) {
        // We only dispatch
        // when the sequence contains the command followed by the expected
        // number of parameter characters at the end. For example, if the
        // registered name is "*341#" with paramLength=3, we look for
        // "*341#<arg1><arg2><arg3>" ending at the end of `cmdName`.
        int cmdLen = current->name.length();
        int totalNeeded = cmdLen + current->paramLength;
        int seqLen = cmdName.length();

        if (seqLen >= totalNeeded) {
            // substring that should match the command name
            int startIndex = seqLen - totalNeeded;
            String candidate = cmdName.substring(startIndex, startIndex + cmdLen);
            if (candidate == current->name) {
                // extract args (may be empty)
                String extractedArgs = "";
                if (current->paramLength > 0) {
                    extractedArgs = cmdName.substring(seqLen - current->paramLength);
                }
                current->callback(extractedArgs);
                found = true;
                break;
            }
        }
        current = current->next;
    }
    if (!found) {
        //Serial.print("Unknown command: ");
        //Serial.println(cmdName);
    }
    return found;
}
