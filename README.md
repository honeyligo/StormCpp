# StormCpp
a sampe descript how to implement multilang of storm in C++. this code is updated from Sasa Petrovic's first contribution.
http://demeter.inf.ed.ac.uk/cross/stormcpp.html

# update
- fix exception errors when run C++ storm topology.
    - error "Anchored onto *  after ack/fail"
    - error "java.lang.InterruptedException at java.util.concurrent.locks.AbstractQueuedSynchronizer$ConditionObject.reportInterruptAfterWait(AbstractQueuedSynchronizer.java:2014)"

    
the cause is since the 0.9.3 storm version, we have to call the bolt synchronize method when we receive a heartbeat tuple.

```
    class Tuple
    {
        private:
            // ...
            int _id;
            std::string _stream;
        public:
            // ...
            bool is_heartbeat_tuple()
            {
                if (this->_id == -1 && this->_stream.compare("__heartbeat") == 0)
                    return true;
                return false;
            }
    }
    class Bolt
    {
        public:
            // ...
            void Run()
		    {
			    Mode = BOLT;
			    std::pair<Json::Value, Json::Value> conf_context = InitComponent();
			    Initialize(conf_context.first, conf_context.second);
			    while (1)
			    {
				    Tuple tuple = ReadTuple();
				    if (tuple.IsHeartbeatTuple())
				    {
					    Sync();
				    }
				    else
				    {
					    Process(tuple);
					    Ack(tuple.GetID());
				    }
			    }
		    }
    }
```

- generate trace infomation to files to check input and output for each module:
    - input： input of spout
    - msg: each message passed by pipe
    - split: each output of spout, also input of split
    - dynamic_result: result of count

