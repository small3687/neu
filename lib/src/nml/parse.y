%{
  
  /*================================= Neu =================================
   
   Copyright (c) 2013-2014, Andrometa LLC
   All rights reserved.
   
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
   
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   
   =======================================================================*/
  
  #include <string>
  #include <iostream>
  
  #include "NMLParser_.h"
  #include "parse.h"
  
  using namespace std;
  using namespace neu;
  
  %}

%name-prefix="numl_"
%pure-parser
%parse-param {NMLParser_* PS}
%parse-param {void* scanner}
%lex-param {yyscan_t* scanner}

%token<v> IDENTIFIER STRING_LITERAL EQ NE GE LE INC ADD_BY SUB_BY MUL_BY DIV_BY MOD_BY AND OR KW_TRUE KW_FALSE KW_NONE KW_UNDEF KW_NEW ENDL DOUBLE INTEGER REAL

%type<v> stmt expr expr_num expr_map expr_vec expr_list get_item get_vec func func_vec block stmts

%left ','
%right '=' ADD_BY SUB_BY MUL_BY DIV_BY MOD_BY
%right OR
%right AND
%right EQ NE
%right '<' '>' GE LE
%left PUSH
%left '+' '-'
%left '*' '/' '%'
%left '^'
%left '!' INC DEC

%%

input: /* empty */
| input stmt {
  PS->emit($2);
}
| input func '{' expr '}'{
  PS->emit(PS->func("Def") << $2 << $4);
}
| input func block {
  PS->emit(PS->func("Def") << $2 << $3);
}
;

expr_num: DOUBLE {
  $$ = PS->var($1);
}
| INTEGER {
  $$ = PS->var($1);
}
| REAL {
  $$ = PS->var($1);
}
;

expr: expr_num {
  $$ = move($1);
}
| IDENTIFIER {
  $$ = PS->sym($1);
}
| KW_TRUE {
  $$ = PS->var(true);
}
| KW_FALSE {
  $$ = PS->var(false);
}
| KW_UNDEF {
  $$ = PS->var(undef);
}
| KW_NONE {
  $$ = PS->var(none);
}
| STRING_LITERAL {
  $$ = move($1);
}
| '-' expr {
  if($2.isNumeric()){
    $$ = PS->var(-$2);
  }
  else{
    $$ = PS->func("Neg") << move($2);
  }
}
| '!' expr {
  $$ = PS->func("Not") << move($2);
}
| expr '+' expr {
  $$ = PS->func("Add") << move($1) << move($3);
}
| expr '-' expr {
  $$ = PS->func("Sub") << move($1) << move($3);
}
| expr '*' expr {
  $$ = PS->func("Mul") << move($1) << move($3);
}
| expr '/' expr {
  $$ = PS->func("Div") << move($1) << move($3);
}
| expr '^' expr {
  $$ = PS->func("Pow") << move($1) << move($3);
}
| expr '%' expr {
  $$ = PS->func("Mod") << move($1) << move($3);
}
| expr INC {
  $$ = PS->func("PostInc") << move($1);
}
| INC expr {
  $$ = PS->func("Inc") << move($2);
}
| expr DEC {
  $$ = PS->func("PostDec") << move($1);
}
| DEC expr {
  $$ = PS->func("Dec") << move($2);
}
| expr ADD_BY expr {
  $$ = PS->func("AddBy") << move($1) << move($3);
}
| expr SUB_BY expr {
  $$ = PS->func("SubBy") << move($1) << move($3);
}
| expr MUL_BY expr {
  $$ = PS->func("MulBy") << move($1) << move($3);
}
| expr DIV_BY expr {
  $$ = PS->func("DivBy") << move($1) << move($3);
}
| expr MOD_BY expr {
  $$ = PS->func("ModBy") << move($1) << move($3);
}
| expr '=' expr {
  if($1.isSymbol()){
    $$ = PS->func("VarSet") << move($1) << move($3);
  }
  else{
    if($1.isFunction("Dot")){
      $1.str() = "DotPut";
    }
    $$ = PS->func("Set") << move($1) << move($3);
  }
}
| expr '<' expr {
  $$ = PS->func("LT") << move($1) << move($3);
}
| expr LE expr {
  $$ = PS->func("LE") << move($1) << move($3);
}
| expr '>' expr {
  $$ = PS->func("GT") << move($1) << move($3);
}
| expr GE expr {
  $$ = PS->func("GE") << move($1) << move($3);
}
| expr EQ expr {
  $$ = PS->func("EQ") << move($1) << move($3);
}
| expr NE expr {
  $$ = PS->func("NE") << move($1) << move($3);
}
| expr AND expr {
  $$ = PS->func("And") << move($1) << move($3);
}
| expr OR expr {
  $$ = PS->func("Or") << move($1) << move($3);
}
| '[' expr_vec ']' {
  $$ = move($2);
}
| '[' ':' expr ',' expr_vec ']' {
  $$ = move($5);
  $$.setHead($3);
}
| '(' expr_list ')' {
  if($2.size() == 1){
    $$ = move($2[0]);
  }
  else{
    $$ = move($2);
  }
}
| '(' expr_list ',' ')' {
  $$ = move($2);
}
| '(' ':' expr ',' expr_list ')' {
  $$ = move($5);
  $$.setHead($3);
}
| IDENTIFIER get_vec {
  $$ = undef;
  PS->handleGet(PS->sym($1), $2, $$);
}
| KW_NEW func {
  $$ = PS->func("New") << move($2);
}
| KW_NEW IDENTIFIER {
  $$ = PS->func("New") << PS->func($2);
}
| func {
  if(nstr::isLower($1.str()[0])){
    $$ = PS->func("Call") << move($1);
  }
  else{
    $$ = 1;
  }
}
;

expr_map: expr ':' expr {
  $$ = nvec();
  $$ << move($1) << move($3);
}

expr_vec: /* empty */ {
  $$ = nvec();
}
| expr_vec ',' expr {
  $$ = move($1);
  $$ << move($3);
}
| expr {
  $$ = nvec();
  $$ << move($1);
}
| expr_map {
  $$ = undef;
  $$($1[0]) = move($1[1]);
}
| expr_vec ',' expr_map {
  $$ = move($1);
  $$($3[0]) = move($3[1]);
}
;

expr_list: /* empty */ {
  $$ = nlist();
}
| expr_list ',' expr {
  $$ = move($1);
  $$ << $3;
}
| expr {
  $$ = nlist();
  $$ << $1;
}
| expr_map {
  $$($1[0]) = move($1[1]);
}
| expr_list ',' expr_map {
  $$ = move($1);
  $$($3[0]) = move($3[1]);
}
;

func: IDENTIFIER '(' func_vec ')' {
  $$ = PS->func($1);
  $$.append($3);
}
;

func_vec: /* empty */ {
  $$ = undef;
}
| func_vec ',' expr {
  $$ = move($1);
  $$ << move($3);
}
| expr {
  $$ = nvec();
  $$ << move($1);
}
;

stmt: expr ';' {
  $$ = move($1);
}
| ';' {
  $$ = none;
}
;

block: '{' stmts '}' {
  $$ = $2;
}
| '{' '}' {
  $$ = PS->func("Block");
}

stmts: stmts stmt {
  $$ = move($1);
  $$ << move($2);
}
| stmt {
  $$ = PS->func("Block") << move($1);
}
;

get_item: '[' expr ']' {
  $$ = PS->func("Idx") << move($2);
}
| '.' IDENTIFIER {
  $$ = PS->func("Dot") << PS->sym($2);
}
| '.' '(' expr ')' {
  $$ = PS->func("Dot") << move($3);
}
| '{' expr '}' {
  $$ = PS->func("Put") << move($2);
}
| '.' func {
  if(nstr::isLower($2.str()[0])){
    $$ = PS->func("Call") << move($2);
  }
  else{
    $$ = PS->func("In") << move($2);
  }
}
;

get_vec: get_vec get_item {
  $$ = move($1);
  $$ << move($2);
}
| get_item {
  $$ = nvec();
  $$ << move($1);
}
;

%%

int numl_error(NMLParser_* parser, const void*, const char *s){
  parser->error(s);
  return 1;
}
