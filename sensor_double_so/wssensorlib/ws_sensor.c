#include "ws_sensor.h"

#define DEBUG(x)  ws_dprintf x

static int mSocket = -1;
static struct sensor_t mSensorList[SENSOR_MAX_VALUE];
static int bAcc = 0;
static int bGyro = 0;
static int mSensorCount = 0;
static int mVirtualSensor = 0;
static int mMaxHalSensorHandle = 0;

//#define  __DEBUG

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

int initCheckEx(){
	if(1 == mVirtualSensor)
		return 1;
	return 0;
}

int sensorHandleCheck(int handle){
	if(handle > mMaxHalSensorHandle)
		return 1;
	return 0;
}

ssize_t getSensorListEx(struct sensors_module_t* sensor_mode, struct sensor_t const** list){
	int i;
	if(!sensor_mode){
		struct sensor_t const* tempList = NULL;
		mSensorCount = sensor_mode->get_sensors_list(sensor_mode, &tempList);

		for(i = 0; i < mSensorCount; i++){

			mSensorList[i] = tempList[i];

			if(SENSOR_TYPE_ACCELEROMETER == mSensorList[i].type)
				bAcc = 1;
			else if(SENSOR_TYPE_GYROSCOPE == mSensorList[i].type)
				bGyro = 1;

			if(mMaxHalSensorHandle < mSensorList[i].handle)
				mMaxHalSensorHandle = mSensorList[i].handle;
		}
	}

	if((0 == bAcc) && (mSensorCount < SENSOR_MAX_VALUE)){
		//add virtual accelerometer sensor
		mSensorList[mSensorCount].name = "Gyroscope Bias(Virtual)";
		mSensorList[mSensorCount].vendor = "AOSP";
		mSensorList[mSensorCount].version = 1;
		mSensorList[mSensorCount].handle = mMaxHalSensorHandle+1;
		mSensorList[mSensorCount].type = SENSOR_TYPE_ACCELEROMETER;
		mSensorCount++;
		mVirtualSensor = 1;
	}
	if((0 == bGyro) && (mSensorCount < SENSOR_MAX_VALUE)){
		//add virtual Gyroscope sensor
		mSensorList[mSensorCount].name = "Corrected Gyroscope Sensor(Virtual)";
		mSensorList[mSensorCount].vendor = "AOSP";
		mSensorList[mSensorCount].version = 1;
		mSensorList[mSensorCount].handle = mMaxHalSensorHandle+2;
		mSensorList[mSensorCount].type = SENSOR_TYPE_GYROSCOPE;
		mSensorCount++;
		mVirtualSensor = 1;
	}

	*list = mSensorList;

	return mSensorCount;
}



int ws_processCmd()
{
	DEBUG(("WSLIB ws_processCmd succ\n"));
	return 29;

}

static void ConstructSensorbuffer(sensors_event_t  * buffer, size_t count)
{
	unsigned int i;
    sensors_event_t event;
    memset(&event, 0, sizeof(event));
	event.version = sizeof(struct sensors_event_t);
	event.sensor =1;
	event.type = SENSOR_TYPE_ACCELEROMETER;
	event.reserved0 = 0;
	event.timestamp = 0;
	event.acceleration.x = 1;
	event.acceleration.y = 1;
	event.acceleration.z = 0;

	for(i = 0 ; i<count ; i++) {
		if(buffer[i].sensor == 1){
		    buffer[i].acceleration.x = event.acceleration.x;
			buffer[i].acceleration.y = event.acceleration.y;
			buffer[i].acceleration.z = event.acceleration.z;
		}
	}
}

void DumpGsensordata(sensors_event_t const * buffer, size_t count){
	unsigned int i;
    for (i=0 ; i<count ; i++) {
		DEBUG(("WSLIB buffer[%d].sensor = %d\n",i,buffer[i].sensor));
		DEBUG(("WSLIB  sendEvents buffer[%d].version=%d\n",i,buffer[i].version));
		DEBUG(("WSLIB  sendEvents buffer[%d].sensor=%d\n",i,buffer[i].sensor));
		DEBUG(("WSLIB  sendEvents buffer[%d].type=%d\n",i,buffer[i].type));
		DEBUG(("WSLIB  sendEvents buffer[%d].reserved0=%d\n",i,buffer[i].reserved0));
		DEBUG(("WSLIB  sendEvents buffer[%d].acceleration.x=%f\n",i,buffer[i].acceleration.x));
		DEBUG(("WSLIB  sendEvents buffer[%d].acceleration.y=%f\n",i,buffer[i].acceleration.y));
		DEBUG(("WSLIB  sendEvents buffer[%d].acceleration.z=%f\n",i,buffer[i].acceleration.z));
		DEBUG(("\n\n"));
    }
}

int  getSensorId(int type) {
	switch(type) {
		case SENSOR_TYPE_ACCELEROMETER:
			return '_acc';
		case SENSOR_TYPE_MAGNETIC_FIELD:
			return '_mag';
		case SENSOR_TYPE_ORIENTATION:
			return '_ori';
		case SENSOR_TYPE_GYROSCOPE:
			return '_gyr';
		case SENSOR_TYPE_LIGHT:
			return '_lit';
		case SENSOR_TYPE_PRESSURE:
			return '_pre';
		case SENSOR_TYPE_TEMPERATURE:
			return '_tem';
		case SENSOR_TYPE_PROXIMITY:
			return '_pro';
		case SENSOR_TYPE_GRAVITY:
			return '_gry';
		case SENSOR_TYPE_LINEAR_ACCELERATION:
			return '_lin';
		case SENSOR_TYPE_ROTATION_VECTOR:
			return '_rov';
		case SENSOR_TYPE_RELATIVE_HUMIDITY:
			return '_hum';
		case SENSOR_TYPE_AMBIENT_TEMPERATURE:
			return '_atm';
	}
	return 'ukn';
}


static int ConstructSensorbufferFromSocket(sensors_event_t  * buffer, int count ,char *recvmsg)
{
	if(count <1)
		return -EINVAL;

	DEBUG(("WSLIB ConstructSensorbufferFromSocket msg=%s\n",recvmsg));

	sensors_event_t event;
	int j =0;
	char *msg;
	int sensor_type = 0;
	int64_t timestamp = 0L;
	float xyz[10]={0.0};

	//sensor_type
	msg = strsep(&recvmsg," ");
	if(msg!=NULL)
		sensor_type = atoi(msg);
	else
		sensor_type = 0;
	event.type = sensor_type;

	//timestamp
	msg = strsep(&recvmsg," ");
	if(msg!=NULL)
		timestamp = atoll(msg);
	else
		timestamp = 0;
	event.timestamp = timestamp;

	//sensor identifier
	event.sensor = getSensorId(sensor_type);

	switch ( sensor_type )
	{
	    case SENSOR_TYPE_ACCELEROMETER:
			//xyz
			j = 0;
			while((msg = strsep(&recvmsg," "))!=NULL)
			{
				xyz[j] = atof(msg);
				j++;
			}
			event.acceleration.x = xyz[0];
			event.acceleration.y = xyz[1];
			event.acceleration.z = xyz[2];
	        break;
	    case SENSOR_TYPE_MAGNETIC_FIELD:

	        break;
	    case SENSOR_TYPE_ORIENTATION:

	        break;
	    case SENSOR_TYPE_GYROSCOPE:
			j = 0;
			while((msg = strsep(&recvmsg," "))!=NULL)
			{
				//ALOGD("<<<<<< LEE >>>>>> msg=%s\n",msg);
				xyz[j] = atof(msg);
				j++;
			}
			event.gyro.x = xyz[0];
			event.gyro.y = xyz[1];
			event.gyro.z = xyz[2];
	        break;
	    case SENSOR_TYPE_LIGHT:

	        break;
	    case SENSOR_TYPE_PRESSURE:

	        break;
	    case SENSOR_TYPE_TEMPERATURE:

	        break;
	    case SENSOR_TYPE_PROXIMITY:

	        break;
	    case SENSOR_TYPE_GRAVITY:

	        break;
	    case SENSOR_TYPE_LINEAR_ACCELERATION:

	        break;
	    case SENSOR_TYPE_ROTATION_VECTOR:

	        break;
	    case SENSOR_TYPE_RELATIVE_HUMIDITY:

	        break;
	    case SENSOR_TYPE_AMBIENT_TEMPERATURE:

	        break;
	    default:
	        break;
	}

	event.version = sizeof(struct sensors_event_t);
	event.sensor =1;
	event.reserved0 = 0;
	memcpy(&buffer[0],&event,sizeof(event));

	return 1;
}


int initWsSocket(int * WsSocket)
{
	DEBUG(("WSLIB initWsSocket WSSocket =%d\n!!!!!!!!!!!", WsSocket));
	DEBUG(("WSLIB initWsSocket WSSocket =%d\n!!!!!!!!!!!", &WsSocket));
	return 0;
}

ssize_t  SensorDevicePoll(sensors_event_t* buffer, size_t count) {
    ssize_t c;
	struct sockaddr_in client_addr;
	socklen_t client_addr_length = sizeof(client_addr);
	char recvmsg[1024]={0};

	//fsz++
	if(mSocket < 0)
	{
		DEBUG(("WSLIB Init a Socket !!!!!!!!!!!\n"));
		mSocket = socket(AF_INET, SOCK_DGRAM, 0);
		if (mSocket < 0)
		{
			DEBUG(("Failed to create socket \n"));
		}

		struct sockaddr_in server_addr;
		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(56458);

		if(bind(mSocket,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
		{
			DEBUG(("Failed to bind socket \n"));
			close(mSocket);
		}
	}
	//fsz++
	DEBUG(("WSLIB  SensorDevicePoll Receive Data  !\n"));
	if(recvfrom(mSocket, recvmsg, 1024,0,(struct sockaddr*)&client_addr, &client_addr_length)<0)
	{
		DEBUG(("WSLIB  SensorDevicePoll Receive Data Failed !\n"));
	}
	ConstructSensorbufferFromSocket(buffer,count,recvmsg);//fsz ++
	c =1;

    return c;
}

int Init_WsSensorDevice(struct ws_device_t **wsdevice ,void *lib ){
    DEBUG(("WSLIB Init_WsSensorDevice \n"));
    int ret = -1;
	(*wsdevice)->fun = (int (*)())(dlsym(lib,"ws_processCmd"));
	(*wsdevice)->DumpGsensordata = (void (*)(sensors_event_t const * buffer, size_t count))(dlsym(lib,"DumpGsensordata"));
	(*wsdevice)->SensorDevicePoll = (ssize_t (*)(sensors_event_t* buffer, size_t count))(dlsym(lib,"SensorDevicePoll"));
	(*wsdevice)->getSensorListEx = (ssize_t (*)(struct sensors_module_t* sensor_mode, struct sensor_t const** list))(dlsym(lib,"getSensorListEx"));
	(*wsdevice)->initCheckEx = (int (*)())(dlsym(lib,"initCheckEx"));
	(*wsdevice)->sensorHandleCheck = (int (*)(int handle))(dlsym(lib,"sensorHandleCheck"));
	ret = (*wsdevice)->fun();
    return ret;
}
