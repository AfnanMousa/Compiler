#include "parse_grammar.h"
#include <bits/stdc++.h>
#include <string>
parse_grammar::parse_grammar()
{
    //ctor
}

void parse_grammar:: prepare_follow (string LHS ,vector<string> tokens){
    int sz = tokens.size();
    for (int i =0; i < sz-1 ; i++){
        if(tokens[i][0]!='\'' && tokens[i]!="\\L"){

            helper_Follow[tokens[i]].first.push_back({tokens[i+1], LHS});
        }
    }
    if(tokens[sz-1][0]!='\'' && tokens[sz-1]!="\\L"){
        helper_Follow[tokens[sz-1]].second.push_back(LHS);
    }
}

vector<string> parse_grammar:: splits(string str, string del){
    while (str[str.length()-1]==' ')
        str.erase(str.length()-1,1);
   /* while (str[0]==' ')
        str.erase(0,1);
    */
	vector<string> result;
	int start = 0;
	int end = str.find(del);
	while (end != -1) {
        string temp = str.substr(start, end - start);
       /* while (temp[temp.length()-1]==' ')
            temp.erase(temp.length()-1,1);*/
		result.push_back(temp);
		start = end + del.size();
		end = str.find(del, start);
	}

	result.push_back(str.substr(start, end - start));
	return result;
}

string parse_grammar:: extract_from_line(string line){
    cout <<line <<endl;
    line.erase(0,2);    // to remove "# " from the start of line
    vector<string> parts = splits(line," = ");
    ordered_follow.push_back(parts[0]);
    vector<string> productions = splits(parts[1],"| ");
    vector <pair<string,string>> temp ;
    for(int i=0;i<productions.size();i++){
        string prod = productions[i];
        vector<string> within_prod = splits(productions[i]," ");
        prepare_follow (parts[0] ,within_prod);
        if(within_prod[0].rfind("'",0)==0){    // terminal
            within_prod[0].erase(0,1);
            within_prod[0].erase(within_prod[0].length()-1,1);
            temp.push_back(make_pair(within_prod[0],prod));
        }
        else if (within_prod[0]=="\\L"){
            temp.push_back(make_pair("\\L",prod));
        }
        else{       //non terminal
            for(int p=0;p<First.find(within_prod[0])->second.size();p++)
                temp.push_back(make_pair(First.find(within_prod[0])->second.at(p).first,prod));
            ///////// here handle special case of First /////
        }
    }
    First.insert({parts[0],temp});
    return parts[0];
}
map<string,vector<pair<string,string>>> parse_grammar::  get_first (){
    return First;
}

vector <string> parse_grammar:: get_ordered_follow(){
    return ordered_follow;
}

map<string,pair<vector<pair<string,string>>,vector<string>>> parse_grammar:: get_helper_Follow(){
    return helper_Follow;
}


