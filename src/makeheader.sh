rm -rf ../src1/*
find . -name '*.hpp' | cpio -pdm ../src1
find . -name '*.h' | cpio -pdm ../src1

#cloc core
#astyle --recursive ~/product/src1/core/*.hpp ~/product/src1/utility/*.hpp
rm -rf ../src1/msg
rm -rf ../src1/third_party
rm -rf ../src1/unittests
rm ../src1/customer/core_ext/gpe/global_heap.hpp
rm ../src1/customer/core_ext/gpe/global_set.hpp
rm ../src1/customer/core_ext/gpe/global_vector.hpp
rm ../src1/customer/core_ext/gpe/registry.hpp
rm ../src1/customer/core_ext/gpe/unionfind.hpp
rm ../src1/customer/core_ext/gpe/enginejoblistener.hpp
rm ../src1/customer/core_ext/gpe/enginejobrunner.hpp
rm ../src1/customer/core_ext/gpe/gpe_daemon.hpp
rm ../src1/customer/core_ext/gpe/gpeconfig.hpp
#rm ../src1/customer/core_ext/gpe/serviceapi.hpp  #DirectUse
#rm ../src1/customer/core_ext/gpe/serviceimplbase.hpp  #DirectUse
#rm ../src1/customer/core_impl/gpe_impl/impl.hpp  #DirectUse
rm ../src1/customer/core_impl/gse_impl/loader_combiner.hpp
rm ../src1/customer/core_impl/gse_impl/loader_mapper.hpp
rm ../src1/customer/core_impl/gse_impl/loader_mapper_distributed.hpp
rm ../src1/customer/core_impl/gse_impl/canonnical_loader_mapper.hpp
rm ../src1/customer/core_impl/gse_impl/loader_mapper_mt.hpp
rm ../src1/customer/core_impl/gse_impl/post_impl.hpp  #DirectUse
rm ../src1/customer/core_impl/udt.hpp  #DirectUse
#rm ../src1/core/gpe/gapi4/baseedgesfilter.hpp  #DirectUse
#rm ../src1/core/gpe/gapi4/baseverticesfilter.hpp  #DirectUse
#rm ../src1/core/gpe/gapi4/edgescollection.hpp  #DirectUse
#rm ../src1/core/gpe/gapi4/graphapi.hpp  #DirectUse
rm ../src1/core/gpe/gapi4/graphreaders.hpp
#rm ../src1/core/gpe/gapi4/verticescollection.hpp  #DirectUse
#rm ../src1/core/gpe/gpelib4/bucketcontainers/addvaluecontainer.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/bucketcontainers/basesinglevaluecontainer.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/bucketcontainers/containerutil.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/bucketcontainers/multiplevaluecontainer.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/bucketcontainers/setvaluecontainer.hpp  #UseTemplate
rm ../src1/core/gpe/gpelib4/dlibimpl/common/attributeinterface.hpp
rm ../src1/core/gpe/gpelib4/dlibimpl/common/contextinterface.hpp
rm ../src1/core/gpe/gpelib4/dlibimpl/common/gpeinclude.hpp
rm ../src1/core/gpe/gpelib4/dlibimpl/common/udfinterface.hpp
rm ../src1/core/gpe/gpelib4/engine/distributedmasteradaptor.hpp
rm ../src1/core/gpe/gpelib4/engine/distributedworkeradaptor.hpp
rm ../src1/core/gpe/gpelib4/engine/distributedworkerrunner.hpp
#rm ../src1/core/gpe/gpelib4/engine/engineedgemaptask.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/engine/engineedgeprinttask.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/engine/enginereducetask.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/engine/enginevertexmaptask.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/engine/enginevertexprinttask.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/engine/initializehelper.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/engine/maphelper.hpp  #UseTemplate
rm ../src1/core/gpe/gpelib4/engine/master.hpp 
rm ../src1/core/gpe/gpelib4/engine/mpimasteradaptor.hpp
rm ../src1/core/gpe/gpelib4/engine/mpiworkeradaptor.hpp
#rm ../src1/core/gpe/gpelib4/engine/reducehelper.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/engine/singlemachineadaptor.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/engine/vertexmaphelper.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/engine/worker.hpp  #UseTemplate
rm ../src1/core/gpe/gpelib4/enginebase/adaptorcache.hpp
rm ../src1/core/gpe/gpelib4/enginebase/basedistributedworkadaptor.hpp
#rm ../src1/core/gpe/gpelib4/enginebase/baseudf.hpp  #DirectUse
#rm ../src1/core/gpe/gpelib4/enginebase/baseudfsetting.hpp  #DirectUse
#rm ../src1/core/gpe/gpelib4/enginebase/context.hpp   #UseTemplate
#rm ../src1/core/gpe/gpelib4/enginebase/engineiteratorsetter.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/enginebase/enginetask.hpp  #Inheritence
#rm ../src1/core/gpe/gpelib4/enginebase/globalvariables.hpp  #Inheritence
#rm ../src1/core/gpe/gpelib4/enginebase/localcontextcenter.hpp  #UseTemplate
#rm ../src1/core/gpe/gpelib4/enginebase/masteradaptor.hpp    #Inheritence
rm ../src1/core/gpe/gpelib4/enginebase/mpicomm.hpp
rm ../src1/core/gpe/gpelib4/enginebase/networktransmitscheduler.hpp
rm ../src1/core/gpe/gpelib4/enginebase/scheduler.hpp
rm ../src1/core/gpe/gpelib4/enginebase/scheduler_fifo.hpp
rm ../src1/core/gpe/gpelib4/enginebase/servicebase.hpp
#rm ../src1/core/gpe/gpelib4/enginebase/typefiltercontroller.hpp    #DirectUse
rm ../src1/core/gpe/gpelib4/enginebase/udfstatus.hpp
#rm ../src1/core/gpe/gpelib4/enginebase/variable.hpp    #DirectUse
#rm ../src1/core/gpe/gpelib4/enginebase/workeradaptor.hpp   #Inheritence
rm ../src1/core/gpe/gpelib4/enginebase/workerudfcreator.hpp
rm ../src1/core/gpe/gpelib4/enginedriver/degreehistogram.hpp
rm ../src1/core/gpe/gpelib4/enginedriver/distributedenginedriver.hpp
rm ../src1/core/gpe/gpelib4/enginedriver/enginedriver.hpp
rm ../src1/core/gpe/gpelib4/enginedriver/enginedriverservice.hpp
rm ../src1/core/gpe/gpelib4/enginedriver/enginempidriver.hpp
#rm ../src1/core/gpe/gpelib4/enginedriver/singleadaptorcache.hpp  #UseTemplate
rm ../src1/core/gpe/gpelib4/ext/idconverter_zmq.hpp
rm ../src1/core/gpe/gperun/enginejoblistener.hpp
rm ../src1/core/gpe/gperun/enginejobrunner.hpp
rm ../src1/core/gpe/gperun/pagerank4.hpp
rm ../src1/core/gpe/gperun/sampletopology4.hpp
rm ../src1/core/gse/ext/idconverter.hpp
rm ../src1/core/gse/gse2/base/gse_base_types.hpp
rm ../src1/core/gse/gse2/base/gse_base_util.hpp
rm ../src1/core/gse/gse2/base/serialisable.hpp
rm ../src1/core/gse/gse2/config/graph_config.hpp
rm ../src1/core/gse/gse2/config/sys_config.hpp
rm ../src1/core/gse/gse2/config/worker_config.hpp
rm ../src1/core/gse/gse2/dict/dict_segmap.hpp
rm ../src1/core/gse/gse2/dict/dict_service.hpp
#rm ../src1/core/gse/gse2/dict/enum/enum_mapper.hpp   #DirectUse
rm ../src1/core/gse/gse2/dict/enum/RedisEnumeratorClient.h
rm ../src1/core/gse/gse2/dict/impl/dict_segmap_redis.hpp
rm ../src1/core/gse/gse2/dict/impl/local_ids_segment_map.hpp
rm ../src1/core/gse/gse2/dict/redis/gse_redis_service.hpp
rm ../src1/core/gse/gse2/ids/impl/ids_client_worker.hpp
rm ../src1/core/gse/gse2/ids/impl/ids_map_impl.hpp
rm ../src1/core/gse/gse2/ids/impl/ids_map_impl_string.hpp
rm ../src1/core/gse/gse2/ids/impl/ids_server_worker.hpp
rm ../src1/core/gse/gse2/ids/interface/gse_loader.hpp
rm ../src1/core/gse/gse2/ids/interface/ids.hpp
rm ../src1/core/gse/gse2/ids/interface/ids_map.hpp
rm ../src1/core/gse/gse2/ids/interface/ids_worker.hpp
rm ../src1/core/gse/gse2/ids/types/ids_types.h
rm ../src1/core/gse/gse2/json/gse_json_lookup.hpp
rm ../src1/core/gse/gse2/json/gse_msg_json.hpp
rm ../src1/core/gse/gse2/json/gse_msg_json_kafka.hpp
rm ../src1/core/gse/gse2/json/gse_msg_mock_rest.hpp
rm ../src1/core/gse/gse2/loader/generic/edge_only_loader.hpp
rm ../src1/core/gse/gse2/loader/gse_loader_datatypes.hpp
rm ../src1/core/gse/gse2/loader/gse_loader_factory.hpp
rm ../src1/core/gse/gse2/loader/gse_loader_unordered_segment.hpp
rm ../src1/core/gse/gse2/loader/gse_parallel_loader.hpp
rm ../src1/core/gse/gse2/loader/gse_single_server_loader.hpp
rm ../src1/core/gse/gse2/loader/gse_single_server_loader_mt.hpp
rm ../src1/core/gse/gse2/loader/canonnical_loader_config.hpp
rm ../src1/core/gse/gse2/loader/canonnical_loader_mapper.hpp
rm ../src1/core/gse/gse2/loader/poc/acxiom_loader.hpp
rm ../src1/core/gse/gse2/loader/poc/alipay_loader.hpp
rm ../src1/core/gse/gse2/loader/poc/alipay_loader2.hpp
rm ../src1/core/gse/gse2/msg/gse_msg.hpp
rm ../src1/core/gse/gse2/msg/gse_msg_reader.hpp
rm ../src1/core/gse/gse2/msg/gse_msg_writer.hpp
rm ../src1/core/gse/gse2/msg/kafka/gse_msg_kafka.hpp
rm ../src1/core/gse/gse2/msg/zmq/gse_zmq_reader.hpp
rm ../src1/core/gse/gse2/msg/zmq/gse_zmq_writer.hpp
rm ../src1/core/gse/gse2/partition/gse_loader_combiner_base.hpp
rm ../src1/core/gse/gse2/partition/gse_part_datafeeder.hpp
rm ../src1/core/gse/gse2/partition/gse_part_merge_meta.hpp
rm ../src1/core/gse/gse2/partition/gse_segment_partition.hpp
rm ../src1/core/gse/gse2/partition/gse_single_server_repartition.hpp
rm ../src1/core/gse/gse2/partition/partitiondatafeeder.hpp
rm ../src1/core/gse/gse2/poc/acxiom_loader.hpp
rm ../src1/core/gse/gse2/test/gse_mock_rest.hpp
rm ../src1/core/gse/post_service/impl/post_delta_file_writer.hpp
rm ../src1/core/gse/post_service/impl/post_delta_kafka_writer.hpp
rm ../src1/core/gse/post_service/impl/post_kafka_reader.hpp
rm ../src1/core/gse/post_service/post_delta_reader.hpp
rm ../src1/core/gse/post_service/post_delta_writer.hpp
rm ../src1/core/gse/post_service/post_json2delta.hpp
rm ../src1/core/gse/post_service/post_listener.hpp
rm ../src1/core/gse/post_service/post_reader.hpp
rm ../src1/core/gse/post_service/post_restart.hpp
#rm ../src1/core/topology/topology4/attribute.hpp    #DirectUse
rm ../src1/core/topology/topology4/deltaattribute.hpp
#rm ../src1/core/topology/topology4/deltabase.hpp    #Base
rm ../src1/core/topology/topology4/deltaitembinary.hpp
rm ../src1/core/topology/topology4/deltarebuilder.hpp
rm ../src1/core/topology/topology4/deltarecords.hpp
#rm ../src1/core/topology/topology4/edgeattribute.hpp    #DirectUse
rm ../src1/core/topology/topology4/edgeblock.hpp
#rm ../src1/core/topology/topology4/edgeblockreader.hpp    #ReferencedByTemplateCode
rm ../src1/core/topology/topology4/edgeblockreadercachefactory.hpp
#rm ../src1/core/topology/topology4/metafiles.hpp    #DirectUse
rm ../src1/core/topology/topology4/samplingcontroller.hpp
rm ../src1/core/topology/topology4/segmentedges.hpp
rm ../src1/core/topology/topology4/segmentvertexattribute.hpp
#rm ../src1/core/topology/topology4/segmentvertexattributereader.hpp   #ReferencedByTemplateCode
rm ../src1/core/topology/topology4/testpartition.hpp
rm ../src1/core/topology/topology4/topologygraph.hpp
rm ../src1/core/topology/topology4/topologyprinter.hpp
#rm ../src1/core/topology/topology4/vertexattribute.hpp    #DirectUse
#rm ../src1/utility/gbucket/bucket.hpp  #UseTemplate
#rm ../src1/utility/gbucket/bucketcontainer.hpp  #Inheritence
#rm ../src1/utility/gbucket/buckettable.hpp  #UseTemplate
rm ../src1/utility/gmmnt/diskiorequest.hpp
rm ../src1/utility/gmmnt/disklocationid.hpp
rm ../src1/utility/gmmnt/globaldiskio.hpp
rm ../src1/utility/gmmnt/globaldiskiomemcache.hpp
#rm ../src1/utility/gmmnt/globalinstances.hpp
rm ../src1/utility/gmmnt/globalmemoryallocator.hpp
rm ../src1/utility/gmmnt/globalmemorybuffer.hpp
rm ../src1/utility/gmmnt/globalmemoryserver.hpp
rm ../src1/utility/gmmnt/memorycomponent.hpp
rm ../src1/utility/gmmnt/memorydatafileobject.hpp
rm ../src1/utility/gmmnt/memorydatamanager.hpp
#rm ../src1/utility/gmmnt/memorydataobject.hpp  #Inheritence
rm ../src1/utility/gmmnt/memorymultipledatafileobject.hpp
rm ../src1/utility/gnet/config.hpp
rm ../src1/utility/gnet/exception.hpp
rm ../src1/utility/gnet/msg_reader.hpp
rm ../src1/utility/gnet/msg_writer.hpp
rm ../src1/utility/gnet/kafka/kafka_group_reader.hpp
rm ../src1/utility/gnet/kafka/kafka_message.hpp
rm ../src1/utility/gnet/kafkaconnector.hpp
rm ../src1/utility/gnet/message.hpp
rm ../src1/utility/gnet/zk/zookeeper_context.hpp
rm ../src1/utility/gnet/zk/zookeeper_daemon.hpp
#rm ../src1/utility/gutil/bitqueue.hpp  #CodeInline
#rm ../src1/utility/gutil/compactreader.hpp  #CodeInline
rm ../src1/utility/gutil/compactwriter.hpp
rm ../src1/utility/gutil/compactwriter2.hpp
rm ../src1/utility/gutil/dummyqueue.hpp
rm ../src1/utility/gutil/filelinereader.hpp
rm ../src1/utility/gutil/gcharbuffer.hpp
rm ../src1/utility/gutil/gcircularbuffer.hpp
rm ../src1/utility/gutil/gcolorconsole.hpp
rm ../src1/utility/gutil/gdiskutil.hpp
rm ../src1/utility/gutil/genvvariable.hpp
rm ../src1/utility/gutil/gjsonhelper.hpp
#rm ../src1/utility/gutil/glogging.hpp  #Base
rm ../src1/utility/gutil/gmemorybuffer.hpp
rm ../src1/utility/gutil/gmmap.hpp
#rm ../src1/utility/gutil/goutputstream.hpp  #DirectUse
#rm ../src1/utility/gutil/gpelibidtypes.hpp  #Base
#rm ../src1/utility/gutil/gsorter.hpp  #DirectUse
rm ../src1/utility/gutil/gsparsearray.hpp
rm ../src1/utility/gutil/gsqlcontainer.hpp
rm ../src1/utility/gutil/gstring.hpp
rm ../src1/utility/gutil/gsyncwriter.hpp
rm ../src1/utility/gutil/gsystem.hpp
rm ../src1/utility/gutil/gthreadpool.hpp
rm ../src1/utility/gutil/gtimelib.hpp
rm ../src1/utility/gutil/gtimer.hpp
rm ../src1/utility/gutil/gyamlhelper.hpp
#rm ../src1/utility/gutil/iterator.hpp  #DirectUse
#rm ../src1/utility/gutil/jsonwriter.hpp  #DirectUse
rm ../src1/utility/gutil/lockfree_ringbuffer.hpp
rm ../src1/utility/gutil/mpi_util.hpp
rm ../src1/utility/gutil/networkmsgutil.hpp
rm ../src1/utility/gutil/requestidmaps.hpp
rm ../src1/utility/gutil/sequentialidmap.hpp
rm ../src1/utility/gutil/simplememarena.hpp
rm ../src1/utility/gutil/simpleringbuffer.hpp
rm ../src1/utility/gutil/simple_queue.hpp
#rm ../src1/utility/gutil/spinlock.hpp  #Base
rm ../src1/utility/gutil/string_ops.hpp
rm ../src1/utility/lib/base_io.hpp
rm ../src1/utility/lib/bit8.hpp
rm ../src1/utility/lib/json_to_gsql.hpp
rm ../src1/utility/lib/error_code.h
rm ../src1/utility/lib/process.h
rm ../src1/utility/lib/stddefines.h
find ../src1/ -type d -empty -delete

#cd src;./makeheader.sh;cd ..;/opt/local/bin/g++ -o build/debug/customer/core_ext/gpe/gperun.o -c -O0 -g3 -Wall -c -fmessage-length=0 -Wno-unused -fpermissive -fno-omit-frame-pointer  -DOS_MACOSX=1 -DOPENCODE -DComponentTest -Isrc/third_party -Isrc/third_party/boost -Isrc/third_party/yaml/usr/local/include -Isrc/third_party/sparsehash/usr/local/include -Isrc/third_party/glog/usr/local/include -Isrc/third_party/jsoncpp/include -Isrc/third_party/redis/deps/hiredis -Isrc/third_party/zlib -Isrc/third_party/zookeeper_c/include/zookeeper -Isrc/third_party/zeromq/include -Isrc1/customer -Isrc1/core -Isrc1/core/gse -Isrc1/core/gpe -Isrc1/core/topology -Isrc1/utility -Isrc1/msg -Isrc1/msg/distributed src/customer/core_ext/gpe/gperun.cpp
