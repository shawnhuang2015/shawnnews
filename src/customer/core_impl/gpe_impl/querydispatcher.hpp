#ifndef GPERUN_QUERYDISPATCHER_HPP_
#define GPERUN_QUERYDISPATCHER_HPP_
#include <gpe/serviceimplbase.hpp>
#include <gpelib4/udfs/udfinclude.hpp>
using namespace gperun;
namespace UDIMPL { 
  class QueryDispatcher {
    private:
      std::string& QueryName;
      ServiceAPI& ServiceApi;
      EngineServiceRequest& Request;

    public:
      QueryDispatcher(
        std::string& query_name,
        ServiceAPI& serviceapi, EngineServiceRequest& request):
        QueryName(query_name),
        ServiceApi(serviceapi),
        Request(request) { }
      bool RunQuery() { return true; }
      template<class QueryUDF>
        bool Run(QueryUDF& udf)  {
          ServiceApi.RunUDF(&Request, &udf);
          if(udf.abortmsg_.size() > 0) {
            Request.error_ = true;
            Request.message_ += udf.abortmsg_;
            return true;
           }
          return true; 
        }
  };
}  // namepsace UDIMPL
#endif /* GPERUN_QUERYDISPATCHER_HPP_ */ 


