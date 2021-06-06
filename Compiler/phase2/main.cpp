#include <iostream>
#include <fstream>
#include "parse_grammar.h"
#include "left_factoring.h"
#include "generate_follow.h"
#include "generate_table.h"
#include "check.h"
#include "../phase1/input_rules.h"
#include <bits/stdc++.h>

using namespace std;
using std::ifstream;
parse_grammar parse2;
generate_follow follower;
Left_Factoring left_fact;
generate_table gtable;

input_rules ip;

void print(map<string,vector< pair<string,string>>> First){
    map<string,vector< pair<string,string>>>::iterator itr;
    for (itr = First.begin(); itr != First.end(); ++itr) {
        for(int i=0;i<itr->second.size();i++){
            cout << itr->first << '\t' ;
            cout << itr->second[i].first << '\t' << itr->second[i].second <<'\n';
        }
        cout<<'\n'<<"-------------------------------------------------------"<<'\n';
    }
}

vector<string> Fix_Left_Recursion(string line){
    line.erase(0,2);
    vector<string> result;
    vector<string> parts = parse2.splits(line," = ");
    vector<string> productions = parse2.splits(parts[1],"| ");
    int has_left_recursion = -1;
    for(int i=0;i<productions.size();i++){
        vector<string> temp = parse2.splits(productions[i]," ");
        if(temp[0]==parts[0]){
            has_left_recursion = i;
            break;
        }
    }
    if(has_left_recursion == -1){       // no left recursion in this production
        result.push_back("# "+line);
        return result;
    }
    string new_nonTerminal = parts[0] + "~";
    productions[has_left_recursion].erase(0,parts[0].length()+1);
    string new_line = "# "+new_nonTerminal + " = " + productions[has_left_recursion] + " " + new_nonTerminal + " | \\L";
    string old_line = "# "+parts[0] + " = ";
    for(int j=0;j<productions.size();j++){
        if(j != has_left_recursion){
            old_line += productions[j] + " " + new_nonTerminal ;
            if(j != productions.size()-1 && (j+1) != has_left_recursion)
                old_line += " | ";
        }
    }
    result.push_back(new_line);
    result.push_back(old_line);
    return result;
}

string readFile(char *fileName){
    string start_grammar = "";
    char c;
    string line ;
    ifstream myFile(fileName,std::ios::ate);
    std::streampos size = myFile.tellg();
    for(int i=1;i<=size;i++){
        myFile.seekg(-i,std::ios::end);
        myFile.get(c);
        if(c=='\n'){
            if(line[0]=='#'){
               /* if (left_fact.left_factoring(line, parse2)){
                    cout <<"hiiiiii" <<endl;
                    map <string, string> my_m =left_fact.get_new_production();
                    for (auto itr = my_m.rbegin(); itr != my_m.rend(); ++itr){
                        parse2.extract_from_line(itr->second);
                    }
                    my_m.clear();
                }
                else
                    parse2.extract_from_line(line);*/
                vector <string> lines = Fix_Left_Recursion(line);
                for(int i=0;i<lines.size();i++)
                    parse2.extract_from_line(lines.at(i));
                line = "";
            }
        }
        else
            line = c + line;
    }

    /* if (left_fact.left_factoring(line, parse2)){
        left_fact.left_factoring(line, parse2);
        map <string, string> my_m =left_fact.get_new_production();
        for (auto itr = my_m.rbegin(); itr != my_m.rend(); ++itr){
            parse2.extract_from_line(itr->second);
        }
        my_m.clear();
    }
    else
        start_grammar = parse2.extract_from_line(line);*/
    vector <string> lines2 = Fix_Left_Recursion(line);
    for(int i=0;i<lines2.size();i++)
          start_grammar = parse2.extract_from_line(lines2.at(i));
    return start_grammar ;
}

void print_follow (map<string,vector<string>> follow){
    for (auto m: follow ){
       cout << m.first<< "                      ";
       for (auto v: m.second){
            cout << v <<" ";
       }
        cout << endl;
   }
}

void print_table (map<string,map<string,string>> Parsing_Table){
   cout<<"****************\n";
   cout<<"Parsing Table is:" <<"\n";
   map<string, map<string, string>>::iterator i;
   map<string, string>::iterator itr;
   for(i = Parsing_Table.begin(); i != Parsing_Table.end(); ++i){
        cout << i-> first << '\n';
        for (itr = i->second.begin(); itr != i->second.end(); ++itr) {
            cout << '\t' << itr->first<< '\t' << itr->second << '\n';
        }
    }
}



int main(){
   string start_grammar = readFile("grammar.txt");
   map<string,vector<pair<string,string>>> first = parse2.get_first();
   print(first);

   map<string,vector<string>> follow = follower.get_follow(parse2);
   print_follow(follow);

   map<string,map<string,string>> Parsing_Table = gtable.get_Parsing_Table(first,follow);
   print_table(Parsing_Table);

   ip.generate_automatas();

    cout<<"\n***************** \n Let's start\n \n";
    cout<<"Stack \t\t\t"<<"Input \t\t\t"<<"Output \t\t\n";
    //check the tokens:
    stack<string> Main_stack;
    Main_stack.push("$");
    Main_stack.push(start_grammar);

    string token = ip.get_next_token() ;
    while(token != "$"){
       Parser_Handle(Parsing_Table, &Main_stack, token);
       token = ip.get_next_token();
    }
    Parser_Handle(Parsing_Table, &Main_stack, "$");

   return 0;
}
