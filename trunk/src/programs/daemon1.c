int daemon1(int argc, char * argv[]){
	if(argc == 2)
		sleep(atoi(argv[1]));
	return 0;
}
