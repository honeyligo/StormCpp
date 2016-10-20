# StormCpp
a sampe descript how to implement multilang of storm in C++. this code is updated from Sasa Petrovic's first contribution.
http://demeter.inf.ed.ac.uk/cross/stormcpp.html

# update
- fix exception errors come out when run C++ storm topology.
    - error "Anchored onto *  after ack/fail"
    - error "java.lang.InterruptedException at java.util.concurrent.locks.AbstractQueuedSynchronizer$ConditionObject.reportInterruptAfterWait(AbstractQueuedSynchronizer.java:2014)"

    
the cause is since the 0.9.3 storm version, we have to call the bolt synchronize method when we receive a heartbeat tuple.
http://stackoverflow.com/questions/30072570/shellbolt-anchored-onto-after-ack-fail

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

- fix error come out when operator[] in jsoncpp called.
error infomation: ambiguous overload for ‘operator[]’ (operand types are ‘Json::Value’ and ‘int’).

Sasa Petrovic's code seems like:
```
class SplitSentence : public Bolt
{
	public:
		...
		void Process(Tuple &tuple)
		{
			std::string s = tuple.GetValues()[0].asString();
			...
		}
}
```
I guess he used a different version of jsoncpp, which doesn't contain operator[] overload members in class Value.
modify as bellow should solve the issue:
```
class SplitSentence : public Bolt
{
	public:
		...
		void Process(Tuple &tuple)
		{
			int i = 0;
			std::string s = tuple.GetValues()[i].asString();
			...
		}
}
```

- generate trace infomation to files to check input and output for each module:
    - input： input of spout
    - msg: each message passed by pipe
    - split: each output of spout, also input of split
    - dynamic_result: result of count

