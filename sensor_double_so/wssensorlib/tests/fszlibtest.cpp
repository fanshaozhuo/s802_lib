/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdint.h>
#include <math.h>
#include <sys/types.h>

#include <cutils/properties.h>

#include <utils/SortedVector.h>
#include <utils/KeyedVector.h>
#include <utils/threads.h>
#include <utils/Atomic.h>
#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <utils/Singleton.h>
#include <utils/String16.h>

#include <binder/BinderService.h>
#include <binder/IServiceManager.h>
#include <binder/PermissionCache.h>

#include <gui/ISensorServer.h>
#include <gui/ISensorEventConnection.h>
#include <gui/SensorEventQueue.h>

#include <hardware/sensors.h>


#include <dlfcn.h>  //fsz ++
#include <dirent.h> //fsz ++


#define 	DEBUG_ERR 

#ifdef DEBUG_ERR
#define syslog_err(x,arg...)  		do{SLOGE("SENSOR_LIB_TEST :%s(),"x, __func__,## arg);}while(0)
#else
#define syslog_err(x,arg...)  		do{}while(0)
#endif

#ifdef DEBUG_ERR
#define syslog_info(x,arg...)  		do{SLOGI("SENSOR_LIB_TEST :%s(),"x, __func__,## arg);}while(0)
#else
#define syslog_info(x,arg...)  		do{}while(0)
#endif


using namespace android;
struct WSDEVICE{
	int (*fun)(void);
	void (*processCmdDevice)(char*,int,char *);
};

const char * const     WS_SENSOR_LIB_PATH = "/system/lib/libws_sensor.so";
void *fun = NULL;
void *lib = NULL;

int main(int argc, char** argv)
{
	int ret;
	ret = -1;
	DIR *dir = NULL;
	char strinit[50];
	struct WSDEVICE *device=NULL;
	device = NULL;

	device = (struct WSDEVICE *)malloc(sizeof(struct WSDEVICE));
	if(device==NULL){
		syslog_err("------malloc  device failure-----\n");
		return false;
	}
	
	lib = dlopen(WS_SENSOR_LIB_PATH,RTLD_NOW);
	if(lib == NULL){
		syslog_err("no load the sensors library--%s--%s-\n",WS_SENSOR_LIB_PATH,dlerror());
	}else{
		syslog_info("load the sensors library:%s--  %s (succese)\n",WS_SENSOR_LIB_PATH,dlerror());
	}

    memset(strinit,0,sizeof(strinit));					
	strcpy(strinit,"ws_processCmd");
	fun = dlsym(lib,strinit);
	if(ret == NULL){
		syslog_err("no find fun function:%s,:%s\n",strinit,dlerror());
	}
	else{
		syslog_info(" fun function is :%s, :%s\n",strinit,dlerror());
	}
	
	device->fun = (int (*)())fun;
	ret = device->fun();
	syslog_info("fun ret = %d \n",ret);

	return 0;
}
