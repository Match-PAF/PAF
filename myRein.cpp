#include "myRein.h"

//#include "predict.h"

void myRein::insert(IntervalSub& sub, int64_t& origintime)
{
	Timer t;
	double poss = 1;                                                    // poss is the matching possibility
	int level;
	//compute the matching probability of each subscription
	for (int j = 0; j < sub.size; ++j) {
		IntervalCnt& cnt = sub.constraints[j];
		poss *= (cnt.highValue - cnt.lowValue) * 1.0 / valDom;          //poss=poss * (cnt.highValue-cnt.lowValue)*1.0/valDom
	}
	// ����Խ�󣬲���ԽС
	level = (int)floor((log(poss) / log(0.1)) / (Dom)*newlevel);          //Dom is the number of interval constraints contained in s_j;  newlevel is the number of categories
	if (level < 0) level = 0;
	else if (level >= newlevel) level = newlevel - 1;                   //number of levels                           
	int indexId = LvSize[level].size(), buckStep = LvBuckStep[level];   // LvBuckStep[MAX_CONS];   LvSize:ÿһ�㶩�ĵĸ���   
	subtolevel[sub.id] = level;
	checkLevel[sub.id][0] = level;                                     //���checkLevel��һ����ά���飬��һά�Ƕ��ĵ�id���ڶ�ά��0��1��ֵ�͵�ֵ
	checkLevel[sub.id][1] = level;
	LvSubNum[level]++;
	originlevel[sub.id] = level;
	for (int j = 0; j < sub.size; ++j) {
		IntervalCnt& cnt = sub.constraints[j];
		kCombo c;
		c.val = cnt.lowValue;
		c.subID = sub.id;
		c.indexId = indexId;                                            //���indexID Ϊɶ�� LvSize[level].size()
		data[level][cnt.att][0][c.val / buckStep].push_back(c);
		c.val = cnt.highValue;
		data[level][cnt.att][1][c.val / buckStep].push_back(c);
	}
	LvSize[level].push_back(sub.id);                                    // LvSize[MAX_CONS]
	++subnum;

	origintime = t.elapsed_nano();
	//}

}


void myRein::match(const Pub& pub, int& matchSubs, vector<double>& matchDetailPub)
{

	Timer t;

	for (int siIndex = 0; siIndex < newlevel; ++siIndex)                //newlevel is the total number of levels
	{

		vector<bool> bits(LvSize[siIndex].size(), false);  //Initializes a Boolean vector   ���bits��������ɶ��
		int bucks = LvBuck[siIndex], buckStep = LvBuckStep[siIndex];   //buckStep is bucket width
		for (int i = 0; i < pub.size; i++)
		{

			int value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
			vector<kCombo>& data_0 = data[siIndex][att][0][buck];


			int data_0_size = data_0.size();
			for (int k = 0; k < data_0_size; k++)
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
				for (int k = 0; k < data_3_size; k++)
					bits[data_3[k].indexId] = true;
			}
		}

		//Timer t5;
		vector<int>& _Lv = LvSize[siIndex];
		int b = _Lv.size();  //siIndex�� �¼����ĵĸ���
		for (int i = 0; i < b; ++i)
			if (!bits[i] && _Lv[i] != -1)    // �յ���-1
			{
				matchDetailPub.push_back(t.elapsed_nano() / 1000000.0);
				++matchSubs;
				++countlist[_Lv[i]][window_number - 1]; //countlist:the number of events with which s_i matches
			}
	}

}

// Swhua1
vector<int> predict_arima(vector<double> countlist, int stepNum) {

	int n = countlist.size(), begin = 0;  //ƥ�䵽�¼��Ķ���
	vector<int> preMatchEvent;

	while (begin < n && countlist[begin] == 0)
		begin++;
	if (begin)
		while (begin < n && (countlist[begin] == countlist[begin - 1]))
			begin++;
	if (begin == n) {
		for (int i = 0; i < stepNum; i++)
			preMatchEvent.push_back(countlist[n-1]);
		return preMatchEvent;
	}

	double c = (countlist[n - 1] - countlist[begin]) / (n - begin + 1);
	int v = countlist[n - 1]; //(countlist[n - 1]+ countlist[n - 2]+ countlist[n - 3])/3;
	for (int i = 0; i < stepNum; i++) {     //stepNum��Ԥ�ⲽ��
		v += c;      // cstep: adjustment time window ���ٸ��¼���һ������
		if (v >= cstep) {   
			v = cstep;
			c = 0;
		}
		else if (v <= 0) {
			v = 0;
			c = 0;
		}
		preMatchEvent.push_back(v);
	}
	return preMatchEvent;
}

// Swhua2
vector<int> predict_arimas(vector<double> countlist, int stepNum) {

	int n = countlist.size(), begin = 0;
	vector<int> preMatchEvent;
	while (begin < n && countlist[begin] == 0)
		begin++;
	if (begin == n) {
		for (int i = 0; i < stepNum; i++)
			preMatchEvent.push_back(0);
		return preMatchEvent;
	}
	
	int maxv = -1, maxp = -1, minv = cstep, minp = -1;
	for (int i = begin; i < n; i++) {
		if (countlist[i] >= maxv) {
			maxv = countlist[i];
			maxp = i;
		}
		if (countlist[i] <= minv) {
			minv = countlist[i];
			minp = i;
		}
	}
	if (maxv == minv) {
		for (int i = 0; i < stepNum; i++)
			preMatchEvent.push_back(countlist[i]);
		return preMatchEvent;
	}
	
	double c = (maxv - minv) / (maxp-minp);
	int v = countlist[n - 1]; //(countlist[n - 1]+ countlist[n - 2]+ countlist[n - 3])/3;
	for (int i = 0; i < stepNum; i++) {
		v += c;
		if (v >= cstep) {
			v = cstep;
			c = 0;
		}
		else if (v <= 0) {
			v = 0;
			c = 0;
		}
		preMatchEvent.push_back(v);
	}
	return preMatchEvent;
}

// ����ģ��
void load_model(LSTMModel& model, const std::string& path) {
	torch::serialize::InputArchive input_archive;
	input_archive.load_from(path);
	model.load(input_archive);
	std::cout << "Model loaded from: " << path << std::endl;
}


 //LSTM
 // ʹ�ü��ص�ģ�ͽ���Ԥ��
vector<int> predictWithLoadedModel(LSTMModel& loaded_model, const std::vector<float>& input_data, int stepNum) {
	// ����һ�� vector �Դ洢Ԥ����
	vector<int> preMatchEvent;

	// ����Ԥ�ⴰ����
	int uplimit = stepNum;

	// ѭ������Ԥ��
	for (int l = 0; l < uplimit; l++) {
		// ����������ת��Ϊ PyTorch Tensor
		torch::Tensor inputs = torch::tensor(input_data, torch::dtype(torch::kFloat32)).view({ 1, static_cast<long>(input_data.size()), 1 });

		// ����ģ��Ϊ����ģʽ
		loaded_model.eval();

		// ʹ�ü��ص�ģ�ͽ���ǰ�򴫲��Ի�ȡԤ����
		torch::Tensor predictions = loaded_model.forward(inputs);

		// ��Ԥ�����洢�� preMatchEvent ��
		for (int i = 0; i < predictions.size(1); ++i) {
			// �� PyTorch Tensor ����ȡԤ��ֵ��ת��Ϊ����
			int predicted_value = static_cast<int>(predictions[0][i].item<float>());

			// ��Ԥ��ֵ��ӵ���� vector ��
			preMatchEvent.push_back(predicted_value);
		}
	}

	// ���ذ���Ԥ��ֵ�� vector
	return preMatchEvent;
}



//// ʹ�ü��ص�ģ�ͽ��жಽԤ��
//std::vector<int> predictWithLoadedModel(LSTMModel& model, const std::vector<float>& input_data, int stepNum) {
//	std::vector<int> predictions;
//	torch::Tensor inputs = torch::tensor(input_data, torch::dtype(torch::kFloat32)).view({ 1, static_cast<long>(input_data.size()), 1 });
//	model.eval();
//
//	for (int step = 0; step < stepNum; ++step) {
//		torch::Tensor output = model.forward(inputs);
//		int predicted_value = static_cast<int>(output[-1].item<float>());
//		inputs = torch::cat({ inputs.slice(1, 1, -1), output[-1].unsqueeze(0).unsqueeze(0) }, 1);
//		predictions.push_back(predicted_value);
//	}
//
//	return predictions;
//}





vector<int> predict_arimaA(vector<double> countlist, int stepNum)  //��ÿ��������˵��
{
	//freopen("data.txt", "r", stdin);
	//double gets;
	//vector<double> countlist;
	//while (cin >> gets) {
	//    countlist.push_back(gets);
	//    cout << gets << endl;
	//}
	//��������������ô���ݣ�
	ARIMAModel* arima = new ARIMAModel(countlist); //ѵ��ARIMAModel ģ�ͣ�new �ǿ���һ���¿ռ䣬����coutlist �������ʷ����ѵ��ģ�͡�
	vector<vector<int>> list;                      //���list ����ɶ��
	vector<int> preMatchEvent;
	int modelcnt = 5;

	int period = 7;



	int uplimit = stepNum;// Ԥ�ⴰ����
	for (int l = 0; l < uplimit; l++) {
		//cout << "Times: " << l << endl;
		int cnt = 0;
		vector<int> tmpPredict(modelcnt);  //ÿ����������ʱԤ��modelCnt����
		for (int k = 0; k < modelcnt; ++k)			//����ͨ��������������м������յĽ��
		{
			vector<int> bestModel = arima->getARIMAModel(period, list, (k == 0) ? false : true);//if k==0,k=false; else k=true.
			//cout<<bestModel.size()<<endl;
			cnt++;
			if (bestModel.size() == 0)// ����bestModel��Ԫ�صĸ���
			{
				tmpPredict[k] = (int)countlist[countlist.size() - period];   //Ϊʲô countlist.size() - period��
				break;
			}
			else
			{
				//cout<<bestModel[0]<<bestModel[1]<<endl;
				int predictDiff = arima->predictValue(bestModel[0], bestModel[1], period);//predictDiff ��ʾɶ��
				//cout<<"fuck"<<endl;
				tmpPredict[k] = arima->aftDeal(predictDiff, period);
				//cnt++;
			}
			//cout << bestModel[0] << " " << bestModel[1] << endl;
			list.push_back(bestModel); //list ��������洢����bestModel?
		}

		double sumPredict = 0.0;
		//cout << "cnt=" << cnt << endl; // Ԥ���˶��ٸ�ֵ 
		for (int k = 0; k < cnt; ++k)
		{
			sumPredict += ((double)max(0, tmpPredict[k]));
			//cout << "tmp results=" << tmpPredict[k] << endl;
		}
		int predict = (int)round(sumPredict / (double)cnt);
		//cout << "Predict time = " << countlist.size() << endl;
		//cout << "Predict value = " << predict << endl;
		if (predict > cstep) predict = cstep;
		preMatchEvent.push_back(predict);
	}
	return preMatchEvent;
}

////һ��ָ����Ϊ�˹����ڴ棬����ָ����Ϊ���޸��ڴ�󻹿��Թ����ڴ档
////��Ԥ�⺯�����в��У�������е����ԭ����ʲô���� 
////������˫ָ���ԭ�򣺣�����
////voidָ����������ָ�룬�����ڵĺܶ�����У���������ȷ��ʱ��������ָ����棬��һ���ָ�����߳�\���̺������ر𳣼�
//void predict(void* pld) {  //pld�����������ݡ� ��predict�������洫����һ��ָ�롣
//	//cout << "PredictThread: Prediction Begin.\n";
//	ParallelData* pd = (ParallelData*)pld; //pd:ָ������ת�������Ҹ�ֵ�� ����Ӳ�������, �����ԣ�����ʾǿ������ת����ת������ͬ���ͣ������໥��ֵ��
//	//countlist�Ƕ�ά���飬n�����������ĵĸ���
//	int n = (*(pd->countlist)).size();  // ���ĸ�������ά�����size()�Ƕ��ĵø�������������size�����С� const vector<vector<int>>* countlist
//	//cout << "n=" << n << endl;
//	//cout << "ParallelData: " << &(*pd) << " predict_countlist" << *(pd->predict_countlist);
//	//vector<vector<int>>* p_countlist = new vector<vector<int>>;//�����ֵ�����ã�
//	//(*p_countlist).resize(n, vector<int>(pd->stepNum));
//	vector<vector<int>>* p_countlist = *(pd->predict_countlist);
//	//cout << "****************\n";
//	//delete* (pd->predict_countlist);
//	//windowNum ��ParallelData����ĳ�Ա���ԣ��ǵ�ǰ��������
//	int history_window_number = *(pd->windowNum); // history_window_number ���е���ʷ����������
//	int begin = history_window_number > 8 ? history_window_number - 8 : 0;//ֻ�������8������ΪArima��Ϊ9��ʱ�������8��ʱ�����ð�������ʷ���ݶ������ˣ����Դӵ��߸����ڿ�ʼ�����ˡ�
//	int window_number_predict = history_window_number > 8 ? 8 : history_window_number; // ���ֻ�����8�����ڽ���Ԥ��
//	int numPredictSub = 0;  // ��������Ԥ���˵Ķ��ĸ���
//
//   //stepNum=3,  predict_countlist����������n, ÿһ��Ԫ��Ϊvector<int>(pd->stepNum)����Ϊ�У����У�����ʱ�䴰�ڡ�
//   //n��3��	
//	//��Ԥ��ֵ
//	// ���������Ĵ�СΪn�����ݺ��ÿ��Ԫ�ص�ֵΪvector<int>(pd->stepNum)
//
//	//delete(*(pd->predict_countlist)); //ɾ��һ��ָ�롣
//	//#pragma omp parallel for shared(countlist)                // num_threads(2) ordered  schedule(static,2)  //���̣߳�
//	//cout<<(*(pd->countlist)).size()<<endl;
//	//cout<<" predict_countlist: "<<(*((*pd).predict_countlist)).size()<<endl;
//	//cout << history_window_number << endl;
//	for (int i = 0; i < n; i++) {    // ���Զ�ÿ�����Ľ���Ԥ��
//		//cout << i << " countlist: " << (*(pd->countlist))[i].size() << endl;
//		int fluctuation = abs((*(pd->countlist))[i][history_window_number - 3] - (*(pd->countlist))[i][history_window_number - 2])
//			+ abs((*(pd->countlist))[i][history_window_number - 2] - (*(pd->countlist))[i][history_window_number - 1]);
//
//		if ((*(pd->countlist))[i][history_window_number - 1] < 0.1 * cstep && fluctuation <= 0.3 * cstep) {  // ����ҪԤ��   cstep����ÿ��ʱ�䴰���¼��ø�������Ӧ����0.1*cstep��Ϊ��ֵ���й��ˡ�
//			(*p_countlist)[i].clear();
//			//printf("%d continue\n", i); fflush(stdout);
//			continue;
//		}
//		numPredictSub++;
//		//cout<<i<<"   1\n";
//		//window_number_predict һ����8��
//		//���¶���һ��Ԥ�⺯����Ҫ�����顣
//		vector<double> countList(window_number_predict); //Ԥ�⺯������Ҫ�õ�����ʷ���ݡ� ��ParallelData�����const vector<vector<int>>* countlistһ����
//		//int begin = history_window_number > 8 ? history_window_number - 8 : 0;//ֻ�������8������ΪArima��Ϊ9��ʱ�������8��ʱ�����ð�������ʷ���ݶ������ˣ����Դӵ��߸����ڿ�ʼ�����ˡ�
//		//int window_number_predict = history_window_number > 8 ? 8 : history_window_number; // ���ֻ�����8�����ڽ���Ԥ��
//
//		for (int j = 0; j < window_number_predict; j++) {
//			//���counList[j]��һά���顣��һ����ά���鸳��һά���飬���԰ɣ� const vector<vector<int>>* countlist
//			countList[j] = (double)(*(pd->countlist))[i][begin + j];// i�Ƕ��ģ�begin+j Ԥ����Ҫ����ʷ���ݡ���double��ǿ��ת��Ϊ�˸�ֵ��
//		}
//		//cout <<"predict"<<i<< "sub\n";
//		//ָ�����飬vector<vector<int>>* predict_countlist
//
//		(*p_countlist)[i] = predict_arima(countList, stepNum); //��vector���������ʱ��ֻ�ô��������Ϳ��ԣ����ô���С��
//		//cout<<"Sub"<<i<<" predict done.\n";
//	}
//
//	//printf("delete\n"); fflush(stdout);
//	//delete (*(pd->predict_countlist)); //ɾ��һ��ָ�롣
//	//printf("set\n"); fflush(stdout);
//	//*(pd->predict_countlist) = p_countlist;
//	printf("Prediction thread ends, %d subs were predicted.\n", numPredictSub);
//	fflush(stdout);
//	*(pd->state) = 2; // Ԥ�����
//	return;
//}

void predict(void* pld) {  //pld�����������ݡ� ��predict�������洫����һ��ָ�롣
	//cout << "PredictThread: Prediction Begin.\n";
	ParallelData* pd = (ParallelData*)pld; //pd:ָ������ת�������Ҹ�ֵ�� ����Ӳ�������, �����ԣ�����ʾǿ������ת����ת������ͬ���ͣ������໥��ֵ��
	//countlist�Ƕ�ά���飬n�����������ĵĸ���
	int n = (*(pd->countlist)).size();  // ���ĸ�������ά�����size()�Ƕ��ĵø�������������size�����С� const vector<vector<int>>* countlist
	//cout << "n=" << n << endl;
	//cout << "ParallelData: " << &(*pd) << " predict_countlist" << *(pd->predict_countlist);
	//vector<vector<int>>* p_countlist = new vector<vector<int>>;//�����ֵ�����ã�
	//(*p_countlist).resize(n, vector<int>(pd->stepNum));
	vector<vector<int>>* p_countlist = *(pd->predict_countlist);
	//cout << "****************\n";
	//delete* (pd->predict_countlist);
	//windowNum ��ParallelData����ĳ�Ա���ԣ��ǵ�ǰ��������
	int history_window_number = *(pd->windowNum); // history_window_number ���е���ʷ����������
	int begin = history_window_number > 8 ? history_window_number - 8 : 0;//ֻ�������8������ΪArima��Ϊ9��ʱ�������8��ʱ�����ð�������ʷ���ݶ������ˣ����Դӵ��߸����ڿ�ʼ�����ˡ�
	int window_number_predict = history_window_number > 8 ? 8 : history_window_number; // ���ֻ�����8�����ڽ���Ԥ��
	int numPredictSub = 0;  // ��������Ԥ���˵Ķ��ĸ���


	// ���� LSTM ģ��
// ����ģ�Ͳ���
	//const int64_t input_size = 1;//�������ݵ�����ά��
	//const int64_t hidden_size = 8;//���ز���Ԫ�ĸ���
	//const int64_t output_size = 1;//������ݵ�����ά��
	LSTMModel lstm_model(input_size, hidden_size, output_size);
	std::string model_path = "D:/SCSLlibtorch/LSTM/LSTM/model.pth"; // �滻Ϊ����ģ��·��
	load_model(lstm_model, model_path);
   //stepNum=3,  predict_countlist����������n, ÿһ��Ԫ��Ϊvector<int>(pd->stepNum)����Ϊ�У����У�����ʱ�䴰�ڡ�
   //n��3��	
	//��Ԥ��ֵ
	// ���������Ĵ�СΪn�����ݺ��ÿ��Ԫ�ص�ֵΪvector<int>(pd->stepNum)

	//delete(*(pd->predict_countlist)); //ɾ��һ��ָ�롣
	//#pragma omp parallel for shared(countlist)                // num_threads(2) ordered  schedule(static,2)  //���̣߳�
	//cout<<(*(pd->countlist)).size()<<endl;
	//cout<<" predict_countlist: "<<(*((*pd).predict_countlist)).size()<<endl;
	//cout << history_window_number << endl;
	for (int i = 0; i < n; i++) {    // ���Զ�ÿ�����Ľ���Ԥ��
		//cout << i << " countlist: " << (*(pd->countlist))[i].size() << endl;
		int fluctuation = abs((*(pd->countlist))[i][history_window_number - 3] - (*(pd->countlist))[i][history_window_number - 2])
			+ abs((*(pd->countlist))[i][history_window_number - 2] - (*(pd->countlist))[i][history_window_number - 1]);

		if ((*(pd->countlist))[i][history_window_number - 1] < 0.1 * cstep && fluctuation <= 0.3 * cstep) {  // ����ҪԤ��   cstep����ÿ��ʱ�䴰���¼��ø�������Ӧ����0.1*cstep��Ϊ��ֵ���й��ˡ�
			(*p_countlist)[i].clear();
			//printf("%d continue\n", i); fflush(stdout);
			continue;
		}
		numPredictSub++;
		//cout<<i<<"   1\n";
		//window_number_predict һ����8��
		//���¶���һ��Ԥ�⺯����Ҫ�����顣
		// 
		// ׼�� LSTM ģ�͵���������
		vector<float> input_data;
		for (int j = 0; j < window_number_predict; j++) {
			input_data.push_back(static_cast<float>((*(pd->countlist))[i][begin + j]));
		}
		//vector<double> countList(window_number_predict); //Ԥ�⺯������Ҫ�õ�����ʷ���ݡ� ��ParallelData�����const vector<vector<int>>* countlistһ����
		////int begin = history_window_number > 8 ? history_window_number - 8 : 0;//ֻ�������8������ΪArima��Ϊ9��ʱ�������8��ʱ�����ð�������ʷ���ݶ������ˣ����Դӵ��߸����ڿ�ʼ�����ˡ�
		////int window_number_predict = history_window_number > 8 ? 8 : history_window_number; // ���ֻ�����8�����ڽ���Ԥ��

		//for (int j = 0; j < window_number_predict; j++) {
		//	//���counList[j]��һά���顣��һ����ά���鸳��һά���飬���԰ɣ� const vector<vector<int>>* countlist
		//	countList[j] = (double)(*(pd->countlist))[i][begin + j];// i�Ƕ��ģ�begin+j Ԥ����Ҫ����ʷ���ݡ���double��ǿ��ת��Ϊ�˸�ֵ��
		//}
		//cout <<"predict"<<i<< "sub\n";
		//ָ�����飬vector<vector<int>>* predict_countlist

		// ʹ�� LSTM ģ�ͽ���Ԥ��
		vector<int> lstm_predictions = predictWithLoadedModel(lstm_model, input_data, pd->stepNum);

		// �洢 LSTM ģ�͵�Ԥ����
		(*p_countlist)[i] = lstm_predictions;
	}

	//printf("delete\n"); fflush(stdout);
	//delete (*(pd->predict_countlist)); //ɾ��һ��ָ�롣
	//printf("set\n"); fflush(stdout);
	//*(pd->predict_countlist) = p_countlist;
	printf("Prediction thread ends, %d subs were predicted.\n", numPredictSub);
	fflush(stdout);
	*(pd->state) = 2; // Ԥ�����
	return;
}


//Ԥ���ǲ���Ӧ��д��ƥ��֮����ΪҪ����ƥ������ݡ�
//limitnum �����ã�
//Gain �еĶ��ģ�����̰���㷨���е�����
//��Ϊ���������Ƕ����ģ���̬��
//���˾��Ƕ�̬���£�������һ�ε��Ľ�����и���
//��һ��ֻ�ǳ�ʼ����������


int myRein::change(const vector<IntervalSub>& subList, const vector<vector<int>>* predict_countlist, vector<vector<int>>& changedSub, int cstep, double matchingtime)  //adjustment time window
{
	unordered_set<int> newChangedSub;
	//unordered_map<int, bool> newChangedDirection;
	int changenum = 0, totalShouldChange = 0, limitnum = subnum / 100;   //����һ������1%     //maxcount=0;    what's the subnum?   subnum >1billion,
	int numAction = 0; //�Ѿ�ȷ��Ҫ���Ķ��ĸ���
	bool stopflag = false;                                                                        //  what's the stopflag? 
	if (!firstchange)                                                                          // firstchange = true
	{
		limitnum = (int)(matchingtime * cstep * limitscale / adjustcost);           //cstep: adjustment time window       //adjust limit
		//limitnum = 0.5 * cstep;
	}

	Timer t;
	/*for(int i=0;i<subnum;++i){
		if(countlist[i]>maxcount){
			maxcount=countlist[i];
		}
	}*/

	// ���Ƕ����ж��Ķ�Ҫ�������Ƕ� sort(Gain.begin(), Gain.end(),greater<int>())�����˳����е�������Ҫ�����ж�������
	//vector<vector<int>>* predict_countlist = predict(countlist, stepNum, cstep); //���countlist ��ÿ��ʱ�䴰���ڣ�ÿ������ƥ�䵽�¼��Ķ��١�
	//vector<double>Gain(subnum, 0);     //sublist subnum subs ���Ǳ�ʾ���ĸ�����   ���0�����ʼ��Ϊ0
	double r = 1;
	double z = pow(r, 0) + pow(r, 1) + pow(r, 2);
	//pair<double, int>���棬double:Gain ;int: subID;
	//vector<pair<int, int>> Gain;
	vector<pair<int, int>> Gain;
	//���Ƕ����еĶ��Ķ�Ҫ���м������棬�Թ��˹��Ķ��Ľ��м��㡣
	//��r��ȡֵҲ���е��ԡ�
	int minGainValue = 0;
	for (int i = 0; i < subnum; ++i) // ������������ж��Ķ���ǰsubnum��λ����
	{
		//(predict_countlist)[i].size() �е�size,���ĵĶ��٣���(predict_countlist)�������Ǳ������
		if ((*predict_countlist)[i].size() > 0&&lastChangedSub.count(i)==0) { // ���������Ԥ���� 
			/*double gain = (pow(r, 0) * ((max(0, (*predict_countlist)[i][0] - countlist[i][*historyWindowNumberWhenPredict - 1])) +
				(max(0.0, (*predict_countlist)[i][0] - 0.1 * cstep)))
				+ pow(r, 1) * ((max(0, (*predict_countlist)[i][1] - countlist[i][*historyWindowNumberWhenPredict - 1])) +
					(max(0.0, (*predict_countlist)[i][1] - 0.1 * cstep)))
				+ pow(r, 2) * ((max(0, (*predict_countlist)[i][2] - countlist[i][*historyWindowNumberWhenPredict - 1])) +
					(max(0.0, (*predict_countlist)[i][2] - 0.1 * cstep)))) / 2.7;*/
			/*double gain = (pow(r, 0) * (*predict_countlist)[i][0] +
				pow(r, 1) * (*predict_countlist)[i][1] +
				pow(r, 2) * (*predict_countlist)[i][2]) / z;*/
			// �ಽ����
			int gain = 0.0;
			for (int j = 0; j < stepNum; j++)
				gain += (*predict_countlist)[i][j];
			gain = gain / z;

			//if (gain >= minGainValue)
				Gain.push_back({ gain, i });
		}
		//- 3 * (limitnum * adjustcost)
		//pow��r, 0)* (countlist[i] - pre_4_countlist[i]) + pow��r, 0)* (countlist[i] - pre_5_countlist[i])-
		//��һ������1��Goal[i], 2��++Goal[i] ����Goal[i++],������ɶ��һ����
	}
	//֮ǰ��ע�͵��ģ���֪��ΪɶҪע�͵�
	sort(Gain.begin(), Gain.end(), [](const pair<double, int>& a, const pair<double, int>& b) {return a.first > b.first; });

	/*for (int i = 0; i < Gain.size(); i++) {
		printf("sub %ld gains %lf; ", Gain[i].second, Gain[i].first); fflush(stdout);
	}*/

	//�����Ź���֮��ģ�����  ˳�򣬽��е�����
	int numSameLevel = 0;
	for (int i = 0; i < Gain.size(); ++i) {
		//int level=newlevel-1-(int)floor((double)countlist[i]/(maxcount+1e-6)*newlevel);
	    //int level=originlevel[i]-(int)round((double)countlist[i]/cstep*originlevel[i]);
		// countlist[Gain[i].second][*historyWindowNumberWhenPredict - 1]  (*predict_countlist)[i][0]
		//int level = (int)round(pow(1 - (double)Gain[i].first / cstep, 2) * originlevel[Gain[i].second]);// (newLevel-1)
		int level = mapTable[originlevel[Gain[i].second]][Gain[i].first];
		int oldlevel = subtolevel[Gain[i].second];
		int distancelevel = abs(level - oldlevel);
		//bool direction = level < oldlevel;
		// �ϴ�û�� ���� �ϴε��˵�����ͬ�������
		//if (lastChangedSub.count(i) == 0 || direction == lastChangeDirection[i]) {
			/*if(countlist[i]==0){
				level=oldlevel+1;
				if(level>=newlevel)
					level=oldlevel;
			}*/
			//if(level == oldlevel) continue;

			if (distancelevel == 0 || totalShouldChange >= limitnum) {
				if (distancelevel == 0)
					numSameLevel++;
				//printf("Origin: %d, old: %d, new: %d\n", originlevel[Gain[i].second], oldlevel, level);
				continue;// �ڶ������Ӧ��break��
			}
			changeaction tmp;
			tmp.id = Gain[i].second;
			tmp.oldlevel = oldlevel;
			tmp.newlevel = level;
			changelist[distancelevel].push_back(tmp);    //���� sort(Gain.begin(), Gain.end(),greater<int>())�����˳����е�����
			++totalShouldChange; //�����ı���
		//}
	}
	/*vector<int> &_Lv = LvSize[0];
	int l=_Lv.size();
	int num=0;
	for(int j=0;j<l;++j)
		if(_Lv[j]!=-1)
			++num;
	cout<<"level zero: "<<l<<' '<<num<<endl;*/
	//cout<<"maxcount: "<<maxcount<<endl;
	//windowcontent += to_string(totalshouldchange) +"\t";

	for (int i = newlevel - 1; i > 0; --i) {
		//cout<<i<<' ';
		int l = changelist[i].size();
		//cout<<l<<endl;
		for (int ii = 0; ii < l; ++ii) {

			changeaction& action = changelist[i][ii];
			int id = action.id, oldlevel = action.oldlevel, level = action.newlevel;
			int indexId;
			if (emptymark[level].empty()) {
				indexId = LvSize[level].size();                                                       // indexId=LvSize[level].size()
				LvSize[level].push_back(id);
			}
			else {
				indexId = emptymark[level].back();
				emptymark[level].pop_back();
				LvSize[level][indexId] = id;
			}
			int oldindexId = -1, buckStep = LvBuckStep[level], oldbuckStep = LvBuckStep[oldlevel];          //    oldindexId=-1��ɶ��˼��  ȥ��-1Ϊɶ�ͳ���
			subtolevel[id] = level;
			checkLevel[id][window_number] = level;
			IntervalSub sub = subList[id];

			for (int j = 0; j < sub.size; ++j) {
				IntervalCnt cnt = sub.constraints[j];
				vector<kCombo>::iterator ed = data[oldlevel][cnt.att][0][cnt.lowValue / oldbuckStep].end();
				//bool fl=false;
				for (vector<kCombo>::iterator it = data[oldlevel][cnt.att][0][cnt.lowValue / oldbuckStep].begin(); it != ed; ++it)
					if (it->subID == id) {
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


				kCombo c;
				c.val = cnt.lowValue;
				c.subID = id;
				c.indexId = indexId;
				data[level][cnt.att][0][c.val / buckStep].push_back(c);
				c.val = cnt.highValue;
				data[level][cnt.att][1][c.val / buckStep].push_back(c);
			}
			LvSize[oldlevel][oldindexId] = -1;
			emptymark[oldlevel].push_back(oldindexId);

			changedSub[window_number].push_back(id);
			changedSub[window_number].push_back(oldlevel);
			changedSub[window_number].push_back(level); // Bug: ��level����newlevel
			LvSubNum[oldlevel]--;
			LvSubNum[level]++;
			++changenum;
			newChangedSub.insert(id);
			//newChangedDirection[id] = level < oldlevel;
			if (changenum >= limitnum) {  // ǰ���Ѿ����ƺ��ˣ�changenum�����ܴ���limitnum
				stopflag = true;
				break;
			}
		}
		if (stopflag)break;
	}
	lastChangedSub = newChangedSub;
	//lastChangeDirection = newChangedDirection;

	printf("Window %d, gainSize: %d, numSameLevel: %d, totalShouldChange: %d, modify: %d, limitnum: %d", *historyWindowNumberWhenPredict, Gain.size(), numSameLevel, totalShouldChange, changenum, limitnum);
	fflush(stdout);
	if (changenum) {
		adjustcost = (double)t.elapsed_nano() / 1000000 / changenum;
		firstchange = false;
		printf(", avgModifyTime: %lfms", adjustcost);
		fflush(stdout);
	}
	printf("\n\n");
	fflush(stdout);
	//�Ե�ǰʱ�䴰�ڣ���ն����б�
	for (int i = newlevel - 1; i > 0; --i)
		changelist[i].clear();
	*state = 0; // ������ϣ����Կ�ʼ��һ��Ԥ����
	//windowcontent += to_string(changenum) +"\n";
	return changenum;
}


int myRein::coldChange(const vector<IntervalSub>& subList, vector<vector<int>>& changedSub, double matchingtime) {
	int changenum = 0, totalShouldChange = 0, limitnum = subnum / 100, numSameLevel = 0;   //maxcount=0;    what's the subnum?   subnum >1billion,
	bool stopflag = false;                                               //  what's the stopflag? 
	if (!firstchange)                                                    // firstchange = true      ȡ��false ��������� rein ����ֲ�����
	{
		limitnum = (int)(matchingtime * cstep * limitscale / adjustcost);//cstep: adjustment time window  , adjust limit
	}
	Timer t;

	for (int i = 0; i < subnum; ++i) {
		//int level = (int)round(pow(1 - (double)countlist[i][window_number - 1] / cstep, 2) * originlevel[i]);
		int level = mapTable[originlevel[i]][countlist[i][window_number - 1]];
		int oldlevel = subtolevel[i];
		/*if (level >= 10) {
			cout << "level=" << level << endl;
			cout << countlist[window_number - 1][i] << endl;
			cout << pow(1 - (double)countlist[window_number - 1][i] / cstep, 2) << endl;
		}*/

		/*if(countlist[i]==0){
			level=oldlevel+1;
			if(level>=newlevel)
				level=oldlevel;
		}*/

		//if(level == oldlevel) continue;
		int distancelevel = abs(level - oldlevel);
		if (distancelevel == 0) {
			numSameLevel++;
			continue;                                //��������ľ���Ϊ1�����ҵ����ĸ���>limitnum, Ҳ�������ѭ����
		}
		else if (distancelevel == 1 && changelist[1].size() >= limitnum) continue;//vector<changeaction> changelist[MAX_CONS];1 ����distancelevel=1,  
		changeaction tmp;
		tmp.id = i;
		tmp.oldlevel = oldlevel;
		tmp.newlevel = level;
		changelist[distancelevel].push_back(tmp);
		++totalShouldChange;
	}
	/*vector<int> &_Lv = LvSize[0];
	int l=_Lv.size();
	int num=0;
	for(int j=0;j<l;++j)
		if(_Lv[j]!=-1)
			++num;
	cout<<"level zero: "<<l<<' '<<num<<endl;*/

	for (int i = newlevel - 1; i > 0; --i) {                             //newlevel-1����0��ʼ������̰���㷨�� �ӿ�ȸߵĵ���ȵ͵ĵ���������distancelevel ̰�� �ݹ�                                     

		int l = changelist[i].size();
		//cout<<l<<endl;
		for (int ii = 0; ii < l; ++ii) {

			changeaction& action = changelist[i][ii];
			int id = action.id, oldlevel = action.oldlevel, level = action.newlevel;                    //���id����Ҫ�����Ķ��ĵ�id. ���newlevel�����ܲ����Ǹ�newlevel.
			int indexId;           //���indexId �Ǵ�0��ʼ����

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

				kCombo c;
				c.val = cnt.lowValue;
				c.subID = id;
				c.indexId = indexId;
				data[level][cnt.att][0][c.val / buckStep].push_back(c);
				c.val = cnt.highValue;
				data[level][cnt.att][1][c.val / buckStep].push_back(c);
			}
			LvSize[oldlevel][oldindexId] = -1;
			emptymark[oldlevel].push_back(oldindexId);
			changedSub[window_number].push_back(id), changedSub[window_number].push_back(oldlevel), changedSub[window_number].push_back(level);
			++changenum;
			if (changenum >= limitnum) {
				stopflag = true;
				break;
			}
		}
		if (stopflag)break;
	}

	printf("Window %d, numSameLevel: %d, totalShouldChange: %d, modify: %d, limitnum: %d", *historyWindowNumberWhenPredict, numSameLevel, totalShouldChange, changenum, limitnum);
	fflush(stdout);
	if (changenum) {
		adjustcost = (double)t.elapsed_nano() / 1000000 / changenum;
		firstchange = false;
		printf(", avgModifyTime: %lfms", adjustcost);
		fflush(stdout);
	}
	printf("\n\n");
	fflush(stdout);
	//�Ե�ǰʱ�䴰�ڣ���ն����б�
	for (int i = newlevel - 1; i > 0; --i)
		changelist[i].clear();
	*state = 0; // ������ϣ����Կ�ʼ��һ��Ԥ����
	//windowcontent += to_string(changenum) +"\n";
	return changenum;
}
//����ÿһ�㶩�ĵĸ�����
void myRein::check() {
	for (int i = 0; i < newlevel; ++i) {
		vector<int>& _Lv = LvSize[i];
		int l = _Lv.size();
		int num = 0;
		for (int j = 0; j < l; ++j)
			if (_Lv[j] != -1)
				++num;
		cout << i << ' ' << l << ' ' << num << endl;
	}
}

