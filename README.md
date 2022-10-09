# ACL

```
cd 20221003-GraduationDesign
sudo cmake .
sudo make
sudo cpufreq-set -c 8 -g performance
sudo cpufreq-set -c 8 -d 3.6G
sudo cpufreq-set -c 8 -u 4.9G
ulimit -s unlimited; taskset -c 8 ./pcRun

```