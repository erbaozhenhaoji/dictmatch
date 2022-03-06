/*****************************************************************************************
 * Filename	: dmprop.h
 * Descript	: define the properties of the dict.
 * Author	: Paul Yang, zhenhaoji@sohu.com
 * Create	: 2005-05-29
 * Modified	: 2005-05-29
 ****************************************************************************************/
#ifndef _DM_PROP_H
#define	_DM_PROP_H


#define DM_HONGGE			0x00000001
#define DM_IS_HONGGE(prop)		((prop) & DM_HONGGE)
#define DM_SET_HONGGE(prop)		((prop) | DM_HONGGE)

#define DM_MINGZHU			0x00000002
#define DM_IS_MINGZHU(prop)		((prop) & DM_MINGZHU)
#define DM_SET_MINGZHU(prop)	        ((prop) | DM_MINGZHU)

#define DM_SANJI			0x00000004
#define DM_IS_SANJI(prop)		((prop) & DM_SANJI)
#define DM_SET_SANJI(prop)       	((prop) | DM_SANJI)


int dm_prop_str2int(const char* pstr);

#endif

