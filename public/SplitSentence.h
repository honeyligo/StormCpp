/*
Author: Sasa Petrovic (montyphyton@gmail.com)
Copyright (c) 2012, University of Edinburgh
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SPLIT_SENTENCE_H
#define SPLIT_SENTENCE_H

#include <string>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <unistd.h>
#include <unordered_map>

#include "Storm.h"
#include "json/json.h"

using namespace std;

namespace storm
{

size_t split(const std::string &src, const std::string &delim, std::vector<std::string> &dst)
{
	dst.clear();
	size_t idx = 0;
	size_t pos = src.find(delim, idx);
	while (pos != std::string::npos)
	{
		dst.push_back(src.substr(idx, pos - idx));
		idx = pos + delim.length();
		pos = src.find(delim, idx);
	}

	dst.push_back(src.substr(idx));
	return dst.size();
}

class CountWord : public Bolt
{
public:
	CountWord()
	{
		os_.open("/usr/local/storm/bin/dynamic_result", std::ios_base::out | std::ios_base::app);
	}

	~CountWord()
	{
		os_.close();
	}

	void Initialize(Json::Value conf, Json::Value context) { }
	void Process(Tuple &tuple)
	{
		int i = 0;
		std::string s = tuple.GetValues()[i].asString();
		kCount_[s]++;
		os_ << s << ":" << kCount_[s] << std::endl;
	}

private:
	std::ofstream os_;
	std::unordered_map<std::string, int> kCount_;
};

class SplitSentence : public Bolt
{
	public:
		void Initialize(Json::Value conf, Json::Value context) { }
		void Process(Tuple &tuple)
		{
			int i = 0;
			std::string s = tuple.GetValues()[i].asString();

			std::ofstream os;
			os.open("/usr/local/storm/bin/split", std::ios_base::out | std::ios_base::app);
			os << s << std::endl;
			os.close();

			std::vector<std::string> tokens;
			split(s, " ", tokens);
			for (int i = 0; i < tokens.size(); ++i)
			{
				Json::Value j_token;
				j_token.append(tokens[i]);
				Tuple t(j_token);
				Emit(t);
			}
		}
};

class SpoutSentence : public Spout
{
public:
	SpoutSentence()
	{
		len_ = 0;
		sentences_.push_back("the cow jumped over the moon");
		sentences_.push_back("an apple a day keeps the doctor away");
		sentences_.push_back("four score and seven years ago");
		sentences_.push_back("snow white and the seven dwarfs");
		sentences_.push_back("i am at two with nature");
	};

	virtual void Initialize(Json::Value conf, Json::Value context)
	{
	};
	// Read the next tuple and write it to stdout.
	virtual void NextTuple()
	{
		if (len_ >= sentences_.size())
		{
			sleep(10);
		}
		else
		{
			std::string s = sentences_[len_++];
			std::ofstream os;
			os.open("/usr/local/storm/bin/input", std::ios_base::out | std::ios_base::app);
			os << s << std::endl;
			os.close();

			Json::Value root;
			root.append(s);
			Tuple t(root);

			Emit(t);
			Log(s);
		}
	}

private:
	std::string guid() 
	{
		std::string uuid;
		std::ifstream fin("/proc/sys/kernel/random/uuid");
		std::getline(fin, uuid);
		fin.close();
		return uuid;
	}

private:
	vector<string> sentences_;
	int len_;
};

}

#endif
