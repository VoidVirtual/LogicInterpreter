#pragma once
#include <string>
#include <stack>
inline int getType(std::string& str)
{
    if(str=="(")
        return LEFT_BRACE;
    if(str==")")
        return RIGHT_BRACE;
    if(str=="|")
        return OR;
    if(str==",")
        return AND;
    return -1;
}
inline std::string getString(OperationType _t)
{
    switch(_t)
    {
        case LEFT_BRACE:
            return "(";
        case RIGHT_BRACE:
            return ")";
        case OR:
            return "|";
        case AND:
            return ",";
        default:
            return "";
    }
}
inline std::string infixToPolish(const std::string& infix)
{
    std::string polish;
    std::string temp;
    std::stack<OperationType> _stack;
    for(unsigned int i = 0; i <= infix.size(); i++)
    {
        if(infix[i]=='(' ||infix[i]==')'||infix[i]=='|'||infix[i]==','||infix[i]=='-'||i==infix.size())
        {
            polish+=temp;
            polish+=" ";
            temp = "";
            temp+=infix[i];
        }
        else
            temp+=infix[i];
        int type = getType(temp);
        if(type>=0)
        {
            switch(type)
            {
                case OR:
                    while(!_stack.empty() &&_stack.top()==AND)
                    {
                        polish+=getString(_stack.top());
                        polish+=" ";
                        _stack.pop();
                    }
                    _stack.push(OR);
                    break;
                case AND:
                    _stack.push(AND);
                    break;
                case LEFT_BRACE:
                    _stack.push(LEFT_BRACE);
                    break;
                case RIGHT_BRACE:
                    while(!_stack.empty())
                    {
                        polish+=getString(_stack.top());
                        _stack.pop();
                        if(_stack.top()== LEFT_BRACE)
                        {
                            _stack.pop();
                            break;
                        }
                    }
                    break;
            }
            temp = "";
        }
    }
    while(!_stack.empty())
    {
        polish+=getString(_stack.top());
        polish+=" ";
        _stack.pop();
    }
    return polish;
}
inline std::string getStatement(std::istream& in)
{
    std::string res;
    while(true)
    {
        char c = in.get();
        if(c=='"')
            break;
        res+=c;
    }
    return res;
}
inline std::string getWord(std::istream& in)
{
    std::string res;
    while(true)
    {
        char c = in.get();
        if(c==' '|| c=='\n'||c=='\t')
            break;
        res+=c;
    }
    return res;
}
