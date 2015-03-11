
#include <ReducerLib.hpp>

using namespace std;


/**
 *  Unit testing of the token bank functions
 */ 
int main(){

  vector<uint64_t> input;

  input.push_back(1);
  input.push_back(2);
  input.push_back(3);
  input.push_back(4);

  cout<<gsql_sum_uint(input)<<endl;


  vector<const char*> input2;
  vector<uint32_t> inputLen;

  char a[3]= {'a','b','c'};
  char b[2]= {'d','e'};
  char c[1]= {'f'};

  input2.push_back(a);
  input2.push_back(b);
  input2.push_back(c);

  inputLen.push_back(3);
  inputLen.push_back(2);
  inputLen.push_back(1);

  uint32_t outputLen;
  char outputBuffer[2000];

  gsql_concat_str(input2, inputLen, outputBuffer, outputLen);

  for (int i=0; i<outputLen; i ++){
    cout<<outputBuffer[i]<<"," ;
  }
  cout<<endl;


}
