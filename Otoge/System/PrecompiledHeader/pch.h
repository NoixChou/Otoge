#pragma once

// コンパイラ設定
#pragma execution_character_set("utf-8")
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

// Windows api
#include <Windows.h>
#include <Shlwapi.h>
#include <Imm.h>
#include <intrin.h>


// ランタイム
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <cmath>

// 標準ライブラリ
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <chrono>
#include <ios>
#include <map>
#include <utility>
#include <limits>
#include <unordered_set>
#include <unordered_map>
#include <forward_list>
#include <list>
#include <tuple>
#include <random>
#include <exception>
#include <future>
#include <thread>
#include <numeric>
#include <string>
#include <optional>
#include <locale>
#include <codecvt>

// Boostライブラリ
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// DXライブラリ
#include <DxLib.h>
