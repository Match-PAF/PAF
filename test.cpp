//#include "rein.h"
//#include "oldrein.h"
//#include "myRein.h"
//
//using namespace std;
//int mai22n(int argc, char** argv)
//{
//
//	vector<double>countlist = { 3,6,4,6,5,5,3,5,3};
//	ARIMAModel* arima = new ARIMAModel(countlist); //训练ARIMAModel 模型，new 是开辟一个新空间，利用coutlist 里面的历史数据训练模型。
//	vector<vector<int>> list;                                                      //这个list 代表啥？
//	vector<int> preMatchEvent(stepNum);
//	int modelcnt = 5;
//
//	int period = 7;
//
//
//
//	int uplimit = stepNum;// 预测窗口数
//	for (int l = 0; l < uplimit; l++) {
//		cout << "Times: " << l << endl;
//		int cnt = 0;
//		vector<int> tmpPredict(modelcnt);  //每个窗口内临时预测modelCnt个？
//		for (int k = 0; k < modelcnt; ++k)			//控制通过多少组参数进行计算最终的结果
//		{
//			vector<int> bestModel = arima->getARIMAModel(period, list, (k == 0) ? false : true);//if k==0,k=false; else k=true.
//			//cout<<bestModel.size()<<endl;
//			cnt++;
//			if (bestModel.size() == 0)// 返回bestModel中元素的个数
//			{
//				tmpPredict[k] = (int)countlist[countlist.size() - period];   //为什么 countlist.size() - period？
//				break;
//			}
//			else
//			{
//				//cout<<bestModel[0]<<bestModel[1]<<endl;
//				int predictDiff = arima->predictValue(bestModel[0], bestModel[1], period);//predictDiff 表示啥？
//				//cout<<"fuck"<<endl;
//				tmpPredict[k] = arima->aftDeal(predictDiff, period);
//				//cnt++;
//			}
//			cout << bestModel[0] << " " << bestModel[1] << endl;
//			list.push_back(bestModel);//list 数组里面存储得是bestModel?
//		}
//
//	}
//	return 0;
//}


//#include<iostream>
//#include<string>
//#include <iomanip>
//#include <vector>
//#include <cmath>
//#include<set>
//#include<map>
//#include<queue>
//#include <stack>
//#include <thread>
//#include<algorithm>
//#define _for(i,a,b) for( int i=a; i<b; ++i)
//#define mfor(i,b,a) for(int i=b;i>=a;i--)
//
//using namespace std;
//std::thread::id main_thread_id = std::this_thread::get_id();
//
//void hello()
//{
//	std::cout << "Hello Concurrent World\n";
//	if (main_thread_id == std::this_thread::get_id())
//		std::cout << "This is the main thread.\n";
//	else
//		std::cout << "This is not the main thread.\n";
//}
//
//void pause_thread(int n) {
//	std::this_thread::sleep_for(std::chrono::seconds(n));
//	std::cout << "pause of " << n << " seconds ended\n";
//}
//
//
//int main() {
//	std::thread t(hello);
//	cout << main_thread_id << endl;
//	std::cout << t.hardware_concurrency() << std::endl;//可以并发执行多少个(不准确)
//	std::cout << "native_handle " << t.native_handle() << std::endl;//可以并发执行多少个(不准确)
//	t.join();
//	std::thread a(hello);
//	a.detach();
//	std::thread threads[5];                         // 默认构造线程
//
//	std::cout << "Spawning 5 threads...\n";
//	for (int i = 0; i < 5; ++i)
//		threads[i] = std::thread(pause_thread, i + 1);   // move-assign threads
//	std::cout << "Done spawning threads. Now waiting for them to join:\n";
//	for (auto& thread : threads)
//		thread.join(); // detach();
//	std::this_thread::sleep_for(std::chrono::seconds(3));
//	hello();
//	std::cout << "All threads joined!\n";
//
//	return 0;
//}

//
//#include<iostream>
//#include<string>
//#include <iomanip>
//#include <vector>
//#include <cmath>
//#include<set>
//#include<map>
//#include<queue>
//#include <stack>
//#include <thread>
//#include<algorithm>
//#define _for(i,a,b) for( int i=a; i<b; ++i)
//#define mfor(i,b,a) for(int i=b;i>=a;i--)
//
//using namespace std;
//std::thread::id main_thread_id = std::this_thread::get_id();
//
//class classA {
//private:
//	int a1;
//public:
//	int a2;
//	classA(int a, int b) :a1(a), a2(b) {
//
//	}
//
//	void test_classA() {
//		cout << "classA: " << a1 << " " << a2 << endl;
//	}
//
//	void multi_thread() {
//		thread t(&classA::test_classA, this);
//		t.join();
//	}
//};
//
//
//
//void hello()
//{
//	std::cout << "Hello Concurrent World\n";
//	if (main_thread_id == std::this_thread::get_id())
//		std::cout << "This is the main thread.\n";
//	else
//		std::cout << "This is not the main thread.\n";
//}
//
//void pause_thread(int n) {
//	std::this_thread::sleep_for(std::chrono::seconds(n));
//	std::cout << "pause of " << n << " seconds ended\n";
//}
//
//
//int main() {
//	//std::thread t(hello);
//	//cout << main_thread_id << endl;
//	//std::cout << t.hardware_concurrency() << std::endl;//可以并发执行多少个(不准确)
//	//std::cout << "native_handle " << t.native_handle() << std::endl;//可以并发执行多少个(不准确)
//	//t.join();
//	//std::thread a(hello);
//	//a.detach();
//	//std::thread threads[5];                         // 默认构造线程
//
//	//std::cout << "Spawning 5 threads...\n";
//	//for (int i = 0; i < 5; ++i)
//	//	threads[i] = std::thread(pause_thread, i + 1);   // move-assign threads
//	//std::cout << "Done spawning threads. Now waiting for them to join:\n";
//	//for (auto& thread : threads)
//	//	thread.join(); // detach();
//	//std::this_thread::sleep_for(std::chrono::seconds(3));
//	//hello();
//	//std::cout << "All threads joined!\n";
//
//	//classA A(44, 55);
//	//thread q(&classA::multi_thread, &A);
//	//q.join();
//
//	//classA * a=new classA(1,2);
//	//classA* b = new classA(3, 4);
//	//delete a;
//	//a = b;
//	//a->multi_thread();
//
//	int* a = new int;
//	*a = 111;
//	cout << *a;
//	return 0;
//}



//#include<iostream>
//#include<string>
//#include <iomanip>
//#include <vector>
//#include <cmath>
//#include<set>
//#include<map>
//#include<queue>
//#include <stack>
//#include <thread>
//#include<algorithm>
//using namespace std;
//void t(int** q) {
//	int* w = new int;
//	*w = 23;
//	delete *q;
//	*q = w;
//}
//
//void t2(int* q) {
//	int* w = new int;
//	*w = 23;
//	delete q;
//	q = nullptr;
//	q = w;
//}
//
//int mai() {
//	int** q = new int*;
//	*q = new int;
//	**q = 90;
//	t(q);
//	cout << **q;
//
//	int* q2 = new int;
//	*q2 = 90;
//	t2(q2);
//	cout << *q2;
//	return 0;
//}
//
//#include <thread>
//#include<iostream>
//#include<vector>
//#include<stdio.h>
//using namespace std;
//
//struct ParallelData
//{
//    int stepNum;
//    int cstep;
//    int* state;
//    int* windowNum; // 有效窗口数，countlist的前多少列
//    const vector<vector<int>>* countlist;
//    vector<vector<int>>** predict_countlist;
//};
//
//
//struct ParallelData2
//{
//    vector<vector<int>>** predict_countlist;
//    const vector<vector<int>>* countlist;
//    int* state;
//    int stepNum;
//    int cstep;
//    int* windowNum; // 有效窗口数，countlist的前多少列
//};
//
//void Predict(void* pld) {
//    ParallelData* pd = (ParallelData*)pld;
//    //printf("&pd=%d, pc=%d, *pc=%d\ncl=%d, *cl=%d\n",pd, &(pd->predict_countlist), pd->predict_countlist, &(pd->countlist), pd->countlist);
//    //fflush(stdout);
//    printf("Thread: pd= %d, pc= %d\n", (*(pd->countlist)).size(), (**(pd->predict_countlist)).size());
//    fflush(stdout);
//}
//
//void Predict2(void* pld) {
//    ParallelData2* pd = (ParallelData2*)pld;
//    //printf("&pd=%d, pc=%d, *pc=%d\ncl=%d, *cl=%d\n",pd, &(pd->predict_countlist), pd->predict_countlist, &(pd->countlist), pd->countlist);
//    //fflush(stdout);
//    printf("Thread2: pd= %d, pc= %d\n", (*(pd->countlist)).size(), (**(pd->predict_countlist)).size());
//    fflush(stdout);
//}
//
//int main() {
//
//    vector<vector<int>>** predict_countlist = new vector<vector<int>>*;
//    *predict_countlist = new vector<vector<int>>;
//    (**predict_countlist).resize(90);
//    vector<vector<int>> countlist(4, vector<int>(90));
//    int s = 9, c = 3;
//    int* st = new int; *st = 6;
//    int* w = new int; *w = 8;
//    ParallelData pd = { s,c,st,w, &countlist,predict_countlist };
//
//    printf("pc: %d, cl: %d, s: %d, c: %d, st: %d, w: %d, pd: %d\n", sizeof(predict_countlist), sizeof(countlist), sizeof(s), sizeof(c), sizeof(st), sizeof(w),sizeof(pd) );
//    fflush(stdout);
//    ParallelData2 pd2 = { predict_countlist,&countlist,st,s,c,w, };
//    printf("pc: %d, cl: %d, s: %d, c: %d, st: %d, w: %d, pd: %d\n", sizeof(predict_countlist), sizeof(countlist), sizeof(s), sizeof(c), sizeof(st), sizeof(w), sizeof(pd2));
//    fflush(stdout);
//
//   /* pd.predict_countlist = new vector<int>[10];
//    pd.countlist = new vector<vector<int>>(4);
//  */
//    //printf("&pd=%d, pc=%d, *pc=%d\ncl=%d, *cl=%d\n", &pd, &(pd.predict_countlist), pd.predict_countlist, &(pd.countlist), pd.countlist);
//    //fflush(stdout);
//    thread predict_thread(Predict, (void*)&pd);
//    predict_thread.detach();
//    int i;
//    cin >> i;
//    thread predict_thread2(Predict2, (void*)&pd2);
//    predict_thread2.detach();
//    cin >> i;
//	return 0;
//}