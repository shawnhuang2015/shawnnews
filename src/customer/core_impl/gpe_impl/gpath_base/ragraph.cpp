#include "ragraph.hpp"
    
namespace UDIMPL {
  namespace GPATH_TOOLS {
// RA node factory based on ra type
    RANode* RANode::RANodeFactory(Json::Value& value) {
      std::string raType = value[ RA_TYPE ].asString();
      if (raType == SELECT) {
        return new SelectRANode(value);
      }
      else if (raType == PROJECT) {
        return new ProjectRANode(value);
      }
      else if (raType == GROUP) {
        return new GroupRANode(value);
      }
      else if (raType == UNION) {
        return new UnionRANode(value);
      }

      return NULL;
    }
  }
}
