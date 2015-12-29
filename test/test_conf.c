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
#include "nimo/conf.h"

int main(int argc, char *argv[])
{

//	for (int i=0;i!=100;i++) {
		config_t* config = nimo_config_load_from_file("./test.conf");

		nimo_config_print(config);
		fprintf(stderr,"key pidfile : %s\n",nimo_config_string(config,"pidfile"));
		nimo_config_release(config);
//	}

}
