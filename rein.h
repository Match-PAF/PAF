#ifndef _REIN_H
#define _REIN_H
#define _USE_MATH_DEFINES
#include <vector>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <assert.h>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "constant.h"
#include "data_structure.h"

const int MAX_BUCKS = 2000;
//const int MAX_CONS = 11;   // the maximal number of constraints
//int window_event;     //Number of events in a time window

class Rein {
	int valDom, Dom, subnum;                                    //  valDom: Cardinality of values.    what are the Dom and subnum?
	int subSetSize;
	vector<kCombo> data[newlevel][attDom][2][MAX_BUCKS];      // 0:left parenthesis, 1:right parenthesis
	vector<int> LvSize[newlevel], emptymark[newlevel];                 //MAX_CONS=newlevel

	int originlevel[subs];                                       //MAX_SUBS determines the  number of dimensions
	int LvBuck[newlevel];                                           //what's the LvBuck?
	int LvBuckStep[newlevel];                                       //LvBuckStep?
	int subtolevel[subs];                                       //sub.id => level
	vector<changeaction> changelist[newlevel];                      //�Ƕ�ά���������ֶ��巽ʽ��ô��⡣max_cons ��ʾ����Ԫ�ظ�����
	bool firstchange;
	double adjustcost;
	double limitscale;

public:
	int window_number;
	int LvSubNum[newlevel];        // ÿ���ж��ٸ�����
	vector<vector<int>> countlist; // 100���У�һ�м�¼һ������
	Rein(int _valDom, int k, int buck_num, double limitscale,int subsetsize) :valDom(_valDom), Dom(k), limitscale(limitscale)
	{
		subSetSize = subsetsize;
		subnum = 0;
		buck_num /= newlevel;                                                                              //ÿһ��Ͱ�ĸ���
		assert(buck_num <= MAX_BUCKS);                                                                      //assert ������ô�ã�                           
		for (int i = 0; i < newlevel; ++i)
		{
			/* LvBuck[i] = (int)floor(buck_num * sqrt(i+1.0) / newlevel) + 1;
			 LvBuckStep[i] = (valDom - 1) / LvBuck[i] + 1;
			 LvBuck[i] = (valDom - 1) / LvBuckStep[i] + 1;
			 assert(LvBuck[i]<MAX_BUCKS);*/
			LvBuckStep[i] = (valDom - 1) / buck_num + 1;   //buckstep ��levelɶ��ϵ       
			LvBuck[i] = (valDom - 1) / LvBuckStep[i] + 1;
		}

		//memset(countlist, 0, sizeof(countlist));                                                                              //memset�������ã�
		firstchange = true;
		window_number = 1;
		countlist.push_back(vector<int>(subSetSize));
		memset(LvSubNum, 0, sizeof(int) * newlevel);
	}
	//void insert(Sub &sub);
	void insert(IntervalSub& sub, int64_t& origintime);
	void match(const Pub& pub, int& matchSubs, vector<double>& matchDetailPub);
	int change(const vector<IntervalSub>& subList, int cstep, double matchingtime, string& windowcontent, vector<int>& changeSub); //return numbers of subs changed by function
	void check();
};

#endif
