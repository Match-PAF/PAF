#include "rein.h"
//���������ĳ�Ա������
void Rein::insert(IntervalSub& sub, int64_t& origintime)                    //��ÿһ�����Ķ�ִ�в���Ĳ��������origintime ���βΣ�
{
	Timer t;                                                                //Ϊɶ���Ա�ʾ��ʱ����
	double poss = 1;                                                        // poss is the matching possibility;  ��ʼ��Ŀ���Ǹ�ֵ��
	int level;                                                              //����һ���������븳ֵ��  �����붨�������
	for (int j = 0; j < sub.size; ++j)
	{
		IntervalCnt& cnt = sub.constraints[j];
		poss *= (cnt.highValue - cnt.lowValue) * 1.0 / valDom;              //poss=poss * (cnt.highValue-cnt.lowValue)*1.0/valDom
	}
	level = (int)floor(log(poss) / log(0.1) / Dom * newlevel);              //Dom is the number of interval constraints contained in s_j;  newlevel is the number of categories
	if (level < 0) level = 0;
	else if (level >= newlevel) level = newlevel - 1;                       //level��ʾ���һ��ı�ţ�from 0.     LvBuckStep[MAX_CONS]: ÿһ��Ŀ�ȶ���һ����                     
	int indexId = LvSize[level].size(), buckStep = LvBuckStep[level];       // indexId:level��ڼ������ġ�  LvSize:ÿһ�㶩�ĵĸ�����level��Ŀ�ȣ�  
	subtolevel[sub.id] = level;                                             //��subtolevel ����ֵ����rein�������Ѿ��������ˣ��ǽж�����?��
	originlevel[sub.id] = level;
	LvSubNum[level]++;
	for (int j = 0; j < sub.size; ++j)                                      //������Ͱ������붩����
	{
		IntervalCnt& cnt = sub.constraints[j];
		kCombo c;
		c.val = cnt.lowValue;										     
		c.subID = sub.id;                                                   //  c.subID ��ô���ˣ�                
		c.indexId = indexId;                                                //���indexID Ϊɶ�� LvSize[level].size()
		data[level][cnt.att][0][c.val / buckStep].push_back(c);             //û�в���IDѽ��oldrein ����Ҳû�С�
		c.val = cnt.highValue;
		data[level][cnt.att][1][c.val / buckStep].push_back(c);
	}
	//ÿ����һ��insert����������LvSize[level]�д���һ��sub
	LvSize[level].push_back(sub.id); //���vector���÷����涨�������Ԫ�ظ��� LvSize[MAX_CONS]       ��sub.id �浽LvSize��������level�㣬����û˵��ÿ���ж��ٸ���,  
	++subnum;           //the number of subs           ���¼���sub�����ã���subs �ظ��ˡ�    ���subnum ��ô������

	origintime = t.elapsed_nano();                                          //���ʱ���Ǹ�ɶ�ģ�                                                                                                   
//}

}




void Rein::match(const Pub& pub, int& matchSubs, vector<double>& matchDetailPub)
{

	Timer t;

	for (int siIndex = 0; siIndex < newlevel; ++siIndex)                   //�Ӳ����͵ı����������ߵģ�the number of the level is low,the priority is high.                             
	{
		vector<bool> bits(LvSize[siIndex].size(), false);                  //��LvSize[siIndex].size()����λ�ö����Ϊfalse,��ʼ������ʾ��ƥ�䡣
		int bucks = LvBuck[siIndex], buckStep = LvBuckStep[siIndex];       //LvBuck[siIndex]��ʾsiIndex��Ͱ�ø�����
		for (int i = 0; i < pub.size; i++)                                 //ÿ��pub �����������ԡ�rein��ȱ�ݡ�
		{

			int value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
			vector<kCombo>& data_0 = data[siIndex][att][0][buck];


			int data_0_size = data_0.size();                               // data_0.size():siIndex ��Ķ��ĸ�����
			for (int k = 0; k < data_0_size; k++)                          //data_0 ����ά���飬���k����ڼ������ġ�
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

		//Timer t5;//���for ѭ����Ҫ��ʲô���ã�ʵ��ʲô���ܡ�
		vector<int>& _Lv = LvSize[siIndex];
		int b = _Lv.size();           //ÿ��Ķ��ĵĸ���                                                 
		for (int i = 0; i < b; ++i)
			if (!bits[i] && _Lv[i] != -1)                                //���Ŀ������յ���-1��  �յ���-1      ��siIndex�㣬��i������ ������-1.
			{
				matchDetailPub.push_back(t.elapsed_nano() / 1000000.0);  //matchDetailPub ���ָ���ɶ�ã������¼�ƥ���ʱ�䣿��ʲô�ã�
				++matchSubs;                                             //ƥ�䶩�ĵĸ���,û�б���ǵĸ�����
				++countlist[window_number - 1][_Lv[i]];                  //countlist:the number of events with which s_i matches��û�б���ǣ����Ҳ�Ϊ�յļ�����ʲôʱ��Ϊ�գ�ɾ��֮��Ϊɶ��һ
			}
	}


}

//  vector<IntervalSub> &subList �����ô��⣿
//  change::return numbers of subs changed by function
int Rein::change(const vector<IntervalSub>& subList, int cstep, double matchingtime, string& windowcontent, vector<int>& changeSub)  //adjustment time window
{
	int changenum = 0, totalshouldchange = 0, limitnum = subnum / 100;   //maxcount=0;    what's the subnum?   subnum >1billion,
	bool stopflag = false;                                               //  what's the stopflag? 
	if (!firstchange)                                                    // firstchange = true      ȡ��false ��������� rein ����ֲ�����
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


	//window_number �ǵ����ġ�
	for (int i = 0; i < subnum; ++i) {
		//int level=newlevel-1-(int)floor((double)countlist[i]/(maxcount+1e-6)*newlevel);
		//int level=originlevel[i]-(int)round((double)countlist[i]/cstep*originlevel[i]);//cstep:the size of the time window in terms of the number of events.
		int level = (int)round(pow(1 - (double)countlist[window_number - 1][i] / cstep, 2) * originlevel[i]);   //countlist:the number of events with which s_i matches;  �����16�㡣 
		//д��һ�ε�Ŀ����ɶ�� level Ϊ����10��
		if (level >= 10) {
			cout << "level=" << level << endl;
			cout << countlist[window_number - 1][i] << endl;
			cout << pow(1 - (double)countlist[window_number - 1][i] / cstep, 2) << endl;
		}
		int oldlevel = subtolevel[i];                                    // ԭʼ��level��
		/*if(countlist[i]==0){
			level=oldlevel+1;
			if(level>=newlevel)
				level=oldlevel;
		}*/
		//if(level == oldlevel) continue;
		int distancelevel = abs(level - oldlevel);
		if (distancelevel == 0) continue;                                //��������ľ���Ϊ1�����ҵ����ĸ���>limitnum, Ҳ�������ѭ����
		if (distancelevel == 1 && changelist[1].size() >= limitnum) continue;//vector<changeaction> changelist[MAX_CONS];1 ����distancelevel=1,  
		changeaction tmp;                  //ʵ����һ����������Ϊ
		tmp.id = i;                                     //i<subnum
		tmp.oldlevel = oldlevel;
		tmp.newlevel = level;
		changelist[distancelevel].push_back(tmp);
		++totalshouldchange;  //�����ı���
	}
	/*vector<int> &_Lv = LvSize[0];
	int l=_Lv.size();
	int num=0;
	for(int j=0;j<l;++j)
		if(_Lv[j]!=-1)
			++num;
	cout<<"level zero: "<<l<<' '<<num<<endl;*/
	//cout<<"maxcount: "<<maxcount<<endl;
	cout << "totalshouldchange: " << totalshouldchange << endl;          //totalshouldchange�� �������ô����ģ�
	//windowcontent += to_string(totalshouldchange) +"\t";
	//�����Ե���newlevel  �㡣
	for (int i = newlevel - 1; i > 0; --i) {                             //newlevel-1����0��ʼ������̰���㷨�� �ӿ�ȸߵĵ���ȵ͵ĵ���������distancelevel ̰�� �ݹ�                                     
		//cout<<i<<' ';                                                  
		int l = changelist[i].size();
		//cout<<l<<endl;
		for (int ii = 0; ii < l; ++ii) {

			changeaction& action = changelist[i][ii];                    //changelist ������Ϊ��������ô��ɶ�ά�ˣ�
			int id = action.id, oldlevel = action.oldlevel, level = action.newlevel;                    //���id����Ҫ�����Ķ��ĵ�id. ���newlevel�����ܲ����Ǹ�newlevel.
			int indexId;           //���indexId �Ǵ�0��ʼ����

			//������ѭ����Ҫ�����Ч���������������뵽��ɾ���յ�λ�ã�����ĳ��λ��Ϊɶ����insert������
			//����ֻ�ǲ���ID�����ĵ�Լ��ֵ����ô�죿
			//emptymark[]����������������������յ�λ�ã�����ͳ�ƵȲ�����			                         //vector<int>emptymark[MAX_CONS]
			if (emptymark[level].empty()) {                                                              //�տ�Ϊ��,  ��level ��յ�λ�ñ�ǡ�
				indexId = LvSize[level].size();                                                          // indexId=LvSize[level].size()
				LvSize[level].push_back(id);
			}
			else {																	                  
				indexId = emptymark[level].back();                                                       //���ص��ǿյ�λ�ã���level������һ����
				emptymark[level].pop_back();                                                             //ɾ�����ǣ� �����һ����ʼɾ��
				LvSize[level][indexId] = id;                                                             //���Ϊ�վͲ������Ҫ������sub_id����ʡ�ռ䡣
			} //������Ͱ��Ĺ�ϵ��
			int oldindexId = -1, buckStep = LvBuckStep[level], oldbuckStep = LvBuckStep[oldlevel];       //  oldindexId=-1��ɶ��˼�� ʲôԭ��ֱ������ȡ��1.
			subtolevel[id] = level;                                                                      //sub.id => level��������ĵ�����Ŀ�ģ�
			IntervalSub sub = subList[id];                                                               //���sublist ��ʾʲô��
 //���������е�������ɶ����Ҫ�����ʲô���⡣


			//�Ӿɵ�Ͱ����ɾ����ͬ���Ѿ��������룩�Ķ���
			for (int j = 0; j < sub.size; ++j) {        //IntervalSub sub=subList[id];  j��ʾ����Լ���ĸ�����
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

				//����������ݵ�Ŀ���ǣ�
				kCombo c;
				c.val = cnt.lowValue;
				c.subID = id;
				c.indexId = indexId;
				data[level][cnt.att][0][c.val / buckStep].push_back(c);
				c.val = cnt.highValue;
				data[level][cnt.att][1][c.val / buckStep].push_back(c);
			}
			LvSize[oldlevel][oldindexId] = -1;                             //  ��ǿ�λ�ã�
			emptymark[oldlevel].push_back(oldindexId);                     //  �����е����ã�   
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
		adjustcost = (double)t.elapsed_nano() / 1000000 / changenum;       //����һ�����ĵĳɱ�
		firstchange = false;                                               //��仰��ɶ�ã�   
	}
	for (int i = newlevel - 1; i > 0; --i) changelist[i].clear();          //Ϊɶ��գ�    
	//memset(countlist, 0, sizeof(countlist));                             //�������ʲôʱ��ʹ�ã�������ɶ��
	//windowcontent += to_string(changenum) +"\n";
	//cout<<zerocount<<" "<<ninecount<<endl;
	return changenum;
}

// ���ÿһ�㶩�ĵĸ���
void Rein::check() {
	for (int i = 0; i < newlevel; ++i) {
		vector<int>& _Lv = LvSize[i];
		int l = _Lv.size();
		int num = 0;
		for (int j = 0; j < l; ++j)
			if (_Lv[j] != -1)
				++num;
		cout << i << ' ' << l << ' ' << num << endl;   //����i�� l��λ�ò�Ϊ�յĸ�����
	}
}

