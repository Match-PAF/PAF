#include <iostream>
#include "rein.h"
#include "oldrein.h"
#include "myRein.h"
#include "constant.h"
#include "lstm.h"
using namespace std;

int main(int argc, char** argv)
{
	// string expID = "2";
	// int subs;             // Number of subscriptions.
	// int pubs;             // Number of publications.
	// int k;                // Max Number of constraints(predicates) in one sub.
	// int m;                // Number of constraints in one pub.
	// int attDom;           // Total number of attributes, i.e. dimensions.
	// int valDom;           // Cardinality of values.
	// int old_buck_num;
	// int new_buck_num;     // ���в��Ͱ��֮��
	// double limitscale;    // adjust limit
	// int newlevel;         // number of levels
	// int cstep;            // adjustment time window
	//ifstream parafile;    // �����ļ�����
	//parafile.open(string("./paras/paras_") + string(argv[1]) + string(".txt")); //�����÷��ĺô��ǣ�string��ɶ�÷���
	//parafile >> subs >> pubs >> k >> m >> attDom >> valDom >> old_buck_num >> new_buck_num >> limitscale >> newlevel >> cstep;//��ȡ�ļ�����
	//parafile.close();
	
	vector<double> matchDetailTime[7];   // ÿ�б�ʾһ��scale����¼ÿ���¼���ƥ�䵽���scale�����Ķ���ʱ�����ѵ�ʱ�䣬
	//m = atts;                          // Note that Rein requires m == atts.
	vector<double> insertTimeList;
	vector<double> matchTimeList;        // ��¼ÿ���¼��ܹ����ѵ�ƥ��ʱ�� 
	vector<double> matchSubList;         // ��¼ÿ���¼�ƥ���˶��ٸ�����
	vector<double> matchDetailPub;       // ��¼��ǰ�¼�ƥ���ÿ�����������ѵ�ʱ�䣨������Ӧʱ�������ɵ����飩
	string outputFileName, content;


	// Initiate generator
	intervalGenerator gen(subs, pubs, k, m, 0, 0, 0, 1, attDom, valDom, 1, 1, 1, 1, 0.5, 0.3);  //�����вι��캯��
	//gen.ReadSubList();
	//gen.ReadPubList();
	gen.GenSubList();
	gen.GenPubList();
	//cout << "read datalist finished\n" << endl;

	//ʵ��������Ȼ��ȥ�������ڵĳ�Ա����
	oldRein old_Rein(valDom, old_buck_num);
	Rein new_Rein(valDom, k, new_buck_num, limitscale, subs);   // 10 is k: Number of constraints(predicates) in one sub.
	Rein new_ReinSConly(valDom, k, new_buck_num, limitscale, subs);
	myRein my_Rein(valDom, k, new_buck_num, limitscale);

	

	ofstream fileStream;
	int percentindex;                                                 // what does percentindex represent for?
	int numChange = 0;
	string windowcontent = "";                                        //���windowcontent Ϊɶ�ǿյģ�
	bool firstchange = true;
	double matchingtime = 0;
	double totalAdjustTime = 0;
	double responseTime = 0.0;

	///////////////////////////////////////start old rein///////////////////////////////////////////
	//// insert
	for (int i = 0; i < subs; i++)
	{
		Timer subStart;

		old_Rein.insert(gen.subList[i]);                       // Insert sub[i] into data structure.

		int64_t insertTime = subStart.elapsed_nano();    // Record inserting time in nanosecond.
		insertTimeList.push_back((double)insertTime / 1000000);  //Convert nanoseconds to millisecond
	}

	cout << "insert old_Rein finished" << endl;

	// match
	for (int i = 0; i < pubs; i++)
	{
		if (!((i + 1) % cstep)) cout << "matching " << i + 1 << "..." << endl;                               //  !((i+1)%100��ÿ��ʱ�䴰����100��pub?
		int matchSubs = 0;                                                                           // Record the number of matched subscriptions.
		matchDetailPub.clear();
		Timer matchStart;

		old_Rein.match(gen.pubList[i], matchSubs, matchDetailPub);

		int64_t eventTime = matchStart.elapsed_nano();      // Record matching time in nanosecond.
		matchTimeList.push_back((double)eventTime / 1000000);  //matchTimeList:��¼ƥ��ƥ���¼���ʱ�� 
		matchSubList.push_back(matchSubs);            //
		if (matchDetailPub.size())
			for (int j = 0; j < percentscalelength; ++j) {
				percentindex = (int)floor(matchSubs * percentscale[j]);
				matchDetailTime[j].push_back(matchDetailPub[percentindex]);
			}


	}

	cout << "matching old_Rein finished" << endl;


	// output
	outputFileName = "./Detail/old_rein.txt";        //�����ô�ã�
	content = expID+"\t"+Util::Int2String(subs) + "\t" + Util::Double2String(Util::Average(insertTimeList)) + "\t" +     //Ϊʲô��ת��string ���ͣ�  
		Util::Double2String(Util::Average(matchTimeList)) + "\t" +
		Util::Double2String(Util::Average(matchSubList)) + "\t";
	for (int i = 0; i < percentscalelength; ++i) content += Util::Double2String(Util::Average(matchDetailTime[i])) + "\t";
	Util::WriteData(outputFileName.c_str(), content);   //�������զ�ã�


	//////////////////////////////////start new rein//////////////////////////////////////////////////



	insertTimeList.clear();
	matchTimeList.clear();
	matchSubList.clear();
	firstchange = true;
	matchDetailPub.clear();
	responseTime = 0.0;
	matchingtime = 0;
	for (int i = 0; i < percentscalelength; ++i) {
		matchDetailTime[i].clear();                //�ֱ��¼ 0.01,0.05,0.25,0.5,0.75,0.95,0.99 ��ƥ��ʱ�䣬Ϊɶclear?
	}
	windowcontent = "";                //��ʼ�����ַ�����ʼ��Ϊɶ�ǿյģ�
	// insert
	for (int i = 0; i < subs; i++)
	{
		//Timer subStart;
		int64_t insertTime;                                               //sublist[i] is the set of sub
		new_Rein.insert(gen.subList[i], insertTime);                       // Insert sub[i] into data structure.
		cout << "insert new_Rein" << endl;
		//int64_t insertTime = subStart.elapsed_nano();   // Record inserting time in nanosecond.
		insertTimeList.push_back((double)insertTime / 1000000);  //
		//cout << "insert new_Rein_01" << endl;
	}

	cout << "insert new_Rein finished" << endl;

	outputFileName = "./Detail/LvSubNum_new_Rein.txt";

	content = "newRein after insertion:\n";
	for (int i = 0; i < newlevel; i++) {
		content += "Level" + to_string(i + 1) + ": " + to_string(new_Rein.LvSubNum[i]) + " Sub\n";
	}
	Util::WriteData2(outputFileName.c_str(), content);
	//cout << "insert new_Rein_02" << endl;
	//new_Rein100.check();
	// match
	vector<vector<int>> newReinChange;
	for (int i = 0; i < pubs; i++)
	{

		int matchSubs = 0;    // Record the number of matched subscriptions.
		matchDetailPub.clear();


		Timer matchStart;
		int64_t eventTime;
		new_Rein.match(gen.pubList[i], matchSubs, matchDetailPub);

		eventTime = matchStart.elapsed_nano();                 // Record matching time in nanosecond.
		matchTimeList.push_back((double)eventTime / 1000000);            //matchTimeList��eventTime������
		matchSubList.push_back(matchSubs);
		responseTime += Util::Sum(matchDetailPub);
		if (matchDetailPub.size())                        //
			for (int j = 0; j < percentscalelength; ++j) {
				percentindex = (int)floor(matchSubs * percentscale[j]);
				matchDetailTime[j].push_back(matchDetailPub[percentindex]);
			}

		if (!((i + 1) % cstep)) {
			cout << "matching " << i + 1 << "..." << endl;
			if (i + 1 == pubs)break;
			if (true) {
				matchingtime = Util::Average(matchTimeList);
				//firstchange=false;
			}
			//windowcontent = to_string(i+1);
			vector<int> changeSub;
			Timer changeStart;    //how to change?
			int changenum = new_Rein.change(gen.subList, cstep, matchingtime, windowcontent, changeSub);       //���windowcontent����ã� windowcontent��cstep������
			
			//cout << "cout windowcontent" <<windowcontent << endl;

			int64_t changeTime = changeStart.elapsed_nano();
			totalAdjustTime += (double)changeTime / 1000000;
			newReinChange.push_back(changeSub);
			cout << "change complete " << changenum << endl;
			new_Rein.countlist.push_back(vector<int>(subs));
			new_Rein.window_number++;
			//new_Rein100.check();
		}
	}

	// output
	cout << "matching new_Rein finished\n\n" << endl;

	outputFileName = "./Detail/LvSubNum_new_Rein.txt";
	content = "newRein at last\n";
	for (int i = 0; i < newlevel; i++) {
		content += "Level" + to_string(i + 1) + ": " + to_string(new_Rein.LvSubNum[i]) + " Sub\n";
	}
	Util::WriteData(outputFileName.c_str(), content);

	outputFileName = "./Detail/new_Rein.txt"; 
	content = expID + "\t"+ Util::Int2String(newlevel) + "\t" + Util::Double2String(Util::Average(insertTimeList)) + "\t";//����content+���÷���ô��⣿
	content += Util::Double2String(Util::Average(matchTimeList)) + "\t";
	matchTimeList[0] += totalAdjustTime;                //matchTimeList[0]�е�0��ɶ��˼��
	content += Util::Double2String(Util::Average(matchTimeList)) + "\t";
	content += Util::Double2String(Util::Average(matchSubList)) + "\t";
	for (int i = 0; i < percentscalelength; ++i) content += Util::Double2String(Util::Average(matchDetailTime[i])) + "\t";
	content += Util::Double2String(responseTime/ Util::Sum(matchSubList));
	Util::WriteData(outputFileName.c_str(), content);   //д�����ݣ�outputFileName.c_str()ʲô��˼��


	/*outputFileName = "./Detail/detail.txt";
	Util::WriteData(outputFileName.c_str(), windowcontent);*/

	outputFileName = "./Detail/change_new_Rein.txt";
	content = "";
	numChange = 0;
	for (int i = 0; i < newReinChange.size(); i++) {
		content += "Window" + to_string(i+1) + ", change " + to_string(newReinChange[i].size()/3) + ": ";
		numChange += newReinChange[i].size();
		
		for (int j = 0; j < newReinChange[i].size(); j += 3)
			content += to_string(newReinChange[i][j]) + " " + to_string(newReinChange[i][j + 1]) + " " + to_string(newReinChange[i][j + 2]) + ", ";
		content += "\n";
	}
	content += "Total change: " + to_string(numChange/3) + "\n";
	Util::WriteData2(outputFileName.c_str(), content);




	//////////////////////////////////////start new SConly//////////////////////////////////////////////////

	insertTimeList.clear();
	matchTimeList.clear();
	matchSubList.clear();
	firstchange = true;
	matchDetailPub.clear();

	matchingtime = 0;
	for (int i = 0; i < percentscalelength; ++i) {
		matchDetailTime[i].clear();
	}
	windowcontent = "";
	// insert
	for (int i = 0; i < subs; i++)
	{
		//Timer subStart;
		int64_t insertTime;
		new_ReinSConly.insert(gen.subList[i], insertTime);                       // Insert sub[i] into data structure.

		//int64_t insertTime = subStart.elapsed_nano();   // Record inserting time in nanosecond.
		insertTimeList.push_back((double)insertTime / 1000000);
	}

	cout << "insert new_ReinSConly finished" << endl;
	//new_Rein100.check();
	// match
	
	for (int i = 0; i < pubs; i++)
	{

		int matchSubs = 0;                                        // Record the number of matched subscriptions.
		matchDetailPub.clear();

		Timer matchStart;
		int64_t eventTime;
		cout << "i  " << i << endl;
		new_ReinSConly.match(gen.pubList[i], matchSubs, matchDetailPub);
		cout << "match " << endl;
		eventTime = matchStart.elapsed_nano();  // Record matching time in nanosecond.
		matchTimeList.push_back((double)eventTime / 1000000);
		matchSubList.push_back(matchSubs);
		
		for (int j = 0; j < percentscalelength; ++j) {
			percentindex = (int)floor(matchSubs * percentscale[j]); //   ����ȡ��
			if(percentindex==0)matchDetailTime[j].push_back(0);
			else matchDetailTime[j].push_back(matchDetailPub[percentindex]);
		}

		if (!((i + 1) % cstep)) {
			cout << "matching " << i + 1 << "..." << endl;
		}
	}

	cout << "matching new_ReinSConly finished" << endl;



	// output
	outputFileName = "./Detail/new_ReinSConly.txt";
	content = expID + "\t"+ Util::Int2String(new_buck_num) + "\t" + Util::Double2String(Util::Average(insertTimeList)) + "\t";
	content += Util::Double2String(Util::Average(matchTimeList)) + "\t" +
		Util::Double2String(Util::Average(matchSubList)) + "\t";
	for (int i = 0; i < percentscalelength; ++i) content += Util::Double2String(Util::Average(matchDetailTime[i])) + "\t";
	Util::WriteData(outputFileName.c_str(), content);



	////////////////////////////////////start new my_Rein//////////////////////////////////////////////////

	insertTimeList.clear();
	matchTimeList.clear();
	matchSubList.clear();
	firstchange = true;
	matchDetailPub.clear();
	responseTime = 0.0;
	matchingtime = 0;

//	//lstm ѵ��
//	LSTMModel lstm_model(input_size, hidden_size, output_size);
//	lstm_model.trainModel(input_data, target_data, 3000, 0.03);
//		//����ģ��
//     std::string path = "model.pth";
////	//torch::save(lstm_model, path);
//	 lstm_model.save_model( path);
////
////
////	// ���� LSTMModel ���ʵ�� loaded_model
//	LSTMModel loaded_model(input_size, hidden_size, output_size);
////
////	// ʹ�� torch::load ���ļ�����Ԥѵ����ģ�Ͳ���
//	// loaded_model.load_model( path);  //�����ǲ��������⣿



	for (int i = 0; i < percentscalelength; ++i) 
		matchDetailTime[i].clear();
	
	windowcontent = "";                //��ʼ��
	// insert
	for (int i = 0; i < subs; i++)
	{
		//Timer subStart;
		int64_t insertTime;                                               //sublist[i] is the set of sub
		my_Rein.insert(gen.subList[i], insertTime);                       // Insert sub[i] into data structure.

		//int64_t insertTime = subStart.elapsed_nano();                   // Record inserting time in nanosecond.
		insertTimeList.push_back((double)insertTime / 1000000);
	}

	cout << "insert my_Rein finished" << endl;
	//new_Rein100.check();

	outputFileName = "./Detail/LvSubNum_my_Rein.txt";
	content = "myRein after insertion\n";
	for (int i = 0; i < newlevel; i++) {
		content += "Level" + to_string(i + 1) + ": " + to_string(my_Rein.LvSubNum[i]) + " Sub\n";
	}
	Util::WriteData2(outputFileName.c_str(), content);

	const int coldWindowNumber = cstep * 3;                               // ������ô����¼����ٿ�ʼԤ��
	vector<vector<int>>** pCountList = new vector<vector<int>>*;
	*pCountList = new vector<vector<int>>;
	(**pCountList).resize(subs);
	vector<vector<int>> changedSub(pubs / cstep+1); // ÿ�����ڵ�������Щ���ĵ���Щ�㣬��ʽ��(subID, oldLevel, newLevel)
	thread* predict_thread = new thread;
	// match
	for (int i = 0; i < pubs; i++)
	{
		/*printf("Begin to match event %d\n", i + 1);
		fflush(stdout);*/
	
		int matchSubs = 0;                                                // Record the number of matched subscriptions.
		matchDetailPub.clear();

		Timer my_Rein_MatchStart;
		int64_t eventTime;
		my_Rein.match(gen.pubList[i], matchSubs, matchDetailPub);

		eventTime = my_Rein_MatchStart.elapsed_nano();
		matchTimeList.push_back((double)eventTime / 1000000);
		matchSubList.push_back(matchSubs);   //��¼ÿ���¼�ƥ���˶��ٸ�����
		responseTime += Util::Sum(matchDetailPub);
		if (matchDetailPub.size())
			for (int j = 0; j < percentscalelength; ++j) {
				percentindex = (int)floor(matchSubs * percentscale[j]);
				matchDetailTime[j].push_back(matchDetailPub[percentindex]);
			}


		if (!((i + 1) % cstep)) {
			printf("%d events have been matched.\n",i+1);
			fflush(stdout);
			for (int j = 0; j < subs; j++) { //����һ��ʱ�䴰�ڷ���λ��
				my_Rein.checkLevel[j].push_back(my_Rein.checkLevel[j][my_Rein.checkLevel[j].size() - 1]); // ��ʼ��Ϊ��һ��ʱ�䴰�ڵ�λ��
			}
			if (i+1 >= coldWindowNumber) {
				if (i + 1 == pubs)break;
				if (true) {
					matchingtime = Util::Average(matchTimeList);  // ��¼ÿ���¼��ܹ����ѵ�ƥ��ʱ��
					firstchange=false;
				}
				//windowcontent = to_string(i+1);
				
				if (*my_Rein.state == 0) { // ���Կ�ʼԤ��
					printf("Begin to predict the window %d.\n", (i + 1) / cstep + 1);
					fflush(stdout);
					Timer changeStart1;         // ������Ԥ���ʱ���������ʱ����
					*my_Rein.state = 1;         // Ԥ����
					*my_Rein.historyWindowNumberWhenPredict = (i + 1) / cstep;
					ParallelData pd = { stepNum, cstep, my_Rein.state, my_Rein.historyWindowNumberWhenPredict,&(my_Rein.countlist),pCountList };
					// ParallelData* p=&pd;
					// cout<<(*(p->predict_countlist)).size()<<endl;
					//*predict_thread=thread(predict, (void*)&pd);
					//predict_thread->detach();
					predict(&pd);
					int64_t changeTime = changeStart1.elapsed_nano();
					totalAdjustTime += (double)changeTime / 1000000;
				}
				if (*my_Rein.state == 2)     //Ԥ�����״̬����ʼ����
				{
					printf("Bingo! Prediction task of window%d is done after matching event%d\n", *my_Rein.historyWindowNumberWhenPredict + 1, i + 1);
					fflush(stdout);
					Timer changeStart2;
					my_Rein.change(gen.subList, *pCountList, changedSub, cstep, matchingtime);
					int64_t changeTime = changeStart2.elapsed_nano();
					totalAdjustTime += (double)changeTime / 1000000;
				}
				//my_Rein.check();
			}
			else { // ��������
				if (i + 1 == pubs)break;
				Timer changeStart0;             
				int changenum = my_Rein.coldChange(gen.subList, changedSub, matchingtime);
				int64_t changeTime = changeStart0.elapsed_nano();
				totalAdjustTime += (double)changeTime / 1000000;
			}
			my_Rein.window_number++; // ������
			for (int j = 0; j < subs; j++) { // ����һ��ʱ�䴰�ڷ���λ��
				my_Rein.countlist[j].push_back(0);
			}
		}

		if (*my_Rein.state == 2)   // ����Ƿ�Ԥ�����
		{
			printf("Prediction task of window%d is done after matching event%d\n", *my_Rein.historyWindowNumberWhenPredict + 1, i + 1);
			fflush(stdout);
			Timer changeStart3;
			my_Rein.change(gen.subList, *pCountList, changedSub, cstep, matchingtime);
			int64_t changeTime = changeStart3.elapsed_nano();
			totalAdjustTime += (double)changeTime / 1000000;
		}
	}  // match for loop

	// output
	printf("matching my_Rein finished\n");
	fflush(stdout);

	outputFileName = "./Detail/LvSubNum_my_Rein.txt";
	content = "myRein at last\n";
	for (int i = 0; i < newlevel; i++) {
		content += "Level" + to_string(i + 1) + ": " + to_string(my_Rein.LvSubNum[i]) + " Sub\n";
	}
	Util::WriteData(outputFileName.c_str(), content);

	outputFileName = "./Detail/mapTable_my_Rein.txt";
	content = "";
	for (int i = 0; i < newlevel; i++) {
		content += "OriginalLevel" + to_string(i) + ": \n";
		for (int j = 0; j <cstep; j++)
		{
			content += to_string(j)+" "+ to_string(my_Rein.mapTable[i][j]) + ", ";
			if (j % 10 == 0 && j > 0)
				content += "\n";
		}
		content += "\n\n";
	}
	Util::WriteData2(outputFileName.c_str(), content);
	
	outputFileName = "./Detail/countlist_my_Rein.txt";
	content = "";
	for (int i = 0; i < subs; i++) {
		//content += "Sub"+to_string(i)+": ";
		for (int j = 0; j < my_Rein.countlist[i].size(); j++)
		{
			content += to_string(my_Rein.countlist[i][j]) + " ";
		}
		content += "\n";
	}
	Util::WriteData2(outputFileName.c_str(), content);

	outputFileName = "./Detail/level_my_Rein.txt";
	content = "";
	for (int i = 0; i < subs; i++) {
		content += "Sub" + to_string(i) + ": ";
		for (int j = 0; j < my_Rein.checkLevel[i].size(); j++)
			content += to_string(my_Rein.checkLevel[i][j]) + " ";
		content += "\n";
	}
	Util::WriteData2(outputFileName.c_str(), content);

	outputFileName = "./Detail/change_my_Rein.txt";
	content = "";
	numChange = 0;
	for (int i = 1; i <= pubs/cstep; i++) {
		numChange += changedSub[i].size();
		content += "Window" + to_string(i) + ", change "+to_string(changedSub[i].size()/3)+": ";
		for (int j = 0; j < changedSub[i].size(); j += 3)
			content += to_string(changedSub[i][j]) + " " + to_string(changedSub[i][j + 1]) + " " + to_string(changedSub[i][j+2]) + ", ";
		content += "\n";
	}
	content += "Total change: " + to_string(numChange/3)+"\n";
	Util::WriteData2(outputFileName.c_str(), content);
	outputFileName = "./Detail/my_Rein.txt";            //����ļ������֣���ô�ã�
	content = expID + "\t"+ Util::Int2String(newlevel) + "\t" + Util::Double2String(Util::Average(insertTimeList)) + "\t";//����content+���÷���ô��⣿
	content += Util::Double2String(Util::Average(matchTimeList)) + "\t";
	matchTimeList[0] += totalAdjustTime;                //matchTimeList[0]�е�0��ɶ��˼��
	content += Util::Double2String(Util::Average(matchTimeList)) + "\t";
	content += Util::Double2String(Util::Average(matchSubList)) + "\t";
	for (int i = 0; i < percentscalelength; ++i) content += Util::Double2String(Util::Average(matchDetailTime[i])) + "\t";
	content += Util::Double2String(responseTime / Util::Sum(matchSubList));
	Util::WriteData(outputFileName.c_str(), content);  //д�����ݣ�outputFileName.c_str()ʲô��˼��

	/*outputFileName = "./Detail/detail.txt";
	Util::WriteData(outputFileName.c_str(), windowcontent);*/
	
	// predict_thread->join();  // �д�
	int q;
	cin >> q; // �ȴ����һ��Ԥ��������������������ֹ�����
	delete predict_thread;
	
	return 0;
}
