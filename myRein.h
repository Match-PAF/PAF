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
	int valDom, Dom, subnum;                                           //  valDom: Cardinality of values. Dom ����ν�ʸ���
	vector<kCombo> data[newlevel][attDom][2][MY_MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis ͨ�׵��������������߾��ǵ�ֵ
	vector<int> LvSize[newlevel], emptymark[newlevel];           //MAX_CONS=newlevel emptymark[newlevel] ��������ɶ�ã�

	int originlevel[subs];                                      //MAX_SUBS determines the  number of dimensions
	int LvBuck[newlevel];                                          
	int LvBuckStep[newlevel];                                     
	int subtolevel[subs];                                       //sub.id => level
	vector<changeaction> changelist[newlevel];                      
	bool firstchange;
	double adjustcost;
	double limitscale;
	//vector<vector<int>> pre_countlist; //��ά����
	
public:
	int window_number;
	vector<vector<int>> countlist;
	vector<vector<int>> checkLevel;
	int mapTable[newlevel][cstep+1];  // ����ÿ��ƥ�������Ӧ�Ĳ�  //cstep:  adjustment time window
	int LvSubNum[newlevel];    // ÿ���ж��ٸ�����
	int* state; // 0��ʾû��Ԥ��״̬(��ʼ״̬)��1��ʾԤ���У�2��ʾԤ����ϵȴ�������״̬
	int* historyWindowNumberWhenPredict; // ��ʼԤ��ʱ�Ĵ��ڸ�������Ԥ����Ĵ��ڸ����Ƚ�
	unordered_set<int> lastChangedSub;
	unordered_map<int, bool> lastChangeDirection; // false ��ʾ�������, true��ʾ������С

	myRein(int _valDom, int k, int buck_num, double limitscale) :valDom(_valDom), Dom(k), limitscale(limitscale)
	{
		subnum = 0;                        // ��ǰ���в�һ�����˶��ٸ�����
		buck_num /= newlevel;              //ÿһ��Ͱ�ĸ���
		assert(buck_num <= MY_MAX_BUCKS);   //assert ������ô�ã�                           
		for (int i = 0; i < newlevel; ++i)
		{
			/* LvBuck[i] = (int)floor(buck_num * sqrt(i+1.0) / newlevel) + 1;
			 LvBuckStep[i] = (valDom - 1) / LvBuck[i] + 1;
			 LvBuck[i] = (valDom - 1) / LvBuckStep[i] + 1;
			 assert(LvBuck[i]<MAX_BUCKS);*/
			LvBuckStep[i] = (valDom - 1) / buck_num + 1;    //  С�Ĳ㶩�ıȽ϶�Ͱ��������΢��һ��
			LvBuck[i] = (valDom - 1) / LvBuckStep[i] + 1;
			//cout << "��" << i << "�㣬Ͱ��LvBuckStep[" << i << "]=" << LvBuckStep[i] << ", Ͱ����LvBuck[" << i << "]=" << LvBuck[i] << endl;
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
			checkLevel[i].push_back(-1);// ��һ��ʱ�䴰�ڽ���ʱÿ���������ڵĲ��
		}

		//memset(countlist, 0, sizeof(countlist));                      //memset�������ã�
		firstchange = true;
		window_number = 1;
		adjustcost = 0;
		state = new int; //��ָ����������ڴ棬
		historyWindowNumberWhenPredict = new int;
		*state = 0;  //ʹ��ָ�롣
		*historyWindowNumberWhenPredict = 0;

		// ��ʼ��mapTable
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
void predict(void* pd); // ��ʵҲ����ֱ�Ӵ�this������ഫ��һЩ����Ҫ�õ������ݣ� Ϊʲô�ᴫ��һЩ����Ҫ�����ݡ���void * �ĺô��ǣ�

#endif
