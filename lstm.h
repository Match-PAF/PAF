#include <torch/torch.h>
#include <iostream>
#include <vector>

using namespace torch::nn;

// 定义一个简单的 LSTM 模型
//class LSTMModel : public torch::nn::Module {
//	torch::nn::LSTM lstm{ nullptr };
//	torch::nn::Linear linear{ nullptr };
//
//public:
//	//LSTMModel(int64_t input_size, int64_t hidden_size, int64_t output_size) {
//	//	// LSTM 层
//	//	lstm = register_module("lstm", torch::nn::LSTM(torch::nn::LSTMOptions(input_size, hidden_size)));
//
//	//	// 全连接层
//	//	linear = register_module("linear", torch::nn::Linear(hidden_size, output_size));
//	//}
//
//	//torch::Tensor forward(torch::Tensor x) {
//	//	// LSTM 层
//	//	auto lstm_output = lstm->forward(x);
//
//	//	// 提取 LSTM 序列的最后一个输出
//	//	auto last_output = std::get<0>(lstm_output)[-1];
//
//	//	// 全连接层
//	//	return linear->forward(last_output);
//	//}
//
//	LSTMModel(int64_t input_size, int64_t hidden_size, int64_t output_size);
//	torch::Tensor forward(torch::Tensor x);
//	void trainModel(const std::vector<float>& input_data, const std::vector<float>& target_data, int epochs, float learning_rate);
//	void predictWithModel(const std::vector<float>& input_data);
//	void save_model(const std::string& model_save_path);
//	void load_model(const std::string saved_path);
//
//};

// LSTMModel.h
#ifndef LSTM_MODEL_H
#define LSTM_MODEL_H

//#include <torch/torch.h>

class LSTMModel : public torch::nn::Module {
	torch::nn::LSTM lstm{ nullptr };
	torch::nn::Linear linear{ nullptr };

public:
	LSTMModel(int64_t input_size, int64_t hidden_size, int64_t output_size);

	torch::Tensor forward(torch::Tensor x);
};

#endif // LSTM_MODEL_H



//// 训练模型
//void trainModel(LSTMModel& model, const std::vector<float>& input_data, const std::vector<float>& target_data, int epochs, float learning_rate) {
//	torch::optim::Adam optimizer(model.parameters(), torch::optim::AdamOptions(learning_rate));
//	torch::Tensor inputs = torch::tensor(input_data, torch::dtype(torch::kFloat32).requires_grad(false)).reshape({ 1, 6, 1 });
//	torch::Tensor targets = torch::tensor(target_data, torch::dtype(torch::kFloat32).requires_grad(false)).reshape({ 6, 1 });
//	for (int epoch = 0; epoch < epochs; ++epoch) {
//		// 转换输入和目标数据为 PyTorch Tensor
//		//torch::Tensor inputs = torch::from_blob(input_data.data(), { 1, input_data.size(), 1 });
//		//torch::Tensor targets = torch::from_blob(target_data.data(), { 1, target_data.size(), 1 });
//		// 设置梯度为零
//		optimizer.zero_grad();
//
//		// 前向传播
//		torch::Tensor predictions = model.forward(inputs);
//
//		// 计算损失
//		torch::Tensor loss = torch::mse_loss(predictions, targets);
//
//		// 反向传播和优化
//		loss.backward();
//		optimizer.step();
//
//		// 打印损失
//		if (epoch % 100 == 0)
//			std::cout << "Epoch: " << epoch + 1 << ", Loss: " << loss.item<float>() << std::endl;
//	}
//}
//
//// 使用模型进行预测
//void predictWithModel(LSTMModel& model, const std::vector<float>& input_data) {
//	// 转换输入数据为 PyTorch Tensor
//	torch::Tensor inputs = torch::tensor(input_data, torch::dtype(torch::kFloat32).requires_grad(false)).view({ 1, 6, 1 });
//
//	// 设置模型为评估模式
//	model.eval();
//
//	// 前向传播
//	torch::Tensor predictions = model.forward(inputs);
//
//	// 打印预测结果
//	//std::cout << predictions[0].item<float>() << std::endl;
//
//	// 打印预测结果
//	std::cout << "Predictions: ";
//	for (int i = 0; i < predictions.size(0); ++i) {
//		std::cout << i << " ";
//		std::cout << predictions[i].item<float>() << std::endl;
//	}
//	std::cout << std::endl;
//}
//
//
//// 保存模型需要先序列化然后再保存
//void save_model(LSTMModel& myModel, const std::string& model_save_path) {
//	torch::serialize::OutputArchive output_model_archive;
//	myModel.to(torch::kCPU);  // 将模型移动到 CPU
//	myModel.save(output_model_archive);
//	output_model_archive.save_to(model_save_path);
//
//	std::cout << "\n================================\n"
//		<< "    Model saved as:\n   "
//		<< model_save_path << "\n\n";
//}
////恢复模型也需要先序列化再恢复
//void load_model(LSTMModel& myModel, const std::string saved_path) {
//	// Load model state
//
//	torch::serialize::InputArchive input_archive;
//	input_archive.load_from(saved_path);
//	myModel.load(input_archive);
//	std::cout << "\n================================\n"
//		<< "    model loaded from:\n   "
//		<< saved_path << "\n\n";
//}

//void load_model(MyModel& net, const std::string& saved_path) {
//	// Load model state
//	torch::serialize::InputArchive input_archive;
//	input_archive.load_from(saved_path);
//	net.load(input_archive);
//	std::cout << "\n================================\n"
//		<< "    Model loaded from:\n   "
//		<< saved_path << "\n\n";
//}

//int main() {
//	std::cout << "CUDA : " << torch::cuda::is_available() << std::endl;
//	// 定义模型参数
//	const int64_t input_size = 1;
//	const int64_t hidden_size = 8;
//	const int64_t output_size = 1;
//
//	// 创建 LSTM 模型
//	LSTMModel lstm_model(input_size, hidden_size, output_size);
//
//	// 定义训练数据和目标数据
//	std::vector<float> input_data = { 2, 4, 8, 6, 2, 22 };
//	std::vector<float> target_data = { 4, 8, 6, 2, 22, 34 };
//
//	// 训练模型
//	trainModel(lstm_model, input_data, target_data, 3000, 0.03);
//
//	// 使用模型进行预测
//	predictWithModel(lstm_model, input_data);
//
//	//保存模型
//	std::string path = "model.pth";
//	//torch::save(lstm_model, path);
//	save_model(lstm_model, path);
//
//
//	// 创建 LSTMModel 类的实例 loaded_model
//	LSTMModel loaded_model(input_size, hidden_size, output_size);
//
//	// 使用 torch::load 从文件加载预训练的模型参数
//	load_model(loaded_model, path);
//
//
//
//	return 0;
//}

