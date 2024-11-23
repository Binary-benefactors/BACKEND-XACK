#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/opencv/cv_image.h>
#include <dlib/gui_widgets.h>
#include <iostream>

using namespace std;
using namespace cv;
using namespace dlib;

int main() {
    // Открываем камеру
    VideoCapture video(0);
    if (!video.isOpened()) {
        cerr << "Ошибка: Не удалось открыть камеру!" << endl;
        return -1;
    }

    // Создаём детектор лиц и загрузчик модели ключевых точек
    frontal_face_detector detector = get_frontal_face_detector();
    shape_predictor predictor;
    try {
        deserialize("shape_predictor_68_face_landmarks.dat") >> predictor;
    } catch (const serialization_error& e) {
        cerr << "Ошибка: Не удалось загрузить модель ключевых точек!" << endl;
        return -1;
    }

    Mat frame;
    while (true) {
        // Читаем кадр с камеры
        video >> frame;
        if (frame.empty()) {
            cerr << "Ошибка: Не удалось получить кадр с камеры!" << endl;
            break;
        }

        // Конвертируем изображение в формат Dlib
        cv_image<bgr_pixel> cimg(frame);

        // Обнаруживаем лица
        std::vector<rectangle> faces = detector(cimg);

        // Для каждого обнаруженного лица
        for (const auto& face : faces) {
            // Находим ключевые точки
            full_object_detection shape = predictor(cimg, face);

            // Рисуем прямоугольник вокруг лица
            rectangle(frame, Point(face.left(), face.top()), Point(face.right(), face.bottom()), Scalar(0, 255, 0), 2);

            // Рисуем ключевые точки
            for (int i = 0; i < shape.num_parts(); ++i) {
                Point pt(shape.part(i).x(), shape.part(i).y());
                circle(frame, pt, 2, Scalar(0, 0, 255), -1);
            }
        }

        // Отображаем результат
        imshow("Web", frame);

        // Ждём нажатия клавиши (1 мс задержка)
        if (waitKey(1) == 27) { // Если нажата клавиша 'Esc'
            break;
        }
    }

    return 0;
}
