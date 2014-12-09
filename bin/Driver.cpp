#include <iostream>
#include <dlfcn.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>

using namespace std;

int main(){

  typedef void (*TokenFunc)(const char* const input, uint32_t input_len, char *const output,uint32_t& output_len);
  typedef std::map<std::string, TokenFunc> FuncMap;

  FuncMap Name2FuncMap; 

  // open the library
  cout << "Opening TokenBank.so...\n";
  void* handle = dlopen("TokenBank.so", RTLD_LAZY);
  if (!handle) {
    cerr << "Cannot open library: " << dlerror() << '\n';
    return 1;
  }

  //create 2-element function array
  int funcNum = 2;
  char *error;
  void (*T[funcNum])(const char* const input, uint32_t input_len, char *const output,uint32_t& output_len);

  string f1 = "Reverse";
  string f2 = "Zero";

  uint32_t len;

  T[0]= (void (*)(const char* const input, uint32_t input_len, char *const output,uint32_t& output_len))dlsym(handle, f1.c_str());

  if ((error = dlerror()) != NULL)  {
    cout<<error<< " 1"<<endl;
    exit(1);
  }

  Name2FuncMap[f1]=T[0];

  T[1]= (void (*)(const char* const input, uint32_t input_len, char *const output, uint32_t& output_len))dlsym(handle, f2.c_str());

  if ((error = dlerror()) != NULL)  {
    cout<<error<<endl;
    exit(1);
  }

  Name2FuncMap[f2]=T[1];

  char a[3]={'a','b','c'};
  char b[100];

  FuncMap::const_iterator iter = Name2FuncMap.find(f1);
  if (iter == Name2FuncMap.end())
  {
    cout<<"cannot find funct " << f1<<endl;
  } else{
    (*iter->second)(a,3,b,len);
  }

  //(*T[0])(a,3,b,len);

  for(int i =0; i<3; i++){
    std::cout<<b[i]<<",";
  }

  std::cout<<len <<std::endl;

  iter = Name2FuncMap.find(f2);
  if (iter == Name2FuncMap.end())
  {
    cout<<"cannot find funct " << f2<<endl;
  } else{
    (*iter->second)(a,3,b,len);
  }

  //(*T[1])(a,3,b, len);

  for(int i =0; i<3; i++){

    std::cout<<b[i]<<",";
  }
  std::cout<<len<<std::endl;

}

