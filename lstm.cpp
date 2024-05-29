// lstm.h

#include "lstm.h"
//using namespace std;

LSTMModel::LSTMModel(int64_t input_size, int64_t hidden_size, int64_t output_size) {
    // LSTM 层
    lstm = register_module("lstm", torch::nn::LSTM(torch::nn::LSTMOptions(input_size, hidden_size)));
    // 全连接层
    linear = register_module("linear", torch::nn::Linear(hidden_size, output_size));
}



torch::Tensor LSTMModel::forward(torch::Tensor x) {
    // LSTM 层
    auto lstm_output = lstm->forward(x);
    // 提取 LSTM 序列的最后一个输出
    auto last_output = std::get<0>(lstm_output);
    // 全连接层
    return linear->forward(last_output);
}



//LSTMModel::LSTMModel(int64_t input_size, int64_t hidden_size, int64_t output_size) {
//    lstm = register_module("lstm", torch::nn::LSTM(input_size, hidden_size));
//    linear = register_module("linear", torch::nn::Linear(hidden_size, output_size));
//}
//
//torch::Tensor LSTMModel::forward(torch::Tensor x) {
//    auto lstm_output = lstm->forward(x);
//    auto last_output = std::get<0>(lstm_output);
//    return linear->forward(last_output);
//}



//
//void LSTMModel::trainModel(const std::vector<float>& input_data, const std::vector<float>& target_data, int epochs, float learning_rate) {
//    torch::optim::Adam optimizer(parameters(), torch::optim::AdamOptions(learning_rate));
//    torch::Tensor inputs = torch::tensor(input_data, torch::dtype(torch::kFloat32).requires_grad(false)).reshape({ 1,  static_cast<long long>(input_data.size()), 1 });
//    torch::Tensor targets = torch::tensor(target_data, torch::dtype(torch::kFloat32).requires_grad(false)).reshape({ static_cast<long long>(target_data.size()), 1 });
//
//    for (int epoch = 0; epoch < epochs; ++epoch) {
//        optimizer.zero_grad();
//        torch::Tensor predictions = forward(inputs);
//        torch::Tensor loss = torch::mse_loss(predictions, targets);
//        loss.backward();
//        optimizer.step();
//
//        if (epoch % 100 == 0)
//            std::cout << "Epoch: " << epoch + 1 << ", Loss: " << loss.item<float>() << std::endl;
//    }
//}
//
//void LSTMModel::predictWithModel(const std::vector<float>& input_data) {
//    torch::Tensor inputs = torch::tensor(input_data, torch::dtype(torch::kFloat32).requires_grad(false)).view({ 1,  static_cast<long long>(input_data.size()), 1 });
//    eval();
//    torch::Tensor predictions = forward(inputs);
//
//    std::cout << "Predictions: ";
//    for (int i = 0; i < predictions.size(0); ++i) {
//        std::cout << i << " ";
//        std::cout << predictions[i].item<float>() << std::endl;
//    }
//    std::cout << std::endl;
//}
//
//void LSTMModel::save_model(const std::string& model_save_path) {
//    torch::serialize::OutputArchive output_model_archive;
//    to(torch::kCPU);
//    save(output_model_archive);
//    output_model_archive.save_to(model_save_path);
//
//    std::cout << "\n================================\n"
//        << "    Model saved as:\n   "
//        << model_save_path << "\n\n";
//}
//
//void LSTMModel::load_model(const std::string saved_path) {
//    torch::serialize::InputArchive input_archive;
//    input_archive.load_from(saved_path);
//    load(input_archive);
//
//    std::cout << "\n================================\n"
//        << "    Model loaded from:\n   "
//        << saved_path << "\n\n";
//}
