#include <iostream>
#include <deque>
#include <vector>
#include <cassert>
#include <any>
#include <cxxabi.h>

#include "magic_enum.hpp"

#define MEM_SIZE 30000

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
        printError("ERROR: unable to cast value\n"
                "Original:  "  + 
                std::string(abi::__cxa_demangle(val.type().name(),0,0, nullptr)) +
                "\nCasted to: "+ std::string(typeid(T).name()) + 
                std::string(abi::__cxa_demangle(typeid(T).name(),0,0, nullptr)) + "\n"
                , 1);
    }
    return T(0);
}

enum class TokType : size_t{
    NOP,
    PRINT,
    ADD_VAR,
};
enum class InstType : size_t{
    NOP,
    GOTO,
    PRINT,
    ADD, SUB,
    CLEAR,
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
    enum MemType : uint16_t{
        EMPTY,
        VAR,CHAR
    };

    std::pair<uint8_t, MemType>* mem;
    std::pair<uint8_t, MemType>* mem_pos;
    std::vector<Var> vVar;
    
    ProgState(){
        mem = new std::pair<uint8_t,MemType>[MEM_SIZE];
        std::fill(mem, mem + MEM_SIZE, std::make_pair(0, EMPTY));
        mem_pos = mem;
    }
    ~ProgState(){
        delete[] mem;
    }
    inline void putCell(size_t index, uint8_t c, MemType type){
        mem[index] = std::make_pair(c, type);
    }
    inline void freeCell(size_t index){
        mem[index] = std::make_pair(0, EMPTY);
    }

    inline size_t currInd() const {
        return mem_pos - mem;
    }
    inline size_t firstEmpty() const {
        size_t out;
        for (out=0; out<MEM_SIZE; out++){
            if (mem[out].second == EMPTY)
                return out;
        }
        printError("ERROR: MEM_SIZE (" + std::to_string(MEM_SIZE) + ") is too small (filled all of it)\n",1);
        return out;
    }
};


std::deque<Inst> preprocess(const std::deque<Token>& dTok){
    ProgState state;
    std::deque<Inst> out;
    for (size_t ip=0; ip<dTok.size(); ip++){
        const Token& token = dTok[ip];
        
        switch(token.type){
        case TokType::NOP:
            break;
        case TokType::PRINT:
            {
                std::string sPrint = handled_any_cast<std::string>(token.val);
                size_t nGOTO = state.firstEmpty();
                size_t nCurrInd = state.currInd();
                out.push_back(Inst(InstType::GOTO, nGOTO));
                for (char& c: sPrint){
                    // uint8_t c = (uint8_t)cc;
                    uint8_t currVal = state.mem_pos->first;
                    if (c > currVal)      out.push_back(Inst(InstType::ADD, uint8_t(c - currVal)));
                    else if (currVal > c) out.push_back(Inst(InstType::SUB, uint8_t(currVal - c)));
                    state.mem_pos->first = c;
                    out.push_back(Inst(InstType::PRINT));
                }
                state.mem_pos->first = 0;
                out.push_back(Inst(InstType::CLEAR));
                out.push_back(Inst(InstType::GOTO, nCurrInd));
            }
            break;
        case TokType::ADD_VAR:
            // TODO
            assert(false && "UNIMPLEMENTED");
            break;
        }
    }
    return out;
}
void evalInst(const std::deque<Inst>& dInst){
    ProgState state;
    for (size_t ip=0; ip<dInst.size(); ip++){
        const Inst& inst = dInst[ip];

        switch(inst.type){
        case InstType::NOP:
            break;
        case InstType::GOTO:
            {
                size_t nGOTO = handled_any_cast<size_t>(inst.val);
                size_t nCurrInd = state.currInd();

                // TODO: clean this mess
                if (nCurrInd < nGOTO){
                    do{
                        std::cout << ">";
                        nCurrInd++;
                    }while (nCurrInd < nGOTO);
                }else if (nGOTO < nCurrInd) {
                    do{
                        std::cout << "<";
                        nCurrInd--;
                    }while (nCurrInd > nGOTO);
                }
                state.mem_pos = state.mem + nGOTO;
            }
            break;
        case InstType::PRINT:
            std::cout << "."; 
            break;
        case InstType::ADD:
            {
                uint8_t c = handled_any_cast<uint8_t>(inst.val);
                for (uint8_t i=0; i<c; i++)
                    std::cout << "+";
                state.mem_pos->first += c;
            }
            break;
        case InstType::SUB:
            {
                uint8_t c = handled_any_cast<uint8_t>(inst.val);
                for (uint8_t i=0; i<c; i++)
                    std::cout << "-";
                state.mem_pos->first -= c;
            }
            break;
        case InstType::CLEAR:
            if (state.mem_pos->first < 128) std::cout << "[-]";
            else std::cout << "[+]";
            break;
        }
    } 
}

int main(){
    std::deque<Token> dTok;
    dTok.push_back(Token(TokType::PRINT, std::string("Hello World!\n")));

    std::deque<Inst> dInst = preprocess(dTok);

    evalInst(dInst);

    return 0;
}
