/*
 * =====================================================================================
 *
 *       Filename:  confloader_driver.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/13/2012 02:41:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ub.h"

int main(int argc, char *argv[])
{

//	for (int i=0;i!=100;i++) {
		config_t* config = ub_config_load_from_file("./test.conf");

		ub_config_print(config);
		fprintf(stderr,"number convert : %ld\n",ub_config_integer(config,"num"));
		fprintf(stderr,"number convert : %ld\n",ub_config_integer(config,"num1"));
		fprintf(stderr,"number convert : %ld\n",ub_config_integer(config,"num2"));
		fprintf(stderr,"number convert : %ld\n",ub_config_integer(config,"num3"));
		fprintf(stderr,"number convert : %ld\n",ub_config_integer(config,"num4"));
		fprintf(stderr,"number convert : %ld\n",ub_config_integer(config,"num5"));
		fprintf(stderr,"number convert : %ld\n",ub_config_integer(config,"num6"));
		ub_config_release(config);
//	}

}
