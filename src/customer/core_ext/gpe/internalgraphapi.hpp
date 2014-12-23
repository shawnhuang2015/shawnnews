#include <gapi4/graphapi.hpp>
#include <gpelib4/engine/singlemachineadaptor.hpp>
#include <gpelib4/enginedriver/singleadaptorcache.hpp>
#include <gpelib4/enginedriver/engineservicerequest.hpp>
#include <gse2/dict/enum/enum_mapper.hpp>

namespace topology4{
  class TopologyGraph;
}

namespace gse2{
  class PostJson2Delta;
}

namespace gperun{
  class ServiceImplBase;
  class EngineJobRunner;
}

namespace gnet{
  class MessageQueueFactory;
}

using gpelib4::EngineServiceRequest;
using gapi4::GraphAPI;
using gpelib4::WorkerAdaptor;
using gse2::PostJson2Delta;
using gse2::EnumMappers;
using topology4::TopologyMeta;
using gnet::MessageQueueFactory;
