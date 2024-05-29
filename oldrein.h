#ifndef OLD_REIN_H
#define OLD_REIN_H
#include<vector>
#include <cstring>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "constant.h"
#include "data_structure.h"

const int OLD_MAX_BUCKS = 2000;

class oldRein{
    int valDom, buckStep, bucks, subnum;
    vector<Combo> data[attDom][2][OLD_MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis,  2 represent  the number of elemnets in array
public:  
    oldRein(int _valDom, int buck_num):valDom(_valDom), subnum(0){
		buckStep = (valDom - 1) / buck_num + 1;                  //what's the buckstep?
		bucks = (valDom - 1) / buckStep + 1;
	}
    void insert(Sub &sub);
    void insert(IntervalSub &sub);
    void match(const Pub &pub, int &matchSubs, vector<double> &matchDetailPub);
};

#endif
