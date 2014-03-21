/*
 
      ___           ___           ___     
     /\__\         /\  \         /\__\    
    /::|  |       /::\  \       /:/  /    
   /:|:|  |      /:/\:\  \     /:/  /     
  /:/|:|  |__   /::\~\:\  \   /:/  /  ___ 
 /:/ |:| /\__\ /:/\:\ \:\__\ /:/__/  /\__\
 \/__|:|/:/  / \:\~\:\ \/__/ \:\  \ /:/  /
     |:/:/  /   \:\ \:\__\    \:\  /:/  / 
     |::/  /     \:\ \/__/     \:\/:/  /  
     /:/  /       \:\__\        \::/  /   
     \/__/         \/__/         \/__/    
 
 
Neu, Copyright (c) 2013-2014, Andrometa LLC
All rights reserved.

neu@andrometa.net
http://neu.andrometa.net

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
 
*/

#include <neu/NMParser.h>

#include "NMParser_.h"

using namespace std;
using namespace neu;

namespace{
  
  enum FunctionKey{
    FKEY_Times_n=1,
    FKEY_Plus_n,
    FKEY_Power_2,
    FKEY_Set_2,
    FKEY_Mod_2,
    FKEY_Less_2,
    FKEY_Greater_2,
    FKEY_LessEqual_2,
    FKEY_GreaterEqual_2,
    FKEY_Equal_2,
    FKEY_Unequal_2,
    FKEY_Not_1,
    FKEY_And_2,
    FKEY_Or_2,
    FKEY_PreIncrement_1,
    FKEY_Increment_1,
    FKEY_PreDecrement_1,
    FKEY_Decrement_1,
    FKEY_Exp_1,
    FKEY_Sqrt_1,
    FKEY_Abs_1,
    FKEY_Log_1,
    FKEY_Cos_1,
    FKEY_ArcCos_1,
    FKEY_Cosh_1,
    FKEY_Sin_1,
    FKEY_ArcSin_1,
    FKEY_Sinh_1,
    FKEY_Tan_1,
    FKEY_ArcTan_1,
    FKEY_Tanh_1
  };
  
  enum SymbolKey{
    SKEY_True=1,
    SKEY_False
  };
  
  typedef NMap<nstr, SymbolKey> SymbolMap;
  
  typedef NMap<pair<nstr, int>, FunctionKey> FunctionMap;
  
  static FunctionMap _functionMap;
  
  static SymbolMap _symbolMap;
  
  void _initFunctionMap(){
    _functionMap[{"Times", -1}] = FKEY_Times_n;
    _functionMap[{"Plus", -1}] = FKEY_Plus_n;
    _functionMap[{"Power", 2}] = FKEY_Power_2;
    _functionMap[{"Set", 2}] = FKEY_Set_2;
    _functionMap[{"Mod", 2}] = FKEY_Mod_2;
    _functionMap[{"Less", 2}] = FKEY_Less_2;
    _functionMap[{"Greater", 2}] = FKEY_Greater_2;
    _functionMap[{"LessEqual", 2}] = FKEY_LessEqual_2;
    _functionMap[{"GreaterEqual", 2}] = FKEY_GreaterEqual_2;
    _functionMap[{"Equal", 2}] = FKEY_Equal_2;
    _functionMap[{"Unequal", 2}] = FKEY_Unequal_2;
    _functionMap[{"Not", 1}] = FKEY_Not_1;
    _functionMap[{"And", 2}] = FKEY_And_2;
    _functionMap[{"Or", 2}] = FKEY_Or_2;
    _functionMap[{"PreIncrement", 1}] = FKEY_PreIncrement_1;
    _functionMap[{"Increment", 1}] = FKEY_Increment_1;
    _functionMap[{"PreDecrement", 1}] = FKEY_PreDecrement_1;
    _functionMap[{"Decrement", 1}] = FKEY_Decrement_1;
    _functionMap[{"Exp", 1}] = FKEY_Exp_1;
    _functionMap[{"Sqrt", 1}] = FKEY_Sqrt_1;
    _functionMap[{"Abs", 1}] = FKEY_Abs_1;
    _functionMap[{"Log", 1}] = FKEY_Log_1;
    _functionMap[{"Cos", 1}] = FKEY_Cos_1;
    _functionMap[{"ArcCos", 1}] = FKEY_ArcCos_1;
    _functionMap[{"Cosh", 1}] = FKEY_Cosh_1;
    _functionMap[{"Sin", 1}] = FKEY_Sin_1;
    _functionMap[{"ArcSin", 1}] = FKEY_ArcSin_1;
    _functionMap[{"Sinh", 1}] = FKEY_Sinh_1;
    _functionMap[{"Tan", 1}] = FKEY_Tan_1;
    _functionMap[{"ArcTan", 1}] = FKEY_ArcTan_1;
    _functionMap[{"Tanh", 1}] = FKEY_Tanh_1;
  }
  
  void _initSymbolMap(){
    _symbolMap["True"] = SKEY_True;
    _symbolMap["False"] = SKEY_False;
  }
  
  class _FunctionMapLoader{
  public:
    _FunctionMapLoader(){
      _initSymbolMap();
      _initFunctionMap();
    }
  };
  
  static _FunctionMapLoader* _functionMapLoader = new _FunctionMapLoader();
  
} // end namespace

void NMParser_::translate(nvar& v){
  if(v.isSymbol()){
    SymbolMap::const_iterator itr = _symbolMap.find(v.str());
    
    SymbolKey key = itr->second;
    
    switch(key){
      case SKEY_True:
        v = true;
        break;
      case SKEY_False:
        v = false;
        break;
      default:
        break;
    }
    return;
  }
  
  size_t size = v.size();
  
  FunctionMap::const_iterator itr =
  _functionMap.find({v.str(), size});
  
  if(itr == _functionMap.end()){
    itr = _functionMap.find({v.str(), -1});
  }
  
  if(itr == _functionMap.end()){
    error(v, "unrecognized function: " + v);
    return;
  }
  
  FunctionKey key = itr->second;
  
  switch(key){
    case FKEY_Set_2:
    case FKEY_Mod_2:
    case FKEY_Not_1:
    case FKEY_And_2:
    case FKEY_Or_2:
    case FKEY_Exp_1:
    case FKEY_Sqrt_1:
    case FKEY_Abs_1:
    case FKEY_Log_1:
    case FKEY_Cos_1:
    case FKEY_Sin_1:
    case FKEY_Sinh_1:
    case FKEY_Tan_1:
    case FKEY_Tanh_1:
      return;
    case FKEY_Times_n:
      if(size < 2){
        break;
      }
      
      v.str() = "Mul";
      return;
    case FKEY_Plus_n:
      if(size < 2){
        break;
      }
      
      v.str() = "Add";
      return;
    case FKEY_Power_2:
      v.str() = "Pow";
      return;
    case FKEY_Less_2:
      v.str() = "LT";
      return;
    case FKEY_Greater_2:
      v.str() = "GT";
      return;
    case FKEY_LessEqual_2:
      v.str() = "LE";
      return;
    case FKEY_GreaterEqual_2:
      v.str() = "GE";
      return;
    case FKEY_Equal_2:
      v.str() = "EQ";
      return;
    case FKEY_Unequal_2:
      v.str() = "NE";
      return;
    case FKEY_Increment_1:
      v.str() = "PostInc";
      return;
    case FKEY_PreIncrement_1:
      v.str() = "Inc";
      return;
    case FKEY_Decrement_1:
      v.str() = "PostDec";
      return;
    case FKEY_PreDecrement_1:
      v.str() = "Dec";
      return;
    case FKEY_ArcCos_1:
      v.str() = "ACos";
      return;
    case FKEY_Cosh_1:
      v.str() = "CosH";
      return;
    case FKEY_ArcSin_1:
      v.str() = "ASin";
      return;
    case FKEY_ArcTan_1:
      v.str() = "Atan";
      return;
  }
  
  error(v, "unrecognized function: " + v);
}

NMParser::NMParser(){
  x_ = new NMParser_(this);
}

NMParser::~NMParser(){
  delete x_;
}

nvar NMParser::parse(const nstr& code, nvar* tags){
  return x_->parse(code, tags);
}

nvar NMParser::parseFile(const nstr& path, nvar* tags){
  return x_->parseFile(path, tags);
}
