clean:
	rm -rf g_omp g_serial out.txt

serial: g_serial.cpp
	g++ -std=c++11 g_serial.cpp -o g_serial

parallel: g_omp.cpp
	g++ -std=c++11 -fopenmp g_omp.cpp -o g_omp