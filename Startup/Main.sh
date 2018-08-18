#!/bin/bash
#Arguments -f FILENAME -m MESSAGE [-d cgdb|strace]
#-f FILENAME=MARTe configuration file
#-m MESSAGE=Start message
#-d cgdb=Run with cgdb
#-d strace=Run with strace
#-x DAN_CONFIG_LOCATION=Location of the DANConfig.xml (e.g. ~/Projects/Event-Recorder-Falcon/Configurations/Config.xml)
#-t location of the falcon_event tree

#Run with cgdb or strace?
DEBUG=""

#Consume input arguments
while [[ $# -gt 1 ]]
do
key="$1"

case $key in
    -f|--file)
    FILE="$2"
    shift # past argument
    ;;
    -m|--message)
    MESSAGE="$2"
    shift # past argument
    ;;
    -s|--state)
    STATE="$2"
    shift # past argument
    ;;
    -d|--debug)
    DEBUG="$2"
    shift # past argument
    ;;
    -x|--dan_config)
    DAN_CONFIG_LOCATION="$2"
    shift # past argument
    ;;   
    -i|--dan_ip)
    DAN_MASTER_IP="$2"
    shift # past argument
    ;;   
    -r|--root)
    RUN_AS_ROOT="root"
    shift # past argument
    ;;
    -t|--tree)
    MDSTREE="$2"
    shift # past argument
    ;;    
    --default)
    DEFAULT=YES
    ;;
    *)
            # unknown option
    ;;
esac
shift # past argument or value
done

if [ -z ${MARTe2_DIR+x} ]; then echo "Please set the MARTe2_DIR environment variable"; exit; fi
if [ -z ${MARTe2_Components_DIR+x} ]; then echo "Please set the MARTe2_Components_DIR environment variable"; exit; fi

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../Build/x86-linux/Components/GAMs/PacketGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../Build/x86-linux/Components/DataSources/CRIOUARTDataSource/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_DIR/Build/x86-linux/Core/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/CursesDataSource/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/EPICSCA/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/LinuxTimer/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/LoggerDataSource/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/DAN/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/SDN/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/UDP/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/MDSWriter/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/RealTimeThreadSynchronisation/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/FileDataSource/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/IOGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/ConversionGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/FilterGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/StatisticsGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/WaveformGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/Interfaces/SysLogger/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/Interfaces/EPICS/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$EPICS_BASE/lib/$EPICS_HOST_ARCH
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/mdsplus/lib64/

echo $LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH

#Starts the DAN services only if required
if [ ! -z "$DAN_CONFIG_LOCATION" ]
then
export DAN_ARCHIVE_MASTER=$DAN_MASTER_IP
echo $DAN_MASTER_IP
/opt/codac/bin/danApiTool api close
/opt/codac/bin/danApiTool api init $DAN_CONFIG_LOCATION
fi


if [ ! -z ${RUN_AS_ROOT+x} ]; then 

#Disable CPU speed changing
service cpuspeed stop

#Allocate dynamic ticks to CPU #0
for i in `pgrep rcu[^c]` ; do taskset -pc 0 $i ; done

#disable watchdog
sudo sysctl kernel.nmi_watchdog=0
sudo sysctl kernel.watchdog=0

NR_CPUS=$(getconf _NPROCESSORS_ONLN)

#Isolate cpus 1-3 (tasks and interrupts)
tuna -c 1-$(($NR_CPUS-1)) --isolate

#delay vm statistic
echo 1000 > /proc/sys/vm/stat_interval

#disable cpu usage control
echo -1 > /proc/sys/kernel/sched_rt_runtime_us

echo 0 > /proc/sys/vm/dirty_writeback_centisecs
echo 1 > /proc/sys/vm/laptop_mode

#disable swap memory
swapoff -a

# Migrate nocb tasks to CPU 0
# Beware, this assume that there is no online CPU > NR_CPUS
for CPU in $(seq $(($NR_CPUS-1)))
do
PIDS=$(ps -o pid= -C rcuob/$CPU,rcuos/$CPU,rcuop/$CPU)
for PID in $PIDS
do
taskset -cp 0 $PID
done
done

# Migrate processes to CPU0 
for D in $(ls /proc/irq)
do
if [ -x "/proc/irq/$D" ] && [ $D != "0" ]
then
echo $D
echo 1 > /proc/irq/$D/smp_affinity
fi
done

for D in $(ls /proc)
do
CMDL=$(cat /proc/$D/cmdline)
if [ -x "/proc/$D" ] && [ $D != $$ ] 
then
taskset -cp 0 $D
fi
done

fi

if [ ! -z "$DAN_CONFIG_LOCATION" ]
then
/etc/init.d/dan_archiver_writer -f start
fi

if [ ! -z "$MDSTREE" ]
then
export falcon_event_path=$MDSTREE
#Create a new pulse
mdstcl < CreateTestTree.tcl
fi

#Start with cgdb or with strace
if [ "$DEBUG" = "cgdb" ]
then
    cgdb --args $MARTe2_DIR/Build/x86-linux/App/MARTeApp.ex -l RealTimeLoader -f $FILE -m $MESSAGE
elif [ "$DEBUG" = "strace" ]
then
    strace -o/tmp/strace.err $MARTe2_DIR/Build/x86-linux/App/MARTeApp.ex -l RealTimeLoader -f $FILE -m $MESSAGE
else
    if [ -z ${RUN_AS_ROOT+x} ]; then
		if [ -z ${STATE+x} ]; then
	         $MARTe2_DIR/Build/x86-linux/App/MARTeApp.ex -l RealTimeLoader -f $FILE -m $MESSAGE
	    else
    		 $MARTe2_DIR/Build/x86-linux/App/MARTeApp.ex -l RealTimeLoader -f $FILE -s $STATE 
	    fi
    else
		if [ -z ${STATE+x} ]; then
	         taskset 1 $MARTe2_DIR/Build/x86-linux/App/MARTeApp.ex -l RealTimeLoader -f $FILE -m $MESSAGE
	    else
    		 taskset 1 $MARTe2_DIR/Build/x86-linux/App/MARTeApp.ex -l RealTimeLoader -f $FILE -s $STATE 
	    fi
	fi
fi

if [ ! -z "$DAN_CONFIG_LOCATION" ]
then
/opt/codac/bin/danApiTool api close
fi
