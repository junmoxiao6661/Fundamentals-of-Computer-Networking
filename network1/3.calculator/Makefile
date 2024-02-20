.PHONY:all
all:servercal clientcal
Lib=-ljsoncpp
servercal:ServerCal.cc
	g++ -o $@ $^ -std=c++11 $(Lib)
clientcal:ClientCal.cc
	g++ -o $@ $^ -std=c++11 $(Lib)
.PHONY:clean
clean:
	rm -f servercal clientcal
