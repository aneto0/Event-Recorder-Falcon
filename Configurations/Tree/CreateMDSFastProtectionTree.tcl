edit falcon_event/new

add node .CRIO_IN
set def \FALCON_EVENT::TOP.CRIO_IN
add node START_HVPS/usage=signal
add node MODULATE/usage=signal
add node RESET_HVPS/usage=signal
add node EU_ARC/usage=signal
add node RU_ARC/usage=signal
add node PLC_TRIP_1/usage=signal
add node PLC_TRIP_2/usage=signal
add node START_TIME/usage=signal
add node CRITICAL_ERR/usage=signal
add node ADMIN_MODE/usage=signal
add node MODE_LOSS/usage=signal
add node STRAY_1/usage=signal
add node STRAY_2/usage=signal
add node RESET_FPGA/usage=signal

set def \FALCON_EVENT::TOP
add node .PRE_PROC_OUT
set def \FALCON_EVENT::TOP.PRE_PROC_OUT
add node EU_ARC/usage=signal
add node RU_ARC/usage=signal
add node PLC_TRIP_1/usage=signal
add node PLC_TRIP_2/usage=signal
add node MODE_LOSS/usage=signal
add node STRAY_1/usage=signal
add node STRAY_2/usage=signal
add node L_EU_ARC/usage=signal
add node L_RU_ARC/usage=signal
add node L_PLC_TRIP_1/usage=signal
add node L_PLC_TRIP_2/usage=signal
add node L_MODE_LOSS/usage=signal
add node L_STRAY_1/usage=signal
add node L_STRAY_2/usage=signal
add node B_EU_ARC/usage=signal
add node B_RU_ARC/usage=signal
add node B_PLC_TRIP_1/usage=signal
add node B_PLC_TRIP_2/usage=signal
add node B_MODE_LOSS/usage=signal
add node B_STRAY_1/usage=signal
add node B_STRAY_2/usage=signal

set def \FALCON_EVENT::TOP
add node .ADHOC
set def \FALCON_EVENT::TOP.ADHOC
add node TRIP_OUT/usage=signal

set def \FALCON_EVENT::TOP
add node .OUT
set def \FALCON_EVENT::TOP.OUT
add node FLS/usage=signal

set def \FALCON_EVENT::TOP
add node .HVPS_OUT
set def \FALCON_EVENT::TOP.HVPS_OUT
add node TRIP_OUT/usage=signal
add node ON_MPS/usage=signal
add node ON_BPS/usage=signal
add node STATE_OUT/usage=signal

set def \FALCON_EVENT::TOP
add node .OUT_IF
set def \FALCON_EVENT::TOP.OUT_IF
add node TRIP_PLC/usage=signal
add node TRIP_MPS/usage=signal
add node TRIP_BPS/usage=signal

set def \FALCON_EVENT::TOP
add node .FLS_MANAGER
set def \FALCON_EVENT::TOP.FLS_MANAGER
add node STATE_OUT/usage=signal

set def \FALCON_EVENT::TOP
add node .ERROR_CODE
set def \FALCON_EVENT::TOP.ERROR_CODE
add node VALUE/usage=signal

set def \FALCON_EVENT::TOP
add node .LOGIC_MODE
set def \FALCON_EVENT::TOP.LOGIC_MODE
add node VALUE/usage=signal

set def \FALCON_EVENT::TOP
add node .DEBUG
set def \FALCON_EVENT::TOP.DEBUG
add node PACKET_COUNT/usage=signal
add node SYNCH_BYTE/usage=signal
add node CRC/usage=signal
add node MARTE_CODE/usage=signal
add node ORIGINAL/usage=signal

write
close
exit
