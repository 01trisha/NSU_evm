#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
//для очереди с двумя концами
#include <deque>
//для суммирования эл-в контейнера
#include <numeric> 

using namespace cv;
using namespace std;

int main(int argc, char *argv[]){
    //поток с первой попавшейся камеры
    VideoCapture cap(0);
    //создаем матрицы img для хранения изображения
    Mat img;
    //кол-во кадров
    int FrameCount = 0; //кол-во кадров
    //общее время работы обработки
    double totalFrameTime = 0.0;
    //кол-во кадров, обрабатываемых в секунду
    double FPS = 0.0; 
    //среднее время ввода видеоданных
    double avgReadTime = 0.0;
    //среднее время преобразования изображения
    double avgConverTime = 0.0;
    //среднее время показа изображения 
    double avgShowTime = 0.0; 
    //общее время ввода видеоданных
    double totalReadTime = 0.0;
     //общее время преобразования изображения
    double totalConverTime = 0.0;
    //общее время показа изображения
    double totalShowTime = 0.0;
     //общее время WaitKey
    double totalWaitTime = 0.0;
   // Структуры timespec для измерения времени начала и конца каждого этапа обработки
    timespec startRead, endRead, startConvert, endConvert, startShow, endShow, startFrame, endFrame, startWait, endWait, startFPS, endFPS;

    // Скользящее окно для времени последних 15 кадров
    const int fpsWindowSize = 15;
    //очередь с двумя концами для хранения времен обработки последних кадров    
    deque<double> frameTimes;


    //время начала обработки кадров
    clock_gettime(CLOCK_MONOTONIC, &startFrame); 

    while(1){
        //время начала обработки 1 кадра
        clock_gettime(CLOCK_MONOTONIC, &startFPS);

        //время начала чтения кадра с камеры
        clock_gettime(CLOCK_MONOTONIC, &startRead);

        //чтение кадра с камеры
        cap >> img;

        //время конца чтения кадра с камеры
        clock_gettime(CLOCK_MONOTONIC, &endRead);
        //вычисляем время ввода видеоданных и добавление в общее время
        totalReadTime += 1000.0 * ((endRead.tv_sec - startRead.tv_sec) + (endRead.tv_nsec - startRead.tv_nsec) * 0.000000001);

        //время начала преобразования изображения
        clock_gettime(CLOCK_MONOTONIC, &startConvert);

        //изменение размера текущего кадра
        resize(img, img, Size(1500, 500));

        //поворот камеры на 180
        rotate(img, img, ROTATE_180);
        //переворот камеры по 0 - оси x, >0 по y, <0 по обеим
        flip(img, img, 1);

        // изменение яркости
        double alpha = 1.7; //контраст
        int beta = 50; //яркость пиксела
        img.convertTo(img, -1, alpha, beta);

        //время конца преобразования изобржения
        clock_gettime(CLOCK_MONOTONIC, &endConvert);
        //вычисляем время преобразования изображения и добавляем в общее
        totalConverTime += 1000.0 * ((endConvert.tv_sec - startConvert.tv_sec) + (endConvert.tv_nsec - startConvert.tv_nsec) * 0.000000001);

        //время начала показа изображения
        clock_gettime(CLOCK_MONOTONIC, &startShow);

        putText(img, "FPS: " + to_string(static_cast<int>(FPS)), Point(30, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
        //показ кадра(уже все изменили)
        imshow("test", img);

        //время конца показа изображения
        clock_gettime(CLOCK_MONOTONIC, &endShow);

        //вычисляем время показа изображения и добавляем в общее
        totalShowTime += 1000.0 * ((endShow.tv_sec - startShow.tv_sec) + (endShow.tv_nsec - startShow.tv_nsec) * 0.000000001);

        clock_gettime(CLOCK_MONOTONIC, &startWait);
        //прерывание при нажатии esc
        char c = waitKey(1);
        if (c == 27) break;

        clock_gettime(CLOCK_MONOTONIC, &endWait);
        totalWaitTime += 1000.0 * ((endWait.tv_sec - startWait.tv_sec) + (endWait.tv_nsec - startWait.tv_nsec) * 0.000000001);

        // Засекаем конец обработки кадра
        clock_gettime(CLOCK_MONOTONIC, &endFPS);
        double frameTime = 1000.0 * ((endFPS.tv_sec - startFPS.tv_sec) + (endFPS.tv_nsec - startFPS.tv_nsec) * 0.000000001);
        
        // Добавляем время обработки кадра в скользящее окно
        frameTimes.push_back(frameTime);
        
        //убираем старые прошлые кадры
        if (frameTimes.size() > fpsWindowSize) {
            frameTimes.pop_front();
        }

        // Рассчитываем FPS на основе среднего времени обработки последних 15 кадров
        double averageFrameTime = accumulate(frameTimes.begin(), frameTimes.end(), 0.0) / frameTimes.size();
        FPS = 1000.0 / averageFrameTime;

        ++FrameCount;

    }

    //время конца обработки кадров и вычисления общего времени
    clock_gettime(CLOCK_MONOTONIC, &endFrame);
    //общее время обработки кадров
    totalFrameTime = endFrame.tv_sec - startFrame.tv_sec + 0.000000001 * (endFrame.tv_nsec - startFrame.tv_nsec);

    //общее время суммами каждого действия (ввод ...)
    double sumTotalFrameTime = (totalReadTime + totalConverTime + totalShowTime + totalWaitTime);

    // Вычисление FPS
    //FPS = FrameCount / totalFrameTime;  
    //находим доли
    double ShareReadTime = (totalReadTime / (totalFrameTime * 1000.0)) * 100.0;
    double ShareConvertTime = (totalConverTime / (totalFrameTime * 1000.0)) * 100.0;
    double ShareShowTime = (totalShowTime / (totalFrameTime * 1000.0)) * 100.0;
    double ShareWaitTime = (totalWaitTime / (totalFrameTime * 1000.0)) * 100.0;
    //погрешность
    double FaultShare = 100 - ShareReadTime - ShareConvertTime - ShareShowTime - ShareWaitTime;
    //освобождение ресурсов (1 освобождение матрицы, 2 уничтожение окна)
    cap.release();
    destroyWindow("test");

    // Вывод результата
    //cout << fixed << setprecision(4);
    cout << "Доля времени ввода " << ShareReadTime << "%" <<endl;
    cout << "Доля времени преобразования " << ShareConvertTime << "%" <<endl;
    cout << "Доля показа " << ShareShowTime << "%" <<endl;
    cout << "Доля WaitKey " << ShareWaitTime << "%" <<endl;
    cout << "Время суммами общего времени: " << sumTotalFrameTime << " мс; Общее время работы цикла: " << totalFrameTime * 1000.0 << " мс;" << endl;
    cout << "Погрешность = " << FaultShare << "%" << endl;
    return 0;
}