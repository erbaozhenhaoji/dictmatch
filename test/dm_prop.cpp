/*========================================================================================
 * function : convert the string property to an integer. 
 * argu		: pstr, the string property
 * return	: the integer property.
 ========================================================================================*/

#include <stdio.h>
#include <string.h>
#include "dm_prop.h"

int dm_prop_str2int(const char*pstr)
{
	unsigned i;
	int property = 0;

	for(i=0;i<strlen(pstr);i++){
		switch(pstr[i]){
			case 'h' :
				property=DM_SET_HONGGE(property);
				break;
			case '3' :
				property=DM_SET_SANJI(property);
				break;
			case 'm' :
				property=DM_SET_MINGZHU(property);
				break;
			default:
				break;
		}
	}

	return property;
}

