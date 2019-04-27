#include "Easing.hpp"

double Easing::InQuad(double t, double totaltime, double max, double min)
{
    max -= min;
    t /= totaltime;
    return max * t * t + min;
}

double Easing::OutQuad(double t, double totaltime, double max, double min)
{
    max -= min;
    t /= totaltime;
    return -max * t * (t - 2) + min;
}

double Easing::InOutQuad(double t, double totaltime, double max, double min)
{
    max -= min;
    t /= totaltime;
    if(t / 2 < 1) return max / 2 * t * t + min;
    --t;
    return -max * (t * (t - 2) - 1) + min;
}

double Easing::InCubic(double t, double totaltime, double max, double min)
{
    max -= min;
    t /= totaltime;
    return max * t * t * t + min;
}

double Easing::OutCubic(double t, double totaltime, double max, double min)
{
    max -= min;
    t = t / totaltime - 1;
    return max * (t * t * t + 1) + min;
}

double Easing::InOutCubic(double t, double totaltime, double max, double min)
{
    max -= min;
    t /= totaltime;
    if(t / 2 < 1) return max / 2 * t * t * t + min;
    t -= 2;
    return max / 2 * (t * t * t + 2) + min;
}

double Easing::InQuart(double t, double totaltime, double max, double min)
{
    max -= min;
    t /= totaltime;
    return max * t * t * t * t + min;
}

double Easing::OutQuart(double t, double totaltime, double max, double min)
{
    max -= min;
    t = t / totaltime - 1;
    return -max * (t * t * t * t - 1) + min;
}

double Easing::InOutQuart(double t, double totaltime, double max, double min)
{
    max -= min;
    t /= totaltime;
    if(t / 2 < 1) return max / 2 * t * t * t * t + min;
    t -= 2;
    return -max / 2 * (t * t * t * t - 2) + min;
}

double Easing::InQuint(double t, double totaltime, double max, double min)
{
    max -= min;
    t /= totaltime;
    return max * t * t * t * t * t + min;
}

double Easing::OutQuint(double t, double totaltime, double max, double min)
{
    max -= min;
    t = t / totaltime - 1;
    return max * (t * t * t * t * t + 1) + min;
}

double Easing::InOutQuint(double t, double totaltime, double max, double min)
{
    max -= min;
    t /= totaltime;
    if(t / 2 < 1) return max / 2 * t * t * t * t * t + min;
    t -= 2;
    return max / 2 * (t * t * t * t * t + 2) + min;
}

double Easing::InSine(double t, double totaltime, double max, double min)
{
    max -= min;
    return -max * cos(t * (DX_PI / 2) / totaltime) + max + min;
}

double Easing::OutSine(double t, double totaltime, double max, double min)
{
    max -= min;
    return max * sin(t * (DX_PI / 2) / totaltime) + min;
}

double Easing::InOutSine(double t, double totaltime, double max, double min)
{
    max -= min;
    return -max / 2 * (cos(t * DX_PI / totaltime) - 1) + min;
}

double Easing::InExp(double t, double totaltime, double max, double min)
{
    max -= min;
    return t == 0.0 ? min : max * pow(2, 10 * (t / totaltime - 1)) + min;
}

double Easing::OutExp(double t, double totaltime, double max, double min)
{
    max -= min;
    return t == totaltime ? max + min : max * (-pow(2, -10 * t / totaltime) + 1) + min;
}

double Easing::InOutExp(double t, double totaltime, double max, double min)
{
    if(t == 0.0) return min;
    if(t == totaltime) return max;
    max -= min;
    t /= totaltime;
    if(t / 2 < 1) return max / 2 * pow(2, 10 * (t - 1)) + min;
    --t;
    return max / 2 * (-pow(2, -10 * t) + 2) + min;
}

double Easing::InCirc(double t, double totaltime, double max, double min)
{
    max -= min;
    t /= totaltime;
    return -max * (sqrt(1 - t * t) - 1) + min;
}

double Easing::OutCirc(double t, double totaltime, double max, double min)
{
    max -= min;
    t = t / totaltime - 1;
    return max * sqrt(1 - t * t) + min;
}

double Easing::InOutCirc(double t, double totaltime, double max, double min)
{
    max -= min;
    t /= totaltime;
    if(t / 2 < 1) return -max / 2 * (sqrt(1 - t * t) - 1) + min;
    t -= 2;
    return max / 2 * (sqrt(1 - t * t) + 1) + min;
}

double Easing::InBack(double t, double totaltime, double max, double min, double s)
{
    max -= min;
    t /= totaltime;
    return max * t * t * ((s + 1) * t - s) + min;
}

double Easing::OutBack(double t, double totaltime, double max, double min, double s)
{
    max -= min;
    t = t / totaltime - 1;
    return max * (t * t * ((s + 1) * t * s) + 1) + min;
}

double Easing::InOutBack(double t, double totaltime, double max, double min, double s)
{
    max -= min;
    s *= 1.525;
    if(t / 2 < 1)
    {
        return max * (t * t * ((s + 1) * t - s)) + min;
    }
    t -= 2;
    return max / 2 * (t * t * ((s + 1) * t + s) + 2) + min;
}

double Easing::OutBounce(double t, double totaltime, double max, double min)
{
    max -= min;
    t /= totaltime;
    if(t < 1 / 2.75) return max * (7.5625 * t * t) + min;
    else if(t < 2 / 2.75)
    {
        t -= 1.5 / 2.75;
        return max * (7.5625 * t * t + 0.75) + min;
    }
    else if(t < 2.5 / 2.75)
    {
        t -= 2.25 / 2.75;
        return max * (7.5625 * t * t + 0.9375) + min;
    }
    else
    {
        t -= 2.625 / 2.75;
        return max * (7.5625 * t * t + 0.984375) + min;
    }
}

double Easing::InBounce(double t, double totaltime, double max, double min)
{
    return max - OutBounce(totaltime - t, totaltime, max - min, 0) + min;
}

double Easing::InOutBounce(double t, double totaltime, double max, double min)
{
    if(t < totaltime / 2) return InBounce(t * 2, totaltime, max - min, max) * 0.5 + min;
    else return OutBounce(t * 2 - totaltime, totaltime, max - min, 0) * 0.5 + min + (max - min) * 0.5;
}

double Easing::Linear(double t, double totaltime, double max, double min)
{
    return (max - min) * t / totaltime + min;
}
