#pragma once


#include<thread>
#include<chrono>
#include<functional>

class Timer
{
	std::thread Thread;
	bool Alive = false; //boolean value for if timer is running
	long CallNumber = -1L; //how many times we want to run a function, L = long (does not become int)
	long repeat_count = -1L; //how many times a certain function has been called so far 
	std::chrono::milliseconds interval = std::chrono::milliseconds(0);
	std::function<void(void)> funct = nullptr;

	void SleepAndRun() {
		std::this_thread::sleep_for(interval);
		if (Alive) {
			Function()(); //double parentheses: 1st pair calls the function to return a function, second pair calls the function the 1st function returns
		}
	}
	void ThreadFunc() {
		if (CallNumber == Infinite) 
			while (Alive)
				SleepAndRun();
		else
			while (repeat_count--)
				SleepAndRun();
	}

public:
	static const long Infinite = -1L;
	Timer() {}
	Timer(const std::function<void(void)>& f) :funct(f) {}
	Timer(const std::function<void(void)>& f, const unsigned long& i, const long repeat = Timer::Infinite) 
		:funct(f), interval(std::chrono::milliseconds(i)), CallNumber(repeat) {}

	//if timer is alive or not 
	bool IsAlive() const{
		return Alive;
	}

	//starts the timer
	void Start(bool Async = true) {
		if (IsAlive())
			return;
		Alive = true;
		repeat_count = CallNumber;
		//this is creating the Thread we initialized above
		if (Async) {
			Thread = std::thread(&Timer::ThreadFunc, this);
		}
		else
			ThreadFunc();
	}
	//manually stops the timer
	void Stop() {
		Alive = false;
		Thread.join();
	}

	//sets how many times it is going to repeat
	void RepeatCount(const long r) {
		if (!IsAlive())
			CallNumber = r;
	}
	//sets the function to be executed
	void SetFunction(std::function<void(void)>& f) {
		funct = f;
	}
	//returns how many iterations are left
	long GetLeftCount() const{
		return repeat_count;
	}
	//returns how many iterations we had originally requested
	long RepeatCount() const{
		return CallNumber;
	}
	void SetInterval(long unsigned int& i) {
		if (!IsAlive())
			interval = std::chrono::milliseconds(i);
	}
	//.count() converts from chrono milliseconds to long
	unsigned long Interval() const{
		return interval.count();
	}

	const std::function<void(void)>& Function() const{
		return funct;
	}

};

