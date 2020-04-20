#pragma once
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <stack>
#include <memory>
class Interpreter;
class Expression;
class Variable;
class Operation;
class ExpressionTree;
enum BoolExtended
{
    FALSE,
    TRUE,
    UNDEFINED
};
enum OperationType
{
    OR,
    AND,
    CONSEQUENT,
    LEFT_BRACE,
    RIGHT_BRACE
};
/**********************************************************************/
class ExpressionVisitor
{
public:
    virtual BoolExtended visit(Operation&) = 0;
    virtual BoolExtended visit(Variable&) = 0;
    virtual BoolExtended visit(ExpressionTree&) = 0;
};
class Expression
{
    public:
        virtual ~Expression(){}
        virtual void Print() = 0;
        virtual BoolExtended accept(ExpressionVisitor& v) = 0;
        friend class ExpressionTree;
        friend class ExpressionVisitor;
};
class Variable:public::Expression
{
    public:
         virtual ~Variable(){}
         Variable(const std::string&);
         virtual void Print()override;
         virtual BoolExtended accept(ExpressionVisitor& v)override;
         friend class Interpreter;
    private:
        std::string m_name;
};
class Operation:public::Expression
{
    public:
        Operation(const std::string&);
        virtual ~Operation();
        virtual void Print()override;
        virtual BoolExtended accept(ExpressionVisitor& v)override;
        friend class ExpressionTree;
        friend class Interpreter;
    private:
        OperationType m_type;
        std::shared_ptr<Expression> left = nullptr;
        std::shared_ptr<Expression> right = nullptr;
};
class ExpressionTree
{
    public:
        ExpressionTree(){}
        ExpressionTree(const std::string &);
        ~ExpressionTree();
        BoolExtended accept(ExpressionVisitor& v);
        void Init(const std::string &);
        void Parse(const std::string &);
        void AddExpr(const std::string&);
        void InitExpr(std::shared_ptr<Expression>&);
        void Print();
        friend class Interpreter;
    private:
        std::shared_ptr<Expression> m_root;
        std::stack<std::string> m_stack;
        bool m_visited = false;
        BoolExtended m_value = UNDEFINED;
};
class Interpreter:public ExpressionVisitor
{
    public:
        Interpreter();
        virtual ~Interpreter();
        void UpdateFacts();
        void PrintNewFacts();
        void AddRule(const std::string&);
        void AddRule(const std::string&, const std::string&);
        void ScanFacts(std::istream&);
        void ScanRules(std::istream&);
        bool isFact(const std::string&);
        BoolExtended visit(Operation&);
        BoolExtended visit(Variable&);
        BoolExtended visit(ExpressionTree&);
    private:
        std::unordered_map<std::string, ExpressionTree> m_rules;
        std::unordered_set<std::string> m_facts;
        std::unordered_set<std::string> m_result;
};
