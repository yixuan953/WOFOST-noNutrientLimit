#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wofost.h"
#include "extern.h"

/* ----------------------------------------------------------------------------*/
/*  function GetMeteoInput()                                                   */
/*  Purpose: Get the names of the crop, soil, management and site files as     */
/*           as well if the simulation starting day and whether the simulation */
/*           starts at emergence (1) or sowing (0)                             */
/*-----------------------------------------------------------------------------*/

int GetSimInput(char *list) /* 获取模拟输入数据 */
{
    FILE *ifp; // 声明了一个指向 FILE 类型的指针 ifp

    SimUnit *initial = NULL; // 指向第一个仿真单元的指针

    int Emergence; // 发芽标志
    int count;     // 计数器

    char line[MAX_STRING];                                                 // 存放每行读取的数据
    char path[MAX_STRING];                                                 // 存放路径
    char cropfile[MAX_STRING];                                             // 存放作物文件名
    char soilfile[MAX_STRING];                                             // 存放土壤文件名
    char sitefile[MAX_STRING];                                             // 存放站点文件名
    char management[MAX_STRING];                                           // 存放管理文件名
    char output_daily[MAX_STRING];                                         // 存放输出文件名
    char output_annual[MAX_STRING];                                        // 存放输出文件名
    char start[MAX_STRING];                                                // 存放开始日期
    char cf[MAX_STRING], sf[MAX_STRING], mf[MAX_STRING], site[MAX_STRING]; // cf, sf, mf, site分别存放作物、土壤、管理和站点的文件名缩写

    ifp = fopen(list, "r"); // 打开输入文件

    if (ifp == NULL) // 文件打开失败
    {
        fprintf(stderr, "Can't open input, %s\n", list);
        exit(1);
    }

    count = 0;
    while (fgets(line, MAX_STRING, ifp))
    { // 逐行读取文件
        if (line[0] == '*' || line[0] == ' ' || line[0] == '\n')
        {
            continue; // 忽略注释、空行和换行符
        }

        sscanf(line, "%s %s %s %s %s %s %d %s %s",
               path, cf, sf, mf, site, start, &Emergence, output_daily, output_annual); // 解析行数据

        // 初始化文件名字符串
        memset(cropfile, '\0', MAX_STRING);
        memset(sitefile, '\0', MAX_STRING);
        memset(soilfile, '\0', MAX_STRING);
        memset(management, '\0', MAX_STRING);

        // 拼接文件路径和文件名
        strncpy(cropfile, path, strlen(path));
        strncat(cropfile, cf, strlen(cf));

        strncpy(soilfile, path, strlen(path));
        strncat(soilfile, sf, strlen(sf));

        strncpy(management, path, strlen(path));
        strncat(management, mf, strlen(mf));

        strncpy(sitefile, path, strlen(path));
        strncat(sitefile, site, strlen(site));

        /* 计算输出文件的数量 */
        /* number 是文件指针列表的索引号 */
        /* count the number of output files */
        /* number is the index number of the list of file pointers */
        if (initial == NULL) // 如果是第一个元素
        {
            Grid = initial = malloc(sizeof(SimUnit)); // 分配空间并设置为初始指针
        }
        else // 如果不是第一个元素
        {
            Grid->next = malloc(sizeof(SimUnit)); // 为下一个仿真单元分配空间
            Grid = Grid->next;                    // 移动到下一个仿真单元
        }

        // 分配空间并获取数据
        GetCropData(Grid->crp = malloc(sizeof(Plant)), cropfile);
        GetSiteData(Grid->ste = malloc(sizeof(Field)), sitefile);
        GetManagement(Grid->mng = malloc(sizeof(Management)), management);
        GetSoilData(Grid->soil = malloc(sizeof(Soil)), soilfile);

        // 检查文件名长度是否超出限制
        if (strlen(sf) >= MAX_STRING)
            exit(0);
        if (strlen(output_daily) >= MAX_STRING)
            exit(0);
        if (strlen(output_annual) >= MAX_STRING)
            exit(0);
        if (strlen(start) >= MAX_STRING)
            exit(0);

        // 初始化输出和开始字符串
        memset(Grid->output_daily, '\0', MAX_STRING);
        memset(Grid->output_annual, '\0', MAX_STRING);
        memset(Grid->start, '\0', MAX_STRING);

        strncpy(Grid->output_daily, output_daily, strlen(output_daily)); // Name of output file // 设置输出文件名
        strncpy(Grid->output_annual, output_annual, strlen(output_annual)); // Name of output file // 设置输出文件名
        strncpy(Grid->start, start, strlen(start));    // Starting string month day of the simulations// 设置模拟的开始月日

        Grid->file_DO = count++;        // number of elements in Grid carousel// 设置Grid轮询中的元素数量
        Grid->file_AO = count++;        // number of elements in Grid carousel// 设置Grid轮询中的元素数量
        Grid->emergence = Emergence; // Start the simulations at emergence (1) or at sowing (0)// 设置模拟开始时是在发芽(1)还是在播种(0)
        Grid->crp->Sowing = 0;
        Grid->crp->Emergence = 0; // Crop emergence has not yet occurred// 设置作物发芽尚未发生
        Grid->next = NULL;
    }

    fclose(ifp);

    /* Set Grid back to initial address */
    Grid = initial;

    /* Return the number of meteo files */
    return count;
}
