//============================================================================
// Name        : DQSS Basic tests
// Author      : Andrey
// Version     :
// Copyright   : 
//============================================================================


#include <boost/thread.hpp>
#include <gtest/gtest.h>

#include <DQSession.h>
#include <gutil/gtimer.hpp>

#include <WorkerScheduler.h>
#include <MasterScheduler.h>

using namespace std;
using namespace gdist;
typedef std::list<boost::thread *> tList_t;

const static string zkRoot = "/root";
const static string zkHosts = "127.0.0.1:13010";
const static int    zkTimeout = 600000;

const static string testDBSconfigName = "testConfig";
const static string connectionStr = "127.0.0.1:13011"; // "localhost:12345,localhost:9092";

class TestMaster  {

public:
	void operator()() {

		try {
			boost::posix_time::ptime tStart =
					boost::posix_time::microsec_clock::local_time();

			// per-udf invocation
			vector<string> listen2;
			IJobController* jm = master_.startUDFJob("test","foo job config", configName_ );
			IDistributedBucketSet* io = jm->getBucketSet();
			lastJobID_ = jm->getJobID();

			stringstream ss;

			// TODO: need to parameterize the job
			jm->executePhase("Init","bar1");
			int number = 100;
			for (int i = 0; i < 2; i++) {
				ss.str(std::string()); ss << "Map" << i;
				jm->executePhase(ss.str(),"bar1map");
				ss.str(std::string());
				ss << number;
				io->write("masterGV", "master's head",ss.str().size(), (unsigned char*) ss.str().c_str());
				ss.str(std::string()); ss << "Add" << i;
				jm->executePhase(ss.str(),"bar1add");
				vector<IMessage*> msgs = io->readBucket("localGV");
				number = 0;
				for (uint m=0; m< msgs.size(); m++) {
					number += atoi(reinterpret_cast<const char*>(msgs[0]->getValue()) );
				}
				LOG (INFO) << "ADDITION RESULT is " << number;
			}

			EXPECT_EQ(406, number);

			master_.endUDFJob(jm);
			// TODO: what if something went wrong? The job is done already, right?

			// end job

			boost::posix_time::ptime tEnd =
					boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration diff = tEnd - tStart;

			cout << "It took me " << (((float)diff.total_milliseconds())/1000)
	                    				   << " seconds to run udf test on master side." << endl;

		} catch (DQException &exc){
			std::cout << " Got Exception: "<< exc.what() << endl;
		}
	}

	TestMaster (string DQSSConnectionStr,  ClusterConfig* cConf, DBSConfig* dbsConf) :
				master_(
			 DQSSConnectionStr,  // Implementation dependent. Currently its Kafka broker list
			 zkHosts, // ZK server that the Daemon will connect to
			 zkRoot,  // namespace for this cluster in ZK -- has to be the same for the master and all workers -- must start with "/"
			 zkTimeout,   // (in msec) should be ~30000 in production, and 1000000 for debugging
			 cConf
	), lastJobID_ ("None"), config_(dbsConf) {
		configName_ = testDBSconfigName;
		master_.setDistributedBucketSetConfiguration(configName_, config_);
	}

	~TestMaster () {
		master_.stopDaemon();
		//while ( !this->stopped_ ) 	usleep(5000);
	}

	MasterScheduler master_;
	string lastJobID_;
	DBSConfig* config_;
	string configName_;
};

//-------------------------------- WORKER SIDE

/*
 *  Test Dummy Implementation of the runner interface
 */
class TestJobRunner : public IJobRunner {

public:
	TestJobRunner(string workerHostPort, DBSConfig* config) {
		workerHostPort_ = workerHostPort;
		config_ = config;
		isDone_ = false;
	};

	~TestJobRunner() {};

	// return a list of bucketID this job will read (all from cluster config for now)
	// FIXME: what's in JobConf now?
	JobConfig startJob(string jobID, string jobConfig) {
		LOG (INFO) << " Starting job " << jobID << " with job config " << jobConfig;
		JobConfig jc;

		for (list<DistributedBucketConfig*>::iterator iter = config_->buckets_.begin();
				iter != config_->buckets_.end() ; ++iter) {
			string bucketID = (*iter)->name_;
			// let's see if I need to read this one
			list<string> consumers = (*iter)->consumerIDs_;
			if (std::find(consumers.begin(), consumers.end(), workerHostPort_) != consumers.end()) {
				// yep, I need to listen to this bucket
				jc.bucketsToListenTo.push_back(bucketID);
			}
		}
		return jc;
	}

	void endJob(string jobID) { 		isDone_ = true; };

	void runJobPhase(string jobID, string phaseID, string config,
				IDistributedBucketSet* io) {

		LOG (INFO) << " Starting phase " << phaseID << " of job " << jobID << " with config " << config;

		//
		// Workers send 2 messages to first w2w bucket (so we test an empty bucket also), just in case
		//
		stringstream ss;
		string foobar = "foobar";
		for (list<DistributedBucketConfig*>::iterator iter = config_->buckets_.begin();
				iter != config_->buckets_.end(); ++iter) {
			if ( (*iter)->isW2W()) {
				string bucketID = (*iter)->name_;
				for (uint m=0; m < 2; m++) {
					ss.str(std::string());
					ss << "b_" << bucketID << "_msg_" << m;
					unsigned char* msg = (unsigned char*) (string(foobar)).c_str();
					io->write(bucketID, ss.str(), foobar.length(), msg);
				}
				break;
			}
		}


		// If this is not Init phase, receive all w2w buckets
		if ( phaseID.substr(0,4).compare("Init") !=0) {
			for (list<DistributedBucketConfig*>::iterator iter = config_->buckets_.begin();
					iter != config_->buckets_.end(); ++iter) {
				if ( (*iter)->isW2W()) {
					string bucketID = (*iter)->name_;
					vector<IMessage*> msgs = io->readBucket(bucketID);
					LOG (INFO) << "Worker at " << workerHostPort_ << " got " <<  msgs.size() << " messages from bucket " << bucketID;
					io->deleteBucket(bucketID);
				}
			}
		}

		//
		// If this is an Add phase, let's read masterGV and reply on localGV
		//
		if ( phaseID.substr(0,3).compare("Add") ==0) {
			vector<IMessage*> msgs = io->readBucket("masterGV");
			LOG (INFO) << "Adder at " << workerHostPort_ << " got " <<  msgs.size() << " messages from the master";
			if (msgs.size() == 1) {
				int num = atoi(reinterpret_cast<const char*>(msgs[0]->getValue()) ) + 1;
				LOG (INFO) << "Adder at " << workerHostPort_ << " will write back" << num;
				stringstream ss;
				ss << num;
				io->write("localGV", "some head",ss.str().size(), (unsigned char*) ss.str().c_str());
			}
		}
		io->deleteBucket("masterGV");
		io->close();
	}

	void processBucketMessage(string jobID, string phaseID, int bucketID, DQSSMessage* msg) {
		// NOOP for now.
	}

	bool isDone() { return isDone_;};

private:
	string workerHostPort_;  // who am I
	DBSConfig* config_;   // how are the cluster buckets configured?
	bool isDone_;
};

class TestWorker  {
public:

	void operator()() {
		boost::posix_time::ptime tStart =
				boost::posix_time::microsec_clock::local_time();

		while ( !runner_.isDone() ) {
			sleep(1);
		}

		boost::posix_time::ptime tEnd =
				boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration diff = tEnd - tStart;

		cout << "It took me " << (((float)diff.total_milliseconds())/1000)
		                    						   << " seconds to run udf test on worker side." << endl;

	}

	TestWorker (string DQSSConnectionStr,  string hostportStr, DBSConfig* config) :
		runner_( hostportStr, config ),
		scheduler_(&runner_,
			 DQSSConnectionStr,  // Implementation dependent. Currently its Kafka broker list
			 zkHosts, // ZK server that the Daemon will connect to
			 zkRoot,  // namespace for this cluster in ZK -- has to be the same for the master and all workers -- must start with "/"
			 zkTimeout,   // (in msec) should be ~30000 in production, and 1000000 for debugging
			 hostportStr  // unique ID for this process, should be the same after cluster restart
		)	{
		// the scheduler runs it's own threads for receiving data
	}

	~TestWorker () {
		scheduler_.stopDaemon();
		//while ( !scheduler_.stopped_ )	usleep(5000);

		//delete scheduler_;
		//delete runner_;
	}

	TestJobRunner runner_;
	WorkerScheduler scheduler_;

};


#ifdef KAFKATEST
TEST(dist,noop) {
	int WW = 2;
	cout << " Hello world!" << endl;

	// 1. Setup master and worker hostport ID's
	string masterID = "masterrr";
	list<string> workerIDs;
	stringstream ss;
	for (int i=0; i < WW; i++) {
		ss.str(std::string());
		ss << "worker_" << i;
		workerIDs.push_back(ss.str());
	}

	ClusterConfig cConf(masterID,workerIDs);  // create cluster setup object
	DBSConfig config(&cConf ); // configuration that encodes (one for now) partitioning of graph topology in the cluster
	config.subjectID_ = testDBSconfigName;

	// 2. setup Global Var buckets
	config.newM2Wbucket( "masterGV" );
	config.newW2Mbucket( "localGV" );

	// 3. setup Worker-2-Worker Distributed buckets
	//    for now, let's have two buckets and every worker will consume both
	for (int b=0; b<2; b++) {
		ss.str(std::string());
		ss << "w2wBucket" << b;
		string bname = ss.str();
		config.newW2Wbucket(bname, workerIDs);
	}

	// clean up ZK state once -- the second run loop has to start with "dirty" ZK
	ZooKeeperContext ctx;
	ctx.connect(zkHosts,zkTimeout);
	ctx.recursiveDelete(zkRoot);

	// we'll do test twice, to make sure everything gets cleaned up
	for (int loop=0; loop < 2; loop++) {
		tList_t   workingThreads_;
		boost::thread *t;

		// wait a little, just in case I make recursiveDelete() async one of these days
		usleep(100000);

		TestWorker* tw;
		list<TestWorker*> workers;

		for (list<string>::iterator iter = workerIDs.begin(); iter != workerIDs.end(); ++iter) {
			tw = new TestWorker(connectionStr, (*iter),  &config );
			workers.push_back(tw);
			t = new boost::thread(boost::ref(*tw));
			workingThreads_.push_back( t );
		}

		// wait a little -- let workers wake up find the tail of the queue
		usleep(100000);

		vector<string> masterWrites;
		vector<string> masterReads;
		TestMaster* tm = new TestMaster(connectionStr,  &cConf, &config);
		t = new boost::thread(boost::ref(*tm));
		workingThreads_.push_back( t );

		for (tList_t::iterator it = workingThreads_.begin(); it!= workingThreads_.end(); ++it) {
			(*it)->join();
		}
		string testJobID = tm->lastJobID_;
		LOG(INFO) << "Test Master finished job " << testJobID;
		delete tm;

		// master thread is done, so the job is done - let's kill worker daemons
		for (list<TestWorker*>::iterator iter = workers.begin();
				iter != workers.end(); ++iter) {
			(*iter)->scheduler_.testPrint();
			EXPECT_EQ(0, (*iter)->scheduler_.getJobRC(testJobID) );
			delete (*iter);
		}
	}

}
#endif
