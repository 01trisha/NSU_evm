import matplotlib.pyplot as plt

def plot_from_txt(filepaths, labels, title="График зависимости тактов от размера массива"):
    """
    Строит график зависимости тактов процессора от размера массива из txt файлов с точками на графике.

    :param filepaths: Список путей к файлам с данными. Формат данных в файле: (размер массива; такты процессора).
    :param labels: Список меток для каждой линии на графике.
    :param title: Заголовок графика.
    """
    if len(filepaths) != len(labels):
        raise ValueError("Количество файлов должно совпадать с количеством меток.")

    plt.figure(figsize=(12, 8))

    for filepath, label in zip(filepaths, labels):
        sizes = []
        cycles = []

        # Читаем данные из txt файла
        with open(filepath, 'r') as file:
            for line in file:
                # Проверяем, что строка не пустая и содержит ';'
                if not line or ';' not in line:
                    continue
                # Убираем скобки и разделяем значения
                line = line.strip().strip("()")
                size, cycle = map(float, line.split(";"))
                #sizes.append(size/128 if size/128 < 1024 else size/128/1024)
                sizes.append(size/128)
                cycles.append(cycle)

        # Генерируем равномерно распределенные индексы для оси X
        indices = list(range(len(sizes)))

        # Добавляем линию на график
        line = plt.plot(indices, cycles, label=label)

        # Добавляем точки на график
        plt.scatter(indices, cycles, color=line[0].get_color(), s=50, label= None)

        # Указываем реальные значения размеров массива в качестве меток оси X
        if len(indices) > 1:  # Если данных много, используем каждое 10-е значение
            xtick_indices = indices[::1]
            xtick_labels = [int(sizes[i]) for i in xtick_indices]
        else:
            xtick_indices = indices
            xtick_labels = [int(size) for size in sizes]

        plt.xticks(ticks=xtick_indices, labels=xtick_labels, rotation=45)

    # Настройка осей
    plt.xlabel("Размер массива", fontsize=14)
    plt.ylabel("Такты процессора", fontsize=14)
    plt.title(title, fontsize=16)
    plt.legend(fontsize=12)
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)

    # Настройка тиков оси Y
    max_cycles = max(max(cycles) for filepath in filepaths)
    yticks = list(range(0, int(max_cycles + 10), 10))  # Шаг 10 для оси Y
    plt.yticks(ticks=yticks, labels=yticks)

    # Отображение графика
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    filepaths = ["streightMass.txt", "reverseMass.txt", "randomMass.txt"]
    labels = ["Прямой обход", "Обратный обход", "Случайный обход"]
    plot_from_txt(filepaths, labels)
