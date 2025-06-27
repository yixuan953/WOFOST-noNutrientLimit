/* General global parameters */ /* 通用全局参数 */
#include "wofost.h"

#ifndef EXTERN_H
#define EXTERN_H

size_t Day;    // 天，用于记录时间的天数
size_t Lon;    // 经度
size_t Lat;    // 纬度
float Temp;    // 温度
float DayTemp; // 日平均温度

/* General help functions */                     /* 通用辅助函数 */
extern float max(float a, float b);              // 返回两个浮点数中的最大值
extern float min(float a, float b);              // 返回两个浮点数中的最小值
extern float Afgen();                            // 阿弗根函数（可能用于查找某些特定的函数值）
extern float List();                             // 列表函数
extern float limit(float a, float b, float c);   // 限制函数，限制a在b和c之间
extern float notnul(float x);                    // 非空函数，检查x不为0
extern float insw(float x1, float x2, float x3); // 插值开关函数
extern int leap_year(int year);                  // 判断闰年的函数
extern void RatesToZero();                       // 将速率设为零的函数
extern void Output_Daily();                            // 输出函数
extern void Output_Annual();                           // 输出函数
extern void header_DO();                            // 生成头信息的函数
extern void header_AO();                            // 生成头信息的函数
extern int GetSimInput();                        // 获取模拟输入的函数
extern void GetMeteoInput();                     // 获取气象输入的函数
extern void GetFertInput();                     // 获取肥料输入的函数
extern void GetIrriInput();                     // 获取灌溉输入的函数
extern void IfSowing();                          // 判断是否播种的函数
extern char* DekadDate(float sowing_float);

/* Additional functions */        /* 额外的函数 */
extern int Astro();               // 天文算法函数
extern void CalcPenman();         // Penman蒸发方程计算函数
extern void CalcPenmanMonteith(); // Penman-Monteith蒸发方程计算函数
extern void Clean();              // 清理函数
extern void CleanMeteo();         // 清理气象数据的函数
extern void CleanFert();          // 清理施肥数据的函数
extern void CleanIrri();          // 清理施肥数据的函数
extern void Moment();             // 时刻函数

/* Crop growth */                           /* 作物生长 */
extern void Partioning();                   // 分配函数
extern void HeatStress();                   // 热应力函数
extern void RateCalculationCrop();          // 作物生长速率计算函数
extern void Growth(float NewPlantMaterial); // 生长函数，根据新植物材料
extern void IntegrationCrop();              // 作物生长积分函数
extern void InitializeCrop();               // 初始化作物函数
extern int EmergenceCrop(int Emergence);    // 作物出苗函数

extern void DevelopmentRate();                                                                                              // 发育率函数
extern void LeaveGrowth();                                                                                                  // 叶片生长函数
extern float DailyTotalAssimilation();                                                                                      // 每日总同化率函数
extern float DyingLeaves();                                                                                                 // 枯萎叶片函数
extern float InstantAssimilation(float KDiffuse, float EFF, float AssimMax, float SinB, float PARDiffuse, float PARDirect); // 即时同化率函数
extern float LeaveAreaIndex();                                                                                              // 叶面积指数函数
extern float Correct(float GrossAssimilation);                                                                              // 纠正函数（可能用于校正总同化率）
extern float RespirationRef(float TotalAssimilation);                                                                       // 呼吸参考函数
extern float Conversion(float NetAssimilation);                                                                             // 转换函数（可能用于同化作用的净产物转换）

/* Nutrients */
extern void CropNutrientRates();       // 作物养分速率函数
extern void InitializeNutrients();     // 初始化营养元素函数
extern void IntegrationNutrients();    // 营养元素积分函数
extern void NutritionINDX();           // 营养指数函数
extern void NutrientLoss();            // 养分损失函数
extern void NutrientMax();             // 养分最大值函数
extern void NutrientPartioning();      // 养分分配函数
extern void NutrientRates();           // 养分速率函数
extern void NutrientOptimum();         // 养分最佳函数
extern void NutrientDemand();          // 养分需求函数
extern void SoilNutrientRates();       // 土壤营养素变化率计算函数
extern void NutrientTranslocation();   // 营养素转移函数
extern void RateCalcultionNutrients(); // 营养素速率计算函数

/* Read data */
extern int GetAfgenTables();  // 获取Afgen表的函数
extern int GetMeteoData();    // 获取气象数据的函数
extern int FillAfgenTables(); // 填充Afgen表的函数

extern void GetCropData();         // 获取作物数据的函数
extern void FillCropVariables();   // 填充作物变量的函数
extern void GetSiteData();         // 获取场地数据的函数
extern void FillSiteVariables();   // 填充场地变量的函数
extern void GetSoilData();         // 获取土壤数据的函数
extern void FillSoilVariables();   // 填充土壤变量的函数
extern void GetManagement();       // 获取管理数据的函数
extern void FillManageVariables(); // 填充管理变量的函数

/* Water balance */
extern void InitializeWatBal();     // 初始化水分平衡的函数
extern void RateCalulationWatBal(); // 计算水分平衡的速率函数
extern void IntegrationWatBal();    // 水分平衡积分函数
extern void EvapTra();              // 蒸发和蒸腾计算函数

#endif // EXTERN_H