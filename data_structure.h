#ifndef _DATA_STRUCTURE_H
#define _DATA_STRUCTURE_H
#include<string>
#include<vector>
using namespace std;

struct Cnt{
    int att;
    int value;
    int op;     //  op==0 -> "="  op==1 -> ">="  op==2 -> "<=" op=operation
};

struct IntervalCnt{
    int att;
    int lowValue, highValue;
};

struct Sub {
    int id;                                  //s_i
    int size; 								//number of predicates
    vector<Cnt> constraints;				//list of constraints
};

struct IntervalSub{
    int id;
    int size;
    vector<IntervalCnt> constraints;   //What does this array look like?
};

struct ConElement {
    int att;
    int val;
    int subID;
};

struct Combo{
    int val;
    int subID;
};

struct kCombo                               
{
    int val;
    int subID;     
    int indexId;                            
};


struct IntervalCombo{
    int lowValue, highValue;
    int subID;
};

struct Pair{
    int att;
    int value;
};

struct changeaction{
    int id,oldlevel,newlevel;
};

struct Pub{
    int size;
    vector<Pair> pairs;
	int hotnum;                                     //the number of hot events
};

struct WeightPair{
    int id;
    double weight;
};

//һ��ָ����Ϊ�˹����ڴ棬����ָ����Ϊ���޸��ڴ�󻹿��Թ����ڴ档
struct ParallelData 
{
    int stepNum;
    int cstep;
    int* state;
    int* windowNum; // ��Ч��������countlist��ǰ������
    const vector<vector<int>>* countlist;
    vector<vector<int>>** predict_countlist;
};


#endif //_DATA_STRUCTURE_H
