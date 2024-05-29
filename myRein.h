//#pragma once
#ifndef MY_REIN_H
#define MY_REIN_H
#define _USE_MATH_DEFINES
#include <vector>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <assert.h>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "data_structure.h"
#include<omp.h>
#include "ARIMAModel.h"
#include "constant.h"
#include "lstm.h"
#include <thread>
#include<unordered_set>
#include<unordered_map>

const int MY_MAX_BUCKS = 2000; // 301;
//const int MY_MAX_CONS = 11;   // the maximal number of constraints
//int window_event;           // Number of events in a time window

class myRein {
	int valDom, Dom, subnum;                                           //  valDom: Cardinality of values. Dom 订阅谓词个数
	vector<kCombo> data[newlevel][attDom][2][MY_MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis 通俗的理解就是区间的左边就是低值
	vector<int> LvSize[newlevel], emptymark[newlevel];           //MAX_CONS=newlevel emptymark[newlevel] 这个数组干啥用？

	int originlevel[subs];                                      //MAX_SUBS determines the  number of dimensions
	int LvBuck[newlevel];                                          
	int LvBuckStep[newlevel];                                     
	int subtolevel[subs];                                       //sub.id => level
	vector<changeaction> changelist[newlevel];                      
	bool firstchange;
	double adjustcost;
	double limitscale;
	//vector<vector<int>> pre_countlist; //二维数组
	
public:
	int window_number;
	vector<vector<int>> countlist;
	vector<vector<int>> checkLevel;
	int mapTable[newlevel][cstep+1];  // 保存每个匹配个数对应的层  //cstep:  adjustment time window
	int LvSubNum[newlevel];    // 每层有多少个订阅
	int* state; // 0表示没有预测状态(初始状态)，1表示预测中，2表示预测完毕等待调整的状态
	int* historyWindowNumberWhenPredict; // 开始预测时的窗口个数，与预测完的窗口个数比较
	unordered_set<int> lastChangedSub;
	unordered_map<int, bool> lastChangeDirection; // false 表示层数变大, true表示层数变小

	myRein(int _valDom, int k, int buck_num, double limitscale) :valDom(_valDom), Dom(k), limitscale(limitscale)
	{
		subnum = 0;                        // 当前所有层一共存了多少个订阅
		buck_num /= newlevel;              //每一层桶的个数
		assert(buck_num <= MY_MAX_BUCKS);   //assert 函数怎么用？                           
		for (int i = 0; i < newlevel; ++i)
		{
			/* LvBuck[i] = (int)floor(buck_num * sqrt(i+1.0) / newlevel) + 1;
			 LvBuckStep[i] = (valDom - 1) / LvBuck[i] + 1;
			 LvBuck[i] = (valDom - 1) / LvBuckStep[i] + 1;
			 assert(LvBuck[i]<MAX_BUCKS);*/
			LvBuckStep[i] = (valDom - 1) / buck_num + 1;    //  小的层订阅比较多桶数可以稍微多一点
			LvBuck[i] = (valDom - 1) / LvBuckStep[i] + 1;
			//cout << "第" << i << "层，桶宽LvBuckStep[" << i << "]=" << LvBuckStep[i] << ", 桶个数LvBuck[" << i << "]=" << LvBuck[i] << endl;
		}

		countlist.resize(subs);
		//pre_countlist.resize(subs);
		for (int i = 0; i < subs; ++i) {
			//pre_countlist[i].resize(stepNum);
			countlist[i].push_back(0);
		}
		checkLevel.resize(subs);
		//pre_countlist.resize(subs);
		for (int i = 0; i < subs; ++i) {
			//pre_countlist[i].resize(stepNum);
			checkLevel[i].push_back(-1);
			checkLevel[i].push_back(-1);// 第一个时间窗口结束时每个订阅所在的层号
		}

		//memset(countlist, 0, sizeof(countlist));                      //memset函数作用？
		firstchange = true;
		window_number = 1;
		adjustcost = 0;
		state = new int; //给指针变量分配内存，
		historyWindowNumberWhenPredict = new int;
		*state = 0;  //使用指针。
		*historyWindowNumberWhenPredict = 0;

		// 初始化mapTable
		for (int i = 0; i < newlevel; i++) {
			for (int j = 0; j <=cstep; j++) {
				mapTable[i][j] = (int)round(pow(1 - (double)j / cstep, 2) * i);
			}
		}
		memset(LvSubNum, 0, sizeof(int) * newlevel);
	}
	//void insert(Sub &sub);
	void insert(IntervalSub& sub, int64_t& origintime);
	void match(const Pub& pub, int& matchSubs, vector<double>& matchDetailPub);
	int coldChange(const vector<IntervalSub>& subList, vector<vector<int>>& changedSub, double matchingtime); 
	int change(const vector<IntervalSub>& subList,const vector<vector<int>>* predict_countlist, vector<vector<int>>& changedSub, int cstep, double matchingtime); //return numbers of subs changed by function
	void check();

};

vector<int> predict_arima(vector<double> countlist, int stepNum);
void predict(void* pd); // 其实也可以直接传this，但会多传入一些不需要用到的数据， 为什么会传入一些不需要的数据。用void * 的好处是？

#endif
