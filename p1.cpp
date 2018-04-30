#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stack>
#include <vector>
#include <cstdlib>

//COP4020 Project: SUBC Parser
//David Lott 2810-4729

//Node for First-Child, Next-Sibling Tree
struct Node {
    std::string token;
    Node * left = NULL;
    Node * right = NULL;
};

//Token class, contains actual text from input and token type
class Token {
public:
    std::string token;
    std::string type;
};

//Tree Builder function, taken almost directly from notes
void Build_tree(std::string x, int n, std::stack<Node*> &S){
    std::cout << "BT: " << x << "," << n << std::endl;
    //std::cout << "before stack height: " << S.size() << std::endl;
    Node * p;
    p = new Node;
    Node * c;
    c = new Node;
    Node * l;
    l = new Node;
    if(n > 0){
        for(int i = 0; i < n; ++i){

            c = S.top();
            S.pop();
            c->right = p;
            p = c;
            //std::cout << "built: " << i << std::endl;
        }

    }
    //std::cout << "loop done" << std::endl;
    l->left = p;
    l->token = x;
    S.push(l);
    //std::cout << "after stack height: " << S.size() << std::endl;
    //std::cout << "done" << std::endl;

}

void Read(std::vector<Token> &V, std::string s, std::stack<Node*> &S){
    std::cout << "attempting read: " << s << std::endl;
    if(s == "<integer>"){
        if(V.begin()->type == "integer"){
            Build_tree(V.begin()->token, 0, S);
            Build_tree("<integer>", 1, S);
        } else {
            std::cout <<"read error: integer" << std::endl;
            exit(EXIT_FAILURE);
        }
    } else
    if(s == "<char>"){
        if(V.begin()->type == "char"){
            Build_tree(V.begin()->token, 0, S);
            Build_tree("<char>", 1, S);
        } else {
            std::cout <<"read error: char" << std::endl;
            exit(EXIT_FAILURE);
        }
    } else
    if(s == "<identifier>"){
        if(V.begin()->type == "identifier"){
            Build_tree(V.begin()->token, 0, S);
            Build_tree("<identifier>", 1, S);
        } else {
            std::cout <<"read error: identifier, " << V.begin()->token << std::endl;
            exit(EXIT_FAILURE);
        }
    } else
    if(s == "<string>"){
        if(V.begin()->type == "string"){
            Build_tree(V.begin()->token, 0, S);
            Build_tree("<string>", 1, S);
        } else {
            std::cout <<"read error: string" << std::endl;
            exit(EXIT_FAILURE);
        }
    } else
    if(V.begin()->token != s){
        std::cout <<"read error, " << V.begin()->token << std::endl;
        std::cout << "ERROR";
        exit(EXIT_FAILURE);
    }
    V.erase(V.begin());
}

void Name(std::vector<Token> &V, std::stack<Node*> &S){
    //std::cout << "NAME before stack height: " << S.size() << std::endl;
    Read(V, "<identifier>", S);
    //std::cout << "NAME after stack height: " << S.size() << std::endl;
}

void ConstValue(std::vector<Token> &V, std::stack<Node*> &S){
    if(V.begin()->type == "integer"){
        Read(V, "<integer>", S);
    } else if (V.begin()->type == "char") {
        Read(V, "<char>", S);
    } else {
        Name(V, S);
    }
}

void Const(std::vector<Token> &V, std::stack<Node*> &S){
    Name(V, S);
    Read(V, "=", S);
    ConstValue(V, S);
    Build_tree("=", 2, S);
}

void Consts(std::vector<Token> &V, std::stack<Node*> &S){
    if(V.begin()->token == "const"){
        Read(V, "const", S);
        Const(V, S);
        int x=1;
        while(V.begin()->token == ","){
            Read(V,",", S);
            ++x;
            Const(V, S);
        }
        Read(V, ";", S);
        Build_tree("consts", x, S);
    } else {
        Build_tree("consts", 0, S);
    }
}

void LitList(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 1;
    Read(V, "(", S);
    Name(V, S);
    while(V.begin()->token == ","){
        Read(V, ",", S);
        ++x;
        Name(V, S);
    }
    Read(V, ")", S);
    Build_tree("lit", x, S);
}

void Type(std::vector<Token> &V, std::stack<Node*> &S){
    Name(V,S);
    Read(V, "=", S);
    LitList(V,S);
    Build_tree("type",3, S);
}

void Types(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 1;
    if(V.begin()->token == "type"){
        Read(V, "type", S);
        Type(V, S);
        while(V.begin()->token == ";"){
            ++x;
            Read(V, ";", S);
            Type(V, S);
        }
        Build_tree("types", x, S);
    } else {
        Build_tree("types", 0, S);
    }
}

void Dcln(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 2;
    Name(V, S);
    while(V.begin()->token == ","){
        ++x;
        Read(V, ",", S);
        Name(V, S);
    }
    Read(V, ":", S);
    //std::cout << "DCLN stack height 0: " << S.size() << std::endl;
    Name(V, S);
    //std::cout << "DCLN stack height 1: " << S.size() << std::endl;
    Build_tree("var", x, S);
}

void Params(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 1;
    Dcln(V, S);
    while(V.begin()->token == ";"){
        //std::cout << "params: " << V.begin()->token << std::endl;
        ++x;
        Read(V, ";", S);
        Dcln(V, S);
    }
    Build_tree("params", x, S);
}

void Dclns(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 0;
    if(V.begin()->token == "var"){
        Read(V, "var", S);
        x = 1;
        Dcln(V, S);
        Read(V, ";", S);
        while(V.begin()->token == ";"){
            ++x;
            Dcln(V, S);
            Read(V, ";", S);
        }
    }
    Build_tree("dclns", x, S);

}

void StringNode(std::vector<Token> &V, std::stack<Node*> &S){
    Read(V, "<string>", S);
}

void Term(std::vector<Token> &V, std::stack<Node*> &S);

void Expression(std::vector<Token> &V, std::stack<Node*> &S){
    Term(V, S);
    std::cout << "done term" << std::endl;
    if(V.begin()->token == "<="){
        Read(V, "<=", S);
        Term(V, S);
        Build_tree("<=", 2, S);
    } else
    if(V.begin()->token == "<"){
        Read(V, "<", S);
        Term(V, S);
        Build_tree("<", 2, S);
    } else
    if(V.begin()->token == ">="){
        Read(V, ">=", S);
        Term(V, S);
        Build_tree(">=", 2, S);
    } else
    if(V.begin()->token == ">"){
        std::cout << "recog > " << std::endl;
        Read(V, ">", S);
        Term(V, S);
        Build_tree(">", 2, S);
    } else
    if(V.begin()->token == "="){
        Read(V, "=", S);
        Term(V, S);
        Build_tree("=", 2, S);
    } else
    if(V.begin()->token == "<>"){
        Read(V, "<>", S);
        Term(V, S);
        Build_tree("<>", 2, S);
    }
}



void Primary(std::vector<Token> &V, std::stack<Node*> &S){
    if(V.begin()-> token == "-"){
        Read(V, "-", S);
        Primary(V, S);
        Build_tree("-", 1, S);
    } else
    if(V.begin()-> token == "+"){
        Read(V, "+", S);
        Primary(V, S);
        Build_tree("+", 1, S);
    } else
    if(V.begin()-> token == "not"){
        Read(V, "-", S);
        Primary(V, S);
        Build_tree("-", 1, S);
    } else
    if(V.begin()-> token == "eof"){
        Read(V, "eof", S);
        Build_tree("eof", 0, S);
    } else
    if(V.begin()-> token == "("){
        Read(V, "(", S);
        Expression(V, S);
        Read(V, ")", S);
    } else
    if(V.begin()-> token == "succ"){
        Read(V, "succ", S);
        Read(V, "(", S);
        Expression(V, S);
        Read(V, ")", S);
        Build_tree("succ", 1, S);
    } else
    if(V.begin()-> token == "pred"){
        Read(V, "pred", S);
        Read(V, "(", S);
        Expression(V, S);
        Read(V, ")", S);
        Build_tree("pred", 1, S);
    } else
    if(V.begin()-> token == "chr"){
        Read(V, "chr", S);
        Read(V, "(", S);
        Expression(V, S);
        Read(V, ")", S);
        Build_tree("chr", 1, S);
    } else
    if(V.begin()-> token == "ord"){
        Read(V, "ord", S);
        Read(V, "(", S);
        Expression(V, S);
        Read(V, ")", S);
        Build_tree("ord", 1, S);
    } else
    if(V.begin()-> type == "integer"){
        Read(V, "<integer>", S);
    } else
    if(V.begin()-> type == "char"){
        Read(V, "<char>", S);
    } else {
        Name(V, S);
        if(V.begin()->token == "("){
            int x = 1;
            Read(V, "(", S);
            Expression(V, S);
            while(V.begin()->token == ","){
                Read(V, ",", S);
                Expression(V, S);
                ++x;
            }
            Read(V, ")", S);
            Build_tree("call", x, S);
        }
    }
}

void Factor(std::vector<Token> &V, std::stack<Node*> &S){
    if(V.begin()-> token == "*"){
        Factor(V, S);
        Read(V, "*", S);
        Primary(V, S);
        Build_tree("*", 2, S);
    } else
    if(V.begin()-> token == "/"){
        Factor(V, S);
        Read(V, "/", S);
        Primary(V, S);
        Build_tree("/", 2, S);
    } else
    if(V.begin()-> token == "and"){
        Factor(V, S);
        Read(V, "and", S);
        Primary(V, S);
        Build_tree("and", 2, S);
    } else
    if(V.begin()-> token == "mod"){
        Factor(V, S);
        Read(V, "mod", S);
        Primary(V, S);
        Build_tree("mod", 2, S);
    } else {
        Primary(V,S);
    }
}

void Term(std::vector<Token> &V, std::stack<Node*> &S){
    Factor(V, S);
    while(V.begin()-> token == "+" || V.begin()-> token == "-" || V.begin()-> token == "or"){
        if(V.begin()-> token == "+"){
            Read(V, "+", S);
            Factor(V, S);
            Build_tree("+", 2, S);
        } else
        if(V.begin()-> token == "-"){
            Read(V, "-", S);
            Factor(V, S);
            Build_tree("-", 2, S);
        } else
        if(V.begin()-> token == "or"){
            Read(V, "or", S);
            Factor(V, S);
            Build_tree("or", 2, S);
        }
    }
}

void OutExp(std::vector<Token> &V, std::stack<Node*> &S){
    if(V.begin()->type == "str"){
        StringNode(V, S);
        Build_tree("string", 1, S);
    } else {
        Expression(V, S);
        Build_tree("integer", 1, S);
    }
}

void ForStat(std::vector<Token> &V, std::stack<Node*> &S);
void ForExp(std::vector<Token> &V, std::stack<Node*> &S);
void Caseclauses(std::vector<Token> &V, std::stack<Node*> &S);
void OtherwiseClause(std::vector<Token> &V, std::stack<Node*> &S);

void Statement(std::vector<Token> &V, std::stack<Node*> &S){
    if(V.begin()->token == "block"){
        //Body
        Read(V,"begin", S);
        int x = 1;
        Statement(V, S);
        while(V.begin()->token == ";"){
            ++x;
            Read(V, ";", S);
            Statement(V, S);
        }
        Read(V, "end", S);
        Build_tree("block", x, S);
    } else
    if(V.begin()->token == "output"){
        Read(V, "output", S);
        int x = 1;
        Read(V, "(", S);
        OutExp(V, S);
        while(V.begin()->token == ","){
            ++x;
            Read(V, ",", S);
            OutExp(V, S);
        }
        Read(V, ")", S);
        Build_tree("output", x, S);
    } else
    if(V.begin()->token == "if"){
        Read(V, "if", S);
        Expression(V, S);
        Read(V, "then", S);
        Statement(V, S);
        if(V.begin()->token == "else"){
            Read(V, "else", S);
            Statement(V, S);
            Build_tree("if",3, S);
        } else {
            Build_tree("if", 2, S);
        }
    } else
    if(V.begin()->token == "while"){
        Read(V, "while", S);
        Expression(V, S);
        Read(V, "do", S);
        Statement(V, S);
        Build_tree("while", 2, S);
    } else
    if(V.begin()->token == "repeat"){
        Read(V, "repeat", S);
        Statement(V, S);
        int x = 2;
        while(V.begin()->token == ";"){
            ++x;
            Read(V, ";", S);
            Statement(V, S);
        }
        Read(V, "until", S);
        Expression(V, S);
        Build_tree("repeat", x, S);
    } else
    if(V.begin()->token == "for"){
        Read(V, "for", S);
        Read(V, "(", S);
        ForStat(V, S);
        Read(V, ";", S);
        ForExp(V, S);
        Read(V, ";", S);
        ForStat(V, S);
        Read(V, ")", S);
        Statement(V, S);
        Build_tree("for", 4, S);
    } else
    if(V.begin()->token == "loop"){
        Read(V, "loop", S);
        int x = 1;
        Statement(V, S);
        while(V.begin()->token == ";"){
            ++x;
            Read(V, ";", S);
            Statement(V, S);
        }
        Read(V, "pool", S);
        Build_tree("loop", x, S);
    } else
    if(V.begin()->token == "case"){
        Read(V, "case", S);
        Expression(V, S);
        Read(V, "of", S);
        Caseclauses(V, S);
        OtherwiseClause(V, S);
        Read(V, "end", S);
        Build_tree("case", 3, S);
    } else
    if(V.begin()->token == "read"){
        Read(V, "read", S);
        Read(V, "(", S);
        int x = 1;
        Name(V, S);
        while(V.begin()->token == ","){
            ++x;
            Read(V, ",", S);
            Statement(V, S);
        }
        Read(V, ")", S);
        Build_tree("read", x, S);
    } else
    if(V.begin()->token == "exit"){
        Read(V, "exit", S);
        Build_tree("exit", 0, S);
    } else
    if(V.begin()->token == "return"){
        Read(V, "return", S);
        Expression(V, S);
        Build_tree("return", 1, S);
    } else {
        Build_tree("<null>", 0, S);
    }
}

void Body(std::vector<Token> &V, std::stack<Node*> &S){
    Read(V,"begin", S);
    int x = 1;
    Statement(V, S);
    while(V.begin()->token == ";"){
        ++x;
        Read(V, ";", S);
        Statement(V, S);
    }
    Read(V, "end", S);
    Build_tree("block", x, S);
}

void CaseExpression(std::vector<Token> &V, std::stack<Node*> &S){
    ConstValue(V, S);
    if(V.begin()->token == ".."){
        ConstValue(V, S);
        Build_tree("..", 2, S);
    }
}

void Caseclause(std::vector<Token> &V, std::stack<Node*> &S){
    CaseExpression(V, S);
    int x = 2;
    while(V.begin()->token == ","){
        Read(V, ",", S);
        CaseExpression(V, S);
        ++x;
    }
    Read(V, ":", S);
    Statement(V, S);
    Build_tree("case_clause", x, S);
}

void Caseclauses(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 1;
    Caseclause(V, S);
    while(V.begin()->token == ";"){
        Read(V, ";", S);
        ++x;
        Caseclause(V, S);
    }

}

void OtherwiseClause(std::vector<Token> &V, std::stack<Node*> &S){
    if(V.begin()->token == "otherwise"){
        Statement(V, S);
        Build_tree("otherwise", 1, S);
    }
}

void Assignment(std::vector<Token> &V, std::stack<Node*> &S){
    Name(V, S);
    if(V.begin()->token == ":="){
        Read(V, ":=", S);
        Expression(V, S);
        Build_tree("assign", 2, S);
    } else
    if(V.begin()->token == ":=:"){
        Read(V, ":=:", S);
        Name(V, S);
        Build_tree("swap", 2, S);
    } else {
         exit(EXIT_FAILURE);
    }
}

void ForStat(std::vector<Token> &V, std::stack<Node*> &S){
    if(V.begin()->type == "identifier"){
        Assignment(V, S);
    } else {
        Build_tree("<null>", 0, S);
    }
}

void ForExp(std::vector<Token> &V, std::stack<Node*> &S){
    if(V.begin()->type == "identifier" || V.begin()->type == "char" || V.begin()->token == "+" || V.begin()->token == "not" || V.begin()->token == "succ" || V.begin()->token == "chr" || V.begin()->token == "ord" || V.begin()->token == "eof" || V.begin()->token == "(" || V.begin()->token == ")")
    {
        Expression(V, S);
    } else {
        Build_tree("true", 0, S);
    }
}

void Fcn(std::vector<Token> &V, std::stack<Node*> &S){
    Read(V, "function", S);
    Name(V, S);
    Read(V, "(", S);
    Params(V, S);
    Read(V, ")", S);
    Read(V, ":", S);
    Name(V, S);
    Read(V, ";", S);
    Consts(V, S);
    Types(V, S);
    Dclns(V, S);
    Body(V, S);
    Name(V, S);
    Read(V, ";", S);
    Build_tree("fcn", 8, S);
}

void SubProgs(std::vector<Token> &V, std::stack<Node*> &S){
    int x = 0;
    while(V.begin()->token== "function"){
        Fcn(V, S);
    }
    Build_tree("subprogs" , x, S);
}



void Tiny(std::vector<Token> &V, std::stack<Node*> &S){

    Read(V, "program", S);
    Name(V, S);
    Read(V, ":", S);
    Consts(V, S);
    Types(V, S);
    Dclns(V, S);
    SubProgs(V, S);
    Body(V, S);
    Name(V, S);
    Read(V, ".", S);
    std::cout <<"Tiny" << std::endl;
    Build_tree("program", 7, S);
}

//identifies tokens and removes comment, puts everything into a vector
std::vector<Token> LexVec (std::string file){
    std::vector<Token> out;
    std::ifstream fFile;
    std::string working;
    std::string last = "";
    fFile.open(file);
    if(!fFile.is_open()){
        exit(EXIT_FAILURE);
    }

    //put whole file into string
    std::string whole( (std::istreambuf_iterator<char>(fFile)) , (std::istreambuf_iterator<char>()    ) );
    //add spaces strategically for easier parsing

    for(int i = 0; i<whole.length(); ++i){

        if(whole.at(i) == ';' || whole.at(i) == '(' || whole.at(i) == ')' || whole.at(i) == '.' || whole.at(i) == '+' || whole.at(i) == '-' || whole.at(i) == '*' || whole.at(i) == '/' || whole.at(i) == '\"'){
            //std::cout << "found a thing" << whole.at(i) << std::endl;
            whole.insert(i+1, " ");
            whole.insert(i, " ");
            i+=2;
        } else
        if(whole.at(i) == ':'){

            if(whole.at(i-1) == '='){
                whole.insert(i+1, " ");
                i+=2;
            } else if(whole.at(i+1) == '='){
                whole.insert(i, " ");
                ++i;
            } else {
                //std::cout << "found a lone :" << std::endl;
                whole.insert(i+1, " ");
                whole.insert(i, " ");
                i+=2;
            }

        } else
        if(whole.at(i) == '<' || whole.at(i) == '>'){
            //std::cout << "found a comparison" << std::endl;
            whole.insert(i, " ");
            ++i;
            if(whole.at(i+1) != '=' && whole.at(i+1) != '>'){
                whole.insert(i+1, " ");
                i+=2;
            }
        } else
        if(whole.at(i) == '='){
            if(whole.at(i-1) != '<' && whole.at(i-1) != '>' && whole.at(i-1) != ':'){
                whole.insert(i, " ");
                ++i;
            }
            if(whole.at(i+1) != ':'){
                whole.insert(i+1, " ");
                i+=2;
            }
        }

    }

    //std::cout << whole;


    std::stringstream inFile(whole);
    inFile >> working;

    //classify tokens and add them to the vector, ignoring comments
    while(inFile.good()){
        if(working == "{"){
            while(working != "}"){
                inFile >> working;
            }
            inFile >> working;
        }
        bool varDec = false;
        bool strDec = false;
        Token p;
        p.type = "";
        if(working == "var"){
            varDec = true;
        }
        if(!strDec && working == "\""){
            strDec = true;
        }
        if(!strDec){
            bool isInt = true;
            for(int i = 0; i < working.length(); ++i){
                if(!(working.at(i) >= '0' && working.at(i) <= '9')){
                    isInt = false;
                }
            }
            if(isInt){
                p.type = "integer";
            } else if (working.at(0) == '\''){
                p.type = "char";
                //working = "" + working.at(1);
            } else if (working != "program" && working != "var" && working != "const" &&
                        working != "type" && working != "function" && working != "return" &&
                         working != "begin" && working != "end" && working != ":=:" &&
                          working != ":=" && working != "output" && working != "if" &&
                           working != "then" && working != "else" && working != "while" &&
                            working != "do" && working != "case" && working != "of" &&
                             working != ".." && working != "otherwise" && working != "repeat" &&
                              working != "for" && working != "until" && working != "loop" &&
                               working != "pool" && working != "exit" && working != "<=" &&
                                working != "<>" && working != "<" && working != ">=" &&
                                 working != ">" && working != "=" && working != "mod" &&
                                  working != "and" && working != "or" && working != "not" &&
                                   working != "read" && working != "succ" && working != "pred" &&
                                    working != "chr" && working != "ord" && working != "eof" &&
                                     working != "{" && working != "}" && working != ":" &&
                                      working != ";" && working != "." && working != "," &&
                                       working != "(" && working != ")" && working != "+" &&
                                        working != "-" && working != "*" && working != "/" ){
                                                p.type = "identifier";
                                        }
        } else {
            p.token = working;
            out.push_back(p);
            last = working;
            p.type = "";
            inFile >> working;
            std::string w = working;
            while(working != "\""){
                w += " " + working;
                inFile >> working;
            }
            p.token = w;
            p.type = "string";
        }


        p.token = working;
        out.push_back(p);
        last = working;
        p.type = "";
        inFile >> working;
    }

    return out;

}

int main(int argc, char * argv[]){
    std::string path;
    if(argc > 2){
        path=std::string(argv[2]);
    }else{
        path=std::string(argv[1]);
    }
    std::vector<Token> V = LexVec(path);
    std::stack<Node*> S;
    for(int i = 0; i < V.size(); ++i){
        std::cout << V[i].token << " tt " << V[i].type << std::endl;
    }
    Tiny(V, S);
    return 0;
}
