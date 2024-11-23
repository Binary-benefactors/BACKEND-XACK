#include <crow.h>
#include <fstream>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>

// Глобальные переменные для хранения статусов задач и результатов
std::unordered_map<std::string, std::string> task_status;
std::unordered_map<std::string, std::string> task_results;
std::mutex task_mutex;

// Генерация уникального ID для задачи
std::string generate_task_id() {
    static int id = 0;
    return std::to_string(++id);
}

// Имитация обработки видео
void process_video_task(const std::string& task_id, const std::string& file_path) {
    // Задержка для эмуляции обработки (замените своей логикой обработки видео)
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Обновление статуса задачи и сохранение результата
    std::lock_guard<std::mutex> lock(task_mutex);
    task_status[task_id] = "completed";
    task_results[task_id] = file_path; // Укажите путь к обработанному видео
}

int main() {
    crow::SimpleApp app;

    // Роут для загрузки видео
    CROW_ROUTE(app, "/upload").methods("POST"_method)([] (const crow::request& req) {
        // Генерация ID задачи
        auto task_id = generate_task_id();

        // Сохранение файла на диск
        std::string file_path = "./uploads/" + task_id + ".mp4";
        std::ofstream file(file_path, std::ios::binary);
        if (!file) {
            return crow::response(500, "Failed to save file");
        }
        file.write(req.body.c_str(), req.body.size());
        file.close();

        // Инициализация статуса задачи
        {
            std::lock_guard<std::mutex> lock(task_mutex);
            task_status[task_id] = "processing";
        }

        // Запуск асинхронной обработки
        std::thread(process_video_task, task_id, file_path).detach();

        return crow::response(200, task_id);
    });

    // Роут для проверки статуса задачи и получения результата
    CROW_ROUTE(app, "/result/<string>").methods("GET"_method)([] (const crow::request& req, std::string task_id) {
        std::lock_guard<std::mutex> lock(task_mutex);

        // Проверка наличия задачи
        if (task_status.find(task_id) == task_status.end()) {
            return crow::response(404, "Task ID not found");
        }

        // Проверка статуса задачи
        if (task_status[task_id] == "processing") {
            return crow::response(202, "Task is still processing");
        }

        // Возврат обработанного файла
        std::ifstream file(task_results[task_id], std::ios::binary);
        if (!file.is_open()) {
            return crow::response(500, "Error reading processed video");
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        return crow::response(200, ss.str());
    });

    // Запуск приложения
    app.port(8080).multithreaded().run();

    return 0;
}
