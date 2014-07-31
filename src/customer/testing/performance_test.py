# -*- mode: python; -*-
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# File: Performance Test
# Date: 01/13/2014
# Author: Eric Chu
# Description: Performance test in order to the number of one query time of 
#         get_subgraph_topology for elementum.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# HEADER PACKAGES
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

import os
import sys
import time
import commands
import os.path
import subprocess
import random
import re
import json
import urllib2
import requests
from optparse import OptionParser

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# GLOBAL VARIABLE
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# vertex id soruce file path
source_vertex_file_path = "id_sample.txt"
# steps to traverse & test numer for each step
subgraph_steps_list = [1, 2]
test_num_each_step = 10;
test_result_file = "./get_subgraph_topology_performance_test.txt"
# for histogram statistics & divide the execution time by 100 ms & divide the subgraph size by vertex_num_interval
map_time_interval = 10
map_vertex_num_interval = 100

# query sample: time curl -X GET 'http://54.219.138.173:9000/graph/dummpy/query/subgraph/1?steps=2'
query_statement_template = "http://localhost:9000/kneighborhood/"
# the special characters that do not support in the test
invalid_character_list = ['\\', '#', '/', ' ']

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Test Function
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#@description: return a random testing source vertices for the test
#@return: a list of vertices
def get_random_test_source_vertices(vertex_num):
    test_source_list = []
    total_vertex_list = []

    # for random the input vertex
    #print("start to construct the testing source verices")
    with open(source_vertex_file_path, "r") as source_vertex_file:
        for line in source_vertex_file:
            vertex_id = line.rstrip('\n')
            if len(vertex_id) < 1 or any(invalid_char in vertex_id for invalid_char in invalid_character_list):
                continue
            else:
                total_vertex_list.append(vertex_id)

    # construct the test_source_list
    total_vertex_num = len(total_vertex_list)
    for i in range(0, test_num_each_step):
        random_idx = random.randint(0, total_vertex_num-1)
        test_source_list.append(total_vertex_list[random_idx])
    #print("end of constructing the testing source")
    #print("the source:[%s]" % str(test_source_list))

    return test_source_list

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
def get_map_key_time_interval(elapse):
    lower_bound = (int(elapse)/map_time_interval) * map_time_interval
    upper_bound = lower_bound + map_time_interval

    return str(lower_bound) + "-" + str(upper_bound)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
def get_map_key_vertex_num_interval(vertex_number):
    lower_bound = (vertex_number/map_vertex_num_interval) * map_vertex_num_interval
    upper_bound = lower_bound + map_vertex_num_interval

    return str(lower_bound) + "-" + str(upper_bound)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
def execute_one_test(step):
    # get testing vertices source
    source_vertices_list = get_random_test_source_vertices(test_num_each_step);
    actual_test_vertex_num = len(source_vertices_list)
    max_time = 0
    min_time = sys.maxint

    test_time_dist_map = {}
    test_vertex_num_dist_map = {}

    total_running_time = 0
    current_index = 0;
    for one_vertex in source_vertices_list:
        current_index = current_index + 1
        print("step[%s] process[%s/%s]" % (str(step), str(current_index), str(len(source_vertices_list))))
        
        url = query_statement_template + one_vertex
        #url+="%3Fdepth=1"
        payload = {"depth":step}
        head = {'content-type':'application/json'}
        req  = requests.get(url,params=payload)
        #req  = requests.get(url)
        #print req.url
        content = req.json()
        #print(query_statement)
        #data = urllib2.urlopen(query_statement)

        # start to get the real engine time from the json result
        #content = json.load(data)
#        print content
        results = content["results"]
        impacted_vertex_num = len(results["vertices"])
        total_engine_time = (req.elapsed.total_seconds()*1000)#results["engine_process_time"]
        # print engine_process_time_str
        # # get engine detailed iteration running time
        # # the format is "iteration 1:XX ms, iteration 2:XX ms"
        # iteration_time_str_list = re.findall(r'\:[0-9]*\ ', engine_process_time_str)
        # total_engine_time = 0
        # for one_time in iteration_time_str_list:
        #     total_engine_time += int(one_time[1:-1])

        # for the statistics of the test and create the key for the map
        time_dist_map_key = get_map_key_time_interval(total_engine_time)
        vertex_num_dist_map_key = get_map_key_vertex_num_interval(impacted_vertex_num)
        if time_dist_map_key in test_time_dist_map:
            test_time_dist_map[time_dist_map_key] += 1
        else:
            test_time_dist_map[time_dist_map_key] = 1
        if vertex_num_dist_map_key in test_vertex_num_dist_map:
            test_vertex_num_dist_map[vertex_num_dist_map_key] += 1
        else:
            test_vertex_num_dist_map[vertex_num_dist_map_key] = 1
        # end of get info

        # calculate the min, max & average time
        single_execution_time = total_engine_time
        if single_execution_time > max_time:
            max_time = single_execution_time
        if single_execution_time < min_time:
            min_time = single_execution_time
        total_running_time = total_running_time + single_execution_time

    averate_execution_time = float(total_running_time)/float(actual_test_vertex_num)

    # write the testing result back to the result file
    with open(test_result_file, "a") as resultFile:
        resultFile.write("# -  - - - - - - - - - - - - - - - - - -  - - - - - -\n")
        resultFile.write("# Test summary vertex num[%s] step[%s]\n" % (str(actual_test_vertex_num), str(step)))
        resultFile.write("# -  - - - - - - - - - - - - - - - - - -  - - - - - -\n")
        resultFile.write("\n")
        resultFile.write("min= %s ms max= %s ms avg= %s ms\n" \
                            % (str(min_time), str(max_time), str(averate_execution_time)))
        resultFile.write("\n")
        resultFile.write(" - - - - - - - - - - - - - - - - - - - - - - - - - - \n")
        resultFile.write("execution time distribution:\n")
        for w in sorted(test_time_dist_map, key=test_time_dist_map.get, reverse=True):
            resultFile.write("%s,%s\n" % (w, str(test_time_dist_map[w]))) 
        resultFile.write(" - - - - - - - - - - - - - - - - - - - - - - - - - - \n")
        resultFile.write("vertex number distribution:\n")
        for w in sorted(test_vertex_num_dist_map, key=test_vertex_num_dist_map.get, reverse=True):
            resultFile.write("%s,%s\n" % (w, str(test_vertex_num_dist_map[w]))) 
        resultFile.write("# -  - - - - - - - - - - - - - - - - - -  - - - - - -\n")
        resultFile.write("\n")

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# parse the command line parameters for the stes of the subgraph
def add_step_list(option, opt, value, parser):
    setattr(parser.values, option.dest, value.split(','))

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
def main():
    # provide command line option to specify
    # source_id file: from which to generate the random ID to query
    # step_list: a list of steps, each of which a sub_graph about to traverse
    # test_number: # of tests in one step
    global source_vertex_file_path
    global subgraph_steps_list
    global test_num_each_step
    global query_statement_template

    parser = OptionParser()
    parser.add_option("-f",
        dest="file_name",
        help="the vertex id file",
        metavar="FILE")
    parser.add_option("-s",
            dest="step_list", 
            action="callback",
            type='string',
            callback=add_step_list,
            help="a list of steps seperated by \',\'. Each is a test case.")
    parser.add_option("-n",
        dest="number_samples",
        type='int',
        help="Number of vertex samples in one test case")
    parser.add_option("-i",
            dest="ip_address", 
            type='string',
            help="[optional] the IP address of the graphsql server. By default, it's the localhost")
    (options, args) = parser.parse_args()
    if options.file_name:
        source_vertex_file_path = options.file_name
    else:
        parser.error('Vertex ID file not given. -h for help')
    if options.step_list:
        subgraph_steps_list = options.step_list
    else:
        parser.error('Step list not given. -h for help')
    if options.number_samples:
        test_num_each_step = options.number_samples
    else:
        parser.error('Number of samples not given. -h for help')

    if options.ip_address:
        server_ip_address = options.ip_address
    else:
        server_ip_address = "localhost"

    query_statement_template = "http://" + server_ip_address + ":9000/kneighborhood/"

    # if the result file exist then delete it
    if os.path.isfile(test_result_file):
        subprocess.call("rm " + test_result_file, shell=True)

    # main code starts from here
    print("start testing...")
    test_time_start = int(round(time.time() * 1000))
    for one_step in subgraph_steps_list:
        execute_one_test(one_step)
    test_time_end = int(round(time.time() * 1000))
    test_time = test_time_end - test_time_start
    print("testing is done!")
    print("total test time [%s]ms" % (test_time))
    print("Please check file: " + test_result_file)


if __name__ == "__main__":
    main()
