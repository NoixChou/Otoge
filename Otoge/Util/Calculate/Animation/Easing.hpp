#pragma once

class Easing
{
public:
    using EaseFunction = double(*)(double, double, double, double);

    static double InQuad(double t, double totaltime, double max, double min);
    static double OutQuad(double t, double totaltime, double max, double min);
    static double InOutQuad(double t, double totaltime, double max, double min);
    static double InCubic(double t, double totaltime, double max, double min);
    static double OutCubic(double t, double totaltime, double max, double min);
    static double InOutCubic(double t, double totaltime, double max, double min);
    static double InQuart(double t, double totaltime, double max, double min);
    static double OutQuart(double t, double totaltime, double max, double min);
    static double InOutQuart(double t, double totaltime, double max, double min);
    static double InQuint(double t, double totaltime, double max, double min);
    static double OutQuint(double t, double totaltime, double max, double min);
    static double InOutQuint(double t, double totaltime, double max, double min);
    static double InSine(double t, double totaltime, double max, double min);
    static double OutSine(double t, double totaltime, double max, double min);
    static double InOutSine(double t, double totaltime, double max, double min);
    static double InExp(double t, double totaltime, double max, double min);
    static double OutExp(double t, double totaltime, double max, double min);
    static double InOutExp(double t, double totaltime, double max, double min);
    static double InCirc(double t, double totaltime, double max, double min);
    static double OutCirc(double t, double totaltime, double max, double min);
    static double InOutCirc(double t, double totaltime, double max, double min);
    static double InBack(double t, double totaltime, double max, double min, double s);
    static double OutBack(double t, double totaltime, double max, double min, double s);
    static double InOutBack(double t, double totaltime, double max, double min, double s);
    static double OutBounce(double t, double totaltime, double max, double min);
    static double InBounce(double t, double totaltime, double max, double min);
    static double InOutBounce(double t, double totaltime, double max, double min);
    static double Linear(double t, double totaltime, double max, double min);
};

