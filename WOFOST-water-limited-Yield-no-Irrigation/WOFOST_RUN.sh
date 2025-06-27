#!/bin/bash
#-----------------------------Mail address-----------------------------
#SBATCH --mail-user=yixuan.zhou@wur.nl
#SBATCH --mail-type=FAIL
#-----------------------------Output files-----------------------------
#SBATCH --output=/lustre/nobackup/WUR/ESG/zhou111/WOFOST-Yp/WOFOST-Potential-Yield/HPC_Report/output_%j.txt
#SBATCH --error=/lustre/nobackup/WUR/ESG/zhou111/WOFOST-Yp/WOFOST-Potential-Yield/HPC_Report/error_%j.txt
#-----------------------------Other information------------------------

#-----------------------------Required resources-----------------------
#SBATCH --time=200
#SBATCH --mem=250000

#-----------------------------Environment, Operations and Job steps----
#load modules
module load netcdf
#----------------------------Run WOFOST simulations-----------------------
# echo "Current directory: $(pwd)"
#./wofost_Yp list_Yangtze_maize_Yp.txt meteolist_WFDE5_Yangtze_maize.txt 
./wofost_wl_no_irri list_Yangtze_mainrice_wl_no_irri.txt meteolist_WFDE5_Yangtze_mainrice.txt 
#./wofost_Yp list_Yangtze_secondrice_Yp.txt meteolist_WFDE5_Yangtze_secondrice.txt 
#./wofost_Yp list_Yangtze_soybean_Yp.txt meteolist_WFDE5_Yangtze_soybean.txt 
#./wofost_Yp list_Yangtze_winterwheat_Yp.txt meteolist_WFDE5_Yangtze_winterwheat.txt 