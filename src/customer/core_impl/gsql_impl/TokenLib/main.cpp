#include "TokenLib1.hpp"
#include "TokenLibM.hpp"
#include "ConditionLib1.hpp"
#include "ConditionLibM.hpp"

/**
 *  Unit testing of the token bank functions
 */ 
int main(){

  char* aa[2];
  char d[100]={'a','b','c','d','e','f'};

  aa[0]=&d[0];
  aa[1]=&d[3];

  uint32_t lenn[2];

  lenn[0] = 3;
  lenn[1] = 3;


  char b[100];
  uint32_t  outlen;
  gsql_concat(aa,lenn,2, b, outlen);
  for(int i =0; i<outlen; i++){
    std::cout<<b[i]<<",";
  }
  std::cout<<std::endl;



  char a[3]={'a','b','c'};

  uint32_t len =0;
  gsql_reverse(a,3,b, len);

  for(int i =0; i<3; i++){
    std::cout<<b[i]<<",";
  }

  std::cout<<len <<std::endl;

}

