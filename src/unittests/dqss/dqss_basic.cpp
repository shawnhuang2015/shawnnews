//============================================================================
// Name        : DQSS Basic tests
// Author      : Andrey
// Version     :
// Copyright   : 
//============================================================================

//#include <iostream>
//#include <string>


#include <boost/thread.hpp>
#include <gtest/gtest.h>

#include <DQSession.h>
#include <gutil/gtimer.hpp>

using namespace std;
using namespace gdist;
typedef std::list<boost::thread *> tList_t;

/*
 * Not thread safe. simple collector for testing. only one test client will read from it.
 */
class DQSSTestCollector : public IDQSessionCollector {
public:
	DQSSTestCollector() {
		currentSession_ = NULL;
		currentMsgIdx_ = 0;
	}
	virtual ~DQSSTestCollector() {
		for ( list<IDQSession4Consumer*>::iterator iter = trash_.begin();
				iter != trash_.end(); ++iter) {
			delete (*iter);
		}
		trash_.clear();

	};

	virtual int collect( IDQSession4Consumer* session) {
		outQ_.push(session);
		return 0;
	}

	/*
	 * Reads up to N messages from the Ready Queue
	 * with block == false, I'll sleep once before giving up
	 */
	virtual std::vector<DQSSMessage*> readN(int N, bool block) {
		std::vector<DQSSMessage*> messages;
		do {
			bool empty = !( nextMessage()  );
			if (empty) {
				usleep(10000);
				empty = !( nextMessage()  );
			}
			if (!empty) {
				// we have some messages in memory, let's read all of them... up to N
				for (int i=0; i<N; i++) {
					// navigate to the next message
					if (!nextMessage()) break;
					messages.push_back( currentSession_->readAt(currentMsgIdx_++));
				}
				return messages;
			}
		} while(block);

		return messages;
	}

private:
	// navigate to the next message
	bool nextMessage() {
		while ( nextSession() ) {
			if ( currentMsgIdx_ < currentSession_->getBufferSize() )
				return true;
			else {
				trash_.push_back( currentSession_);
				currentSession_ = NULL;
			}
		}
		return false;
	}

	// navigate to the next session (may have no messages)
	bool nextSession() {
		if (currentSession_ == NULL) {
			if (outQ_.empty()) return false;
			currentSession_ = outQ_.front();
			outQ_.pop();
		}
		return true;
	}

	IDQSession4Consumer* currentSession_;

	int currentMsgIdx_;

	thread_safe::queue<IDQSession4Consumer*> outQ_; // a queue of messages that are ready to be consumed
	list<IDQSession4Consumer*> trash_; // old sessions to be deleted in the end of the test
};


class DQSSTestProducer : public DQSessionProducer {

public:
	   void operator()() {

	      try {

	         boost::posix_time::ptime tStart =
	               boost::posix_time::microsec_clock::local_time();

	         long head = this->getTimedOffset(partID_,-1);
	         long tail = this->getTimedOffset(partID_,-2);

	         if (!regen_ && (head - tail >= Nmsg_)) {
	            // we are done -- we already have enough messages
	            return;
	         }

	         IDQSession4Producer* session = this->openSession("test1");

	         stringstream ss;
	         string value;
	         int size;
	         for (int i=0; i < Nmsg_; i++) {

	            ss << "key" << i;
	            string key = ss.str();
	            ss.str(std::string());

	            if (true) {
	               size = pow(10.0, scale_) * (1 + i*increase_);
	               unsigned char* buffer = new unsigned char[size];
	               for (int c=0; c < size; c++ ) {
	                  buffer[c] = (char) (48 + (i%10));
	               }
	               uLong crc = crc32(0L, buffer, size);
	               ss << crc;
	               string key = ss.str();
	               ss.str(std::string());
	               this->write(session, key, size, buffer);
	               // note, write() will delete the buffer

	            } else {
	            	// small test messages, disabled for now
	               ss << "value" << i;
	               value = ss.str();
	               ss.str(std::string());
	               this->write(session, key, value);
	               size = value.size();
	            }


	            if ( delay_ > 0) {
	               cout << "Produced " << i << "\t key: " << key << " valueLen: " << size << " at time: " << gutil::GTimer::now_str() << endl;
	               usleep(delay_ * 1000);
	            }

	         }

	         gotAcks_ = this->closeSession(session);
	         EXPECT_EQ(this->nConsumers_, gotAcks_);

	         boost::posix_time::ptime tEnd =
	               boost::posix_time::microsec_clock::local_time();
	         boost::posix_time::time_duration diff = tEnd - tStart;

	         cout << "It took me " << (((float)diff.total_milliseconds())/1000)
	                       << " seconds to send " << Nmsg_ << " messages" << endl;

	      } catch (DQException &exc){
	         std::cout<<"KProducer on part " << partID_ << " Got Exception: "<< exc.what()<<std::endl;
	      }
	   }

   DQSSTestProducer (string connectionStr, string clientID, string topic,
		   int Nmsg, bool regen, int sizeScale, int sizeIncrease,
		   int partID=0, int nConsumers=1, int delay=0)
   : DQSessionProducer(connectionStr, clientID, topic, partID, nConsumers),
     Nmsg_(Nmsg), regen_(regen), scale_(sizeScale), increase_(sizeIncrease), delay_(delay), gotAcks_(0)
   {
   }

   int Nmsg_;
   bool regen_;  // produce messages even if there are enough messages in the topic already
   int scale_;   // message size is 10^scale_
   int increase_; // multiplies each message by (1 + incr_ * i)
   int delay_;   // delay (in msec) between messages
   int gotAcks_;

};


class DQSSTestConsumer  {

public:
	void operator()() {
		try{

			int emptyCnt = 0;

			boost::posix_time::ptime tStart =
					boost::posix_time::microsec_clock::local_time();
			//consumer_.maxBytes_ = 1024000;
			//         this->delay_ = 1000;
			//         this->offset_ = offset;

			std::vector<DQSSMessage *> ms;
			ms = collector_.readN(1,true);  // block for the first msg
			int cnt=1;
			for (int i=0; emptyCnt<10; i++) {
				ms = collector_.readN(10,false);
				if (ms.size() <= 0)
					emptyCnt ++;
				else {
					cnt += ms.size();
					emptyCnt = 0;
				}

				cout << "Consumed up to " << cnt << " at time: " << gutil::GTimer::now_str() << endl;
				for ( vector<DQSSMessage *>::iterator iter = ms.begin();
						iter != ms.end(); ++iter) {
					DQSSMessage* msg = (*iter);
					//cout << "last msg key: " << msg->key << " time: " << msg->timestamp_ << endl;
					uLong expectedCRC = atol(msg->getHeader().c_str());
					uLong crc = crc32(0L, msg->getValue(), msg->getValueLength());
					EXPECT_EQ(expectedCRC, crc);
				}
			}

			consumer_.stop();

			boost::posix_time::ptime tEnd =
					boost::posix_time::microsec_clock::local_time();
			boost::posix_time::time_duration diff = tEnd - tStart;

			cout << "It took me " << (((float)diff.total_milliseconds())/1000)
					        		  << " seconds to receive "<< cnt << " messages" << endl;
			EXPECT_EQ(expectMsgs_, cnt);
		}catch(DQException &exc){
			std::cout<<"Got Exception: "<< exc.what()<<std::endl;
		}
	}



	DQSSTestConsumer (string connectionStr, string clientID, string topic, int expectMsgs, int nProducers=1, int delay=500, int partID=0) :
		collector_( ),
		consumer_(&collector_, connectionStr, clientID, topic, partID, nProducers, delay),
		expectMsgs_(expectMsgs)
	{
		t = new boost::thread (boost::ref(consumer_));
	}

	~DQSSTestConsumer() {
		consumer_.stop();
		t->join();
		delete t;
	}

protected:
	DQSSTestCollector collector_;
	DQSessionConsumer consumer_;
	int expectMsgs_;
	boost::thread *t;

};

const static string connectionStr = "127.0.0.1:13011"; // "localhost:12345,localhost:9092";
const static string test_topic = "dqss";

#ifdef KAFKATEST
TEST(dqss,pub3to1) {
	int NMSG =10;
	int PP = 3;
	int CC = 2;
	cout << PP << " producer(s) send 10 messages, each, to " << CC << " consumer(s)." << endl;

	tList_t   workingThreads_;
	boost::thread *t;

	//vector<string> foo;	foo.push_back( connectionStr);

	stringstream ss;

	DQSSTestConsumer* kc;
	list<DQSSTestConsumer*> consumers;
	for (int c=0; c < CC; c++) {
		ss.str(std::string());
        ss << "con" << c;
		kc = new DQSSTestConsumer(connectionStr, ss.str(), test_topic, NMSG*PP, PP );
		consumers.push_back(kc);
		t = new boost::thread(boost::ref(*kc));
		workingThreads_.push_back( t );
	}

	// wait a little -- let consumer find the tail of the queue
	usleep(100000);

	DQSSTestProducer* kp;
	list<DQSSTestProducer*> producers;

	for (int p=0; p < PP; p++) {
		ss.str(std::string());
        ss << "pro" << p;
		kp = new DQSSTestProducer(connectionStr, ss.str(), test_topic,NMSG, true, 3, 0, 0, CC);
		producers.push_back(kp);
		t = new boost::thread(boost::ref(*kp));
		workingThreads_.push_back( t );
	}

	for (tList_t::iterator it = workingThreads_.begin(); it!= workingThreads_.end(); ++it) {
		(*it)->join();
	}

	// destroy TestConsumers
	for (list<DQSSTestConsumer*>::iterator iter = consumers.begin();
			iter != consumers.end(); iter++) {
		delete (*iter);
	}
	consumers.clear();

	for (list<DQSSTestProducer*>::iterator iter = producers.begin();
			iter != producers.end(); iter++) {
		EXPECT_EQ(CC, (*iter)->gotAcks_);
		delete (*iter);
	}
	producers.clear();

	sleep(1);
}


TEST(dqss,pub1to1_large_msg) {
	int NMSG = 9;
	int PP = 1;
	int CC = 1;
	cout << PP << " producer(s) send " << NMSG <<
			" messages, each, to " << CC << " consumer(s)." << endl;

	tList_t   workingThreads_;
	boost::thread *t;

	//vector<string> foo;	foo.push_back( connectionStr);

	stringstream ss;

	DQSSTestConsumer* kc;
	list<DQSSTestConsumer*> consumers;
	for (int c=0; c < CC; c++) {
		ss.str(std::string());
        ss << "con" << c;
		kc = new DQSSTestConsumer(connectionStr, ss.str(), test_topic, NMSG, PP );
		consumers.push_back(kc);
		t = new boost::thread(boost::ref(*kc));
		workingThreads_.push_back( t );
	}

	// wait a little -- let consumer find the tail of the queue
	usleep(100000);

	DQSSTestProducer* kp;
	list<DQSSTestProducer*> producers;
	for (int p=0; p < PP; p++) {
		ss.str(std::string());
        ss << "pro" << p;
		kp = new DQSSTestProducer(connectionStr, ss.str(), test_topic,NMSG, true, 6, 1, 0, CC);
		producers.push_back(kp);
		t = new boost::thread(boost::ref(*kp));
		workingThreads_.push_back( t );
	}

	for (tList_t::iterator it = workingThreads_.begin(); it!= workingThreads_.end(); ++it) {
		(*it)->join();
	}

	// destroy TestConsumers
	for (list<DQSSTestConsumer*>::iterator iter = consumers.begin();
			iter != consumers.end(); iter++) {
		delete (*iter);
	}
	consumers.clear();

	for (list<DQSSTestProducer*>::iterator iter = producers.begin();
			iter != producers.end(); iter++) {
		EXPECT_EQ(CC, (*iter)->gotAcks_);
		delete (*iter);
	}
	producers.clear();

	sleep(1);
}
#endif

//------------------------- DQ test ----------------


using namespace gdist;
using namespace std;
typedef std::list<boost::thread *> tList_t;




class KProducer : public DQProducer {

public:
   void operator()() {

      boost::posix_time::ptime tStart =
            boost::posix_time::microsec_clock::local_time();

      stringstream ss;
      for (int i =1; i<=Nmsg_; i++) {

         ss << "key" << i;
         string key = ss.str();
         ss.str(std::string());
         //tMsg = clock();
         ss << "value" << i;
         string value = ss.str();
         ss.str(std::string());
         this->write(key,value);
      }

      this->flush();

      boost::posix_time::ptime tEnd =
            boost::posix_time::microsec_clock::local_time();
      boost::posix_time::time_duration diff = tEnd - tStart;

      cout << "It took me " << (((float)diff.total_milliseconds())/1000)
				      << " seconds to send " << Nmsg_ << " messages" << endl;

   }

   /*
    *
    */
   KProducer (string connectionStr, string topic, string clientID, int Nmsg, bool optionalMessages, int acks, int partID=0,
		   int maxBufferMsg = 1, int maxBufferBytes = 1024000)
   : DQProducer(connectionStr, clientID, topic, acks, partID, maxBufferMsg, maxBufferBytes) {

	   Nmsg_ = Nmsg;
	   if (optionalMessages) {
	     long head = this->getTimedOffset(partID_,-1);
	      long tail = this->getTimedOffset(partID_,-2);

	      if (head - tail >= Nmsg) {
	         // we are done -- we already have enough messages
	    	  Nmsg_ = 0;
	      }
	   }
   }

   int Nmsg_;

};


class KConsumer : public DQConsumer<> {

public:
	void operator()() {



		boost::posix_time::ptime tStart =
						boost::posix_time::microsec_clock::local_time();
// 		long offset = 0;
//		offset = this->getHeadOffset();
//		this->offset_ = offset;

		this->maxBytes_ = 1024;
		this->setDelay(200);

		vector<DQMessage*> ms;
		int emptyCnt=0;

        while (emptyCnt < 5) {
           ms.clear();
           read(ms);
           if (ms.size() <= 0)
              emptyCnt ++;
           else {
              cnt_ += ms.size();
              emptyCnt = 0;
           }

           //cout << "Consumed up to " << cnt_ << " at time: " << gutil::GTimer::now_str() << endl;
           if ( ms.size() > 0) {
              DQMessage* lastMsg = ms[ms.size()-1];
              LOG (INFO) << "last msg key: " << lastMsg->key << " time: " << lastMsg->timestamp_ << endl;
           }
        }

        boost::posix_time::ptime tEnd =
              boost::posix_time::microsec_clock::local_time();
        boost::posix_time::time_duration diff = tEnd - tStart;

        LOG (INFO) << "It took me " << (((float)diff.total_milliseconds())/1000)
				          << " seconds to receive "<< cnt_ << " messages" << endl;

	}

	//
	void recieve(int offset, vector<DQMessage*> &messages) {
		offset_ = offset;
		return read(messages);

	}

	KConsumer (string connectionStr,string topic, string clientID) :
		DQConsumer<>(connectionStr, clientID, topic) {
		cnt_ = 0;
	}

  int cnt_;
};

#ifdef KAFKATEST
TEST(dqss,dq_two_threads) {
	cout << "Sync Producer test" << endl;

	string topic = "dqss";
	int nmsgs = 20;

	tList_t   workingThreads_;
	boost::thread *t;

	KConsumer* kc = new KConsumer(connectionStr,topic, "con");
	t = new boost::thread(boost::ref(*kc));
	workingThreads_.push_back( t );

	// wait a little -- let consumer find the tail of the queue
	usleep(100000);

	KProducer* kp = new KProducer(connectionStr,topic,"pro", nmsgs, false, 1);  // default (1 ack, 1 msg at a time) case
	t = new boost::thread(boost::ref(*kp));
	workingThreads_.push_back( t );

	for (tList_t::iterator it = workingThreads_.begin(); it!= workingThreads_.end(); ++it) {
		(*it)->join();
	}

	EXPECT_EQ(nmsgs, kc->cnt_);

	delete kc;
	delete kp;

}

TEST(dqss,dq_async) {
	cout << "ASYNC Producer test" << endl;

	string topic = "dqss";
	int nmsgs = 101;

	tList_t   workingThreads_;
	boost::thread *t;

	KConsumer* kc = new KConsumer(connectionStr,topic, "con");
	t = new boost::thread(boost::ref(*kc));
	workingThreads_.push_back( t );

	// wait a little -- let consumer find the tail of the queue
	usleep(100000);

	// async producer, buffers 20 msgs (and 1000KB)
	KProducer* kp1 = new KProducer(connectionStr,topic,"pro1", nmsgs, false, 0, 0, 20);
	t = new boost::thread(boost::ref(*kp1));
	workingThreads_.push_back( t );

	// async producer, buffers 1000 msgs (but only 100B)
	KProducer* kp2 = new KProducer(connectionStr,topic,"pro2", nmsgs, false, 0, 0, 1000, 256);
		t = new boost::thread(boost::ref(*kp2));
		workingThreads_.push_back( t );

	for (tList_t::iterator it = workingThreads_.begin(); it!= workingThreads_.end(); ++it) {
		(*it)->join();
	}

	EXPECT_EQ(nmsgs*2, kc->cnt_);

	delete kc;
	delete kp1;
	delete kp2;

}
#endif
