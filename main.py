from pathlib import Path
import ultralytics
import cv2

# Загрузка модели через PyTorch
model_path = "yolov11m-face.pt"  # Убедитесь, что модель доступна
if not Path(model_path).is_file():
    raise FileNotFoundError(f"Модель {model_path} не найдена.")

model = ultralytics.YOLO(model_path)

# Загрузка видеофайла
video_path = "v4.mp4"
if not Path(video_path).is_file():
    raise FileNotFoundError(f"Видео {video_path} не найдено.")

cap = cv2.VideoCapture(video_path)
if not cap.isOpened():
    print("Не удалось открыть видеофайл")
    exit()

# Настройка для сохранения видео
output_path = "outputv4.mp4"
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
fps = cap.get(cv2.CAP_PROP_FPS)
width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
out = cv2.VideoWriter(output_path, fourcc, fps, (width, height))

frame_counter = 0
n_skip = 3
process_next_frame = True
last_blur_coords = []

while True:
    ret, frame = cap.read()
    if not ret:
        print("Конец видео или ошибка чтения кадра")
        break

    frame_counter += 1

    if process_next_frame or frame_counter % n_skip == 0:
        results = model.predict(frame, show=False, save=False)

        mobs = []
        for r in results:
            for c in r.boxes:
                mobs.append([int(c.xyxy[0][0]), int(c.xyxy[0][1]), int(c.xyxy[0][2]), int(c.xyxy[0][3])])

        print(mobs)

        if mobs:
            process_next_frame = True  # Обнаружены объекты, обрабатываем следующий кадр
            last_blur_coords = mobs  # Обновляем последние координаты
            for det in mobs:
                x1, y1, x2, y2 = det
                # Применение блюра к области детекции
                roi = frame[y1:y2, x1:x2]
                blurred_roi = cv2.GaussianBlur(roi, (51, 51), 30)
                frame[y1:y2, x1:x2] = blurred_roi
        else:
            process_next_frame = False  # Нет объектов, пропускаем следующий кадр
            # Используем последние координаты для блюра
            for det in last_blur_coords:
                x1, y1, x2, y2 = det
                roi = frame[y1:y2, x1:x2]
                blurred_roi = cv2.GaussianBlur(roi, (51, 51), 30)
                frame[y1:y2, x1:x2] = blurred_roi
    else:
        for det in last_blur_coords:
            x1, y1, x2, y2 = det
            roi = frame[y1:y2, x1:x2]
            blurred_roi = cv2.GaussianBlur(roi, (51, 51), 30)
            frame[y1:y2, x1:x2] = blurred_roi

    # Запись обработанного кадра в выходное видео
    out.write(frame)

# Освобождение ресурсов
cap.release()
out.release()
cv2.destroyAllWindows()

print(f"Обработанное видео сохранено в {output_path}")
