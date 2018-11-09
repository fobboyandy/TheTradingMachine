#include "../Indicators/Indicators.h"
#include <iostream>
#include <string>

template <typename T>
struct IndicatorTemplate
{
	template<typename... Params>
	IndicatorTemplate(Params... params):
		tPtr(std::make_shared<T>(params...))
	{
	}

	std::shared_ptr<T> tPtr;
};

template<typename T>
void test(IndicatorTemplate<T> it)
{
	//base case
}

template<typename T, typename Arg1, typename... Args>
void test(IndicatorTemplate<T> it, Arg1 arg1, Args... args)
{
	auto val = it.tPtr->computeIndicatorPoint(arg1);
	for (int i = 0; i < T::SIZE; ++i)
	{
		std::cout << val[i];
	}
	std::cout << std::endl;
	test(it, args...);
}

template<template<typename> typename T, typename B>
void test2(T<B> indicator)
{

}

template<typename T>
struct testStruct
{
	T x;
};

int main()
{
	test2(testStruct<int>());
	//test2(testStruct<int>());

	test(IndicatorTemplate<RelativeStrengthIndex>(5),
		81.591,
		81.062,
		82.873,
		83.004,
		83.615,
		83.15,
		82.84,
		83.99,
		84.55,
		84.36,
		85.53,
		86.54,
		86.89,
		87.77,
		87.29);


	system("pause");
	return 0;
}

/* copy paste playground

81.59
81.06
82.87
83.00
83.61
83.15	72.03
82.84	64.93
83.99	75.94
84.55	79.80
84.36	74.71
85.53	83.03
86.54	87.48
86.89	88.76
87.77	91.48
87.29

*/