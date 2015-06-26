#ifndef __WS_SENSOR_H__
#define __WS_SENSOR_H__

#include <stdint.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<netdb.h>
#include<stdarg.h>
#include<string.h>
#include <semaphore.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <hardware/sensors.h>
#include <dlfcn.h>  //fsz ++
#include <dirent.h> //fsz ++

#define DLLDIR
#define ssize_t          unsigned int
#define SENSOR_MAX_VALUE     20

#define  __ANDROID
#ifdef __ANDROID
#include <android/log.h>
#define TAG_DEBUG "WSLIB"
#endif

struct ws_device_t{
    int (*Init_WsSensorDevice)(struct ws_device_t **wsdevice,void *lib);
	int (*fun)(void);
	void (*DumpGsensordata)(sensors_event_t const * buffer, size_t count);
	ssize_t (*SensorDevicePoll)(sensors_event_t* buffer, size_t count);
	ssize_t (*getSensorListEx)(struct sensors_module_t* sensor_mode, struct sensor_t const** list);
	int (*initCheckEx)(void);
	int (*sensorHandleCheck)(int handle);
};

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
extern DLLDIR  void DumpGsensordata(sensors_event_t const * buffer, size_t count);
extern DLLDIR  int  getSensorId(int type);
extern DLLDIR  ssize_t getSensorListEx(struct sensors_module_t* sensor_mode, struct sensor_t const** list);
extern DLLDIR  int initCheckEx();
extern DLLDIR  int initWsSocket(int * WsSocket);
extern DLLDIR  int Init_WsSensorDevice(struct ws_device_t **wsdevice ,void *lib );
extern DLLDIR  ssize_t  SensorDevicePoll(sensors_event_t* buffer, size_t count);
extern DLLDIR  int sensorHandleCheck(int handle);
extern DLLDIR  void ws_dprintf(const char *format,...);
extern DLLDIR  int ws_processCmd();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __WS_SENSOR_H__ */

