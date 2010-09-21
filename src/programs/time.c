int time(int argc, char * argv[])
{
	if(argc != 1){
		printf("Invalid arguments.\n");
		return 1;
	}
	_outport(0x70, 4);
	int hours = _inport(0x71);
	putchar(((hours & 0xF0)>>4) + '0');
	putchar((hours & 0xF) + '0');
	putchar(':');

	_outport(0x70, 2);
	int minutes = _inport(0x71);
	putchar(((minutes & 0xF0)>>4) + '0');
	putchar((minutes & 0xF) + '0');
	putchar(':');
	
	_outport(0x70, 0);
	int seconds = _inport(0x71);
	putchar(((seconds & 0xF0)>>4) + '0');
	putchar((seconds & 0xF) + '0');
	putchar('\n');
	return 0;
}

