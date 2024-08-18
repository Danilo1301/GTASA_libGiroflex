#pragma once

#include <iostream>
#include <cstdint>
#include <functional>

#define DEFOPCODE(__opcode, __name, __args) const SCRIPT_COMMAND scm_##__name = { 0x##__opcode, #__args }

#define MAX_SCRIPT_VARS 16

struct SCRIPT_COMMAND
{
    uint16_t  opCode;
    char      params[MAX_SCRIPT_VARS];
};

class IScriptCommands {
public:
    unsigned int deltaTime = 0;

    //1.0.0

    /** Calls a script opcode (mini-cleo).
    *  A serious crash may appear if you are using this function and one of the arguments is a pointer (&scmHandle).
    *  Make sure, that the variable you need pointer for (scmHandle1 in this case)
    *  is defined OUTSIDE of the function, otherwise the stack corruption and crash will be your best friends!
    *  For more info, check SAUtils's sautils.cpp, line #1462, you will see.
    *
    *  \param pScriptCommand A pointer to SCRIPT_COMMAND struct var
    *  \param ... All arguments
    *  \return A possibly returned value
    */
    virtual int ScriptCommand(const SCRIPT_COMMAND *pScriptCommand, ...) = 0;

    /** Waits for 'time' seconds and calls 'callback' function after that*/
    virtual void Wait(unsigned int time, std::function<void()> callback) = 0;

    /** Add a function (fn) to be called every game update */
    virtual void AddOnUpdateGame(std::function<void()> fn) = 0;

    virtual void AddOnFirstUpdateGame(std::function<void()> fn) = 0;

    /** Waits for condition (fn) to either return true of to be cancelled
    * If return true, it will call the onComplete function
    * If return false, it will keep testing
    * If you call cancell, it will cancel and call the onCancel function
    */
    virtual void AddCondition(std::function<bool(std::function<void()>)> fn, std::function<void()> onComplete, std::function<void()> onCancel) = 0;

    /** Waits for function (testFn) to return true
    * If return true, it will call the callback function
    * If return false, it will keep testing
    */
    virtual void AddWaitForFunction(std::function<bool()> testFn, std::function<void()> callback) = 0;
};