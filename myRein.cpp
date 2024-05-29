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
	// 概率越大，层数越小
	level = (int)floor((log(poss) / log(0.1)) / (Dom)*newlevel);          //Dom is the number of interval constraints contained in s_j;  newlevel is the number of categories
	if (level < 0) level = 0;
	else if (level >= newlevel) level = newlevel - 1;                   //number of levels                           
	int indexId = LvSize[level].size(), buckStep = LvBuckStep[level];   // LvBuckStep[MAX_CONS];   LvSize:每一层订阅的个数   
	subtolevel[sub.id] = level;
	checkLevel[sub.id][0] = level;                                     //这个checkLevel是一个二维数组，第一维是订阅的id，第二维的0和1高值和低值
	checkLevel[sub.id][1] = level;
	LvSubNum[level]++;
	originlevel[sub.id] = level;
	for (int j = 0; j < sub.size; ++j) {
		IntervalCnt& cnt = sub.constraints[j];
		kCombo c;
		c.val = cnt.lowValue;
		c.subID = sub.id;
		c.indexId = indexId;                                            //这个indexID 为啥是 LvSize[level].size()
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

		vector<bool> bits(LvSize[siIndex].size(), false);  //Initializes a Boolean vector   这个bits的作用是啥？
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
		int b = _Lv.size();  //siIndex层 事件订阅的个数
		for (int i = 0; i < b; ++i)
			if (!bits[i] && _Lv[i] != -1)    // 空的是-1
			{
				matchDetailPub.push_back(t.elapsed_nano() / 1000000.0);
				++matchSubs;
				++countlist[_Lv[i]][window_number - 1]; //countlist:the number of events with which s_i matches
			}
	}

}

// Swhua1
vector<int> predict_arima(vector<double> countlist, int stepNum) {

	int n = countlist.size(), begin = 0;  //匹配到事件的订阅
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
	for (int i = 0; i < stepNum; i++) {     //stepNum：预测步数
		v += c;      // cstep: adjustment time window 多少个事件算一个窗口
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

// 加载模型
void load_model(LSTMModel& model, const std::string& path) {
	torch::serialize::InputArchive input_archive;
	input_archive.load_from(path);
	model.load(input_archive);
	std::cout << "Model loaded from: " << path << std::endl;
}


 //LSTM
 // 使用加载的模型进行预测
vector<int> predictWithLoadedModel(LSTMModel& loaded_model, const std::vector<float>& input_data, int stepNum) {
	// 创建一个 vector 以存储预测结果
	vector<int> preMatchEvent;

	// 设置预测窗口数
	int uplimit = stepNum;

	// 循环进行预测
	for (int l = 0; l < uplimit; l++) {
		// 将输入数据转换为 PyTorch Tensor
		torch::Tensor inputs = torch::tensor(input_data, torch::dtype(torch::kFloat32)).view({ 1, static_cast<long>(input_data.size()), 1 });

		// 设置模型为评估模式
		loaded_model.eval();

		// 使用加载的模型进行前向传播以获取预测结果
		torch::Tensor predictions = loaded_model.forward(inputs);

		// 将预测结果存储到 preMatchEvent 中
		for (int i = 0; i < predictions.size(1); ++i) {
			// 从 PyTorch Tensor 中提取预测值并转换为整数
			int predicted_value = static_cast<int>(predictions[0][i].item<float>());

			// 将预测值添加到结果 vector 中
			preMatchEvent.push_back(predicted_value);
		}
	}

	// 返回包含预测值的 vector
	return preMatchEvent;
}



//// 使用加载的模型进行多步预测
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





vector<int> predict_arimaA(vector<double> countlist, int stepNum)  //对每个订阅来说。
{
	//freopen("data.txt", "r", stdin);
	//double gets;
	//vector<double> countlist;
	//while (cin >> gets) {
	//    countlist.push_back(gets);
	//    cout << gets << endl;
	//}
	//函数里面数组怎么传递？
	ARIMAModel* arima = new ARIMAModel(countlist); //训练ARIMAModel 模型，new 是开辟一个新空间，利用coutlist 里面的历史数据训练模型。
	vector<vector<int>> list;                      //这个list 代表啥？
	vector<int> preMatchEvent;
	int modelcnt = 5;

	int period = 7;



	int uplimit = stepNum;// 预测窗口数
	for (int l = 0; l < uplimit; l++) {
		//cout << "Times: " << l << endl;
		int cnt = 0;
		vector<int> tmpPredict(modelcnt);  //每个窗口内临时预测modelCnt个？
		for (int k = 0; k < modelcnt; ++k)			//控制通过多少组参数进行计算最终的结果
		{
			vector<int> bestModel = arima->getARIMAModel(period, list, (k == 0) ? false : true);//if k==0,k=false; else k=true.
			//cout<<bestModel.size()<<endl;
			cnt++;
			if (bestModel.size() == 0)// 返回bestModel中元素的个数
			{
				tmpPredict[k] = (int)countlist[countlist.size() - period];   //为什么 countlist.size() - period？
				break;
			}
			else
			{
				//cout<<bestModel[0]<<bestModel[1]<<endl;
				int predictDiff = arima->predictValue(bestModel[0], bestModel[1], period);//predictDiff 表示啥？
				//cout<<"fuck"<<endl;
				tmpPredict[k] = arima->aftDeal(predictDiff, period);
				//cnt++;
			}
			//cout << bestModel[0] << " " << bestModel[1] << endl;
			list.push_back(bestModel); //list 数组里面存储得是bestModel?
		}

		double sumPredict = 0.0;
		//cout << "cnt=" << cnt << endl; // 预测了多少个值 
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

////一级指针是为了共享内存，二级指针是为了修改内存后还可以共享内存。
////让预测函数进行并行？这个并行得设计原理是什么？？ 
////考虑用双指针的原因：？？？
////void指针又名万能指针，在现在的很多程序中，当参数不确定时就用万能指针代替，这一类的指针在线程\进程函数里特别常见
//void predict(void* pld) {  //pld：传并行数据。 在predict函数里面传入了一个指针。
//	//cout << "PredictThread: Prediction Begin.\n";
//	ParallelData* pd = (ParallelData*)pld; //pd:指针类型转换，并且赋值。 这个加不加括号, 不可以（）表示强制类型转换。转换成相同类型，才能相互赋值。
//	//countlist是二维数组，n是行数，订阅的个数
//	int n = (*(pd->countlist)).size();  // 订阅个数，二维数组的size()是订阅得个数？最外层的是size，是行。 const vector<vector<int>>* countlist
//	//cout << "n=" << n << endl;
//	//cout << "ParallelData: " << &(*pd) << " predict_countlist" << *(pd->predict_countlist);
//	//vector<vector<int>>* p_countlist = new vector<vector<int>>;//这个赋值的作用？
//	//(*p_countlist).resize(n, vector<int>(pd->stepNum));
//	vector<vector<int>>* p_countlist = *(pd->predict_countlist);
//	//cout << "****************\n";
//	//delete* (pd->predict_countlist);
//	//windowNum 是ParallelData里面的成员属性，是当前窗口总数
//	int history_window_number = *(pd->windowNum); // history_window_number 所有的历史窗口总数。
//	int begin = history_window_number > 8 ? history_window_number - 8 : 0;//只用了最后8个，因为Arima里为9个时会出错，减8的时候，正好把所有历史数据都减掉了，所以从第七个窗口开始不调了。
//	int window_number_predict = history_window_number > 8 ? 8 : history_window_number; // 最多只拿最后8个窗口进行预测
//	int numPredictSub = 0;  // 真正进行预测了的订阅个数
//
//   //stepNum=3,  predict_countlist的容量重置n, 每一个元素为vector<int>(pd->stepNum)，其为列，三列，三个时间窗口。
//   //n行3列	
//	//存预测值
//	// 调整容器的大小为n，扩容后的每个元素的值为vector<int>(pd->stepNum)
//
//	//delete(*(pd->predict_countlist)); //删除一个指针。
//	//#pragma omp parallel for shared(countlist)                // num_threads(2) ordered  schedule(static,2)  //多线程？
//	//cout<<(*(pd->countlist)).size()<<endl;
//	//cout<<" predict_countlist: "<<(*((*pd).predict_countlist)).size()<<endl;
//	//cout << history_window_number << endl;
//	for (int i = 0; i < n; i++) {    // 尝试对每个订阅进行预测
//		//cout << i << " countlist: " << (*(pd->countlist))[i].size() << endl;
//		int fluctuation = abs((*(pd->countlist))[i][history_window_number - 3] - (*(pd->countlist))[i][history_window_number - 2])
//			+ abs((*(pd->countlist))[i][history_window_number - 2] - (*(pd->countlist))[i][history_window_number - 1]);
//
//		if ((*(pd->countlist))[i][history_window_number - 1] < 0.1 * cstep && fluctuation <= 0.3 * cstep) {  // 不需要预测   cstep代表每个时间窗口事件得个数，不应该是0.1*cstep作为阈值进行过滤。
//			(*p_countlist)[i].clear();
//			//printf("%d continue\n", i); fflush(stdout);
//			continue;
//		}
//		numPredictSub++;
//		//cout<<i<<"   1\n";
//		//window_number_predict 一定是8吗？
//		//重新定义一个预测函数需要的数组。
//		vector<double> countList(window_number_predict); //预测函数，需要用到得历史数据。 和ParallelData里面的const vector<vector<int>>* countlist一样吗？
//		//int begin = history_window_number > 8 ? history_window_number - 8 : 0;//只用了最后8个，因为Arima里为9个时会出错，减8的时候，正好把所有历史数据都减掉了，所以从第七个窗口开始不调了。
//		//int window_number_predict = history_window_number > 8 ? 8 : history_window_number; // 最多只拿最后8个窗口进行预测
//
//		for (int j = 0; j < window_number_predict; j++) {
//			//这个counList[j]是一维数组。把一个二维数组赋给一维数组，不对吧？ const vector<vector<int>>* countlist
//			countList[j] = (double)(*(pd->countlist))[i][begin + j];// i是订阅，begin+j 预测需要的历史数据。（double）强制转换为了赋值？
//		}
//		//cout <<"predict"<<i<< "sub\n";
//		//指针数组，vector<vector<int>>* predict_countlist
//
//		(*p_countlist)[i] = predict_arima(countList, stepNum); //传vector类型数组的时候，只用传数组名就可以，不用传大小。
//		//cout<<"Sub"<<i<<" predict done.\n";
//	}
//
//	//printf("delete\n"); fflush(stdout);
//	//delete (*(pd->predict_countlist)); //删除一个指针。
//	//printf("set\n"); fflush(stdout);
//	//*(pd->predict_countlist) = p_countlist;
//	printf("Prediction thread ends, %d subs were predicted.\n", numPredictSub);
//	fflush(stdout);
//	*(pd->state) = 2; // 预测完毕
//	return;
//}

void predict(void* pld) {  //pld：传并行数据。 在predict函数里面传入了一个指针。
	//cout << "PredictThread: Prediction Begin.\n";
	ParallelData* pd = (ParallelData*)pld; //pd:指针类型转换，并且赋值。 这个加不加括号, 不可以（）表示强制类型转换。转换成相同类型，才能相互赋值。
	//countlist是二维数组，n是行数，订阅的个数
	int n = (*(pd->countlist)).size();  // 订阅个数，二维数组的size()是订阅得个数？最外层的是size，是行。 const vector<vector<int>>* countlist
	//cout << "n=" << n << endl;
	//cout << "ParallelData: " << &(*pd) << " predict_countlist" << *(pd->predict_countlist);
	//vector<vector<int>>* p_countlist = new vector<vector<int>>;//这个赋值的作用？
	//(*p_countlist).resize(n, vector<int>(pd->stepNum));
	vector<vector<int>>* p_countlist = *(pd->predict_countlist);
	//cout << "****************\n";
	//delete* (pd->predict_countlist);
	//windowNum 是ParallelData里面的成员属性，是当前窗口总数
	int history_window_number = *(pd->windowNum); // history_window_number 所有的历史窗口总数。
	int begin = history_window_number > 8 ? history_window_number - 8 : 0;//只用了最后8个，因为Arima里为9个时会出错，减8的时候，正好把所有历史数据都减掉了，所以从第七个窗口开始不调了。
	int window_number_predict = history_window_number > 8 ? 8 : history_window_number; // 最多只拿最后8个窗口进行预测
	int numPredictSub = 0;  // 真正进行预测了的订阅个数


	// 加载 LSTM 模型
// 定义模型参数
	//const int64_t input_size = 1;//输入数据的特征维度
	//const int64_t hidden_size = 8;//隐藏层神经元的个数
	//const int64_t output_size = 1;//输出数据的特征维度
	LSTMModel lstm_model(input_size, hidden_size, output_size);
	std::string model_path = "D:/SCSLlibtorch/LSTM/LSTM/model.pth"; // 替换为您的模型路径
	load_model(lstm_model, model_path);
   //stepNum=3,  predict_countlist的容量重置n, 每一个元素为vector<int>(pd->stepNum)，其为列，三列，三个时间窗口。
   //n行3列	
	//存预测值
	// 调整容器的大小为n，扩容后的每个元素的值为vector<int>(pd->stepNum)

	//delete(*(pd->predict_countlist)); //删除一个指针。
	//#pragma omp parallel for shared(countlist)                // num_threads(2) ordered  schedule(static,2)  //多线程？
	//cout<<(*(pd->countlist)).size()<<endl;
	//cout<<" predict_countlist: "<<(*((*pd).predict_countlist)).size()<<endl;
	//cout << history_window_number << endl;
	for (int i = 0; i < n; i++) {    // 尝试对每个订阅进行预测
		//cout << i << " countlist: " << (*(pd->countlist))[i].size() << endl;
		int fluctuation = abs((*(pd->countlist))[i][history_window_number - 3] - (*(pd->countlist))[i][history_window_number - 2])
			+ abs((*(pd->countlist))[i][history_window_number - 2] - (*(pd->countlist))[i][history_window_number - 1]);

		if ((*(pd->countlist))[i][history_window_number - 1] < 0.1 * cstep && fluctuation <= 0.3 * cstep) {  // 不需要预测   cstep代表每个时间窗口事件得个数，不应该是0.1*cstep作为阈值进行过滤。
			(*p_countlist)[i].clear();
			//printf("%d continue\n", i); fflush(stdout);
			continue;
		}
		numPredictSub++;
		//cout<<i<<"   1\n";
		//window_number_predict 一定是8吗？
		//重新定义一个预测函数需要的数组。
		// 
		// 准备 LSTM 模型的输入数据
		vector<float> input_data;
		for (int j = 0; j < window_number_predict; j++) {
			input_data.push_back(static_cast<float>((*(pd->countlist))[i][begin + j]));
		}
		//vector<double> countList(window_number_predict); //预测函数，需要用到得历史数据。 和ParallelData里面的const vector<vector<int>>* countlist一样吗？
		////int begin = history_window_number > 8 ? history_window_number - 8 : 0;//只用了最后8个，因为Arima里为9个时会出错，减8的时候，正好把所有历史数据都减掉了，所以从第七个窗口开始不调了。
		////int window_number_predict = history_window_number > 8 ? 8 : history_window_number; // 最多只拿最后8个窗口进行预测

		//for (int j = 0; j < window_number_predict; j++) {
		//	//这个counList[j]是一维数组。把一个二维数组赋给一维数组，不对吧？ const vector<vector<int>>* countlist
		//	countList[j] = (double)(*(pd->countlist))[i][begin + j];// i是订阅，begin+j 预测需要的历史数据。（double）强制转换为了赋值？
		//}
		//cout <<"predict"<<i<< "sub\n";
		//指针数组，vector<vector<int>>* predict_countlist

		// 使用 LSTM 模型进行预测
		vector<int> lstm_predictions = predictWithLoadedModel(lstm_model, input_data, pd->stepNum);

		// 存储 LSTM 模型的预测结果
		(*p_countlist)[i] = lstm_predictions;
	}

	//printf("delete\n"); fflush(stdout);
	//delete (*(pd->predict_countlist)); //删除一个指针。
	//printf("set\n"); fflush(stdout);
	//*(pd->predict_countlist) = p_countlist;
	printf("Prediction thread ends, %d subs were predicted.\n", numPredictSub);
	fflush(stdout);
	*(pd->state) = 2; // 预测完毕
	return;
}


//预测是不是应该写在匹配之后，因为要利用匹配的数据。
//limitnum 在哪用？
//Gain 中的订阅，利用贪心算法进行调整，
//因为不调整就是定死的，静态；
//调了就是动态更新，根据上一次调的结果进行更新
//第一次只是初始化，冷启动


int myRein::change(const vector<IntervalSub>& subList, const vector<vector<int>>* predict_countlist, vector<vector<int>>& changedSub, int cstep, double matchingtime)  //adjustment time window
{
	unordered_set<int> newChangedSub;
	//unordered_map<int, bool> newChangedDirection;
	int changenum = 0, totalShouldChange = 0, limitnum = subnum / 100;   //限制一次最多调1%     //maxcount=0;    what's the subnum?   subnum >1billion,
	int numAction = 0; //已经确定要调的订阅个数
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

	// 不是对所有订阅都要遍历而是对 sort(Gain.begin(), Gain.end(),greater<int>())里面的顺序进行调整，需要对所有都遍历吗？
	//vector<vector<int>>* predict_countlist = predict(countlist, stepNum, cstep); //这个countlist 是每个时间窗口内，每个订阅匹配到事件的多少。
	//vector<double>Gain(subnum, 0);     //sublist subnum subs 都是表示订阅个数吗？   这个0代表初始化为0
	double r = 1;
	double z = pow(r, 0) + pow(r, 1) + pow(r, 2);
	//pair<double, int>里面，double:Gain ;int: subID;
	//vector<pair<int, int>> Gain;
	vector<pair<int, int>> Gain;
	//不是对所有的订阅都要进行计算收益，对过滤过的订阅进行计算。
	//对r的取值也进行调试。
	int minGainValue = 0;
	for (int i = 0; i < subnum; ++i) // 这里需假设所有订阅都在前subnum个位置上
	{
		//(predict_countlist)[i].size() 行的size,订阅的多少，对(predict_countlist)加括号是必须的吗？
		if ((*predict_countlist)[i].size() > 0&&lastChangedSub.count(i)==0) { // 这个订阅有预测结果 
			/*double gain = (pow(r, 0) * ((max(0, (*predict_countlist)[i][0] - countlist[i][*historyWindowNumberWhenPredict - 1])) +
				(max(0.0, (*predict_countlist)[i][0] - 0.1 * cstep)))
				+ pow(r, 1) * ((max(0, (*predict_countlist)[i][1] - countlist[i][*historyWindowNumberWhenPredict - 1])) +
					(max(0.0, (*predict_countlist)[i][1] - 0.1 * cstep)))
				+ pow(r, 2) * ((max(0, (*predict_countlist)[i][2] - countlist[i][*historyWindowNumberWhenPredict - 1])) +
					(max(0.0, (*predict_countlist)[i][2] - 0.1 * cstep)))) / 2.7;*/
			/*double gain = (pow(r, 0) * (*predict_countlist)[i][0] +
				pow(r, 1) * (*predict_countlist)[i][1] +
				pow(r, 2) * (*predict_countlist)[i][2]) / z;*/
			// 多步收益
			int gain = 0.0;
			for (int j = 0; j < stepNum; j++)
				gain += (*predict_countlist)[i][j];
			gain = gain / z;

			//if (gain >= minGainValue)
				Gain.push_back({ gain, i });
		}
		//- 3 * (limitnum * adjustcost)
		//pow（r, 0)* (countlist[i] - pre_4_countlist[i]) + pow（r, 0)* (countlist[i] - pre_5_countlist[i])-
		//这一步对吗？1、Goal[i], 2、++Goal[i] 或者Goal[i++],这俩有啥不一样。
	}
	//之前是注释掉的，不知道为啥要注释掉
	sort(Gain.begin(), Gain.end(), [](const pair<double, int>& a, const pair<double, int>& b) {return a.first > b.first; });

	/*for (int i = 0; i < Gain.size(); i++) {
		printf("sub %ld gains %lf; ", Gain[i].second, Gain[i].first); fflush(stdout);
	}*/

	//根据排过序之后的，订阅  顺序，进行调整。
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
		// 上次没调 或者 上次调了但是往同方向调的
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
				continue;// 第二种情况应该break？
			}
			changeaction tmp;
			tmp.id = Gain[i].second;
			tmp.oldlevel = oldlevel;
			tmp.newlevel = level;
			changelist[distancelevel].push_back(tmp);    //根据 sort(Gain.begin(), Gain.end(),greater<int>())里面的顺序进行调整。
			++totalShouldChange; //计数的变量
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
			int oldindexId = -1, buckStep = LvBuckStep[level], oldbuckStep = LvBuckStep[oldlevel];          //    oldindexId=-1是啥意思？  去掉-1为啥就出错。
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
			changedSub[window_number].push_back(level); // Bug: 是level不是newlevel
			LvSubNum[oldlevel]--;
			LvSubNum[level]++;
			++changenum;
			newChangedSub.insert(id);
			//newChangedDirection[id] = level < oldlevel;
			if (changenum >= limitnum) {  // 前面已经控制好了，changenum不可能大于limitnum
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
	//对当前时间窗口，清空动作列表。
	for (int i = newlevel - 1; i > 0; --i)
		changelist[i].clear();
	*state = 0; // 调整完毕，可以开始下一次预测了
	//windowcontent += to_string(changenum) +"\n";
	return changenum;
}


int myRein::coldChange(const vector<IntervalSub>& subList, vector<vector<int>>& changedSub, double matchingtime) {
	int changenum = 0, totalShouldChange = 0, limitnum = subnum / 100, numSameLevel = 0;   //maxcount=0;    what's the subnum?   subnum >1billion,
	bool stopflag = false;                                               //  what's the stopflag? 
	if (!firstchange)                                                    // firstchange = true      取反false 还能运行嘛？ rein 里面局部变量
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
			continue;                                //如果调整的距离为1，并且调整的个数>limitnum, 也结束这次循环。
		}
		else if (distancelevel == 1 && changelist[1].size() >= limitnum) continue;//vector<changeaction> changelist[MAX_CONS];1 代表distancelevel=1,  
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

	for (int i = newlevel - 1; i > 0; --i) {                             //newlevel-1，从0开始计数，贪心算法。 从跨度高的到跨度低的调整，根据distancelevel 贪心 递归                                     

		int l = changelist[i].size();
		//cout<<l<<endl;
		for (int ii = 0; ii < l; ++ii) {

			changeaction& action = changelist[i][ii];
			int id = action.id, oldlevel = action.oldlevel, level = action.newlevel;                    //这个id是需要调整的订阅得id. 这个newlevel不是总层数那个newlevel.
			int indexId;           //这个indexId 是从0开始的吗？

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
	//对当前时间窗口，清空动作列表。
	for (int i = newlevel - 1; i > 0; --i)
		changelist[i].clear();
	*state = 0; // 调整完毕，可以开始下一次预测了
	//windowcontent += to_string(changenum) +"\n";
	return changenum;
}
//计算每一层订阅的个数。
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

