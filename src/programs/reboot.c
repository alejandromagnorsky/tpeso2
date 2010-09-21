int reboot(int argc, char * argv[]){
	/* Read until keyboard buffer if empty (bit 1 in port 64h must be zero). */
	while ((_inport(0x64) & 0x02) != 0);
	_outport(0x64, 0xFE);
}
