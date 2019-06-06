/* Shows the use of Blackboard API*/

#include <ACM/USER/ACM_USER_INCLUDE.h>
#include <iomanip>

using namespace ACM_USER;

int main(){


	

	std::cout<<"starting\n";
	

	BLACKBOARD_ID_TYPE my_board;


	RETURN_CODE_TYPE RETURN_CODE;
	char BLACKBOARD_NAME[30]="testboard";
	CREATE_BLACKBOARD(BLACKBOARD_NAME,200,&my_board,&RETURN_CODE);
	std::cout<<"size of char"<<sizeof(char)<<std::endl;

	char message[200]="This is the blackboard message";
	char readData[200];
	MESSAGE_SIZE_TYPE length;
	LOGGER(APP,"READING FROM EMPTY BBOARD");
	READ_BLACKBOARD(my_board,convertSecsToNano(5),(APEX_BYTE*)readData,&length,&RETURN_CODE);

	LOGGER(APP,"Return code from READING FROM EMPTY BBOARD is %s",RETURN_CODE_TO_STRING(RETURN_CODE));
	DISPLAY_BLACKBOARD (my_board,(APEX_BYTE*)message,200*sizeof(char),&RETURN_CODE);
	READ_BLACKBOARD(my_board,0,(APEX_BYTE*)readData,&length,&RETURN_CODE);

	printf("*************************************************************\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("%s",readData);
	printf("\n");
	printf("\n");
	printf("\n");
	printf("*************************************************************\n");
	assert(RETURN_CODE==NO_ERROR);


	CLEAR_BLACKBOARD(my_board,&RETURN_CODE);
	assert(RETURN_CODE==NO_ERROR);
	LOGGER(APP,"Erased BBOARD.Trying to Read Again");
	READ_BLACKBOARD(my_board,convertSecsToNano(5),(APEX_BYTE*)readData,&length,&RETURN_CODE);

	//assert(RETURN_CODE==NO_ERROR);
	LOGGER(APP,"Return code from READING FROM EMPTY BBOARD is %s",RETURN_CODE_TO_STRING(RETURN_CODE));

	//strcpy(message,"Reading Again");
	int i=10;
	DISPLAY_BLACKBOARD (my_board,(APEX_BYTE*)(&i),sizeof(int),&RETURN_CODE);
	int j;
	READ_BLACKBOARD(my_board,0,(APEX_BYTE*)(&j),&length,&RETURN_CODE);

	printf("*************************************************************\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("Integer value %d was read. Size of the message was %ld",j,length);
	printf("\n");
	printf("\n");
	printf("\n");
	printf("*************************************************************\n");
	assert(RETURN_CODE==NO_ERROR);

	return 0;
}
