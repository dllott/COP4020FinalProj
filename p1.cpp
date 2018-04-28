#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <stack>
#include <vector>
#include <cstdlib>

//COP4020 Project: SUBC Parser
//David Lott 2810-4729

//Node for First-Child, Next-Sibling Tree
class Node {
    public :
        std::string token;
        Node * left;
        Node * right;
};

class Token {
public:
    std::string token;
    std::string type;
};

//Tree Builder function, taken almost directly from notes
void Build_tree(std::string x, int n, std::stack<Node*> &S){
    Node * p;
    Node * c;
    Node * l;
    for(int i = 0; i < n; ++i){
        c = S.top();
        S.pop();
        c->right = p;
        p = c;
    }
    l->left = p;
    l->token = x;
    S.push(l);

}

void Read(std::vector<Token> &V, std::string s){
    if(V.begin()->token != s){
        std::cout << "ERROR";
        exit(EXIT_FAILURE);
    }
    V.erase(V.begin());
}

void Name(std::vector<Token> &V, std::stack<Node*> &S){
}

void ConstValue(std::vector<Token> &V, std::stack<Node*> &S){
    if(V.begin()->token == "integer"){
        Read(V, "integer");
    } else if (V.begin()->token == "char") {
        Read(V, "char");
    } else {
        Name(V, S);
    }
}

void Const(std::vector<Token> &V, std::stack<Node*> &S){
    Name(V, S);
    Read(V, "=");
    ConstValue(V, S);
    Build_tree("=", 2, S);
}

void Consts(std::vector<Token> &V, std::stack<Node*> &S){
    if(V.begin()->token == "const"){
        Read(V, "const");
        Const(V, S);
        int x=1;
        while(V.begin()->token == ","){
            Read(V,",");
            ++x;
            Const(V, S);
        }
        Read(V, ";");
        Build_tree("consts", x, S);
    } else {
        Build_tree("consts", 0, S);
    }
}

void LitList(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 1;
    Read(V, "(");
    Name(V, S);
    while(V.begin()->token == ","){
        Read(V, ",");
        ++x;
        Name(V, S);
    }
    Read(V, ")");
    Build_tree("lit", x, S);
}

void Type(std::vector<Token> &V, std::stack<Node*> &S){
    Name(V,S);
    Read("=");
    LitList(V,S);
    Build_tree("type",3);
}

void Types(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 1;
    if(V.begin->token == "type"){
        Read(V, "type");
        Type(V, S);
        while(V.begin()->token == ";"){
            ++x;
            Read(V, ";");
            Type(V, S);
        }
        Build_tree("types", x);
    } else {
        Build_tree("types", 0);
    }
}

void Dcln(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 2;
    Name(V, S);
    while(V.begin()->token == ","){
        ++x;
        Read(V, ",");
        Name(V, S);
    }
    Read(V, ":");
    Name(V, S);
    Build_tree("dclns", x, S);
}

void Params(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 1;
    Dcln(V, S);
    while(V.begin()->token == ";"){
        ++x;
        Read(V, ";");
        Dcln(V, S);
    }
    Build_tree("params", x, S);
}

void Dclns(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 0;
    if(V.begin()->token == "var"){
        Read(V, "var");
        x = 1;
        Dcln(V, S);
        while(V.begin()->token == ";"){
            ++x;
            Read(V, ";");
            Dcln(V, S);
        }
    }
    Build_tree("dclns", x, S);

}

void OutExp(std::vector<Token> &V, std::stack<Node*> &S){

}

void Statement(std::vector<Token> &V, std::stack<Node*> &S){
    if(V.begin()->token == "block"){
        //Body
        Read(V,"begin");
        int x = 1;
        Statement(V, S);
        while(V.begin()->token == ";"){
            ++x;
            Read(V, ";");
            Statement(V, S);
        }
        Read(V, "end");
        Build_tree("block", x, S);
    } else
    if(V.begin()->token == "output"){
        Read(V, "output");
        int x = 1;
        Read(V, "(");
        OutExp(V, S);
        while(V.begin()->token == ","){
            ++x;
            Read(V, ",");
            OutExp(V, S);
        }
        Read(V, ")");
        Build_tree("output", x, S);
    } else
    if(V.begin()->token == "if"){
        Read(V, "if");
        Expression(V, S);
        Read(V, "then");
        Statement(V, S);
        if(V.begin()->token == "else"){
            Read(V, "else");
            Statement(V, S);
            Build_tree("if",3, S);
        } else {
            Build_tree("if", 2, S);
        }
    } else
    if(V.begin()->token == "while"){
        Read(V, "while");
        Expression(V, S);
        Read(V, "do");
        Statement(V, S);
        Build_tree("while", 2, S);
    } else
    if(V.begin()->token == "repeat"){
        Read(V, "repeat");
        Statement(V, S);
        int x = 2;
        while(V.begin()->token == ";"){
            ++x;
            Read(V, ";");
            Statement(V, S);
        }
        Read(V, "until");
        Expression(V, S);
        Build_tree("repeat", x, S);
    } else
    if(V.begin()->token == "for"){
        Read(V, "for");
        Read(V, "(");
        ForStat(V, S);
        Read(V, ";");
        ForExp(V, S);
        Read(V, ";");
        ForStat(V, S);
        Read(V, ")");
        Statement(V, S);
        Build_tree("for", 4, S);
    } else
    if(V.begin()->token == "loop"){
        Read(V, "loop");
        int x = 1;
        Statement(V, S);
        while(V.begin()->token == ";"){
            ++x;
            Read(V, ";");
            Statement(V, S);
        }
        Read(V, "pool");
        Build_tree("loop", x, S);
    } else
    if(V.begin()->token == "case"){
        Read(V, "case");
        Expression(V, S);
        Read(V, "of");
        Caseclauses(V, S);
        OtherwiseClause(V, S);
        Read(V, "end");
        Build_tree("case", 3, S);
    } else
    if(V.begin()->token == "read"){
        Read(V, "read");
        Read(V, "(");
        int x = 1;
        Name(V, S);
        while(V.begin()->token == ","){
            ++x;
            Read(V, ",");
            Statement(V, S);
        }
        Read(V, ")");
        Build_tree("read", x, S);
    } else
    if(V.begin()->token == "exit"){
        Read(V, "exit");
        Build_tree("exit", 0);
    } else
    if(V.begin()->token == "return"){
        Read(V, "return");
        Expression(V, S);
        Build_tree("return", 1, S);
    } else {
        Build_tree("<null>", 0, S);
    }
}

void Body(std::vector<Token> &V, std::stack<Node*> &S){
    Read(V,"begin");
    int x = 1;
    Statement(V, S);
    while(V.begin()->token == ";"){
        ++x;
        Read(V, ";");
        Statement(V, S);
    }
    Read(V, "end");
    Build_tree("block", x, S);
}

void Fcn(std::vector<Token> &V, std::stack<Node*> &S){
    Read(V, "function");
    Name(V, S);
    Read(V, "(");
    Params(V, S);
    Read(V, ")");
    Read(V, ":");
    Name(V, S);
    Read(V, ";");
    Consts(V, S);
    Types(V, S);
    Dclns(V, S);
    Body(V, S);
    Name(V, S);
    Read(V, ";");
    Build_tree("fcn", 8);
}

void SubProgs(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 0;
    while(V.begin()->token== "function"){
        Fcn();
    }
    Build_tree("subprogs" , x, S);
}



void Tiny(std::vector<Token> &V, std::stack<Node*> &S){
    Read(V, "program");
    Name(V, S);
    Read(V, ":");
    Consts(V, S);
    Types(V, S);
    Dclns(V, S);
    SubProgs(V, S);
    Body(V, S);
    Name(V, S);
    Read(V, ".");
    Build_tree("program", 7, S);
}

//Puts input stuff into vector, and removes comment stuff
std::vector<Token> Stacko (std::string file){
    std::vector<Token> out;
    std::ifstream inFile;
    std::string working;
    infile.open(file);
    if(!infile.is_open()){
        exit(EXIT_FAILURE);
    }
    inFile >> working;
    while(inFile.good()){
        if(working == "{"){
            while(working != "}"){
                inFile >> working;
            }
        }
        Token p;
        p.token = working;
        out.push_back(p)
        inFile >> working;
    }
}

int main(int argc, char * argv[]){

}
