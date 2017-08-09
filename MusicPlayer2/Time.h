#pragma once
class Time
{
public:
	int min{};
	int sec{};
	int msec{};

	Time()
	{}

	Time(int _min, int _sec, int _msec) : min{_min}, sec{_sec}, msec{_msec}
	{}

	Time(int time)
	{
		int2time(time);
	}

	~Time()
	{}

	//将int类型的时间（毫秒数）转换成Time结构
	void int2time(int time)
	{
		msec = time % 1000;
		sec = time / 1000 % 60;
		min = time / 1000 / 60;
	}

	//将Time结构转换成int类型（毫秒数）
	int time2int() const
	{
		return msec + sec * 1000 + min * 60000;
	}

	bool operator>(const Time& time) const
	{
		if (min != time.min)
			return (min > time.min);
		else if (sec != time.sec)
			return(sec > time.sec);
		else if (msec != time.msec)
			return(msec > time.msec);
		else return false;
	}

	bool operator==(const Time& time) const
	{
		return (min == time.min && sec == time.sec && msec == time.msec);
	}
	
	bool operator!=(const Time& time) const
	{
		return !(*this == time);
	}

	bool operator>=(const Time& time) const
	{
		if (min != time.min)
			return (min > time.min);
		else if (sec != time.sec)
			return(sec > time.sec);
		else if (msec != time.msec)
			return(msec > time.msec);
		else return true;
	}

	//减法运算符，用于计算两个Time对象的时间差，返回int类型，单位为毫秒
	int operator-(const Time& time) const
	{
		return (min - time.min) * 60000 + (sec - time.sec) * 1000 + (msec - time.msec);
	}

	//加法赋值运算符，用于在当前时间上加上一个int类型的毫秒数
	Time operator+=(int time)
	{
		int added = this->time2int();
		added += time;
		this->int2time(added);
		return *this;
	}

	//加法运算符，用于在当前时间上加上一个int类型的毫秒数，返回Time对象
	Time operator+(int time) const
	{
		int added = this->time2int();
		added += time;
		return Time{ added };
	}

	//将时间转换成字符串（格式：分:秒）
	wstring time2str() const
	{
		wchar_t buff[16];
		if (*this == Time{ 0,0,0 })
			wcscpy_s(buff, L"-:--");
		else
			swprintf_s(buff, L"%d:%.2d", min, sec);
		return wstring(buff);
	}

	//将时间转换成字符串（格式：分:秒.毫秒）
	wstring time2str2() const
	{
		wchar_t buff[16];
		if (*this == Time{ 0,0,0 })
			wcscpy_s(buff, L"-:--");
		else
			swprintf_s(buff, L"%d:%.2d.%.3d", min, sec, msec);
		return wstring(buff);
	}

	//将时间转换成字符串（格式：时:分:秒）
	wstring time2str3() const
	{
		int hour, min1;
		hour = min / 60;
		min1 = min % 60;
		wchar_t buff[16];
		if (*this == Time{ 0,0,0 })
			wcscpy_s(buff, L"-:--:--");
		else
			swprintf_s(buff, L"%d:%.2d:%.2d", hour, min1, sec);
		return wstring(buff);
	}

	//判断时间是否为0
	bool isZero() const
	{
		return (min == 0 && sec == 0 && msec == 0);
	}
};

