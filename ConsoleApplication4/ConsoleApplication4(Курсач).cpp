#include "pch.h"
#include <cstdlib>
#include <locale>
#include <fstream> // для классов ifstream и ofstream
#include <iostream>
#include <opencv2/opencv.hpp>
//#include <opencv2/highgui.hpp>
//#include <stdlib.h>
//#include <stdio.h>
//#include <opencv2/core.hpp>
//#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/core/types.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
using namespace std;
// рассчитать хеш картинки
//_int64 calcImageHash(IplImage* image, bool show_results = false);
__int64 calcImageHash(cv::Mat src, bool show_results);
// рассчёт расстояния Хэмминга
__int64 calcHammingDistance(__int64 x, __int64 y);
//показывает промежуточные картинки
void SHOWME(cv::Mat res, cv::Mat gray, cv::Mat bin);
//заполняет текстовый файл ссылками
void OUTPUT();
//сохраняет кеш
void OUTPUT1(char img_name[100], __int64 hashI);
//сохраняет расстояние хемминга
void OUTPUT2(char img_name[100], __int64 dist);
int result = 0; //количество совпадений
int resultnotfull = 0; //количество частичных совпадений (контрастность, размер изображения)


//Измените следующие 4 ссылки. Текстовые файлы создаются автоматически. При каждом запуске программы они очищаются.

const char* Global_Link_imagetxt("C:\\Users\\HP\\Desktop\\image\\image.txt");  //текстовый файл с ссылками на изображения (имитация Базы данных)
const char* Global_Link_imageHASH("C:\\Users\\HP\\Desktop\\image\\image_hash.txt");  //возможность записи хеша изображения
const char* Global_Link_imageHAMMINGdist("C:\\Users\\HP\\Desktop\\image\\image_Hemming.txt");   //файл для рассчёта расстояния Хэмминга между двумя хэшама // http://en.wikipedia.org/wiki/Hamming_distance// http://ru.wikipedia.org/wiki/Расстояние_Хэмминга
const char* Global_Link_image_DIR("C:\\Users\\HP\\Desktop\\image\\"); //папка с изображениями


int main(int argc, char* argv[])
{
	OUTPUT(); //указываем месторасположение изображений + создаем файл
	cv::Mat object ,image;
		//  1 - Считываем ссылки

		fstream F;
		F.open(Global_Link_imagetxt, ios::in);
		if (!F.is_open())
		{
			cout << "Файл не может быть открыт! Проверьте расположение текстового файла в начале программы! error1\n";
			exit(0);
		}
		else
		{
			char img_name[100];
			char obj_name[100];
			F >> obj_name;
			// имя объекта задаётся первым параметром
			char* object_filename = argc >= 2 ? argv[1] : obj_name;
			//получаем картинку
			object = cv::imread(object_filename);
			//cout << obj_name;
			//построим кеш СРАВНИВАЕМОГО ИЗОБРАЖЕНИЯ
			__int64 hashO = calcImageHash(object, false);    //Возможность увидеть этапы выполнения программы изменением параметра false -> true

			char* image_filename;
			__int64 hashI;
			__int64 dist;
			while (!F.eof())
			{
				F >> img_name;
				// имя картинок задаётся вторым параметром
				image_filename = argc >= 3 ? argv[2] : img_name;
				image = cv::imread(image_filename);
				//строимкеш сравниваемых изображений
				hashI = calcImageHash(image, false);
				//сохраняем кеш
				OUTPUT1(img_name,hashI);
				//рассчитаем расстояние Хэмминга
				dist = calcHammingDistance(hashO, hashI);
				//std::printf("[i] Hamming distance: %I64d \n", dist);
				//сохраняем расстояние Хемминга
				OUTPUT2(img_name, dist);
			}
		}
		F.close();
	cout << "Найдено совпадений: " << result << endl;
	if (result > 0)
	{
		cout << "показать? (Y/N)" << endl;
		char* answer = new char[2];
		cin >> answer;
		if (*answer == L'Y'|| *answer == L'y')
		{
			F.open(Global_Link_imagetxt, ios::in);
			char img_name[100];
			char obj_name[100];
			F >> obj_name;
			object = cv::imread(obj_name);
			__int64 hashO = calcImageHash(object, false);
			cv::Mat img_name3;
			cvNamedWindow("ИСХОДНОЕ ИЗОБРАЖЕНИЕ", CV_WINDOW_AUTOSIZE);
			cv::resize(object, img_name3, cvSize(1280, 720));
			cv::imshow("ИСХОДНОЕ ИЗОБРАЖЕНИЕ", img_name3);
			__int64 hashI;
			__int64 dist;
			cv::Mat img_name2;
			while (!F.eof())
			{
				F >> img_name;
				image = cv::imread(img_name);
				hashI = calcImageHash(image, false);
				dist = calcHammingDistance(hashO, hashI);
				if (dist <= 10)
				{
					cvNamedWindow(img_name, CV_WINDOW_AUTOSIZE);
					img_name2 = cv::imread(img_name);
					cv::resize(img_name2, img_name3, cv::Size(1080, 720));
					cv::imshow(img_name, img_name3);
				}
			}
			F.close();
		}
		else
		{
			cout << answer << " - - > Далее" << endl;
		}
	}
	if (resultnotfull > 0)
	{
		cout << "Найдено частичных совпадений: " << resultnotfull << "небольшой шанс что это может быть ЧАСТЬ от исходной картинки. Показать? (Y/N)" << endl;
		char* answer = new char[2];
		cin >> answer;
		if (*answer == L'Y' || *answer == L'y')
		{
			F.open(Global_Link_imagetxt, ios::in);
			char img_name[100];
			char obj_name[100];
			F >> obj_name;
			object = cv::imread(obj_name);
			__int64 hashO = calcImageHash(object, false);
			cv::Mat img_name3;
			cvNamedWindow("ИСХОДНОЕ ИЗОБРАЖЕНИЕ", CV_WINDOW_AUTOSIZE);
			cv::resize(object, img_name3, cvSize(1280, 720));
			cv::imshow("ИСХОДНОЕ ИЗОБРАЖЕНИЕ", img_name3);
			__int64 hashI;
			__int64 dist;
			cv::Mat img_name2;
			while (!F.eof())
			{
				F >> img_name;
				image = cv::imread(img_name);
				hashI = calcImageHash(image, false);
				dist = calcHammingDistance(hashO, hashI);
				if (dist < 20 && dist > 10)
				{
					cvNamedWindow(img_name, CV_WINDOW_AUTOSIZE);
					img_name2 = cv::imread(img_name);
					cv::resize(img_name2, img_name3, cv::Size(1080, 720));
					cv::imshow(img_name, img_name3);
				}
			}
			F.close();
		}
		else
		{
			cout << answer << " - - > Завершение работы" << endl;
		}
	}
	else
	{
		cout <<" - - > Завершение работы" << endl;
	}
	// удаляем окна
	cvDestroyAllWindows();
	return 0;
}

void SHOWME(cv::Mat res, cv::Mat gray, cv::Mat bin)
{
		// увеличенные картинки для отображения результатов
		cv::Mat dst3(cvSize(128, 128), IPL_DEPTH_8U, 3);
		cv::Mat dst2(cvSize(128, 128), IPL_DEPTH_8U, 1);
		cv::Mat dst1(cvSize(128, 128), IPL_DEPTH_8U, 1);
		// показываем картинки
		cvNamedWindow("ИСХОДНОЕ СРАВНИВАЕМОЕ ИЗОБРАЖЕНИЕ");
		cv::resize(res, dst3, cvSize(128, 128));
		cv::imshow("ИСХОДНОЕ СРАВНИВАЕМОЕ ИЗОБРАЖЕНИЕ", dst3);
		cvNamedWindow("ОттенокСерого");
		cv::resize(gray, dst2, cvSize(128, 128));
		cv::imshow("ОттенокСерого", dst2);
		cvNamedWindow("черно-белое");
		cv::resize(bin, dst1, cvSize(128, 128));
		cv::imshow("черно-белое", dst1);
		cvWaitKey(0);
}
// рассчитать хеш картинки
__int64  calcImageHash(cv::Mat src, bool show_results)
{
	cv::Mat res(cvSize(8, 8), src.depth(), src.channels());     //SIZE (8,8)
	cv::Mat gray(cvSize(8, 8), IPL_DEPTH_8U, 1);                   //SIZE (8,8)
	cv::Mat bin(cvSize(8, 8), IPL_DEPTH_8U, 1);                   //SIZE (8,8)
	
	//SHOWME(src, "0", show_results);
	cv::resize(src, res, cvSize(8, 8));                              //SIZE (8,8)
	//SHOWME(res, "1", show_results);
	cv::cvtColor(res, gray, CV_BGR2GRAY);
	//SHOWME(gray, "2", show_results);
	cvWaitKey(0);
	//вычисляем среднее значение

	cv::Scalar average = cv::mean(gray);               //переделать под opencv v.3++ (after old opencv2) -- DONE
	//std::printf("[i] average: %.2f \n", average.val[0]);
	// получим бинарное изображение относительно среднего
	// для этого воспользуемся пороговым преобразованием
	cv::threshold(gray, bin, average.val[0], 250, CV_THRESH_BINARY);         //Переделать!!! найти новый cvThreshold  --  Работает
	//SHOWME(gray, "3", show_results);
	// построим хэш
	__int64 hash = 0;
	int i = 0;
	// пробегаемся по всем пикселям изображения
	for (int y = 0; y < bin.size().height; y++) {
		uchar* ptr = (uchar*)(bin.data + y * (bin.size().width*bin.channels()));         //ПОЧИНИТЬ!!! - DONE (в новой версии widthStep (шаги) = width * channels)
		for (int x = 0; x < bin.size().width; x++) {
			// 1 канал
			if (ptr[x])
			{
				hash |= 1i64 << i;
			}
			i++;
		}
	}
	//std::printf("[i] hash: %I64X \n", hash);
	if (show_results)
	{
		SHOWME(res, gray, bin);
	}
	return hash;
}

	__int64 calcHammingDistance(__int64 x, __int64 y)
	{
		__int64 dist = 0, val = x ^ y;

		// Count the number of set bits
		while (val)
		{
			++dist;
			val &= val - 1;
		}

		return dist;
	}

	void OUTPUT()
	{
		setlocale(LC_ALL, "RUSSIAN");

	//      1) - Заполняем текстовый редактор ссылками на image
		    ofstream deleteimageHASH(Global_Link_imageHASH, ios_base::trunc);
			if (!deleteimageHASH.is_open())
			{
				cout << "Файл куда записывается ХЕШ не может быть открыт! Проверьте расположение в начале программы! error2\n";
				exit(0);
			}
			ofstream deleteHAMMINGdist(Global_Link_imageHAMMINGdist, ios_base::trunc);
			if (!deleteHAMMINGdist.is_open())
			{
				cout << "Файл куда записывается hamming distance не может быть открыт! Проверьте расположение в начале программы!  error3\n";
				exit(0);
			}
			deleteimageHASH.close();
			deleteHAMMINGdist.close();
		ofstream fout(Global_Link_imagetxt, ios::trunc); // создаем объект класса ofstream и связываем его с image.txt
		if (!fout.is_open())
		{
			cout << "Файл не может быть открыт! (error4) Проверьте расположение в начале программы!\n";
			exit(0);
		}
		else
		{
			for (int i = 0; i < 100; i++)
			{
				fout << Global_Link_image_DIR << "image" << i << ".jpg" << endl; // делаем записи в файл
			}
			fout << Global_Link_image_DIR << "image100.jpg";
			fout.close(); // закрываем файл
		}
	}

	void OUTPUT1(char img_name[100], __int64 hashI)
	{
		setlocale(LC_ALL, "RUSSIAN");

		//      2) - Заполняем текстовый редактор ХЕШом изображений

		ofstream fout(Global_Link_imageHASH, ios_base::app); // создаем объект класса ofstream и связываем его с image.txt
		if (!fout.is_open())
		{
			cout << "Файл не может быть открыт! error5\n";
			exit(0);
		}
		else
		{
			fout << img_name << "  -- ХЕШ --  " << hashI << endl; // делаем записи в файл
			fout.close(); // закрываем файл
		}
	}
	void OUTPUT2(char img_name[100], __int64 dist)
	{
		setlocale(LC_ALL, "RUSSIAN");

		//      3) - Заполняем текстовый редактор расстоянием Хемминга

		ofstream fout(Global_Link_imageHAMMINGdist, ios_base::app); // создаем объект класса ofstream и связываем его с cpp.txt      //
		if (!fout.is_open())
		{
			cout << "Файл не может быть открыт!  error6\n";
			exit(0);
		}
		else
		{
			if (dist <= 10)
			{
				fout << img_name << "  -- расстояние Хемминга --  " << dist << "   ПОХОЖИЕ КАРТИНКИ   " << endl; // делаем записи в файл
				result++;
			}
			if (dist < 20 && dist > 10)
			{
				fout << img_name << "  -- расстояние Хемминга --  " << dist << " -  Небольшой шанс что это изображение - часть исходного" << endl; // делаем записи в файл
				resultnotfull++;
			}
		}
		fout.close(); // закрываем файл
	}
	