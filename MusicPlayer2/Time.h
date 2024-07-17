#pragma once
class Time
{
public:
    unsigned int negative : 1;
    unsigned int min:15;
    unsigned int sec:6;
    unsigned int msec:10;

    Time()
        :negative{}, min{}, sec{}, msec{}
    {}

    Time(int _min, int _sec, int _msec)
        : negative{}, min{ static_cast<unsigned int>(_min) }, sec{ static_cast<unsigned int>(_sec) }, msec{ static_cast<unsigned int>(_msec) }
    {}

    Time(int time)
    {
        fromInt(time);
    }

    ~Time()
    {}

    //将int类型的时间（毫秒数）转换成Time结构
    void fromInt(int time)
    {
        if (time < 0) {
            negative = 1;
            time = -time;
        } else {
            negative = 0;
        }
        msec = time % 1000;
        sec = time / 1000 % 60;
        min = time / 1000 / 60;
    }

    //将Time结构转换成int类型（毫秒数）
    int toInt() const
    {
        int t = msec + sec * 1000 + min * 60000;
        return negative ? -t : t;
    }

    bool operator>(const Time& time) const
    {
        if (negative != time.negative)
            return (negative < time.negative);
        else if (min != time.min)
            return (min > time.min);
        else if (sec != time.sec)
            return(sec > time.sec);
        else if (msec != time.msec)
            return(msec > time.msec);
        else return false;
    }

    bool operator<(const Time& time) const
    {
        return time > *this;
    }

    bool operator==(const Time& time) const
    {
        return (negative == time.negative && min == time.min && sec == time.sec && msec == time.msec);
    }
    
    bool operator!=(const Time& time) const
    {
        return !(*this == time);
    }

    bool operator>=(const Time& time) const
    {
        if (negative != time.negative)
            return (negative < time.negative);
        else if (min != time.min)
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
        if (negative == time.negative) {
            int t = (min - time.min) * 60000 + (sec - time.sec) * 1000 + (msec - time.msec);
            return negative ? -t : t;
        } else {
            int t = (min + time.min) * 60000 + (sec + time.sec) * 1000 + (msec + time.msec);
            return negative ? -t : t;
        }
    }

    //加法赋值运算符，用于在当前时间上加上一个int类型的毫秒数
    Time operator+=(int time)
    {
        int added = this->toInt();
        added += time;
        this->fromInt(added);
        return *this;
    }

    Time operator-=(int time)
    {
        return operator+=(-time);
    }

    //加法运算符，用于在当前时间上加上一个int类型的毫秒数，返回Time对象
    Time operator+(int time) const
    {
        int added = this->toInt();
        added += time;
        return Time{ added };
    }

    //将时间转换成字符串（格式：分:秒）
    wstring toString(bool no_zero = true) const
    {
        wchar_t buff[16]{};
        if (no_zero && *this == Time{ 0,0,0 })
            wcscpy_s(buff, L"-:--");
        else
            swprintf_s(buff, L"%d:%.2d", min, sec);
        return wstring(buff);
    }

    //将时间转换成字符串（格式：分:秒.毫秒）
    wstring toString2(bool no_zero = true) const
    {
        wchar_t buff[16]{};
        if (no_zero && *this == Time{ 0,0,0 })
            wcscpy_s(buff, L"-:--");
        else
            swprintf_s(buff, L"%d:%.2d.%.3d", min, sec, msec);
        return wstring(buff);
    }

    //将时间转换成字符串（格式：时:分:秒）
    wstring toString3(bool no_zero = true) const
    {
        int hour, min1;
        hour = min / 60;
        min1 = min % 60;
        wchar_t buff[16]{};
        if (no_zero && *this == Time{ 0,0,0 })
            wcscpy_s(buff, L"-:--:--");
        else
            swprintf_s(buff, L"%d:%.2d:%.2d", hour, min1, sec);
        return wstring(buff);
    }

    wstring toLyricTimeTag() const
    {
        wchar_t buff[16]{};
        swprintf_s(buff, L"[%.2d:%.2d.%.2d]", min, sec, msec / 10);
        return wstring(buff);
    }

    wstring toVttTimeTag() const
    {
        int hour, min1;
        hour = min / 60;
        min1 = min % 60;
        if (hour > 99)
            return L"99:99:99.999";
        wchar_t buff[16]{};
        swprintf_s(buff, L"%.2d:%.2d:%.2d.%.3d", hour, min1, sec, msec);
        return wstring(buff);
    }

    //判断时间是否为0
    bool isZero() const
    {
        return (min == 0 && sec == 0 && msec == 0);
    }
};

