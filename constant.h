#ifndef _CONSTANT_H
#define _CONSTANT_H
#include<string>
const std::string expID = "134";
const int subs = 2000000;             // Number of subscriptions.
const int pubs = 10000;             // Number of publications.
const int k = 10;                // Max Number of constraints(predicates) in one sub.
const int m = 20;                // Number of constraints in one pub.
const int attDom = 20;           // Total number of attributes, i.e. dimensions.
const int valDom = 1000000;           // Cardinality of values.
const int old_buck_num = 200;   // 原始Rein里的桶
const int new_buck_num = 2000;     // 所有层的桶数之和
const double limitscale = 0.05;    // adjust limit
const int newlevel =20;         // number of levels
const int cstep = 100;            // adjustment time window   多少个事件算一个窗口
const int stepNum = 3;        // 每次预测的步数
const double percentscale[7] = { 0.01,0.05,0.25,0.5,0.75,0.95,0.99 };
const int percentscalelength = 7;

//lstm
const int64_t input_size = 1;
const int64_t hidden_size = 8;
const int64_t output_size = 1;
#endif
