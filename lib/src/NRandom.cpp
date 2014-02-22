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

#include <neu/NRandom.h>

#include <neu/NFuncMap.h>
#include <neu/NFactory.h>

using namespace std;
using namespace neu;

namespace{
  
  class FuncMap : public NFuncMap{
  public:
    FuncMap(){
      
      add("timeSeed", 0,
          [](void* o, const nvar& v) -> nvar{
            return static_cast<NRandom*>(o)->timeSeed();
          });
      
      add("uniform", 0,
          [](void* o, const nvar& v) -> nvar{
            return static_cast<NRandom*>(o)->uniform();
          });
      
      add("uniform", 2,
          [](void* o, const nvar& v) -> nvar{
            return static_cast<NRandom*>(o)->uniform(v[0], v[1]);
          });
      
      add("equilikely", 2,
          [](void* o, const nvar& v) -> nvar{
            return static_cast<NRandom*>(o)->equilikely(v[0], v[1]);
          });
      
      add("exponential", 1,
          [](void* o, const nvar& v) -> nvar{
            return static_cast<NRandom*>(o)->exponential(v[0]);
          });
      
      add("normal", 2,
          [](void* o, const nvar& v) -> nvar{
            return static_cast<NRandom*>(o)->normal(v[0], v[1]);
          });
      
      add("bernoulli", 1,
          [](void* o, const nvar& v) -> nvar{
            return static_cast<NRandom*>(o)->bernoulli(v[0]);
          });
      
      add("binomial", 2,
          [](void* o, const nvar& v) -> nvar{
            return static_cast<NRandom*>(o)->binomial(v[0], v[1]);
          });
      
      add("poisson", 1,
          [](void* o, const nvar& v) -> nvar{
            return static_cast<NRandom*>(o)->poisson(v[0]);
          });
      
      add("expSelect", 2,
          [](void* o, const nvar& v) -> nvar{
            return static_cast<NRandom*>(o)->expSelect(v[0], v[1]);
          });
    }
  };
  
  FuncMap _funcMap;
  
  class Factory : public NFactory{
  public:
    Factory() : NFactory("NRandom"){
      
    }
    
    NObjectBase* create(const nvar& f){
      switch(f.size()){
        case 0:
          return new NRandom;
        case 1:
          return new NRandom(f[0]);
        default:
          return 0;
      }
    }
  };
  
  Factory _factory;
  
} // end namespace

NFunc NRandom::handle(const nvar& v, uint32_t flags){
  return _funcMap.map(v);
}

