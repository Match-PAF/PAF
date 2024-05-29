#ifndef _GENERATOR_H
#define _GENERATOR_H
#include "data_structure.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <cmath>
#include <algorithm>
class generator { 
    void GenUniformAtts(Sub &sub);// how to understand the uniform attributes?

    void GenUniformAtts(Pub &pub);

    void GenZipfAtts(Sub &sub); // what is the difference between the zipf and uniform?

    void GenZipfAtts(Pub &pub);

    void GenUniformValues(Sub &sub);// what does the values represents for?

    void GenZiefValues(Sub &sub);

    void GenUniformValues(Pub &pub);

    void GenZiefValues(Pub &pub);

    bool CheckExist(vector<int> a, int x);  //check who?

    int random(int x);

    int zipfDistribution(int n, double alpha);

    Sub GenOneSub(int id);

    Pub GenOnePub();

    string OPR[3] = {"=", "<=", ">="};

public:
    vector<Sub> subList;
    vector<Pub> pubList;
    int subs, pubs, k, m, subAttDis, subValDis, pubAttDis, pubValDis, attDom, valDom;
    double width, equalRatio, subAttalpha ,subValalpha ,pubAttalpha, pubValalpha;
    int c;
    bool first;
    generator(int subs, int pubs, int k, int m, int subAttDis, int subValDis, int pubAttDis, int pubValDis, int attDom, int valDom,
              double subAttalpha, double subValalpha, double pubAttalpha, double pubValalpha, double width, double equalRatio):
            subs(subs), pubs(pubs), k(k), m(m), subAttDis(subAttDis), subValDis(subValDis), pubAttDis(pubAttDis), pubValDis(pubValDis), attDom(attDom), valDom(valDom),
            subAttalpha(subAttalpha) ,subValalpha(subValalpha) ,pubAttalpha(pubAttalpha), pubValalpha(pubValalpha), width(width), equalRatio(equalRatio)
    {
        srand(time(NULL));// generonte random number
    }


    void GenSubList();

    void GenPubList();
};




class intervalGenerator {
    void GenUniformAtts(IntervalSub &sub);

    void GenUniformAtts(Pub &pub);

    void GenZipfAtts(IntervalSub &sub);

    void GenZipfAtts(Pub &pub);

    void GenUniformValues(IntervalSub &sub);

    void GenZiefValues(IntervalSub &sub);

    void GenUniformValues(Pub &pub);
	void GenHotValues(Pub &pub);

    void GenZiefValues(Pub &pub);
    bool CheckExist(vector<int> a, int x);

    int random(int x);

    int zipfDistribution(int n, double alpha);

    IntervalSub GenOneSub(int id);

    Pub GenOnePub(),GenHotPub();

    Pub GenOneUniformPub();  // Swhua

public:
    vector<IntervalSub> subList;
    vector<Pub> pubList;
	vector<Pub> hotlist;
    int subs, pubs, k, m, subAttDis, subValDis, pubAttDis, pubValDis, attDom, valDom,nexthot,hotsum, nowpub;
	vector<int>hotlife,hotscale;
    double width, equalRatio, subAttalpha ,subValalpha ,pubAttalpha, pubValalpha;
    double c;
    bool first;
    // subs, pubs, k, m, 0, 0, 0, 1, attDom, valDom, 1, 1, 1, 1, 0.5, 0.3
    intervalGenerator(int subs, int pubs, int k, int m, int subAttDis, int subValDis, int pubAttDis, int pubValDis, int attDom, int valDom,
              double subAttalpha, double subValalpha, double pubAttalpha, double pubValalpha, double width, double equalRatio):
            subs(subs), pubs(pubs), k(k), m(m), subAttDis(subAttDis), subValDis(subValDis), pubAttDis(pubAttDis), pubValDis(pubValDis), attDom(attDom), valDom(valDom),
            subAttalpha(subAttalpha) ,subValalpha(subValalpha) ,pubAttalpha(pubAttalpha), pubValalpha(pubValalpha), width(width), equalRatio(equalRatio),nexthot(0),hotsum(0)
    {
        srand(time(NULL));
    }

    void GenSubList();
	void ReadSubList();// how to read?
    void GenPubList();
	void ReadPubList();

    void GenUniformSubList();
    void GenzipfPubList();

    void GenPubList2();  // Swhua
};

#endif
