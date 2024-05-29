#include <torch/torch.h>
#include <iostream>
#include <vector>

using namespace torch::nn;

// ����һ���򵥵� LSTM ģ��
//class LSTMModel : public torch::nn::Module {
//	torch::nn::LSTM lstm{ nullptr };
//	torch::nn::Linear linear{ nullptr };
//
//public:
//	//LSTMModel(int64_t input_size, int64_t hidden_size, int64_t output_size) {
//	//	// LSTM ��
//	//	lstm = register_module("lstm", torch::nn::LSTM(torch::nn::LSTMOptions(input_size, hidden_size)));
//
//	//	// ȫ���Ӳ�
//	//	linear = register_module("linear", torch::nn::Linear(hidden_size, output_size));
//	//}
//
//	//torch::Tensor forward(torch::Tensor x) {
//	//	// LSTM ��
//	//	auto lstm_output = lstm->forward(x);
//
//	//	// ��ȡ LSTM ���е����һ�����
//	//	auto last_output = std::get<0>(lstm_output)[-1];
//
//	//	// ȫ���Ӳ�
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



//// ѵ��ģ��
//void trainModel(LSTMModel& model, const std::vector<float>& input_data, const std::vector<float>& target_data, int epochs, float learning_rate) {
//	torch::optim::Adam optimizer(model.parameters(), torch::optim::AdamOptions(learning_rate));
//	torch::Tensor inputs = torch::tensor(input_data, torch::dtype(torch::kFloat32).requires_grad(false)).reshape({ 1, 6, 1 });
//	torch::Tensor targets = torch::tensor(target_data, torch::dtype(torch::kFloat32).requires_grad(false)).reshape({ 6, 1 });
//	for (int epoch = 0; epoch < epochs; ++epoch) {
//		// ת�������Ŀ������Ϊ PyTorch Tensor
//		//torch::Tensor inputs = torch::from_blob(input_data.data(), { 1, input_data.size(), 1 });
//		//torch::Tensor targets = torch::from_blob(target_data.data(), { 1, target_data.size(), 1 });
//		// �����ݶ�Ϊ��
//		optimizer.zero_grad();
//
//		// ǰ�򴫲�
//		torch::Tensor predictions = model.forward(inputs);
//
//		// ������ʧ
//		torch::Tensor loss = torch::mse_loss(predictions, targets);
//
//		// ���򴫲����Ż�
//		loss.backward();
//		optimizer.step();
//
//		// ��ӡ��ʧ
//		if (epoch % 100 == 0)
//			std::cout << "Epoch: " << epoch + 1 << ", Loss: " << loss.item<float>() << std::endl;
//	}
//}
//
//// ʹ��ģ�ͽ���Ԥ��
//void predictWithModel(LSTMModel& model, const std::vector<float>& input_data) {
//	// ת����������Ϊ PyTorch Tensor
//	torch::Tensor inputs = torch::tensor(input_data, torch::dtype(torch::kFloat32).requires_grad(false)).view({ 1, 6, 1 });
//
//	// ����ģ��Ϊ����ģʽ
//	model.eval();
//
//	// ǰ�򴫲�
//	torch::Tensor predictions = model.forward(inputs);
//
//	// ��ӡԤ����
//	//std::cout << predictions[0].item<float>() << std::endl;
//
//	// ��ӡԤ����
//	std::cout << "Predictions: ";
//	for (int i = 0; i < predictions.size(0); ++i) {
//		std::cout << i << " ";
//		std::cout << predictions[i].item<float>() << std::endl;
//	}
//	std::cout << std::endl;
//}
//
//
//// ����ģ����Ҫ�����л�Ȼ���ٱ���
//void save_model(LSTMModel& myModel, const std::string& model_save_path) {
//	torch::serialize::OutputArchive output_model_archive;
//	myModel.to(torch::kCPU);  // ��ģ���ƶ��� CPU
//	myModel.save(output_model_archive);
//	output_model_archive.save_to(model_save_path);
//
//	std::cout << "\n================================\n"
//		<< "    Model saved as:\n   "
//		<< model_save_path << "\n\n";
//}
////�ָ�ģ��Ҳ��Ҫ�����л��ٻָ�
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
//	// ����ģ�Ͳ���
//	const int64_t input_size = 1;
//	const int64_t hidden_size = 8;
//	const int64_t output_size = 1;
//
//	// ���� LSTM ģ��
//	LSTMModel lstm_model(input_size, hidden_size, output_size);
//
//	// ����ѵ�����ݺ�Ŀ������
//	std::vector<float> input_data = { 2, 4, 8, 6, 2, 22 };
//	std::vector<float> target_data = { 4, 8, 6, 2, 22, 34 };
//
//	// ѵ��ģ��
//	trainModel(lstm_model, input_data, target_data, 3000, 0.03);
//
//	// ʹ��ģ�ͽ���Ԥ��
//	predictWithModel(lstm_model, input_data);
//
//	//����ģ��
//	std::string path = "model.pth";
//	//torch::save(lstm_model, path);
//	save_model(lstm_model, path);
//
//
//	// ���� LSTMModel ���ʵ�� loaded_model
//	LSTMModel loaded_model(input_size, hidden_size, output_size);
//
//	// ʹ�� torch::load ���ļ�����Ԥѵ����ģ�Ͳ���
//	load_model(loaded_model, path);
//
//
//
//	return 0;
//}

