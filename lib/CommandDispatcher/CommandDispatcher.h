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
    CommandDispatcher() : head(nullptr), inputBuffer("") {}

    // API: Register a new command string and its handler function
    void registerCommand(String name, CommandCallback func) {
        CommandNode* newNode = new CommandNode;
        newNode->name = name;
        newNode->name.toUpperCase();
        newNode->callback = func;
        newNode->next = head;
        head = newNode;
    }

    // API: Listen for Serial input and execute commands
    void update() {
        while (Serial.available() > 0) {
            char inChar = (char)Serial.read();
            if (inChar == '\n') {
                processBuffer();
                inputBuffer = "";
            } else if (inChar != '\r') {
                inputBuffer += inChar;
            }
        }
    }

    bool execute(String cmdName, String args) {
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
            Serial.print("Unknown command: ");
            Serial.println(cmdName);
        }
        return found;
    }

private:
    void processBuffer() {
        inputBuffer.trim();
        if (inputBuffer.length() == 0) return;

        int spaceIndex = inputBuffer.indexOf(' ');
        String cmdName = (spaceIndex == -1) ? inputBuffer : inputBuffer.substring(0, spaceIndex);
        String args = (spaceIndex == -1) ? "" : inputBuffer.substring(spaceIndex + 1);

        cmdName.toUpperCase();
        execute(cmdName, args);
    }

    
};

#endif