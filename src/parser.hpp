#pragma once

#include "tokenization.hpp"
#include "arenaAllocator.hpp"
#include <variant>
#include <sstream>
#include <set>
#include <optional>
#include <tuple>
#include <utility>
#include <cstring>

// All variable type definitions, second number is size //! SDFKSD:KLFJ:LSDF TEST NOT GATE NEG OP AND EXLAM WITH CASTS, MIGHT NOT WORK
#define INT_DEF {VarType::INT_32, 4}
#define LONG_DEF {VarType::INT_64, 8}
#define FLOAT_DEF {VarType::FLOAT, 4}
#define DOUBLE_DEF {VarType::DOUBLE, 8}
#define BOOL_DEF {VarType::BOOL, 1}
#define CHAR_DEF {VarType::BYTE, 1}
#define SHORT_DEF {VarType::INT_16, 2}
#define BYTE_DEF {VarType::BYTE, 1}

// SPLIT_EXPR_REG_LIMIT determines how many registers must be available for and expression to be split
constexpr int MAX_REG_NUM = 12;
constexpr int NON_VOLATILE_REG_NUM = 6;
constexpr int SPLIT_EXPR_REG_LIMIT = 7;
constexpr unsigned long IMMEDIATE_BIT_LIMIT_MINUS_ONE = 31;
constexpr unsigned long MEM_CHUNK_SIZE = 1024 * 512;

// UNKNOWN must be the last enum
enum class VarType {INT_64, INT_32, INT_16, BYTE, BOOL, FLOAT, DOUBLE, UNKNOWN}; //TODO casting from signed to unsigned or vise-versa should be included in AST
enum class CompareResult : char {NORMAL = 0, FINAL_NUM = 1, SUB_NUM = 2, SUB_RH_NUM = 3, FORCE_INT_NUM = 4};
//TODO: way in the future remember functions should be run in the order they are written, dont flip two functions
const std::unordered_map<TokenType, std::pair<VarType, u_int8_t>> var_associations {
    {TokenType::K_INT, INT_DEF},
    {TokenType::K_LONG, LONG_DEF},
    {TokenType::K_FLOAT, FLOAT_DEF},
    {TokenType::K_DOUBLE, DOUBLE_DEF},
    {TokenType::K_BOOL, BOOL_DEF},
    {TokenType::K_CHAR, CHAR_DEF},
    {TokenType::K_SHORT, SHORT_DEF},
    {TokenType::K_BYTE, BYTE_DEF}
};

const std::unordered_map<VarType, u_int8_t> var_sizes {
    INT_DEF,
    LONG_DEF,
    FLOAT_DEF,
    DOUBLE_DEF,
    BOOL_DEF,
    CHAR_DEF,
    SHORT_DEF,
    BYTE_DEF
};

enum class OperatorType {ADD, SUB, MUL, DIV, MOD, NOT, BITWISE, NONE};
enum class ExprElementType {MEMORY, IMMEDIATE, PAREN};
enum class AsmSection {DATA, RODATA, BSS, TEXT};

template <class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

struct Variable {
    VarType type;
    bool is_signed;
};

struct NodeExpr;

struct NodeTerm;

struct SubExprElement {
    Variable v;
    NodeExpr* e;
};

// ExprElements are meant to skip parenthetical expressions while subexprelements are not
struct ExprElement {
    ExprElementType t;
    SubExprElement cur;
    SubExprElement lh;
    SubExprElement rh;
};

struct ExprOut {
    NodeExpr* expr;
    SubExprElement expr_element;
};

struct TermOut {
    NodeTerm* term;
    ExprElementType element_type;
    SubExprElement expr_element;
};

struct NodeExprBinAdd {
    NodeExpr* lh;
    NodeExpr* rh;
};

struct NodeExprBinSub {
    NodeExpr* lh;
    NodeExpr* rh;
    bool by_immediate = false;
};

struct NodeExprBinMul {
    NodeExpr* lh;
    NodeExpr* rh;
    bool lh_paren = false;
    char by_immediate = 0;
};

struct NodeExprBinDiv {
    NodeExpr* lh;
    NodeExpr* rh;
    bool by_immediate = false;
};

struct NodeExprBinMod {
    NodeExpr* lh;
    NodeExpr* rh;
    bool by_immediate = false;
};

struct NodeExprBinAddByPar {
    NodeExpr* rh;
    NodeExpr* lh;
    long last_areg_op;
};

struct NodeExprBinSubByPar {
    NodeExpr* rh;
    NodeExpr* lh;
    long last_areg_op;
};

struct NodeExprBinMulByPar {
    NodeExpr* rh;
    NodeExpr* lh;
    long last_areg_op;
    bool by_immediate = false;
};

struct NodeExprBinDivByPar {
    NodeExpr* rh;
    NodeExpr* lh;
    long last_areg_op;
};

struct NodeExprBinModByPar {
    NodeExpr* lh;
    NodeExpr* rh;
    long last_areg_op;
};

struct NodeExprBinAnd {
    NodeExpr* rh;
    NodeExpr* lh;
};

struct NodeExprBinOr {
    NodeExpr* rh;
    NodeExpr* lh;
};

struct NodeExprBinXor {
    NodeExpr* rh;
    NodeExpr* lh;
};

struct NodeExprBinAndByPar {
    NodeExpr* lh;
    NodeExpr* rh;
    long last_areg_op;
};

struct NodeExprBinOrByPar {
    NodeExpr* rh;
    NodeExpr* lh;
    long last_areg_op;
};

struct NodeExprBinXorByPar {
    NodeExpr* rh;
    NodeExpr* lh;
    long last_areg_op;
};

struct NodeExprBinRShift {
    NodeExpr* lh;
    NodeExpr* rh;
    bool by_immediate = false;
};

struct NodeExprBinLShift {
    NodeExpr* lh;
    NodeExpr* rh;
    bool by_immediate = false;
};

struct NodeExprBinRShiftByPar {
    NodeExpr* rh;
    NodeExpr* lh;
    long last_areg_op;
};

struct NodeExprBinLShiftByPar {
    NodeExpr* rh;
    NodeExpr* lh;
    long last_areg_op;
};

struct NodeExprBinRRoll {
    NodeExpr* lh;
    NodeExpr* rh;
    bool by_immediate = false;
};

struct NodeExprBinLRoll {
    NodeExpr* lh;
    NodeExpr* rh;
    bool by_immediate = false;
};

struct NodeExprBinRRollByPar {
    NodeExpr* rh;
    NodeExpr* lh;
    long last_areg_op;
};

struct NodeExprBinLRollByPar {
    NodeExpr* rh;
    NodeExpr* lh;
    long last_areg_op;
};

using ExprBinType = std::variant<NodeExprBinAdd*, NodeExprBinSub*, NodeExprBinMul*, NodeExprBinDiv*,
                                    NodeExprBinMod*, NodeExprBinAddByPar*, NodeExprBinSubByPar*, 
                                    NodeExprBinMulByPar*, NodeExprBinDivByPar*, NodeExprBinModByPar*, 
                                    NodeExprBinAnd*, NodeExprBinOr*, NodeExprBinXor*, NodeExprBinAndByPar*, 
                                    NodeExprBinOrByPar*, NodeExprBinXorByPar*, NodeExprBinRShift*, 
                                    NodeExprBinLShift*, NodeExprBinRShiftByPar*, NodeExprBinLShiftByPar*, 
                                    NodeExprBinRRoll*, NodeExprBinLRoll*, NodeExprBinRRollByPar*, 
                                    NodeExprBinLRollByPar*>;

struct NodeExprBin {
    ExprBinType expr;
};

struct NodeTermIntLit {
    int64_t bin_num;
};

struct NodeTermIdent {
    const char* ident;
};

struct NodeTermParen {
    NodeExpr* expr;
    bool only_too_large_imm = false;
};

struct NodeTermNotExpr {
    NodeTerm* term;
};

struct NodeTermNegExpr {
    NodeTerm* term;
};

struct NodeTermLogNotExpr {
    NodeExpr* expr;
};

struct NodeTerm {
    std::variant<NodeTermIntLit*, NodeTermIdent*, NodeTermParen*, NodeTermNotExpr*, 
                    NodeTermNegExpr*, NodeTermLogNotExpr*> var;
};

struct NodeExprLogOpAnd {
    NodeExpr* lh;
    NodeExpr* rh;
    bool num_result;
};

struct NodeExprLogOpOr {
    NodeExpr* lh;
    NodeExpr* rh;
    bool num_result;
};

struct NodeExprLogTest {
    NodeExprLogTest(ExprElementType& type, std::vector<std::pair<unsigned long, unsigned long>>& last_main, unsigned long expr_pos)
        : ident(type == ExprElementType::MEMORY) {
        last_main.back().second = expr_pos;}

    NodeExpr* expr;
    bool ident;
};

struct NodeExprLogCompareEqu {
    NodeExpr* lh;
    NodeExpr* rh;
    CompareResult num_result = CompareResult::NORMAL;
    bool lh_ident;
    bool rh_ident;
};

struct NodeExprLogCompareNotEqu {
    NodeExpr* lh;
    NodeExpr* rh;
    CompareResult num_result = CompareResult::NORMAL;
    bool lh_ident;
    bool rh_ident;
};

struct NodeExprLogCompareGreater {
    NodeExpr* lh;
    NodeExpr* rh;
    CompareResult num_result = CompareResult::NORMAL;
    bool lh_ident;
    bool rh_ident;
};

struct NodeExprLogCompareLess {
    NodeExpr* lh;
    NodeExpr* rh;
    CompareResult num_result = CompareResult::NORMAL;
    bool lh_ident;
    bool rh_ident;
};

struct NodeExprLogCompareGreaterEqu {
    NodeExpr* lh;
    NodeExpr* rh;
    CompareResult num_result = CompareResult::NORMAL;
    bool lh_ident;
    bool rh_ident;
};

struct NodeExprLogCompareLessEqu {
    NodeExpr* lh;
    NodeExpr* rh;
    CompareResult num_result = CompareResult::NORMAL;
    bool lh_ident;
    bool rh_ident;
};

struct NodeExprLogCompareEquByPar {
    NodeExpr* lh;
    NodeExpr* rh;
    CompareResult num_result = CompareResult::NORMAL;
    bool lh_ident;
    bool comp_as_int;
    long last_areg_op;
};

struct NodeExprLogCompareNotEquByPar {
    NodeExpr* lh;
    NodeExpr* rh;
    CompareResult num_result = CompareResult::NORMAL;
    bool lh_ident;
    bool comp_as_int;
    long last_areg_op;
};

struct NodeExprLogCompareGreaterByPar {
    NodeExpr* lh;
    NodeExpr* rh;
    CompareResult num_result = CompareResult::NORMAL;
    bool lh_ident;
    bool comp_as_int;
    long last_areg_op;
};

struct NodeExprLogCompareLessByPar {
    NodeExpr* lh;
    NodeExpr* rh;
    CompareResult num_result = CompareResult::NORMAL;
    bool lh_ident;
    bool comp_as_int;
    long last_areg_op;
};

struct NodeExprLogCompareGreaterEquByPar {
    NodeExpr* lh;
    NodeExpr* rh;
    CompareResult num_result = CompareResult::NORMAL;
    bool lh_ident;
    bool comp_as_int;
    long last_areg_op;
};

struct NodeExprLogCompareLessEquByPar {
    NodeExpr* lh;
    NodeExpr* rh;
    CompareResult num_result = CompareResult::NORMAL;
    bool lh_ident;
    bool comp_as_int;
    long last_areg_op;
};

using ExprLogType = std::variant<NodeExprLogOpAnd*, NodeExprLogOpOr*, NodeExprLogTest*, 
                                    NodeExprLogCompareEqu*, NodeExprLogCompareNotEqu*,
                                    NodeExprLogCompareGreater*, NodeExprLogCompareLess*, 
                                    NodeExprLogCompareGreaterEqu*, NodeExprLogCompareLessEqu*, 
                                    NodeExprLogCompareEquByPar*, NodeExprLogCompareNotEquByPar*, 
                                    NodeExprLogCompareGreaterByPar*, NodeExprLogCompareLessByPar*, 
                                    NodeExprLogCompareGreaterEquByPar*, NodeExprLogCompareLessEquByPar*>;

struct NodeExprLog {
    ExprLogType var;
};

struct NodeExprCast {
    Variable from_type;
    bool round_float;
    NodeExpr* expr;
};

struct NodeExprTwoPart {
    NodeExpr* first;
    NodeExpr* second;
};

struct NodeExpr {
    std::variant<NodeTerm*, NodeExprBin*, NodeExprLog*, NodeExprCast*, NodeExprTwoPart*> var;
};

/* For properties, last to bits represent if the node is a normal assignment (00),
an assignment to a single immediate term (01), or a constant assignment (11).
The third an fourth bits control if the operation uses either integer signed or
unsigned instructions (10 or 00), or if it uses floating point or double precision
floating point instructions (01 or 11). The fifth bit is 1 if the assignment is a
single integer and the integer is greater than the immediate limit.
*/
const unsigned char term_assign = 1; // 0b01
const unsigned char const_assign = 3; // 0b11
const unsigned char float_instruc = 4; // 0b0100
const unsigned char double_instruc = 12; // 0b1100
const unsigned char too_big_imm_assign = 16; // 0b10000

struct NodeStmtVarAssign { //Assingment nodes must have const char* ident
    VarType type;
    unsigned char properties = 0;
    const char* ident;
    NodeExpr* expr;
};

struct NodeStmtVarSet {
    VarType type;
    unsigned char properties = 0;
    const char* ident;
    NodeExpr* expr;
};

struct NodeStmtExit {
    NodeExpr* val;
};

struct NodeStmt;

struct NodeScope {
    size_t num_bytes;
    int8_t used_non_volatile_regs;
    std::vector<NodeStmt*> stmts;
};

struct NodeStmtGroup {
    std::vector<NodeStmt*> stmts;
};

struct NodeStmtIf {
    Variable if_expr_type;
    NodeExpr* if_expr;
    NodeStmt* stmts;
    std::optional<NodeStmt*> else_stmts;
};

struct NodeStmtIfSet {
    Variable if_expr_type;
    NodeStmtVarSet* var_set;
    NodeStmt* stmts;
    std::optional<NodeStmt*> else_stmts;
};

struct NodeStmtWhile {
    Variable if_expr_type;
    std::optional<NodeExpr*> if_expr;
    NodeStmt* stmts;
    bool do_while;
};

struct NodeStmtFor {
    Variable if_expr_type;
    std::optional<NodeExpr*> if_expr;
    NodeStmt* init_stmt;
    NodeStmt* end_stmt;
    NodeStmt* stmts;
};

struct NodeStmtBreak {
    size_t loop = 0; // 0 for current loop
};

struct NodeStmtContinue {
    size_t loop = 0; // 0 for current loop
};

struct NodeStmtAsm {
    AsmSection section;
    std::string assembly;
};

struct NodeStmtBlank {
    // blank
};

struct NodeStmt {
    std::variant<NodeStmtVarAssign*, NodeStmtVarSet*, NodeStmtExit*, NodeScope*, 
                    NodeStmtGroup*, NodeStmtIf*, NodeStmtIfSet*, NodeStmtWhile*, 
                    NodeStmtFor*, NodeStmtBreak*, NodeStmtContinue*, NodeStmtAsm*, 
                    NodeStmtBlank*> stmt;
};

struct NodeRoot {
    std::vector<NodeStmt*> stmts;
};

bool fits_in_immediate(signed long num, VarType varType) {
    signed long check = num >> IMMEDIATE_BIT_LIMIT_MINUS_ONE;
    return (check >= -1 && (check <= 0 || (varType != VarType::INT_64 && check <= 1)));
}

constexpr int var_switch(const VarType type1, const VarType type2) {
    return (static_cast<int>(type1) << 3) | static_cast<int>(type2);
}

NodeExpr* null_expr = nullptr;

class Parser {
private:
    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    int m_stack_bytes = 0;
    int m_scope_num = 0;
    VarType m_cur_var_type;
    char m_imm_size = 0;
    char m_is_casting = 0;
    int m_used_regs = 0; // This is all just for the specific edge case where someone makes a stupid expression that requires too many registers
    int m_used_regs_ctr = 0;
    long m_extra_space = 0;
    u_int8_t m_extra_single_space = 0;
    unsigned long m_expr_pos = 0;
    std::vector<std::pair<unsigned long, unsigned long>> m_last_main_reg_ops;
    int m_lock_main_reg = 0;
    ExprElement m_left_element;
    ExprElement m_right_element; 
    bool m_elem_used_test = false;
    bool m_use_if_test = false;
    bool m_in_if_stmt = false;
    size_t m_nested_loops = 0;
    Variable m_orig_allowed_type;
    Variable m_decided_type;
    bool m_last_imm_truth;
    std::vector<NodeExprTwoPart*> m_two_part_expr;
    // //scopes are represented with two ints, the first represents scopes as a level,
    // //and the second distinguishes between scopes on the same leve
    // //std::pair<int, int> m_scope;
    // //std::unordered_map<int, int> m_previous_scopes;
    // //first int represents scope num, second num used for multiple scopes on same level
    std::unordered_map<std::string, Variable> m_variables;
    std::set<std::string> m_const_variables;
    ArenaAllocator allocator;

    bool is_signed = true;
    bool is_const = false;

    bool is_cur_expr_single = true;
    bool is_cur_expr_dynamic = false;
    bool is_expr_too_big_imm = false;

    [[nodiscard]] std::optional<Token> next(int offset = 0) const
    {
        int iter = m_index + offset;
        if (iter >= m_tokens.size()) {
            return std::nullopt;
        } else {
            return m_tokens[iter];
        }
    }

    Token consume(size_t skip = 1) 
    {
        Token val = m_tokens.at(m_index);
        m_index += skip;
        return val;
    }

    void error(std::string err) {
        std::cout << "\033[1;31m" << err << get_info() << "\033[0;37m" << std::endl;
        successful = false;
    }

    bool expect(TokenType type, std::string errmsg)
    {
        std::optional<Token> token = next();
        if (token.has_value() && token.value().type == type) {
            consume();
            return true;
        } else {
            error("Syntax error: " + errmsg);
            return false;
        }
    }

    bool descriptor_error(bool used_descriptor) {
        if (used_descriptor) {
            error("Invalid use of descriptor");
            return true;
        }
        return false;
    }

    std::string get_info() 
    {   
        Token dbg_token;
        std::stringstream msg;
        std::optional<Token> next_token = next();
        if (next_token.has_value())
        {
            dbg_token = next_token.value();
        } else {
            msg << " at end of file, previous token";
            dbg_token = m_tokens.at(m_index - 1);
        }
        msg << " at line: " + std::to_string(dbg_token.line) + ", character: " + std::to_string(dbg_token.char_on_line + 1);
        if (dbg_token.value.has_value()) {
            msg << ", token: \"" + dbg_token.value.value() + "\"";
        } else {
            msg << ", token: \"" + debug_list.at(dbg_token.type) + "\"";
        }

        return msg.str();
    }

    //TODO: change how var_name is returned
    const char* get_c_str(std::string& var_name) {
        var_name = consume().value.value();
        const char* c_str = var_name.c_str();
        int len = strlen(c_str)+1;
        char* var_ident = allocator.allocate_c_string(len);
        strncpy(var_ident, c_str, len);

        return var_ident;
    }

    inline void append_to_last_areg() {
        m_last_main_reg_ops.push_back({std::pair<long, long>{m_expr_pos, 0}});
    }

    // returns 0 if unaccepted type was used, updates allowed_type if implicit cast will be used and returns 2 to indicate element_type update
    bool check_type(const VarType& type, const bool is_term_signed, VarType& allowed_type) {
        if (type != allowed_type || type != m_orig_allowed_type.type) { //TODO: signed casts
            if (allowed_type != VarType::UNKNOWN) {
                m_is_casting = 2;
                allowed_type = type; // implicit cast
                return true;
                // m_index--; //TODO: when structs are added this error should happen
                // error("Unaccepted type");
                // return false;
            } else {
                allowed_type = type;
                m_cur_var_type = allowed_type;
                m_decided_type.type = allowed_type;
                m_decided_type.is_signed = is_term_signed;
            }
        }

        return true;
    }

    template <typename T>
    std::optional<T> get_num_from_int_token (TokenType cur_token_type, std::string val) {
        try {
            switch (cur_token_type) {
                case TokenType::LONG_LIT:
                case TokenType::N_LONG_LIT:
                {
                    if (m_cur_var_type != VarType::INT_64) {
                        error("Long literal used in 32 bit expression");
                        return std::nullopt;
                    }
                    signed long num = stol(val);
                    num = cur_token_type == TokenType::LONG_LIT ? num
                        : ~(num) + 1;
                    return (T) (num);
                }
                case TokenType::INT_LIT:
                case TokenType::N_INT_LIT:
                {   
                    if constexpr (std::is_same_v<T, float>) {
                        float num = stof(val);
                        return cur_token_type == TokenType::INT_LIT ? num
                            : -num;
                    } else if constexpr (std::is_same_v<T, double>) {
                        double num = stod(val);
                        return cur_token_type == TokenType::INT_LIT ? num
                            : -num;
                    } else if constexpr (std::is_same_v<T, signed long>) {
                        signed long num = stol(val);
                        num = cur_token_type == TokenType::INT_LIT ? num
                            : ~(num) + 1;
                        return (T) (num);
                    } else if constexpr (std::is_same_v<T, unsigned long>) {
                        unsigned long num = stoul(val);
                        return (T) (num);
                    } else if constexpr (std::is_same_v<T, unsigned int>) {
                        unsigned long num = stoul(val);
                        if (!(num & (~0xFFFFFFFFul))) {
                            num = static_cast<unsigned int>(num);
                        } else throw std::out_of_range("");
                        return (T) (num);
                    } else if constexpr (std::is_same_v<T, signed short>) {
                        int num = stoi(val);
                        if (!(num & (~0xFFFFu))) {
                            num = static_cast<signed short>(num);
                        } else throw std::out_of_range("");
                        num = cur_token_type == TokenType::INT_LIT ? num
                            : ~(num) + 1;
                        return (T) (num);
                    } else if constexpr (std::is_same_v<T, unsigned short>) {
                        int num = stoi(val);
                        if (!(num & (~0xFFFFu))) {
                            num = static_cast<unsigned short>(num);
                        } else throw std::out_of_range("");
                        return (T) (num);
                    } else if constexpr (std::is_same_v<T, signed char>) {
                        int num = stoi(val);
                        if (!(num & (~0xFFu))) {
                            num = static_cast<signed char>(num);
                        } else throw std::out_of_range("");
                        num = cur_token_type == TokenType::INT_LIT ? num
                            : ~(num) + 1;
                        return (T) (num);
                    } else if constexpr (std::is_same_v<T, unsigned char>) {
                        int num = stoi(val);
                        if (!(num & (~0xFFu))) {
                            num = static_cast<unsigned char>(num);
                        } else throw std::out_of_range("");
                        return (T) (num);
                    } else {
                        signed int num = stoi(val);
                        num = cur_token_type == TokenType::INT_LIT ? num
                            : ~(num) + 1;
                        return (T) (num);
                    }
                }
                case TokenType::U_LONG_LIT:
                {
                    unsigned long num = stoul(val);
                    return (T) (num);
                }
                case TokenType::U_INT_LIT:
                {
                    unsigned long num = stoul(val); //no idea why there is no std::stou
                    if (!(num & (~0xFFFFFFFFul))) {
                        num = static_cast<unsigned int>(num);
                    } else throw std::out_of_range("");
                    return (T) (num);
                }
                case TokenType::FLOAT_LIT:
                case TokenType::N_FLOAT_LIT:
                {
                    if (m_cur_var_type != VarType::FLOAT && m_cur_var_type != VarType::DOUBLE) {
                        error("Float literal used in integer expression");
                        return std::nullopt;
                    }
                    if constexpr (std::is_same_v<T, float>) {
                        float num = stof(val);
                        num = cur_token_type == TokenType::FLOAT_LIT ? num
                            : -num;
                        return (T) (num);
                    } else if constexpr (std::is_same_v<T, double>) {
                        double num = stod(val);
                        num = cur_token_type == TokenType::FLOAT_LIT ? num
                            : -num;
                        return (T) (num);
                    }
                }
                default:
                    error("Something went wrong with getting int token");
                    return std::nullopt;
            }
        } catch (std::out_of_range) {
            error("Number too large");
            return std::nullopt;
        }
    }

    std::optional<size_t> loop_statement_nested_check() {
        if (next().has_value() && m_tokens[m_index].type == TokenType::O_BRACK) {
            consume();
            if (next().has_value() && m_tokens[m_index].type == TokenType::INT_LIT) {
                size_t loop = std::stoul(m_tokens[m_index].value.value());
                if (loop > m_nested_loops - 1) {
                    error("Nested loop break or continue statement reaches non-existent loop");
                    return std::nullopt;
                }
                consume();

                if (!expect(TokenType::C_BRACK, "Expected closed bracket")) {
                    return std::nullopt;
                }

                return loop;
            } else {
                error("Expected positive integer value without suffix in nested break or continue statement");
                return std::nullopt;
            }
        } else {
            return 0;
        }
    }

    int64_t get_int_bits_f(float _float) {
        return static_cast<int64_t>(std::bit_cast<int32_t>(_float));
    }

    int64_t get_int_bits_d(double _double) {
        return std::bit_cast<int64_t>(_double);
    }

    template <typename T>
    inline void check_imm_too_big(T num) {
        // float instrucs dont support immediates in the first place, everything is done in memory
        if (!fits_in_immediate(num, m_cur_var_type) && m_cur_var_type != VarType::FLOAT && m_cur_var_type != VarType::DOUBLE) {
            is_expr_too_big_imm = true;
        }
    }

    inline long get_last_areg_amt(bool casted) {
        if (m_is_casting || casted) {
            return m_expr_pos - m_last_main_reg_ops.back().first; // Casts will take care of the mov thanks to use_e
        } else if ((!m_lock_main_reg || m_lock_main_reg == m_used_regs)) {
            return m_last_main_reg_ops.back().second - m_last_main_reg_ops.back().first;
        } else {
            m_lock_main_reg--;
            return m_expr_pos;            
        }
    }

    inline bool is_paren_const() {
        //this checks if the stuff inside the parenthesis is all constant so it knows if it can use it
        size_t temp_index = m_index + 1;
        size_t num_paren = 0;
        TokenType token = m_tokens[temp_index].type;
        do {
            if (token == TokenType::IDENT || token == TokenType::K_LONG || 
                    token == TokenType::K_INT || token == TokenType::K_SHORT || 
                    token == TokenType::K_BYTE ||token == TokenType::K_BOOL || 
                    token == TokenType::K_FLOAT || token == TokenType::K_DOUBLE) {
                return false;
            } else if (m_tokens[temp_index].type == TokenType::O_PAREN) {
                num_paren += 1;
            } else if (m_tokens[temp_index].type == TokenType::C_PAREN) {
                num_paren -= 1;
            }
            token = m_tokens[++temp_index].type;
        } while (temp_index < m_tokens.size() && num_paren > 0);
        
        return true;
    }

    inline void update_areg_val(NodeExpr* expr, size_t last_areg) {
        auto set_last_areg_sub = overloaded {
            [&last_areg](NodeExprBinAddByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprBinSubByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprBinMulByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprBinDivByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprBinModByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprBinAndByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprBinOrByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprBinXorByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprBinLShiftByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprBinRShiftByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprBinLRollByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprBinRRollByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprLogCompareEquByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprLogCompareNotEquByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprLogCompareLessByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprLogCompareLessEquByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprLogCompareGreaterByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](NodeExprLogCompareGreaterEquByPar* arg) {unsigned long& ref = (unsigned long&) arg->last_areg_op; ref = last_areg;},
            [&last_areg](auto&& arg) {;}
        };

        auto set_last_areg = overloaded {
            [&set_last_areg_sub](NodeExprBin* arg) {std::visit(set_last_areg_sub, arg->expr);},
            [&set_last_areg_sub](NodeExprLog* arg) {std::visit(set_last_areg_sub, arg->var);},
            [&set_last_areg_sub](auto&& arg) {;}
        };

        std::visit(set_last_areg, expr->var);
    }

    // This function does lead to a slight excess in heap memory allocated but it is necessary to 
    // conform to C++ type standards while using std::variant //TODO: maybe could flip if possible but might just not be worth it
    template <typename T1, typename target_type>
    void switch_node_type(T1& parent, NodeExpr*& lh, NodeExpr*& rh) {
        target_type* new_struct = allocator.allocate<target_type>();
        new_struct->lh = lh;
        new_struct->rh = rh;

        parent.template emplace<target_type*>(new_struct);
    }

    // a horrid function that shouldnt actually take that long
    void switch_to_paren(NodeExpr* parent_expr) {
        // modern C++ garbage that is so complex it lagged my computer typing it
        auto switch_to_paren_sub = overloaded {
            [this](NodeExprBinAdd*& arg, ExprBinType& parent) {switch_node_type<ExprBinType, NodeExprBinAddByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprBinSub*& arg, ExprBinType& parent) {switch_node_type<ExprBinType, NodeExprBinSubByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprBinMul*& arg, ExprBinType& parent) {switch_node_type<ExprBinType, NodeExprBinMulByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprBinDiv*& arg, ExprBinType& parent) {switch_node_type<ExprBinType, NodeExprBinDivByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprBinMod*& arg, ExprBinType& parent) {switch_node_type<ExprBinType, NodeExprBinModByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprBinAnd*& arg, ExprBinType& parent) {switch_node_type<ExprBinType, NodeExprBinAndByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprBinOr*& arg, ExprBinType& parent) {switch_node_type<ExprBinType, NodeExprBinOrByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprBinXor*& arg, ExprBinType& parent) {switch_node_type<ExprBinType, NodeExprBinXorByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprBinLShift*& arg, ExprBinType& parent) {switch_node_type<ExprBinType, NodeExprBinLShiftByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprBinRShift*& arg, ExprBinType& parent) {switch_node_type<ExprBinType, NodeExprBinRShiftByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprBinLRoll*& arg, ExprBinType& parent) {switch_node_type<ExprBinType, NodeExprBinLRollByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprBinRRoll*& arg, ExprBinType& parent) {switch_node_type<ExprBinType, NodeExprBinRRollByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprLogCompareEqu*& arg, ExprLogType& parent) {switch_node_type<ExprLogType, NodeExprLogCompareEquByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprLogCompareNotEqu*& arg, ExprLogType& parent) {switch_node_type<ExprLogType, NodeExprLogCompareNotEquByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprLogCompareLess*& arg, ExprLogType& parent) {switch_node_type<ExprLogType, NodeExprLogCompareLessByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprLogCompareLessEqu*& arg, ExprLogType& parent) {switch_node_type<ExprLogType, NodeExprLogCompareLessEquByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprLogCompareGreater*& arg, ExprLogType& parent) {switch_node_type<ExprLogType, NodeExprLogCompareGreaterByPar>(parent, arg->lh, arg->rh);},
            [this](NodeExprLogCompareGreaterEqu*& arg, ExprLogType& parent) {switch_node_type<ExprLogType, NodeExprLogCompareGreaterEquByPar>(parent, arg->lh, arg->rh);},
            [this](auto&& arg, ExprBinType parent) {;},
            [this](auto&& arg, ExprLogType parent) {;}
        };

        auto switch_to_paren = overloaded {
            [&switch_to_paren_sub](NodeExprBin*& arg) {std::visit([&arg, &switch_to_paren_sub](auto&& arg2) {switch_to_paren_sub(arg2, arg->expr);}, arg->expr);},
            [&switch_to_paren_sub](NodeExprLog*& arg) {std::visit([&arg, &switch_to_paren_sub](auto&& arg2) {switch_to_paren_sub(arg2, arg->var);}, arg->var);},
            [&switch_to_paren_sub](auto&& arg) {;}
        };

        std::visit(switch_to_paren, parent_expr->var);
        update_areg_val(parent_expr, m_expr_pos - m_last_main_reg_ops.back().first - 1); // subtract 1 due to secondary level of tree
    }

    // returns pointer to cast node
    [[nodiscard]] NodeExprCast* insert_cast(NodeExpr* expr, Variable from_type) {
        NodeExprCast* cast_expr = allocator.allocate<NodeExprCast>();
        NodeExpr* new_expr = allocator.allocate<NodeExpr>();

        new_expr->var = expr->var;
        cast_expr->expr = new_expr;
        cast_expr->from_type = from_type; 
        cast_expr->round_float = false;

        return cast_expr; 
    }

    // returns 1 if right side had a cast added to it (false if entire tree had a cast added)
    inline bool insert_implicit_casts_single(ExprElement& element, Variable adjacent_type, bool final = false) {
        if (element.lh.v.type != element.rh.v.type) {
            if (!final) {
                // Floats are prioritized so that they are only floored at the end
                if ((element.lh.v.type == VarType::FLOAT && element.rh.v.type != VarType::DOUBLE) || element.lh.v.type == VarType::DOUBLE) {
                    element.rh.e->var = insert_cast(element.rh.e, element.rh.v);
                    element.cur.v = element.lh.v;
                    switch_to_paren(element.cur.e);
                    return true;
                } else if (element.rh.v.type == VarType::FLOAT || element.rh.v.type == VarType::DOUBLE) {
                    element.lh.e->var = insert_cast(element.lh.e, element.lh.v);
                    element.cur.v = element.rh.v;
                    return false;
                }
                // Longs are prioritized so that information isnt lost until the end
                if (element.lh.v.type == VarType::INT_64) {
                    element.rh.e->var = insert_cast(element.rh.e, element.rh.v);
                    element.cur.v = element.lh.v;
                    switch_to_paren(element.cur.e);
                    return true;
                } else if (element.rh.v.type == VarType::INT_64) {
                    element.lh.e->var = insert_cast(element.lh.e, element.lh.v);
                    element.cur.v = element.rh.v;
                    return false;
                }
            }

            element.cur.v = adjacent_type;
            if (element.lh.v.type != adjacent_type.type) {
                element.lh.e->var = insert_cast(element.lh.e, element.lh.v);
            }
            if (element.rh.v.type != adjacent_type.type) {
                element.rh.e->var = insert_cast(element.rh.e, element.rh.v);
                switch_to_paren(element.cur.e);
                return true;
            }
            return false;
        }
        // ! CHECK FLOAT TYPE
        if (element.lh.v.type != adjacent_type.type) {
            element.cur.e->var = insert_cast(element.cur.e, element.lh.v);
            element.cur.v = adjacent_type;
            return false;
        }
        // implicit signed casts, helps with optimization
        if (element.lh.v.is_signed != adjacent_type.is_signed) {
            element.lh.e->var = insert_cast(element.lh.e, element.lh.v);
        }
        if (element.rh.v.is_signed != adjacent_type.is_signed) {
            element.rh.e->var = insert_cast(element.rh.e, element.rh.v);
        }
        
        element.cur.v = Variable{.type = element.lh.v.type, .is_signed = adjacent_type.is_signed};
        return false;
    }

    inline bool insert_implicit_casts_doub_par() {
        std::array<int, static_cast<int>(VarType::UNKNOWN)> counts = {0}; // UNKNOWN is the last element and thus gives number of enums

        counts[static_cast<int>(m_left_element.lh.v.type)]++;
        counts[static_cast<int>(m_left_element.rh.v.type)]++;
        counts[static_cast<int>(m_right_element.lh.v.type)]++;
        counts[static_cast<int>(m_right_element.rh.v.type)]++;

        auto most_common_index = std::distance(counts.begin(), std::max_element(counts.begin(), counts.end())); //!?!!!DASKJLDAJKL:SDJKL:ASJKL:DJKAL:S:DJLKA:JKLSDJ:KLA:KLJSDJL:KA:SKLJDKALSDJKL:AKLSDJKLASD

        Variable most_common = Variable{static_cast<VarType>(most_common_index), is_signed};

        insert_implicit_casts_single(m_left_element, most_common);
        return insert_implicit_casts_single(m_right_element, most_common) == 2;
    }

    inline bool insert_implicit_casts() {
        if (m_left_element.t == ExprElementType::PAREN) {
            if (m_right_element.t == ExprElementType::PAREN) {
                return insert_implicit_casts_doub_par();
            } else {
                return insert_implicit_casts_single(m_left_element, m_right_element.cur.v);
            }
        } else if (m_right_element.t == ExprElementType::PAREN) {
            return insert_implicit_casts_single(m_right_element, m_left_element.cur.v);
        }

        return false;
    }

    inline void insert_last_implicit_cast(NodeExpr* final_expr) {
        if (!is_cur_expr_single) {
            if (insert_implicit_casts_single(m_right_element, m_orig_allowed_type, true)) {
                if (m_right_element.t == ExprElementType::PAREN) {
                    update_areg_val(final_expr, 1);
                }
            }
        } else if (m_right_element.cur.v.type != m_orig_allowed_type.type || m_right_element.cur.v.is_signed != m_orig_allowed_type.is_signed) {
            m_right_element.cur.e->var = insert_cast(m_right_element.cur.e, m_right_element.cur.v);
        }
    }

    // in this language bitwise operators have the same precidence as multiplication
    // and division, this is different from C and a lot of languages but is more 
    // intuitive for me
    u_int8_t op_prec(TokenType tokenType) {
        switch(tokenType) {
            case TokenType::DOUB_AMPERSAND:
            case TokenType::DOUB_PIPE:
                return 0;
            case TokenType::DOUB_EQU:
            case TokenType::EXCLAM_EQU:
            case TokenType::GREATER:
            case TokenType::LESS:
            case TokenType::GREATER_EQU:
            case TokenType::LESS_EQU:
                return 1;
            case TokenType::PLUS:
            case TokenType::HYPHEN:
                return 2;
            case TokenType::ASTER:
            case TokenType::SLASH:
            case TokenType::PERCENT:
            case TokenType::AMPERSAND:
            case TokenType::PIPE:
            case TokenType::CARAT:
            case TokenType::DOUB_GREATER:
            case TokenType::DOUB_LESS:
            case TokenType::CARAT_GREATER:
            case TokenType::LESS_CARAT:
                return 3;
            default:
                return 255; //255 represents non operator type;
        }
    }
    // TODO: change the way op_type is returned
    u_int8_t op_prec_and_type(TokenType tokenType, OperatorType& op_type)
    {
        switch (tokenType) {
            case TokenType::DOUB_AMPERSAND:
            case TokenType::DOUB_PIPE:
                op_type = OperatorType::NONE;
                return 0;
            case TokenType::DOUB_EQU:
            case TokenType::EXCLAM_EQU:
            case TokenType::GREATER:
            case TokenType::LESS:
            case TokenType::GREATER_EQU:
            case TokenType::LESS_EQU:
                op_type = OperatorType::NONE;
                return 1;
            case TokenType::PLUS:
                op_type = OperatorType::ADD;
                return 2;
            case TokenType::HYPHEN:
                op_type = OperatorType::SUB;
                return 2;
            case TokenType::ASTER:
                op_type = OperatorType::MUL;
                return 3;
            case TokenType::SLASH:
                op_type = OperatorType::DIV;
                return 3;
            case TokenType::PERCENT:
                op_type = OperatorType::MOD;
                return 3;
            case TokenType::AMPERSAND:
            case TokenType::PIPE:
            case TokenType::CARAT:
            case TokenType::DOUB_GREATER:
            case TokenType::DOUB_LESS:
            case TokenType::CARAT_GREATER:
            case TokenType::LESS_CARAT:
                op_type = OperatorType::BITWISE;
                return 3;
            default:
                return 255; //255 represents non operator type;
        }
    }

    inline void update_rh_elem_full(ExprElementType type, Variable var_type, SubExprElement lh, SubExprElement rh, NodeExpr* expr) {
        m_right_element.lh.e = lh.e;
        m_right_element.lh.v = m_left_element.cur.v;
        m_right_element.rh.e = rh.e;
        m_right_element.rh.v = m_right_element.cur.v;
        m_right_element.t = type;
        m_right_element.cur.v = var_type;
        m_right_element.cur.e = expr;
    }

    inline void update_lh_elem_type(ExprElementType type, NodeExpr* expr) {
        m_left_element.t = type;
        m_left_element.cur.e = expr;
    }

    inline void update_rh_elem_type(ExprElementType type, NodeExpr* expr) {
        m_right_element.t = type;
        m_right_element.cur.e = expr;
    }

    void update_extra_space(int num) {
        if (num > m_used_regs) {
            m_used_regs = num;
        }
    }

public:
    bool successful = true;

    inline explicit Parser(const std::vector<Token> tokens)
        : m_tokens(std::move(tokens)),
        allocator(MEM_CHUNK_SIZE) { // also maximum variable length + 1
        m_last_main_reg_ops.assign({{0, 0}});
        m_left_element.cur.e = null_expr;
        m_right_element.cur.e = null_expr;}

    inline NodeRoot ParseProgram(){
        NodeRoot root;
        while (next().has_value()) {
            std::optional<NodeStmt*> stmt = parseStmt();
            if (stmt.has_value()) {
                root.stmts.push_back(stmt.value());
            } else {
                //Compiler has failed at this point, but will continue to try to compile the rest starting at the next semicolon or closed curly bracket
                while (next().has_value() && m_tokens[m_index].type != TokenType::SEMI) {
                    m_index++;
                }
                if(next().has_value()){
                    consume();
                }
            }
        }

        return root;
    }

    std::optional<NodeStmt*> parseStmt(bool needs_semi = true) {
        TokenType tokenType = m_tokens.at(m_index).type;
        NodeStmt* stmt = allocator.allocate<NodeStmt>();

        bool used_descriptor = false;

        //for descriptors
        while (true) {
            switch (tokenType) {
                case TokenType::K_D_CONST:
                    used_descriptor = true;
                    is_const = true;

                    consume();
                    if (next()) {
                        tokenType = m_tokens.at(m_index).type;
                    } else {
                        error("Expected statement after const discriptor");
                        return std::nullopt;
                    }
                    
                    break;
                case TokenType::K_D_UNSIGNED:
                    used_descriptor = true;
                    is_signed = false;

                    consume();
                    if (next()) {
                        tokenType = m_tokens.at(m_index).type;
                    } else {
                        error("Expected statement after unsigned discriptor");
                        return std::nullopt;
                    }
                    
                    break;
                default:
                    goto out; //its gross but its the one time gotos are actually kind of necessary
            }
        }
        out: //this is scary

        //for statements
        switch (tokenType) {
            case TokenType::K_FLOAT:
            case TokenType::K_DOUBLE:
            case TokenType::K_BYTE:
            case TokenType::K_SHORT:
            case TokenType::K_BOOL:
            case TokenType::K_CHAR:
            case TokenType::K_LONG:
            case TokenType::K_INT:
            {
                consume();
                std::optional<Token> ident = next();
                VarType type = var_associations.at(tokenType).first;
                m_cur_var_type = type;
                m_orig_allowed_type = Variable{.type = type, .is_signed = is_signed};
                m_stack_bytes += var_associations.at(tokenType).second;
                const char* name;

                if (ident.has_value() && ident.value().type == TokenType::IDENT) {
                    std::string var_name;
                    name = get_c_str(var_name);

                    //std::cout << "var: " << name << std::endl;

                    if (!expect(TokenType::EQU, "Expected '='")) {
                        return std::nullopt;
                    }
                } else {
                    error("Expected identifier");
                    return std::nullopt;
                }
                
                std::optional<ExprOut> expr = parseExpr(type);
                if (!expr.has_value()) {
                    return std::nullopt;
                }

                insert_last_implicit_cast(expr.value().expr);

                NodeStmtVarAssign* var_assign = allocator.allocate<NodeStmtVarAssign>();

                var_assign->ident = name;
                var_assign->type = type;
                var_assign->expr = expr.value().expr;

                if (type == VarType::FLOAT) {
                    var_assign->properties |= float_instruc;
                } else if (type == VarType::DOUBLE) {
                    var_assign->properties |= double_instruc;
                } else {
                    var_assign->properties |= is_signed << 3;
                }

                if (is_const) {
                    if (!is_cur_expr_dynamic) {
                        var_assign->properties |= const_assign;

                        //std::cout << "equals const num: " << \
                        //  std::to_string(std::get<NodeTermIntLit*>(std::get<NodeTerm*>(var_assign->expr->var)->var)->bin_num) << "\n\n";
                    }

                    m_const_variables.insert(name);
                }
                
                if (is_cur_expr_single && !is_cur_expr_dynamic) {
                    var_assign->properties |= term_assign;
                    if (is_expr_too_big_imm) {
                        var_assign->properties |= too_big_imm_assign;
                    }
                }

                m_variables.insert({name, Variable {.type = type, .is_signed = is_signed}});

                stmt->stmt = var_assign;

                break;
            }
            case TokenType::IDENT:
            {
                if (descriptor_error(used_descriptor)) {
                    return std::nullopt;
                }

                std::string var_name;
                const char* ident = get_c_str(var_name);

                // if (!variables.contains(var_name) ||
                //     ([this, &var_name] () -> bool {
                //         int num1 = std::get<1>(variables.at(var_name));
                //         int num2 = std::get<2>(variables.at(var_name));
                //         if (num1 > m_scope.first) {
                //             return true;
                //         } else if (num1 == m_scope.first && num2 != m_scope.second) {
                //             return true;
                //         } else {
                //             return false;
                //         }
                //     }())) {
                //     m_index--;
                //     std::cout << "Undeclared variable being set" << get_info() << std::endl;
                //     successful = false;
                //     return std::nullopt;
                // }

                if (!m_variables.contains(var_name)) {
                    m_index--;
                    error("Setting of undefined variable");
                    return std::nullopt;
                }

                if (m_const_variables.contains(var_name)) {
                    m_index--;
                    error("Setting of const variable");
                    return std::nullopt;
                }

                Variable var = m_variables.at(var_name);

                m_cur_var_type = var.type;
                m_orig_allowed_type = Variable{.type = var.type, .is_signed = is_signed};

                if (!expect(TokenType::EQU, "Expected '='")) {
                    return std::nullopt;
                }
                
                std::optional<ExprOut> expr = parseExpr(var.type);
                if (!expr.has_value()) {
                    return std::nullopt;
                }

                insert_last_implicit_cast(expr.value().expr);

                NodeStmtVarSet* var_set = allocator.allocate<NodeStmtVarSet>();

                var_set->type = var.type;
                var_set->ident = ident;
                var_set->expr = expr.value().expr;

                if (var.type == VarType::FLOAT) {
                    var_set->properties |= float_instruc;
                } else if (var.type == VarType::DOUBLE) {
                    var_set->properties |= double_instruc;
                } else {
                    var_set->properties |= var.is_signed << 3;
                }

                if (is_cur_expr_single && !is_cur_expr_dynamic) {
                    var_set->properties |= term_assign;
                    if (is_expr_too_big_imm) {
                        var_set->properties |= too_big_imm_assign;
                    }
                }

                stmt->stmt = var_set;
                
                break;
            }
            case TokenType::O_CURLY:
            {
                if (descriptor_error(used_descriptor)) {
                    return std::nullopt;
                }
                needs_semi = false;

                consume();
                if (!next().has_value()){
                    error("Expected statement after scope");
                    return std::nullopt;
                }
                
                m_scope_num++;
                // std::pair<int, int> last_scope = m_scope;
                // m_scope.first++;
                
                // if (!m_previous_scopes.contains(m_scope.first)) {
                //     m_previous_scopes.insert({m_scope.first, 0});
                //     m_scope.second = 0;
                // } else {
                //     int& num = m_previous_scopes.at(m_scope.first);
                //     num++;
                //     m_scope.second = num;
                // }
                std::vector<NodeStmt*> stmts;

                while (next().has_value() && m_tokens[m_index].type != TokenType::C_CURLY) {
                    std::optional<NodeStmt*> stmt = parseStmt();
                    if (!stmt.has_value()) {
                        //Compiler has failed at this point, but will continue to try to compile the rest starting at the next semicolon
                        while (next().has_value() && m_tokens[m_index].type != TokenType::SEMI && m_tokens[m_index].type != TokenType::C_CURLY) {
                            m_index++;
                        }
                        if(next().has_value()){
                            consume();
                        }

                        continue;
                    }

                    stmts.push_back(stmt.value());
                }

                if (m_scope_num == 1) {
                    NodeScope* scope = allocator.allocate<NodeScope>();
                    scope->stmts = std::move(stmts);
                    scope->num_bytes = m_stack_bytes + m_extra_space + m_extra_single_space;
                    scope->used_non_volatile_regs = std::min(m_used_regs - (MAX_REG_NUM - NON_VOLATILE_REG_NUM), NON_VOLATILE_REG_NUM); //doesnt really matter if its negative

                    stmt->stmt = scope;
                    m_stack_bytes = 0;
                    m_used_regs = 0;
                    m_extra_space = 0;
                    m_extra_single_space = 0;
                } else {
                    NodeStmtGroup* group = allocator.allocate<NodeStmtGroup>();
                    group->stmts = std::move(stmts);
                    
                    stmt->stmt = group;
                }

                if (!expect(TokenType::C_CURLY, "Expected closed curly bracket")) {
                    return std::nullopt;
                }

                m_scope_num--;

                // m_scope = last_scope;

                break;
            }
            case TokenType::K_IF:
            {
                if (descriptor_error(used_descriptor)) {
                    return std::nullopt;
                }
                needs_semi = false;

                consume();
                if (!expect(TokenType::O_PAREN, "Expected open parenthesis")) {
                    return std::nullopt;
                }
                
                std::optional<NodeExpr*> if_expr = parseIfStart();
                if (!if_expr.has_value()) {
                    return std::nullopt;
                }

                if (!expect(TokenType::C_PAREN, "Expected closed parenthesis")) {
                    return std::nullopt;
                }
                
                std::pair<Variable, bool> if_out = parseIfOut();

                std::optional<NodeStmt*> if_stmts = parseStmt();
                if (!if_stmts.has_value()) {
                    return std::nullopt;
                }

                if (if_out.first.type == VarType::UNKNOWN) {
                    if (if_out.second) {
                        NodeStmtGroup* stmt_group = allocator.allocate<NodeStmtGroup>();
                        stmt_group->stmts.push_back(if_stmts.value());
                        stmt->stmt = stmt_group;
                        break;
                    } else {
                        stmt->stmt = (NodeStmtBlank*) nullptr;
                        if (m_tokens[m_index].type == TokenType::K_ELSE) {
                            consume();
                        }
                        break;
                    }
                }

                NodeStmtIf* stmtIf = allocator.allocate<NodeStmtIf>();
                stmtIf = parseIfCreate<NodeStmtIf*>(stmtIf, if_expr.value(), if_stmts.value(), if_out);
                
                std::optional<Token> next_token = next();
                if (next_token.has_value() && next_token.value().type == TokenType::K_ELSE) {
                    consume();
                    std::optional<NodeStmt*> else_stmts = parseStmt();
                    if (!if_stmts.has_value()) {
                        return std::nullopt;
                    }

                    stmtIf->else_stmts = else_stmts;
                }

                stmt->stmt = stmtIf;

                break;
            }
            case TokenType::K_DO:
            case TokenType::K_WHILE:
            {
                if (descriptor_error(used_descriptor)) {
                    return std::nullopt;
                }
                needs_semi = false;

                NodeStmtWhile* stmtWhile = allocator.allocate<NodeStmtWhile>();
                std::optional<NodeStmt*> while_stmts;

                m_nested_loops++;

                if (tokenType == TokenType::K_DO) {
                    consume();
                    while_stmts = parseStmt();
                    if (!while_stmts.has_value()) {
                        // it doesn't matter if m_nested_loops is too high if parsing fails part-way through.
                        return std::nullopt;
                    }

                    if (!expect(TokenType::K_WHILE, "Expected while keyword after do statement")) {
                        return std::nullopt;
                    }

                    stmtWhile->do_while = true;
                } else {
                    consume();
                }

                if (!expect(TokenType::O_PAREN, "Expected open parenthesis")) {
                    return std::nullopt;
                }

                std::optional<NodeExpr*> if_expr = parseIfStart();
                if (!if_expr.has_value()) {
                    return std::nullopt;
                }

                if (!expect(TokenType::C_PAREN, "Expected closed parenthesis")) {
                    return std::nullopt;
                }

                std::pair<Variable, bool> if_out = parseIfOut();

                if (tokenType == TokenType::K_WHILE) {
                    while_stmts = parseStmt();
                    if (!while_stmts.has_value()) {
                        return std::nullopt;
                    }

                    stmtWhile->do_while = false;
                }

                if (if_out.first.type == VarType::UNKNOWN) {
                    if (if_out.second) {
                        stmtWhile->if_expr = std::nullopt;
                        stmtWhile->stmts = while_stmts.value();
                    } else {
                        stmt->stmt = (NodeStmtBlank*) nullptr;
                        break;
                    }
                } else {
                    stmtWhile = parseIfCreate<NodeStmtWhile*>(stmtWhile, if_expr.value(), while_stmts.value(), if_out);
                }

                m_nested_loops--;

                stmt->stmt = stmtWhile;

                break;
            }
            case TokenType::K_FOR:
            {
                if (descriptor_error(used_descriptor)) {
                    return std::nullopt;
                }
                needs_semi = false;

                consume();
                if (!expect(TokenType::O_PAREN, "Expected open parenthesis")) {
                    return std::nullopt;
                }

                std::optional<NodeStmt*> for_init_stmt = parseStmt();
                if (!for_init_stmt.has_value()) {
                    return std::nullopt;
                }

                m_nested_loops++;

                std::optional<NodeExpr*> if_expr = parseIfStart();
                if (!if_expr.has_value()) {
                    return std::nullopt;
                }

                if (!expect(TokenType::SEMI, "Expected semicolon")) {
                    return std::nullopt;
                }

                std::pair<Variable, bool> if_out = parseIfOut();

                 std::optional<NodeStmt*> for_end_stmt = parseStmt(false); 
                if (!for_end_stmt.has_value()) {
                    return std::nullopt;
                }

                if (!expect(TokenType::C_PAREN, "Expected closed parenthesis")) {
                    return std::nullopt;
                }

                std::optional<NodeStmt*> for_stmts = parseStmt();
                if (!for_stmts.has_value()) {
                    return std::nullopt;
                }

                NodeStmtFor* stmtFor = allocator.allocate<NodeStmtFor>();

                if (if_out.first.type == VarType::UNKNOWN) {
                    if (if_out.second) {
                        stmtFor->if_expr = std::nullopt;
                        stmtFor->stmts = for_stmts.value();
                    } else {
                        // For statements with a const false condition should still create initial var
                        NodeStmtGroup* stmt_group = allocator.allocate<NodeStmtGroup>();
                        stmt_group->stmts.push_back(for_init_stmt.value());
                        stmt->stmt = stmt_group;
                        break;
                    }
                } else {
                    stmtFor = parseIfCreate<NodeStmtFor*>(stmtFor, if_expr.value(), for_stmts.value(), if_out);
                }

                stmtFor->init_stmt = for_init_stmt.value();
                stmtFor->end_stmt = for_end_stmt.value();

                m_nested_loops--;

                stmt->stmt = stmtFor;

                break;
            }
            case TokenType::K_BREAK:
            {
                if (descriptor_error(used_descriptor)) {
                    return std::nullopt;
                }

                if (!m_nested_loops) {
                    error("Cannot use break statement outside of loop");
                    return std::nullopt;
                }

                consume();

                std::optional<size_t> loop = loop_statement_nested_check();
                if (!loop.has_value()) {
                    return std::nullopt;
                }

                NodeStmtBreak* breakStmt = allocator.allocate<NodeStmtBreak>();

                breakStmt->loop = loop.value();

                stmt->stmt = breakStmt;
                break;
            }
            case TokenType::K_CONTINUE:
            {
                if (descriptor_error(used_descriptor)) {
                    return std::nullopt;
                }

                if (!m_nested_loops) {
                    error("Cannot use continue statement outside of loop");
                    return std::nullopt;
                }

                consume();

                std::optional<size_t> loop = loop_statement_nested_check();
                if (!loop.has_value()) {
                    return std::nullopt;
                }

                NodeStmtContinue* continueStmt = allocator.allocate<NodeStmtContinue>();

                continueStmt->loop = loop.value();

                stmt->stmt = continueStmt;
                break;
            }
            case TokenType::K_EXIT:
            {
                if (descriptor_error(used_descriptor)) {
                    return std::nullopt;
                }

                consume();
                if (!expect(TokenType::O_PAREN, "Expected open parenthesis")) {
                    return std::nullopt;
                }
                NodeStmtExit* stmtExit = allocator.allocate<NodeStmtExit>();

                m_cur_var_type = VarType::INT_64;
                std::optional<ExprOut> expr = parseExpr(VarType::INT_64);
                if (!expr.has_value()) {
                    return std::nullopt;
                }

                insert_last_implicit_cast(expr.value().expr);

                stmtExit->val = expr.value().expr;
                stmt->stmt = stmtExit;

                if (!expect(TokenType::C_PAREN, "Expected closed parenthesis")) {
                    return std::nullopt;
                }
                break;
            }
            case TokenType::K_ASM:
            case TokenType::K_ASM_BSS:
            case TokenType::K_ASM_DATA:
            case TokenType::K_ASM_RODATA:
            {   
                AsmSection section = AsmSection::TEXT;
                switch (tokenType) {
                    case TokenType::K_ASM_BSS:
                        section = AsmSection::BSS;
                    case TokenType::K_ASM_DATA:
                        section = AsmSection::DATA;
                    case TokenType::K_ASM_RODATA:
                        section = AsmSection::RODATA;
                }

                std::string assembly;
                while (next().has_value() && m_tokens[m_index].type != TokenType::SEMI) {
                    // ADD STRINGS
                }
            }
            case TokenType::SEMI:
                // skips blank semicolon statement
                stmt->stmt = (NodeStmtBlank*) nullptr;
                break;
            default:
                error("Expected statement");
                return std::nullopt;
        }

        if (needs_semi && !expect(TokenType::SEMI, "Expected semicolon")) {
            return std::nullopt;
        }
        
        //reset flags
        is_signed = true;
        is_const = false;
        is_cur_expr_dynamic = false;
        is_cur_expr_single = true;
        m_elem_used_test = false;
        m_is_casting = 0;
        m_expr_pos = 0;
        //m_last_main_reg_ops.assign({{0, 0}}); //TODO: check situations to make sure vars like m_used_regs are reset with each statement

        return stmt;
    }
    //TODO:
    //!Flip expression for types of short or lower which use immediate on the right side
    // meat
    // Most of the parser's important parts are in this function, parseExpr handles all mathmatical expressons,
    // if statements, optimization of commutative operations, and splitting expressions into two parts if more
    // memory would be required to normally compute them
    std::optional<ExprOut> parseExpr(VarType allowed_type, bool new_paren = false, u_int8_t min_prec = 0, OperatorType op_type = OperatorType::NONE) { 
        NodeExpr* expr = allocator.allocate<NodeExpr>();
        SubExprElement expr_element;
        SubExprElement lh_sub_lh;
        SubExprElement lh_sub_rh;
        bool full_expr = false;
        bool used_two_part = false;
        long lh_by_parens = 0;
        int two_parts = -1;
                              
        std::optional<TermOut> term_out = parseTerm(allowed_type, expr, op_type, min_prec);
        if (!term_out.has_value()) {
            return std::nullopt;
        }

        TermOut lh = term_out.value();
        std::optional<ExprOut> rh_expr = ExprOut{};

        lh_sub_lh = m_left_element.cur;
        lh_sub_rh = m_right_element.rh;
        expr->var = lh.term;
        expr_element = lh.expr_element;

        while (true) {
            std::optional<Token> next_token = next();
            if (!next_token.has_value()) {
                break;
            }

            OperatorType op_type;
            u_int8_t prec = op_prec_and_type(next_token.value().type, op_type);
            if (prec == 255) {
                if (next_token.value().type != TokenType::C_PAREN && next_token.value().type != TokenType::SEMI) {
                    error("Bad operator or missing semicolon");
                    return std::nullopt;
                }
                if (full_expr || (!full_expr && lh.element_type != ExprElementType::PAREN)) {
                    update_rh_elem_full(lh.element_type, expr_element.v, expr_element, rh_expr.value().expr_element, expr_element.e);
                }
                if (!full_expr) { // Necessary for too big imm and casting
                    return ExprOut{.expr = expr, .expr_element = expr_element};
                }
                break;
            } else if (new_paren || (prec >= min_prec && min_prec != 0 && !full_expr)) {
                append_to_last_areg();
            }
            new_paren = false;

            if (prec < min_prec) {
                update_rh_elem_full(lh.element_type, expr_element.v, expr_element, rh_expr.value().expr_element, lh.expr_element.e); //TODO
                break;
            }

            bool lh_used_test = m_elem_used_test;
            m_elem_used_test = false; 

            consume();
            rh_expr = parseExpr(allowed_type, false, prec + 1, op_type);
            if (!rh_expr.has_value()) {
                successful = false;
                return std::nullopt;
            }

            NodeExpr* rh = rh_expr.value().expr;

            bool rh_used_test = m_elem_used_test;
            m_elem_used_test = false;

            NodeExpr* expr_cpy = allocator.allocate<NodeExpr>();
            if (lh_by_parens + std::min(m_used_regs_ctr, MAX_REG_NUM) >= MAX_REG_NUM) {
                if (m_used_regs_ctr <= SPLIT_EXPR_REG_LIMIT) {
                    used_two_part = true;
                    m_two_part_expr.push_back(allocator.allocate<NodeExprTwoPart>());
                    NodeExpr* expr_cpy_2 = allocator.allocate<NodeExpr>();
                    *expr_cpy_2 = *expr;
                    m_two_part_expr.back()->first = expr_cpy_2;
                    NodeTerm* last_var = allocator.allocate<NodeTerm>();
                    NodeTermIdent* last_var_ident = allocator.allocate<NodeTermIdent>();
                    last_var_ident->ident = "2";
                    last_var->var = last_var_ident;
                    expr_cpy->var = last_var;
                    lh_by_parens = 0;
                    two_parts++;
                    u_int8_t var_size = var_sizes.at(m_cur_var_type);
                    if (var_size > m_extra_single_space) {
                        m_extra_single_space = var_size;
                    }
                } else {
                    long extra_mem = (lh_by_parens - MAX_REG_NUM) * var_sizes.at(m_cur_var_type);
                    m_lock_main_reg = m_used_regs;
                    if (extra_mem > m_extra_space) {
                        m_extra_space = extra_mem;
                    }
                    *expr_cpy = *expr;
                }
            } else {
                *expr_cpy = *expr;
            }

            lh.expr_element.e = expr_cpy;
            m_left_element.lh = lh_sub_lh;
            m_left_element.rh = lh_sub_rh;

            if (!full_expr) {
                m_left_element.cur.v = lh.expr_element.v;
                update_lh_elem_type(lh.element_type, lh.expr_element.e);
            } else {
                update_lh_elem_type(ExprElementType::PAREN, lh.expr_element.e);
            }

            // there is a lot of convenient stuff with expression flipping that happens that allows
            // use_e to work in generate.hpp

            // relies on the fact that due to parseConstExpr there will never be two immediates in one expression
            switch (next_token.value().type) { // I wish this could be cleaner but there are slight variations for each operator
                case TokenType::ASTER:
                {
                    bool lh_im = m_left_element.t == ExprElementType::IMMEDIATE;
                    bool rh_im = m_right_element.t == ExprElementType::IMMEDIATE;
                    if (m_right_element.t == ExprElementType::PAREN) {
                        bool flip = (m_left_element.t != ExprElementType::PAREN && (is_signed || !lh_im) && !is_expr_too_big_imm);
                        expr->var = makeBinExpr<NodeExprBinMul, NodeExprBinMulByPar>(expr_cpy, rh, lh_by_parens, !flip, 
                            (lh_im ? flip ? 2 : 1 : 0), flip, flip);
                    } else if (rh_im && (!is_signed || is_expr_too_big_imm)) {
                        expr->var = makeBinExpr<NodeExprBinMul, NodeExprBinMulByPar>(expr_cpy, rh, lh_by_parens, 
                            (m_left_element.t == ExprElementType::PAREN), 1, false, true);
                    } else {
                        expr->var = makeBinExpr<NodeExprBinMul, NodeExprBinMulByPar>(expr_cpy, rh, lh_by_parens, false, 
                            (lh_im ? 1 : rh_im ? 2 : 0), (m_left_element.t == ExprElementType::PAREN), false);
                    }
                    if (!is_signed) {
                        // TODO: dont do this if num is in optimize_mul in generate.hpp
                        m_last_main_reg_ops.back().second = m_expr_pos + 1;
                    }
                    break;
                }
                case TokenType::PLUS:
                {
                    expr->var = makeBinExprCommutative<NodeExprBinAdd, NodeExprBinAddByPar>(expr_cpy, rh, lh_by_parens);
                    break;
                }
                case TokenType::HYPHEN:
                    expr->var = makeBinExpr<NodeExprBinSub, NodeExprBinSubByPar>(expr_cpy, rh, lh_by_parens, 
                        (m_right_element.t == ExprElementType::PAREN), (m_right_element.t == ExprElementType::IMMEDIATE), false, false);
                    break;
                case TokenType::SLASH:
                    expr->var = makeBinExpr<NodeExprBinDiv, NodeExprBinDivByPar>(expr_cpy, rh, lh_by_parens, 
                        (m_right_element.t == ExprElementType::PAREN), (m_right_element.t == ExprElementType::IMMEDIATE), false, false);
                    if (m_cur_var_type != VarType::FLOAT && m_cur_var_type != VarType::DOUBLE) {
                        m_last_main_reg_ops.back().second = m_expr_pos + 1;
                    }
                    break;
                case TokenType::PERCENT:
                    if (m_cur_var_type == VarType::FLOAT || m_cur_var_type == VarType::DOUBLE) {
                        error("Cannot use modulus on float operands");
                        return std::nullopt;
                    }
                    expr->var = makeBinExpr<NodeExprBinMod, NodeExprBinModByPar>(expr_cpy, rh, lh_by_parens, 
                        (m_right_element.t == ExprElementType::PAREN), (m_right_element.t == ExprElementType::IMMEDIATE), false, false);
                    m_last_main_reg_ops.back().second = m_expr_pos + 1;
                    break;
                case TokenType::AMPERSAND:
                    if (!makeBinExprBitwise<NodeExprBinAnd, NodeExprBinAndByPar, false>(expr, expr_cpy, rh, lh_by_parens)) {
                        return std::nullopt;
                    }
                    break;
                case TokenType::PIPE:
                    if (!makeBinExprBitwise<NodeExprBinOr, NodeExprBinOrByPar, false>(expr, expr_cpy, rh, lh_by_parens)) {
                        return std::nullopt;
                    }
                    break;
                case TokenType::CARAT:
                    if (!makeBinExprBitwise<NodeExprBinXor, NodeExprBinXorByPar, false>(expr, expr_cpy, rh, lh_by_parens)) {
                        return std::nullopt;
                    }
                    break;
                case TokenType::DOUB_GREATER:
                    if (!makeBinExprBitwise<NodeExprBinRShift, NodeExprBinRShiftByPar, true>(expr, expr_cpy, rh, lh_by_parens)) {
                        return std::nullopt;
                    }
                    break;
                case TokenType::DOUB_LESS:
                    if (!makeBinExprBitwise<NodeExprBinLShift, NodeExprBinLShiftByPar, true>(expr, expr_cpy, rh, lh_by_parens)) {
                        return std::nullopt;
                    }
                    break;
                case TokenType::CARAT_GREATER:
                    if (!makeBinExprBitwise<NodeExprBinRRoll, NodeExprBinRRollByPar, true>(expr, expr_cpy, rh, lh_by_parens)) {
                        return std::nullopt;
                    }
                    break;
                case TokenType::LESS_CARAT:
                    if (!makeBinExprBitwise<NodeExprBinLRoll, NodeExprBinLRollByPar, true>(expr, expr_cpy, rh, lh_by_parens)) {
                        return std::nullopt;
                    }
                    break;
                case TokenType::DOUB_EQU:
                    expr->var = makeLogExpr<NodeExprLogCompareEqu, NodeExprLogCompareEquByPar, NodeExprLogCompareEqu>(
                                            expr_cpy, rh, lh_used_test, rh_used_test);
                    break;
                case TokenType::EXCLAM_EQU:
                    expr->var = makeLogExpr<NodeExprLogCompareNotEqu, NodeExprLogCompareNotEquByPar, NodeExprLogCompareNotEqu>(
                                            expr_cpy, rh, lh_used_test, rh_used_test);
                    break;
                case TokenType::GREATER:
                    expr->var = makeLogExpr<NodeExprLogCompareGreater, NodeExprLogCompareGreaterByPar, NodeExprLogCompareLess>(
                                            expr_cpy, rh, lh_used_test, rh_used_test);
                    break;
                case TokenType::LESS:
                    expr->var = makeLogExpr<NodeExprLogCompareLess, NodeExprLogCompareLessByPar, NodeExprLogCompareGreater>(
                                            expr_cpy, rh, lh_used_test, rh_used_test);
                    break;
                case TokenType::GREATER_EQU:
                    expr->var = makeLogExpr<NodeExprLogCompareGreaterEqu, NodeExprLogCompareGreaterEquByPar, NodeExprLogCompareLessEqu>(
                                            expr_cpy, rh, lh_used_test, rh_used_test);
                    break;
                case TokenType::LESS_EQU:
                    expr->var = makeLogExpr<NodeExprLogCompareLessEqu, NodeExprLogCompareLessEquByPar, NodeExprLogCompareGreaterEqu>(
                                            expr_cpy, rh, lh_used_test, rh_used_test);
                    break;
                case TokenType::DOUB_AMPERSAND:
                    expr->var = makeLogExprOp<NodeExprLogOpAnd>(expr_cpy, rh, lh_used_test, rh_used_test);
                    break;
                case TokenType::DOUB_PIPE:
                    expr->var = makeLogExprOp<NodeExprLogOpOr>(expr_cpy, rh, lh_used_test, rh_used_test);
                    break;
                default:
                    error("Something went very wrong");
                    return std::nullopt;
            }
            
            expr_element.e = expr;
            lh_sub_lh = m_left_element.cur;
            lh_sub_rh = m_right_element.cur;
            full_expr = true;
        }
        
        // There is no actual typed parenthesis if it does this but basically it means an expression like:
        // int x = 5 * y + z * 8;
        // Will be interpreted as:
        // int x = (5 * y) + (z * 8)
        // So that way it can store everything in the right registers
        if (full_expr) {
            update_rh_elem_type(ExprElementType::PAREN, expr_element.e);
        }

        if (used_two_part) {
            NodeExpr* expr_cpy = allocator.allocate<NodeExpr>();
            *expr_cpy = *expr;
            m_two_part_expr.back()->second = expr_cpy;
            while (two_parts) {
                NodeExpr* second_expr = allocator.allocate<NodeExpr>();
                size_t second_to_last = m_two_part_expr.size() - 2;
                m_two_part_expr.at(second_to_last)->second = second_expr;
                m_two_part_expr.at(second_to_last)->second->var = m_two_part_expr.back();
                m_two_part_expr.pop_back();
                two_parts--;
            }
            expr->var = m_two_part_expr.back();
        }

        m_is_casting = 0;
        is_expr_too_big_imm = false;
        return ExprOut{.expr = expr, .expr_element = expr_element};
    }

    template <typename T, typename T2>
    inline NodeExprBin* makeBinExprCommutative(NodeExpr*& expr_cpy, NodeExpr*& rh, long& lh_by_parens) {
        bool lh_im = m_left_element.t == ExprElementType::IMMEDIATE;
        bool rh_im = m_right_element.t == ExprElementType::IMMEDIATE;
        if (m_right_element.t == ExprElementType::PAREN) {
            bool flip = (m_left_element.t != ExprElementType::PAREN && !is_expr_too_big_imm);
            return makeBinExpr<T, T2>(expr_cpy, rh, lh_by_parens, !flip, false, false, flip);
        } else if (rh_im && is_expr_too_big_imm) {
            return makeBinExpr<T, T2>(expr_cpy, rh, lh_by_parens, 
                (m_left_element.t == ExprElementType::PAREN), false, false, true);
        } else {
            return makeBinExpr<T, T2>(expr_cpy, rh, lh_by_parens, false, false, false, false);
        }
    }

    template <typename T, typename T2, bool shift>
    inline bool makeBinExprBitwise(NodeExpr*& expr, NodeExpr*& expr_cpy, NodeExpr*& rh, long& lh_by_parens) {
        if (m_cur_var_type == VarType::FLOAT || m_cur_var_type == VarType::DOUBLE) {
            error("Cannot use bitwise operator on float");
            return false;
        }
        if constexpr (!shift) {
            expr->var = makeBinExprCommutative<T, T2>(expr_cpy, rh, lh_by_parens);
            return true;
        } else {
            expr->var = makeBinExpr<T, T2>(expr_cpy, rh, lh_by_parens, 
                m_right_element.t == ExprElementType::PAREN, m_right_element.t == ExprElementType::IMMEDIATE, false, false);
            return true;
        }
    }

    template <typename T, typename T2>
    NodeExprBin* makeBinExpr(NodeExpr* lh, NodeExpr* rh, long& lh_by_parens, bool by_paren, char by_immediate, bool lh_paren, bool flip) {
        m_expr_pos++;
        if (!by_paren) {
            if (flip && m_right_element.t == ExprElementType::PAREN) {
                m_used_regs_ctr--;
            }
            return makeBinExprS<T>(lh, rh, by_immediate, lh_paren, flip);
        } else { //TODO: update m_used_regs
            lh_by_parens++;
            return makeBinExprS<T2>(lh, rh, by_immediate, lh_paren, flip);
        }
    }

    template <typename T>
    NodeExprBin* makeBinExprS(NodeExpr* lh, NodeExpr* rh, char by_immediate, bool lh_paren, bool flip) {
        NodeExprBin* expr = allocator.allocate<NodeExprBin>();
        T* operation = allocator.allocate<T>();
                    
        if (!flip) {
            operation->lh = lh;
            operation->rh = rh;
        } else {
            operation->rh = lh;
            operation->lh = rh;
        }
        // done at compile time, this makes things much much nicer
        if constexpr (std::is_same_v<T, NodeExprBinSub> || std::is_same_v<T, NodeExprBinMul> || 
                        std::is_same_v<T, NodeExprBinMulByPar> || std::is_same_v<T, NodeExprBinDiv> || 
                        std::is_same_v<T, NodeExprBinMod> || std::is_same_v<T, NodeExprBinRShift> || 
                        std::is_same_v<T, NodeExprBinLShift> || std::is_same_v<T, NodeExprBinRRoll> || 
                        std::is_same_v<T, NodeExprBinLRoll>) {
            operation->by_immediate = by_immediate;
        }
        
        if constexpr (std::is_same_v<T, NodeExprBinMul>) {
            operation->lh_paren = lh_paren;
        }

        bool rh_casted = insert_implicit_casts();

        if constexpr (std::is_same_v<T, NodeExprBinAddByPar> || std::is_same_v<T, NodeExprBinSubByPar> || 
                        std::is_same_v<T, NodeExprBinMulByPar> || std::is_same_v<T, NodeExprBinDivByPar> ||
                        std::is_same_v<T, NodeExprBinModByPar> || std::is_same_v<T, NodeExprBinAndByPar> || 
                        std::is_same_v<T, NodeExprBinOrByPar> || std::is_same_v<T, NodeExprBinXorByPar> || 
                        std::is_same_v<T, NodeExprBinRShiftByPar> || std::is_same_v<T, NodeExprBinLShiftByPar> ||
                        std::is_same_v<T, NodeExprBinRRollByPar> || std::is_same_v<T, NodeExprBinLRollByPar>) {
            // Conveniently this actually still works if the expression is flipped based on the flip conditions
            // (expressions which are flipped can't have a_reg ops on right side since that would be a paren statement)
            operation->last_areg_op = get_last_areg_amt(rh_casted);
            m_last_main_reg_ops.pop_back();
        }

        expr->expr = operation;
        
        is_cur_expr_single = false;
        return expr;
    }

    inline void add_if_test(NodeExpr*& expr, ExprElementType type) {
        if (m_use_if_test && !m_elem_used_test) {
            m_expr_pos++;
            NodeExpr* expr_tmp = allocator.allocate<NodeExpr>();
            NodeExprLog* log_expr = allocator.allocate<NodeExprLog>();
            NodeExprLogTest* log_test = allocator.allocate<NodeExprLogTest>(type, m_last_main_reg_ops, m_expr_pos);

            log_test->expr = expr;
            log_expr->var = log_test;
            expr_tmp->var = log_expr;
            expr = expr_tmp;
        }
    }

    template <typename T, typename T2, typename T3>
    NodeExprLog* makeLogExpr(NodeExpr*& lh, NodeExpr*& rh, bool lh_used_test, bool rh_used_test) {
        m_use_if_test = false;
        m_expr_pos++;
        m_last_main_reg_ops.back().second = m_expr_pos; // Conditional statements must use main reg
        if (m_right_element.t == ExprElementType::PAREN) {
            if (!(m_left_element.t == ExprElementType::IMMEDIATE && is_expr_too_big_imm) && m_left_element.t != ExprElementType::PAREN) {
                std::swap(m_left_element, m_right_element);
                return makeLogExprS<T3>(rh, lh, m_right_element.cur.e, m_left_element.cur.e, rh_used_test, lh_used_test, m_right_element.t, m_left_element.t);
            } else {
                return makeLogExprS<T2>(lh, rh, m_left_element.cur.e, m_right_element.cur.e, lh_used_test, rh_used_test, m_left_element.t, m_right_element.t);
            }
        } else if (m_right_element.t == ExprElementType::IMMEDIATE && is_expr_too_big_imm) {
            if (m_left_element.t == ExprElementType::PAREN) {
                std::swap(m_left_element, m_right_element);
                return makeLogExprS<T2>(rh, lh, m_right_element.cur.e, m_left_element.cur.e, rh_used_test, lh_used_test, m_right_element.t, m_left_element.t);
            } else {
                return makeLogExprS<T3>(rh, lh, m_right_element.cur.e, m_left_element.cur.e, rh_used_test, lh_used_test, m_right_element.t, m_left_element.t);
            }
        } else if (m_left_element.t == ExprElementType::IMMEDIATE && m_right_element.t == ExprElementType::MEMORY) {
            std::swap(m_left_element, m_right_element);
            return makeLogExprS<T3>(rh, lh, m_right_element.cur.e, m_left_element.cur.e, rh_used_test, lh_used_test, m_right_element.t, m_left_element.t);
        } else {
            return makeLogExprS<T>(lh, rh, m_left_element.cur.e, m_right_element.cur.e, lh_used_test, rh_used_test, m_left_element.t, m_right_element.t);
        }   
    }

    template <typename T>
    NodeExprLog* makeLogExprS(NodeExpr*& lh, NodeExpr*& rh, NodeExpr*& lh_skip_par, NodeExpr*& rh_skip_par, bool lh_used_test, 
                                bool rh_used_test, ExprElementType lh_elem, ExprElementType rh_elem) {
        NodeExprLog* log_expr = allocator.allocate<NodeExprLog>();
        T* operation = allocator.allocate<T>();

        operation->lh = lh;
        operation->rh = rh;

        CompareResult num_result_val;
        // I am so utterly completely confused why the () ref = () arg is necesssary but it doesnt compile without it
        // TODO: could maybe replace this with a [[assume]]
        auto set_num_result = overloaded {
            [&num_result_val](auto&& arg) {CompareResult& ref = (CompareResult&) arg->num_result; ref = num_result_val;},
            [&num_result_val](NodeExprLogTest* arg) {;}
        };

        if (lh_used_test) {
            if (rh_used_test) {
                num_result_val = CompareResult::FORCE_INT_NUM;
                std::visit(set_num_result, std::get<NodeExprLog*>(lh_skip_par->var)->var);
                num_result_val = CompareResult::SUB_RH_NUM;
                std::visit(set_num_result, std::get<NodeExprLog*>(rh_skip_par->var)->var); 
            } else {
                num_result_val = CompareResult::SUB_NUM;
                std::visit(set_num_result, std::get<NodeExprLog*>(lh_skip_par->var)->var);
            }
        } else if (rh_used_test) {
            num_result_val = CompareResult::SUB_NUM;
            std::visit(set_num_result, std::get<NodeExprLog*>(rh_skip_par->var)->var);
        }

        bool rh_casted = insert_implicit_casts();
        
        if constexpr (std::is_same_v<T, NodeExprLogCompareEqu> || std::is_same_v<T, NodeExprLogCompareNotEqu> ||
                        std::is_same_v<T, NodeExprLogCompareGreater> || std::is_same_v<T, NodeExprLogCompareGreaterEqu> ||
                        std::is_same_v<T, NodeExprLogCompareLess> || std::is_same_v<T, NodeExprLogCompareLessEqu>) {
            operation->lh_ident = (lh_elem == ExprElementType::MEMORY);
            operation->rh_ident = (rh_elem == ExprElementType::MEMORY);
        } else {
            operation->lh_ident = (lh_elem == ExprElementType::MEMORY); //!Update lh_used_parens_here
            if (lh_used_test && rh_used_test) { // Not optimal but much cleaner
                operation->comp_as_int = true;
            }
            operation->last_areg_op = get_last_areg_amt(rh_casted);
            m_last_main_reg_ops.pop_back();
        }
        
        operation->num_result = static_cast<CompareResult>(!m_in_if_stmt);
        m_elem_used_test = true;
        log_expr->var = operation;
        return log_expr;
    }
    
    template <typename T>
    NodeExprLog* makeLogExprOp(NodeExpr* lh, NodeExpr* rh, bool lh_used_test, bool rh_used_test) {
        m_expr_pos++;
        NodeExprLog* log_expr = allocator.allocate<NodeExprLog>();
        T* operation = allocator.allocate<T>();

        if (!lh_used_test) { //TODO: make this a function
            NodeExpr* lh_temp = allocator.allocate<NodeExpr>();
            NodeExprLog* lh_log_expr = allocator.allocate<NodeExprLog>();
            NodeExprLogTest* log_test = allocator.allocate<NodeExprLogTest>(m_left_element.t, m_last_main_reg_ops, m_expr_pos);

            log_test->expr = lh;
            lh_log_expr->var = log_test;
            lh_temp->var = lh_log_expr;
            lh = lh_temp;
        }
        if (!rh_used_test) {
            NodeExpr* rh_temp = allocator.allocate<NodeExpr>();
            NodeExprLog* rh_log_expr = allocator.allocate<NodeExprLog>();
            NodeExprLogTest* log_test = allocator.allocate<NodeExprLogTest>(m_right_element.t, m_last_main_reg_ops, m_expr_pos);

            log_test->expr = rh;
            rh_log_expr->var = log_test;
            rh_temp->var = rh_log_expr;
            rh = rh_temp;
        }

        m_elem_used_test = true;
        operation->lh = lh;
        operation->rh = rh;
        operation->num_result = !m_use_if_test;
        log_expr->var = operation;
        return log_expr;
    }

    std::optional<NodeExpr*> parseIfStart() {
        m_in_if_stmt = true;
        m_use_if_test = true;
        m_decided_type.type = VarType::UNKNOWN;

        std::optional<ExprOut> if_expr = parseExpr(VarType::UNKNOWN);
        if (!if_expr.has_value()) {
            return std::nullopt;
        }

        insert_last_implicit_cast(if_expr.value().expr);

        add_if_test(if_expr.value().expr, m_right_element.t);

        m_use_if_test = false;
        m_in_if_stmt = false;

        return if_expr.value().expr;
    }

    std::pair<Variable, bool> parseIfOut() {
        // reset flags except signed and const
        is_cur_expr_dynamic = false;
        is_cur_expr_single = true;
        m_elem_used_test = false;
        m_is_casting = 0;
        m_expr_pos = 0;
        //m_last_main_reg_ops.assign({{0, 0}});

        return std::pair<Variable, bool>{m_decided_type, m_last_imm_truth};
    }

    template <typename T>
    T parseIfCreate(T node, NodeExpr* if_expr, NodeStmt* if_stmts, std::pair<Variable, bool> if_out) {
        node->if_expr_type.type = if_out.first.type;
        node->if_expr_type.is_signed = if_out.first.is_signed;
        node->if_expr = if_expr;
        node->stmts = if_stmts;

        return node;
    }

    // allowed_type and expr_element should be copied
    std::optional<TermOut> parseTerm(VarType& allowed_type, NodeExpr* expr_element, const OperatorType& op_type, u_int8_t expr_min_prec = 0) {
        NodeTerm* term = allocator.allocate<NodeTerm>();

        std::optional<Token> token = next();
        
        if (!token.has_value()) {
            error("Expected value");
            return std::nullopt;
        }

        ExprElementType element_type;
        bool is_term_signed;
        SubExprElement lh_transit_element; //doesnt matter if its set while not-initialized it wont be used
        SubExprElement rh_transit_element;
        
        switch (token.value().type) {
            case TokenType::U_INT_LIT:
            {
                std::optional<NodeTermIntLit*> int_lit = invoke_parse_int_lit<unsigned int>(allowed_type, op_type, token.value());
                if (!int_lit.has_value() && !check_type(VarType::INT_32, false, allowed_type)) {
                    return std::nullopt;
                }
                term->var = int_lit.value();
                element_type = ExprElementType::IMMEDIATE;
                is_term_signed = false;
                break;
            }
            case TokenType::U_LONG_LIT:
            {
                std::optional<NodeTermIntLit*> int_lit = invoke_parse_int_lit<unsigned long>(allowed_type, op_type, token.value());
                if (!int_lit.has_value() && !check_type(VarType::INT_64, false, allowed_type)) {
                    return std::nullopt;
                }
                term->var = int_lit.value();
                element_type = ExprElementType::IMMEDIATE;
                is_term_signed = false;
                break;
            }
            case TokenType::FLOAT_LIT:
            case TokenType::N_FLOAT_LIT:
            {
                std::optional<NodeTermIntLit*> int_lit;
                if (allowed_type == VarType::DOUBLE) {
                    int_lit = parse_int_lit<double>(op_type, token.value());
                    if (!check_type(VarType::DOUBLE, true, allowed_type)) {
                        return std::nullopt;
                    }
                } else {
                    int_lit = parse_int_lit<float>(op_type, token.value());
                    if (!check_type(VarType::FLOAT, true, allowed_type)) {
                        return std::nullopt;
                    }
                }
                if (!int_lit.has_value()) {
                    return std::nullopt;
                }
                term->var = int_lit.value();
                element_type = ExprElementType::IMMEDIATE;
                is_term_signed = true;
                break;
            }
            case TokenType::INT_LIT:
            case TokenType::N_INT_LIT:
            {
                std::optional<NodeTermIntLit*> int_lit;
                switch (allowed_type) {
                    case VarType::INT_32:
                    case VarType::UNKNOWN:
                        if (is_signed) {
                            int_lit = invoke_parse_int_lit<signed int>(allowed_type, op_type, token.value());
                        } else {
                            int_lit = invoke_parse_int_lit<unsigned int>(allowed_type, op_type, token.value());
                        }
                        break;
                    case VarType::INT_64:
                        if (is_signed) {
                            int_lit = invoke_parse_int_lit<signed long>(allowed_type, op_type, token.value());
                        } else {
                            int_lit = invoke_parse_int_lit<unsigned long>(allowed_type, op_type, token.value());
                        }
                        break;
                    case VarType::FLOAT:
                        int_lit = invoke_parse_int_lit<float>(allowed_type, op_type, token.value());
                        break;
                    case VarType::DOUBLE:
                        int_lit = invoke_parse_int_lit<double>(allowed_type, op_type, token.value());
                        break;
                    case VarType::INT_16:
                        if (is_signed) {
                            int_lit = invoke_parse_int_lit<signed short>(allowed_type, op_type, token.value());
                        } else {
                            int_lit = invoke_parse_int_lit<unsigned short>(allowed_type, op_type, token.value());
                        }
                        break;
                    case VarType::BYTE:
                    case VarType::BOOL:
                        if (is_signed) {
                            int_lit = invoke_parse_int_lit<signed char>(allowed_type, op_type, token.value());
                        } else {
                            int_lit = invoke_parse_int_lit<unsigned char>(allowed_type, op_type, token.value());
                        }
                        break;
                    default:
                        error("Something went wrong with start getting int token");
                        return std::nullopt;
                }
                if (!int_lit.has_value()) {
                    return std::nullopt;
                }
                term->var = int_lit.value();
                element_type = ExprElementType::IMMEDIATE;
                is_term_signed = is_signed;
                break;
            }
            case TokenType::LONG_LIT:
            case TokenType::N_LONG_LIT:
            {
                if (allowed_type != VarType::INT_64) {
                    error("Long literal used in 32 bit expression");
                    return std::nullopt;
                }
                std::optional<NodeTermIntLit*> int_lit = invoke_parse_int_lit<long>(allowed_type, op_type, token.value());
                if (!int_lit.has_value() && !check_type(VarType::INT_64, true, allowed_type)) {
                    return std::nullopt;
                }
                term->var = int_lit.value();
                element_type = ExprElementType::IMMEDIATE;
                is_term_signed = true;
                break;
            }
            case TokenType::IDENT:
            {
                NodeTermIdent* ident = allocator.allocate<NodeTermIdent>();

                std::string var_name;
                ident->ident = get_c_str(var_name);

                // if (!variables.contains(var_name) ||
                //     ([this, &var_name] () -> bool {
                //         int num1 = std::get<1>(variables.at(var_name));
                //         int num2 = std::get<2>(variables.at(var_name));
                //         if (num1 > m_scope.first) {
                //             return true;
                //         } else if (num1 == m_scope.first && num2 != m_scope.second) {
                //             return false;
                //         } else {
                //             return true;
                //         }
                //     }())) {
                //     m_index--;
                //     std::cout << "Undefined variable" << get_info() << std::endl;
                //     successful = false;
                //     return std::nullopt;
                // }

                if (!m_variables.contains(var_name)) {
                    m_index--;
                    error("Undefined variable");
                    return std::nullopt;
                }

                Variable var = m_variables.at(var_name);
                if (!check_type(var.type, var.is_signed, allowed_type)) {
                    return std::nullopt;
                }

                is_cur_expr_dynamic = true;

                element_type = ExprElementType::MEMORY;
                is_term_signed = var.is_signed;
                term->var = ident;
                break;
            }
            case TokenType::O_PAREN:
            {
                NodeTermParen* paren = allocator.allocate<NodeTermParen>();

                consume();

                if (next(2).has_value()) { // A valid cast must have at least 3 tokens before EOF
                    bool is_cast = false;
                    bool round_float = false;
                    Variable type;
                    size_t tmp_index = m_index;
                    if (m_tokens[tmp_index].type == TokenType::TILDE) {
                        round_float = true;
                        tmp_index++;
                    } else {
                        round_float = false;
                    }
                    if (m_tokens[tmp_index].type == TokenType::K_D_UNSIGNED) {
                        type.is_signed = false;
                        type.type = VarType::INT_32;
                        is_cast = true;
                        tmp_index++;
                    } else {
                        type.is_signed = true;
                    }

                    if (m_tokens[tmp_index].type == TokenType::K_LONG || m_tokens[tmp_index].type == TokenType::K_INT ||
                        m_tokens[tmp_index].type == TokenType::K_SHORT || m_tokens[tmp_index].type == TokenType::K_BYTE ||
                        m_tokens[tmp_index].type == TokenType::K_BOOL || m_tokens[tmp_index].type == TokenType::K_FLOAT ||
                        m_tokens[tmp_index].type == TokenType::K_DOUBLE) {
                        is_cast = true;
                        type.type = var_associations.at(m_tokens[tmp_index].type).first;
                        m_index = tmp_index + 1;
                    }

                    if (is_cast) {
                        if (!expect(TokenType::C_PAREN, "Expected closed parenthesis after cast statement")) {
                            return std::nullopt;
                        }

                        if (!check_type(type.type, type.is_signed, allowed_type)) {
                            return std::nullopt;
                        }

                        NodeExprCast* expr_cast = allocator.allocate<NodeExprCast>();

                        expr_cast->round_float = round_float;
                        
                        VarType new_allowed_type = VarType::UNKNOWN;
                        std::optional<TermOut> cast_term = parseTerm(new_allowed_type, expr_element, OperatorType::NONE);
                        if (!cast_term.has_value()) {
                            return std::nullopt;
                        }
                        element_type = cast_term.value().element_type;
                        
                        // This is when an actual cast will happen, other situations will do nothing
                        switch (var_switch(m_decided_type.type, type.type)) {
                            case var_switch(VarType::FLOAT, VarType::INT_32):
                            case var_switch(VarType::FLOAT, VarType::INT_16):
                            case var_switch(VarType::FLOAT, VarType::BYTE):
                            case var_switch(VarType::FLOAT, VarType::INT_64):
                            case var_switch(VarType::FLOAT, VarType::DOUBLE):
                            case var_switch(VarType::DOUBLE, VarType::INT_32):
                            case var_switch(VarType::DOUBLE, VarType::INT_16):
                            case var_switch(VarType::DOUBLE, VarType::BYTE):
                            case var_switch(VarType::DOUBLE, VarType::INT_64): 
                            case var_switch(VarType::DOUBLE, VarType::FLOAT):
                            case var_switch(VarType::INT_32, VarType::FLOAT):
                            case var_switch(VarType::INT_16, VarType::FLOAT):
                            case var_switch(VarType::BYTE, VarType::FLOAT):
                            case var_switch(VarType::INT_64, VarType::FLOAT):
                            case var_switch(VarType::INT_32, VarType::DOUBLE):
                            case var_switch(VarType::INT_16, VarType::DOUBLE):
                            case var_switch(VarType::BYTE, VarType::DOUBLE):
                            case var_switch(VarType::INT_32, VarType::INT_64):
                            case var_switch(VarType::INT_16, VarType::INT_64):
                            case var_switch(VarType::BYTE, VarType::INT_64):
                            case var_switch(VarType::INT_32, VarType::BOOL):
                            case var_switch(VarType::INT_16, VarType::BOOL):
                            case var_switch(VarType::BYTE, VarType::BOOL):
                            case var_switch(VarType::INT_64, VarType::BOOL):
                            case var_switch(VarType::FLOAT, VarType::BOOL):
                            case var_switch(VarType::DOUBLE, VarType::BOOL):
                                m_is_casting = 1;
                                append_to_last_areg();
                                element_type = ExprElementType::PAREN;
                        }

                        NodeExpr* term_holder = allocator.allocate<NodeExpr>();

                        expr_cast->from_type = m_decided_type;
                        term_holder->var = cast_term.value().term;
                        expr_cast->expr = term_holder;

                        is_cur_expr_single = false;
                        allowed_type = type.type;
                        is_signed = type.is_signed;
                        m_cur_var_type = allowed_type;

                        NodeExpr* expr = allocator.allocate<NodeExpr>();

                        expr->var = expr_cast;
                        paren->expr = expr;
                        paren->only_too_large_imm = false;

                        is_term_signed = is_signed;
                        term->var = paren;
                        break;
                    }
                }

                append_to_last_areg();
                int prev_used_regs_ctr = m_used_regs_ctr;
                m_used_regs_ctr++;

                std::optional<ExprOut> paren_expr = parseExpr(allowed_type, expr_min_prec);
                if (!paren_expr.has_value()) {
                    return std::nullopt;
                }

                expr_element = paren_expr.value().expr;

                // if parse_expr only parses one term and ends in a paren this is still set to true afterward
                if (is_expr_too_big_imm && !fits_in_immediate(std::get<NodeTermIntLit*>(std::get<NodeTerm*>(paren_expr.value().expr->var)->var)->bin_num, m_cur_var_type)) {
                    paren->only_too_large_imm = true;
                }

                if (!expect(TokenType::C_PAREN, "Expected closed parenthesis")) {
                    return std::nullopt;
                }

                is_cur_expr_dynamic = true;
                is_cur_expr_single = false; // not optimal but I dont think this will ever be important

                // if a commutative operation expression was flipped then m_used_regs_ctr will stay the same
                update_extra_space(m_used_regs_ctr);
                m_used_regs_ctr = prev_used_regs_ctr;
                
                element_type = ExprElementType::PAREN;
                is_term_signed = is_signed;
                paren->expr = paren_expr.value().expr;
                term->var = paren;
                break;
            }
            case TokenType::TILDE:
            case TokenType::HYPHEN:
            { // nots are done here since it is an operator on one single thing
                if (allowed_type == VarType::FLOAT || allowed_type == VarType::DOUBLE) {
                    error("Cannot use bitwise not operator in float expression");
                    return std::nullopt;
                }

                is_term_signed = is_signed;
                
                std::optional<Token> next_token = next(1);
                bool _not = (token.value().type == TokenType::TILDE);
                if (next_token.has_value()) {
                    if (next_token.value().type == TokenType::O_PAREN) {
                        if (is_paren_const()) {
                            m_index += 2;
                            std::optional<TermOut> single_op_term = parseTerm(allowed_type, expr_element, OperatorType::NONE);
                            if (!single_op_term.has_value()) {
                                return std::nullopt;
                            }
                            element_type = single_op_term.value().element_type;
                            if (!expect(TokenType::C_PAREN, "Expected closed parenthesis")) {
                                return std::nullopt;
                            }
                            // this is a bit of a workaround but it is the easiest way to do this.
                            NodeTermIntLit* int_lit = std::get<NodeTermIntLit*>(single_op_term.value().term->var);
                            if (_not) {
                                if (m_imm_size == 4) {
                                    int_lit->bin_num = ~int_lit->bin_num & 0xFFFFFFFF;
                                } else {
                                    int_lit->bin_num = ~int_lit->bin_num;
                                }
                            } else {
                                int_lit->bin_num = -int_lit->bin_num;
                            }
                            term->var = int_lit;
                            break;
                        }
                    } else if (next_token.value().type == TokenType::INT_LIT || next_token.value().type == TokenType::N_INT_LIT ||
                                next_token.value().type == TokenType::LONG_LIT || next_token.value().type == TokenType::N_LONG_LIT ||
                                next_token.value().type == TokenType::FLOAT_LIT || next_token.value().type == TokenType::N_FLOAT_LIT ||
                                next_token.value().type == TokenType::U_INT_LIT || next_token.value().type == TokenType::U_LONG_LIT) {
                        consume();
                        std::optional<TermOut> single_op_term = parseTerm(allowed_type, expr_element, OperatorType::NOT);
                        if (!single_op_term.has_value()) {
                            return std::nullopt;
                        }
                        element_type = single_op_term.value().element_type;
                        NodeTermIntLit* int_lit = std::get<NodeTermIntLit*>(single_op_term.value().term->var);
                        term->var = int_lit;
                        break;
                    }
                }

                consume();
                
                if (_not) {
                    NodeTermNotExpr* not_expr = allocator.allocate<NodeTermNotExpr>();

                    std::optional<TermOut> not_term = parseTerm(allowed_type, expr_element, OperatorType::NOT);
                    if (!not_term.has_value()) {
                        return std::nullopt;
                    }

                    not_expr->term = not_term.value().term;
                    term->var = not_expr;
                } else {
                    NodeTermNegExpr* neg_expr = allocator.allocate<NodeTermNegExpr>();

                    std::optional<TermOut> neg_term = parseTerm(allowed_type, expr_element, OperatorType::NOT);
                    if (!neg_term.has_value()) {
                        return std::nullopt;
                    }

                    neg_expr->term = neg_term.value().term;
                    term->var = neg_expr;
                }
                element_type = ExprElementType::PAREN;
                break;
            }
            case TokenType::EXCLAM:
            {
                NodeTermLogNotExpr* log_not_expr = allocator.allocate<NodeTermLogNotExpr>();

                consume();
                std::optional<Token> next_token = next();
                bool expect_paren = false;
                if (next_token.has_value() && next_token.value().type == TokenType::O_PAREN) {
                    expect_paren = true;
                    consume();
                }
                std::optional<ExprOut> not_expr = parseExpr(allowed_type, expr_min_prec);
                if (!not_expr.has_value()) {
                    return std::nullopt;
                }

                expr_element = not_expr.value().expr;

                if (expect_paren) {
                    if (!expect(TokenType::C_PAREN, "Expected closed parenthesis")) {
                        return std::nullopt;
                    }
                } 
                
                if (!m_elem_used_test) {
                    NodeExpr* expr = allocator.allocate<NodeExpr>();
                    NodeExprLog* log_expr = allocator.allocate<NodeExprLog>();
                    // m_right_element will be set to the element type of the final expression
                    NodeExprLogTest* log_test = allocator.allocate<NodeExprLogTest>(m_right_element.t, m_last_main_reg_ops, m_expr_pos);

                    log_test->expr = not_expr.value().expr;
                    log_expr->var = log_test;
                    expr->var = log_expr;
                    not_expr.value().expr = expr;
                    m_elem_used_test = true;
                }

                element_type = ExprElementType::PAREN;
                is_term_signed = is_signed;
                log_not_expr->expr = not_expr.value().expr;
                term->var = log_not_expr;
                break;
            }
            default:
                error("Bad expression");
                return std::nullopt;
        }

        return TermOut{.term = term, .element_type = element_type, .expr_element = 
                        SubExprElement{.v = Variable{.type = allowed_type, 
                        .is_signed = is_term_signed}, .e = expr_element}};
    }

    template <typename T>
    inline std::optional<NodeTermIntLit*> invoke_parse_int_lit(VarType& allowed_type, const OperatorType& op_type, Token token) {
        if (allowed_type == VarType::FLOAT) {
            return parse_int_lit<float>(op_type, token);
        } else if (allowed_type == VarType::DOUBLE) {
            return parse_int_lit<double>(op_type, token);
        } else {
            return parse_int_lit<T>(op_type, token);
        }
    }

    template <typename T>
    std::optional<NodeTermIntLit*> parse_int_lit(const OperatorType& op_type, Token token) {
        NodeTermIntLit* int_lit = allocator.allocate<NodeTermIntLit>();

        std::optional<T> num;

        switch (op_type) {
            case OperatorType::ADD:
            case OperatorType::SUB:
            case OperatorType::NOT:
            case OperatorType::NONE:
            {
                long val = 0;
                size_t index;
                std::optional<Token> first_operator = next(1);
                if (!first_operator.has_value()) {
                    num = get_num_from_int_token<T>(token.type, consume().value.value());
                    break;
                }

                num = parseConstExpr<T>(token.type, (op_type == OperatorType::SUB), (first_operator.value().type == TokenType::ASTER), true, val, index);
                break;
            }
            case OperatorType::MUL:
            {
                num = get_num_from_int_token<T>(token.type, consume().value.value());
                std::optional<Token> next_num = next(1);
                std::optional<Token> next_operator = next();
                while (next_num.has_value() && next_operator.value().type == TokenType::ASTER && 
                    (next_num.value().type == TokenType::INT_LIT || next_num.value().type == TokenType::N_INT_LIT)) {
                    consume();
                    std::optional<signed long> num2 = get_num_from_int_token<T>(next_num.value().type, consume().value.value());
                    if (!num2.has_value()) {
                        return std::nullopt;
                    }
                    num.value() *= num2.value();
                    next_num = next(1);
                    next_operator = next();
                }
                break;
            }
            default: 
                num = get_num_from_int_token<T>(token.type, consume().value.value());
        }

        if (!num.has_value()) {
            return std::nullopt;
        }
        if constexpr (sizeof(T) == 8) {
            check_imm_too_big<T>(num.value());
        }
        if (m_cur_var_type == VarType::FLOAT) {
            int_lit->bin_num = get_int_bits_f(num.value());
        } else if (m_cur_var_type == VarType::DOUBLE) {
            int_lit->bin_num = get_int_bits_d(num.value());
        } else {
            int_lit->bin_num = static_cast<int64_t>(num.value());
        }
        
        m_last_imm_truth = (!!int_lit->bin_num);
        return int_lit;
    }

    // returns -1 if an error occured and 0 if parseConstExpr while loop should break
    template <typename T>
    inline char parseConstExprParen(u_int8_t& min_prec, u_int8_t& prec, TokenType& op, T& num, std::optional<T>& rnum, long& final_result, size_t& final_index) {
        if (prec == 255 || prec < min_prec) {
            return 0;
        }

        if (op == TokenType::PLUS || op == TokenType::HYPHEN) {
            final_result = num;
            final_index = m_index - 1;
        }

        m_index += 2;

        std::optional<Token> token = next();
        
        size_t num_parens = 1;
        while (token.has_value() && token.value().type == TokenType::O_PAREN) {
            ++num_parens;
            token = m_tokens[++m_index];
        }

        long val = 0;
        size_t index;
        rnum = parseConstExpr<T>(next().value().type, false, false, true, val, index);

        for (; num_parens > 0; num_parens--) {
            if (!expect(TokenType::C_PAREN, "Expected closed parenthesis")) {
                return -1;
            }
        }

        return 1;
    }
    
    template <typename T>
    std::optional<T> parseConstExpr(TokenType cur_token_type, bool is_subtraction, bool use_multiplication, bool first_step, 
        long& final_result, size_t& final_index, u_int8_t min_prec = 0) {
        
        char initial_operator = 0;
        if (m_tokens[m_index - 1].type == TokenType::TILDE) {
            if (m_cur_var_type == VarType::FLOAT || m_cur_var_type == VarType::DOUBLE) {
                error("Cannot use bitwise operator in float expression");
                return std::nullopt;
            }
            initial_operator = 1;
        } else if (m_tokens[m_index - 1].type == TokenType::HYPHEN && op_prec(m_tokens[m_index - 2].type) != 255) {
            initial_operator = 2;
        }

        std::optional<T> first_num = get_num_from_int_token<T>(cur_token_type, consume().value.value());
        T num;

        if (first_num.has_value()) {
            num = first_num.value();
        } else {
            return std::nullopt;
        }

        if constexpr (!(std::is_same_v<T, float> || std::is_same_v<T, double>)) {
            if (initial_operator == 1) {
                num = ~num;
            }
        }

        if (initial_operator == 2) {
            num = -num;
        }

        if (first_step) {
            final_result = num;
            final_index = m_index;
            m_imm_size = sizeof(T);
        }

        //Makes expressions between immediate values constexpr
        while (true) {
            std::optional<Token> nextTerm = next(1);
            std::optional<T> rnum;
            std::optional<Token> temp_op = next();

            //removes the need for an if check at the end that checks if the temp_op exists
            if (!nextTerm.has_value()) {
                return final_result;
            }

            TokenType op = temp_op.value().type;

            if (op == TokenType::PLUS || op == TokenType::HYPHEN ||
                (use_multiplication && (op == TokenType::ASTER || op == TokenType::SLASH))) {
                final_result = num;
                final_index = m_index;
            }
            
            //*might change later but would have to use gotos instead of break statements
            if (nextTerm.value().type == TokenType::INT_LIT || nextTerm.value().type == TokenType::N_INT_LIT ||
                nextTerm.value().type == TokenType::LONG_LIT || nextTerm.value().type == TokenType::N_LONG_LIT ||
                nextTerm.value().type == TokenType::FLOAT_LIT || nextTerm.value().type == TokenType::N_FLOAT_LIT ||
                nextTerm.value().type == TokenType::U_INT_LIT || nextTerm.value().type == TokenType::U_LONG_LIT) {

                u_int8_t prec = op_prec(op);

                if (prec == 255 || prec < min_prec) {
                    break;
                }

                consume();

                rnum = parseConstExpr<T>(nextTerm.value().type, is_subtraction, false, false, final_result, final_index, prec + 1);
            } else if (nextTerm.value().type == TokenType::O_PAREN) {
                if (is_paren_const()) {
                    u_int8_t prec = op_prec(next().value().type);
                    char parse_paren = parseConstExprParen<T>(min_prec, prec, op, num, rnum, final_result, final_index);
                    if (parse_paren == -1) {
                        return std::nullopt;
                    } else if (parse_paren == 0) {
                        break;
                    }
                } else {
                    break;
                }
            } else if (nextTerm.value().type == TokenType::TILDE || nextTerm.value().type == TokenType::HYPHEN) {
                if (nextTerm.value().type == TokenType::TILDE) {
                    if (m_cur_var_type == VarType::FLOAT || m_cur_var_type == VarType::DOUBLE) {
                        error("Cannot use bitwise not operator in float expression");
                        return std::nullopt;
                    }
                }

                std::optional<Token> next_token = next();
                u_int8_t prec = op_prec(next().value().type);
                if (next_token.has_value()) {
                    if (next_token.value().type == TokenType::O_PAREN) {
                        if (is_paren_const()) {
                            consume();
                            char parse_paren = parseConstExprParen<T>(min_prec, prec, op, num, rnum, final_result, final_index);
                            if (parse_paren == -1) {
                                return std::nullopt;
                            } else if (parse_paren == 0) {
                                break;
                            }
                            if (rnum.has_value()) {
                                if (nextTerm.value().type == TokenType::TILDE) {
                                    if constexpr (!(std::is_same_v<T, float> || std::is_same_v<T, double>)) {
                                        rnum.value() = ~rnum.value();
                                    }
                                } else {
                                    rnum.value() = -rnum.value();
                                }
                            }
                        } else {
                            break;
                        }
                    } else {
                        u_int8_t prec = op_prec(op);

                        if (prec == 255 || prec < min_prec) {
                            break;
                        }

                        m_index += 2;
                        rnum = parseConstExpr<T>(next_token.value().type, is_subtraction, false, false, final_result, final_index, prec + 1);
                    } 
                } else {
                    return std::nullopt;
                }
            } else {
                break;
            }

            if (!rnum.has_value()) {
                return std::nullopt;
            }

            switch (op) {
                case TokenType::PLUS:
                    use_multiplication = false;
                    if (!is_subtraction) {
                        num += rnum.value();
                        break;
                    } else {
                        num -= rnum.value();
                        break;
                    }
                case TokenType::HYPHEN:
                    use_multiplication = false;
                    if (!is_subtraction) {
                        num -= rnum.value();
                        break;
                    } else {
                        num += rnum.value();
                        break;
                    }
                case TokenType::ASTER:
                    num *= rnum.value();
                    break;
                case TokenType::SLASH:
                    num /= rnum.value();
                    break;
                default:
                    if constexpr (!(std::is_same_v<T, float> || std::is_same_v<T, double>)) {
                        switch (op) {
                            case TokenType::PERCENT:
                                num %= rnum.value();
                                break;
                            case TokenType::AMPERSAND:
                                num &= rnum.value();
                                break;
                            case TokenType::PIPE:
                                num |= rnum.value();
                                break;
                            case TokenType::CARAT:
                                num ^= rnum.value();
                                break;
                            case TokenType::DOUB_GREATER:
                                num >>= rnum.value();
                                break;
                            case TokenType::DOUB_LESS:
                                num <<= rnum.value();
                                break;
                            case TokenType::CARAT_GREATER: // C++ doesnt have roll operators, this works for all types
                            {
                                char shift = sizeof(num) * 8 - 1;
                                num = ((num >> rnum.value()) & ~(((1 << shift) >> rnum.value()) << 1)) | (num << (sizeof(num) * 8 - rnum.value()));
                                break;
                            }
                            case TokenType::LESS_CARAT:
                            {
                                char shift = sizeof(num) * 8 - 1;
                                num = (num << rnum.value()) | ((num >> (sizeof(num) * 8 - rnum.value())) & ~(((1 << shift) >> rnum.value()) << 1));
                                break;
                            }
                            default:
                                error("Something went wrong with parsing constexpr");
                                return std::nullopt;
                        }
                    }
            }
        }

        TokenType last_token = next().value().type;

        if (!first_step || last_token == TokenType::PLUS || last_token == TokenType::HYPHEN || 
            last_token == TokenType::SEMI || last_token == TokenType::C_PAREN) {
            return num;
        }

        if (first_step) {
            m_index = final_index;
        }
        return final_result;
    }
};

