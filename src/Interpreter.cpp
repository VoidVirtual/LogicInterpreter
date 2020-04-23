#include "Interpreter.h"
#include "Parser.h"
Interpreter::Interpreter()
{

}
Interpreter::~Interpreter()
{

}
void Interpreter::ScanFacts(std::istream& in)
{
    while(true)
    {
        char c = in.get();
        if(c=='\n')
            break;
        if(c=='"')
        {
            auto rule = getStatement(in);
            m_facts.insert(rule);
        }
    }
}
void Interpreter::ScanRules(std::istream& in)
{
    while(true)
    {
        char c = in.get();
        if(c=='\n' ||c== 'EOF')
            break;
        if(c=='"')
        {
            auto rule = getStatement(in);
            AddRule(rule);
        }
    }
}
void Interpreter::AddRule(const std::string& name, const std::string& expr)
{
    if(expr.rfind('>')==std::string::npos)
    {
        auto polish = infixToPolish(expr);
        auto it = m_rules.find(name);
        if (it != m_rules.end())
            it->second.AddExpr(polish);
        else
            m_rules.insert(std::make_pair(name, ExpressionTree(polish)));
    }
    else
    {
        int pos = expr.rfind('>');
        std::string newName = expr.substr(pos+1);
        std::string newExpr = expr.substr(0,pos-1);
        AddRule(name, newName);
        AddRule(newName,newExpr);
    }
}
void Interpreter::AddRule(const std::string& str)
{
    int pos = str.rfind('>');
    std::string name = str.substr(pos+1);
    std::string expr = str.substr(0,pos-1);
    AddRule(name, expr);
}
bool Interpreter::isFact(const std::string& name)
{
    auto it = m_facts.find(name);
    if(it!=m_facts.end())
    {
        return true;
    }
    return false;
}
void Interpreter::UpdateFacts()
{
    for(auto& it: m_rules)
    {
       for(auto& it2: m_rules)
            it2.second.m_visited = false;
       if(it.second.accept(*this)==TRUE)
            m_result.insert(it.first);
    }
}
void Interpreter::PrintNewFacts()
{
   if(m_result.empty())
   {
       std::cout<<"No new facts.";
       return;
   }
   std::cout<<"New facts: ";
   for(auto& name: m_result)
        std::cout<<name<<" ";
    std::cout<<"\n";
}
Variable::Variable(const std::string& name)
{
    m_name = name;
}
Operation::Operation(const std::string& type)
{
    if(type == "|")
        m_type = OR;
    if(type == ",")
        m_type = AND;
}
void ExpressionTree::Init(const std::string& polish)
{
    Parse(polish);
    InitExpr(m_root);
}
ExpressionTree::ExpressionTree(const std::string& polish)
{
    Init(polish);
}
void ExpressionTree::Parse(const std::string &polish)
{
    std::string temp;
    for(unsigned int i = 0; i <= polish.size(); i++)
    {
        if(polish[i]==' ' || i==polish.size())
        {
            if(temp!= "")
                m_stack.push(temp);
            temp = "";
        }
        else
            temp+=polish[i];
    }
}
void ExpressionTree::AddExpr(const std::string& polish)
{
    Parse(polish);
    auto newRoot = std::shared_ptr<Operation>(new Operation("|"));
    std::shared_ptr<Expression> left;
    InitExpr(left);
    newRoot->left = left;
    newRoot->right = m_root;
    m_root = newRoot;
}
void ExpressionTree::InitExpr(std::shared_ptr<Expression>& expr)
{
    if(!m_stack.empty())
    {
        auto name = m_stack.top();
        m_stack.pop();
        if(name=="|" || name=="," || name=="->")
        {
            expr = std::shared_ptr<Operation>(new Operation(name));
            auto o  = std::dynamic_pointer_cast<Operation>(expr);
            InitExpr(o->left);
            InitExpr(o->right);
        }
        else
            expr = std::shared_ptr<Variable>(new Variable(name));
    }
}
void Operation::Print()
{
    if(left)
        left->Print();
    if(m_type == OR)
        std::cout<<" | ";
    if(m_type == AND)
        std::cout<<" , ";
    if(right)
        right->Print();
}
void Variable::Print()
{
   std::cout<<m_name;
}
Operation::~Operation()
{
}
ExpressionTree::~ExpressionTree()
{
}
void ExpressionTree::Print()
{
   m_root->Print();
}
BoolExtended ExpressionTree::accept(ExpressionVisitor& v)
{
    return v.visit(*this);
}
BoolExtended Operation::accept(ExpressionVisitor& v)
{
    return v.visit(*this);
}
BoolExtended Variable::accept(ExpressionVisitor& v)
{
    return v.visit(*this);
}
BoolExtended Interpreter::visit(ExpressionTree& t)
{
    if(t.m_value!=UNDEFINED)
        return t.m_value;
    if(t.m_visited==false)
    {
        t.m_visited = true;
        t.m_value = t.m_root->accept(*this);
    }
    return t.m_value;
}
BoolExtended Interpreter::visit(Operation& o)
{
    switch(o.m_type)
    {
        case OR:
            if(o.left->accept(*this)==TRUE)
                return TRUE;
            if(o.right->accept(*this)==TRUE)
                return TRUE;
            return FALSE;
        case AND:
            if(o.left->accept(*this)!=TRUE)
                return FALSE;
            if(o.right->accept(*this)!=TRUE)
                return FALSE;
            return TRUE;
    }
}
BoolExtended Interpreter::visit(Variable& v)
{
    if(isFact(v.m_name))
        return TRUE;
    auto it = m_rules.find(v.m_name);
    if(it!=m_rules.end())
        return it->second.accept(*this);
    else
        return FALSE;
}
