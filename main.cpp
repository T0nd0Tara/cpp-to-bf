#include <iostream>
#include <deque>
#include <vector>
#include <cassert>
#include <any>

#include "magic_enum.hpp"

#define MEM_SIZE 30000

enum class TokType : size_t{
    NOP,
    PRINT,
    ADD_VAR,
};
enum class InstType : size_t{
    NOP,
    GOTO,
    PRINT,
};

struct Token{
    TokType type;
    std::any val;

    Token(TokType typ, std::any value = NULL): type(typ), val(value) {}
};

struct Inst{
    InstType type;
    std::any val;

    Inst(InstType typ, std::any value = NULL): type(typ), val(value) {}
};

struct Var{
    size_t mem_pos;
    uint8_t val;
    static const size_t mem_len;
};
const size_t Var::mem_len = 3;

struct ProgState{
    uint8_t* mem;
    uint8_t* mem_pos;
    std::vector<Var> vVar;
    
    ProgState(){
        mem = new uint8_t[MEM_SIZE]();
        mem_pos = mem;
    }
    ~ProgState(){
        delete[] mem;
    }

    size_t end_of_used_mem(){
        
    }
};

// if exitCode == 0 the function will NOT terminate the program
void printError(std::string message, int exitCode = 0, std::ostream& os = std::cerr){
    os << message;
    if (exitCode != 0) exit(exitCode);
}

template <typename T>
T handled_any_cast(std::any val){
    try {
        return std::any_cast<T>(val);
    } catch (std::bad_any_cast& e){
        printError("ERROR: unable to cast value to " + std::string(typeid(T).name()) + ".\n", 1);
    }
}

std::deque<Inst> preprocess(const std::deque<Token>& dTok){
    ProgState state;
    std::deque<Inst> out;
    for (size_t ip=0; ip<dTok.size(); ip++){
        const& token = dTok[ip];
        switch(token.type){
        case NOP:
            break;
        case PRINT:
            std::string sPrint = handled_any_cast<std::string>(token.val);

            break;
        case ADD_VAR:
            break;
        }
    }
    return out;
}

void evalInst(const std::deque<Inst>& dInst){
    for (size_t ip=0; ip<dInst.size(); ip++){
        const Inst& inst = dInst[ip];

        switch(inst.type){
        case InstType::NOP:
            break;
        case InstType::PRINT:
            std::cout << "."; 
            break;
        }
    } 
}

int main(){
    std::deque<Token> dTok;


    std::deque<Inst> dInst = preprocess(dTok);

    evalInst(dInst);

    return 0;
}
