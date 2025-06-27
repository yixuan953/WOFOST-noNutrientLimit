#!/bin/bash
#-----------------------------Mail address-----------------------------
#SBATCH --mail-user=yixuan.zhou@wur.nl
#SBATCH --mail-type=FAIL
#-----------------------------Output files-----------------------------
#SBATCH --output=/lustre/nobackup/WUR/ESG/zhou111/WOFOST-Nutrient/HPC_Report/output_%j.txt
#SBATCH --error=/lustre/nobackup/WUR/ESG/zhou111/WOFOST-Nutrient/HPC_Report/error_output_%j.txt
#-----------------------------Other information------------------------

#-----------------------------Required resources-----------------------
#SBATCH --time=200
#SBATCH --mem=250000

#-----------------------------Environment, Operations and Job steps----
#load modules
module load netcdf
#----------------------------Run WOFOST simulations-----------------------
# echo "Current directory: $(pwd)"
./wofost_wl_with_irri list_Yangtze_mainrice.txt meteolist_WFDE5_Yangtze_mainrice.txt fertlist_Yangtze_mainrice.txt irrilist_Yangtze_mainrice.txt
#./wofost_wl_with_irri list_Yangtze_soybean.txt meteolist_WFDE5_Yangtze_soybean.txt fertlist_Yangtze_soybean.txt irrilist_Yangtze_soybean.txt