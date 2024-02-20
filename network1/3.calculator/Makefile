.PHONY:all
all:servercal clientcal

servercal:ServerCal.cc
	g++ -o $@ $^ -std=c++11 
clientcal:ClientCal.cc
	g++ -o $@ $^ -std=c++11
.PHONY:clean
clean:
	rm -f servercal clientcal
