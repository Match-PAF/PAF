#include "rein.h"
//这个属于类的成员函数。
void Rein::insert(IntervalSub& sub, int64_t& origintime)                    //对每一个订阅都执行插入的操作。这个origintime 是形参，
{
	Timer t;                                                                //为啥可以表示计时器？
	double poss = 1;                                                        // poss is the matching possibility;  初始化目的是赋值？
	int level;                                                              //声明一个变量必须赋值？  声明与定义得区别？
	for (int j = 0; j < sub.size; ++j)
	{
		IntervalCnt& cnt = sub.constraints[j];
		poss *= (cnt.highValue - cnt.lowValue) * 1.0 / valDom;              //poss=poss * (cnt.highValue-cnt.lowValue)*1.0/valDom
	}
	level = (int)floor(log(poss) / log(0.1) / Dom * newlevel);              //Dom is the number of interval constraints contained in s_j;  newlevel is the number of categories
	if (level < 0) level = 0;
	else if (level >= newlevel) level = newlevel - 1;                       //level表示最后一层的编号，from 0.     LvBuckStep[MAX_CONS]: 每一层的宽度都不一样吗？                     
	int indexId = LvSize[level].size(), buckStep = LvBuckStep[level];       // indexId:level层第几个订阅。  LvSize:每一层订阅的个数；level层的宽度？  
	subtolevel[sub.id] = level;                                             //给subtolevel 赋初值，在rein类里面已经声明过了（是叫定义吗?）
	originlevel[sub.id] = level;
	LvSubNum[level]++;
	for (int j = 0; j < sub.size; ++j)                                      //这是往桶里面插入订阅吗？
	{
		IntervalCnt& cnt = sub.constraints[j];
		kCombo c;
		c.val = cnt.lowValue;										     
		c.subID = sub.id;                                                   //  c.subID 怎么用了？                
		c.indexId = indexId;                                                //这个indexID 为啥是 LvSize[level].size()
		data[level][cnt.att][0][c.val / buckStep].push_back(c);             //没有插入ID呀，oldrein 里面也没有。
		c.val = cnt.highValue;
		data[level][cnt.att][1][c.val / buckStep].push_back(c);
	}
	//每调用一次insert函数，就在LvSize[level]中存入一个sub
	LvSize[level].push_back(sub.id); //这个vector的用法？规定的数组的元素个数 LvSize[MAX_CONS]       把sub.id 存到LvSize这个数组的level层，但是没说明每层有多少个？,  
	++subnum;           //the number of subs           重新计数sub得作用，和subs 重复了。    这个subnum 怎么递增？

	origintime = t.elapsed_nano();                                          //这个时间是干啥的？                                                                                                   
//}

}




void Rein::match(const Pub& pub, int& matchSubs, vector<double>& matchDetailPub)
{

	Timer t;

	for (int siIndex = 0; siIndex < newlevel; ++siIndex)                   //从层数低的遍历到层数高的，the number of the level is low,the priority is high.                             
	{
		vector<bool> bits(LvSize[siIndex].size(), false);                  //把LvSize[siIndex].size()所有位置都标记为false,初始话，表示都匹配。
		int bucks = LvBuck[siIndex], buckStep = LvBuckStep[siIndex];       //LvBuck[siIndex]表示siIndex层桶得个数。
		for (int i = 0; i < pub.size; i++)                                 //每个pub 包含所有属性。rein的缺陷。
		{

			int value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
			vector<kCombo>& data_0 = data[siIndex][att][0][buck];


			int data_0_size = data_0.size();                               // data_0.size():siIndex 层的订阅个数。
			for (int k = 0; k < data_0_size; k++)                          //data_0 是四维数组，这个k代表第几个订阅。
				if (data_0[k].val > value)
					bits[data_0[k].indexId] = true;

			for (int j = buck + 1; j < bucks; j++) {
				vector<kCombo>& data_1 = data[siIndex][att][0][j];

				int data_1_size = data_1.size();
				for (int k = 0; k < data_1_size; k++)
					bits[data_1[k].indexId] = true;
			}



			vector<kCombo>& data_2 = data[siIndex][att][1][buck];
			int data_2_size = data_2.size();
			for (int k = 0; k < data_2_size; k++)
				if (data_2[k].val < value)
					bits[data_2[k].indexId] = true;

			for (int j = buck - 1; j >= 0; j--) {
				vector<kCombo>& data_3 = data[siIndex][att][1][j];
				//if (data_3.empty()) break;
				int data_3_size = data_3.size();
				
				for (int k = 0; k < data_3_size; k++) {
					/*cout << "data_3.size()  " << data_3.size() << endl;
					cout << "k  " << k << endl;
					cout <<"data_3[k].indexId   "<< data_3[k].indexId << endl;*/
					bits[data_3[k].indexId] = true;
				}
			}
		}

		//Timer t5;//这个for 循环主要起到什么作用？实现什么功能。
		vector<int>& _Lv = LvSize[siIndex];
		int b = _Lv.size();           //每层的订阅的个数                                                 
		for (int i = 0; i < b; ++i)
			if (!bits[i] && _Lv[i] != -1)                                //从哪看出来空的是-1？  空的是-1      第siIndex层，第i个订阅 不等于-1.
			{
				matchDetailPub.push_back(t.elapsed_nano() / 1000000.0);  //matchDetailPub 这个指标干啥用？计算事件匹配的时间？干什么用？
				++matchSubs;                                             //匹配订阅的个数,没有被标记的个数。
				++countlist[window_number - 1][_Lv[i]];                  //countlist:the number of events with which s_i matches，没有被标记，并且不为空的计数，什么时候为空，删除之后？为啥减一
			}
	}


}

//  vector<IntervalSub> &subList 这个怎么理解？
//  change::return numbers of subs changed by function
int Rein::change(const vector<IntervalSub>& subList, int cstep, double matchingtime, string& windowcontent, vector<int>& changeSub)  //adjustment time window
{
	int changenum = 0, totalshouldchange = 0, limitnum = subnum / 100;   //maxcount=0;    what's the subnum?   subnum >1billion,
	bool stopflag = false;                                               //  what's the stopflag? 
	if (!firstchange)                                                    // firstchange = true      取反false 还能运行嘛？ rein 里面局部变量
	{
		limitnum = (int)(matchingtime * cstep * limitscale / adjustcost);//cstep: adjustment time window  , adjust limit
	}
	Timer t;
	/*for(int i=0;i<subnum;++i){
		if(countlist[i]>maxcount){
			maxcount=countlist[i];
		}
	}*/
	//int zerocount=0,ninecount=0;


	//window_number 是递增的。
	for (int i = 0; i < subnum; ++i) {
		//int level=newlevel-1-(int)floor((double)countlist[i]/(maxcount+1e-6)*newlevel);
		//int level=originlevel[i]-(int)round((double)countlist[i]/cstep*originlevel[i]);//cstep:the size of the time window in terms of the number of events.
		int level = (int)round(pow(1 - (double)countlist[window_number - 1][i] / cstep, 2) * originlevel[i]);   //countlist:the number of events with which s_i matches;  会算出16层。 
		//写这一段的目的是啥？ level 为大于10吗？
		if (level >= 10) {
			cout << "level=" << level << endl;
			cout << countlist[window_number - 1][i] << endl;
			cout << pow(1 - (double)countlist[window_number - 1][i] / cstep, 2) << endl;
		}
		int oldlevel = subtolevel[i];                                    // 原始的level。
		/*if(countlist[i]==0){
			level=oldlevel+1;
			if(level>=newlevel)
				level=oldlevel;
		}*/
		//if(level == oldlevel) continue;
		int distancelevel = abs(level - oldlevel);
		if (distancelevel == 0) continue;                                //如果调整的距离为1，并且调整的个数>limitnum, 也结束这次循环。
		if (distancelevel == 1 && changelist[1].size() >= limitnum) continue;//vector<changeaction> changelist[MAX_CONS];1 代表distancelevel=1,  
		changeaction tmp;                  //实例化一个调整的行为
		tmp.id = i;                                     //i<subnum
		tmp.oldlevel = oldlevel;
		tmp.newlevel = level;
		changelist[distancelevel].push_back(tmp);
		++totalshouldchange;  //计数的变量
	}
	/*vector<int> &_Lv = LvSize[0];
	int l=_Lv.size();
	int num=0;
	for(int j=0;j<l;++j)
		if(_Lv[j]!=-1)
			++num;
	cout<<"level zero: "<<l<<' '<<num<<endl;*/
	//cout<<"maxcount: "<<maxcount<<endl;
	cout << "totalshouldchange: " << totalshouldchange << endl;          //totalshouldchange： 这个是怎么计算的？
	//windowcontent += to_string(totalshouldchange) +"\t";
	//最多可以调整newlevel  层。
	for (int i = newlevel - 1; i > 0; --i) {                             //newlevel-1，从0开始计数，贪心算法。 从跨度高的到跨度低的调整，根据distancelevel 贪心 递归                                     
		//cout<<i<<' ';                                                  
		int l = changelist[i].size();
		//cout<<l<<endl;
		for (int ii = 0; ii < l; ++ii) {

			changeaction& action = changelist[i][ii];                    //changelist 不是以为数组吗？怎么变成二维了？
			int id = action.id, oldlevel = action.oldlevel, level = action.newlevel;                    //这个id是需要调整的订阅得id. 这个newlevel不是总层数那个newlevel.
			int indexId;           //这个indexId 是从0开始的吗？

			//这两个循环主要解决有效插入的问题吗？如果想到先删除空的位置，插入某个位置为啥不用insert函数。
			//这里只是插入ID，订阅的约束值，怎么办？
			//emptymark[]：在哪里往这个数组里面存空的位置，进行统计等操作。			                         //vector<int>emptymark[MAX_CONS]
			if (emptymark[level].empty()) {                                                              //空空为满,  把level 层空的位置标记。
				indexId = LvSize[level].size();                                                          // indexId=LvSize[level].size()
				LvSize[level].push_back(id);
			}
			else {																	                  
				indexId = emptymark[level].back();                                                       //返回的是空的位置？在level层的最后一个。
				emptymark[level].pop_back();                                                             //删除的是？ 从最后一个开始删除
				LvSize[level][indexId] = id;                                                             //如果为空就插入这个要调整的sub_id，节省空间。
			} //层数和桶宽的关系？
			int oldindexId = -1, buckStep = LvBuckStep[level], oldbuckStep = LvBuckStep[oldlevel];       //  oldindexId=-1是啥意思？ 什么原理直接让他取负1.
			subtolevel[id] = level;                                                                      //sub.id => level，这个订阅调整得目的？
			IntervalSub sub = subList[id];                                                               //这个sublist 表示什么？
 //上面这三行的作用是啥？主要解决了什么问题。


			//从旧的桶里面删除相同（已经调整插入）的订阅
			for (int j = 0; j < sub.size; ++j) {        //IntervalSub sub=subList[id];  j表示订阅约束的个数。
				IntervalCnt cnt = sub.constraints[j];
				vector<kCombo>::iterator ed = data[oldlevel][cnt.att][0][cnt.lowValue / oldbuckStep].end();
				//bool fl=false;
				for (vector<kCombo>::iterator it = data[oldlevel][cnt.att][0][cnt.lowValue / oldbuckStep].begin(); it != ed; ++it)
					if (it->subID == id)
					{
						oldindexId = it->indexId;
						data[oldlevel][cnt.att][0][cnt.lowValue / oldbuckStep].erase(it);
						//fl=true;
						break;
					}
				//if(!fl)cout<<"error!not found lowValue"<<endl;
				//fl=false;
				ed = data[oldlevel][cnt.att][1][cnt.highValue / oldbuckStep].end();
				for (vector<kCombo>::iterator it = data[oldlevel][cnt.att][1][cnt.highValue / oldbuckStep].begin(); it != ed; ++it)
					if (it->subID == id) {
						data[oldlevel][cnt.att][1][cnt.highValue / oldbuckStep].erase(it);
						//fl=true;
						break;
					}
				//if(!fl)cout<<"error!not found highValue"<<endl;

				//这里插入数据的目的是？
				kCombo c;
				c.val = cnt.lowValue;
				c.subID = id;
				c.indexId = indexId;
				data[level][cnt.att][0][c.val / buckStep].push_back(c);
				c.val = cnt.highValue;
				data[level][cnt.att][1][c.val / buckStep].push_back(c);
			}
			LvSize[oldlevel][oldindexId] = -1;                             //  标记空位置？
			emptymark[oldlevel].push_back(oldindexId);                     //  这两行的作用？   
			changeSub.push_back(id), changeSub.push_back(oldlevel), changeSub.push_back(level);
			LvSubNum[oldlevel]--;
			LvSubNum[level]++;
			++changenum;
			if (changenum >= limitnum) {
				stopflag = true;
				break;
			}
		}
		if (stopflag)break;
	}

	if (changenum) {
		adjustcost = (double)t.elapsed_nano() / 1000000 / changenum;       //调整一个订阅的成本
		firstchange = false;                                               //这句话干啥用？   
	}
	for (int i = newlevel - 1; i > 0; --i) changelist[i].clear();          //为啥清空？    
	//memset(countlist, 0, sizeof(countlist));                             //这个函数什么时候使用，作用是啥？
	//windowcontent += to_string(changenum) +"\n";
	//cout<<zerocount<<" "<<ninecount<<endl;
	return changenum;
}

// 输出每一层订阅的个数
void Rein::check() {
	for (int i = 0; i < newlevel; ++i) {
		vector<int>& _Lv = LvSize[i];
		int l = _Lv.size();
		int num = 0;
		for (int j = 0; j < l; ++j)
			if (_Lv[j] != -1)
				++num;
		cout << i << ' ' << l << ' ' << num << endl;   //检查第i层 l个位置不为空的个数。
	}
}

