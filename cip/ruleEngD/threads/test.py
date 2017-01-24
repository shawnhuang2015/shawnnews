import WorkerPool
import time 

if __name__ == "__main__":
    wp = WorkerPool.WorkerPool(3,2)
    worker = wp.acquireRuleSubmitter()
    worker2 = wp.acquireRuleSubmitter()
    worker3 = wp.acquireRuleSubmitter()
    worker4 = wp.acquireRuleSubmitter()
    worker.setJob("hello")
    worker2.setJob('worker2')
    worker2.run()
    worker.run()
    print 'kkkk'
    time.sleep(3)

