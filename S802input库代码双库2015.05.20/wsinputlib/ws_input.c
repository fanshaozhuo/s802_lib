#include "ws_input.h"

static sensor_t mSensorList[SENSOR_MAX_VALUE];
static int32_t mJoystickDeviceId[10]= {0};
//static int32_t mJoyCount = 0;



typedef struct AxisInfo_ws {
	int32_t axis;
    // When split, this is the axis used for values after higher than the split position.
    int32_t highAxis;
    // The split value, or 0 if not split.
    int32_t splitValue;
    // The flat value, or -1 if none.
    int32_t flatOverride;
}AxisInfo;

#define __DEBUG
void ws_dprintf(const char *format,...)
{
#ifndef __DEBUG
    int ret = 0;
    va_list args;
    va_start(args,format);
    ret = vprintf(format,args);
    va_end(args);
    fflush(stdout);
#else
    va_list ap;
    char buf[1024];
    va_start(ap, format);
    vsnprintf(buf, 1024, format, ap);
    va_end(ap);
    __android_log_write(ANDROID_LOG_DEBUG, TAG_DEBUG, buf);
#endif
}

int ws_processCmd()
{
	DEBUG(("WSLIB ws_input lib load succ version =%d\n",37));
	return 1;	
}

ssize_t  AddWsFd(int mEpollFd, int OP){
	int ret;
	ret = -1;
	if(mSocketFd < 0)
	{
		DEBUG(("WSLIB Init ws_input Socket "));
		mSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
		if (mSocketFd < 0)
		{
			DEBUG(("WSLIB Failed to bind socket"));
			return -1;
		}
	
		struct sockaddr_in server_addr;
		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(56457);
	
		if(bind(mSocketFd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
		{
			DEBUG(("WSLIB Failed to bind socket"));
			close(mSocketFd);
			return -1;
		}
	    epoll_event eventItem;
		eventItem.events = EPOLLIN;
	    eventItem.data.u32 = EPOLL_ID_SOKCET;
        DEBUG(("WSLIB eventItem.data.u32 =%d\n",eventItem.data.u32));
		DEBUG(("WSLIB mEpollFd =%d\n",mEpollFd));
		ret = epoll_ctl(mEpollFd, OP, mSocketFd, &eventItem);
		if(ret != 0){
			DEBUG((ret != 0, "WSLIB Could not add socket to epoll instance.  errno=%d",
				errno));
		}
	}
	DEBUG(("WSLIB Init a Socket SUC\n"));
	return 0;
}

static inline nsecs_t now() {
    return systemTime(SYSTEM_TIME_MONOTONIC);
}
int ConstructSensorbufferFromSocket_input(RawEvent** eventBuffer ,char *recvmsg, int32_t deviceID)
{
	RawEvent event;
	int j =0;
	char *msg;
	char *temp_msg;
	int32_t type = 0;
	nsecs_t timestamp = now();
	int32_t code =0;
	int32_t value = 0;
	int count = 0;	
	int32_t readSize = read(mSocketFd, recvmsg,1024);
	DEBUG(("WSLIB  recmsg =%s\n",recvmsg));

	//fsz 527 ++
	if(strncmp("create",recvmsg,6)==0){
		DEBUG(("<<<<<< WSLIB  >>>>>>  create "));
		return count;
	}else if (strncmp("destroy",recvmsg,7)==0)
	{
		DEBUG(("<<<<<< WSLIB  >>>>>>  destroy "));
		return count;
	}else if (strncmp("clearall",recvmsg,8)==0)
	{		
		DEBUG(("<<<<<< WSLIB  >>>>>> clearall "));
		return count;
  	} 
	//fsz 527 ++
	
	if(readSize >0){		
		while((temp_msg = strsep(&recvmsg,";"))!=NULL)
		{
		event.when = timestamp;
		//type
		msg = strsep(&temp_msg," ");
		if(msg!=NULL)
			type = atoi(msg);
		else
			type = 0;
		event.type = type;
		//code
		msg = strsep(&temp_msg," ");
		if(msg!=NULL)
			code = atoi(msg);
		else
			code = 0;
		event.code = code;
		//value
		msg = strsep(&temp_msg," ");
		if(msg!=NULL)
			value = atoi(msg);
		else
			value = 0;
		event.value = value;
		//deviceid
		event.deviceId = deviceID;

		DEBUG(("WSLIB %d %d %d = \n",type,code,value));
		memcpy(*eventBuffer,&event,sizeof(event));
		*eventBuffer += 1;
		count++;
		}
	}
	else{
        DEBUG(("WSLIB Received unexpected epoll event for socket\n"));
	}
	return count;
}

int mapAxisEx(int32_t deviceId, int32_t scanCode, AxisInfo* outAxisInfo, int32_t mJoyCount){
	int i,j;
	for(i=0;i<mJoyCount;i++)
	{
    	if(mJoystickDeviceId[i] == deviceId)
		{
			for(j = 0; j < sizeof(axisTab)/2;j++)
			{
				if(axisTab[j][0] == scanCode)
				{
//					outAxisInfo->mode = AxisInfo::MODE_NORMAL;
					outAxisInfo->highAxis = -1;
					outAxisInfo->flatOverride = -1;
					outAxisInfo->splitValue = 0;
					outAxisInfo->axis = axisTab[j][1];
					return 1;
				}
			}
		}
	}

	return 0;

}

int getAbsoluteAxisInfoEx(int32_t deviceId, int axis, RawAbsoluteAxisInfo* outAxisInfo, int32_t mJoyCount){
	int i =0;
	for(i=0;i<mJoyCount;i++)
	{

		DEBUG(("<<<<<<WSLIB >>>>>> getAbsoluteAxisInfo  %d !!!!!!!!!!!",i));
		if(mJoystickDeviceId[i] == deviceId)
		{
			switch (axis)
			{
				case ABS_X:
				case ABS_Y:
				case ABS_Z:
				case ABS_RZ:
					outAxisInfo->valid = 1;
					outAxisInfo->minValue = 0;
					outAxisInfo->maxValue = 255;
					outAxisInfo->flat = 15;
					outAxisInfo->fuzz = 0;
					outAxisInfo->resolution = 0;
					return 1;

				case ABS_HAT0X:
				case ABS_HAT0Y:
					outAxisInfo->valid = 1;
					outAxisInfo->minValue = -1;
					outAxisInfo->maxValue = 1;
					outAxisInfo->flat = 0;
					outAxisInfo->fuzz = 0;
					outAxisInfo->resolution = 0;
					return 1;
			}
		}
	}
	return 0;

}
