from ultralytics import YOLO

# Загрузка модели YOLO
model = YOLO("yolo11m.pt")
# print(model.cfg)
# Экспорт модели в ONNX с динамическими размерами и упрощением графа
model.export(format='onnx', dynamic=False, simplify=True, )
